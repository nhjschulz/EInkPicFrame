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
#ifndef FILIO_H_INCLUDED
#define FILIO_H_INCLUDED

#include <stdint.h>

namespace service
{
    /**
     * @brief Lightweight abstraction class for FileIO.
     * 
     * Abstracts from the used FAT FS implementation to limit
     * rework in case we'll use another library in future.
     */
    class FileIo
    {
        public:

        /**
         * @brief size of shared io buffer.
         * 
         */
        static const uint8_t SHARED_BUF_SIZE = 100u;

        /**
         * @brief Shared data buffer for file IO
         * 
         * A shared buffer is used to reduce precious RAM consumption
         * on the AVR 328P. 
         */
        static uint8_t iobuf[SHARED_BUF_SIZE];

        /**
         * @brief Enable file IO access
         * 
         * Power SD Card modul and initialze it
         * 
         */
        static bool enable();

        /**
         * @brief Disable FileIO
         * 
         */
        static void disable();

        /**
         * @brief Initialize FileIO
         * 
         * FatFS mount and directory read initialisation.
         * 
         * @return true All worked fine
         * @return false error occured
         */
        static bool init();

        /**
         * @brief open current file
         * 
         * @return true 
         * @return false 
         */
        static bool open(const char * fname = getFileName());

        /**
         * @brief closde current file
         * 
         * @return true 
         * @return false 
         */
        static bool close();

        /**
         * @brief advance to next file
         * 
         * @return true 
         * @return false 
         */
        static bool next();
 
        /**
         * @brief read a block from open file
         * 
         * @param buf buffer to receive data
         * @param size number of bytes in buffer
         * @param read return number of byes read
         * @return true 
         * @return false 
         */
        static bool read(uint8_t * buf, uint16_t size, uint16_t& read);

        /**
         * @brief Get the current filename for diretory iterating
         * 
         * @return const char*  next filename
         */
        static const char * getFileName();

        private:

        FileIo(const FileIo&);
        FileIo& operator=(const FileIo&);
    };
}

#endif /* FILIO_H_INCLUDED */