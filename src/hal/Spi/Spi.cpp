
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

#include "Spi.h"

#include "service/Debug/Debug.h"
#include <avr/io.h>
#include <avr/pgmspace.h>

/*******************************************************************************
    Module statics
*******************************************************************************/

/** Block until current byte transmissing completes.
 */
static inline void waitTXcomplete();

/*******************************************************************************
    Implementation
*******************************************************************************/
namespace hal
{
    Spi::SlaveSelect Spi::m_slaveSelect;

    void Spi::hardwareInit()
    {
        m_slaveSelect = nullptr;

        /* initial pin setup before enabling SPI in MASTER mode
         */ 
        DDRB |=  _BV(PB5) | _BV(PB3) | _BV(PB2); // SCK and MOSI ,SS as output

        DDRB &= ~_BV(PB4);         // MISO is input
        PORTB |= _BV(PB4);         // MISO pulled high

        SPCR |= /*_BV(SPR1) |  */      // div 64 (slow for breadboard)
                _BV(MSTR) |        // MCU is SPI master
                _BV(SPE);          // turn on SPI
    }

    void Spi::configure(
            Spi::Mode mode,
            Spi::ByteOrder order,
            Spi::SlaveSelect slaveSelect)
    {
        uint8_t localSPCR(SPCR);

        if (BYTEORDER_MSB == order)
        {
            localSPCR &= ~_BV(1<<DORD);
        }
        else
        {
            localSPCR |= _BV(1<<DORD);
        }

        switch(mode)
        {
            case MODE_0:
                localSPCR &= ~(_BV(CPOL) | _BV(CPHA));
                break;

            case MODE_1:
                localSPCR &= ~(_BV(CPOL));
                localSPCR |= _BV(CPHA); 
                break;
            
            case MODE_2:
                localSPCR &= ~(_BV(CPHA));
                localSPCR |= _BV(CPOL); 
                break;

            case MODE_3:
                localSPCR |= (_BV(CPOL) | _BV(CPHA)); 
                break;
        }
        
        SPCR = localSPCR;

        m_slaveSelect = slaveSelect;
    }

    void Spi::read(uint8_t buffer[], uint16_t size)
    {
        if (nullptr != m_slaveSelect) 
        {
            m_slaveSelect(true);
        }

        for (uint16_t idx(0u); 0u != size; --size)
        {
            SPDR = 0xFF;            // dummy write to generate SPI clocks
            waitTXcomplete();
            buffer[idx++] = SPDR;   // store result
        }

        if (nullptr != m_slaveSelect) 
        {
            m_slaveSelect(false);
        }    }

    void Spi::write(const uint8_t buffer[], uint16_t size)
    {
        if (nullptr != m_slaveSelect) 
        {
            m_slaveSelect(true);
        }

        for (uint16_t idx(0u); 0u != size; --size)
        {
            SPDR = buffer[idx++];    // send next byte
            waitTXcomplete();
        }

        if (nullptr != m_slaveSelect) 
        {
            m_slaveSelect(false);
        }
    }

    void Spi::write_P(const uint8_t buffer[], uint16_t size)
    {
        if (nullptr != m_slaveSelect) 
        {
            m_slaveSelect(true);
        }

        uint16_t idx(0u);
        while (0u != size)
        {
            uint8_t val(pgm_read_byte(&(buffer[idx])));
            SPDR = val;    // send next byte
            ++idx;
            --size;
            waitTXcomplete();
        }

        if (nullptr != m_slaveSelect) 
        {
            m_slaveSelect(false);
        }
    }

    void Spi::exchange(uint8_t buffer[], uint16_t size)
    {
        if (nullptr != m_slaveSelect) 
        {
            m_slaveSelect(true);
        }

        for (uint16_t idx(0u); 0u != size; --size)
        {
            SPDR = buffer[idx];    // send next byte
            waitTXcomplete();
            buffer[idx++] = SPDR;  // replace with received one
        }

        if (nullptr != m_slaveSelect) 
        {
            m_slaveSelect(false);
        }
    }
}

static inline void waitTXcomplete()
{
    while (0u == (SPSR & _BV(SPIF)))
    {

    }
}