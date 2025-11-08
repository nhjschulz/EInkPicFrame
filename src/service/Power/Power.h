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
#ifndef POWER_H_INCLUDED
#define POWER_H_INCLUDED

#include "hal/Cpu/Cpu.h"
#include "hal/Timer/WakeUpTimer.h"
#include "hal/Timer/TickTimer.h"

#include "hal/Adc/Adc.h"

namespace service
{
    class Power
    {
        public:

        /**
         * @brief devices with switchable power
         *
         */
        enum Device
        {
            POW_SDCARD,         /**< SDCARD module   */
            POW_DISPLAY
        };

        /**
         * @brief Halt system.
         *
         * Requires reset to resume execution from start.
         */
        static void halt(void);

        /**
         * @brief sleep CPU
         *
         */
        static void sleep(void)
        {
            hal::Cpu::enterPowerSave();
        }

        /** Get sleep duration in ms
         *
         * In our case we are woken up by the WakeTimer, get its max interval.
         */
        static uint16_t getSleepDurationMs()
        {
            return hal::WakeUpTimer::WAKEUP_INTERVAl_MS;
        }

        /**
         * @brief Enter idle mode for given timer ticks (1tick = 10ms)
         *
         * @param ticks10ms Number of timer ticks to idle
         */
        static void idle(uint8_t ticks10ms)
        {
            hal::Cpu::enterIdle(ticks10ms);
        }

        /**
         * @brief Reboot system
         *
         */
        static void reboot(void)
        {
            hal::Cpu::reset();
        }

        /**
         * @brief Resume power for devices
         *
         * @param adjustTimeMs Sleep time to adjust millis count
         */
        static void resume(uint32_t adjustTimeMs);

        /**
         * @brief Suspend power
         *
         */
        static void suspend(void);

        /**
         * @brief Get uptime since power up in milliseconds
         *
         * @return uint32_t
         */
        static uint32_t uptime_mS(void)
        {
            return hal::TickTimer::getMillis();
        }

        /**
         * @brief Get the Supply Voltage in milivolt
         *
         * @return uint16_t voltage in mV
         */
        static uint16_t getSupplyVoltage_mV(void)
        {
            return hal::Adc::readChannel(hal::Adc::ADC_CHN_SUPPLY_VOLTAGE_MV);
        }

        static uint16_t getReferenceVoltage_mV(void)
        {
            return hal::Adc::readChannel(hal::Adc::ADC_CHN_CALIBRATION_MV);
        }

        /**
         * @brief Set the voltage parameter used for ADC calibration.
         *
         * @param refVoltage_mV  Voltage on AVCC pin for calibration
         * @param supVoltage_mv  Calibrated reference voltage
         */
        static void setCalibrationVoltages(
                uint16_t refVoltage_mV,
                uint16_t supVoltage_mv);

        private:
            Power(const Power&);
            Power& operator=(const Power&);

            /**
             * @brief Enable (power) given device.
             *
             * @param device  Device ID
             */
            static void enable(Device device);

            /**
             * @brief Disable (unpower) given device.
             *
             * @param device Device ID
             */
            static void disable(Device device);
    };
}
#endif