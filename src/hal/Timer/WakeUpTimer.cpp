/*
 * BSD 3-Clause License
 * 
 * Copyright (c) 2021, Norbert Schulz
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

#include "hal/Timer/WakeUpTimer.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>

/*******************************************************************************
    Module statics
*******************************************************************************/


/*******************************************************************************
    Implementation
*******************************************************************************/

namespace hal
{
    void WakeUpTimer::init()
    { 
        power_timer2_enable();
     
        TCCR2A = 0u;  /* Normal operation, no waveform pin output */
        TCCR2B &= ~(_BV(CS22) | _BV(CS21)  | _BV(CS20));  /* no clock, stopped */
        TIMSK2 = 0u;  /* interrupts disabled */
    }

    void WakeUpTimer::enable()
    {
        TCNT2 = 0u;
        TCCR2B |= _BV(CS22) | _BV(CS21)  | _BV(CS20);  /* prescale 1024 */

        TIMSK2 |= _BV(TOIE2);   /* enable overflow interrupt */

    }

    void WakeUpTimer::disable()
    {        
        TIMSK2 = 0u;   /* interrupts disabled */
        TCCR2B &= ~(_BV(CS22) | _BV(CS21)  | _BV(CS20));  /* no clock, stopped */

        power_timer2_disable();
    }
}

/** overflow tick interrupt handler, expected every WAKEUP_INTERVAl_MS
 */
ISR(TIMER2_OVF_vect)
{ 
}