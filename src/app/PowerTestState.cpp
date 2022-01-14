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

#if WITH_POWER_TEST != 0

#include "app/PowerTestState.h"

#include "service/ServiceInit.h"
#include "service/Debug/Debug.h"
#include "service/Power/Power.h"

#include "hal/Cpu/Cpu.h"
#include "hal/Timer/WakeUpTimer.h"
#include "hal/Timer/TickTimer.h"
#include "hal/Uart/Uart.h"
#include "hal/Spi/Spi.h"

static void tickCB()
{

}

static void sleepOne()
{
        hal::Cpu::delayMS(100);
        hal::Cpu::setClock(hal::Cpu::CLK_SLEEP);
        hal::WakeUpTimer::init();
        hal::WakeUpTimer::enable();    
    
        hal::Cpu::powerSafe();

        hal::WakeUpTimer::disable();
        hal::Cpu::setClock(hal::Cpu::CLK_NORMAL);
        hal::Cpu::delayMS(100);
}

namespace app
{
    static PowerTestState g_powerTestState;

    PowerTestState& PowerTestState::instance()
    {
        return g_powerTestState;
    }

    void PowerTestState::enter(void)
    {
        service::init();
        DEBUG_INIT();
        hal::Cpu::irqEnable();

        _delay_ms(100);
        DEBUG_LOGP("PowerTest start\r\n\r\n");

    }

    void PowerTestState::process(StateHandler& stateHandler)
    { 
        (void)stateHandler;

        /* Loop turning devices on and off while measuring system current consumption.
         * This allows to tune each component regarding low power.
         */
        for(;;)
        {
            DEBUG_LOGP("Tick on\r\n");
            hal::TickTimer::init();
            hal::TickTimer::enable(tickCB);
            sleepOne();

            DEBUG_LOGP("Tick off\r\n");
            hal::TickTimer::disable();
            sleepOne();

            DEBUG_LOGP("uart off\r\n");
            hal::Uart::get().close();
            sleepOne();
            
            DEBUG_INIT();
            DEBUG_LOGP("uart on\r\n");
            sleepOne();

            DEBUG_LOGP("SPI on\r\n");
            hal::Spi::init();
            hal::Spi::enable();
            sleepOne();

            DEBUG_LOGP("SD on\r\n");
            service::Power::enable(service::Power::POW_SDCARD);
            sleepOne();

            DEBUG_LOGP("SD off\r\n");
            hal::Spi::disable();
            service::Power::disable(service::Power::POW_SDCARD);
            sleepOne();

            DEBUG_LOGP("SPI off\r\n");
            hal::Spi::disable();
            sleepOne();
        }
    }
}

#endif