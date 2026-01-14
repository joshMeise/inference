/*
 * count_unique_types.cpp - 
 *
 * Josh Meise
 * 01-13-2026
 * Description: 
 *
 */

#include <mavlink.h>
#include <string>
#include <fstream>
#include <unordered_set>
#include <vector>
#include <cstdint>
#include <iostream>

int main(int argc, char **argv) {
    std::string iname;
    std::ifstream ifile;
    std::unordered_set<int> ids;
    int bytes_consumed, id, num_ids, num_msgs;
    std::vector<uint8_t> bytes;
    std::uintmax_t fs;
    std::filesystem::path p;
    MAVLinkMessage message;

    // Check arguments.
    if (argc < 2) {
        std::cerr << "usage: ./extract <infile.bin>\n";
        return 1;
    }

    iname = std::string(argv[1]);

    // Check tha file exists.
    if (!std::filesystem::exists(iname)) {
        std::cerr << "File does not exist.\n";
        return 1;
    }

    // Open file and read in messages.
    ifile.open(iname, std::ios::binary);

    if (!ifile.is_open()) {
        std::cerr << "Failed to open file.\n";
        return 1;
    }

    // Get number of bytes in file.
    p = std::filesystem::path(iname);
    fs = std::filesystem::file_size(p);

    // Create memory for given number of bytes.
    bytes.resize(fs);

    // Read in given number of bytes.
    ifile.read(reinterpret_cast<char*>(bytes.data()), fs);

    ifile.close();
    
    num_ids = 0;
    num_msgs = 0;

    // Convert bytes into messages.
    bytes_consumed = 0;
    while (bytes_consumed < bytes.size()) {
        // Create message object.
        message = MAVLinkMessage(bytes, bytes_consumed);

        // Remove bytes from byte vector after conversion to message.
        bytes_consumed += message.get_len();

        id = message.get_msg_id();

        if (!ids.contains(id)) {
            num_ids++;
            ids.insert(id);
        }
    
        num_msgs++;
    }

    std::cout << num_ids << " present in data capture\n";
    std::cout << num_msgs << " total messages present\n";
    
    return 0;

}

