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
#include "Uart.h"

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

/** UART driver for the AVR328P USART in asynchronous mode */
namespace hal
{
    /** UART singleton instance
     */
    Uart Uart::m_instance;

    /** Baudrate configuration values
     */
    static const struct BaudrateSettings
    {
        uint8_t high;   /**< High byte   */
        uint8_t low;    /**< Low byte    */
        uint8_t u2x;    /**< U2X to set? */
    
    } regBaudrate[Uart::BAUD_INVALID] PROGMEM =
    {
        #define BAUD    2400
        #include <util/setbaud.h>
            {   UBRRH_VALUE, UBRRL_VALUE, USE_2X    },
        #undef BAUD

        #define BAUD    4800
        #include <util/setbaud.h>
            {   UBRRH_VALUE, UBRRL_VALUE, USE_2X    },
        #undef BAUD

        #define BAUD    9600
        #include <util/setbaud.h>
            {   UBRRH_VALUE, UBRRL_VALUE, USE_2X    },
        #undef BAUD

        #if F_CPU > 1000000

        #define BAUD    14400
        #include <util/setbaud.h>
            {   UBRRH_VALUE, UBRRL_VALUE, USE_2X    },
        #undef BAUD

        #define BAUD    19200
        #include <util/setbaud.h>
            {   UBRRH_VALUE, UBRRL_VALUE, USE_2X    },
        #undef BAUD

        #define BAUD    28800
        #include <util/setbaud.h>
            {   UBRRH_VALUE, UBRRL_VALUE, USE_2X    },
        #undef BAUD

        #define BAUD    38400
        #include <util/setbaud.h>
            {   UBRRH_VALUE, UBRRL_VALUE, USE_2X    },
        #undef BAUD

        #define BAUD    57600
        #include <util/setbaud.h>
            {   UBRRH_VALUE, UBRRL_VALUE, USE_2X    },
        #undef BAUD

        #define BAUD    76800
        #include <util/setbaud.h>
            {   UBRRH_VALUE, UBRRL_VALUE, USE_2X    },
        #undef BAUD

        #define BAUD    115200
        #include <util/setbaud.h>
            {   UBRRH_VALUE, UBRRL_VALUE, USE_2X    },
        #undef BAUD

        #define BAUD    234000
        #include <util/setbaud.h>
            {   UBRRH_VALUE, UBRRL_VALUE, USE_2X    },
        #undef BAUD

        #define BAUD    250000
        #include <util/setbaud.h>
            {   UBRRH_VALUE, UBRRL_VALUE, USE_2X    },
        #undef BAUD
        
        #endif
    };

    /** Parity register bit settings in UCSR0C
    */
    static const uint8_t regParityBits[Uart::PARITY_INVALID] PROGMEM =
    {
        (0 << UPM01) | (0 << UPM00), // Uart::PARITY_NONE
        (1 << UPM01) | (0 << UPM00), // Uart::EVEN
        (1 << UPM01) | (1 << UPM00), // Uart::ODD
    };

    /** Stop bit settings in UCSR0C
    */
    static const uint8_t regStopBits[Uart::STOPBIT_INVALID] PROGMEM =
    {
        (0 << USBS0),    // Uart::STOPBIT_1
        (1 << USBS0)     // Uart::STOPBIT_2
    };

    Uart::Uart() :
        m_isOpen(false),
        m_cfg()
    {
    }

    Uart::~Uart()
    {
    }

