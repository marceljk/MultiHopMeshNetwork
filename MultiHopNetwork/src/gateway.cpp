#include "gateway.h"

MeshNetwork gatewayNetwork(GATEWAY_ADDRESS, handle);
std::unordered_multimap<uint8_t, Node> connectedNodes;

void handle(Message &msg, uint8_t from)
{
    switch (msg.variableHeader->controlPacketType)
    {
    case CONNECT:
    {
        ConnectHeader *connectHeader = static_cast<ConnectHeader *>(msg.variableHeader.get());

        uint8_t networkId = connectedNodes.size() + 2;
        addNode(connectHeader->uuid, networkId);

        Message ackMessage = createConnackMessage(msg, networkId);
        gatewayNetwork.sendMessage(from, ackMessage);
        break;
    }

    case PUBLISH:
    {
        PublishHeader *publishHeader = static_cast<PublishHeader *>(msg.variableHeader.get());

        if (msg.header.qosLevel == 1)
        {
            Message ackMessage = createConnackMessage(msg, from);
            gatewayNetwork.sendMessage(from, ackMessage);
        }

        // TODO @GateWay: DO SOMETHING WITH RECEIVED MESSAGE
        break;
    }

    case PUBACK:
    {
        PubackHeader *pubackHeader = static_cast<PubackHeader *>(msg.variableHeader.get());

        // TODO @noone: MARK MESSAGE AS RECEIVED ON NODE (or similar. Not relevant for our project)

        break;
    }

    case SUBSCRIBE:
    {
        std::vector<Node *> nodes = getNodes(from);

        // TODO @noone: SUBSCRIBE NODES TO TOPICS (not relevant for our project)

        Message ackMessage = createSubackMessage(msg);
        gatewayNetwork.sendMessage(from, ackMessage);
        break;
    }

    case DISCONNECT:
        DisconnectHeader *disconnectHeader = static_cast<DisconnectHeader *>(msg.variableHeader.get());

        deleteNode(from, disconnectHeader->uuid);
        break;
    }
}

void setup()
{
    gatewayNetwork.setup();
}

void loop()
{
    gatewayNetwork.loop();
}

// Node class constructor definition
Node::Node(const std::array<uint8_t, 16> uuid, uint8_t networkId)
    : networkId(networkId), uuid(uuid)
{
}

// Function to add a node to the map
void addNode(const std::array<uint8_t, 16> uuid, uint8_t networkId)
{
    connectedNodes.emplace(networkId, Node(uuid, networkId));
}

// Function to get all nodes with a given network ID
std::vector<Node *> getNodes(uint8_t networkId)
{
    std::vector<Node *> result;
    auto range = connectedNodes.equal_range(networkId);
    for (auto i = range.first; i != range.second; ++i)
    {
        result.push_back(&(i->second));
    }
    return result;
}

// Function to delete a node with a given network ID and UUID
void deleteNode(uint8_t networkId, const std::array<uint8_t, 16> &uuid)
{
    auto range = connectedNodes.equal_range(networkId);
    for (auto i = range.first; i != range.second; ++i)
    {
        if (i->second.uuid == uuid)
        {
            connectedNodes.erase(i);
            break;
        }
    }
}
