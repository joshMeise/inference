/*
 * infer.cpp
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
#include <regex>

int main(int argc, char **argv) {
    Grammar grammar;
    std::ifstream ifile;
    std::ofstream gfile, lfile;
    std::vector<uint8_t> bytes;
    std::filesystem::path p;
    std::string fname, pname, lname, ratio, gname;
    std::uintmax_t fs;
    std::vector<MAVLinkMessage> all_messages, train_messages, test_messages;
    std::vector<std::vector<uint8_t>> test_bytes;
    MAVLinkMessage message;
    FILE* gp;
    int i, bytes_consumed, test, train, div;
    std::random_device rd;
    std::mt19937 rng;
    std::regex pattern;
    std::smatch match;

    if (argc != 6) {
        std::cout << "usage: ./infer <input_file.bin> <train:test> <plot_file.png> <log_file.txt> <grammar_file.xbnf>\n";
        return 1;
    }

    fname = std::string(argv[1]);
    ratio = std::string(argv[2]);
    pname = std::string(argv[3]);
    lname = std::string(argv[4]);
    gname = std::string(argv[5]);
    
    // Check tha file exists.
    if (!std::filesystem::exists(fname)) {
        std::cerr << "File does not exist.\n";
        return 1;
    }

    // Check format of ratio and extract integer values.
    pattern = std::regex(R"(^(\d+):(\d+)$)");

    if (!std::regex_match(ratio, match, pattern)) {
        std::cerr << "train:test ratio format incorrect\n";
        return 1;
    }
    
    train = std::stoi(match[1].str());
    test = std::stoi(match[2].str());

    if (train + test != 100) {
        std::cerr << "Train and test need to add to 100.\n";
        return 1;
    }

    // Get number of bytes in file.
    p = std::filesystem::path(fname);
    fs = std::filesystem::file_size(p);

    // Open file for binary reading.
    ifile.open(fname, std::ios::binary);

    // Check that file is open.
    if (!ifile.is_open()) {
        std::cerr << "Failed to open file.\n";
        return 1;
    }

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

        all_messages.push_back(message);
    }

    // Randomize order of messages in vector prior to adding to grammar.
    rng = std::mt19937(rd());
    std::shuffle(all_messages.begin(), all_messages.end(), rng);

    // Extract train and test messages.
    div = all_messages.size()*train/100;

    train_messages = std::vector<MAVLinkMessage>(all_messages.begin(), all_messages.begin() + div);
    test_messages = std::vector<MAVLinkMessage>(all_messages.begin() + div, all_messages.end());

    // Serialize test messages.
    for (auto message : test_messages)
        test_bytes.push_back(message.serialize());

    // Begin plotting process.
    gp = popen("gnuplot", "w");

    // Make grammar and plot.
    i = 0;
    fprintf(gp,"$DATA << EOD\n");
    for (const auto& message : train_messages) {
        grammar.add_message(message.serialize());
        fprintf(gp,"%d %d %d %d\n", i, grammar.get_num_non_terminals() + grammar.get_num_terminals(), grammar.get_num_non_terminals(), grammar.get_num_terminals());
        i++;
    }
    fprintf(gp,"EOD\n");

    // Set plot attributes.
    fprintf(gp,"set terminal png size 900,700\n");
    fprintf(gp,"set output '%s'\n", pname.c_str());
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
    
    // Open file for writing.
    gfile.open(gname);

    // Check that file is open.
    if (!gfile.is_open()) {
        std::cerr << "Failed to open file.\n";
        return 1;
    }

    gfile << grammar;

     // Open file for writing.
    lfile.open(lname);

    // Check that file is open.
    if (!lfile.is_open()) {
        std::cerr << "Failed to open file.\n";
        return 1;
    }

    grammar.print_attr(&lfile);

    // Evaluate accuracy of inferred grammar and write results to file.
    grammar.evaluate(test_bytes, &lfile);

    gfile.close();
    lfile.close();
    ifile.close();

    return 0;
}
