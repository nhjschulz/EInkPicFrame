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

#ifndef DISPLAY_H_INCLUDED
#define DISPLAY_H_INCLUDED

#include <stdint.h>

namespace service 
{
    /** Display driver for Waveshare EPD_5IN65F 7 Color E-Paper Display
     */
    class Epd 
    {
    public:

        /** Possible color values for pixels (cleaning results in ~sepia)
         */
        enum Color 
        {
            BLACK = 0x0,
            WHITE = 0x1,
            GREEN = 0x2,
            BLUE  = 0x3,
            RED   = 0x4,
            YELLOW= 0x5,
            ORANGE= 0x6,
            CLEAN = 0x7        /**< clean to avoid after images */
        };

        /** Initialize display after power on 
         */
        static bool init(void);

        /** Reset display
         */
        static void reset(void);

        /** Initiate painting
         * 
         *  Must be done before calling sendBlock
         */
        static void beginPaint();

        /** Send a block of pixel data to the display
         *  Must be called between beginPaint() and endPaint()
         *  @param block A pointer to image, each byte holds 2 4Bit color values
         *  @param size  number of bytesd to send
         */
        static void sendBlock(const uint8_t * block, uint8_t size);

        /* Finish update 
         */
        static void endPaint();

        /** Put display into deep sleep 
         */
        static void sleep(void);

        /** Clear (fill) display with given color
         */
        static void clear(Color color);

        /** Display X Resolution
         */
        static uint16_t getWidth()
        {
            return 600u;
        }

        /** Display Y Resolution
         */
        static uint16_t getHeight()
        {
            return 448u;
        }

    private:

        /**
         * @brief Wait until display becomes idle (BUSYN  high) 
         * 
         * @param timeOut_ms  timeout period for waiting
         * @return true  display entered idle
         * @return false timeout waiting for idle
         */
        static bool waitForIdle(uint8_t timeOut_ms);

        /**
         * @brief  Wait until display becomes busy (BUSYN  low) 
         * 
         * @param timeOut_ms  timeout period for waiting
         * @return true  display entered idle
         * @return false timeout waiting for busy
         */
        static bool waitForBusy(uint8_t timeOut_ms);

        /** Send command from program space
         * @param cmd pointer to command byte + cmd data bytes (if any)
         * @param size number of bytes used by this commmand
         */
        static void sendCmd_P(const uint8_t * cmd, uint8_t size);

        /** Configure SPI for display usage
         */
        static void configureSpi();
    };
}
#endif /* DISPLAY_H_INCLUDED */
