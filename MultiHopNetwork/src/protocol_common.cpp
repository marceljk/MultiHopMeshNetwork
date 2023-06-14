#include <protocol_common.h>

std::string Message::toString()
{
    std::string str = "Message:";
    str += "\n" + header.toString();
    str += "\n" + variableHeader->toString();
    if (header.controlPacketType == PUBLISH)
    {
        str += "\nPayload: ";
        str += payload;
    }

    return str;
}

std::string FixedHeader::toString()
{
    std::string str = "Fixed Header: ";

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

    auto variableHeader = std::unique_ptr<VariableHeader>(new ConnectHeader(0, uuid));

    return Message(header, std::move(variableHeader), "");
}

Message createConnectionMessage(uint8_t uuid[16])
{
    FixedHeader fixedHeader = FixedHeader(CONNECT, false, false, 1, 19);

    auto variableHeader = std::unique_ptr<VariableHeader>(new ConnectHeader(1, uuid));

    Message response = Message(fixedHeader, std::move(variableHeader), "");
    return response;
}

Message createConnackMessage(Message &msg, uint8_t networkID, ConnackReturnCode returnCode)
{
    // Request Header
    ConnectHeader *connectHeader = static_cast<ConnectHeader *>(msg.variableHeader.get());

    FixedHeader fixedHeader = FixedHeader(CONNACK, false, msg.header.retain, msg.header.qosLevel, 4);

    auto variableHeader = std::unique_ptr<VariableHeader>(new ConnackHeader(returnCode, networkID, connectHeader->uuid));

    Message response = Message(fixedHeader, std::move(variableHeader), "");
    return response;
}
Message createPublishMessage(std::string topicName, uint16_t packetID, bool duplicate, bool retain, uint8_t qosLevel)
{

    auto variableHeader = std::unique_ptr<VariableHeader>(new PublishHeader(topicName.length(), topicName, packetID));

    FixedHeader fixedHeader = FixedHeader(PUBLISH, duplicate, retain, qosLevel, HEADER_SIZE + variableHeader->size);

    Message response = Message(fixedHeader, std::move(variableHeader), "");
    return response;
}

Message createPubackMessage(Message &msg)
{
    // Request Header
    PublishHeader *publishHeader = static_cast<PublishHeader *>(msg.variableHeader.get());

    std::unique_ptr<VariableHeader> variableHeader = std::unique_ptr<VariableHeader>(new PubackHeader(publishHeader->packetID));

    FixedHeader fixedHeader = FixedHeader(PUBACK, false, msg.header.retain, msg.header.qosLevel, variableHeader.get()->size);

    Message response = Message(fixedHeader, std::move(variableHeader), "");
    return response;
}

Message createSubscribeMessage(std::string topicName, uint16_t packetID, bool duplicate, bool retain, uint8_t qosLevel)
{
    auto variableHeader = std::unique_ptr<VariableHeader>(new SubscribeHeader(topicName.length(), topicName, packetID));

    FixedHeader fixedHeader = FixedHeader(SUBSCRIBE, duplicate, retain, qosLevel, HEADER_SIZE + variableHeader->size);

    Message response = Message(fixedHeader, std::move(variableHeader), "");
    return response;
}

Message createSubackMessage(Message &msg)
{
    SubscribeHeader *subscribeHeader = static_cast<SubscribeHeader *>(msg.variableHeader.get());

    std::unique_ptr<VariableHeader> variableHeader = std::unique_ptr<VariableHeader>(new SubackHeader(subscribeHeader->packetID));

    FixedHeader fixedHeader = FixedHeader(SUBACK, false, msg.header.retain, msg.header.qosLevel, variableHeader.get()->size);

    Message response = Message(fixedHeader, std::move(variableHeader), "");
    return response;
}

Message createDisconnectMessage(uint8_t uuid[16])
{
    FixedHeader fixedHeader = FixedHeader(DISCONNECT, false, false, 0, 0);
    std::unique_ptr<VariableHeader> variableHeader = std::unique_ptr<VariableHeader>(new DisconnectHeader(uuid));
    Message response = Message(fixedHeader, std::move(variableHeader), "");
    return response;
}