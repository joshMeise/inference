# mavlink_bin_to_ascii.py - converts MAVLink data from binary format to ASCII format
#
# Josh Meise
# 10-31-2025
# Description: 
#

import sys

def main():
    if len(sys.argv) != 3:
        print("usage: python3 mavlink_bin_to_ascii.py <infile.bin> <outfile.txt>")
        sys.exit(1)

    infile = sys.argv[1]
    outfile = sys.argv[2]

    with open(infile, 'rb') as ifile:
        data = file.read()

    print(data)

if __name__ == "__main__":
    main()
