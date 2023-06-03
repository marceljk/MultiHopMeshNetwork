#include "protocol.h"

std::set<int> validControlPackageValues{CONNECT, CONNACK, PUBLISH, PUBACK, SUBSCRIBE, SUBACK, DISCONNECT};

// Function to parse a message
Message parseMessage(uint8_t *rawMessage, size_t rawMessageSize)
{
    Message message;

    message.header = parseHeader(rawMessage);
    message.variableHeader = parseVariableHeader(message.header.controlPacketType, rawMessage + HEADER_SIZE);

    size_t payloadSize = message.header.packetLength - HEADER_SIZE - message.variableHeader.size;

    memcpy(message.payload, rawMessage + HEADER_SIZE + message.variableHeader.size, payloadSize);
    message.payloadSize = payloadSize;

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

Header parseHeader(uint8_t rawHeader[2])
{
    if (rawHeader == nullptr)
    {
        throw std::invalid_argument("rawHeader cannot be null");
    }

    Header header;
    uint8_t controlPacketIndex = (rawHeader[0] & CONTROL_PACKET_MASK) >> CONTROL_PACKET_SHIFT;

    header.controlPacketType = static_cast<ControlPacketType>(controlPacketIndex);
    if (validControlPackageValues.find(header.controlPacketType) == validControlPackageValues.end())
    {
        throw std::invalid_argument("Invalid control packet type");
    }

    header.isDuplicate = (rawHeader[0] & DUP_FLAG);
    header.qosLevel = (rawHeader[0] & QOS_FLAG) >> 1;
    header.retain = (rawHeader[0] & RETAIN_FLAG) > 0;

    header.packetLength = rawHeader[1];

    return header;
}

void serializeHeader(Header header, uint8_t rawHeader[2])
{
    if (validControlPackageValues.find(header.controlPacketType) == validControlPackageValues.end())
    {
        throw std::invalid_argument("Invalid control packet type");
    }

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
}

VariableHeader parseVariableHeader(ControlPacketType controlPacketType, uint8_t *rawVariableHeader)
{
    if (rawVariableHeader == nullptr)
    {
        throw std::invalid_argument("rawVariableHeader cannot be null");
    }
    VariableHeader variableHeader;
    variableHeader.controlPacketType = controlPacketType;

    switch (controlPacketType)
    {
    case CONNECT:
        variableHeader.connect.protocolVersion = rawVariableHeader[0];
        for (int i = 0; i < 16; i++)
        {
            variableHeader.connect.uuid[i] = rawVariableHeader[i + 1];
        }

        variableHeader.size = 17;
        break;

    case CONNACK:
        variableHeader.connack.returnCode = rawVariableHeader[0];
        variableHeader.size = 1;
        break;

    case PUBLISH:
        variableHeader.publish.topicNameLength = (rawVariableHeader[0] << 8) | rawVariableHeader[1];
        variableHeader.publish.topicName = new char[variableHeader.publish.topicNameLength];
        memcpy(variableHeader.publish.topicName, &rawVariableHeader[2], variableHeader.publish.topicNameLength);

        variableHeader.publish.packetID = (rawVariableHeader[variableHeader.publish.topicNameLength + 2] << 8) | rawVariableHeader[variableHeader.publish.topicNameLength + 3];

        variableHeader.size = 4 + variableHeader.publish.topicNameLength;
        break;

    case PUBACK:
        variableHeader.puback.packetID = (rawVariableHeader[0] << 8) | rawVariableHeader[1];

        variableHeader.size = 2;
        break;

    case SUBSCRIBE:
        variableHeader.subscribe.topicNameLength = (rawVariableHeader[0] << 8) | rawVariableHeader[1];
        variableHeader.subscribe.topicName = new char[variableHeader.subscribe.topicNameLength];
        memcpy(variableHeader.subscribe.topicName, &rawVariableHeader[2], variableHeader.subscribe.topicNameLength);

        variableHeader.subscribe.packetID = (rawVariableHeader[variableHeader.subscribe.topicNameLength + 2] << 8) | rawVariableHeader[variableHeader.subscribe.topicNameLength + 3];

        variableHeader.size = 4 + variableHeader.subscribe.topicNameLength;

        break;

    case SUBACK:
        variableHeader.suback.packetID = (rawVariableHeader[0] << 8) | rawVariableHeader[1];

        variableHeader.size = 2;
        break;

    default:
        variableHeader.size = 0;
        break;
    }

    return variableHeader;
}

void serializeVariableHeader(VariableHeader variableHeader, uint8_t *rawVariableHeader)
{
    if (rawVariableHeader == nullptr)
    {
        throw std::invalid_argument("rawVariableHeader cannot be null");
    }

    switch (variableHeader.controlPacketType)
    {
    case CONNECT:
        rawVariableHeader[0] = variableHeader.connect.protocolVersion;
        for (int i = 0; i < 16; i++)
        {
            rawVariableHeader[i + 1] = variableHeader.connect.uuid[i];
        }
        break;

    case CONNACK:
        rawVariableHeader[0] = variableHeader.connack.returnCode;
        break;

    case PUBLISH:
        rawVariableHeader[0] = variableHeader.publish.topicNameLength >> 8;
        rawVariableHeader[1] = variableHeader.publish.topicNameLength;
        memcpy(&rawVariableHeader[2], variableHeader.publish.topicName, variableHeader.publish.topicNameLength);

        rawVariableHeader[variableHeader.publish.topicNameLength + 2] = variableHeader.publish.packetID >> 8;
        rawVariableHeader[variableHeader.publish.topicNameLength + 3] = variableHeader.publish.packetID;
        break;

    case PUBACK:
        rawVariableHeader[0] = variableHeader.puback.packetID >> 8;
        rawVariableHeader[1] = variableHeader.puback.packetID;
        break;

    case SUBSCRIBE:
        rawVariableHeader[0] = variableHeader.subscribe.topicNameLength >> 8;
        rawVariableHeader[1] = variableHeader.subscribe.topicNameLength;
        memcpy(&rawVariableHeader[2], variableHeader.subscribe.topicName, variableHeader.subscribe.topicNameLength);

        rawVariableHeader[variableHeader.subscribe.topicNameLength + 2] = variableHeader.subscribe.packetID >> 8;
        rawVariableHeader[variableHeader.subscribe.topicNameLength + 3] = variableHeader.subscribe.packetID;
        break;

    case SUBACK:
        rawVariableHeader[0] = variableHeader.suback.packetID >> 8;
        rawVariableHeader[1] = variableHeader.suback.packetID;
        break;

    default:
        break;
    }
}

// Function to create a message
void serializeMessage(Message message, uint8_t *msg)
{
    serializeVariableHeader(message.variableHeader, msg + 2);

    size_t summedHeaderSize = message.variableHeader.size + 2;

    // Ensure the message does not exceed MAX_MESSAGE_SIZE
    if (message.payloadSize > MAX_MESSAGE_SIZE - summedHeaderSize)
    {
        message.payloadSize = MAX_MESSAGE_SIZE - summedHeaderSize;
    }

    message.header.packetLength = summedHeaderSize + message.payloadSize;
    serializeHeader(message.header, msg);

    memcpy(msg + summedHeaderSize, message.payload, message.payloadSize);
}

bool operator==(const Header &lhs, const Header &rhs)
{
    return lhs.controlPacketType == rhs.controlPacketType && lhs.isDuplicate == rhs.isDuplicate && lhs.qosLevel == rhs.qosLevel;
}

bool operator!=(const Header &lhs, const Header &rhs)
{
    return !(lhs == rhs);
}

void generateRandomMessage(Message msg)
{
    Header header = Header((ControlPacketType)random(validControlPackageValues.size()), (bool)random(0, 1), (bool)random(0, 1), (uint8_t)random(0, 1));

    Serial.println("###########");
    Serial.println("Header Coinflip: ");
    Serial.println(header.toString().c_str());
    Serial.println("######");

    VariableHeader variableHeader;
    variableHeader.controlPacketType = header.controlPacketType;

    char *topicName = nullptr;

    try
    {
        switch (variableHeader.controlPacketType)
        {
        case CONNECT:
        {
            variableHeader.connect.protocolVersion = random(0, 120);
            for (int i = 0; i < 16; i++)
            {
                variableHeader.connect.uuid[i] = (uint8_t)random(0, 255);
            }

            variableHeader.size = 17;
            break;
        }

        case CONNACK:
        {
            uint8_t options[] = {0, 1, 3, 5};
            variableHeader.connack.returnCode = options[random(3)];
            variableHeader.size = 1;
            break;
        }

        case PUBLISH:
        {
            variableHeader.publish.topicNameLength = random(0, 30);
            topicName = new char[variableHeader.publish.topicNameLength];
            variableHeader.publish.topicName = topicName;
            for (int i = 0; i < variableHeader.publish.topicNameLength; i++)
            {
                variableHeader.publish.topicName[i] = (char)random(255);
            }

            variableHeader.publish.packetID = (uint16_t)random(65536);

            variableHeader.size = 4 + variableHeader.publish.topicNameLength;
            break;
        }

        case PUBACK:
        {
            variableHeader.puback.packetID = (uint16_t)random(65536);

            variableHeader.size = 2;
            break;
        }

        case SUBSCRIBE:
        {
            variableHeader.subscribe.topicNameLength = random(0, 30);
            topicName = new char[variableHeader.subscribe.topicNameLength];
            variableHeader.subscribe.topicName = topicName;
            for (int i = 0; i < variableHeader.subscribe.topicNameLength; i++)
            {
                variableHeader.subscribe.topicName[i] = (char)random(255);
            }

            variableHeader.subscribe.packetID = (uint16_t)random(65536);

            variableHeader.size = 4 + variableHeader.subscribe.topicNameLength;
            break;
        }

        case SUBACK:
        {
            variableHeader.suback.packetID = (uint16_t)random(65536);

            variableHeader.size = 2;
            break;
        }

        default:
            variableHeader.size = 0;
            break;
        }
    }
    catch (std::bad_alloc &ba)
    {
        std::cerr << "bad_alloc caught: " << ba.what() << '\n';
        delete[] topicName;
        throw;
    }

    msg.payloadSize = random(20, 255);
    msg.payload[msg.payloadSize];
    for (int i = 0; i < msg.payloadSize; i++)
    {
        msg.payload[i] = random(65, 122);
    }
    msg.header = header;
    msg.variableHeader = variableHeader;

    delete[] topicName;
}