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

#ifndef UART_H_INCLUDED
#define UART_H_INCLUDED

/** UART driver for the AVR328P USART in asynchronous mode */

#include "Queue.h"

#include <stdint.h>

namespace hal
{
    /** AVR ATMega328 Uart driver */

    class Uart {
        public:

        /** API error codes                                              */
        enum RetVal 
        {
            RET_SUCCESS,             /* it worked                        */
            RET_OPENALREADY,         /* attempt to open again            */
            RET_NOTOPEN,             /* attempt to close without opening */
            RET_INVPARAM,            /* invalid parameter                */
            RET_INVMODE,             /* open mode does not support this  */
            RET_NOSPACE,             /* not enough space to support this */
            RET_NODATA               /* no data available on input       */
        };

        /** Supported baudrates */
        enum BaudRate
        {        
           BAUD_2400,      /**< 2400 Baud           */
           BAUD_4800,      /**< 4800 Baud           */
           BAUD_9600,      /**< 9600 Baud           */
           
     #if F_CPU > 1000000

           BAUD_14400,     /**< 14400 Baud          */
           BAUD_19200,     /**< 19200 Baud          */
           BAUD_28800,     /**< 28800 Baud          */
           BAUD_38400,     /**< 38400 Baud          */
           BAUD_57600,     /**< 57600 Baud          */
           BAUD_76800,     /**< 76800 Baud          */
           BAUD_115200,    /**< 115200 Baud         */
           BAUD_230400,    /**< 230400 Baud         */
           BAUD_250000,    /**< 250000 Baud         */
    #endif

           BAUD_INVALID    /**< defined wrong value */
        };
        
        /** Supported parity modes */
        enum Parity
        {
            PARITY_NONE,   /**< None parity         */
            PARITY_EVEN,   /**< Even parity         */
            PARITY_ODD,    /**< Odd parity          */
            PARITY_INVALID /**< defined wrong value */
        };
#if 0
        /** Supported data bit width  - DISABLED - Who ever uses != 8 ?*/
        enum DataWidth
        {
            DATA_5,        /**< 5 bit per data word */
            DATA_6,        /**< 6 bit per data word */
            DATA_7,        /**< 7 bit per data word */
            DATA_8,        /**< 8 bit per data word */
            DATA_INVALID   /**< defined wrong value */
        };
#endif
        enum StopBits
        {
            STOPBIT_1,      /**< 1 stopbit           */
            STOPBIT_2,      /**< 2 stopbit           */
            STOPBIT_INVALID /**< defined wrong value */
        };

    	/** Supported modes */
        enum Mode 
        {
            MODE_READ =      0x01,     /**< read enabled        */
            MODE_WRITE =     0x02,     /**< write enabled       */
            MODE_READWRITE = 0x03,     /**< read/write          */
            MODE_INVALID  =  0x00      /**< defined wrong value */
        };

        /** Uart configuration for opening */
        struct Cfg 
        {
            uint8_t       m_mode;       /**< r/w/rw mode   */
            BaudRate      m_baudRate;   /**< baudrate      */
            Parity        m_parity;     /**< paritity mode */
            StopBits      m_stopBits;   /**< stop bits     */
            ByteQueue *   m_inputQ;     /**< input queue, NULL for MODE_WRITE */
            ByteQueue *   m_outputQ;    /**< output queue, NULL for MODE_READ */
        };

        /** Open com port with given configuration
         * @param config port configuration 
         * @return RetVal enum value
         */
        RetVal open(const Cfg& config);

        /** Close port 
         *  This will stop communication after draining pending data.
         */
        RetVal close();
 
        /** Send a byte
         */
        RetVal send(uint8_t byte);

        /** Send string from progmem 
         */
        RetVal sendP(const uint8_t buffer[], uint8_t len);

        /** Read a byte
         */
        RetVal receive(uint8_t& byte);

        /** Check if UART is opened in write mode */
        bool canWrite() 
        {
            return (m_isOpen && ( 0u != (m_cfg.m_mode & MODE_WRITE)));
        }

        /** Singelton access 
        */
        static Uart& get() { return m_instance; }

        private:

        bool m_isOpen;             /**< indicate if UART connection is aktiv */
        Cfg  m_cfg;                /**< configuration used on open           */

        Uart(const Uart&);
        Uart& operator=(const Uart&);

        Uart();
        ~Uart();

        /** Interface for Data reg empty interrupt. 
         *  Used to obtain next byte from output queue
         * @paramn[out] byte next byte from output queue
         * 
         * @return true if a byte was available.
         */
        friend bool uart_isrGet(uint8_t& byte);

        /** Interface for RX interrupt to data int the input queue
         * @paramn[out] byte next byte from output queue
         * 
         * @return true if a byte could be stored.
         */
        friend bool uart_isrPut(uint8_t byte);
      
        /**singleton  instance 
         */
        static Uart m_instance; 

    };
}
#endif // UART_H_INCLUDED