#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <Arduino.h>

#include <string>
#include <set>
#include <memory>

#include <protocol_common.h>
#include <variable_headers.h>
#include <config.h>

// MUST BE UPDATED IF MORE CONTROL PACKET TYPES ARE ADDED
extern std::set<int> validControlPackageValues;

void serializeMessage(Message &message, uint8_t *serializedMessageLocation, size_t reservedSpace);
void serializeVariableHeader(VariableHeader &variableHeader, uint8_t *serializedVariableHeaderLocation, size_t reservedSpace);
void serializeHeader(FixedHeader &header, uint8_t *serializedHeaderLocation, size_t reservedSpace);

Message parseIncomingPacket(uint8_t *incomingPacket, size_t availableSpace);
FixedHeader parseFixedHeader(uint8_t *serializedHeader);
std::unique_ptr<VariableHeader> parseVariableHeader(ControlPacketType controlPacketType, uint8_t *serializedVariableHeader);

#endif
