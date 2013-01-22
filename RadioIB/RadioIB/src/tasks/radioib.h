/** \file
 * radioib.h
 * \brief Radio IB header file
 *
 * Created: 3/5/2012
 *  Author: Liran
 */ 


#ifndef RADIOIB_H_
#define RADIOIB_H_

#define RADIO_IB_COMMAND_PACKET_SIZE	3			///< Size in bytes of the command packet
#define NOOP_COMMAND					0x00		///< No Op command code
#define ACK_SIZE						3			///< Size in bytes of the Acknowledge packet

/// Structure of the command packet
typedef struct {
	uint8_t				Command_Header;				///< 8 bit header of the command packet
	uint16_t			Command_Argument; 			///< 16 bit argument of the command packet 
} Command_Packet_t;	

Command_Packet_t		Command_packet;

Bool					Command_received;			///< Flag to indicate a command packet for the MCU is ready	
uint8_t					ACK[ACK_SIZE];				///< Buffer to hold the Ack packet


#endif /* RADIOIB_H_ */