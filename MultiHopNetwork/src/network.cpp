#include "network.h"

void MeshNetwork::setup()
{
    Serial.begin(9600);
    Serial.print(F("initializing with network id "));
    SPI.begin(LLG_SCK, LLG_MISO, LLG_MOSI, LLG_CS);
    if (!manager.init())
    {
        Serial.println(" init failed");
    }
    else
    {
        Serial.println(" done");
    }

    rf95.setTxPower(10, false);
    rf95.setFrequency(868.0);
    rf95.setCADTimeout(500);

    if (!rf95.setModemConfig(RH_RF95::Bw125Cr45Sf128))
    {
        Serial.println(F("set config failed"));
    }
    Serial.println("RF95 ready");
}

void MeshNetwork::loop()
{
    uint8_t buf[MAX_MESSAGE_SIZE];
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from))
    {
        Serial.print("This is node ");
        Serial.print(manager.thisAddress());
        Serial.print(".\nReceived message from node ");
        Serial.print(from);
        Serial.println(": \n");
        try
        {
            Message msg = parseIncomingPacket(buf, len);
            Serial.println(msg.toString().c_str());
            Serial.print("rssi: ");
            Serial.println(rf95.lastRssi());
            Serial.println("----------");
            handleMessage(msg, from);
        }
        catch (std::invalid_argument e)
        {
            Serial.println("Parsing failed with the following error:");
            Serial.println(e.what());
        }
    }
}

bool MeshNetwork::sendMessage(uint8_t to, Message &message)
{
    uint8_t serializedMessage[MAX_MESSAGE_SIZE];
    serializeMessage(message, serializedMessage, MAX_MESSAGE_SIZE);

    Serial.print("This is node ");
    Serial.print(manager.thisAddress());

    Serial.print(".\nSending Message to node: ");
    Serial.print(to);
    Serial.print(".\n");

    Serial.println(message.toString().c_str());

    Serial.println("----------");

    return manager.sendtoWait(serializedMessage, message.header.packetLength, to) == RH_ROUTER_ERROR_NONE;
}

void MeshNetwork::updateNetworkId(uint8_t newId)
{
    manager.setThisAddress(newId);
}
