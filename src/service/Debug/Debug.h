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

#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#include <avr/pgmspace.h>

/** Debugging support (enabled with -D WITH_DEBUG=1) */

#if WITH_DEBUG != 0

#define DEBUG_INIT()         service::Debug::init()
#define DEBUG_LOG(fmt, ...)  service::Debug::log(fmt, ##__VA_ARGS__)
#define DEBUG_LOGP(fmt, ...) service::Debug::logP(PSTR(fmt), ##__VA_ARGS__)
#define DEBUG_TRIGGER(v)     service::Debug::setTriggerPin(v)
namespace service
{
    /** Initialize debug module 
     *  @return false on failure UART openend with write before ?)
     */
    class Debug 
    {
        public:

        /** Initialize debug module 
        */
        static bool init(void);

        /** Printf wrapper to emit logs 
         * @param[in] fmt string  with printf fmt
         */
        static void log(const char * fmt, ...);

        /** Printf wrapper to emit logs with fmt sting in PROGMEM flash
         * @param[in] fmt string CONSTANT with printf fmt
         */
        static void logP(const char * fmt, ...);

        /**
         * @brief Set the Trigger Pin
         * 
         * @param set  true = high, false = low
         */
        static void setTriggerPin(bool set);
    };
}

#else //defined(WITH_DEBUG) 

#define DEBUG_INIT() true
#define DEBUG_LOG(fmt, ...) 
#define DEBUG_LOGP(fmt, ...) 
#endif //defined(WITH_DEBUG) 

#endif //DEBUG_H_INCLUDED