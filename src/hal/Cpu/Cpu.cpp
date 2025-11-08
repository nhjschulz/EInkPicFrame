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

#include "hal/Cpu/Cpu.h"
#include "hal/Timer/TickTimer.h"

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

namespace hal
{
    void Cpu::setClock(Clock clkMode)
    {
        switch (clkMode)
        {
            case CLK_NORMAL:
                clock_prescale_set(clock_div_2);     /* 4 Mhz using internal 8Mhz clock */
                break;

            case CLK_SLEEP:
                clock_prescale_set(clock_div_256);   /* 31.25 Khz, slowest possible clock */
                break;
        }
    }

    void Cpu::halt(void)
    {
        irqDisable();

        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        sleep_enable();
        sleep_bod_disable();
        /* irqEnable();   <--- deliberately not in this case */
        sleep_cpu();
        sleep_disable();
    }   

    void Cpu::enterPowerSave(void)
    {
        irqDisable();

        set_sleep_mode(SLEEP_MODE_PWR_SAVE);
        sleep_enable();
        sleep_bod_disable();
        irqEnable();
        sleep_cpu();
        sleep_disable();
    }

    uint8_t Cpu::getIdleTickTime_ms()
    {
        return TickTimer::TICK_TIME_MS;
    }
    
    void Cpu::enterIdle(uint8_t ticks)
    {
        uint8_t currTicks(TickTimer::getTickCount());
        uint8_t delta;

        do {
            irqDisable();

            set_sleep_mode(SLEEP_MODE_IDLE);
            sleep_enable();
            irqEnable();
            sleep_cpu();
            sleep_disable();

            delta = TickTimer::getTickCount() - currTicks;
        } while (delta <= ticks);
        
    }

    void Cpu::reset(void)
    {
        /* reset through watchdog timeout */
        irqDisable();   
        wdt_enable(WDTO_15MS);

        for(;;)
        {

        }
    }

} // namespace hal
