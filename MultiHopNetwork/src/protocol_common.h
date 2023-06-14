#ifndef PROTOCOL_COMMON_H
#define PROTOCOL_COMMON_H

#include <Arduino.h>
#include <string>
#include <variable_headers.h>
#include <memory>

#include "config.h"

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
    std::unique_ptr<VariableHeader> variableHeader;
    std::string payload;

    Message(FixedHeader hdr, std::unique_ptr<VariableHeader> vHdr, std::string pl) : header(hdr), variableHeader(std::move(vHdr)), payload(pl) {}
    std::string toString();
};

Message createConnectionMessage(uint8_t uuid[16]);

Message createConnackMessage(Message &msg, uint8_t networkID, ConnackReturnCode returnCode = ACCEPTED);

Message createPublishMessage(std::string topicName, uint16_t packetID, bool duplicate = false, bool retain = false, uint8_t qosLevel = 0);

Message createPubackMessage(Message &msg);

Message createSubscribeMessage(std::string topicName, uint16_t packetID, bool duplicate = false, bool retain = false, uint8_t qosLevel = 1);

Message createSubackMessage(Message &msg);

Message createDisconnectMessage(uint8_t uuid[16]);
#endif