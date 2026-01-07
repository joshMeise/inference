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
#include <unordered_set>
#include <vector>
#include <cstdint>
#include <iostream>

class Grammar {
public:
    // Default constructor.
    Grammar(void) = default;

    // Add message to grammar.
    void add_message(std::vector<uint8_t> message);

    // Printing methods.
    void print(std::ostream *os);
    friend std::ostream& operator<<(std::ostream& os, Grammar& grammar);

private:
    std::unordered_map<int, std::vector<std::unordered_set<uint8_t>>> grammar;

};
