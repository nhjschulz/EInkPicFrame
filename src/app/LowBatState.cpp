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

#include "app/LowBatState.h"

#include "service/Display/Display.h"
#include "service/FileIo/FileIo.h"
#include "service/Power/Power.h"
#include "service/Debug/Debug.h"
#include "app/Parameter.h"
#include "app/UpdateState.h"

#include <avr/pgmspace.h>

namespace app
{
    /**
     * @brief this state singleton
     */
    static LowBatState g_lowBatState;

    /**
     * @brief location of low battery EPD img
     */
    static const char g_lowBatImgFile[] PROGMEM = "/epd/err/lb.epd";

    LowBatState& LowBatState::instance()
    {
        return g_lowBatState;
    }

    void LowBatState::enter(void)
    {
        DEBUG_LOGP("Enter LowBat: %d mV \r\n",
                service::Power::getSupplyVoltage_mV());
                
        /* display low battery image on EPD 
         */
        service::Power::enable(service::Power::POW_DISPLAY);
        service::Epd::init();

        if (service::FileIo::enable())
        {
            char path[sizeof(g_lowBatImgFile)];
            strcpy_P(path, g_lowBatImgFile);

            if (service::FileIo::open(path))
            {
                uint16_t readRet(0u);

                service::Epd::beginPaint();

                do {
                    service::FileIo::read(
                        service::FileIo::iobuf,
                        service::FileIo::SHARED_BUF_SIZE, readRet);

                    if (0u != readRet)
                    {
                        service::Epd::sendBlock(
                            service::FileIo::iobuf,
                            readRet);
                    }
                } while (0u != readRet);

                service::FileIo::close();
                service::Epd::endPaint();
            }
        }
        
        service::Epd::sleep();
        service::FileIo::disable();
        service::Power::disable(service::Power::POW_DISPLAY);
    }

    void LowBatState::process(StateHandler& stateHandler)
    {
        /* sleep 10min before checking power state again.
         */
        uint32_t loops((10u * 60u * 1000u) /
            service::Power::getSleepDurationMs());

        service::Power::suspend();

        while(loops--)
        {
            service::Power::sleep();
        }
    
        service::Power::resume();


        /* Resume if supply power is 50 mV higher than the low voltage
         * level. Likely a charger is attached in this case.
         */ 
        uint16_t power(service::Power::getSupplyVoltage_mV());

        if ((Parameter::getMinVoltage() + 50u) <= power)
        {
            DEBUG_LOGP("Leaving LowBat: %d mV \r\n",power);

            stateHandler.setState(UpdateState::instance());
        }
        else
        {
            DEBUG_LOGP("LowBat: %d mV \r\n",power);
        }
    }
}