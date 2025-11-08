/*
 * BSD 3-Clause License
 * 
 * Copyright (c) 2022-2025, Norbert Schulz
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "hal/Gpio/Gpio.h"

namespace hal
{

    void Gpio::init(void)
    {
        /* Unused pins are configured as inputs with pullup to save power. 
         * HW V1 uses lowside MOSFET switches. All IO pins connected to
         * display or SD need to be high during sleep to avoid discharging
         * over signal lines.
        */
    
        /* Port B: PB6/7 unused  others outputs*/
        PORTB = _BV(PB7) | _BV(PB6) |   /* unused input, enable pullup  */
                _BV(PB5) |              /* SPICLK high */
                _BV(PB4) |              /* MISO high   */
                _BV(PB3) |              /* MOSI high   */
                _BV(PB2);               /* display SPI Chip select high */
        DDRB  = _BV(PB5) | _BV(PB3) | _BV(PB2) | _BV(PB1) | _BV(PB0);

        /* Port  C ist unused, all input pullup */
        DDRC  = ~(_BV(DDC6) | _BV(DDC5) | _BV(DDC4) | _BV(DDC3) | _BV(DDC2) | _BV(DDC1) | _BV(DDC0)); 
        PORTC =   _BV(DDC6) | _BV(DDC5) | _BV(DDC4) | _BV(DDC3) | _BV(DDC2) | _BV(DDC1) | _BV(DDC0); 

#if BOARD_REVISION < 0x0200 /* N-Mosfets on display/SDcard */
        /* Port D: PD7 input (Display busy), others output */
        PORTD = _BV(PD4);  /* SD Card SPI Chip delect high */
#else    
        /* Port D: PD7 input (Display busy), others output   */
        PORTD = _BV(PD4) |  /* SD Card SPI Chip delect high  */
                _BV(PD6);   /* P-FET SD Card Gate high = off */
#endif
        DDRD  = _BV(PD6) | _BV(PD5) | _BV(PD4) | _BV(PD3) | _BV(PD2) | _BV(PD1) | _BV(PD0);
    }
}