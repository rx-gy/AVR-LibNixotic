/* UART.c
 *
 * Copyright 2009-2012 Nixotic Design
 *
 * This file is part of AVR-LibNixotic.
 *
 * AVR-LibNixotic is free software: you can redistribute it and/or modify
 * it under the terms of the Lesser GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * AVR-LibNixotic is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Lesser GNU General Public License for more details.
 *
 * You should have received a copy of the Lesser GNU General Public License
 * along with AVR-LibNixotic.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "UART.h"

/* Static Variables */
uint8_t __attribute__ ((aligned(UART_RX_BUFFER_SIZE))) UART_RxBuf[UART_RX_BUFFER_SIZE] ;
volatile uint8_t UART_RxHead ;
volatile uint8_t UART_RxTail ;
uint8_t __attribute__ ((aligned(UART_TX_BUFFER_SIZE))) UART_TxBuf[UART_TX_BUFFER_SIZE] ;
volatile uint8_t UART_TxHead ;
volatile uint8_t UART_TxTail ;
volatile uint8_t rtx_stat ;
#define MASK8BIT 255

/* Initialize UART */
void InitUART ( uint16_t baudrate )
{

	uint8_t x ;

//	UBRRH = ( uint8_t ) ( baudrate >> 8 ) ;
	UBRRH = 0;
	UBRRL = ( uint8_t ) ( baudrate & MASK8BIT ) ; 	/* Set the baud rate */
	/* Enable UART receiver and transmitter, and receive interrupt */
//	UCSRA = ( 1 << U2X ) /* Double the transmission speed */;
	UCSRA = ( 1 << U2X ) /* Double the transmission speed */;
//	UCSRA = 0; /* WARNING: without this Arduino Pro Mini defaults to U2X = 1!! */
	UCSRB = ( ( 1 << RXCIE ) | ( 1 << RXEN ) | ( 1 << TXEN ) ) ;
	UCSRC = ( ( 1 << USBS ) | ( 1 << UCSZ0 ) | ( 1 << UCSZ1 ) ) ; /* 8N2 */
//	UCSRC = ( ( 1 << UCSZ0 ) | ( 1 << UCSZ1 ) ) ; /* 8N1 */

	x = 0 ; 			    /* Flush receive buffer */
	rtx_stat = 0 ;
	UART_RxTail = x ;
	UART_RxHead = x ;
	UART_TxTail = x ;
	UART_TxHead = x ;
}

void SetBaud ( uint16_t baudrate )
{
	UBRRH = ( uint8_t ) ( baudrate >> 8 ) ;
	UBRRL = ( uint8_t ) ( baudrate & MASK8BIT ) ; 	/* Set the baud rate */
}

