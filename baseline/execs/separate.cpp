/*
 * separate.cpp --
 *
 * Josh Meise
 * 01-14-2026
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
    std::string iname, oname, odir, pass;
    std::ifstream ifile;
    std::ofstream ofile;
    int i, bytes_consumed;
    std::vector<uint8_t> bytes, msg;
    std::uintmax_t fs;
    std::filesystem::path p;
    MAVLinkMessage message;

    // Check arguments.
    if (argc != 3) {
        std::cerr << "usage: ./extract <infile.bin> <outdir>\n";
        return 1;
    }

    iname = std::string(argv[1]);
    odir = std::string(argv[2]);

    // Check that file exists.
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

    // Check that output directory exists.
    if (!std::filesystem::is_directory(odir)) {
        std::cerr << "Output directory does not exist.\n";
        return 1;
    }

    // Convert bytes into messages.
    bytes_consumed = 0;
    i = 1;
    pass = std::string("pass");
    while (bytes_consumed < bytes.size()) {
        // Create message object.
        message = MAVLinkMessage(bytes, bytes_consumed);

        // Remove bytes from byte vector after conversion to message.
        bytes_consumed += message.get_len();

        oname = odir + "/" + pass + "." + std::to_string(i++);

        // Open file and read in messages.
        ofile.open(oname, std::ios::binary);

        if (!ofile.is_open()) {
            std::cerr << "Failed to open file.\n";
            return 1;
        }

        msg = message.serialize();

        ofile.write(reinterpret_cast<char*>(msg.data()), msg.size());

        ofile.close();

    }

    return 0;

}

