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

#include "app/SleepState.h"
#include "app/UpdateState.h"
#include "app/LowBatState.h"
#include "app/InitState.h"
#include "app/Parameter.h"

#include "service/Debug/Debug.h"
#include "service/Power/Power.h"
#include "service/FileIo/FileIo.h"


static void printTime()
{
#if WITH_DEBUG != 0
    uint32_t seconds(service::Power::uptime_mS() / 1000u);
    uint32_t hours( seconds / 3600u);
    uint32_t minutes((seconds % 3600u) / 60);

    DEBUG_LOGP("Time: %ld:%02ld:%02ld\r\n", hours, minutes, seconds % 60u);

#endif
}
namespace app
{
    static SleepState g_sleepState; /**< state instance */
    static uint32_t g_vsn;  /**< volume serial number when entering sleep */
    static uint16_t g_loops = 0u; /**< number of sleep/wakeups to run */
    static uint32_t  g_timeAdjust = 0ul; /**< lost ticks during sleep */

    SleepState& SleepState::instance()
    {
        return g_sleepState;
    }

    void SleepState::enter(void)
    {
        DEBUG_LOGP("Sleep()\r\n");

        if (!service::FileIo::getVolumeSerialNumber(g_vsn))
        {
            g_vsn = 0u;
        }

        if (!g_loops)
        {
            /* Calculate sleep loops to delay wanted minutes
             */
            uint32_t loops(Parameter::getInterval()); /* get minutes to sleep */
            loops = (loops * 60000ul) / service::Power::getSleepDurationMs();
            g_loops = (uint16_t)loops;
            g_timeAdjust = loops * service::Power::getSleepDurationMs();

            DEBUG_LOGP("sleep loops: %ld\r\n", loops);
        }
        printTime();
        service::Power::suspend();
    }
    
    void SleepState::process(StateHandler& stateHandler)
    {
        for (uint16_t i(g_loops); i != 0u; --i)
        {
            service::Power::sleep();
        }

        service::Power::resume(g_timeAdjust);

        IState* nextState(&UpdateState::instance());

        uint32_t vsn;
        if ( (0u != g_vsn) && 
             service::FileIo::getVolumeSerialNumber(vsn) &&
             (g_vsn != vsn))
        {
            DEBUG_LOGP("Card Swapped, restarting");
            nextState = &InitState::instance();
        }
        else
        {
            /* check for sufficient supply voltage
            */
            uint16_t supplyVoltage(service::Power::getSupplyVoltage_mV());
                        DEBUG_LOGP("voltage: %d\r\n",supplyVoltage);

            if (supplyVoltage < Parameter::getMinVoltage())
            {
                nextState = &LowBatState::instance();
                DEBUG_LOGP("goto lowpbattery\r\n");
            }
        }
        
        stateHandler.setState(*nextState);
    }

    void SleepState::leave(void)
    {
        DEBUG_LOGP("Wakeup()\r\n");
        printTime();
    }
}