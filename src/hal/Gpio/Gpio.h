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

#ifndef GPIO_H_INCLUDED
#define GPIO_H_INCLUDED

#include <avr/io.h>

namespace hal
{
    class Gpio 
    {
        public:

        /** SW accessed pins
        */
        enum Pins
        {
            DEBUG_LED    = PD2,
            DEBUG_TRIG   = PD3,

            SDCARD_CS    = PD4,
            SDCARD_POW   = PD5,

            DISP_BUSY    = PD7,
            DISP_RESET   = PB0,
            DISP_DC      = PB1,
            DISP_CS      = PB2,
            DISP_POW     = PD6,
            SPI_MOSI     = PB3,
            SPI_MISO     = PB4,
            SPI_CLK      = PB5
        };

        /**
         * @brief Bring all pins into a defined and save start state.
         * 
         */
        static void init(void);    

        /* inliners for each used pin
         */

        inline static  void setDebugLed()     { PORTD |=  _BV(DEBUG_LED);  }
        inline static  void clrDebugLed()     { PORTD &= ~_BV(DEBUG_LED);  }
        inline static  void setDebugTrig()    { PORTD |=  _BV(DEBUG_TRIG); }
        inline static  void clrDebugTrig()    { PORTD &= ~_BV(DEBUG_TRIG); }
  
        inline static void setSdCardCS()     { PORTD |=  _BV(SDCARD_CS);  }
        inline static void clrSdCardCS()     { PORTD &= ~_BV(SDCARD_CS);  }
        inline static void setSdCardPower()  { PORTD |=  _BV(SDCARD_POW); }
        inline static void clrSdCardPower()  { PORTD &= ~_BV(SDCARD_POW); }

        inline static void setDispReset()    { PORTB |=  _BV(DISP_RESET); }
        inline static void clrDispReset()    { PORTB &= ~_BV(DISP_RESET); }
        inline static void setDispDC()       { PORTB |=  _BV(DISP_DC);    }
        inline static void clrDispDC()       { PORTB &= ~_BV(DISP_DC);    }
        inline static void setDispCS()       { PORTB |=  _BV(DISP_CS);    }
        inline static void clrDispCS()       { PORTB &= ~_BV(DISP_CS);    }
        inline static void setDispPow()      { PORTD |=  _BV(DISP_POW);   }
        inline static void clrDispPow()      { PORTD &= ~_BV(DISP_POW);   }

        inline static bool getDispBusy()     { return (PIND & _BV(DISP_BUSY)) ? true : false ; }
    };
}
#endif /* GPIO_H_INCLUDED */
