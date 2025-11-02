# test_stats.py - 
#
# Josh Meise
# 11-02-2025
# Description: 
#

import stats_lib as sl
import sys
import numpy as np

def main():
    # Check arguments.
    if len(sys.argv) != 2:
        print("usage: python3 test_stats.py <infile.txt>")
        sys.exit(1)
    
    infile = sys.argv[1]

    # Read into 2-D array.
    with open(infile, 'r') as ifile:
        data = ifile.read()

    # Split data up and remove last entry that is only newline.
    data = data.split('\n')[:-1]
    
    # Convert each hex string into a list of bytes
    data = np.array([list(bytes.fromhex(x)) for x in data], dtype=np.uint8)

    mean = sl.mean_per_byte(data, quiet = False)

    variance = sl.variance_per_byte(data, quiet = False)

    bit_entropy = sl.bit_entropy_per_byte(data, quiet = False)

    freqs = sl.byte_freqs_per_byte(data, quiet = False)
    
if __name__ == "__main__":
    main()
