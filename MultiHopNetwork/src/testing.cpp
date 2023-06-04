// // TESTING FILE AS I COULDN'T GET THE PLATFORMIO UNIT TESTS TO WORK

// #include <testing.h>
// #include <cstring>

// void testingHeaderParsing()
// {
//     uint8_t control_packet = 1 << 4;
//     uint8_t control_flags = 0b11110000;
//     uint8_t packet_length = 30;

//     uint8_t rawMessage[] = {0b00110001, packet_length};

//     Header header;
//     header.controlPacketType = CONNECT;
//     header.isDuplicate = false;
//     header.qosLevel = 0;

//     uint8_t serialized[2];
//     serializeHeader(header, serialized);
//     Header serialized_and_parsed_header = parseHeader(serialized);

//     Header parsed_header = parseHeader(rawMessage);

//     Serial.println("original: ");
//     Serial.println(header.toString().c_str());
//     Serial.println("fake: ");
//     Serial.println(serialized_and_parsed_header.toString().c_str());
// }

// void testingVariableHeaderParsing()
// {
//     VariableHeader variableHeader;
//     variableHeader.controlPacketType = CONNECT;
//     variableHeader.size = 17;
//     variableHeader.connect.protocolVersion = 1;
//     for (int i = 0; i < 16; i++)
//     {
//         variableHeader.connect.uuid[i] = (uint8_t)11;
//     }

//     uint8_t buf[255];

//     serializeVariableHeader(variableHeader, buf);
//     VariableHeader serializedHeader = parseVariableHeader(CONNECT, buf);

//     Serial.println("----------------");

//     Serial.println("original: ");
//     Serial.println(variableHeader.toString().c_str());
//     Serial.println("fake: ");
//     Serial.println(serializedHeader.toString().c_str());
// }

// void testingFullMessageParsing()
// {
//     Header header;
//     header.controlPacketType = CONNECT;
//     header.isDuplicate = false;
//     header.qosLevel = 0;

//     VariableHeader variableHeader;
//     variableHeader.controlPacketType = CONNECT;
//     variableHeader.size = 17;
//     variableHeader.connect.protocolVersion = 1;
//     for (int i = 0; i < 16; i++)
//     {
//         variableHeader.connect.uuid[i] = (uint8_t)11;
//     }

//     Message message;
//     message.header = header;
//     message.variableHeader = variableHeader;
//     const char *str = "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Donec quam felis,.";
//     size_t len = std::strlen(str);
//     uint8_t payload[len + 1];

//     for (size_t i = 0; i < len; i++)
//     {
//         payload[i] = static_cast<uint8_t>(str[i]);
//     }
//     payload[len] = '\0';

//     uint8_t buf[255];

//     serializeMessage(message, buf);
//     VariableHeader serializedMessage = parseVariableHeader(CONNECT, buf);

//     Serial.println("----------------");
//     Serial.println("original Message: ");
//     Serial.println(message.toString().c_str());
//     Serial.println("fake Message: ");
//     Serial.println(serializedMessage.toString().c_str());
// }