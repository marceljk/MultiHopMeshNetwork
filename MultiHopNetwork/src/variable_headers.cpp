#include <variable_headers.h>

std::string VariableHeader::toString()
{
    std::string str = "Variable Header: ";

    str += "Size: ";
    str += std::to_string(size);
    str += ", Control Packet Type: ";
    str += std::to_string(static_cast<int>(controlPacketType));

    return str;
}

std::string ConnectHeader::toString()
{
    std::string str = VariableHeader::toString();

    str += ", Protocol Version: ";
    str += std::to_string(protocolVersion);
    str += ", UUID: ";

    std::stringstream ss;
    for (int i = 0; i < 16; ++i)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(uuid[i]);
    }
    str += ss.str();

    return str;
}

std::string ConnackHeader::toString()
{
    std::string str = VariableHeader::toString();

    str += ", Return Code: ";
    str += std::to_string(returnCode);
    str += ", Network Address: ";
    str += std::to_string(networkID);
    // str += ", UUID: ";
    // for (int i = 0; i < 16; ++i)
    //     str += std::to_string(uuid[i]);
    return str;
}

std::string PublishHeader::toString()
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
std::string PubackHeader::toString()
{
    std::string str = VariableHeader::toString();

    str += ", Packet ID: ";
    str += std::to_string(packetID);
    return str;
}
std::string SubscribeHeader::toString()
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
std::string SubackHeader::toString()
{
    std::string str = VariableHeader::toString();

    str += ", Packet ID: ";
    str += std::to_string(packetID);
    return str;
}
std::string DisconnectHeader::toString()
{
    std::string str = VariableHeader::toString();
    str += ", UUID: ";
    for (int i = 0; i < 16; ++i)
        str += std::to_string(uuid[i]);
    return str;
}
