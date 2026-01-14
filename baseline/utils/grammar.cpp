/*
 * grammar.cpp - 
 *
 * Josh Meise
 * 01-06-2026
 * Description: 
 *
 */

#include "grammar.h"
#include <format>

#define RPL 8

// Default constructor.
Grammar::Grammar(void) {
    num_non_terminals = 1;
    num_terminals = 0;
}

// Add message to grammar.
void Grammar::add_message(std::vector<uint8_t> message) {
    int len;
    bool exists;
    int i;

    // Get message length.
    len = message.size();

    // See if key exists in map.
    exists = grammar.contains(len);

    if (!exists) {
        // Create a vector of vectors.
        std::vector<std::unordered_set<uint8_t>> value;
        value.reserve(len);

        for (auto val : message) {
            value.emplace_back(std::unordered_set<uint8_t>{val});
            num_terminals++;
        }
        
        // Insert into map.
        grammar.emplace(len, std::move(value));

        // Increment number of non-terminals.
        num_non_terminals++;

        // Insert into message per length map.
        mpl.emplace(len, 1);
    } else {
        // Find value in grammar.
        auto& value = grammar.at(len);
        
        // For each byte insert into grammar.
        i = 0;
        for (const auto& val : message) {
            // Adding another terminal/non-terminal.
            if (value[i].find(val) == value[i].end()) {
                if (value[i].size() != 1) num_terminals++;
                else {
                    num_non_terminals++;
                    num_terminals++;
                }
            }

            value[i].insert(val);
            i++;
        }

        // Increment number of messages per length.
        mpl[len]++;
    }
}

// Getter methds.
int Grammar::get_num_non_terminals(void) const { return num_non_terminals; }
int Grammar::get_num_terminals(void) const { return num_terminals; }

// Evaluate an inferred grammar and stats to log file.
void Grammar::evaluate(std::vector<std::vector<uint8_t>> message_bytes, std::ostream *os) {
    int tp, fn, len, i;
    std::unordered_map<int, std::vector<std::vector<uint8_t>>> groups;

    tp = 0;
    fn = 0;

    for (const auto& message : message_bytes) {
        len = message.size();

        // If key is not in map, each of the bytes in the message qualifies as a false negative.
        if (grammar.find(len) == grammar.end()) fn += len;
        else {
            // Get gramamr rule for given length's non-terminal.
            const auto& value = grammar.at(len);

            i = 0;
            for (const auto& byte : message) {
                // If the byte does not exist in the grammar rule at that position it is a false negative. If it does exist it is a true positive.
                if (value[i].find(byte) == value[i].end()) fn++;
                else tp++;

                i++;
            }
        }
    }

    *os << "False negatives: " << fn << std::endl;
    *os << "True positives: " << tp << std::endl;
    *os << "Recall: " << std::format("{:.4f}", (double)tp/(tp + fn)*100) << std::endl;

}

// Printing methods.
void Grammar::print_attr(std::ostream *os) {
    *os << "Messages per length:\n";
    for (auto pair : mpl)
        *os << "Length: " << pair.first << ", number of messages: " << pair.second << std::endl;
    *os << "\n\n";

    *os << "Number of terminals = " << num_terminals << std::endl;
    *os << "Number of non-terminals = " << num_non_terminals << std::endl << std::endl;

}

void Grammar::print_grammar(std::ostream *os) {
    int i, j;

    *os << std::format("/* xBNF Grammar */\n");
    *os << std::format("start : ");
    
    i = 0;
    for (auto it = grammar.begin(); it != grammar.end();) {
        const auto& pair = *it;
        
        *os << std::format("nt_{}", pair.first);

        if (++it != grammar.end()) *os << std::format(" |");
        
        i++;

        if (i%RPL == 0) *os << "\n\t";
        else *os << " ";
    }

    *os << std::format(";\n\n");

    for (const auto& pair : grammar) {
        *os << std::format("nt_{} : ", pair.first);
        
        i = 0;
        for (const auto& rule : pair.second) {
            // Directly to a token.
            if (rule.size() == 1) *os << std::format("'\\x{:02X}'", *rule.begin());
            else *os << std::format("nt_{}_{}", pair.first, i);

            i++;

            if (i%RPL == 0) *os << "\n\t";
            else *os << " ";
        }

        *os << std::format(";\n\n");

        i = 0;
        for (const auto& rule : pair.second) {
            // Write out terminal for rule.
            if (rule.size() > 1) {
                *os << std::format("nt_{}_{} : ", pair.first, i);
                
                j = 0;
                for (auto it = rule.begin(); it != rule.end();) {
                    const auto& token = *it;

                    *os << std::format("'\\x{:02X}'", token);

                    if (++it != rule.end()) *os << std::format(" |");

                    j++;

                    if (j%RPL == 0) *os << "\n\t";
                    else *os << " ";
                }

                *os << std::format(";\n\n");
            }
            i++;
        }
    }


}

std::ostream& operator<<(std::ostream& os, Grammar& grammar) {
    grammar.print_grammar(&os);
    return os;
}
