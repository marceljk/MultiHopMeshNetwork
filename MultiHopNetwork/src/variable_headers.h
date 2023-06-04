#ifndef VARIABLE_HEADER_H
#define VARIABLE_HEADER_H

#include <Arduino.h>

#include <iomanip>
#include <iostream>
#include <string>
#include <set>

#include <algorithm>

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

struct VariableHeader
{
    size_t size;
    ControlPacketType controlPacketType;

public:
    VariableHeader(size_t size, ControlPacketType controlPacketType) : size(size), controlPacketType(controlPacketType) {}
    virtual std::string toString();
};

struct ConnectHeader : VariableHeader
{
    uint8_t protocolVersion;
    uint8_t uuid[16];

public:
    ConnectHeader(size_t size, ControlPacketType controlPacketType, uint8_t protocolVersion, uint8_t *uuid) : VariableHeader(size, controlPacketType), protocolVersion(protocolVersion)
    {
        memcpy(this->uuid, uuid, 16);
    }
    std::string toString() override;
};

struct ConnackHeader : VariableHeader
{
    uint8_t returnCode;

public:
    ConnackHeader(size_t size, ControlPacketType controlPacketType, uint8_t returnCode) : VariableHeader(size, controlPacketType), returnCode(returnCode) {}
    std::string toString() override;
};

struct PublishHeader : VariableHeader
{
    uint8_t topicNameLength;
    std::string topicName;
    uint16_t packetID;

public:
    PublishHeader(size_t size, ControlPacketType controlPacketType, uint8_t topicNameLength, std::string topicName, uint16_t packetID) : VariableHeader(size, controlPacketType), topicNameLength(topicNameLength), topicName(topicName), packetID(packetID) {}
    std::string toString() override;
};

struct PubackHeader : VariableHeader
{
    uint16_t packetID;

public:
    PubackHeader(size_t size, ControlPacketType controlPacketType, uint16_t packetID) : VariableHeader(size, controlPacketType), packetID(packetID) {}
    std::string toString() override;
};

struct SubscribeHeader : VariableHeader
{
    uint8_t topicNameLength;
    std::string topicName;
    uint16_t packetID;

public:
    SubscribeHeader(size_t size, ControlPacketType controlPacketType, uint8_t topicNameLength, std::string topicName, uint16_t packetID) : VariableHeader(size, controlPacketType), topicNameLength(topicNameLength), topicName(topicName), packetID(packetID) {}
    std::string toString() override;
};

struct SubackHeader : VariableHeader
{
    uint16_t packetID;

public:
    SubackHeader(size_t size, ControlPacketType controlPacketType, uint16_t packetID) : VariableHeader(size, controlPacketType), packetID(packetID) {}
    std::string toString() override;
};

#endif