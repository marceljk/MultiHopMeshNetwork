#include <variable_headers.h>

const std::string VariableHeader::toString()
{
    std::string str = "Variable Header: ";

    str += "Size: ";
    str += std::to_string(size);

    return str;
}

const std::string ConnectHeader::toString()
{
    std::string str = VariableHeader::toString();

    str += ", Protocol Version: ";
    str += std::to_string(protocolVersion);
    str += ", UUID in Hex: ";

    char buffer[37]; // Buffer large enough to hold the UUID string plus the null terminator

    // Format the UUID
    sprintf(buffer, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
            uuid[0], uuid[1], uuid[2], uuid[3],
            uuid[4], uuid[5],
            uuid[6], uuid[7],
            uuid[8], uuid[9],
            uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]);

    str += buffer;

    return str;
}

const std::string ConnackHeader::toString()
{
    std::string str = VariableHeader::toString();

    str += ", Return Code: ";
    str += std::to_string(returnCode);
    str += ", Network Address: ";
    str += std::to_string(networkID);
    str += ", UUID in Hex: ";

    char buffer[37]; // Buffer large enough to hold the UUID string plus the null terminator

    // Format the UUID
    sprintf(buffer, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
            uuid[0], uuid[1], uuid[2], uuid[3],
            uuid[4], uuid[5],
            uuid[6], uuid[7],
            uuid[8], uuid[9],
            uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]);

    str += buffer;
    return str;
}

const std::string PublishHeader::toString()
{
    std::string str = VariableHeader::toString();

    str += ", Topic Name Length: ";
    str += std::to_string(topicNameLength);
    str += ", Topic Name: ";
    str += topicName;
    str += ", Packet ID: ";
    str += std::to_string(packetID);
    return str;
}
const std::string PubackHeader::toString()
{
    std::string str = VariableHeader::toString();

    str += ", Packet ID: ";
    str += std::to_string(packetID);
    return str;
}
const std::string SubscribeHeader::toString()
{
    std::string str = VariableHeader::toString();

    str += ", Topic Name Length: ";
    str += std::to_string(topicNameLength);
    str += ", Topic Name: ";
    str += topicName;
    str += ", Packet ID: ";
    str += std::to_string(packetID);
    return str;
}
const std::string SubackHeader::toString()
{
    std::string str = VariableHeader::toString();

    str += ", Packet ID: ";
    str += std::to_string(packetID);
    return str;
}
const std::string DisconnectHeader::toString()
{
    std::string str = VariableHeader::toString();
    str += ", UUID in Hex: ";

    char buffer[37]; // Buffer large enough to hold the UUID string plus the null terminator

    // Format the UUID
    sprintf(buffer, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
            uuid[0], uuid[1], uuid[2], uuid[3],
            uuid[4], uuid[5],
            uuid[6], uuid[7],
            uuid[8], uuid[9],
            uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]);

    str += buffer;
    return str;
}

std::string controlPacketTypeToString(ControlPacketType ControlPacketType)
{
    switch (ControlPacketType)
    {
    case CONNECT:
        return "CONNECT";
    case CONNACK:
        return "CONNACK";
    case PUBLISH:
        return "PUBLISH";
    case PUBACK:
        return "PUBACK";
    case SUBSCRIBE:
        return "SUBSCRIBE";
    case SUBACK:
        return "SUBACK";
    case DISCONNECT:
        return "DISCONNECT";
    }
}
