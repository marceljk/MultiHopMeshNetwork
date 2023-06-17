#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <SSD1306.h>
#include <qrcode.h>

#include "config.h"

class DisplayHandler
{
public:
    DisplayHandler();
    void displayUUID(std::array<uint8_t, 16> &uuid);
    void displayInstructions(int timeLeft);
    void clearScreen();

private:
    SSD1306Wire display;
    QRCode qrcode;
};

#endif
