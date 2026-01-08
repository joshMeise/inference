/*
 * mavlink.h - 
 *
 * Josh Meise
 * 01-06-2026
 * Description: Currently only suports MAVLink 2 messages;signatures currently not supported.
 *
 * Note: Use std::variant for different options on headers.
 *       Use std::optional to define signature
 */

#pragma once
#include <cstdint>
#include <vector>
#include <iostream>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                         *
 *                         Header                          *
 *                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

class Header {
public:
    // Default constructor.
    Header(void) = default;

    // Deserializer.
    Header(const std::vector<uint8_t>& bytes, int offset);

    // Serializer.
    std::vector<uint8_t> serialize(void) const;

    // Getter methods.
    int get_len(void) const;

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
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                         *
 *                         Payload                         *
 *                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

class Payload {
public:
    // Default constructor.
    Payload(void) = default;

    // Desrializer.
    Payload(const std::vector<uint8_t>& bytes, int offset, int len);

    // Serializer.
    std::vector<uint8_t> serialize(void) const;

    // Printing methods.
    void print(std::ostream *os);
    friend std::ostream& operator<<(std::ostream& os, Payload& payload);

private:
    // Member variables.
    std::vector<uint8_t> pl;
    uint8_t checksum[2];
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                         *
 *                    MAVLinkMessage                       *
 *                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

class MAVLinkMessage {
public:
    // Default constructor.
    MAVLinkMessage(void) = default;

    // Deserializer.
    MAVLinkMessage(const std::vector<uint8_t>& bytes, int offset);

    // Serializer.
    std::vector<uint8_t> serialize(void) const;

    // Getter methods.
    int get_len(void) const;

    // Printing methods.
    void print(std::ostream *os);
    friend std::ostream& operator<<(std::ostream& os, MAVLinkMessage& message);

private:
    // Member variables.
    Header header;
    Payload payload;

};
