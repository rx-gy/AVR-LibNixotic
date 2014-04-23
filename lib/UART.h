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
#define UART_TX_BUFFER_SIZE 32

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


#define MASK8BIT 0xFF
#define MASK4BIT 0x0F

/* BAUD_RATES */
#if F_CPU == 8000000
/* 8 MHz */
#define BAUD_4800   103 //ERROR 0.2%
#define BAUD_9600    51 //ERROR 0.2%
#define BAUD_14400   34 //ERROR -0.8%
#define BAUD_19200   25 //ERROR 0.2%
#define BAUD_38400   12 //ERROR 0.2%
#define BAUD_250K     1 //ERROR 0.0%
#define BAUD_500K     0 //ERROR 0.0%
#elif F_CPU == 16000000
/* 16 MHz */
#define BAUD_4800   207 //ERROR 0.2%
#define BAUD_9600   103 //ERROR 0.2%
#define BAUD_14400   68 //ERROR 0.6%
#define BAUD_19200   51 //ERROR 0.2%
#define BAUD_38400   25 //ERROR 0.2%
#define BAUD_250K     3 //ERROR 0.0%
#define BAUD_500K     1 //ERROR 0.0%
#define BAUD_1M       0 //ERROR 0.0%
#endif

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
 #define USART_RXC_vect USART_RX_vect
#endif

#if defined (__AVR_ATmega32U4__)
 #define UDR UDR1
 #define U2X U2X1
 #define UCSRA UCSR1A
 #define UCSRB UCSR1B
 #define UCSRC UCSR1C
 #define UBRRL UBRR1L
 #define UBRRH UBRR1H
 #define TXC TXC1
 #define RXEN RXEN1
 #define TXEN TXEN1
 #define UDRIE UDRIE1
 #define TXCIE TXCIE1
 #define RXCIE RXCIE1
 #define UCSZ2 UCSZ02
 #define UCSZ1 UCSZ11
 #define UCSZ0 UCSZ10
 #define USBS USBS0
 #define USART_RXC_vect USART1_RX_vect
 #define USART_UDRE_vect USART1_UDRE_vect
#endif
#endif
