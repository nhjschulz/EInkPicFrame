/*
 * BSD 3-Clause License
 * 
 * Copyright (c) 2021, Norbert Schulz
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

#ifndef SPI_H_INCLUDED
#define SPI_H_INCLUDED

#include <stdint.h>

namespace hal
{
    /** SPI Driver for AVR 328P
    */
    class Spi 
    {
        public:

            /** Function pointer type for user provided slave select function
             *  This function gets called prior and after a transmission.
             * 
             * @param select true at start of transmission, false afterwards.
             */
            typedef void (*SlaveSelect)(bool select);

            /** SPI data modes
             */
            enum Mode 
            {
                MODE_0, /**< CPOL=0, CPHA=0 Sample (Rising) Setup (Falling) */
                MODE_1, /**< CPOL=0, CPHA=1 Setup (Rising) Sample (Falling) */
                MODE_2, /**< CPOL=1, CPHA=0 Sample (Falling) Setup (Rising) */
                MODE_3  /**< CPOL=1, CPHA=1 Setup (Falling) Sample (Rising) */
            };

            enum ByteOrder
            {
                BYTEORDER_MSB,   /**< MSB first */
                BYTEORDER_LSB    /**< LSB first */
            };

            /** Initialize SPI hardware
             */
            static void hardwareInit();

            /** Configure SPI transfers
             *  @param[in] mode tansfer mode @see enum Mode
             *  @param[in] order bit order in transmits
             *  @param[in] slaveSelect slave select/deselect function
             */
            static void configure(Mode mode, ByteOrder order, SlaveSelect slaveSelect);

            /** Read bytes from SPI (by sending zeros)
             *  @param[out] buffer received data
             *  @param[in] size number of bytes to store in buffer
             */
            static void read(uint8_t buffer[], uint16_t size);

            /** Write bytes to SPI (ignoring received data)
             *  @param[out] buffer received data
             *  @param[in] size number of bytes to store in buffer
             */
            static void write(const uint8_t buffer[], uint16_t size);

            /** Write bytes to SPI from program space (ignoring received data)
             *  @param[out] buffer received data
             *  @param[in] size number of bytes to store in buffer
             */
            static void write_P(const uint8_t buffer[], uint16_t size);

            /** Exchange bytes over SPI
             * 
             *  Buffer gets filled with received bytes while sending
             *  @param[out] buffer received data
             *  @param[in] size number of bytes to store in buffer
             */
            static void exchange(uint8_t buffer[], uint16_t size);

        private:
            static SlaveSelect m_slaveSelect;  /**< slave selection function */

        private:
            Spi(const Spi&);
            Spi& operator=(const Spi&);
    };
}

#endif // SPI_H_INCLUDED