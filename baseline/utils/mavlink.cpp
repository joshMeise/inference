/*
 * mavlink.cpp - 
 *
 * Josh Meise
 * 01-06-2026
 * Description: 
 *
 */

#include <mavlink.h>

#define M2HDRLEN 10
#define CSLEN 2

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                         *
 *                    MAVLinkMessage                       *
 *                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// Deserializer.
Message::Message(std::vector<uint8_t> bytes) {

}
