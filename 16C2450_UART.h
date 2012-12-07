/* 16C2450_UART.h
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

#include <avr/io.h>
#include <avr/interrupt.h>
#define UXTAL_FREQ_HZ 19660800
#define BAUD_DIV4800M 0x01 /* Used to set DLM to select baud rate 4.8k given a xtal freq of 19.6608 MHz */
#define BAUD_DIV4800L 0x00 /* Used to set DLL to select baud rate 4.8k given a xtal freq of 19.6608 MHz */
#define BAUD_DIV4800 0x0100 /* Used to set DLL to select baud rate 4.8k given a xtal freq of 19.6608 MHz */
#define BAUD_DIV9600 0x80 /* Used to set DLL to select baud rate 9.6k given a xtal freq of 19.6608 MHz */
#define BAUD_DIV19200 0x40 /* Used to set DLL to select baud rate 19.2k given a xtal freq of 19.6608 MHz */

/* UART Buffer Defines */
#define UART16C_RX_BUFFER_SIZE 8   /* 2,4,8,16,32,64,128 or 256 bytes */
#define UART16C_TX_BUFFER_SIZE 8

#define UART16C_RX_BUFFER_MASK (UART16C_RX_BUFFER_SIZE - 1)
#if (UART16C_RX_BUFFER_SIZE & UART16C_RX_BUFFER_MASK)
	#error RX buffer size is not a power of 2
#endif

#define UART16C_TX_BUFFER_MASK (UART16C_TX_BUFFER_SIZE - 1)
#if (UART16C_TX_BUFFER_SIZE & UART16C_TX_BUFFER_MASK)
	#error TX buffer size is not a power of 2
#endif


/* IO Defines */

#define UART16C_DATA PORTB

#define UART16C_INTERRUPTS PORTD
#define INTA INT0
#define INTB INT1
#define PIN_INTA PD2
#define PIN_INTB PD3


#define UART16C_CTRL PORTC
#define CSA PC0 /* WARNING: ACTIVE LOW OUTPUT */
#define CSB PC1 /* WARNING: ACTIVE LOW OUTPUT */
#define IOR PC2
#define IOW PC3


#define UART16C_ADDR PORTD
#define UA0 PD4
#define UA1 PD5
#define UA2 PD6
#define UART16C_RESET PD7

/* Register and internal defines */
/* Address defines */
#define RHR 0x00 /* Receive Holding Register */
#define THR 0x00 /* Transmit Holding Register */
#define DLL 0x00 /* Div Latch Low Byte */
#define DLM 0x01 /* Div Latch High Byte */
#define IER 0x01 /* Interrupt Enable Register */
#define UISR 0x02 /* Interrupt Status Register */
#define LCR 0x03 /* Line Control Register */
#define MCR 0x04 /* Modem Control Register */
#define LSR 0x05 /* Line Status Register */
#define MSR 0x06 /* Modem Status Register */
#define SPR 0x07 /* Scratch Pad Register */

#define URXIE 0 /* RX Data Interrupt Enable - Bit 0, IER */
#define UTXIE 1 /* TX Data Empty Interrupt Enable - Bit 1, IER */
#define URXLIE 2 /* RX Line Status Interrupt Enable - Bit 2, IER */
#define UMSIE 3 /* Modem Status Interrupt Enable - Bit 3, IER */

#define NOINTERRUPT 0x01 /* No interrupts pending */
#define MSRINT 0x00 /* Modem status register interrupt - No bits, ISR */
#define TXRDY 0x02 /* Transmit data ready - Bit 1, ISR */
#define RXRDY 0x04 /* Receive data ready - Bit 2, ISR */
#define LSRINT 0x06 /* LSR Generated interrupt - Bit 2 | Bit 1, ISR */
#define ISRMASK 0x0F /* Mask bits 0:3 */

#define UWL0 0 /* Word Length Bit 0 - Bit 0, LCR */
#define UWL1 1 /* Word Length Bit 1 - Bit 1, LCR */
#define USBT 2 /* Stop Bits - Bit 2, LCR */
#define UPEN 3 /* Parity Enable - Bit 3, LCR */
#define UPEV 4 /* Even Parity - Bit 4, LCR */
#define UPSE 5 /* Set Parity - Bit 5, LCR */
#define UTXBR 6 /* Set TX Break - Bit 6, LCR */
#define UDVEN 7 /* Divisor Enable - Bit 7, LCR */

#define UDTROC 0 /* DTR# Output Control - Bit 0, MCR */
#define URTSOC 1 /* RTS# Output Control - Bit 1, MCR */
#define UOP1 2 /* Rsrvd (OP1#) - Bit 2, MCR */
#define UOP2 3 /* OP2# INT Output Enabl - Bit 3, MCR */
#define UILBE 4 /* Internal Loop-back Enable - Bit 4, MCR */

#define URXRDY 0 /* RX Data Ready - Bit 0, LSR */
#define URXORE 1 /* RX Overrun Error - Bit 1, LSR */
#define URXPE 2 /* RX Parity Error - Bit 2, LSR */
#define URXFE 3 /* RX Framing Error - Bit 3, LSR */
#define URXBRK 4 /* RX Break - Bit 4, LSR */
#define UTHRE 5 /* THR Empty - Bit 5, LSR */
#define UTSRE 6 /* THR & TSR Empty - Bit 6, LSR */

#define UDCTS 0 /* Delta CTS# - Bit 0, MSR */
#define UDDSR 1 /* Delta DSR# - Bit 1, MSR */
#define UDRI 2 /* Delta RI# - Bit 2, MSR */
#define UDCD 3 /* Delta CD# - Bit 3, MSR */
#define UCTS 4 /* CTS# Input - Bit 4, MSR */
#define UDSR 5 /* DSR# Input - Bit 5, MSR */
#define URI 6 /* RI# Input - Bit 6, MSR */
#define UCD 7 /* CD# Input - Bit 7, MSR */



/* Prototypes */
void    generic_16C2450_Setup(void) ;

void    InitUARTA(uint16_t baudrate) ;
uint8_t UARTA_ReceiveByte(void) ;
void    UARTA_TransmitByte(uint8_t data) ;
uint8_t UARTA_DataInReceiveBuffer (void) ;
uint8_t UARTA_DataInTransmitBuffer (void) ;
void    UA_TX(void) ;

void    UARTA_TransmitString(const char string[], short int length) ;

void    enableUARTAInterrupt(void) ;
void    enableUARTBInterrupt(void) ;

void    InitUARTB(uint16_t baudrate) ;
uint8_t UARTB_ReceiveByte(void) ;
void    UARTB_TransmitByte(uint8_t data) ;
uint8_t UARTB_DataInReceiveBuffer (void) ;
uint8_t UARTB_DataInTransmitBuffer (void) ;

