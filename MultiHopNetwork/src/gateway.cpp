#include <RHMesh.h>
#include <RH_RF95.h>
#include <SPI.h>

#include <config.h>
#include <protocol.h>
#include <protocol_common.h>
#include <variable_headers.h>

RH_RF95 rf95(LLG_CS, LLG_DI0);

RHMesh manager(rf95, GATEWAY_ADDRESS);

void setup()
{
    Serial.begin(9600);
    Serial.print(F("initializing gateway with network id "));
    Serial.print(GATEWAY_ADDRESS);
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

uint8_t buf[MAX_MESSAGE_SIZE];
uint8_t res;

void loop()
{
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from))
    {
        Serial.print("message from node n.");
        Serial.print(from);
        Serial.print(": \n\n");
        try
        {
            Message msg = parseIncomingPacket(buf, MAX_MESSAGE_SIZE);
            Serial.print(msg.toString().c_str());
        }
        catch (std::invalid_argument e)
        {
            Serial.println(e.what());
        }
        Serial.println("\n raw (likely with a few null bytes):");

        for (int j = 0; j < MAX_MESSAGE_SIZE; j++)
        {
            uint8_t value = buf[j];
            for (int i = 7; i >= 0; i--)
            {
                Serial.print((value >> i) & 1);
            }
            Serial.println();
        }

        Serial.print(" rssi: ");
        Serial.println(rf95.lastRssi());
        Serial.println("------");
    }
}
