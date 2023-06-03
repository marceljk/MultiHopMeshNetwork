#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <Arduino.h>

#include <iomanip>
#include <iostream>
#include <string>
#include <set>

#define MAX_MESSAGE_SIZE 244
#define HEADER_SIZE 2

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

// MUST BE UPDATED IF MORE CONTROL PACKET TYPES ARE ADDED
extern std::set<int> validControlPackageValues;

struct ConnectHeader
{
    uint8_t protocolVersion;
    uint8_t uuid[16];

    std::string toString()
    {
        std::string str = "CONNECT\n";
        str += "Protocol Version: ";
        str += std::to_string(protocolVersion);
        str += ", UUID: ";
        for (int i = 0; i < 16; ++i)
            str += std::to_string(uuid[i]);
        return str;
    }
};

struct ConnackHeader
{
    uint8_t returnCode;

    std::string toString()
    {

        std::string str = "CONNACK\n";

        str += "Return Code: ";
        str += std::to_string(returnCode);
        return str;
    }
};

struct PublishHeader
{
    uint16_t topicNameLength;
    char *topicName;
    uint16_t packetID;

    std::string toString()
    {
        std::string str = "PUBLISH\n";

        str += "Topic Name Length: ";
        str += std::to_string(topicNameLength);
        str += ", Topic Name: ";
        str += topicName;
        str += ", Packet ID: ";
        str += std::to_string(packetID);
        return str;
    }
};

struct PubackHeader
{
    uint16_t packetID;

    std::string toString()
    {
        std::string str = "PUBACK\n";

        str += "Packet ID: ";
        str += std::to_string(packetID);
        return str;
    }
};

struct SubscribeHeader
{
    uint16_t topicNameLength;
    char *topicName;
    uint16_t packetID;

    std::string toString()
    {
        std::string str = "SUBSCRIBE\n";

        str += "Topic Name Length: ";
        str += std::to_string(topicNameLength);
        str += ", Topic Name: ";
        str += topicName;
        str += ", Packet ID: ";
        str += std::to_string(packetID);
        return str;
    }
};

struct SubackHeader
{
    uint16_t packetID;

    std::string toString()
    {
        std::string str = "SUBACK\n";

        str += "Packet ID: ";
        str += std::to_string(packetID);
        return str;
    }
};

struct DisconnectHeader
{

    std::string toString()
    {
        std::string str = "DISCONNECT\n";
        return str;
    }
};

struct VariableHeader
{
    size_t size;
    ControlPacketType controlPacketType;
    union
    {
        ConnectHeader connect;
        ConnackHeader connack;
        PublishHeader publish;
        PubackHeader puback;
        SubscribeHeader subscribe;
        SubackHeader suback;
        DisconnectHeader disconnect;
    };

    std::string toString()
    {
        std::string str = "Variable Header: ";

        str += "Size: ";
        str += std::to_string(size);
        str += ", Control Packet Type: ";
        str += std::to_string(static_cast<int>(controlPacketType));

        switch (controlPacketType)
        {
        case CONNECT:
            str += connect.toString();
            break;
        case CONNACK:
            str += connack.toString();
            break;
        case PUBLISH:
            str += publish.toString();
            break;
        case PUBACK:
            str += puback.toString();
            break;
        case SUBSCRIBE:
            str += subscribe.toString();
            break;
        case SUBACK:
            str += suback.toString();
            break;
        case DISCONNECT:
            str += disconnect.toString();
            break;
        default:
            str += "Unknown Control Packet Type";
            break;
        }
        return str;
    }
};

struct Header
{
    ControlPacketType controlPacketType;
    bool isDuplicate;
    bool retain;
    uint8_t qosLevel;
    uint8_t packetLength;

    Header() {}
    Header(ControlPacketType cpt, bool duplicate, bool r, uint8_t qos)
        : controlPacketType(cpt), isDuplicate(duplicate), retain(r), qosLevel(qos) {}
    Header(ControlPacketType cpt, bool duplicate, bool r, uint8_t qos, uint8_t length)
        : controlPacketType(cpt), isDuplicate(duplicate), retain(r), qosLevel(qos), packetLength(length) {}

    std::string toString()
    {
        std::string str = "Header \n";

        str += "ControlPacketType: ";
        str += std::to_string(static_cast<int>(controlPacketType));
        str += ", IsDuplicate: ";
        str += std::to_string(isDuplicate);
        str += ", Retain: ";
        str += std::to_string(retain);
        str += ", QOS Level: ";
        str += std::to_string(qosLevel);
        str += ", Packet Length: ";
        str += std::to_string(packetLength);
        return str;
    }
};

struct Message
{
    Header header;
    VariableHeader variableHeader;
    uint8_t payload[MAX_MESSAGE_SIZE];
    size_t payloadSize;

    std::string toString()
    {
        std::string str = "Message: \n";

        str += "Header: ";
        str += header.toString();
        str += "\nVariable Header: ";
        str += variableHeader.toString();
        str += "\nPayload: ";

        for (size_t i = 0; i < payloadSize; i++)
            str += char(payload[i]);

        return str;
    }
};

// Function to create a message
void serializeMessage(Message message, uint8_t *buf);

// Function to parse a message
Message parseMessage(uint8_t *rawMessage, size_t rawMessageSize);

Header parseHeader(uint8_t rawHeader[2]);

void serializeHeader(Header header, uint8_t rawHeader[2]);

VariableHeader parseVariableHeader(ControlPacketType controlPacketType, uint8_t *rawVariableHeader);

void serializeVariableHeader(VariableHeader variableHeader, uint8_t *rawVariableHeader);

boolean operator==(const Header &lhs, const Header &rhs);
boolean operator!=(const Header &lhs, const Header &rhs);

void generateRandomMessage(Message msg);

#endif // PROTOCOL_H
