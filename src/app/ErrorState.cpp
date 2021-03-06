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

#include "app/ErrorState.h"

#include "service/Display/Display.h"
#include "service/Power/Power.h"
#include "service/Led/Led.h"
#include "service/Debug/Debug.h"
namespace app
{
    static ErrorState errorState;

    ErrorState& ErrorState::instance()
    {
        return errorState;
    }

    void ErrorState::enter()
    {
        DEBUG_LOGP("Enter ErrorState\r\n");

        if (service::Epd::init())
        {
            /* display blank red screen on error
             */
            service::Epd::clear(service::Epd::RED);
            service::Epd::sleep();
        }
        else
        {
            for (uint8_t loop(10u); loop; --loop)
            {
                service::Led::enable();
                service::Power::idle(25u);
                service::Led::disable();
                service::Power::idle(25u);
            }
        }
        
    	/* Game over, shutdown now :( 
         */
        DEBUG_LOGP("halt\r\n");

        service::Power::suspend();
        service::Power::halt();
    }
}