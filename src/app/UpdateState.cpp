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

#include "service/Display/Display.h"
#include "service/FatFS/source/ff.h"
#include "service/Debug/Debug.h"
#include "app/ErrorState.h"
#include "app/SleepState.h"

namespace app
{
    static UpdateState updateState;

    static FIL fil;
    FATFS fs;

    static uint8_t iobuf[100];

    UpdateState& UpdateState::instance()
    {
        return updateState;
    }

    void UpdateState::process(StateHandler& stateHandler)
    {
        FRESULT res;

        DEBUG_LOGP("UpdateState::process");

        res = f_mount(&fs, "", 1);
        if (res != FR_OK)
        {
            DEBUG_LOGP("f_mount returned %02x\r\n", res);
            stateHandler.setState(ErrorState::instance());
            return;
        }
            
        service::Epd::clear(service::Epd::CLEAN);

        res = f_open(&fil, "P4063214.EPD", FA_READ);

        DEBUG_LOGP("f_open->%d\r\n", res);
        if (FR_OK == res)
        {
            UINT read = 0;

            service::Epd::beginPaint();
            do {
                res = f_read(&fil, iobuf, sizeof(iobuf), &read);
                if (0u != read)
                {
                    service::Epd::sendBlock(iobuf, read);
                }
            } while(0u != read);

            service::Epd::endPaint();
        }
        service::Epd::sleep();
        DEBUG_LOGP("displyDrv.Sleep() done\r\n");
        stateHandler.setState(SleepState::instance());
    }
}