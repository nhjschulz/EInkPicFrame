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
    /** SW accessed pins */
    enum Pins
    {
        PIN_DEBUG_LED    = PD2,
        PIN_DEBUG_TRIG   = PD3,

        PIN_SDCARD_CS    = PD4,
        PIN_SDCARD_POW   = PD5,

        PIN_DISP_BUSY    = PD7,
        PIN_DISP_RESET   = PB0,
        PIN_DISP_DC      = PB1,
        PIN_DISP_CS      = PB2,
        PIN_DISP_POW     = PD6,
        PIN_SPI_MOSI     = PB3,
        PIN_SPI_MISO     = PB4,
        PIN_SPI_CLK      = PB5
    };

    /**
     * @brief Bring all pins into a defined and save start state.
     * 
     */
    void initGpio(void);    

    inline void setPinDebugLed()     { PORTB |=  _BV(PIN_DEBUG_LED);  }
    inline void clrPinDebugLed()     { PORTB &= ~_BV(PIN_DEBUG_LED);  }
    inline void setPinDebugTrig()    { PORTB |=  _BV(PIN_DEBUG_TRIG); }
    inline void clrPinDebugTrig()    { PORTB &= ~_BV(PIN_DEBUG_TRIG); }

    inline void setPinSdCardCS()     { PORTD |=  _BV(PIN_SDCARD_CS);  }
    inline void clrPinSdCardCS()     { PORTD &= ~_BV(PIN_SDCARD_CS);  }
    inline void setPinSdCardPower()  { PORTD |=  _BV(PIN_SDCARD_CS);  }
    inline void clrPinSdCardPower()  { PORTD &= ~_BV(PIN_SDCARD_CS);  }

    inline void setPinDispReset()    { PORTB |=  _BV(PIN_DISP_RESET); }
    inline void clrPinDispReset()    { PORTB &= ~_BV(PIN_DISP_RESET); }
    inline void setPinDispDC()       { PORTB |=  _BV(PIN_DISP_DC);    }
    inline void clrPinDispDC()       { PORTB &= ~_BV(PIN_DISP_DC);    }
    inline void setPinDispCS()       { PORTB |=  _BV(PIN_DISP_CS);    }
    inline void clrPinDispCS()       { PORTB &= ~_BV(PIN_DISP_CS);    }
    inline void setPinDispPow()      { PORTD |=  _BV(PIN_DISP_POW);   }
    inline void clrPinDispPow()      { PORTD &= ~_BV(PIN_DISP_POW);   }

    inline bool getPinDispBusy()     { return (PIND & _BV(PIN_DISP_BUSY) ? true : false ); }

}
#endif /* GPIO_H_INCLUDED */