    Uart::RetVal Uart::open(const Uart::Cfg& cfg)
    {
        if (m_isOpen)
        {
            return RET_OPENALREADY;
        }

        uint8_t localUCSR0A(UCSR0A);
        uint8_t localUCSR0B(UCSR0B);
        uint8_t localUCSR0C(UCSR0C);

        /* Hardcode data width to 8 bit, never seen anything else ... 
         */
        localUCSR0C |= (_BV(UCSZ01) | _BV(UCSZ00));
        localUCSR0B &= ~_BV(UCSZ02);
        

        /* Configure parity mode */
        if (cfg.m_parity < Uart::PARITY_INVALID)
        {
            localUCSR0C &= ~(_BV(UPM01) | _BV(UPM00));
            localUCSR0C |= pgm_read_byte(&regParityBits[cfg.m_parity]);
        }
        else
        {
            return RET_INVPARAM;
        }

        /* Configure stopbits
         */
        if (cfg.m_parity < Uart::PARITY_INVALID)
        {
            localUCSR0C &= ~_BV(USBS0);
            localUCSR0C |= pgm_read_byte(&regStopBits[cfg.m_stopBits]);
        }
        else
        {
            return RET_INVPARAM;
        }

        /* Configure baudrate
         */
        if (cfg.m_baudRate < Uart::BAUD_INVALID)
        {
            if (0 == pgm_read_byte(&regBaudrate[cfg.m_baudRate].u2x))
            {
                localUCSR0A &= ~_BV(U2X0);
            }
            else
            {
                localUCSR0A |= _BV(U2X0);
            }

            UBRR0H = pgm_read_byte(&regBaudrate[cfg.m_baudRate].high);
            UBRR0L = pgm_read_byte(&regBaudrate[cfg.m_baudRate].low);;
        }
        else
        {
            return RET_INVPARAM;
        }

        /* Enable transitter / receiver based on mode
         */
        localUCSR0B &= ~(_BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0) | _BV(UDRIE0));
        switch(cfg.m_mode)
        {
            case MODE_READ:
                if (nullptr == cfg.m_inputQ)
                {
                    return RET_INVPARAM;
                }
                cfg.m_inputQ->clear();

                localUCSR0B |= (_BV(RXEN0) | _BV(RXCIE0));
                break;

            case MODE_WRITE:
                if (nullptr == cfg.m_outputQ)
                {
                    return RET_INVPARAM;
                }
                cfg.m_outputQ->clear();

                localUCSR0B |= _BV(TXEN0);
                break;
            
            case MODE_READWRITE:
                if ((nullptr == cfg.m_inputQ) || (nullptr == cfg.m_outputQ))
                {
                    return RET_INVPARAM;
                }
                cfg.m_inputQ->clear();
                cfg.m_outputQ->clear();

                localUCSR0B |= (_BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0));
                break;
            
            case MODE_INVALID:
            default:
                return RET_INVPARAM;
                break;
        }

        m_cfg = cfg;
        m_isOpen = true;

        /* Shoot! (B last, it enables UART RX/TX)... 
         */
        UCSR0A = localUCSR0A;
        UCSR0C = localUCSR0C;
        UCSR0B = localUCSR0B;

        return RET_SUCCESS;
    }

    Uart::RetVal Uart::close()
    {
        if (!m_isOpen)
        {
            return RET_NOTOPEN;
        }

        m_isOpen = false;

        /* disable receiver/transmitter and interrupts
        */
        UCSR0B &= ~(_BV(RXEN0)  | _BV(TXEN0)  | 
                    _BV(RXCIE0) | _BV(TXCIE0) | _BV(UDRIE0));

        return RET_SUCCESS;
    }

    Uart::RetVal Uart::send(uint8_t byte)
    {
        if (!m_isOpen)
        {
            return RET_NOTOPEN;
        }

        if ((m_cfg.m_mode != MODE_WRITE) && (m_cfg.m_mode != MODE_READWRITE))
        {
            return RET_INVMODE;
        }

        if (!m_cfg.m_outputQ->put(byte))
        {
                return RET_NOSPACE;
        }

        /* if transmit interrupt is not enabled, we have no ongoing
         * transmition. Enable it now and put the current character out.
         */
        if (0u == (UCSR0B & _BV(UDRIE0)))
        {
            m_cfg.m_outputQ->get(byte);
            UDR0 = byte;
            UCSR0B |= _BV(UDRIE0);

        }

        return RET_SUCCESS;
    }

    Uart::RetVal Uart::sendP(const uint8_t buffer[] , uint8_t len)
    {
        if (!m_isOpen)
        {
            return RET_NOTOPEN;
        }

        if ((m_cfg.m_mode != MODE_WRITE) && (m_cfg.m_mode != MODE_READWRITE))
        {
            return RET_INVMODE;
        }

        for (uint8_t idx(0); idx  < len; ++idx)
        {
            send(pgm_read_byte(&buffer[idx]));
        }

        return RET_SUCCESS;
    }


    Uart::RetVal Uart::read(uint8_t& byte)
    {
        if (!m_isOpen)
        {
            return RET_NOTOPEN;
        }

        return m_cfg.m_inputQ->get(byte) ? RET_SUCCESS : RET_NODATA;
    }

    inline bool uart_isrGet(uint8_t& byte)
    {
        return Uart::m_instance.m_cfg.m_outputQ->get(byte);
    } 

    inline bool uart_isrPut(uint8_t byte)
    {
        return Uart::m_instance.m_cfg.m_inputQ->put(byte);
    } 
}

/** Interrupt entry for USART data register empty */
ISR(USART_UDRE_vect)
{
    uint8_t nextByte(0);

    if (hal::uart_isrGet(nextByte))
    {
        UDR0 = nextByte;
    }
    else
    {
        /* no further data, disable tx complete interrupt
         */
        UCSR0B &= ~(_BV(UDRIE0)); 
    }
}

/** Interrupt entry for USART data receive complete */
ISR(USART_RX_vect)
{
    uint8_t byte(UDR0);

    PORTB |= (1<<PB0);

    hal::uart_isrPut(byte);
    PORTB &= ~(1<<PB0);

}

/* EOF */