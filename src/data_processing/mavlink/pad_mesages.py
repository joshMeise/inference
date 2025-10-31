# pad_mesages.py - pads variable-length messages with 0s so that they are all uniform lenght
#
# Josh Meise
# 10-31-2025
# Description: Takes in a binary file, writes to an ASCII file.
#

import sys
import os
from pymavlink.dialects.v20 import common as mavlink2_common

# Message IDs not handled by pymavlink.
UNHANDLED_IDS = [8, 300, 380, 397, 410, 411]

LEN_MAP = {
    "8": 53,
    "300": 22,
    "380": 10,
    "397": 108,
    "410": 53,
    "411": 3,
}

TYPE_SIZES = {
    "char": 1,
    "int8_t": 1,
    "uint8_t": 1,
    "int16_t": 2,
    "uint16_t": 2,
    "int32_t": 4,
    "uint32_t": 4,
    "float": 4,
    "int64_t": 8,
    "uint64_t": 8,
    "double": 8,
}

def payload_len_from_field_types(field_types):
    return sum(TYPE_SIZES[f] for f in field_types if f in TYPE_SIZES)

def main():
    if len(sys.argv) != 3:
        print("usage: python3 pad_messages.py <infile.bin> <outfile.txt>")
        sys.exit(1)

    infile = sys.argv[1]
    outfile = sys.argv[2]
    
    file_size = os.path.getsize(infile)

    with open(infile, "rb") as ifile, open(outfile, 'w') as ofile :
        bytes_processed = 0
        
        while bytes_processed < file_size:
            code = ifile.read(1)
            length = ifile.read(1)
            string = code + length

            # MAVLink 2 read in flags.
            if code == b'\xfd':
                inc_flags = ifile.read(1)
                cmp_flags = ifile.read(1)
                string += (inc_flags + cmp_flags)
            
            seq = ifile.read(1)
            sys_id = ifile.read(1)
            comp_id = ifile.read(1)

            # MAVLink 2 has 3-byte message ID, MAVLink 1 has 1 byte
            if code == b'\xfd':
                msg_id = ifile.read(3)
            elif code == b'\xfe':
                msg_id = ifile.read(1)

            payload = ifile.read(int.from_bytes(length))
            checksum = ifile.read(2)
            string += (seq + sys_id + comp_id + msg_id + payload + checksum)

            # If signed message.
            if code == b'\xfd' and inc_flags == b'\x01':
                signature = ifile.read(13)
                string += signature

            bytes_processed += len(string)
            
            msg_id_int = int.from_bytes(msg_id, byteorder = "little")
            
            if msg_id_int in UNHANDLED_IDS:
                payload_len = LEN_MAP[str(msg_id_int)]
            else:
                msg_class = mavlink2_common.mavlink_map[msg_id_int]
                payload_len = payload_len_from_field_types(msg_class.fieldtypes)
            
            if int.from_bytes(length) < payload_len:
                payload += b'\x00'*(payload_len - int.from_bytes(length))
                length = payload_len.to_bytes()

            write_string = code + length
            if code == b'\xfd': write_string += (inc_flags + cmp_flags)
            write_string += (seq + sys_id + comp_id + msg_id + payload + checksum)

            ofile.write(write_string.hex() + '\n') 

if __name__ == "__main__":
    main()
