#include "gateway.h"

MeshNetwork gatewayNetwork(GATEWAY_ADDRESS, handle);
Gateway gateway;
DisplayHandler displayHandler;

#define INTERVAL 500
unsigned long nextMsgTime;
File firmware;
bool completeSend = false;

void handle(Message &msg, uint8_t from)
{
  displayHandler.displayMessage(true, from, gatewayNetwork.getCurrentNetworkId(), msg);
  displayHandler.displayNodeId(gatewayNetwork.getCurrentNetworkId());

  switch (msg.variableHeader->controlPacketType)
  {
  case CONNECT:
  {
    ConnectHeader *connectHeader = static_cast<ConnectHeader *>(msg.variableHeader.get());
    uint8_t networkID = gateway.getNextNetworkID();
    gateway.addNode(gateway.getNextNetworkID(), connectHeader->uuid);
    Message ackMessage = createConnackMessage(msg, networkID);
    gatewayNetwork.sendMessage(from, ackMessage);
    break;
  }

  case PUBLISH:
  {
    PublishHeader *publishHeader = static_cast<PublishHeader *>(msg.variableHeader.get());

    if (msg.header.qosLevel == 1)
    {
      Message ackMessage = createPubackMessage(msg);
      gatewayNetwork.sendMessage(from, ackMessage);
    }

    // TODO @GateWay: DO SOMETHING WITH RECEIVED MESSAGE

    // Topic Name:
    std::string topicName = publishHeader->topicName;
    // Payload:
    std::string payload = msg.payload;

    // NetworkID (dont think you need it but whatever):
    uint8_t networkID = from;
    // UUID:
    std::array<uint8_t, 16> uuid = gateway.getUUIDByNetworkID(networkID);

    if (topicName == "v1/updates/missing")
    {
      Serial.println("requested Packet!");

      DynamicJsonDocument doc(255);
      const char *charArrayPayload = payload.c_str();
      deserializeJson(doc, charArrayPayload);
      uint16_t missingBlockIndex = doc["content"]["missingBlockIndex"];
      size_t newPosition = missingBlockIndex * UPDATE_PACKET_SIZE;
      Serial.print("missingBlockIndex");
      Serial.println(missingBlockIndex);
      Serial.println(newPosition);
      firmware.seek(newPosition);
      // TODO @UPDATE-GROUP: Handle the missing packet request.
    }

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
    SubscribeHeader *subscribeHeader = static_cast<SubscribeHeader *>(msg.variableHeader.get());

    // TODO @noone: SUBSCRIBE NODES TO TOPICS (not relevant for our project)

    Message ackMessage = createSubackMessage(msg);
    gatewayNetwork.sendMessage(from, ackMessage);
    break;
  }

  case DISCONNECT:
    DisconnectHeader *disconnectHeader = static_cast<DisconnectHeader *>(msg.variableHeader.get());

    gateway.deleteNode(from);
    break;
  }
}

void broadcastUpdate(UpdateBlock updateBlock)
{
  gatewayNetwork.broadcastUpdateBlock(updateBlock);
}

void sendUpdate()
{
  uint16_t numberOfBlocks = std::ceil((float)firmware.size() / (float)UPDATE_PACKET_SIZE);

  uint16_t blockIndex = std::ceil((float)firmware.position() / (float)UPDATE_PACKET_SIZE);
  uint8_t chunk[UPDATE_PACKET_SIZE];
  size_t bytesRead = firmware.read(chunk, UPDATE_PACKET_SIZE);

  if (bytesRead != UPDATE_PACKET_SIZE) completeSend = true;
  std::vector<uint8_t> blockContent(std::begin(chunk), std::end(chunk));
  UpdateBlock block = createUpdateBlock(2, numberOfBlocks, blockIndex, blockContent);
  broadcastUpdate(block);
}

void setup()
{
  gatewayNetwork.setup();
  displayHandler.init();
  displayHandler.displayNodeId(gatewayNetwork.getCurrentNetworkId());
  SPIFFS.begin();
  firmware = SPIFFS.open("/firmware.bin", "r");
}

void loop()
{
  gatewayNetwork.loop();

  if (millis() > nextMsgTime)
  {
    // TODO @UPDATE-GROUP: Send update.
    sendUpdate();
    /*std::string content = "Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque laudantium, totam rem aperiam, eaque ipsa quae ab illo inventore veritatis et quasi architecto beatae vitae dicta sunt explicabo. Nemo enim ipsam volup";
    std::vector<uint8_t> contentVector(content.begin(), content.end());
    UpdateBlock block = createUpdateBlock(1, 2, 0, contentVector);
    broadcastUpdate(block);*/
    nextMsgTime = millis() + INTERVAL;
  }
}

Gateway::Gateway() : nextNetworkID(1) {}

bool Gateway::addNode(uint8_t networkID, std::array<uint8_t, 16> uuid)
{
  for (auto it = connectedNodes.begin(); it != connectedNodes.end(); ++it)
  {
    if (it->second == uuid)
    {
      connectedNodes.erase(it);
      break;
    }
  }

  if (connectedNodes.find(networkID) != connectedNodes.end())
  {
    return false;
  }
  connectedNodes[networkID] = uuid;
  while (connectedNodes.find(nextNetworkID) != connectedNodes.end())
  {
    nextNetworkID++;
  }
  return true;
}

uint8_t Gateway::getNextNetworkID() const
{
  return nextNetworkID;
}

bool Gateway::deleteNode(uint8_t networkID)
{
  auto it = connectedNodes.find(networkID);
  if (it == connectedNodes.end())
  {
    return false;
  }

  connectedNodes.erase(it);
  if (networkID < nextNetworkID)
  {
    nextNetworkID = networkID;
  }

  return true;
}

int Gateway::getConnectedNodesCount()
{
  return connectedNodes.size();
}

std::array<uint8_t, 16> Gateway::getUUIDByNetworkID(uint8_t networkID)
{
  return connectedNodes[networkID];
}