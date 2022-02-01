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

#include "Adc.h"

#include <avr/power.h>
#include <avr/io.h>
#include <util/delay.h>

/*******************************************************************************
    Module statics
*******************************************************************************/

static uint16_t g_refVoltage_mV(1100u);   /**< internal reference volatage    */
static uint16_t g_supVoltage_mV(5000ul);  /**< supply voltage for calibration */
namespace hal
{
    static inline uint16_t triggerConversion(Adc::AdcChannel channel)
    {
        (void)channel; /* always the same gets used */
        
        /* start conversion */
        ADCSRA |= (1 << ADSC);

        /* wait for finish */
        while (ADCSRA & (1 << ADSC))
        {
        }

        return ADC;
    }

    void Adc::init(void)
    {
        Adc::disable();
    }

    void Adc::enable(void)
    {
        power_adc_enable();

        /* 
         * REF:   01b AVCC with external capacitor at AREF pin
         * ADLAR: 0b right adjust
         * MUX:   1110b measure bandgap voltage of fixed 1.1V
         */
        ADMUX = (0 << REFS1) | (1 << REFS0)
                | (0 << ADLAR)
                | (1 << MUX3) | (1 << MUX2)| (1 << MUX1) | (0 << MUX0);

        /*
         * Enable ADC  (1 = ADEN)
         * Don't start yet (0= ADSC)
         * No AutoTrigger (0 = ADATE)
         * No interrupts (0=ADIF, 0= ADIE) 
         * Prescaler 32 (101b): 4Mhz / 32 = 125khz
         */
        ADCSRA = (1 << ADEN)
                | (0 << ADSC)
                | (0 << ADATE)
                | (0 << ADIF) | (0 << ADIE)
                | (1 << ADPS2) | (0 << ADPS1) | (1 << ADPS0);  

        _delay_us(500);    /* wait for selected voltages to stabilize */

        /* dummy read as first result may be inaccurate.
         */
        triggerConversion(ADC_CHN_SUPPLY_VOLTAGE_MV); 

    }

    uint16_t Adc::readChannel(Adc::AdcChannel channel)
    {
        uint16_t result(0xFFFF);
        uint32_t adc(triggerConversion(channel));

        switch(channel)
        {
            case Adc::ADC_CHN_SUPPLY_VOLTAGE_MV:
                /* ADC = (VIN * 1024) / VREF =>   VREF = (VIN * 1024) / ADC
                 * VIN = 1100 mV
                */
                if (0u != adc)
                {
                    result = (g_refVoltage_mV * 1024ul) / ADC;
                }
                else
                {
                    result = 0u;
                }
                break;

            case Adc::ADC_CHN_CALIBRATION_MV:
                /* ADC = (VIN * 1024) / VREF =>   VIN = (ADC * VREF) / 1024
                 * VIN = 1100 mV, VREF assumed to be calibrated to 5.0 V
                */
                result = (uint16_t)((adc * g_supVoltage_mV) / 1024ul);
                break;
        }

        return result;
    }

    void Adc::disable(void)
    {
        ADCSRA = 0x00;      /* disable ADC               */
        power_adc_disable();
    }

    void Adc::calibrate(
            uint16_t refVoltage_mV,
            uint16_t supVoltage_mv)
    {
        g_refVoltage_mV = refVoltage_mV;
        g_supVoltage_mV = supVoltage_mv;
    }

}