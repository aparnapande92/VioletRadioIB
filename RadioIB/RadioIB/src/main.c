/**
 * \file main.c
 *
 *  Author: Liran
*/

#include <asf.h>

#include "scheduler/scheduler.h"

/**
 * Name         : main
 *
 * Synopsis     : int main()
 *
 * Description  : Entry point. Initialize the board and run the scheduler. 
 * 
 */
int main (void)
{
	// Init - in init.c
	board_init();
			
	// Run scheduler - in scheduler.c
	scheduler();
}
