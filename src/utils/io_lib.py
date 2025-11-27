# io_lib.py - 
#
# Josh Meise
# 11-02-2025
# Description: 
#

import numpy as np

'''
Reads data from text file of hex strings into 2-D array.

Inputs: 
    - Filename.

Outputs:
    - 2-D array with rows being messages and columns being byte positions
'''
def read_file(fname):
    # Read into 2-D array.
    with open(fname, 'r') as ifile:
        data = ifile.read()

    # Split data up and remove last entry that is only newline.
    data = data.split('\n')[:-1]
    
    # Convert each hex string into a list of bytes
    data = np.array([list(bytes.fromhex(x)) for x in data], dtype = np.uint8)

    return data

