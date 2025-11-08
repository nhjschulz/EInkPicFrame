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

/** Debugging support - yes printf like (enabled with -D WITH_DEBUG=1) */

#if WITH_DEBUG != 0

#include "service/Debug/Debug.h"

#include "hal/Uart/Uart.h"
#include "hal/Gpio/Gpio.h"
#include "hal/Timer/TickTimer.h"

#include <stdio.h>
#include <stdarg.h>

/*******************************************************************************
    Module statics data
*******************************************************************************/
/** FILE stream to UART */
static FILE uartFileStream;


static uint8_t sendBuff[120];
static ByteQueue uartSendQ(sendBuff, sizeof(sendBuff));

static uint8_t readBuff[16u];
static Queue<uint8_t> uartReadQ(readBuff, sizeof(readBuff));

/*******************************************************************************
    Module statics
*******************************************************************************/

/** Send a byte to uart
 *  @param[in] character byte to send
 *  @param[in] stream ignored, always uart
 */
static int uartPutChar(char character, FILE *stream);

namespace service
{

    bool Debug::init()
    {
        bool result(false);

        hal::Uart::Cfg cfg;

        cfg.m_baudRate = hal::Uart::BAUD_9600;
        cfg.m_mode = hal::Uart::MODE_READWRITE;
        cfg.m_parity = hal::Uart::PARITY_NONE;
        cfg.m_stopBits = hal::Uart::STOPBIT_1;
        cfg.m_inputQ = &uartReadQ;
        cfg.m_outputQ = &uartSendQ;

        hal::Uart& uart(hal::Uart::get());

        uart.open(cfg);

        if (uart.canWrite())
        {
            uartFileStream.put = uartPutChar;
            uartFileStream.get = nullptr;
            uartFileStream.flags = _FDEV_SETUP_WRITE;
            result = true;
        }

        return result;
    }

    void Debug::log(const char * fmt, ...)
    {
        va_list args;
        va_start (args, fmt);
        vfprintf (&uartFileStream, fmt, args);
        va_end (args);
    }

    static const char  pfx[] PROGMEM= "%8ld : ";

    void Debug::logP(const char * fmt, ...)
    {
        fprintf_P(&uartFileStream, pfx, hal::TickTimer::getMillis());
        va_list args;
        va_start (args, fmt);
        vfprintf_P(&uartFileStream, fmt, args);
        va_end (args);
    }

    void DebugsetTriggerPin(bool set)
    {
        if (set)
        {
            hal::Gpio::setDebugTrig();
        }
        else
        {
            hal::Gpio::clrDebugTrig();
        }
    }
}

static int uartPutChar(char character, FILE *stream)
{
    (void)stream;

    while (hal::Uart::RET_SUCCESS != hal::Uart::get().send(character))
    {

    }

	return 0;
}

#endif // defined(WITH_DEBUG)
