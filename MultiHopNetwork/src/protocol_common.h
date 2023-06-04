#ifndef PROTOCOL_COMMON_H
#define PROTOCOL_COMMON_H

#include <Arduino.h>

#include <iomanip>
#include <iostream>
#include <string>
#include <set>
#include <variable_headers.h>

#include <variant>

#define MAX_MESSAGE_SIZE 244
#define HEADER_SIZE 2

struct FixedHeader
{
    ControlPacketType controlPacketType;
    bool isDuplicate;
    bool retain;
    uint8_t qosLevel;
    uint8_t packetLength;
    FixedHeader(ControlPacketType cpt, bool duplicate, bool r, uint8_t qos, uint8_t length)
        : controlPacketType(cpt), isDuplicate(duplicate), retain(r), qosLevel(qos), packetLength(length) {}
    std::string toString();
};

struct Message
{
    FixedHeader header;
    VariableHeader variableHeader;
    std::string payload;

    Message(FixedHeader hdr, VariableHeader vHdr, std::string pl) : header(hdr), variableHeader(vHdr), payload(pl) {}
    std::string toString();
};

Message dummyMessage();

#endif