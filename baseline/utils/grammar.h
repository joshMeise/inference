/*
 * grammar.h - 
 *
 * Josh Meise
 * 01-06-2026
 * Description: 
 *
 */

#pragma once
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <vector>
#include <cstdint>
#include <iostream>

class Grammar {
public:
    // Default constructor.
    Grammar(void);

    // Add message to grammar.
    void add_message(std::vector<uint8_t> message);

    // Getter methods.
    int get_num_non_terminals(void) const;
    int get_num_terminals(void) const;

    // Printing methods.
    void print_attr(std::ostream *os);
    void print_grammar(std::ostream *os);
    friend std::ostream& operator<<(std::ostream& os, Grammar& grammar);

private:
    std::unordered_map<int, std::vector<std::unordered_set<uint8_t>>> grammar;
    int num_non_terminals;
    int num_terminals;
    std::map<int, int> mpl;
};
