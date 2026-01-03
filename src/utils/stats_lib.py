# stats_lib.py - 
#
# Josh Meise
# 11-01-2025
# Description: 
#

import numpy as np
from scipy.stats import entropy
import struct

'''
Computes the mean across each byte position in a dataset.

Inputs:
    - Two-dimensional array of data, rows are messages, columns are values in each message
    - Option to print out values.

Outputs:
    - One-dimensional array of means across each byte position.
'''
def mean_per_byte(data, quiet = True):
    # Compute column-wise mean.
    mean = np.mean(data, axis = 0)
    
    if not quiet:
        print(f"Byte     | Mean")
        print("-----------------------")
        for i, m in enumerate(mean):
            print(f"{i:<8} | {m:12.6e}")
        print('\n')

    return mean

'''
Computes the variance across each byte position in a dataset.

Inputs:
    - Two-dimensional array of data, rows are messages, columns are values in each message
    - Option to print out values.

Outputs:
    - One-dimensional array of variances across each byte position.
'''
def variance_per_byte(data, quiet = True):
    # Compute column-wise variance.
    variance =  np.var(data, axis = 0)

    if not quiet:
        print(f"Byte     | Variance")
        print("-----------------------")
        for i, v in enumerate(variance):
            print(f"{i:<8} | {v:12.6e}")
        print('\n')

    return variance

'''
Computes the entropy (in bits) across each byte position in a dataset.

Inputs:
    - Two-dimensional array of data, rows are messages, columns are values in each message
    - Option to print out values.

Outputs:
    - One-dimensional array of entropies across each byte position.
'''
def bit_entropy_per_byte(data, quiet = True):
    # Find number of bytes in message to create entropies array.
    num_messages, num_bytes = data.shape
    entropies = np.zeros(num_bytes)

    for i in range(num_bytes):
        # Find how many times each byte occurs across the messages.
        _, counts = np.unique(data[:, i], return_counts = True)

        # Find the probability that each byte occurs.
        probs = counts/num_messages

        # Calculate entropy.
        entropies[i] = entropy(probs, base = 2)
        
    if not quiet:
        print(f"Byte     | Entropy")
        print("-----------------------")
        for i, e in enumerate(entropies):
            print(f"{i:<8} | {e:12.6e}")
        print('\n')

    return entropies

'''
Computes the frequency of byte values across each byte position in a dataset.

Inputs:
    - Two-dimensional array of data, rows are messages, columns are values in each message.
    - Option to print out values.

Outputs:
    - List of dictionaries mapping byte values to number of occurances.
'''
def byte_freqs_per_byte(data, quiet = True):
    # Find number of bytes in message to create list.
    _, num_bytes = data.shape
    freqs = [{}]*num_bytes
    
    for i in range(num_bytes):
        # Find how many times each byte occurs.
        vals, counts = np.unique(data[:, i], return_counts = True)

        # Create map of byte value to count.
        val_count_dict = dict(zip(vals, counts))

        freqs[i] = val_count_dict
    
    if not quiet:
        print("Byte frequencies:")
        for i in range(num_bytes):
            print("-----------------------")
            print(f"Byte {i}:")
            print("Value    | Frequency   ")
            print("-----------------------")
            for val in freqs[i]:
                print(f"0x{val:>02x}     | 0x{freqs[i][val]}")
            print("-----------------------")
        print('\n')

    return freqs

'''
Finds the minumum and maximum byte values for each byte position in the message.

Inputs:
    - Two-dimensional array of data, rows are messages, columns are values in each message.
    - Option to print out values.

Outputs:
    - Two-dimensional array of minima and maxima for each byte value.
'''
def min_max_per_byte(data, quiet = True):
    # Find minima and maxima.
    mins = np.min(data, axis = 0)
    maxs = np.max(data, axis = 0)

    if not quiet:
        print(f"Byte     | Minimum | Maximum ")
        print("-----------------------------")
        for i in range(len(mins)):
            print(f"{i:<8} | 0x{maxs[i]:>02x}    | 0x{maxs[i]:>02x}")
        print('\n')

    return np.stack((mins, maxs))









