/* 16C2450_UART.c
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

#include "16C2450_UART.h"

#define READ 0x00
#define WRITE 0xFF

#define nop() __asm__ __volatile__ ("nop" ::)

/* Static Variables */
uint8_t __attribute__ ((aligned(UART16C_RX_BUFFER_SIZE))) UARTA_RxBuf[UART16C_RX_BUFFER_SIZE] ;
volatile uint8_t UARTA_RxHead ;
volatile uint8_t UARTA_RxTail ;
uint8_t __attribute__ ((aligned(UART16C_TX_BUFFER_SIZE))) UARTA_TxBuf[UART16C_TX_BUFFER_SIZE] ;
volatile uint8_t UARTA_TxHead ;
volatile uint8_t UARTA_TxTail ;

uint8_t __attribute__ ((aligned(UART16C_RX_BUFFER_SIZE))) UARTB_RxBuf[UART16C_RX_BUFFER_SIZE] ;
volatile uint8_t UARTB_RxHead ;
volatile uint8_t UARTB_RxTail ;
uint8_t __attribute__ ((aligned(UART16C_TX_BUFFER_SIZE))) UARTB_TxBuf[UART16C_TX_BUFFER_SIZE] ;
volatile uint8_t UARTB_TxHead ;
volatile uint8_t UARTB_TxTail ;




inline void writeStrobe(void) ;
inline uint8_t readStrobe(void) ;
inline void UARTA_Select(void) ;
inline void UARTB_Select(void) ;
inline void UART16C_BusDir(uint8_t) ;
inline void UART16C_SetAddress (uint8_t) ;
inline void enableUARTAInterrupt(void) ;
inline void enableUARTBInterrupt(void) ;
inline void disableUARTAInterrupt(void) ;
inline void disableUARTBInterrupt(void) ;


/* Initialise UART */

void InitUARTA (uint16_t baudrate)
{

	uint8_t x ;

	/* Data Direction etc */
	UART16C_BusDir(WRITE) ;

	/* Set Baud, Parity, Data and Stop bits */
	UARTA_Select() ;
	UART16C_SetAddress(LCR) ; /* Line control */
	UART16C_DATA = (1 << UDVEN) ;
	writeStrobe() ;
	UART16C_SetAddress(DLM) ; /* Baud control */
	UART16C_DATA = (baudrate >> 8) ;
	writeStrobe() ;
	UART16C_SetAddress(DLL) ; /* Baud control */
	UART16C_DATA = (baudrate & 0xFF) ; 
	writeStrobe() ;
	UART16C_SetAddress(LCR) ;
	UART16C_DATA = (1 << UWL0) | (1 << UWL1) ; /* 8N1 */
	writeStrobe() ;

	/* Enable interrupts */
	UART16C_SetAddress(IER) ;
	UART16C_DATA = (1 << URXIE) | (1 << URXLIE) ; /* Enable RX Interrupt */
	writeStrobe() ;
	UART16C_SetAddress(MCR) ;
	UART16C_DATA = (1 << UOP2) ;
	writeStrobe() ;


	MCUCR |= (1 << ISC00) | (1 << ISC01) ; /* Rising edge triggers INT0/INTA */
	GICR |= (1 << INT0) ;


	x = 0 ; 			    /* Flush receive buffer */
	UARTA_RxTail = x ;
	UARTA_RxHead = x ;
	UARTA_TxTail = x ;
	UARTA_TxHead = x ;

}
/* Initialise UART */
void InitUARTB (uint16_t baudrate)
{

	uint8_t x ;

	UART16C_BusDir(WRITE) ;

	/* Set Baud, Parity, Data and Stop bits */
	UARTB_Select() ;
	UART16C_SetAddress(LCR) ; /* Line control */
	UART16C_DATA = (1 << UDVEN) ;
	writeStrobe() ;
	UART16C_SetAddress(DLM) ; /* Baud control */
	UART16C_DATA = (baudrate >> 8) ; 
	writeStrobe() ;
	UART16C_SetAddress(DLL) ; /* Baud control */
	UART16C_DATA = (baudrate & 0xFF) ; 
	writeStrobe() ;
	UART16C_SetAddress(LCR) ;
	UART16C_DATA = (1 << UWL0) | (1 << UWL1) ; /* 8N1 */
	writeStrobe() ;

	/* Enable interrupts */
	UART16C_SetAddress(IER) ;
	UART16C_DATA = (1 << URXIE) | (1 << URXLIE); /* Enable RX Interrupt */
	writeStrobe() ;
	UART16C_SetAddress(MCR) ;
	UART16C_DATA = (1 << UOP2) ;
	writeStrobe() ;


	MCUCR |= (1 << ISC10) | (1 << ISC11) ; /* Rising edge triggers INT0/INTA */
	GICR |= (1 << INT1) ;


	x = 0 ; 			    /* Flush receive buffer */
	UARTB_RxTail = x ;
	UARTB_RxHead = x ;
	UARTB_TxTail = x ;
	UARTB_TxHead = x ;

}


