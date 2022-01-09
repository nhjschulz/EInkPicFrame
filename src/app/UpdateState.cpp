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
    static uint8_t g_iobuf[100];

    UpdateState& UpdateState::instance()
    {
        return g_updateState;
    }

    void UpdateState::process(StateHandler& stateHandler)
    {
        DEBUG_LOGP("UpdateState::process");
        bool errorOccured(false);

        service::Power::enable(service::Power::POW_DISPLAY);

        // clear takes as long as redraw and doesn't seem to be necessary.
        // service::Epd::clear(service::Epd::CLEAN);

        if (service::FileIo::enable())
        {
            DEBUG_LOGP("FIle %s\r\n", service::FileIo::getFileName());
            
            service::Epd::beginPaint();

            if (service::FileIo::open())
            {
                uint16_t readRet(0u);
                uint32_t total(0);

                do {

                    service::FileIo::read(g_iobuf, sizeof(g_iobuf), readRet);
                    if (0u != readRet)
                    {
                        service::Epd::sendBlock(g_iobuf, readRet);
                        total += readRet;
                    }
                } while (0u != readRet);

                service::FileIo::close();

                DEBUG_LOGP("read %ld\r\n", total);
            }
            
            if (!service::FileIo::next())
            {
                DEBUG_LOGP("no next\r\n");
                errorOccured = true;
            }

            service::FileIo::disable();

            service::Epd::endPaint();
        }
        else
        {
            errorOccured = true;
        }

        if (errorOccured)
        {
            stateHandler.setState(ErrorState::instance());
        }
        else
        {
            stateHandler.setState(SleepState::instance());
        }
    }
}