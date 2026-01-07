/*
 * mavlink.cpp - 
 *
 * Josh Meise
 * 01-06-2026
 * Description: 
 *
 */

#include "mavlink.h"
#include <format>

#define HDRLEN 10
#define CSLEN 2

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                         *
 *                         Header                          *
 *                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// Deserializer.
Header::Header(std::vector<uint8_t> bytes) {
    stx = bytes[0];
    len = bytes[1];
    inc_flags = bytes[2];
    comp_flags = bytes[3];
    seq = bytes[4];
    sys_id = bytes[5];
    comp_id = bytes[6];
    msg_id[0] = bytes[7];
    msg_id[1] = bytes[8];
    msg_id[2] = bytes[9];
}

// Serializer.
std::vector<uint8_t> Header::serialize(void) const {
    std::vector<uint8_t> bytes;

    bytes.push_back(stx);
    bytes.push_back(len);
    bytes.push_back(inc_flags);
    bytes.push_back(comp_flags);
    bytes.push_back(seq);
    bytes.push_back(sys_id);
    bytes.push_back(comp_id);
    bytes.push_back(msg_id[0]);
    bytes.push_back(msg_id[1]);
    bytes.push_back(msg_id[2]);

    return bytes;
}

// Getter methods.
int Header::get_len(void) const { return len; }

// Printing methods.
void Header::print(std::ostream *os) {
    *os << std::format("stx = 0x{:02X}\n", stx);
    *os << std::format("len = 0x{:02X}\n", len);
    *os << std::format("inc_flags = 0x{:02X}\n", inc_flags);
    *os << std::format("comp_flags = 0x{:02X}\n", comp_flags);
    *os << std::format("seq = 0x{:02X}\n", seq);
    *os << std::format("sys_id = 0x{:02X}\n", sys_id);
    *os << std::format("comp_id = 0x{:02X}\n", comp_id);
    *os << std::format("msg_id = 0x{:02X} 0x{:02X} 0x{:02X}\n", msg_id[0], msg_id[1], msg_id[2]);
}

std::ostream& operator<<(std::ostream& os, Header& header) {
    header.print(&os);
    return os;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                         *
 *                         Payload                         *
 *                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// Deserializer.
Payload::Payload(std::vector<uint8_t> bytes) {
    pl = std::vector<uint8_t>(bytes.begin(), bytes.end() - CSLEN);
    checksum[0] = bytes[bytes.size() - 2];
    checksum[1] = bytes[bytes.size() - 1];
}

// Serializer.
std::vector<uint8_t> Payload::serialize(void) const {
    std::vector<uint8_t> bytes;

    bytes.insert(bytes.begin(), pl.begin(), pl.end());
    bytes.push_back(checksum[0]);
    bytes.push_back(checksum[1]);

    return bytes;
}

// Printing methods.
void Payload::print(std::ostream *os) {
    *os << "payload = ";

    for (uint8_t byte : pl) *os << std::format("0x{:02X} ", byte);
    *os << "\n";
    *os << std::format("checksum = 0x{:02X} 0x{:02X}\n", checksum[0], checksum[1]);
}

std::ostream& operator<<(std::ostream& os, Payload& payload) {
    payload.print(&os);
    return os;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                         *
 *                    MAVLinkMessage                       *
 *                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// Deserializer.
MAVLinkMessage::MAVLinkMessage(std::vector<uint8_t> bytes) :
    header(std::vector<uint8_t>(bytes.begin(), bytes.begin() + HDRLEN)) {
    // Construct payload.
    payload = Payload(std::vector<uint8_t>(bytes.begin() + HDRLEN, bytes.begin() + HDRLEN + header.get_len() + CSLEN));
}

// Serializer.
std::vector<uint8_t> MAVLinkMessage::serialize(void) const {
    std::vector<uint8_t> h_bytes, pl_bytes, bytes;
    
    h_bytes = header.serialize();
    pl_bytes = payload.serialize();

    bytes.insert(bytes.begin(), h_bytes.begin(), h_bytes.end());
    bytes.insert(bytes.end(), pl_bytes.begin(), pl_bytes.end());

    return bytes;
}

// Getter methods.
int MAVLinkMessage::get_len(void) const { return (HDRLEN + CSLEN + header.get_len()); }

// Printing methods.
void MAVLinkMessage::print(std::ostream *os) {
    header.print(os);
    payload.print(os);
}

std::ostream& operator<<(std::ostream& os, MAVLinkMessage& message) {
    message.print(&os);
    return os;
}


