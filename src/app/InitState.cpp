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

#include "app/InitState.h"
#include "app/UpdateState.h"
#include "app/ErrorState.h"
#include "app/LowBatState.h"
#include "app/Parameter.h"

#include "service/ServiceInit.h"
#include "service/FileIo/FileIo.h"
#include "service/Power/Power.h"
#include "service/Led/Led.h"

namespace app
{

    static InitState initState;

    InitState& InitState::instance()
    {
        return initState;
    }

    void InitState::enter(void)
    {
        service::init();
        service::Led::enable();
        service::Power::resume(0u);
        service::Power::idle(50);
        service::Led::disable();
    }

    void InitState::process(StateHandler& stateHandler)
    {
        if (!service::FileIo::init())
        {
            stateHandler.setState(ErrorState::instance());
        }
        else
        {
            Parameter::init();
            service::Power::setCalibrationVoltages(
                    Parameter::getRefVoltage(),
                    Parameter::getCalVoltage()
            );
            
            /* check for sufficient supply voltage
             */
            uint16_t supplyVoltage(service::Power::getSupplyVoltage_mV());


            if (supplyVoltage < Parameter::getMinVoltage())
            {
                stateHandler.setState(LowBatState::instance());
            }
            else
            {
                stateHandler.setState(UpdateState::instance());
            }
        }
    }
}