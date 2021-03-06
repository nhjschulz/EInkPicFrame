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
#include "hal/Gpio/Gpio.h"
#include "hal/Spi/Spi.h"
#include "hal/Cpu/Cpu.h"
#include "service/Display/Display.h"
#include "service/Debug/Debug.h"

namespace service 
{
    /** Slave select function for SPI when talking to display
     * @see hal::SPi::configure
     */
    static void dispSlaveSelect(bool select)
    {
        if (select)
        {
            hal::Gpio::clrDispCS();
        }
        else
        {
            hal::Gpio::setDispCS();
        }
    }

    /* Command Sequences taken from documents here: 
     * https://www.waveshare.com/wiki/5.65inch_e-Paper_Module_(F)
     */

    /** R00H (PSR): Panel setting Register
     * UD(1): scan up
     * SHL(1) shift right
     * SHD_N(1) DC-DC on
     * RST_N(1) no reset
     */
    static const uint8_t R00_cmdPSR[] PROGMEM = { 0x00, 0xEF, 0x08 };

    /** R01H (PWR): Power setting Register
     * internal DC-DC power generation
     */
    static const uint8_t R01_cmdPWR[] PROGMEM = { 0x01, 0x07, 0x00, 0x00, 0x00 };


    /** R02H (POF): Power OFF Command
     */
    static const uint8_t R02_cmdPOF[] PROGMEM = { 0x02 };

    /** R03H (PFS): Power off sequence setting Register
     * T_VDS_OFF (00) = 1 frame
     */
    static const uint8_t R03_cmdPFS[] PROGMEM = { 0x03, 0x00 };

    /** R04H (PON): Power ON Command
     */
    static const uint8_t R04_cmdPON[] PROGMEM = { 0x04 };

    /** R06h (BTST): Booster Soft Start
     */
    static const uint8_t R06_cmdBTST[] PROGMEM = { 0x06, 0xC7, 0xC7, 0x1D };

    /** 
     * R07H (DSLP): Deep sleep#
     * Note Documentation @  Waveshare shows cmd code as 0x10 in table, but
     * 0x10 is DTM1.
     */
    static const uint8_t R07_cmdDSLP[] PROGMEM = { 0x07, 0xA5 };

    /** R10H (DTM1): Data Start Transmission 1
     */
    static const uint8_t R10_cmdDTM1[] PROGMEM = { 0x10 };

    /** R11H (DSP): Data Stop
     */
    static const uint8_t R11_cmdDSP[] PROGMEM = { 0x11 };

    /** R12H (DRF): Display Refresh
     */
    static const uint8_t R12_cmdDRF[] PROGMEM = { 0x12 };

    /** R13H (IPC): Image Process Command
     */
    static const uint8_t R13_cmdIPC[] PROGMEM = { 0x13, 0x00 };


    /** R30H (PLL): PLL Control
     * 0x3C = 50Hz
     */
    static const uint8_t R30_cmdPLL[] PROGMEM = { 0x30, 0x3C };

    /** R41H (TSE): Temperature Sensor Enable
     * TSE(0) enable, TO(0000) +0 degree offset
     */
    static const uint8_t R41_cmdTSE[] PROGMEM = { 0x41, 0x00 };

    /** R50H (CDI) VCOM and Data interval setting
     *  CDI(0111) 10
     *  DDX(1), VBD(001) Border output "White"
     */
    static const uint8_t R50_cmdCDI[] PROGMEM = { 0x50, 0x37 };

    /** R60H (TCON) Gate and Source non overlap period command
     *  S2G(10) 12 units
     *  G2S(10) 12 units
     */
    static const uint8_t R60_cmdTCON[] PROGMEM = { 0x60, 0x22 };

    /** R61H (TRES) Resolution Setting
     * 0x258 = 600
     * 0x1C0 = 448
     */
    static const uint8_t R61_cmdTRES[] PROGMEM = { 0x61, 0x02, 0x58, 0x01, 0xC0 };

    /** RE3H (PWS) Power Savings
     */
    static const uint8_t RE3_cmdPWS[] PROGMEM = { 0xE3, 0xAA };

