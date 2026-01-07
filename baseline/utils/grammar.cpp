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

void Grammar::add_message(std::vector<uint8_t> message) {
    int len;
    bool exists;

    // Get message length.
    len = message.size();

    // See if key exists in map.
    exists = grammar.contains(len);

    if (!exists) {
        // Create a vector of vectors.
        std::vector<std::unordered_set<uint8_t>> value;
        value.reserve(len);

        for (auto val : message)
            value.emplace_back(std::unordered_set<uint8_t>{val});
        
        // Insert into map.
        grammar.emplace(len, std::move(value));
    
    } else {
        // Find value in grammar.
        auto& value = grammar.at(len);
        
        // For each byte insert into grammar.
        int i = 0;
        for (auto val : message) {
            value[i].insert(val);
            i++;
        }
    }
}
        
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
                for (const auto& token : rule) {
                    *os << std::format("'\\x{:02X}'", token);
                
                    j++;
                    if (j%RPL == 0) *os << "\n\t";
                    else *os << " ";
                }

                i++;
                *os << std::format(";\n\n");
            }
        }
    }


}

std::ostream& operator<<(std::ostream& os, Grammar& grammar) {
    grammar.print(&os);
    return os;
}
