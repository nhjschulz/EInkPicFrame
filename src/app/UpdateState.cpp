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

#include "app/UpdateState.h"

#include "service/Power/Power.h"
#include "service/Display/Display.h"
#include "service/FileIo/FileIo.h"
#include "service/Debug/Debug.h"
#include "app/ErrorState.h"
#include "app/SleepState.h"

namespace app
{
    static UpdateState g_updateState;

    UpdateState& UpdateState::instance()
    {
        return g_updateState;
    }

    void UpdateState::process(StateHandler& stateHandler)
    {
        DEBUG_LOGP("Power: %d mV (ref: %d): \r\n",
                service::Power::getSupplyVoltage_mV(),
                service::Power::getReferenceVoltage_mV());
                
        bool errorOccured(false) ;

        service::Power::enable(service::Power::POW_DISPLAY);
        DEBUG_LOGP("Epd::init()...");
        if (!service::Epd::init())
        {
            DEBUG_LOGP("timeout!!\r\n");
            errorOccured = true;
        }
        else
        {
            DEBUG_LOGP("done\r\n");

            if (!updateScreen())
            {
                errorOccured = true;
            }
        }

        if (errorOccured)
        {
            stateHandler.setState(ErrorState::instance());
        }
        else
        {
            stateHandler.setState(SleepState::instance());
            service::Power::disable(service::Power::POW_DISPLAY);
        }
    }

    bool UpdateState::updateScreen(void)
    {
        bool result(true);

        if (service::FileIo::enable())
        {
            DEBUG_LOGP("File %s\r\n", service::FileIo::getFileName());
            
            DEBUG_LOGP("Epd::beginPaint()...");

            service::Epd::beginPaint();
            DEBUG_LOGP("done\r\n");

            if (service::FileIo::open())
            {
                uint16_t readRet(0u);
                uint32_t total(0);

                do {
                    service::FileIo::read(
                        service::FileIo::iobuf,
                        service::FileIo::SHARED_BUF_SIZE, readRet);

                    if (0u != readRet)
                    {
                        service::Epd::sendBlock(
                            service::FileIo::iobuf, readRet);
                        total += readRet;
                    }
                } while (0u != readRet);

                service::FileIo::close();

                DEBUG_LOGP("read %ld\r\n", total);
            }
            
            if (!service::FileIo::next())
            {
                DEBUG_LOGP("no next\r\n");
                result = false;
            }

            service::FileIo::disable();

            DEBUG_LOGP("Epd::endPaint()...");
            service::Epd::endPaint();
            DEBUG_LOGP("done\r\n");
            service::Epd::sleep();
        }
        else
        {
            result  = false;
        }

        return result;
    }
}