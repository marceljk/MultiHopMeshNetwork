#ifndef CONFIG_H
#define CONFIG_H

#define GATEWAY_ADDRESS 1
#define INITIAL_NODE_ADDRESS 254

#define MAX_MESSAGE_SIZE 244
#define HEADER_SIZE 2

#define HARDCODED_NETWORK_ID 0

#define REGENERATING_UUID_EACH_START false

#define LLG_SCK 5
#define LLG_MISO 19
#define LLG_MOSI 27
#define LLG_CS 18
#define LLG_RST 16
#define LLG_DI0 26
#define LLG_DI1 35
#define LLG_DI2 34

static const uint8_t HARDCODED_UUID[16] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

#endif