#include <RH_RF95.h>

#define SS 18
#define RST 14
#define DI0 26

RH_RF95 rf95(SS, DI0);

void setup()
{
  Serial.begin(9600);
  Serial.println("Initializing...");
  if (!rf95.init())
    Serial.println("Init failed");
  else
    Serial.println("Init succeeded");
}

void send()
{
  Serial.println("Sending...");
  uint8_t data[] = "Hello World!";
  rf95.send(data, sizeof(data));
  rf95.waitPacketSent();
  Serial.println("Packet sent!");
}

void receive()
{
  if (rf95.available())
  {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len))
    {
      Serial.print("Received: ");
      Serial.println((char *)buf);
    }
    else
    {
      Serial.println("Received message is corrupted.");
    }
  }
  else
  {
    Serial.println("No package received!");
  }
}

void loop()
{
  receive();

  // send();
  delay(1000);
}