void generic_16C2450_Setup()
{
	/* ADDRESS */
	/* UA0:2,UART16C_RESET output. INTA, INTB as input. */
	DDRD |= (1 << DDD4) | (1 << DDD5) | (1 << DDD6) | (1 << DDD7) ;
	PORTD &= 0x00 ;
	/* CONTROL */
	/* CSA,CSB,IOR,IOW output. */
	DDRC |= (1 << DDC0) | (1 << DDC1) | (1 << DDC2) | (1 << DDC3) ;
	UART16C_CTRL |= (1 << CSA) | (1 << CSB) | (1 << IOR) | (1 << IOW) ; /* WARNING: ACTIVE LOW SIGNALS */
	/* */
	UART16C_ADDR = (1 << UART16C_RESET) ; /* Reset the UART */
	UART16C_ADDR = 0 ;
}

/* Interrupt handlers */
ISR (INT0_vect)
{
	uint8_t data ;
	uint8_t lsrVal, interruptType ;
	uint8_t tmptail ;
	uint8_t tmphead ;
	UART16C_BusDir(READ) ; /* This should mostly be reading */
	UARTA_Select() ;
	UART16C_SetAddress(LSR) ;
	lsrVal = readStrobe() ;

	do
	{
		UART16C_SetAddress(MSR) ;//clear any MSR interrupts
		readStrobe() ;
		if ((lsrVal & (1 << UTHRE)))
		{
			if (UARTA_TxHead != UARTA_TxTail)
			{
				/* Calculate buffer index */
				tmptail = (UARTA_TxTail + 1) & UART16C_TX_BUFFER_MASK ;
				UARTA_TxTail = tmptail ;      /* Store new index */
				UART16C_BusDir(WRITE) ;
				UART16C_SetAddress(THR) ;
				UART16C_DATA = UARTA_TxBuf [ tmptail ] ;  /* Start transmission */
				writeStrobe() ;
				if(UARTA_DataInTransmitBuffer() == 0)
				{
					disableUARTAInterrupt() ;
				}
			}
			else
			{
				disableUARTAInterrupt() ;
			}
		}

		if ((lsrVal & (1 << URXRDY)))
		{
			if ((lsrVal & ((1 << URXORE) | (1 << URXPE) | (1 << URXFE))) == 0 ) /* No recieve errors */
			{
				UART16C_BusDir(READ) ;
				UART16C_SetAddress(RHR) ;
				data = readStrobe() ;                /* Read the received data */

				/* Calculate buffer index */
				tmphead = (UARTA_RxHead + 1) & UART16C_RX_BUFFER_MASK ;
				if (tmphead == UARTA_RxTail)
				{
					;/* ERROR! Receive buffer overflow */
				}
				UARTA_RxHead = tmphead ;      /* Store new index */
				UARTA_RxBuf[ tmphead ] = data ; /* Store received data in buffer */
			}
			else
			{
				UART16C_SetAddress(RHR) ;
				readStrobe() ;
			}
		}

		if ((lsrVal & (1 << URXBRK)) == (1 << URXBRK))
		{
			UART16C_SetAddress(RHR) ;
			readStrobe() ;
		}

		UART16C_BusDir(READ) ;
		UART16C_SetAddress(UISR) ;
		interruptType = readStrobe() ;
		UART16C_SetAddress(LSR) ;
		lsrVal = readStrobe() ;

	}while((interruptType  & NOINTERRUPT) != 1);

}
ISR (INT1_vect)
{
	uint8_t data ;
	uint8_t lsrVal, interruptType ;
	uint8_t tmptail ;
	uint8_t tmphead ;
	UART16C_BusDir(READ) ; /* This should mostly be reading */
	UARTB_Select() ;
	UART16C_SetAddress(LSR) ;
	lsrVal = readStrobe() ;

	do
	{
		UART16C_SetAddress(MSR) ;//clear any MSR interrupts
		readStrobe() ;
		if ((lsrVal & (1 << UTHRE)))
		{
			if (UARTB_TxHead != UARTB_TxTail)
			{
				/* Calculate buffer index */
				tmptail = (UARTB_TxTail + 1) & UART16C_TX_BUFFER_MASK ;
				UARTB_TxTail = tmptail ;      /* Store new index */
				UART16C_BusDir(WRITE) ;
				UART16C_SetAddress(THR) ;
				UART16C_DATA = UARTB_TxBuf [ tmptail ] ;  /* Start transmission */
				writeStrobe() ;
				if(UARTB_DataInTransmitBuffer() == 0)
				{
					disableUARTBInterrupt() ;
				}
			}
			else
			{
				disableUARTBInterrupt() ;
			}
		}
		if ((lsrVal & (1 << URXRDY)))
		{
			if ((lsrVal & ((1 << URXORE) | (1 << URXPE) | (1 << URXFE))) == 0 ) /* No recieve errors */
			{
				UART16C_BusDir(READ) ;
				UART16C_SetAddress(RHR) ;
				data = readStrobe() ;                /* Read the received data */

				/* Calculate buffer index */
				tmphead = (UARTB_RxHead + 1) & UART16C_RX_BUFFER_MASK ;
				if (tmphead == UARTB_RxTail)
				{
					;/* ERROR! Receive buffer overflow */
				}
				UARTB_RxHead = tmphead ;      /* Store new index */
				UARTB_RxBuf[ tmphead ] = data ; /* Store received data in buffer */
			}
			else
			{
				UART16C_SetAddress(RHR) ;
				readStrobe() ;
			}
		}

		if ((lsrVal & (1 << URXBRK)) == (1 << URXBRK))
		{
			UART16C_SetAddress(RHR) ;
			readStrobe() ;
		}

		UART16C_BusDir(READ) ;
		UART16C_SetAddress(UISR) ;
		interruptType = readStrobe() ;
		UART16C_SetAddress(LSR) ;
		lsrVal = readStrobe() ;

	}while((interruptType  & NOINTERRUPT) != 1);

}


