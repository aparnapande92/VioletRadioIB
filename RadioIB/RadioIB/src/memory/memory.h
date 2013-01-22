/** \file
 * memory.h
 * \brief Memory header file
 *
 *  Author: Liran
 */ 


#ifndef MEMORY_H_
#define MEMORY_H_

#include <asf.h>
#include "../config/conf_board.h"
#include "../config/conf_usart_serial.h"
#include "LightweightRingBuff.h"
#include "dma_driver.h"
#include "../vcp/common.h"
#include "../vcp/vcp_library.h"
#include "../tasks/tasks.h"
#include "../tasks/radioib.h"

// Non-VCP receive Buffers size
#define RADIO_RECEIVE_MESSAGE_BUFF_SIZE		256			///< Radio receive buffer size(non - VCP)
#define CDHIB_RECEIVE_MESSAGE_BUFF_SIZE		256			///< CDHIB receive buffer size(non - VCP)

// VCP transmit Buffers size
#define CDHIB_TRANSMIT_MESSAGE_BUFF_SIZE	256			///< CDHIB transmit buffer size (VCP)

// Non-VCP transmit buffers
#define RADIO_TRANSMIT_MESSAGE_BUFF_SIZE	256			///< Radio transmit buffer size(non - VCP)


/// Peripheral structure
typedef struct {
	
	// Hardware
	USART_t *					USART;					///< USART associated with this peripheral
	volatile DMA_CH_t *			DMA_channel;			///< DMA channel for data transmission
	
	// Buffers
	Receive_RingBuff_t 			rx_ringbuff;			///< ring buffer to receive from USART
	vcp_ptrbuffer				vcp_rx_msg;				///< VCP buffer pointer
	uint8ptr					rx_data;				///< linear buffer for non VCP received data
	uint16_t					rx_data_buffer_size;	///< allocated size of receive buffer
	uint8ptr					tx_data;				///< linear buffer for VCP frame ready to transmit
	uint16_t					tx_data_buffer_size;	///< allocated size of transmit buffer
	
	// Flags and Counters
	volatile uint8_t			rx_ringbuff_overflow;	///< counts receive ring buffer overflow
	uint16_t					rx_byte_count;			///< number of received bytes after VCP decoding (actual data size)
	Bool						rx_data_ready;			///< flag for VCP decoding done and non-VCP data ready 
	uint16_t					tx_byte_count;			///< bytes to tx in transmit buffer (actual data size)

	// VCP
	uint8_t						VCP_address;			///< VCP address
	uint8_t						rx_data_destination;	///< destination for received data (VCP address from received packet)
	uint8_t						VCP_rx_status;			///< VCP receive status register
	uint8_t						VCP_tx_status;			///< VCP transmit status register
	uint16_t					rx_packet_count;		///< keeps track of number of packets received from this peripheral 
	uint16_t					tx_packet_count;		///< keeps track of number of packets transmitted to this peripheral 

	#ifdef DEBUG
		uint8_t					tx_LED_pin;				///< DEUBG - STK LED pin	
		uint8_t					rx_LED_pin;				///< DEUBG - STK LED pin
	#endif
	
} peripheral_t;

// Declare peripheral structures
peripheral_t					radio;					///< Radio Peripheral
peripheral_t					cdhib;					///< CDHIB Peripheral
peripheral_t					radioib;				///< RadioIB (Self) Peripheral - enables the use of all the peripheral functions

// FC and Radio transmit queue ring buffers
Queue_RingBuff_t 				cdhib_queue_ringbuff;	///< CDHIB Queue ring buffer
Queue_RingBuff_t 				radio_queue_ringbuff;	///< Radio Queue ring buffer

// Data buffers allocation
uint8_t radio_rx_data			[RADIO_RECEIVE_MESSAGE_BUFF_SIZE];	///< Radio receive buffer allocation
uint8_t radio_tx_data			[RADIO_TRANSMIT_MESSAGE_BUFF_SIZE];	///< Radio transmit buffer allocation
uint8_t cdhib_rx_data			[CDHIB_RECEIVE_MESSAGE_BUFF_SIZE];	///< CDHIB receive buffer allocation
uint8_t cdhib_tx_data			[CDHIB_TRANSMIT_MESSAGE_BUFF_SIZE];	///< CDHIB transmit buffer allocation


// Functions
void memory_init				(void);
void dma_init					(void);
void read_VCP_receive_buff		(peripheral_t* Peripheral);
void read_Non_VCP_receive_buff	(peripheral_t* Peripheral);
void DMA_transmit				(peripheral_t* Peripheral);
void VCP_DMA_transmit			(peripheral_t* source, peripheral_t* destination);

#endif /* MEMORY_H_ */