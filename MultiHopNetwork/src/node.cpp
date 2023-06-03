#include <RHMesh.h>
#include <RH_RF95.h>
#include <SPI.h>
#include <config.h>
#include <protocol.h>
#include <testing.h>

#define TXINTERVAL 3000
unsigned long nextTxTime;

RH_RF95 rf95(LLG_CS, LLG_DI0);

RHMesh manager(rf95, NODE_ADDRESS);

Message message;

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

    testingHeaderParsing();
}

uint8_t buf[RH_MESH_MAX_MESSAGE_LEN];
uint8_t res;

void dummyMessage()
{
    Header header;
    header.controlPacketType = CONNECT;
    header.isDuplicate = false;
    header.qosLevel = 0;

    VariableHeader variableHeader;
    variableHeader.controlPacketType = CONNECT;
    variableHeader.size = 17;
    variableHeader.connect.protocolVersion = 1;
    for (int i = 0; i < 16; i++)
    {
        variableHeader.connect.uuid[i] = (uint8_t)11;
    }

    message.header = header;
    message.variableHeader = variableHeader;
}

void loop()
{

    if (millis() > nextTxTime)
    {
        Serial.println("---------------------------------");
        uint8_t payload[MAX_MESSAGE_SIZE];
        try
        {
            dummyMessage();
            Serial.println(message.toString().c_str());
            serializeMessage(message, payload);

            Serial.println("\n raw: ");

            for (int j = 0; j < 20; j++)
            {
                uint8_t value = payload[j];
                for (int i = 7; i >= 0; i--)
                {
                    Serial.print((value >> i) & 1);
                }
                Serial.println(); // Print a newline character after each element
            }
            Serial.println("%%%%%%%%%%%%%%");
        }
        catch (std::invalid_argument)
        {
            Serial.println("payload broken");
        }
        nextTxTime += TXINTERVAL;
        Serial.print("Sending to bridge n.");
        Serial.print(GATEWAY_ADDRESS);
        Serial.print(" res=");

        res = manager.sendtoWait(payload, sizeof(payload), GATEWAY_ADDRESS);
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
        // nextTxTime += TXINTERVAL;
        // try
        // {
        //     testingHeaderParsing();
        // }
        // catch (std::invalid_argument)
        // {
        //     Serial.println("payload broken");
        // }
    }

    // uint8_t len = sizeof(buf);
    // uint8_t from;
    // if (manager.recvfromAck(buf, &len, &from))
    // {
    //     Serial.print("message from node n.");
    //     Serial.print(from);
    //     Serial.print(": ");
    //     Serial.print((char *)buf);
    //     Serial.print(" rssi: ");
    //     Serial.println(rf95.lastRssi());
    // }
}
