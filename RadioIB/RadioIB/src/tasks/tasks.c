/** \file
 * tasks.c
 * \brief Tasks source file
 *
 *  Author: Liran
 */ 

#include <asf.h>
#include "tasks.h"

#ifdef DEBUG


#endif


/*********/
/* Tasks */
/*********/

#ifdef DEBUG
/**
 * Name         : debug_task
 *
 * Synopsis     : void debug_task	(void)
 *
 * Description  : Task for debug only. Runs when DEBUG is defined in conf_board.h
 * 
 */
void debug_task	(void)
{
		
	for(;;)
	{
		// test external and internal osc
		
		if (xosc_recovey)
			switch_to_ext_osc ();
		
		if (OSC.STATUS & OSC_XOSCRDY_bm)
			PORTA.OUTCLR =	PIN1_bm;
		else
			PORTA.OUTSET =	PIN1_bm;
			
		if (OSC.STATUS & OSC_RC32MRDY_bm)
			PORTA.OUTCLR =	PIN2_bm;
		else
			PORTA.OUTSET =	PIN2_bm;
			
	}
}
#endif // DEBUG

/**
 * Name         : cdhib_uart_task
 *
 * Synopsis     : void cdhib_uart_task	(void)
 *
 * Description  : CDHIB Task
 * *			Read the CDHIB USART receive buffer
 * *			Check for received CDHIB data determine the destination
 * *			Check the CDHIB transmit queue, and transmit data to the CDHIB if available
 * 
 */
void cdhib_uart_task	(void)
{
	read_VCP_receive_buff(&cdhib);
	
	if (cdhib.rx_data_ready)										// New data from CDHIB ready
	{  
		cdhib.rx_data_ready =	false;
		
		// Check the destination for the new data
		if (cdhib.rx_data_destination == radioib.VCP_address)
		{
			// Data for radio
		}
		else if (cdhib.rx_data_destination == radioib.VCP_address)
		{
			// Command for radio IB
			memcpy(&Command_packet, cdhib.rx_data, RADIO_IB_COMMAND_PACKET_SIZE);
			Command_received = true;		
		}
		else
		{
			// Bad address
		}
	}
	
	// Check transmit queue and transmit to CDHIB 
	if (!Queue_RingBuffer_IsEmpty(&cdhib_queue_ringbuff))				// There's something in the queue
	{
		uint8 source_vcp_address = Queue_RingBuffer_Remove(&cdhib_queue_ringbuff); // what's the source for this data?
		
		// Check the source of the new data
		if (source_vcp_address == radio.VCP_address)
		{
			VCP_DMA_transmit(&radio, &cdhib);	// build VCP frame and transmit with DMA 
		}		
		else if (source_vcp_address == radioib.VCP_address)
		{
			VCP_DMA_transmit(&radioib, &cdhib);	// build VCP frame and transmit with DMA 
		}
		else
		{
			// Bad address
		}
	}	
		
}

/**
 * Name         : radio_uart_task
 *
 * Synopsis     : void radio_uart_task	(void)
 *
 * Description  : Radio Task
 * 
 */
void radio_uart_task	(void)
{

}

/**
 * Name         : radio_ib_task
 *
 * Synopsis     : void radio_ib_task	(void)
 *
 * Description  : Radio IB (Local) Task
 * *			If internal oscillator is used as clock source - try to switch to external oscillator
 * *			Check for new command from CDHIB and execute
 * 
 */
void radio_ib_task	(void)
{

	// In case internal oscillator is in use - try to switch to external oscillator
	if (xosc_recovey)
		switch_to_ext_osc ();
		
	if (Command_received)
	{
		Command_received = false;	
		// parse command...
		switch(Command_packet.Command_Header)
		{
			case NOOP_COMMAND:
				// ACK back to cdhib
				cdhib.rx_data =			ACK;
				cdhib.rx_byte_count =	ACK_SIZE;
				Queue_RingBuffer_Insert(&cdhib_queue_ringbuff, radioib.VCP_address);	// Insert to cdhib transmit queue
				break;
			case 1:
				break;
			case 2:
				break;
			default:
				break;
		}

	}		
}