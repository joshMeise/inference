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
        message = MAVLinkMessage(bytes, bytes_consumed);

        // Remove bytes from byte vector after conversion to message.
        bytes_consumed += message.get_len();

        messages.push_back(message);
    }

    // Randomize order of messages in vector prior to adding to grammar.
    rng = std::mt19937(rd());
    std::shuffle(messages.begin(), messages.end(), rng);

    // Begin plotting process.
    gp = popen("gnuplot", "w");

    // Make grammar and plot.
    i = 0;
    fprintf(gp,"$DATA << EOD\n");
    for (const auto& message : messages) {
        grammar.add_message(message.serialize());
        fprintf(gp,"%d %d %d %d\n", i, grammar.get_num_non_terminals() + grammar.get_num_terminals(), grammar.get_num_non_terminals(), grammar.get_num_terminals());
        i++;
    }
    fprintf(gp,"EOD\n");

    // Set plot attributes.
    fprintf(gp,"set terminal png size 900,700\n");
    fprintf(gp,"set output 'plot.png'\n");
    fprintf(gp,"set multiplot\n");
    fprintf(gp,"set size 1,0.5\n");
    fprintf(gp,"set origin 0,0.5\n");
    fprintf(gp,"set title 'Total terminals and non-terminals vs. number of messages'\n");
    fprintf(gp,"set xlabel 'Number of messages'\n");
    fprintf(gp,"set ylabel 'Terminals and non-terminals'\n");
    fprintf(gp,"unset key\n");
    fprintf(gp,"plot $DATA using 1:2 w l title 'Total'\n");
    fprintf(gp,"set size 0.5,0.5\n");
    fprintf(gp,"set origin 0,0\n");
    fprintf(gp,"set title 'Non-terminals vs. number of messages'\n");
    fprintf(gp,"set xlabel 'Number of messages'\n");
    fprintf(gp,"set ylabel 'Non-terminals'\n");
    fprintf(gp,"unset key\n");
    fprintf(gp,"plot $DATA using 1:3 w l title 'Non-terminals'\n");
    fprintf(gp,"set origin 0.5,0\n");
    fprintf(gp,"set title 'Terminals vs. number of messages'\n");
    fprintf(gp,"set xlabel 'Number of messages'\n");
    fprintf(gp,"set ylabel 'Terminals'\n");
    fprintf(gp,"unset key\n");
    fprintf(gp,"plot $DATA using 1:4 w l title 'Terminals'\n");
    fprintf(gp,"unset multiplot\n");
 
    pclose(gp);

    std::cout << grammar;

    ifile.close();

    return 0;
}
