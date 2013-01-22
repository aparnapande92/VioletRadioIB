/**
 * \file
 *
 * \brief User board configuration template
 *
 */

#ifndef CONF_BOARD_H
#define CONF_BOARD_H

/** Define which Radio IB this is	*/
#define RADIO_IB_1
//#define RADIO_IB_2

/** Define DEBUG to run debug task only (when debugging on STK600)	*/
#define DEBUG

// VCP Addresses. Make permanent change in vcp library !
#ifdef RADIO_IB_1
	#define VCP_RADIOIB		0x0C			
	#define VCP_RADIO		VCP_RADIO_1	
	#define VCP_CDHIB		VCP_CDHIB_1
#else
	#define VCP_RADIOIB		0x0D
	#define VCP_RADIO		VCP_RADIO_2
	#define VCP_CDHIB		VCP_CDHIB_2
#endif
	

// User declarations:
//extern Bool xosc_recovey;

// User functions:
void clock_init				(void); // in init.c
void switch_to_ext_osc		(void); // in init.c
void timers_init			(void); // in init.c
void interrupts_init		(void); // in init.c
void usart_init				(void); // in init.c
void io_init				(void); // in init.c

#endif // CONF_BOARD_H
