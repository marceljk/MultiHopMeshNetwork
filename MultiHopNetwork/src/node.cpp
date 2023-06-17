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
    DisplayHandler displayHandler;
    prefs.begin("network", false);

    if (USING_DEFAULT_UUID)
    {
        std::copy(HARDCODED_UUID, HARDCODED_UUID + 16, uuid.begin());
    }
    else if (!prefs.getBytesLength("uuid") || REGENERATING_UUID_EACH_START)
    {
        generateUUID(uuid.data());

        int timer = TIME_TILL_UUID_IS_SAVED;
        while (timer >= 0)
        {
            delay(1000);
            displayHandler.displayUUID(uuid);
            displayHandler.displayInstructions(timer);
            timer--;
        }
        prefs.putBytes("uuid", uuid.data(), 16);
    }
    else
    {
        prefs.getBytes("uuid", uuid.data(), 16);
    }
    prefs.end();

    network.setup();

    Message msg = createConnectionMessage(uuid);
    network.sendMessage(GATEWAY_ADDRESS, msg);

    displayHandler.clearScreen();
}

uint8_t res;

void loop()
{
    network.loop();

    if (millis() > nextMsgTime && acknowledged)
    {
        Message message = createPublishMessage("v1/backend/measurements", 1234, "lorem ipsum dolor sit amet oder so", false, false, 1);

        nextMsgTime += INTERVAL;

        try
        {
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

        if (connackHeader->returnCode == ACCEPTED && uuid == connackHeader->uuid)
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
