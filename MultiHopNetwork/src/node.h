#ifndef NODE_H
#define NODE_H

#include <RHMesh.h>
#include <RH_RF95.h>
#include <SPI.h>
#include <set>
#include <Preferences.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include "network.h"
#include "config.h"
#include "protocol.h"
#include "protocol_common.h"
#include "variable_headers.h"
#include "display.h"

void handle(Message &msg, uint8_t from);
void generateUUID(byte *uuid);

void setup();
void loop();

#endif
