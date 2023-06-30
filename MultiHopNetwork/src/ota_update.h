#ifndef OTAUPDATE_H
#define OTAUPDATE_H

#include "Update.h"

#include "protocol_common.h"

class OTAUpdate
{
  private:
    uint16_t expectedBlockIndex;
    uint16_t numberOfBlocks;
    uint16_t version;
    bool isRunning = false;

    void reset();

  public:
    OTAUpdate();

    bool begin(uint16_t numberOfBlocks, uint16_t version);
    bool write(UpdateBlock updateBlock);
    bool isUpdateRunning();
    u_int16_t getVersion();
    u_int16_t getExpectedBlockIndex();
    
    void abortUpdate();
};

#endif