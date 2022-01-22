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
#include "service/Power/Power.h"

#include "hal/Cpu/Cpu.h"
#include "hal/Gpio/Gpio.h"
#include "hal/Spi/Spi.h"
#include "hal/Timer/TickTimer.h"
#include "hal/Uart/Uart.h"
#include "hal/Timer/WakeUpTimer.h"

#include "service/FileIo/FileIo.h"
#include "service/Debug/Debug.h"


extern "C" void disk_timerproc (void);

namespace service
{
    void Power::halt(void)
    {
        for(;;)
        {
            hal::Cpu::halt();
        }
    }

    void Power::enable(Device device)
    {
        switch(device)
        {
            case POW_SDCARD:
#if BOARD_REVISION < 0x0200 /* N-Mosfets on display/SDcard */
            	hal::Gpio::setSdCardPower();
#else
            	hal::Gpio::clrSdCardPower();
#endif
                hal::Cpu::delayMS(20u);
                break;

            case POW_DISPLAY:
                hal::Gpio::setDispPow();
                hal::Cpu::delayMS(50u);
                break;
        }
    }

     void Power::disable(Device device)
     {
         switch(device)
         {
            case POW_SDCARD:
#if BOARD_REVISION < 0x0200 /* N-Mosfets on display/SDcard */
                hal::Gpio::clrSdCardPower();
#else
             	hal::Gpio::setSdCardPower();
#endif
                break;

            case POW_DISPLAY:
                hal::Gpio::clrDispPow();
                break;
         }
     }

    void Power::suspend()
    {
        hal::Uart::get().close();

        /* ensure external modules off */
        service::Power::disable(service::Power::POW_SDCARD);
        service::Power::disable(service::Power::POW_DISPLAY);

        /* disable on chip devices */
        hal::Spi::disable();
        hal::TickTimer::disable();

        /* slow down clock */
        hal::Cpu::setClock(hal::Cpu::CLK_SLEEP);

        /* enable wakeup timer */
        hal::WakeUpTimer::init();
        hal::WakeUpTimer::enable();    
    }
    
    void Power::resume(void)
    {
        /* disable wakeup timer */
        hal::WakeUpTimer::disable();

        /* speed up clock */
        hal::Cpu::setClock(hal::Cpu::CLK_NORMAL);
        
        /* turn on onchip devices */
        hal::Spi::init();
        hal::Spi::enable();

        hal::TickTimer::init();
        hal::TickTimer::enable(disk_timerproc);

        DEBUG_INIT();

        hal::Cpu::irqEnable();
    }

}
