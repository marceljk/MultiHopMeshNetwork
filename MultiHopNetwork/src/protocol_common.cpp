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

    std::array<uint8_t, 16> uuid;
    for (int i = 0; i < 16; i++)
    {
        uuid[i] = (uint8_t)11;
    }

    auto variableHeader = std::unique_ptr<VariableHeader>(new ConnectHeader(0, uuid));

    return Message(header, std::move(variableHeader), "");
}

Message createConnectionMessage(const std::array<uint8_t, 16> uuid)
{
    printUUID(uuid, "create connection message");
    FixedHeader fixedHeader = FixedHeader(CONNECT, false, false, 1, 19);

    std::unique_ptr<VariableHeader> variableHeader = std::unique_ptr<VariableHeader>(new ConnectHeader(1, uuid));

    Message response = Message(fixedHeader, std::move(variableHeader), "");
    return response;
}

Message createConnackMessage(const Message &msg, const uint8_t networkID, const ConnackReturnCode returnCode)
{
    // Request Header
    ConnectHeader *connectHeader = static_cast<ConnectHeader *>(msg.variableHeader.get());

    FixedHeader fixedHeader = FixedHeader(CONNACK, false, msg.header.retain, msg.header.qosLevel, 4);

    auto variableHeader = std::unique_ptr<VariableHeader>(new ConnackHeader(returnCode, networkID, connectHeader->uuid));

    Message response = Message(fixedHeader, std::move(variableHeader), "");
    return response;
}
Message createPublishMessage(const std::string topicName, const uint16_t packetID, const std::string payload, const bool duplicate, const bool retain, const uint8_t qosLevel)
{

    auto variableHeader = std::unique_ptr<VariableHeader>(new PublishHeader(topicName.length(), topicName, packetID));

    FixedHeader fixedHeader = FixedHeader(PUBLISH, duplicate, retain, qosLevel, HEADER_SIZE + variableHeader->size);

    Message response = Message(fixedHeader, std::move(variableHeader), payload);
    return response;
}

Message createPubackMessage(const Message &msg)
{
    // Request Header
    PublishHeader *publishHeader = static_cast<PublishHeader *>(msg.variableHeader.get());

    std::unique_ptr<VariableHeader> variableHeader = std::unique_ptr<VariableHeader>(new PubackHeader(publishHeader->packetID));

    FixedHeader fixedHeader = FixedHeader(PUBACK, false, msg.header.retain, msg.header.qosLevel, variableHeader.get()->size);

    Message response = Message(fixedHeader, std::move(variableHeader), "");
    return response;
}

Message createSubscribeMessage(const std::string topicName, const uint16_t packetID, const bool duplicate, const bool retain, const uint8_t qosLevel)
{
    auto variableHeader = std::unique_ptr<VariableHeader>(new SubscribeHeader(topicName.length(), topicName, packetID));

    FixedHeader fixedHeader = FixedHeader(SUBSCRIBE, duplicate, retain, qosLevel, HEADER_SIZE + variableHeader->size);

    Message response = Message(fixedHeader, std::move(variableHeader), "");
    return response;
}

Message createSubackMessage(const Message &msg)
{
    SubscribeHeader *subscribeHeader = static_cast<SubscribeHeader *>(msg.variableHeader.get());

    std::unique_ptr<VariableHeader> variableHeader = std::unique_ptr<VariableHeader>(new SubackHeader(subscribeHeader->packetID));

    FixedHeader fixedHeader = FixedHeader(SUBACK, false, msg.header.retain, msg.header.qosLevel, variableHeader.get()->size);

    Message response = Message(fixedHeader, std::move(variableHeader), "");
    return response;
}

Message createDisconnectMessage(const std::array<uint8_t, 16> uuid)
{
    FixedHeader fixedHeader = FixedHeader(DISCONNECT, false, false, 0, 0);
    std::unique_ptr<VariableHeader> variableHeader = std::unique_ptr<VariableHeader>(new DisconnectHeader(uuid));
    Message response = Message(fixedHeader, std::move(variableHeader), "");
    return response;
}

void printUUID(std::array<uint8_t, 16> uuid, std::string location)
{
    char buffer[37]; // Buffer large enough to hold the UUID string plus the null terminator

    // Format the UUID
    sprintf(buffer, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
            uuid[0], uuid[1], uuid[2], uuid[3],
            uuid[4], uuid[5],
            uuid[6], uuid[7],
            uuid[8], uuid[9],
            uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]);
    Serial.print("uuid [special method] invoked at ");
    Serial.print(location.c_str());
    Serial.print(": ");
    Serial.println(buffer);
}