/* Interrupt handlers */
ISR ( USART_RX_vect )
{
	uint8_t data ;
	uint8_t tmphead ;

	data = UDR ;                 /* Read the received data */

	/* Calculate buffer index */
	tmphead = ( UART_RxHead + 1 ) & UART_RX_BUFFER_MASK ;
	if ( tmphead == UART_RxTail )
	{
		/* ERROR! Receive buffer overflow - BYTE LOST */
	}
	else
	{
		UART_RxHead = tmphead ;        /* Store new index */
		UART_RxBuf[ tmphead ] = data ; /* Store received data in buffer */
	}
}
#ifdef PREFIXED_TRANSMIT
ISR ( USART_UDRE_vect )
{
	uint8_t tmptail ;

	/* Check if all data is transmitted */
	if ( UART_TxHead != UART_TxTail )
	{
		if( rtx_stat == 0 )
		{
			rtx_stat = 1 ;
			UCSRA |= ( 1 << TXC ) ; /* Clear Transmit Complete Flag */
			//UDR = 0x01 ;  /* Start transmission */
			UDR = '/' ;  /* Start transmission */
		}
		else if( rtx_stat == 1 )
		{
			rtx_stat = 2 ;
			UCSRA |= ( 1 << TXC ) ;
			//UDR = 0xAA ;  /* Start transmission */
			UDR = '\\' ;  /* Start transmission */
		}
		else
		{
			rtx_stat = 0 ;
			/* Calculate buffer index */
			tmptail = ( UART_TxTail + 1 ) & UART_TX_BUFFER_MASK ;
			UART_TxTail = tmptail ;      /* Store new index */
			UCSRA |= ( 1 << TXC ) ;
			UDR = UART_TxBuf [ tmptail ] ;  /* Start transmission */
		}
	}
	else
	{
		UCSRB &= ~( 1 << UDRIE ) ;         /* Disable UDRE interrupt */
	}
}
#else
ISR ( USART_UDRE_vect )
{
	uint8_t tmptail ;

	/* Check if all data is transmitted */
	if ( UART_TxHead != UART_TxTail )
	{
		/* Calculate buffer index */
		tmptail = ( UART_TxTail + 1 ) & UART_TX_BUFFER_MASK ;
		UART_TxTail = tmptail ;      /* Store new index */
		UCSRA |= ( 1 << TXC ) ;
		UDR = UART_TxBuf [ tmptail ] ;  /* Start transmission */
	}
	else
	{
		UCSRB &= ~( 1 << UDRIE ) ;         /* Disable UDRE interrupt */
	}
}
#endif
/* Read and write functions */
uint8_t UART_ReceiveByte ( void )
{
	uint8_t tmptail ;

	while ( UART_RxHead == UART_RxTail )  /* Wait for incoming data */
		;
	tmptail = ( UART_RxTail + 1 ) & UART_RX_BUFFER_MASK ;/* Calculate buffer index */

	UART_RxTail = tmptail ;                /* Store new index */

	return UART_RxBuf [ tmptail ] ;           /* Return data */
}

uint8_t UART_TransmitByte ( uint8_t data )
{
	uint8_t tmphead ;
	uint8_t status ;
	/* Calculate buffer index */
	tmphead = ( UART_TxHead + 1 ) & UART_TX_BUFFER_MASK ;
	if ( tmphead == UART_TxTail ) /* Check for space in the buffer */
	{
		status = ERROR_UART_TX_FULL ;
	}
	else
	{
		UART_TxBuf [ tmphead ] = data ;			/* Store data in buffer */
		UART_TxHead = tmphead ;					/* Store new index */
		status = 0 ;
		UCSRB |= ( 1 << UDRIE ) ;				/* Enable UDRE interrupt */
	}
	return status ;
}

uint8_t UART_QueueTXByte ( uint8_t data )
{
	uint8_t tmphead ;
	uint8_t status ;
	/* Calculate buffer index */
	tmphead = ( UART_TxHead + 1 ) & UART_TX_BUFFER_MASK ;
	if ( tmphead == UART_TxTail ) /* Check for space in the buffer */
	{
		status = ERROR_UART_TX_FULL ;
	}
	else
	{
		UART_TxBuf [ tmphead ] = data ;			/* Store data in buffer */
		UART_TxHead = tmphead ;					/* Store new index */
		status = 0 ;
	}
	return status ;
}
uint8_t UART_QueueTXBuffer ( uint8_t *buffer, uint8_t len )
{
	for( uint8_t i = 0; i<len; i++ )
	{
		if( UART_QueueTXByte( buffer[i] ) == ERROR_UART_TX_FULL )
		{
			return ERROR_UART_TX_FULL;
		}
	}
	return 0;
}
void UART_StartTransmission ( void )
{
	UCSRB |= ( 1 << UDRIE ) ;				/* Enable UDRE interrupt */
}


uint8_t UART_DataInReceiveBuffer ( void )
{
	return ( UART_RxHead != UART_RxTail ) ; /* Return 0 (FALSE) if the receive buffer is empty */
}
uint8_t  UART_DataInTransmitBuffer ( void )
{
	return ( UART_TxHead != UART_TxTail ) ; /* Return 0 (FALSE) if the receive buffer is empty */
}
