// #include "networking.h"
// #include <RH_RF95.h>

// // Singleton instance of the radio driver
// RH_RF95 rf95;

// Network::Network()
// {
//     // constructor
// }

// void Network::begin()
// {
//     if (!rf95.init())
//     {
//         // Handle error during initialization
//         Serial.println("LoRa radio init failed");
//         while (1)
//             ;
//     }
//     // Setup carrier frequency
//     if (!rf95.setFrequency(915.0))
//     { // frequency in MHz, adjust according to your region
//         Serial.println("setFrequency failed");
//         while (1)
//             ;
//     }
//     rf95.setTxPower(14, false); // Max power, second parameter is 'boost' mode
// }

// void Network::sendMessage(String destination, String message)
// {
//     // The destination node is typically embedded in the message according to the protocol
//     // Convert the message to a char array to send it
//     char msgToSend[message.length() + 1];
//     message.toCharArray(msgToSend, sizeof(msgToSend));
//     rf95.send((uint8_t *)msgToSend, sizeof(msgToSend));
//     rf95.waitPacketSent();
// }

// String Network::receiveMessage()
// {
//     if (rf95.available())
//     {
//         // Don't read unless there is a valid message
//         uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
//         uint8_t len = sizeof(buf);
//         if (rf95.recv(buf, &len))
//         {
//             // Message received and is in buf
//             return String((char *)buf);
//         }
//     }
//     return "";
// }

// bool Network::messageAvailable()
// {
//     return rf95.available();
// }
