/*
 * BSD 3-Clause License
 * 
 * Copyright (c) 2022, Norbert Schulz
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

#ifndef WAKEUPTIMER_H_INCLUDED
#define WAKEUPTIMER_H_INCLUDED

#include <stdint.h>

namespace hal
{
    /** Timer 2  Driver for AVR 328P power safe wakeup
     * 
     * Code assumes CPU clock divider as max (256), @see Cpu::setClock
     */ 
    class WakeUpTimer 
    {
        public:

            /** Duration in miliseconds until sleep phase ends
             * 
             *  The 328P can roughly sleep 8 seconds until the power down 
             *  wakekup timer2 overflows and triggers.
             * 
             *  Formula: 8Mhz / 256(clk prescale) / 1024(timer2 prescale) / 256(cnt)
             *           -> 0,11920929 Hz or 8388,608 seconds 
             * 
             */
            static const uint32_t WAKEUP_INTERVAl_MS = 8389u;

            /** low level init of timer (does not enable it)
             */
            static void init();

            /** Enable timer
             */
            static void enable();

            /** Disable timer
             */
            static void disable();

        private:
            WakeUpTimer(const WakeUpTimer&);
            WakeUpTimer& operator=(const WakeUpTimer&);
    };
}

#endif /* WAKEUPTIMER_H_INCLUDED */
