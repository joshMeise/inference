# pad_mesages.py - pads variable-length messages with 0s so that they are all uniform lenght
#
# Josh Meise
# 10-31-2025
# Description: Takes in a binary file, writes to an ASCII file.
#
# TODO:
# - Check message types with arrays/strings.
#

import sys
import os

# Maps message IDs to maximum lengths for MAVLink 2 messages.
LEN_MAP = {
    0: 9,
    1: 43,
    2: 12,
    4: 14,
    8: 36,
    11: 6,
    21: 2,
    22: 25,       # contains string value
    23: 23,       # contains string value
    24: 52,
    29: 16,
    30: 28,
    31: 48,       # contains array
    32: 28,
    33: 28,
    42: 18,
    43: 3,
    44: 9,
    45: 3,
    46: 2,
    47: 8,
    51: 5,
    69: 30,
    73: 38,
    74: 20,
    76: 33,
    77: 10,
    83: 37,       # contains array
    85: 51,
    87: 51,
    109: 9,
    132: 39,      # contains array
    141: 32,
    147: 54,      # contains array
    148: 78,      # contains array
    230: 42,
    241: 32,
    242: 60,      # contains array
    245: 2,
    253: 54,      # constains string
    300: 22,      # contains array
    340: 70,      # contains array
    380: 20,
    397: 108,     # contains string
    410: 53,      # contains array
    411: 3,
    12901: 59,    # contains array
    12904: 54,    # contains array
}

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
            
            # MAVLink 2 messages may be truncated.
            if code == b'\xfd':
                payload_len = LEN_MAP[msg_id_int]
            
                if int.from_bytes(length) < payload_len:
                    payload += b'\x00'*(payload_len - int.from_bytes(length))
                    length = payload_len.to_bytes()
                elif int.from_bytes(length) > payload_len:
                    print(f"Length greater than max payload length on message id {msg_id_int}.")

            write_string = code + length
            if code == b'\xfd': write_string += (inc_flags + cmp_flags)
            write_string += (seq + sys_id + comp_id + msg_id + payload + checksum)

            ofile.write(write_string.hex() + '\n') 

if __name__ == "__main__":
    main()