### BYTE-WWISE CALCULATIONS ####
'''
Computes the mean of a value specified by a range of bytes.

Inputs:
    - Two-dimensional array of data, rows are messages, columns are various byte values per message
    - Option to consider it as little endian representation
    - Option to specify whether data is signed
    - Data type (integer or float)
    - Option to print out values

Outputs:
    - Mean value for the bytes specified.
'''
def mean_per_bytes(data, little_endian = False, signed = False, dtype = "integer", quiet = True):
    # Find the value specified by each row.
    if dtype == "integer":
        if little_endian:
            vals = [int.from_bytes(row.tobytes(), byteorder = "little", signed = signed) for row in data]
        else:
            vals = [int.from_bytes(row.tobytes(), byteorder = "big", signed = signed) for row in data]
    elif dtype == "float":
        if little_endian:
            vals = [struct.unpack("<f", row) for row in data]
        else:
            vals = [struct.unpack(">f", row) for row in data]
    else:
        print("Invalid data type.")
        return

    # Compute mean.
    mean = np.mean(vals)
    
    if not quiet:
        print(f"Mean")
        print("------------")
        print(f"{mean:.6e}")
        print('\n')

    return mean

'''
Computes the variance of a value specified by a range of bytes.

Inputs:
    - Two-dimensional array of data, rows are messages, columns are various byte values per message
    - Option to consider it as little endian representation
    - Option to specify whether data is signed
    - Data type (integer or float)
    - Option to print out values.

Outputs:
    - Variance for the bytes specified.
'''
def variance_per_bytes(data, little_endian = False, signed = False, dtype = "integer", quiet = True):
    # Find the value specified by each row.
    if dtype == "integer":
        if little_endian:
            vals = [int.from_bytes(row.tobytes(), byteorder = "little", signed = signed) for row in data]
        else:
            vals = [int.from_bytes(row.tobytes(), byteorder = "big", signed = signed) for row in data]
    elif dtype == "float":
        if little_endian:
            vals = [struct.unpack("<f", row) for row in data]
        else:
            vals = [struct.unpack(">f", row) for row in data]
    else:
        print("Invalid data type.")
        return

    # Compute variance.
    variance =  np.var(vals)

    if not quiet:
        print(f"Variance")
        print("------------")
        print(f"{variance:.6e}")
        print('\n')

    return variance

'''
Computes the entropy (in bits) for a given value in the dataset.

Inputs:
    - Two-dimensional array of data, rows are messages, columns are variouts byte values per message
    - Option to consider it as little-endian representation
    - Option to specify whether the data is signed
    - Data type (integer or float)
    - Option to print out values

Outputs:
    - Entropy for the bytes specified
'''
def bit_entropy_per_bytes(data, little_endian = False, signed = False, dtype = "integer", quiet = True):
    # Find the value specified by each row.
    if dtype == "integer":
        if little_endian:
            vals = [int.from_bytes(row.tobytes(), byteorder = "little", signed = signed) for row in data]
        else:
            vals = [int.from_bytes(row.tobytes(), byteorder = "big", signed = signed) for row in data]
    elif dtype == "float":
        if little_endian:
            vals = [struct.unpack("<f", row) for row in data]
        else:
            vals = [struct.unpack(">f", row) for row in data]
    else:
        print("Invalid data type.")
        return

    # Find how many times each value occurs.
    _, counts = np.unique(vals, return_counts = True)
        
    # Find the probability that each byte occurs.
    probs = counts/len(vals)

    # Calculate entropy.
    ent = entropy(probs, base = 2)
        
    if not quiet:
        print(f"Entropy")
        print("------------")
        print(f"{ent:12.6e}")
        print('\n')

    return ent


