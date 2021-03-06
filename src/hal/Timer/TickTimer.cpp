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

#include "hal/Timer/TickTimer.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <util/atomic.h>

/*******************************************************************************
    Module statics
*******************************************************************************/

/** application timer tick callback function */
static hal::TickTimer::TickFunctionCB tickCallback = nullptr;

/** milliseconds since power on */
static volatile uint32_t g_millies = 0u;

/*******************************************************************************
    Implementation
*******************************************************************************/

namespace hal
{
    void TickTimer::init()
    {
        GPIOR0  = 0u;

        power_timer0_enable();

        TCNT0 = 0u;

        /* 10ms tick interval at 4 Mhz F_CPU */
        TIMSK0 &= ~((1 << OCIE0B) | (1 << OCIE0A) | (1 << TOIE0)); /* no int */
        TCCR0A |= ((1 << WGM01) | (0 << WGM00));              /* CTC mode */
        TCCR0B &= ~((0 << CS02) | (0 << CS01) |(0 << CS00));   /* off */ 
    }

    void TickTimer::enable(TickTimer::TickFunctionCB callback)
    {        
        TCNT0 = 0u;
        OCR0A = 156u;
        OCR0B = 159u;

        TCCR0B |= ((1 << CS02) | (0 << CS01) |(0 << CS00));   /* clk/256  */ 

        tickCallback = callback;
        TIMSK0 |= _BV(OCIE0A);     /* enable match OCRA interrupt */
    }

    void TickTimer::disable()
    {        
        TIMSK0 &= ~(_BV(OCIE0A));
        TCCR0B &= ~((0 << CS02) | (0 << CS01) |(0 << CS00));   /* off */ 

        power_timer0_disable();
    }

    uint8_t TickTimer::getTickCount(void)
    {
        return GPIOR0;
    }

    uint32_t TickTimer::getMillis(void)
    {
        uint32_t elapsed;

        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
           elapsed = g_millies;
        }
        return elapsed;
    }

    void TickTimer::adjustMillies(uint32_t deltaMs)
    {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            g_millies += deltaMs;
        }
    }
}


/** 10ms tick interrupt handler
 */
ISR(TIMER0_COMPA_vect)
{ 
    PORTD ^=_BV(PD3);

    uint8_t fastTickCnt(GPIOR0++);

    OCR0A = (fastTickCnt % 4) ? 156u : 157u;    /* 10mhz is precisely 156.25 ticks */

    g_millies += 10u;

    if (nullptr != tickCallback)
    {
        (*tickCallback)();
    }
}