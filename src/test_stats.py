# test_stats.py - 
#
# Josh Meise
# 11-02-2025
# Description: 
#

from utils import stats_lib as sl
from utils import io_lib as iol
import sys
import numpy as np

def main():
    # Check arguments.
    if len(sys.argv) != 2:
        print("usage: python3 test_stats.py <infile.txt>")
        sys.exit(1)
    
    infile = sys.argv[1]

    # Read data from file.
    data = iol.read_file(infile)

    mean = sl.mean_per_byte(data, quiet = False)

    variance = sl.variance_per_byte(data, quiet = False)

    bit_entropy = sl.bit_entropy_per_byte(data, quiet = False)

    freqs = sl.byte_freqs_per_byte(data, quiet = False)
    
    mins_maxs = sl.min_max_per_byte(data, quiet = False)

    # Extrct the first 4 columns of data.
    first_4_cols = [row[4:8] for row in data]

    mean = sl.mean_per_bytes(first_4_cols, little_endian = True, quiet = False, dtype = "float")

    variance = sl.variance_per_bytes(first_4_cols, little_endian = True, quiet = False, dtype = "integer")

    entropy = sl.bit_entropy_per_bytes(first_4_cols, little_endian = True, quiet = False, dtype = "integer")


if __name__ == "__main__":
    main()
