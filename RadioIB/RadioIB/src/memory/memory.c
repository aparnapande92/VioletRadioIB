/** \file
 * memory.c
 * \brief Memory source file
 *
 *  Author: Liran
 */ 

#include "memory.h"

/**
 * Name         : memory_init
 *
 * Synopsis     : void memory_init (void)
 *
 * Description  : Initialize structure members and buffers for all the Peripherals
 * 
 */
void memory_init (void)
{
	// CDH IB
	RingBuffer_InitBuffer			(&cdhib.rx_ringbuff);	
	Queue_RingBuffer_InitBuffer		(&cdhib_queue_ringbuff);
	cdhib.USART =					&CDHIB_UART;
	cdhib.DMA_channel =				&DMA.CH0;
	cdhib.rx_data =					cdhib_rx_data;
	cdhib.rx_data_buffer_size =		CDHIB_RECEIVE_MESSAGE_BUFF_SIZE;
	cdhib.tx_data =					cdhib_tx_data;
	cdhib.tx_data_buffer_size =		CDHIB_TRANSMIT_MESSAGE_BUFF_SIZE;
	cdhib.VCP_address =				VCP_CDHIB; 
	
		
	// Radio
	RingBuffer_InitBuffer			(&radio.rx_ringbuff);	
	Queue_RingBuffer_InitBuffer		(&radio_queue_ringbuff);
	radio.USART =					&RADIO_UART; 
	radio.DMA_channel =				&DMA.CH1;
	radio.rx_data =					radio_rx_data;
	radio.rx_data_buffer_size =		RADIO_RECEIVE_MESSAGE_BUFF_SIZE;
	radio.tx_data =					radio_tx_data;
	radio.tx_data_buffer_size =		RADIO_TRANSMIT_MESSAGE_BUFF_SIZE;
	radio.VCP_address =				VCP_RADIO;
	
	// RADIO IB
	radioib.VCP_address =			VCP_RADIOIB;	
	// Acknowledge message
	ACK[0] =						0x41;	// "A"
	ACK[1] =						0x43;	// "C"
	ACK[2] =						0x4B;	// "K"

}


/**
 * Name         : dma_init
 *
 * Synopsis     : void dma_init (void)
 *
 * Description  : Initialize and enable DMA for all Peripheral USARTs
 * 
 */
void dma_init (void)
{
	// Enable clock to the DMA
	sysclk_enable_peripheral_clock(&DMA);
	
	DMA_Enable();
	DMA_SetPriority(DMA_PRIMODE_RR0123_gc);					// Round Robin on channels 0/1/2/3
	
	// CDHIB
	DMA_EnableSingleShot(cdhib.DMA_channel);				// Single shot - every trigger pulls one byte 
    DMA_SetTriggerSource(cdhib.DMA_channel, 
						DMA_CH_TRIGSRC_CDHIB_UART_DRE_gc);	// USART Trigger source - Data Register Empty
	
	// Radio
	DMA_EnableSingleShot(radio.DMA_channel);
    DMA_SetTriggerSource(radio.DMA_channel, 
						DMA_CH_TRIGSRC_RADIO_UART_DRE_gc);	
	
}


/**
 * Name         : read_VCP_receive_buff
 *
 * Synopsis     : void read_VCP_receive_buff(peripheral_t* Peripheral)
 *
 *	\param	Peripheral	Address to the peripheral which is the source of the transmission
 *
 * Description  : This function reads a VCP peripheral ring buffer into a linear non-VCP buffer.
 *				  When data is ready, it raises flag.
 * 
 */
