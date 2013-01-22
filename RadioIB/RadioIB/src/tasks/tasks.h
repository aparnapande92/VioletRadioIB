/** \file
 * tasks.h
 * \brief Tasks header file
 *
 *  Author: Liran
 */ 

#ifndef TASKS_H_
#define TASKS_H_

#include <asf.h>
#include "../config/conf_board.h"
#include "../memory/memory.h"

volatile extern Bool	xosc_recovey;		///< Flag for attempt to recover from external oscillator failure


void cdhib_uart_task				(void);
void radio_uart_task				(void);
void radio_ib_task					(void);
#ifdef DEBUG
void debug_task						(void);
#endif

#endif /* TASKS_H_ */