/* Read and write functions */
uint8_t UARTA_ReceiveByte (void)
{
	uint8_t tmptail ;

	while (UARTA_RxHead == UARTA_RxTail) ; /* Wait for incoming data */
	tmptail = (UARTA_RxTail + 1) & UART16C_RX_BUFFER_MASK ;/* Calculate buffer index */
	UARTA_RxTail = tmptail ;                /* Store new index */
	return UARTA_RxBuf [ tmptail ] ;           /* Return data */
}

void UARTA_TransmitByte (uint8_t data)
{
	uint8_t tmphead ;
	/* Calculate buffer index */
	tmphead = (UARTA_TxHead + 1) & UART16C_TX_BUFFER_MASK ; /* Wait for free space in buffer */
	while (tmphead == UARTA_TxTail) ;

	UARTA_TxBuf [ tmphead ] = data ;			/* Store data in buffer */
	UARTA_TxHead = tmphead ;					/* Store new index */

	enableUARTAInterrupt() ;
}
void UARTA_TransmitString(const char string[] , short int length)
{/* length = the length of the string */
	uint8_t endHead, beginHead ;
	short int i, j, tmpLength ;
	if(length > UART16C_TX_BUFFER_SIZE)
	{
		j = 0 ;
		tmpLength = 0 ;
		while(j < length)
		{
			tmpLength = ((length - tmpLength) < UART16C_TX_BUFFER_SIZE) ? (length - tmpLength) : (UART16C_TX_BUFFER_SIZE - 2);
			endHead = (UARTA_TxHead + tmpLength) & UART16C_TX_BUFFER_MASK ;
			beginHead = (UARTA_TxHead + 1) & UART16C_TX_BUFFER_MASK ;
			for(i = 0 ; i < tmpLength ; i++)
			{
				UARTA_TxBuf [ (beginHead + i) & UART16C_TX_BUFFER_MASK ] = string[ j ] ;			/* Store data in buffer */
				j++ ;
			}
			UARTA_TxHead = endHead ;					/* Store new index */
			enableUARTAInterrupt() ;
			while (UARTA_DataInTransmitBuffer()) ;
		}
	}
	else
	{
		/* Calculate buffer index */
		beginHead = (UARTA_TxHead + 1) & UART16C_TX_BUFFER_MASK ;
		endHead = (UARTA_TxHead + length) & UART16C_TX_BUFFER_MASK ; /* Wait for free space in buffer */
		for(i = 0 ; i < length ; i++)
		{
			UARTA_TxBuf [ (beginHead + i) & UART16C_TX_BUFFER_MASK ] = string[ i ] ;			/* Store data in buffer */
		}
		UARTA_TxHead = endHead ;					/* Store new index */
		enableUARTAInterrupt() ;
	}

}
uint8_t UARTB_ReceiveByte (void)
{
	uint8_t tmptail ;

	while (UARTB_RxHead == UARTB_RxTail) ; /* Wait for incoming data */
	tmptail = (UARTB_RxTail + 1) & UART16C_RX_BUFFER_MASK ;/* Calculate buffer index */
	UARTB_RxTail = tmptail ;                /* Store new index */
	return UARTB_RxBuf [ tmptail ] ;           /* Return data */
}

void UARTB_TransmitByte (uint8_t data)
{
	uint8_t tmphead ;
	/* Calculate buffer index */
	tmphead = (UARTB_TxHead + 1) & UART16C_TX_BUFFER_MASK ; /* Wait for free space in buffer */
	while (tmphead == UARTB_TxTail) ;

	UARTB_TxBuf [ tmphead ] = data ;			/* Store data in buffer */
	UARTB_TxHead = tmphead ;					/* Store new index */

	enableUARTBInterrupt() ;
}


