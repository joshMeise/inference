/*
 * extract.cpp - 
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
#include <regex>
#include <cstdint>
#include <iostream>

int main(int argc, char **argv) {
    std::string iname, oname;
    std::ifstream ifile;
    std::ofstream ofile;
    std::unordered_set<int> ids;
    std::regex pattern;
    int i, bytes_consumed, id;
    std::vector<uint8_t> bytes, msg;
    std::uintmax_t fs;
    std::filesystem::path p;
    MAVLinkMessage message;

    // Check arguments.
    if (argc < 4) {
        std::cerr << "usage: ./extract <infile.bin> <outfile.bin> <ids to extract>\n";
        return 1;
    }

    iname = std::string(argv[1]);
    oname = std::string(argv[2]);

    // Check tha file exists.
    if (!std::filesystem::exists(iname)) {
        std::cerr << "File does not exist.\n";
        return 1;
    }

    // Check format of ids and add to set of ids.
    pattern = std::regex(R"(^(\d+)$)");

    for (i = 3; i < argc; i++) {
        if (!std::regex_match(std::string(argv[i]), pattern)) {
            std::cerr << "usage: ./extract <infile.bin> <outfile.bin> <ids to extract>\n";
            return 1;
        }

        ids.insert(atoi(argv[i]));
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

    // Open file for binary writing and write relevant messages to file.
    ofile.open(oname, std::ios::binary);

    if (!ofile.is_open()) {
        std::cerr << "Failed to open file.\n";
        return 1;
    }

    // Convert bytes into messages.
    bytes_consumed = 0;
    while (bytes_consumed < bytes.size()) {
        // Create message object.
        message = MAVLinkMessage(bytes, bytes_consumed);

        // Remove bytes from byte vector after conversion to message.
        bytes_consumed += message.get_len();

        id = message.get_msg_id();

        if (ids.contains(id)) {
            msg = message.serialize();
            ofile.write(reinterpret_cast<const char*>(msg.data()), msg.size());
        }
    }

    ofile.close();

    return 0;

}

