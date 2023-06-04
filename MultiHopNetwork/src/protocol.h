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
#include <vector>
#define MAX_MESSAGE_SIZE 244
#define HEADER_SIZE 2

// MUST BE UPDATED IF MORE CONTROL PACKET TYPES ARE ADDED
extern std::set<int> validControlPackageValues;

std::vector<uint8_t> serializeMessage(Message &message);
std::vector<uint8_t> serializeHeader(FixedHeader &header);
std::vector<uint8_t> serializeVariableHeader(VariableHeader &variableHeader);

Message parseMessage(std::vector<uint8_t> &rawMessage);
FixedHeader parseHeader(std::vector<uint8_t> &rawHeader);
VariableHeader parseVariableHeader(ControlPacketType controlPacketType, std::vector<uint8_t> &rawVariableHeader);

// void generateRandomMessage(Message msg);

#endif // PROTOCOL_H
