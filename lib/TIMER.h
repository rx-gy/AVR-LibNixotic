/* TIMER.h
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


void timer0_basic( unsigned char, unsigned char );
extern volatile unsigned char timer0_triggered;
extern volatile unsigned char timer0_alpha;
extern volatile unsigned int counter0;

#define TIMER_OFF (uint8_t) 0
#define TIMER_PRE_1 (uint8_t) _BV(CS00)
#define TIMER_PRE_8 (uint8_t) _BV(CS01)
#define TIMER_PRE_64 (uint8_t) (_BV(CS01)|_BV(CS00))
#define TIMER_PRE_256 (uint8_t) _BV(CS02)
#define TIMER_PRE_1024 (uint8_t) (_BV(CS02)|_BV(CS00))
