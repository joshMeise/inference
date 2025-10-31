# bin_to_ascii.py - converts MAVLink data from binary format to ASCII format
#
# Josh Meise
# 10-31-2025
# Description: 
#

import sys
import os

def main():
    if len(sys.argv) != 3:
        print("usage: python3 bin_to_ascii.py <infile.bin> <outfile.txt>")
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

            #MAVLink 2 ahs 3-byte message ID, MAVLink 1 has 1 byte
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
            ofile.write(string.hex() + '\n') 

if __name__ == "__main__":
    main()
