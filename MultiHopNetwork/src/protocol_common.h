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

Message createConnectionMessage(const std::array<uint8_t, 16> uuid);

Message createConnackMessage(const Message &msg, const uint8_t networkID, const ConnackReturnCode returnCode = ACCEPTED);

Message createPublishMessage(const std::string topicName, const uint16_t packetID, const std::string payload, const bool duplicate = false, const bool retain = false, const uint8_t qosLevel = 0);

Message createPubackMessage(const Message &msg);

Message createSubscribeMessage(const std::string topicName, const uint16_t packetID, const bool duplicate = false, const bool retain = false, const uint8_t qosLevel = 1);

Message createSubackMessage(const Message &msg);

Message createDisconnectMessage(const std::array<uint8_t, 16> uuid);

void printUUID(const std::array<uint8_t, 16> uuid, std::string location);
#endif