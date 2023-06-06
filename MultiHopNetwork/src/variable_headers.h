#ifndef VARIABLE_HEADER_H
#define VARIABLE_HEADER_H

#include <string>

#include <Arduino.h>
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

    VariableHeader(size_t size, ControlPacketType controlPacketType) : size(size), controlPacketType(controlPacketType) {}
    virtual std::string toString();
};

struct ConnectHeader : VariableHeader
{
    uint8_t protocolVersion;
    uint8_t uuid[16];

    ConnectHeader(size_t size, ControlPacketType controlPacketType, uint8_t protocolVersion, uint8_t *uuid) : VariableHeader(size, controlPacketType), protocolVersion(protocolVersion)
    {
        memcpy(this->uuid, uuid, 16);
    }
    std::string toString() override;
};

struct ConnackHeader : VariableHeader
{
    uint8_t returnCode;

    ConnackHeader(size_t size, ControlPacketType controlPacketType, uint8_t returnCode) : VariableHeader(size, controlPacketType), returnCode(returnCode) {}
    std::string toString() override;
};

struct PublishHeader : VariableHeader
{
    uint8_t topicNameLength;
    std::string topicName;
    uint16_t packetID;

    PublishHeader(size_t size, ControlPacketType controlPacketType, uint8_t topicNameLength, std::string topicName, uint16_t packetID) : VariableHeader(size, controlPacketType), topicNameLength(topicNameLength), topicName(topicName), packetID(packetID) {}
    std::string toString() override;
};

struct PubackHeader : VariableHeader
{
    uint16_t packetID;

    PubackHeader(size_t size, ControlPacketType controlPacketType, uint16_t packetID) : VariableHeader(size, controlPacketType), packetID(packetID) {}
    std::string toString() override;
};

struct SubscribeHeader : VariableHeader
{
    uint8_t topicNameLength;
    std::string topicName;
    uint16_t packetID;

    SubscribeHeader(size_t size, ControlPacketType controlPacketType, uint8_t topicNameLength, std::string topicName, uint16_t packetID) : VariableHeader(size, controlPacketType), topicNameLength(topicNameLength), topicName(topicName), packetID(packetID) {}
    std::string toString() override;
};

struct SubackHeader : VariableHeader
{
    uint16_t packetID;

    SubackHeader(size_t size, ControlPacketType controlPacketType, uint16_t packetID) : VariableHeader(size, controlPacketType), packetID(packetID) {}
    std::string toString() override;
};

#endif