#include "node.h"

#define INTERVAL 8000
unsigned long nextMsgTime;

MeshNetwork network(HARDCODED_NETWORK_ID ? HARDCODED_NETWORK_ID : INITIAL_NODE_ADDRESS, handle);

std::array<uint8_t, 16> uuid;
uint8_t networkID;

bool acknowledged = false;

void setup()
{
    Preferences prefs;
    prefs.begin("network", false);

    if (HARDCODED_UUID != nullptr)
    {
        std::copy(HARDCODED_UUID, HARDCODED_UUID + 16, uuid.begin());
    }
    else if (!prefs.getBytesLength("uuid") || REGENERATING_UUID_EACH_START)
    {
        generateUUID(uuid.data()); // Assuming generateUUID takes a pointer to the start of the UUID array
        prefs.putBytes("uuid", uuid.data(), uuid.size());
    }
    else
    {
        prefs.getBytes("uuid", uuid.data(), 16);
    }

    network.setup();

    printUUID(uuid, "first init");

    Message msg = createConnectionMessage(uuid);
    network.sendMessage(GATEWAY_ADDRESS, msg);

    prefs.end();
}

// Message message = createSubscribeMessage("v1/backend/measurements", 1234);
uint8_t buf[RH_MESH_MAX_MESSAGE_LEN - 5];
uint8_t res;

void loop()
{
    network.loop();

    if (millis() > nextMsgTime && acknowledged)
    {
        // Message message = createConnectionMessage(uuid);
        Message message = createPublishMessage("v1/backend/measurements", 1234, "lorem ipsum dolor sit amet oder so");
        nextMsgTime += INTERVAL;

        try
        {
            printUUID(uuid, "main loop ");
            serializeMessage(message, buf, RH_MAX_MESSAGE_LEN);
            network.sendMessage(GATEWAY_ADDRESS, message);
        }
        catch (std::exception e)
        {
            Serial.println(e.what());
        }
    }
}

void handle(Message &msg, uint8_t from)
{
    switch (msg.variableHeader->controlPacketType)
    {
    case CONNACK:
    {
        ConnackHeader *connackHeader = static_cast<ConnackHeader *>(msg.variableHeader.get());

        if (connackHeader->returnCode == ACCEPTED) // && uuid == connackHeader->uuid
        {
            Preferences prefs;
            if (HARDCODED_NETWORK_ID)
            {
                network.updateNetworkId(HARDCODED_NETWORK_ID);
            }
            else
            {
                network.updateNetworkId(connackHeader->networkID);
            }
            acknowledged = true;
        }
        break;
    }
    case PUBLISH:
    {
        PublishHeader *publishHeader = static_cast<PublishHeader *>(msg.variableHeader.get());

        // TODO: DO SOMETHING WITH RECEIVED MESSAGE (not relevant for our project)
        break;
    }
    case PUBACK:
    {
        PubackHeader *pubackHeader = static_cast<PubackHeader *>(msg.variableHeader.get());

        // TODO: MARK MESSAGE AS RECEIVED ON GATEWAY (or similar. Not relevant for our project)
        break;
    }
    case SUBACK:
    {
        SubackHeader *subackHeader = static_cast<SubackHeader *>(msg.variableHeader.get());

        // TODO: MARK TOPIC AS SUCCESSFULLY SUBSCRIBED (or similar. Not relevant for our project)
        break;
    }
    default:
        Serial.println("Something else happened:");
        Serial.println(msg.toString().c_str());
        break;
    }
}

void generateUUID(byte *uuid)
{
    for (int i = 0; i < 16; i++)
    {
        uuid[i] = esp_random() % 256;
    }
}