uint8_t UARTA_DataInReceiveBuffer (void)
{
	return ((((UART16C_RX_BUFFER_SIZE - UARTA_RxTail) + UARTA_RxHead) & UART16C_RX_BUFFER_MASK)) ; /* Return 0 if the receive buffer is empty otherwise the number of chars */
}
uint8_t  UARTA_DataInTransmitBuffer (void)
{
	return ((((UART16C_TX_BUFFER_MASK - UARTA_TxTail) + UARTA_TxHead) & UART16C_TX_BUFFER_MASK)) ;
}

uint8_t UARTB_DataInReceiveBuffer (void)
{
	return ((((UART16C_RX_BUFFER_SIZE - UARTB_RxTail) + UARTB_RxHead) & UART16C_RX_BUFFER_MASK)) ; /* Return 0 if the receive buffer is empty otherwise the number of chars */
}
uint8_t  UARTB_DataInTransmitBuffer (void)
{
	return ((((UART16C_TX_BUFFER_MASK - UARTB_TxTail) + UARTB_TxHead) & UART16C_TX_BUFFER_MASK)) ;
}

inline void writeStrobe()
{
	UART16C_CTRL &= ~(1 << IOW) ; /* Strobe the write line */
	nop() ;
	UART16C_CTRL |= (1 << IOW) ; /* Should be ok - min strobe time is 40ns, at 8MHz 1 cycle = 125ns */
}
inline uint8_t readStrobe()
{
	uint8_t tmp ;
	UART16C_CTRL &= ~(1 << IOR) ; /* Strobe the write line */
	nop() ;
	tmp = PINB ;
	UART16C_CTRL |= (1 << IOR) ; /* Should be ok - min strobe time is 40ns, at 8MHz 1 cycle = 125ns */
	return tmp ;
}
inline void UARTA_Select()
{
	UART16C_CTRL &= ~(1 << CSA) ;
	UART16C_CTRL |= (1 << CSB) ;
}
inline void UARTB_Select()
{
	UART16C_CTRL &= ~(1 << CSB) ;
	UART16C_CTRL |= (1 << CSA) ;
}
inline void UART16C_BusDir (uint8_t dir)
{
	DDRB = dir ;
}
inline void UART16C_SetAddress(uint8_t addr)
{
	/* This may seem a long winded way to set the address but it is done like this for two reasons:
	 * 1 - The address output is on a shared port that serves other functions - so do not clobber values that are already set.
	 * 2 - A single line |= may not be sufficient to set the zeros, so do an &= first to make sure that the zeros are there.
	 */
	UART16C_ADDR &= (addr << UA0) ;
	UART16C_ADDR |= (addr << UA0) ;
}
void enableUARTAInterrupt(void)
{
	UARTA_Select() ;
	UART16C_BusDir(READ) ;
	UART16C_SetAddress(IER) ;
	if (!(readStrobe() & (1 << UTXIE)))/* Enable TX Interrupt (don't disable RX interrupt) */
	{
		UART16C_BusDir(WRITE) ;
		UART16C_SetAddress(IER) ;
		UART16C_DATA = (1 << URXIE) | (1 << UTXIE) | (1 << URXLIE) ; /* Enable TX Interrupt (don't disable RX interrupt) */
		writeStrobe() ;
	}
}
inline void disableUARTAInterrupt(void)
{
	UARTA_Select() ;
	UART16C_BusDir(WRITE) ;
	UART16C_SetAddress(IER) ;
	UART16C_DATA = (1 << URXIE) | (1 << URXLIE) ; /* Enable TX Interrupt (don't disable RX interrupt) */
	writeStrobe() ;
}
inline void enableUARTBInterrupt(void)
{
	UARTB_Select() ;
	UART16C_BusDir(READ) ;
	UART16C_SetAddress(IER) ;
	if (!(readStrobe() & (1 << UTXIE)))/* Enable TX Interrupt (don't disable RX interrupt) */
	{
		UART16C_BusDir(WRITE) ;
		UART16C_SetAddress(IER) ;
		UART16C_DATA = (1 << URXIE) | (1 << UTXIE) | (1 << URXLIE) ; /* Enable TX Interrupt (don't disable RX interrupt) */
		writeStrobe() ;
	}
}
inline void disableUARTBInterrupt(void)
{
	UARTB_Select() ;
	UART16C_BusDir(WRITE) ;
	UART16C_SetAddress(IER) ;
	UART16C_DATA = (1 << URXIE) | (1 << URXLIE) ; /* Enable TX Interrupt (don't disable RX interrupt) */
	writeStrobe() ;
}
