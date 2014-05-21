/* ADC.h
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

#include <avr/io.h>
#include <avr/interrupt.h>

#define ADCREF_AREF 0
#define ADCREF_AVCC _BV(REFS0)
#define ADCREF_INT (_BV(REFS1) | _BV(REFS0))

#define ADCTRG_FREE 0
#define ADCTRG_ANLG _BV(ADTS0)
#define ADCTRG_EIR0 _BV(ADTS1)
#define ADCTRG_T0MA (_BV(ADTS1) | _BV(ADTS0))
#define ADCTRG_T0VF _BV(ADTS2)
#define ADCTRG_T1MB (_BV(ADTS2) | _BV(ADTS0))
#define ADCTRG_T1VF (_BV(ADTS2) | _BV(ADTS1))
#define ADCTRG_T1CA (_BV(ADTS2) | _BV(ADTS1) | _BV(ADTS0))

#define ADC_CH0 0
#define ADC_CH1 1
#define ADC_CH2 2
#define ADC_CH3 3
#define ADC_CH4 4
#define ADC_CH5 5
#define ADC_CH6 6
#define ADC_CH7 7

void setup_adc(uint8_t, uint8_t, uint8_t, uint8_t);

extern volatile uint8_t adc_ready;
extern volatile uint16_t adc_val;
