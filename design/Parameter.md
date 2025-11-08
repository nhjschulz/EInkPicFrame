# Parameter

EPD supports a parameter file to influence its behavior.
The parameter file is optional. Internal defaults are used
if no parameter block is present in the file system.

## Parameter File

The software expects the parameter file at the following location:

    /epd/epd.cfg

The file is a binary file in the format defined below.
A binary file method was chosen to unburden the microcontroller from
text parsing needs. EPD also offers no user interface for editing
parameter. Producing the parameter set on a PC and store it in the file
system is therefore the easiest solution.

## Paramter File Layout

The contents of the parameter file is a short header, followed by a
sequence of 16 bit values (the parameters) in little endian byte
order (same as on a PC).

### Parameter Header Layout

The parameter header is a sequence of 6 bytes defined below:

             0x00         0x01
          -------------------------
    0x00  |    'E'    |    'P'    |    "EPD" Prefix
    0x02  |    'D'    | <version> |    Parameter record Layout Version (==1)
    0x04  |   <len>   |   <crc>   |    # of Parameter, CRC8 over Parameters
          -------------------------

The used 8-Bit Crc is CRC-8-CCITT with init value 0x00 as defined here:
[https://www.nongnu.org/avr-libc/user-manual/group__util__crc.html](https://www.nongnu.org/avr-libc/user-manual/group__util__crc.html)

### Parmeter Layout (Version 1)

The following table shows the supported parameters:

|Offset| Parameter  |   Definiton           | Unit | Default       |  Range |
|------|------------|-----------------------|---------|----------------|-----|
| 0x06 | Interval   | Update Interval       | Minutes | 1440 (1 Day) | 1-65535 |
| 0x08 | MinVoltage | Minimum supply voltage before entering LowBattery mode | MilliVolt | 3300mV | 33000-5000 |
| 0x0A | RefVoltage | Calibrated internal ADC reference voltage.| MilliVolt | 1100mv (BandGap on AVR) | 1000-5000 |
| 0x0C | SupVoltage | Supply voltage on AVCC pin. Only used for RefVoltage calibration| MilliVolt | 5000 | 3300-5000 |


## Creating the Parameter File with epdcfg.py

A Python base tool for generating the parameter binary file based
on JSON is available in the parameter folder.
It generates the parameter data based on the following JSON record:

    {
    "Header" :
        {
            "Version" : 1
        },
    "Parameter" :
        {
            "Interval"   : 1440,
            "MinVoltage" : 3300,
            "RefVoltage" : 1100,
            "SupVoltage" : 5001
        }
    }

Call the tool es follows:

    $ python epdcfg.py param.json
    Reading data from .\param.json

    Interval   : 1440 minutes
    MinVoltage : 3300 mV
    refVoltage : 1100 mV
    supVoltage : 5001 mV
    crc        : 0x42

    Storing configuration into epd.cfg

This process is a little cumbersome. If there is space left for
parsing at the end, I'll probably replace it with a ASCII parser
and a text file approach.
