/* UART.h
 *
 * Copyright 2009-2012 Nixotic Design
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the Lesser GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Lesser GNU General Public License for more details.
 *
 * You should have received a copy of the Lesser GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UART_H
#define UART_H

#include <avr/io.h>
#include <avr/interrupt.h>


/* UART Buffer Defines */
#define UART_RX_BUFFER_SIZE 8     /* 2,4,8,16,32,64,128 or 256 bytes */
#define UART_TX_BUFFER_SIZE 8

#define ERROR_UART_TX_FULL 0x01

#define UART_RX_BUFFER_MASK ( UART_RX_BUFFER_SIZE - 1 )
#if ( UART_RX_BUFFER_SIZE & UART_RX_BUFFER_MASK )
	#error RX buffer size is not a power of 2
#endif

#define UART_TX_BUFFER_MASK ( UART_TX_BUFFER_SIZE - 1 )
#if ( UART_TX_BUFFER_SIZE & UART_TX_BUFFER_MASK )
	#error TX buffer size is not a power of 2
#endif

/* Prototypes */
void InitUART( unsigned int baudrate ) ;
uint8_t UART_ReceiveByte( void ) ;
uint8_t UART_TransmitByte( uint8_t data ) ;
uint8_t UART_DataInReceiveBuffer ( void ) ;
uint8_t  UART_DataInTransmitBuffer ( void ) ;
void SetBaud( unsigned int baudrate ) ;
uint8_t UART_QueueTXByte ( uint8_t data ) ;
uint8_t UART_QueueTXBuffer ( uint8_t *buffer, uint8_t len );
void UART_StartTransmission ( void ) ;


/* BAUD_RATES */
/* 8 MHz */
#define BAUD_8M_48 	103	//ERROR 0.2%
#define BAUD_8M_96 	51		//ERROR 0.2%
#define BAUD_8M_144 	34 	//ERROR -0.8%
#define BAUD_8M_192	25		//ERROR 0.2%
#define BAUD_8M_384	12		//ERROR 0.2%
#define BAUD_8M_250K	1		//ERROR 0.0%
#define BAUD_8M_500K 0		//ERROR 0.0%
#if defined (__AVR_ATmega328P__) || (__AVR_ATmega328__)
	#define UDR UDR0
	#define U2X U2X0
	#define UCSRA UCSR0A
	#define UCSRB UCSR0B
	#define UCSRC UCSR0C
	#define UBRRL UBRR0L
	#define UBRRH UBRR0H
	#define TXC TXC0
	#define RXEN RXEN0
	#define TXEN TXEN0
	#define UDRIE UDRIE0
	#define TXCIE TXCIE0
	#define RXCIE RXCIE0
	#define UCSZ2 UCSZ02
	#define UCSZ1 UCSZ01
	#define UCSZ0 UCSZ00
	#define USBS USBS0
#endif
#endif
