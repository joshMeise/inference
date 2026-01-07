/*
 * main.cpp - 
 *
 * Josh Meise
 * 01-06-2026
 * Description: 
 *
 */

#include <iostream>
#include <grammar.h>
#include <fstream>
#include <vector>
#include <cstdint>

#define N1 44
#define N2 21
#define N3 40
#define N4 21

int main(void) {
    Grammar grammar;
    std::ifstream ifile;
    std::vector<uint8_t> bytes_1(N1), bytes_2(N2), bytes_3(N3), bytes_4(N4);

    // Open file for binary reading.
    ifile.open("../files/raw_dataset.bin", std::ios::binary);
    
    // Read in specified number of bytes.
    ifile.read(reinterpret_cast<char*>(bytes_1.data()), N1);

    grammar.add_message(bytes_1);

    // Read in specified number of bytes.
    ifile.read(reinterpret_cast<char*>(bytes_2.data()), N2);

    grammar.add_message(bytes_2);

    // Read in specified number of bytes.
    ifile.read(reinterpret_cast<char*>(bytes_3.data()), N3);

    grammar.add_message(bytes_3);

    // Read in specified number of bytes.
    ifile.read(reinterpret_cast<char*>(bytes_4.data()), N4);

    grammar.add_message(bytes_4);

    ifile.close();

    std::cout << grammar;

    return 0;
}
