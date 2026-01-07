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
    
    } else {
        // Find value in grammar.
        auto& value = grammar.at(len);
        
        // For each byte insert into grammar.
        i = 0;
        for (auto val : message) {
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
    }
}

// Getter methds.
int Grammar::get_num_non_terminals(void) const { return num_non_terminals; }
int Grammar::get_num_terminals(void) const { return num_terminals; }

// Printing methods.
void Grammar::print(std::ostream *os) {
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
    grammar.print(&os);
    return os;
}
