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

#include "app/Parameter.h"

#include "service/FileIo/FileIo.h"
#include "service/FatFS/source/ff.h"
#include "service/Debug/Debug.h"
#include "hal/Cpu/Cpu.h"
#include <util/crc16.h>

namespace app
{
    /** Version 1 header format
     */
    static const uint8_t g_headerV1[] PROGMEM = { 'E', 'P', 'D', 1u, 4u };

     /** Version 1 config file content
      */
    struct CfgFileV1
    {
        char               signature[3];  /**< 'E' 'P' 'D'            */
        uint8_t            version;       /**< version(1)             */
        uint8_t            count;         /**< number of parameter(4) */
        uint8_t            crc8;          /**< CRC8 over parameter    */
        union 
        {
            Parameter::ParamV1 param;
            uint8_t bytes[sizeof(Parameter::ParamV1)];
        } u;
    };

    /** Parameter runtime storage
     * 
     * Initialized with defaults 
     */
    Parameter::ParamV1 Parameter::m_param = 
    {
        1440u,  /* Interval 1440 min = 1 day        */
        3300u,  /* low supply voltage limit         */
        1100u,  /* reference voltage                */
        5000u   /* supply voltage during calibation */
    };

    bool Parameter::init()
    {
        bool result(false);

        FIL file;       
        FRESULT res(f_open(&file, "epd.cfg", FA_READ));
        DEBUG_LOGP("Parameter::open() -> %d\r\n", res);

        if (FR_OK == res)
        {
            CfgFileV1 cfg;

            UINT read(0u);
            res = f_read(&file, &cfg, sizeof(cfg), &read);
            DEBUG_LOGP("Parameter::read() -> %d\r\n", res);

            if ((FR_OK == res) && (sizeof(cfg) == read))
            {
                /*  read ok, validate header and parameter CRC 
                 */
                if (!memcmp_P(&cfg, g_headerV1, sizeof(g_headerV1)))
                {
                    uint8_t crc8(0u);
                    for (uint8_t i(0u); i < sizeof(Parameter::ParamV1); ++i)
                    {
                        crc8 = _crc8_ccitt_update(crc8, cfg.u.bytes[i]);
                    }
                    
                    if (crc8 == cfg.crc8)
                    {
                        m_param = cfg.u.param;    /* accept cfg file data*/
                        result = true;
                    }
                    else
                    { 
                        DEBUG_LOGP(
                            "param: crc mismatch: %d-%d\r\n", 
                            crc8, cfg.crc8);
                    }
                }
                else 
                {
                    DEBUG_LOGP("unsupported parameter header\r\n");
                }
            }
            else
            {
                DEBUG_LOGP("param:read: got %d, wanted %d\r\n",read, sizeof(cfg));
            }

            f_close(&file);
        }

        DEBUG_LOGP("p.interval   : %d min\r\n", m_param.interval);
        DEBUG_LOGP("p.lowVoltage : %d mv\r\n", m_param.minVoltage);
        DEBUG_LOGP("p.refVoltage : %d mv\r\n", m_param.refVoltage);
        DEBUG_LOGP("p.supVoltage : %d mv\r\n", m_param.supVoltage);

        return result;
    }
}