void read_VCP_receive_buff(peripheral_t* Peripheral)
{
	while(!RingBuffer_IsEmpty(&Peripheral->rx_ringbuff))
	{
		//if there's no vcp buffer, initialize it
		if (Peripheral->vcp_rx_msg.message == NULL)
		{
			vcpptr_init(&(Peripheral->vcp_rx_msg), Peripheral->rx_data, Peripheral->rx_data_buffer_size);
			Peripheral->rx_byte_count = 0;
		}
		
		// Get byte from receive ring buffer
		uint8_t rx_byte = RingBuffer_Remove(&Peripheral->rx_ringbuff);
		// decode VCP
		Peripheral->VCP_rx_status = Receive_VCP_byte(&(Peripheral->vcp_rx_msg), rx_byte);

		if (Peripheral->VCP_rx_status & VCP_OVR_ERR)	{}
		if (Peripheral->VCP_rx_status & VCP_CRC_ERR)	
		{
			Peripheral->VCP_rx_status = 0;
		
		}
		if (Peripheral->VCP_rx_status & VCP_NULL_ERR)	{}
		if (Peripheral->VCP_rx_status & VCP_ADDR_ERR)	{}
		if (Peripheral->VCP_rx_status & VCP_ESC_ERR)	{}
		if (Peripheral->VCP_rx_status == VCP_TERM) // Done with no errors
		{
			// save received byte count
			Peripheral->rx_byte_count = Peripheral->vcp_rx_msg.index;
			// Destination from VCP address field
			Peripheral->rx_data_destination = Peripheral->vcp_rx_msg.address;
			// Set Data ready flag			
			Peripheral->rx_data_ready = true;											

			// Add to received packet count
			Peripheral->rx_packet_count++;
			// kill VCP buffer
			Peripheral->vcp_rx_msg.message = NULL;
			
			// Toggle the RX LED to show packet received
			#ifdef DEBUG
				PORTA.OUTTGL = Peripheral->rx_LED_pin;
			#endif
			
			// Exit the while loop
			break;
		}	
	}		
}

/**
 * Name         : DMA_transmit
 *
 * Synopsis     : void DMA_transmit(peripheral_t*	Peripheral)
 *
 * \param	Peripheral	Address to the peripheral which is the destination for the transmission
 *
 * Description  : Transmit data block through USART using DMA
 * 
 */
void DMA_transmit(peripheral_t*	Peripheral)
{
	// Set up the block transfer
	DMA_SetupBlock(	Peripheral->DMA_channel,				// DMA Channel
					Peripheral->tx_data,					// Source buffer address
					DMA_CH_SRCRELOAD_NONE_gc,				// No reload
					DMA_CH_SRCDIR_INC_gc,					// Source address direction - Increment address
					(void *)&Peripheral->USART->DATA,		// Destination - USART DATA reg
					DMA_CH_DESTRELOAD_NONE_gc,				// No reload
					DMA_CH_DESTDIR_FIXED_gc,				// Destination address direction - Fixed address
					Peripheral->tx_byte_count,				// Block size
					DMA_CH_BURSTLEN_1BYTE_gc,				// 1 byte per transfer
					0,										// No repeat
					false);									// No repeat

	// Enable channel - the channel will be automatically disabled when a transfer is finished
	DMA_EnableChannel(Peripheral->DMA_channel);
	
	// Toggle the TX LED to show packet sent
	#ifdef DEBUG
		PORTA.OUTTGL = Peripheral->tx_LED_pin;
	#endif
		
	// Add to transmit packet count
	Peripheral->tx_packet_count++;
}

/**
 * Name         : VCP_DMA_transmit
 *
 * Synopsis     : void VCP_DMA_transmit(peripheral_t* source, peripheral_t* destination)
 *
 * \param	source			Address to the peripheral which is the source of the transmission
 * \param	destination		Address to the peripheral which is the destination for the transmission
 *
 * Description  : Package a packet from a source peripheral rx buffer
 *				  to a destination peripheral tx buffer in VCP frame and transmit using DMA
 * 
 */
void VCP_DMA_transmit(peripheral_t* source, peripheral_t* destination)
{
	// Reset transmit data count to full buffer size
	destination->tx_byte_count = destination->tx_data_buffer_size;
	
	// create VCP frame in the peripheral transmit buffer
	destination->VCP_tx_status = Create_VCP_frame(	destination->tx_data, 
													(uint16ptr)&destination->tx_byte_count, 
													source->VCP_address, 
													source->rx_data, 
													source->rx_byte_count);

	if (destination->VCP_tx_status == VCP_OVR_ERR)	{}
	if (destination->VCP_tx_status == VCP_NULL_ERR)	{}
	if (destination->VCP_tx_status == VCP_ADDR_ERR)	{}
	if (destination->VCP_tx_status == VCP_TERM)			// Done with no errors
	{

		#ifndef DEBUG
		// Reset the source received byte count
		source->rx_byte_count = 0;
		#endif
		
		// Transmit with DMA
		DMA_transmit(destination);
	}		
}
