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

#include <stdio.h>

#include <avr/io.h>
#include <util/delay.h>

#include "hal/Uart/Uart.h"
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "service/Debug/Debug.h"

uint8_t sendBuff[40]; 
ByteQueue uartSendQ(sendBuff, sizeof(sendBuff));

uint8_t readBuff[16u]; 
Queue<uint8_t> uartReadQ(readBuff, sizeof(readBuff));

int main(int argc, char** argv)
{

    (void)argc;
    (void)argv;

    hal::Uart::Cfg cfg;

    cfg.m_baudRate = hal::Uart::BAUD_9600;
    cfg.m_mode = hal::Uart::MODE_READWRITE;
    cfg.m_parity = hal::Uart::PARITY_NONE;
    cfg.m_stopBits = hal::Uart::STOPBIT_1;
    cfg.m_inputQ = &uartReadQ;
    cfg.m_outputQ = &uartSendQ;

    hal::Uart& uart(hal::Uart::get());

    uart.open(cfg);
    DEBUG_INIT();

    // pin used for LED debug temporary 
    DDRB |= (1<<PB0);
    PORTB &= ~(1<<PB0);
//    PORTB |= (1<<PB0);


    sei();

    DEBUG_LOGP("EInkPicFrame V%d.%d\r\n", 0, 1);

    for(;;)
    {

        uint8_t byte(0);

        // local echo over UART test
        //
        if (hal::Uart::RET_SUCCESS == uart.receive(byte))
        {
            uart.send(byte);
        }
    }

    return 0;
}