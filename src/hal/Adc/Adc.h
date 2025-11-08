/*
 * BSD 3-Clause License
 * 
 * Copyright (c) 2022-2025, Norbert Schulz
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

#ifndef ADC_H_INCLUDED
#define ADC_H_INCLUDED

#include <stdint.h>

namespace hal 
{
    /** 
     * ADC interface for determining supply voltage.
     * 
     * The supply voltage is determined based on the internal reference
     * voltage of ~ 1100mv. Note that the internal voltage varies slightly
     * between chips. The result is therefore not accurate, but good enough
     * for defining a cut-off voltage that avoids draining recharchable
     * battries to death.
     *
     * It is possible to calibrate the voltage based on knowledge of the 
     * exact internal voltage. For this, the supply voltage must be 
     * accurately known and set (see define SUPPLY_VOLTAGE). The channel
     * ADC_CHN_CALIBRATION_MV delivers the internal voltage in mV, given
     * a known supply voltage.
     */
    class Adc
    {
        public:

            enum AdcChannel
            {
                 ADC_CHN_SUPPLY_VOLTAGE_MV,   /**< Supply Voltage millivolt */
                 ADC_CHN_CALIBRATION_MV       /**< read internal bandgap mV */
            };

            /**
             * @brief Initialisation of ADC module
             * 
             */
            static void init(void);

            /**
             * @brief Enable (turn on) ADC module
             * 
             */
            static void enable(void);

            /**
             * @brief Disable (turn off) ADC module
             * 
             */
            static void disable(void);

            /**
             * @brief Read a ADC channel
             * 
             * @param channel  channel ID
             * @return uint16_t  16bit result in channel unitss
             */
            static uint16_t readChannel(AdcChannel channel);

            /**
             * @brief 
             * 
             * @param refVoltage_mV  Voltage on AVCC pin for calibration
             * @param supVoltage_mv  Calibrated reference voltage
             */
            static void calibrate(
                    uint16_t refVoltage_mV,
                    uint16_t supVoltage_mv);

        private:
            Adc();
            Adc(const Adc&);
            Adc& operator=(const Adc&);
    };
}
#endif /* ADC_H_INCLUDED */

