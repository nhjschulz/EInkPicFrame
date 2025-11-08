/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2022-2025, Norbert Schulz
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

#include "service/FileIo/FileIo.h"

#include "service/Power/Power.h"
#include "service/Debug/Debug.h"
#include "service/FatFS/source/ff.h"
#include "service/FatFS/source/diskio.h"

/**
 * @brief Internal state of file system access
 *
 */
enum FileIoStatus
{
    FIO_RESET,
    FIO_MOUNT,
    FIO_READY,
    FIO_OPEN,
    FIO_ERROR
};

/*******************************************************************************
    Module statics
*******************************************************************************/

static FileIoStatus  g_status = FIO_RESET;
static FATFS    g_fs;     /**< file system information for FatFS */
static DIR      g_dir;    /**< directory information for FatFS   */
static FILINFO  g_fno;    /**< directory fíle info for FatFS     */
static FIL      g_fil;    /**< open file handle                  */
static bool     g_enable; /**< true if enabled                   */

static const char g_fnPattern[] = "*.epd";
static const char g_dirPath[] = "/epd/img";

/*******************************************************************************
    Implementation
*******************************************************************************/
namespace service
{
    uint8_t FileIo::iobuf[service::FileIo::SHARED_BUF_SIZE];

    bool FileIo::init(void)
    {
        FRESULT res(f_mount(&g_fs, "", 1));

        DEBUG_LOGP("FileIo f_mount-> %d\r\n", res);

        if (FR_OK == res)
        {
           g_status = FIO_MOUNT;

            /* expect data in epd directory */
            res = f_chdir(g_dirPath);
            DEBUG_LOGP("FileIo f_chdir-> %d\r\n", res);

            if (FR_OK == res)
            {
                /* start to search for *.epd in /epd  directory
                 */
                res = f_findfirst(&g_dir, &g_fno, g_dirPath, g_fnPattern);
                DEBUG_LOGP("FileIo f_findfirst-> %d\r\n", res);
                if ((FR_OK == res) && (0 != g_fno.fname[0]))
                {
                    g_status = FIO_READY;
                }
            }
        }

        return FIO_READY == g_status;
    }

    bool FileIo::next()
    {
        if (false == g_enable)
        {
            DEBUG_LOGP("FileIo selectNextFile-> NE\r\n");
            return false;
        }

        if (FIO_READY != g_status)
        {
            DEBUG_LOGP("FileIo selectNextFile-> NR\r\n");
            return false;
        }

        FRESULT res(f_findnext(&g_dir, &g_fno));
        DEBUG_LOGP("FileIo f_findnext-> %d\r\n", res);

        if (FR_OK != res)
        {
            g_status = FIO_ERROR;
            return false;
        }

        if (0 == g_fno.fname[0])
        {
            /* reached end of entries, restart */
            f_closedir(&g_dir);

            res = f_findfirst(&g_dir, &g_fno, g_dirPath, g_fnPattern);
            DEBUG_LOGP("FileIo f_findfirst-> %d\r\n", res);
            if ((FR_OK == res) && (0 != g_fno.fname[0]))
            {
                g_status = FIO_READY;
            }
            else
            {
                g_status = FIO_ERROR;
                return false;
            }
        }

        return true;
    }

    const char * FileIo::getFileName()
    {
        return g_fno.fname;
    }

    bool FileIo::open(const char * fname)
    {
        if (FIO_READY == g_status)
        {
            FRESULT res(f_open(&g_fil, fname, FA_READ));
            DEBUG_LOGP("FileIo::f_open() -> %d\r\n", res);

            if (FR_OK == res)
            {
                g_status = FIO_OPEN;
            }
        }

        return FIO_OPEN == g_status;
    }

    bool FileIo::close()
    {
        if (FIO_OPEN == g_status)
        {
            FRESULT res(f_close(&g_fil));
            DEBUG_LOGP("FileIo::f_close() -> %d\r\n", res);
            /* expect data in epd directory */
            res = f_chdir(g_dirPath);
            DEBUG_LOGP("FileIo f_chdir-> %d\r\n", res);

            if (FR_OK == res)
            {
                g_status = FIO_READY;
            }
        }

        return FIO_READY == g_status;
    }

    bool FileIo::read(void * buf, uint16_t size, uint16_t& read)
    {
        if (FIO_OPEN == g_status)
        {
            UINT retRead;
            FRESULT res(f_read(&g_fil, buf, size, &retRead));

            if (FR_OK == res)
            {
                read = (uint16_t)retRead;
            }
            else
            {
                DEBUG_LOGP("FileIo::read() -> %d\r\n", res);
                read = 0u;
            }

            return FR_OK == res;
        }

        return false;
    }

    bool FileIo::enable()
    {
        if (false == g_enable)
        {
            DEBUG_LOGP("FileIo::enable()\r\n");

            DSTATUS status(disk_initialize(0));
            DEBUG_LOGP("FileIo disk_initialize-> %d\r\n", status);

            if (status & STA_NOINIT)
    	    {
                g_enable = false;
            }
            else
            {
                g_enable = true;
            }
        }

        return g_enable;
    }

    void FileIo::disable()
    {
        if (true == g_enable)
        {
            g_enable = false;
            DEBUG_LOGP("FileIo::disable()\r\n");
        }
    }

    bool FileIo::getVolumeSerialNumber(uint32_t& vsn)
    {
        char null (0u);
        char label[12+1];

        FRESULT res(f_getlabel(&null, label, &vsn));
        DEBUG_LOGP("FileIo::vsn(%04lx-%04lx) -> %02x\r\n", vsn>>16, (vsn&0xFFFF), res);

        return FR_OK == res;
    }

}