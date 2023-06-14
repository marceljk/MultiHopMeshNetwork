#ifndef VARIABLE_HEADER_H
#define VARIABLE_HEADER_H

#include <string>

#include <Arduino.h>
#include <algorithm>
#include <iomanip>
#include <sstream>

enum ControlPacketType
{
    CONNECT = 1,
    CONNACK = 2,
    PUBLISH = 3,
    PUBACK = 4,
    SUBSCRIBE = 8,
    SUBACK = 9,
    DISCONNECT = 14
};

enum ConnackReturnCode
{
    ACCEPTED = 0,
    UNSUPPORTED_VERSION = 1,
    UNAVAILABLE = 3,
    UNAUTHORIZED = 5,
};

struct VariableHeader
{
    size_t size;
    ControlPacketType controlPacketType;

    VariableHeader(size_t size, ControlPacketType controlPacketType) : size(size), controlPacketType(controlPacketType) {}
    virtual std::string toString();
    virtual ~VariableHeader() = default;
};

struct ConnectHeader : VariableHeader
{
    uint8_t protocolVersion;
    uint8_t uuid[16];

    ConnectHeader(uint8_t protocolVersion, uint8_t *uuid) : VariableHeader(17, CONNECT), protocolVersion(protocolVersion)
    {
        memcpy(this->uuid, uuid, 16);
    }
    std::string toString() override;
};

struct ConnackHeader : VariableHeader
{
    ConnackReturnCode returnCode;
    uint8_t networkID;
    uint8_t uuid[16];

    ConnackHeader(ConnackReturnCode returnCode, uint8_t networkID, uint8_t *uuid) : VariableHeader(2, CONNACK), returnCode(returnCode), networkID(networkID)
    {
        memcpy(this->uuid, uuid, 16);
    }
    std::string toString() override;
};

struct PublishHeader : VariableHeader
{
    uint8_t topicNameLength;
    std::string topicName;
    uint16_t packetID;

    PublishHeader(uint8_t topicNameLength, std::string topicName, uint16_t packetID) : VariableHeader(topicNameLength + 3, PUBLISH), topicNameLength(topicNameLength), topicName(topicName), packetID(packetID) {}
    std::string toString() override;
};

struct PubackHeader : VariableHeader
{
    uint16_t packetID;

    PubackHeader(uint16_t packetID) : VariableHeader(2, PUBACK), packetID(packetID) {}
    std::string toString() override;
};

struct SubscribeHeader : VariableHeader
{
    uint8_t topicNameLength;
    std::string topicName;
    uint16_t packetID;

    SubscribeHeader(uint8_t topicNameLength, std::string topicName, uint16_t packetID) : VariableHeader(topicNameLength + 3, SUBSCRIBE), topicNameLength(topicNameLength), topicName(topicName), packetID(packetID) {}
    std::string toString() override;
};

struct SubackHeader : VariableHeader
{
    uint16_t packetID;

    SubackHeader(uint16_t packetID) : VariableHeader(2, SUBACK), packetID(packetID) {}
    std::string toString() override;
};

struct DisconnectHeader : VariableHeader
{
    uint8_t uuid[16];
    DisconnectHeader(uint8_t *uuid) : VariableHeader(16, DISCONNECT)
    {
        memcpy(this->uuid, uuid, 16);
    }
    std::string toString() override;
};

#endif