    bool Epd::init(void) 
    { 
        bool result(false);

        configureSpi();

        reset();

        if (waitForIdle(100u)) 
        {
            sendCmd_P(R00_cmdPSR, sizeof(R00_cmdPSR));
            sendCmd_P(R01_cmdPWR, sizeof(R01_cmdPWR));
            sendCmd_P(R03_cmdPFS, sizeof(R03_cmdPFS));
            sendCmd_P(R06_cmdBTST, sizeof(R06_cmdBTST));
            sendCmd_P(R30_cmdPLL, sizeof(R30_cmdPLL));
            sendCmd_P(R41_cmdTSE,sizeof(R41_cmdTSE));
            sendCmd_P(R50_cmdCDI, sizeof(R50_cmdCDI));
            sendCmd_P(R60_cmdTCON, sizeof(R60_cmdTCON));
            sendCmd_P(R61_cmdTRES, sizeof(R61_cmdTRES));
    	    sendCmd_P(RE3_cmdPWS, sizeof(RE3_cmdPWS));

            hal::Cpu::enterIdle(10);
            sendCmd_P(R50_cmdCDI, sizeof(R50_cmdCDI));

            result = true;
        }

        return result;
    }


    void Epd::sendCmd_P(const uint8_t * cmd, uint8_t size)
    {
        /* first byte is command */
        hal::Gpio::clrDispDC();
        hal::Spi::write_P(cmd, 1u);

        /* cmd data bytes */
        if (1u < size)
        {
            hal::Gpio::setDispDC();  /* data bytes */
            ++cmd; 
            --size;
            hal::Spi::write_P(cmd, size);
        }
    }

    bool Epd::waitForIdle(uint8_t tmo_ms)
    {
        tmo_ms /= hal::Cpu::getIdleTickTime_ms();

        do
        {
            hal::Cpu::enterIdle(1u);
            if (0u < tmo_ms)
            {
                --tmo_ms;
            }

            /* If BUSYN=0 then waiting */
        } while((0u < tmo_ms) && (false == hal::Gpio::getDispBusy())); 

        return true == hal::Gpio::getDispBusy();
    }

    bool Epd::waitForBusy(uint8_t tmo_ms)
    {
        tmo_ms /= hal::Cpu::getIdleTickTime_ms();

        do 
        {
            hal::Cpu::enterIdle(1u);
        if (0u < tmo_ms)
            {
                --tmo_ms;
            }
            /* If BUSYN=1 then waiting */
        }
        while((0u < tmo_ms) && (true == hal::Gpio::getDispBusy()));

        return false == hal::Gpio::getDispBusy();
    }

    void Epd::configureSpi()
    {
        hal::Spi::configure(
            hal::Spi::CLK_2000000,
            hal::Spi::MODE_0, 
            hal::Spi::BITORDER_MSB, dispSlaveSelect);
    } 

    void Epd::beginPaint()
    {
        configureSpi();

        sendCmd_P(R61_cmdTRES, sizeof(R61_cmdTRES));
        sendCmd_P(R10_cmdDTM1, sizeof(R10_cmdDTM1));
        hal::Gpio::setDispDC();
    }

    void Epd::sendBlock(const uint8_t * block, uint8_t size)
    {
        configureSpi();

        hal::Spi::write(block, size);
    }

    void Epd::endPaint()
    {
        configureSpi();

        sendCmd_P(R04_cmdPON, sizeof(R04_cmdPON)); // power on
        while(!waitForIdle(100u))
        {

        }

        sendCmd_P(R12_cmdDRF, sizeof(R12_cmdDRF)); // refresh
        while(!waitForIdle(255u))
        {

        }

        sendCmd_P(R02_cmdPOF, sizeof(R02_cmdPOF));  // power off
        while(!waitForBusy(100u))
        {

        }
    }

    void Epd::reset(void)
    {
        configureSpi();

        hal::Gpio::clrDispReset();                /* low = module reset  */
        hal::Cpu::delayMS(1);

        hal::Gpio::setDispReset();
        hal::Cpu::enterIdle(20);    
    }

    void Epd::clear(Epd::Color color)
    {
        configureSpi();

        const uint8_t twoPixel((color<<4)|color);

        uint16_t width(getWidth() >> 1u);
        uint16_t height(getHeight());

        /* There is no clear command, set every pixel to given color.
         */
        beginPaint();

        for(uint16_t i=0; i < width; i++) {
            for(uint16_t j=0; j < height; j++) {
                sendBlock(&twoPixel, 1);
    		}
    	}

        endPaint();
    }

    void Epd::sleep(void)
    {
        configureSpi();
        
        sendCmd_P(R07_cmdDSLP, sizeof(R07_cmdDSLP));

        hal::Cpu::enterIdle(10);
    }
}
