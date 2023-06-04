#include <protocol_common.h>

std::string Message::toString()
{
    std::string str = "Message: \n";
    str += "\n" + header.toString();
    str += "\n" + variableHeader.toString();
    str += "\nPayload: ";
    str += payload;
    return str;
}

std::string FixedHeader::toString()
{
    std::string str = "Fixed Header \n";

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

Message dummyMessage()
{
    FixedHeader header(CONNECT, false, false, 0, 20);

    uint8_t uuid[16];
    for (int i = 0; i < 16; i++)
    {
        uuid[i] = (uint8_t)11;
    }

    ConnectHeader variableHeader(17, CONNECT, 0, uuid);

    return Message(header, variableHeader, "");
}
