/*
 * mavlink.h - 
 *
 * Josh Meise
 * 01-06-2026
 * Description: 
 *
 * Note: Use std::variant for different options on headers.
 *       Use std::optional to define signature
 */

#pragma once
#include <cstdint>
#include <vector>
#include <iostream>

class Header {
public:
    // Default constructor.
    Header(void) = default;

    // Deserializer.
    Header(std::vector<uint8_t> bytes);

    // Printing methods.
    void print(std::ostream *os);
    friend std::ostream& operator<<(std::ostream& os, Header& header);

private:
    uint8_t stx;
    uint8_t len;
    uint8_t inc_flags;
    uint8_t comp_flags;
    uint8_t seq;
    uint8_t sys_id;
    uint8_t comp_id;
    uint8_t msg_id[3];
}

class Payload {
public:
    // Default constructor.
    Payload(void) = default;

    // Desrializer.
    Payload(std::vector<uint8_t> bytes);

   // Printing methods.
    void print(std::ostream *os);
    friend std::ostream& operator<<(std::ostream& os, Payload& payload);

private:
    // Member variables.
    std::vector<uint8_t> payload;
    uint8_t checksum[2];
}

class MavlinkMessage {
public:
    // Default constructor.
    Message(void) = default;

    // Deserializer.
    Message(std::vector<uint8_t> bytes);

    // Getter methods.
    int get_length(void) const;

    // Printing methods.
    void print(std::ostream *os);
    friend std::ostream& operator<<(std::ostream& os, MavlinkMessage& message);

private:
    // Member variables.
    Header header;
    Payload payload;
    int length;
}
