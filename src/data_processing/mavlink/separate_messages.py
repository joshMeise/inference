# separate_messages.py - separates MAVLink messages by type (with headers)
#
# Josh Meise
# 10-31-2025
# Description: Input file should be full binary MAVLink dataset.
#              -h flag indicates that headers should be written to file too
#              absence of -h flag indicates only payloads should be written
#

import sys
import os

def main():
    if len(sys.argv) != 3 and len(sys.argv) != 4:
        print("usage: python3 separate_messages.py <infile.bin> <outdir> [-h]")
        sys.exit(1)

    if len(sys.argv) == 4 and sys.argv[3] != "-h":
        print("usage: python3 separate_messages.py <infile.bin> <outdir> [-h]")
        sys.exit(1)
    
    infile = sys.argv[1]
    outdir = sys.argv[2]
    if len(sys.argv) == 4: headers = True
    else: headers = False

    file_size = os.path.getsize(infile)

    ids_written = set()

    with open(infile, "rb") as ifile:
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

            msg_id_int = int.from_bytes(msg_id, byteorder = "little")
            fname = str(msg_id_int) + ".txt"
            fpath = outdir + '/' + fname
            
            if not headers: string = payload

            if msg_id_int in ids_written:
                with open(fpath, 'a') as ofile:
                    ofile.write(string.hex() + '\n')
            else:
                # Start writing fresh.
                with open(fpath, 'w') as ofile:
                    ofile.write(string.hex() + '\n')

                ids_written.add(msg_id_int)

if __name__ == "__main__":
    main()
