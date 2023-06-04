#include "protocol.h"

std::set<int> validControlPackageValues{CONNECT, CONNACK, PUBLISH, PUBACK, SUBSCRIBE, SUBACK, DISCONNECT};

// Function to parse a message
Message parseMessage(std::vector<uint8_t> &rawMessage)
{
    if (rawMessage.size() < HEADER_SIZE)
    {
        throw std::invalid_argument("Invalid message size");
    }

    FixedHeader header = parseHeader(rawMessage);
    rawMessage.erase(rawMessage.begin(), rawMessage.begin() + HEADER_SIZE);

    if (header.packetLength > rawMessage.size())
    {
        throw std::invalid_argument("Invalid packet length in header");
    }

    VariableHeader variableHeader = parseVariableHeader(header.controlPacketType, rawMessage);
    rawMessage.erase(rawMessage.begin(), rawMessage.begin() + variableHeader.size);

    if (variableHeader.size > rawMessage.size())
    {
        throw std::invalid_argument("Invalid variable header size");
    }

    std::string payload(reinterpret_cast<const char *>(rawMessage.data()), header.packetLength - HEADER_SIZE - variableHeader.size);
    Message message = Message(header, variableHeader, payload);

    rawMessage.erase(rawMessage.begin(), rawMessage.begin() + header.packetLength);

    return message;
}

// Control packet bit mask
const uint8_t CONTROL_PACKET_MASK = 0xF0;
// Control packet shift count
const uint8_t CONTROL_PACKET_SHIFT = 4;

// Flags for checking the properties of the header
const uint8_t DUP_FLAG = 0b00001000;
const uint8_t QOS_FLAG = 0b00000110;
const uint8_t RETAIN_FLAG = 0b00000001;

FixedHeader parseHeader(std::vector<uint8_t> &rawHeader)
{
    uint8_t controlPacketIndex = (rawHeader[0] & CONTROL_PACKET_MASK) >> CONTROL_PACKET_SHIFT;
    ControlPacketType controlPacketType = static_cast<ControlPacketType>(controlPacketIndex);

    if (validControlPackageValues.find(controlPacketType) == validControlPackageValues.end())
    {
        throw std::invalid_argument("Invalid control packet type");
    }

    bool duplicate = (rawHeader[0] & DUP_FLAG);
    int qosLevel = (rawHeader[0] & QOS_FLAG) >> 1;
    bool retain = (rawHeader[0] & RETAIN_FLAG) > 0;
    int packetLength = rawHeader[1];

    return FixedHeader(controlPacketType, duplicate, retain, qosLevel, packetLength);
}

std::vector<uint8_t> serializeHeader(FixedHeader &header)
{
    std::vector<uint8_t> rawHeader(HEADER_SIZE);

    // Insert the control packet index into the first byte
    uint8_t controlPacketIndex = static_cast<uint8_t>(header.controlPacketType);
    rawHeader[0] = (controlPacketIndex << CONTROL_PACKET_SHIFT) & CONTROL_PACKET_MASK;

    // Insert the DUP, QOS, and RETAIN flags into the first byte
    if (header.isDuplicate)
    {
        rawHeader[0] |= DUP_FLAG;
    }
    rawHeader[0] |= (header.qosLevel << 1) & QOS_FLAG;
    if (header.retain)
    {
        rawHeader[0] |= RETAIN_FLAG;
    }

    // Insert the packet length into the second byte
    rawHeader[1] = header.packetLength;
    return rawHeader;
}

VariableHeader parseVariableHeader(ControlPacketType controlPacketType, std::vector<uint8_t> &rawVariableHeader)
{

    VariableHeader variableHeader = VariableHeader(0, controlPacketType);
    variableHeader.controlPacketType = controlPacketType;

    switch (controlPacketType)
    {
    case CONNECT:
    {
        uint8_t protocolVersion = rawVariableHeader[0];
        uint8_t uuid[16];
        for (int i = 0; i < 16; i++)
        {
            uuid[i] = rawVariableHeader[i + 1];
        }

        size_t size = 17;

        variableHeader = ConnectHeader(size, controlPacketType, protocolVersion, uuid);
        break;
    }

    case CONNACK:
    {
        uint8_t returnCode = rawVariableHeader[0];
        size_t size = 1;

        variableHeader = ConnackHeader(size, controlPacketType, returnCode);
        break;
    }

    case PUBLISH:
    {
        uint8_t topicNameLength = rawVariableHeader[0];
        std::string topicName(reinterpret_cast<const char *>(rawVariableHeader.data()), rawVariableHeader.size());

        uint16_t packetID = (rawVariableHeader[topicNameLength + 1] << 8) | rawVariableHeader[topicNameLength + 2];

        size_t size = 3 + topicNameLength;
        variableHeader = PublishHeader(size, controlPacketType, topicNameLength, topicName, packetID);
        break;
    }
    case PUBACK:
    {
        uint16_t packetID = (rawVariableHeader[0] << 8) | rawVariableHeader[1];
        size_t size = 2;

        variableHeader = PubackHeader(size, controlPacketType, packetID);
        break;
    }

    case SUBSCRIBE:
    {
        uint8_t topicNameLength = rawVariableHeader[0];
        std::string topicName(reinterpret_cast<const char *>(rawVariableHeader.data()), rawVariableHeader.size());

        uint16_t packetID = (rawVariableHeader[topicNameLength + 1] << 8) | rawVariableHeader[topicNameLength + 2];

        size_t size = 3 + topicNameLength;
        variableHeader = PublishHeader(size, controlPacketType, topicNameLength, topicName, packetID);

        SubscribeHeader(size, controlPacketType, topicNameLength, topicName, packetID);
        break;
    }

    case SUBACK:
    {
        uint16_t packetID = (rawVariableHeader[0] << 8) | rawVariableHeader[1];
        size_t size = 2;

        variableHeader = SubackHeader(size, controlPacketType, packetID);
        break;
    }

    default:
    {
        break;
    }
    }

    return variableHeader;
}

