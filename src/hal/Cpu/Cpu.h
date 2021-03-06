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

#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

namespace hal
{
    /**
     * @brief CPU related services
     * 
     */
    class Cpu 
    {
        public:
        
        /**
         * @brief Clock speed modes
         * 
         */
        enum Clock
        {
            CLK_NORMAL,     /**< configure cpu clock for normal operation */
            CLK_SLEEP       /**< configure cpu clock for sleep operation  */
        };

        /**
         * @brief disable all interrupts
         * 
         */
        inline static void irqDisable() 
        { 
            cli();
        }

        /**
         * @brief enable all interrupts
         * 
         */
        inline static void irqEnable()
        {
            sei();
        }

        /**
         * @brief busy wait
         * 
         * @param ms wait duration in ms
         */
        inline static void delayMS(uint16_t ms)
        {
            _delay_ms(ms);
        }

        /**
         * @brief Configure CPU clock
         * 
         * @param  clkMode New clock mode s
         */
        static void setClock(Clock clkMode);

        /** Halt (shutdown) cpu/system
         */
        static void halt(void);

        /**
         * @brief Enter Power save mode
         * 
         */
        static void enterPowerSave();

        /**
         * @brief Reset system
         * 
         */
        static void reset(void);

        /**
         * @brief Get the Idle time for one tick in ms.
         * 
         * @return uint8_t  miliseconds per idle tick
         */
    	static uint8_t getIdleTickTime_ms();

        /* Enter idle until next tickTimer tick 
         * 
         * Put cpu in idle for ticks ticktimer ticks (which is ~10ms ). Use
         * as a power friendly alternative to delayMS();
         * 
         * @param ticks How often to enter idle
         */ 
        static void enterIdle(uint8_t ticks);
    };
}
#endif /* CPU_H_INCLUDED */