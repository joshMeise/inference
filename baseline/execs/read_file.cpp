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
#include <cstdio>
#include <algorithm>
#include <random>

int main(void) {
    Grammar grammar;
    std::ifstream ifile;
    std::vector<uint8_t> bytes;
    std::filesystem::path p;
    std::string fname;
    std::uintmax_t fs;
    std::vector<MAVLinkMessage> messages;
    MAVLinkMessage message;
    FILE* gp;
    int i, bytes_consumed;
    std::random_device rd;
    std::mt19937 rng;

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
    bytes_consumed = 0;
    while (bytes_consumed < bytes.size()) {
        // Create message object.
        message = MAVLinkMessage(std::vector<uint8_t>(bytes.begin() + bytes_consumed, bytes.end()));

        // Remove bytes from byte vector after conversion to message.
        bytes_consumed += message.get_len();

        messages.push_back(message);
    }

    // Randomize order of messages in vector prior to adding to grammar.
    rng = std::mt19937(rd());
    std::shuffle(messages.begin(), messages.end(), rng);

    // Begin plotting process.
    gp = popen("gnuplot", "w");
    fprintf(gp, "set title 'Number of terminals and non-terminals in grammar vs. number of messages'\n");
    fprintf(gp, "set xlabel 'Number of messages'\n");
    fprintf(gp, "set ylabel 'Number of terminals and non-terminals'\n");
    fprintf(gp, "unset key\n");
    fprintf(gp, "set terminal png\n");
    fprintf(gp, "set output 'plot.png'\n");
    fprintf(gp, "plot '-' with lines\n");

    // Make grammar and plot.
    i = 0;
    for (const auto& message : messages) {
        grammar.add_message(message.serialize());
        fprintf(gp, "%d %d\n", i, grammar.get_num_non_terminals() + grammar.get_num_terminals());
        i++;
    }
    
    pclose(gp);
    ifile.close();

    return 0;
}
