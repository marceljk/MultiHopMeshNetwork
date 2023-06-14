#ifndef MESH_NETWORK_H
#define MESH_NETWORK_H

#include <RHMesh.h>
#include <RH_RF95.h>
#include <SPI.h>
#include <set>

#include <protocol.h>
#include <config.h>

// Forward declaration of Message class
class Message;



class MeshNetwork
{
private:
    RH_RF95 rf95;
    RHMesh manager;
    std::function<void(Message &, uint8_t from)> handleMessage;

public:
    MeshNetwork(uint8_t networkAddress, std::function<void(Message &, uint8_t from)> handleMessage)
        : rf95(LLG_CS, LLG_DI0), manager(rf95, networkAddress), handleMessage(handleMessage) {}

    void setup();
    void loop();
    bool sendMessage(uint8_t to, Message &msg);
    void updateNetworkId(uint8_t newID);
};

#endif // MESH_NETWORK_H
