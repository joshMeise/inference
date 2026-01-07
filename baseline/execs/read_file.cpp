/*
 * read_file.cpp - 
 *
 * Josh Meise
 * 01-07-2026
 * Description: 
 *
 */

#include <iostream>
#include <grammar.h>
#include <mavlink.h>
#include <fstream>
#include <vector>
#include <cstdint>

int main(void) {
    Grammar grammar;
    std::ifstream ifile;
    std::vector<uint8_t> bytes;
    std::filesystem::path p;
    std::string fname;
    std::uintmax_t fs;
    std::vector<MAVLinkMessage> messages;
    MAVLinkMessage message;
    
    fname = "../files/raw_dataset.bin";

    // Get number of bytes in file.
    p = std::filesystem::path(fname);
    fs = std::filesystem::file_size(p);

    // Open file for binary reading.
    ifile.open("../files/raw_dataset.bin", std::ios::binary);

    // Create memory for given number of bytes.
    bytes.resize(fs);

    // Read in given number of bytes.
    ifile.read(reinterpret_cast<char*>(bytes.data()), fs);

    // Convert bytes into messages.
    while (bytes.size() > 0) {
        // Create message object.
        message = MAVLinkMessage(bytes);

        // Remove bytes from byte vector after conversion to message.
        bytes.erase(bytes.begin(), bytes.begin() + message.get_len());

        messages.push_back(message);
    }

    // Make grammar.
    for (const auto& message : messages) {
        grammar.add_message(message.serialize());
        std::cout << "Number of terminals/non-terminals = " << grammar.get_num_terminals() + grammar.get_num_non_terminals() << std::endl;
    }
        
    ifile.close();

    return 0;
}
