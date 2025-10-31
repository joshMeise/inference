# separate_messages.py - separates MAVLink messages by type (with headers)
#
# Josh Meise
# 10-31-2025
# Description: -h flag indicates that headers should be written to file too
#              absence of -h flag indicates only payloads should be written
#              -b flag indicates that input file is a binary file
#

import sys
import os

def main():
    if len(sys.argv) not in (3, 4, 5):
        print("usage: python3 separate_messages.py <infile.bin> <outdir> [-h] [-b]")
        sys.exit(1)

    if len(sys.argv) == 4 and sys.argv[3] not in ("-h", "-b"):
        print("usage: python3 separate_messages.py <infile.bin> <outdir> [-h] [-b]")
        sys.exit(1)

    if len(sys.argv) == 5:
        flags = sys.argv[3:]
        if not(("-h" in flags) and ("-b" in flags)):
            print("usage: python3 separate_messages.py <infile.bin> <outdir> [-h] [-b]")
            sys.exit(1)

    infile = sys.argv[1]
    outdir = sys.argv[2]
    
    headers = False
    binary = False
    
    if len(sys.argv) > 3:
        headers = "-h" in sys.argv[3:]
        binary = "-b" in sys.argv[3:]

    file_size = os.path.getsize(infile)

    ids_written = set()

    if binary: 
        read_type = "rb"
        mult = 1
    else: 
        read_type = "r"
        mult = 2

    with open(infile, read_type) as ifile:
        bytes_processed = 0
                
        while bytes_processed < file_size:
            code = ifile.read(1*mult)
            length = ifile.read(1*mult)
            string = code + length

            if not binary: 
                byte_code = bytes.fromhex(code)
                byte_length = bytes.fromhex(length)
            else: 
                byte_code = code
                byte_length = length

            # MAVLink 2 has flags.
            if byte_code == b'\xfd':
                inc_flags = ifile.read(1*mult)
                cmp_flags = ifile.read(1*mult)
                string += (inc_flags + cmp_flags)
                
                if not binary: byte_inc_flags = bytes.fromhex(inc_flags)
                else: byte_inc_flags = inc_flags

            seq = ifile.read(1*mult)
            sys_id = ifile.read(1*mult)
            comp_id = ifile.read(1*mult)

            # MAVLink 2 has 3-byte message ID, MAVLink 1 has 1 byte
            if byte_code == b'\xfd': msg_id = ifile.read(3*mult)
            elif byte_code == b'\xfe': msg_id = ifile.read(1*mult)

            payload = ifile.read(int.from_bytes(byte_length)*mult)
            checksum = ifile.read(2*mult)
            string += (seq + sys_id + comp_id + msg_id + payload + checksum)

            # If signed message.
            if byte_code == b'\xfd' and byte_inc_flags == b'\x01':
                signature = ifile.read(13*mult)
                string += signature
            
            bytes_processed += len(string)

            # Read newline character if ASCII.
            if not binary:
                ifile.read(1)
                bytes_processed += 1

            if binary: msg_id_int = int.from_bytes(msg_id, byteorder = "little")
            else: msg_id_int = int.from_bytes(bytes.fromhex(msg_id), byteorder = "little")

            fname = str(msg_id_int) + ".txt"
            fpath = outdir + '/' + fname
            
            if not headers: string = payload
            
            if binary: string = string.hex()
            
            if msg_id_int in ids_written:
                with open(fpath, 'a') as ofile:
                    ofile.write(string + '\n')
            else:
                # Start writing fiel from scratch.
                with open(fpath, 'w') as ofile:
                    ofile.write(string + '\n')

                ids_written.add(msg_id_int)

if __name__ == "__main__":
    main()
