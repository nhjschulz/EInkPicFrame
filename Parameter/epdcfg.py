import json
import os
import sys
from crc import Crc8, TableBasedCrcRegister, CrcRegister

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print('usage {}: <json file> [output]'.format(sys.argv[0]))
        exit(1)

    print('Reading data from {}\n'.format(sys.argv[1]))

    input = open(sys.argv[1], "r")
    json_data = json.load(input)

    output_file = "epd.cfg"
    if  3 == len(sys.argv) :
        output_file = sys.argv[2]

    header_bytes = bytearray()
    param_bytes = bytearray()

    # build parameter bytes
    interval = json_data['Parameter']['Interval']
    minVoltage = json_data['Parameter']['MinVoltage']
    refVoltage = json_data['Parameter']['RefVoltage']
    supVoltage = json_data['Parameter']['SupVoltage']

    param_bytes += interval.to_bytes(2, 'little')
    param_bytes += minVoltage.to_bytes(2, 'little')
    param_bytes += refVoltage.to_bytes(2, 'little')
    param_bytes += supVoltage.to_bytes(2, 'little')

    print('Interval   : {} minutes'.format(interval))
    print('MinVoltage : {} mV'.format(minVoltage))
    print('refVoltage : {} mV'.format(refVoltage))
    print('supVoltage : {} mV'.format(supVoltage))

    # build header bytes
    crc8 = CrcRegister(Crc8.CCITT)
    crc8.init()
    crc8.update(param_bytes)

    header_bytes.append(69)   # E
    header_bytes.append(80)   # P
    header_bytes.append(68)   # D
    header_bytes.append(1)    # version 1
    header_bytes.append(4)    # 4 parameter
    header_bytes.append(crc8.digest())  #  CRC

    print('crc        : 0x{:02x}'.format(crc8.digest()))

    print('\r\nStoring configuration into {}'.format(output_file))
    output = open(output_file, "wb")

    output.write(header_bytes)
    output.write(param_bytes)
    output.close()