constexpr size_t UUID_SIZE = 16;
constexpr uint8_t SHIFT_BYTE = 8;

std::vector<uint8_t> serializeVariableHeader(VariableHeader &variableHeader)
{
    std::vector<uint8_t> rawVariableHeader(2);

    switch (variableHeader.controlPacketType)
    {
    case CONNECT:
    {
        ConnectHeader &connectHeader = static_cast<ConnectHeader &>(variableHeader);
        rawVariableHeader[0] = connectHeader.protocolVersion;
        for (int i = 0; i < 16; i++)
        {
            rawVariableHeader[i + 1] = connectHeader.uuid[i];
        }
        break;
    }
    case CONNACK:
    {
        ConnackHeader &connackHeader = static_cast<ConnackHeader &>(variableHeader);
        rawVariableHeader[0] = connackHeader.returnCode;
        break;
    }
    case PUBLISH:
    {
        PublishHeader &publishHeader = static_cast<PublishHeader &>(variableHeader);

        rawVariableHeader[0] = publishHeader.topicNameLength >> SHIFT_BYTE;
        rawVariableHeader[1] = publishHeader.topicNameLength;
        std::copy(publishHeader.topicName.begin(), publishHeader.topicName.end(), rawVariableHeader.begin() + 3);

        if (rawVariableHeader.size() < publishHeader.topicNameLength + 4)
        {
            throw std::out_of_range("Buffer size too small for operation");
        }

        rawVariableHeader[publishHeader.topicNameLength + 2] = publishHeader.packetID >> SHIFT_BYTE;
        rawVariableHeader[publishHeader.topicNameLength + 3] = publishHeader.packetID;
        break;
    }

    case PUBACK:
    {
        PubackHeader &pubackHeader = static_cast<PubackHeader &>(variableHeader);

        rawVariableHeader[0] = pubackHeader.packetID >> 8;
        rawVariableHeader[1] = pubackHeader.packetID;
        break;
    }

    case SUBSCRIBE:
    {
        SubscribeHeader &subscribeHeader = static_cast<SubscribeHeader &>(variableHeader);

        rawVariableHeader[0] = subscribeHeader.topicNameLength >> 8;
        rawVariableHeader[1] = subscribeHeader.topicNameLength;
        std::copy(subscribeHeader.topicName.begin(), subscribeHeader.topicName.end(), rawVariableHeader.begin() + 3);

        rawVariableHeader[subscribeHeader.topicNameLength + 3] = subscribeHeader.packetID;
        rawVariableHeader[subscribeHeader.topicNameLength + 2] = subscribeHeader.packetID >> 8;
        break;
    }

    case SUBACK:
    {
        SubackHeader &subackHeader = static_cast<SubackHeader &>(variableHeader);

        rawVariableHeader[0] = subackHeader.packetID >> 8;
        rawVariableHeader[1] = subackHeader.packetID;
        break;
    }
    }
    return rawVariableHeader;
}

std::vector<uint8_t> serializeMessage(Message &message)
{
    std::vector<uint8_t> serializedVariableHeader = serializeVariableHeader(message.variableHeader);
    size_t summedHeaderSize = message.variableHeader.size + HEADER_SIZE;

    if (message.payload.size() > MAX_MESSAGE_SIZE - summedHeaderSize)
    {
        message.payload.resize(MAX_MESSAGE_SIZE - summedHeaderSize);
    }

    message.header.packetLength = summedHeaderSize + message.payload.size();
    std::vector<uint8_t> serializedMessage = serializeHeader(message.header);

    serializedMessage.insert(serializedMessage.end(), serializedVariableHeader.begin(), serializedVariableHeader.end());
    serializedMessage.insert(serializedMessage.end(), message.payload.begin(), message.payload.end());

    return serializedMessage;
}