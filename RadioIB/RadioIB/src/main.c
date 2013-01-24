/**
 * \file main.c
 *
 *  Author: Liran
*/

#include <asf.h>

#include "scheduler/scheduler.h"
#include "uart.h"
#include "vcp/vcp_library.h"

/**
 * Name         : main
 *
 * Synopsis     : int main()
 *
 * Description  : Entry point. Initialize the board and run the scheduler. 
 * 
 */


// Changes : Changed the port initializations to remove portG and DDRA commands
//           Changed the port commands to DIR and OUT
//           Not sure if we need the TIMSK, OCR0 and TCCR0 constants to be
//           initiliazed so left them in there
//           The timer counter isnt required as of yet, so I commented that
//           Uart init(): 
//           Don't need the telemetry and vcp packets to communicate yet.
//           Don't need to initialize svit
//           Don't need the adc conversions or the voltage and current upper bounds

// IMPORTANT: Check what uart_init() does to initialize, located in uart.c

void initialize( void )
{
   // pin initialization
  PORTA.DIR = 0b11111111;
  PORTA.OUT = 0b00000111;

  PORTB.DIR = 0b11111111;
  PORTB.OUT = 0b11100000;

  PORTC.DIR = 0b11111111;
  PORTC.OUT = 0b11111111;

  PORTD.DIR = 0b11111011;
  PORTD.OUT = 0b11110000;

  PORTE.DIR = 0b11111110;
  PORTE.OUT = 0b00000000;

  PORTF.DIR = 0b11110000;
  PORTF.OUT = 0b00000000;

  TIMSK = ( 1 << OCIE0 );
  OCR0 = 71;

  // enable clear on match mode, set prescalar to 1024
  TCCR0 = ( 1 << WGM01 ) | ( 1 << CS02 ) | ( 1 << CS01 ) | ( 1 << CS00 );

  //timer0_counter[0] = 99;
  //timer0_counter[1] = 199;
  
  // uart.h
  uart_init();

  rx_flag[0] = 0;
  rx_flag[1] = 0;

  // rev up those interrupts
  sei();
}


int main (void)
{
	// Init - in init.c
	board_init();

  initialize();
  //fprintf( stdout, "uart initialized\n" );

  while(1)
  {
    if ( rx_flag[0] > 0 )
    {
      rx_flag[0]--;
      // Would send and receive messages here
      //receive_message( 0, uart_vcp_buff[0]->message, uart_vcp_buff[0]->index );
      //vcpptr_init( uart_vcp_buff[0], uart_message_buff[0], BUFFER_SIZE );
    }
	  if ( rx_flag[1] > 0 )
    {
      rx_flag[1]--;
      // Would send and receive messages here
      //receive_message( 1, uart_vcp_buff[1]->message, uart_vcp_buff[1]->index );
      //vcpptr_init( uart_vcp_buff[1], uart_message_buff[1], BUFFER_SIZE );
    }
  }

	// Run scheduler - in scheduler.c
	scheduler();
}
