/* ADC.c
 *
 * Copyright 2009-2013 Nixotic Design
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

#include "ADC.h"


volatile uint8_t adc_ready = FALSE;
volatile uint16_t adc_val = 0;

uint8_t adc_chan;

void setup_adc(uint8_t reference, uint8_t channel, uint8_t trigger, uint8_t disable_digital)
{
#if defined (__AVR_ATmega32U4__)
  PRR0 &= ~_BV(PRADC);
#else
  PRR &= ~_BV(PRADC);
#endif
  ADMUX = reference | channel | _BV(ADLAR);
  ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADATE) | _BV(ADIF) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); /* Enable, Start, Auto trigger, Clear Interrupt, Enable Interrupt, Clkdiv = 128 */
  ADCSRB = trigger;
  DIDR0 = disable_digital;
  adc_chan = channel;
}

ISR(ADC_vect)
{
  adc_ready = adc_chan + 1; // 0 = false, 1 = channel 0 complete, 2 = channel 1 complete ...
  adc_val = (uint16_t) ADC;
  //adc_val |= (uint16_t) (ADCH<<8);
  //adc_val = (uint16_t) 65318;
  //adc_val |= (uint16_t) (ADCH<<8);
  //adc_val = (uint16_t) ADCH<<8;
  
  //adc_val = ADCL;
}
