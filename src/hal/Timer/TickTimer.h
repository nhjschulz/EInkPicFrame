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

#ifndef TICKTIMER_H_INCLUDED
#define TICKTIMER_H_INCLUDED

#include <stdint.h>

namespace hal
{
    /** Timer 0 Driver for AVR 328P (10ms interval)
    */
    class TickTimer 
    {
        public:

            /**
             * @brief Time between ticks in ms
             * 
             */
            static const uint8_t TICK_TIME_MS = 10u;

            /** User callback function, called every tick
             */
            typedef void (*TickFunctionCB)(void);

            /** low level init of timer (does not enable it)
             */
            static void init();

            /** Enable timer with given callback
             * 
             * @param callback user provided callback function, maybe nullptr
             */
            static void enable(TickFunctionCB callback);

            /** Disable timer
             */
            static void disable();

            /**
             * @brief Get the current number of passed ticks 
             * 
             * Wraps every 256 * 0,010s = 2,56s. Can be used to
             * measure small time deltas, tick resolution is 10ms
             * 
             * @return uint8_t 
             */
            static uint8_t getTickCount(void)
            {
                return m_ticks;
            }

        private:

            /**
             * @brief Helper function to give ISR access to tick counter
             * 
             */
            friend void incTickIsr(void);

            static volatile uint8_t m_ticks;

        private:
            TickTimer(const TickTimer&);
            TickTimer& operator=(const TickTimer&);
    };
}

#endif /* TICKTIMER_H_INCLUDED */
