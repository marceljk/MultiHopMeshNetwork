#ifndef GATEWAY_H
#define GATEWAY_H

#include <RHMesh.h>
#include <RH_RF95.h>
#include <SPI.h>
#include <set>
#include <cstring>
#include <array>
#include <unordered_map>
#include <vector>

#include "network.h"
#include "config.h"
#include "protocol.h"
#include "protocol_common.h"

#include "variable_headers.h"

class Node
{
public:
    std::array<uint8_t, 16> uuid;
    uint8_t networkId;

    Node(const std::array<uint8_t, 16> uuid, uint8_t networkId);
};

void addNode(const std::array<uint8_t, 16> uuid, uint8_t networkId);
std::vector<Node *> getNodes(uint8_t networkId);
void deleteNode(uint8_t networkId, const std::array<uint8_t, 16> &uuid);

void handle(Message &msg, uint8_t from);

void setup();
void loop();

#endif
