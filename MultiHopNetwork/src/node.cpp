#include <RHMesh.h>
#include <RH_RF95.h>
#include <SPI.h>
#include <config.h>
#include <protocol.h>
#include <protocol_common.h>
#include <variable_headers.h>

#define TXINTERVAL 3000
unsigned long nextTxTime;

RH_RF95 rf95(LLG_CS, LLG_DI0);

RHMesh manager(rf95, NODE_ADDRESS);

Message message = dummyMessage();

void setup()
{
    Serial.begin(9600);
    Serial.print(F("initializing node "));
    Serial.print(NODE_ADDRESS);
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
    nextTxTime = millis();

    // testingHeaderParsing();

    message = dummyMessage();
}

uint8_t buf[RH_MESH_MAX_MESSAGE_LEN - 5];
uint8_t res;

void loop()
{

    if (millis() > nextTxTime)
    {
        Serial.println("---------------------------------");

        Serial.println(message.toString().c_str());
        serializeMessage(message, buf, RH_MAX_MESSAGE_LEN);

        Serial.println("\n raw: ");

        for (int j = 0; j < 20; j++)
        {
            uint8_t value = buf[j];
            for (int i = 7; i >= 0; i--)
            {
                Serial.print((value >> i) & 1);
            }
            Serial.println();
        }
        Serial.println("%%%%%%%%%%%%%%");

        nextTxTime += TXINTERVAL;
        Serial.print("Sending to bridge n.");
        Serial.print(GATEWAY_ADDRESS);
        Serial.print(" res=");
        // manager.sendto(buf, sizeof(buf), RH_BROADCAST_ADDRESS);
        res = manager.sendtoWait(buf, sizeof(buf), GATEWAY_ADDRESS);
        Serial.println(res);
        if (res == RH_ROUTER_ERROR_NONE)
        {
            // Data has been reliably delivered to the next node.
            // now we do...
        }
        else
        {
            // Data not delivered to the next node.
            Serial.println("sendtoWait failed. Are the bridge/intermediate mesh nodes running?");
        }
    }
    // nextTxTime += TXINTERVAL;
    // try
    // {
    //     testingHeaderParsing();
    // }
    // catch (std::invalid_argument)
    // {
    //     Serial.println("payload broken");
    // }

    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from))
    {
        Serial.print("message from node n.");
        Serial.print(from);
        Serial.print(": ");
        Serial.print((char *)buf);
        Serial.print(" rssi: ");
        Serial.println(rf95.lastRssi());
    }
}
