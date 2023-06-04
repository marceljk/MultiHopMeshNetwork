#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <Arduino.h>

#include <iomanip>
#include <iostream>
#include <string>
#include <set>

#include <algorithm>
#include <protocol_common.h>
#include <variable_headers.h>
#define MAX_MESSAGE_SIZE 244
#define HEADER_SIZE 2

// MUST BE UPDATED IF MORE CONTROL PACKET TYPES ARE ADDED
extern std::set<int> validControlPackageValues;

void serializeMessage(Message &message, uint8_t *serializedMessageLocation, size_t reservedSpace);
void serializeVariableHeader(VariableHeader &variableHeader, uint8_t *serializedVariableHeaderLocation, size_t reservedSpace);
void serializeHeader(FixedHeader &header, uint8_t *serializedHeaderLocation, size_t reservedSpace);

Message parseIncomingPacket(uint8_t *incomingPacket, size_t availableSpace);
FixedHeader parseFixedHeader(uint8_t *serializedHeader);
VariableHeader parseVariableHeader(ControlPacketType controlPacketType, uint8_t *serializedVariableHeader);

// void generateRandomMessage(Message msg);

#endif // PROTOCOL_H
