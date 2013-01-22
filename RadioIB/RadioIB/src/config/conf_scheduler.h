/** \file
 * conf_scheduler.h
 *
 * \brief Scheduler configuration
 *
 *	SCHEDULER CONFIGURATION 
 * *	Define the appropriate tasks and their order for each MCU
 * *	If there are more than 10 tasks, add ifdefs in scheduler.c and scheduler.h
 * *	Tasks are in tasks.c
 *
 *  Author: Liran
 */ 


#ifndef _CONF_SCHEDULER_H_
#define _CONF_SCHEDULER_H_

#include "conf_board.h"

#define Scheduler_debug_task	debug_task
#define Scheduler_task_1        cdhib_uart_task
#define Scheduler_task_2        radio_uart_task
#define Scheduler_task_3        radio_ib_task
//#define Scheduler_task_4        _task
//#define Scheduler_task_5        _task
//#define Scheduler_task_6        _task
//#define Scheduler_task_7        _task
//#define Scheduler_task_8        _task
	
#endif  //! _CONF_SCHEDULER_H_

