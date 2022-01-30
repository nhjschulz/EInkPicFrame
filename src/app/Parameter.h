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

#ifndef PARAMETER_H_INCLUDED
#define PARAMETER_H_INCLUDED

#include <stdint.h>

namespace app
{
    /** Manage supported parameter to influence 
     *  runtime behavior. 
     * 
     * Parameter are read from the file /epd/epd.cfg.
     */
    class Parameter
    {
        public:
            /**
             * @brief Try to load epd.cfg file
             * 
             * @return true  if parameter updated from cfg file
             * @return false running with defaults
             */
            static bool init(void);
            
            /**
             * @brief Get the Interval value in minutes
             * 
             * @return uint16_t minutes between updates
             */
            static uint16_t getInterval(void);

            /**
             * @brief Get the Min Voltage 
             * 
             * Lower voltage limit value in mV. Processing stops with
             * charge indication if voltage drops below this value.
             * Values below 3300 mV are pointless, as peripherals
             * need it.
             * 
             * @return uint16_t minimal operation voltage.
             */
            static uint16_t getMinVoltage(void);

            /**
             * @brief Get the Ref Voltage
             * 
             * It defines the 
             * reference voltage for the ADC conversion to determine
             * the supply voltage. It is usually not needed to tweak
             * this, as an error of aroud 10% should not hurt. 
             * 
             * Background is that the AVR internal reference is not 
             * accurate. It is only 'near' the defined 1.1V and varies 
             * from chip to chip.
             * 
             * @return uint16_t calibrated internal reference voltage. 
             */
            static uint16_t getRefVoltage(void);

            /**
             * @brief Get the calibraton Voltage
             * 
             * This value is not relevant for normal operation. It is used
             * for calibrating the reference voltage. Set this parameter to
             * the value on the AVCC pin when measured with a trusted
             * multimeter. Then check the values of the debug prints for the
             * reference voltage. If it shows i.e. 1080, change the reference
             * voltage from 1100 to 1080 to get better acurracy for computing
             * the supply voltage.
             * 
             * @return uint16_t given supply voltage during calibration
             */
            static uint16_t getCalVoltage(void);

             /** Initial (v1) parameter set Definition
              */
            struct ParamV1
            {
                uint16_t interval;
                uint16_t minVoltage;
                uint16_t refVoltage;
                uint16_t supVoltage;
            };

        private:
            static ParamV1 m_param;  /**< valid paramter during runtime */
    };

    inline uint16_t Parameter::getInterval(void) 
    {
        return m_param.interval;
    }

    inline uint16_t Parameter::getMinVoltage(void) 
    {
        return m_param.minVoltage;
    }

    inline uint16_t Parameter::getRefVoltage(void) 
    {
        return m_param.refVoltage;
    }

    inline uint16_t Parameter::getCalVoltage(void) 
    {
        return m_param.supVoltage;
    }
}

#endif /* PARAMETER_H_INCLUDED */