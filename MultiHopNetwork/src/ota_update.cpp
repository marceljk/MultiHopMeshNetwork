#include "ota_update.h"

OTAUpdate::OTAUpdate(){};

bool OTAUpdate::begin(uint16_t numberOfBlocks, uint16_t version)
{
  if (version <= CURRENT_VERSION) return false;

  bool beginUpdate = Update.begin();

  if (beginUpdate)
  {
    OTAUpdate::numberOfBlocks = numberOfBlocks;
    OTAUpdate::version = version;
    OTAUpdate::expectedBlockIndex = 0;
    OTAUpdate::isRunning = true;
    Update.printError(Serial);
  }
  return beginUpdate;
}

/**
 * @return true, if writting of updateBlock was successful. false, if updateBlock had not expectedBlockIndex or version.
 */
bool OTAUpdate::write(UpdateBlock updateBlock)
{
  if(!isRunning) return false;
  bool isExpectedVersion = OTAUpdate::version == updateBlock.versionNumber;
  bool isExpectedBlockIndex = OTAUpdate::expectedBlockIndex == updateBlock.blockIndex;

  if (!isExpectedBlockIndex || !isExpectedVersion)
  {
    return false;
  }

  Update.write(updateBlock.blockContent.data(), updateBlock.blockContent.size());

  if (OTAUpdate::expectedBlockIndex == numberOfBlocks)
  {
    if (Update.end(true))
    {
      Serial.println("Update Success!");
      ESP.restart();
    }
    else
    {
      Update.printError(Serial);
      OTAUpdate::reset();
    }
  }

  OTAUpdate::expectedBlockIndex++;
  return true;
}

bool OTAUpdate::isUpdateRunning()
{
  return OTAUpdate::isRunning;
}

uint16_t OTAUpdate::getVersion()
{
  return OTAUpdate::version;
}

uint16_t OTAUpdate::getExpectedBlockIndex()
{
  return OTAUpdate::expectedBlockIndex;
}

void OTAUpdate::abortUpdate()
{
  Update.abort();
  OTAUpdate::reset();
}

void OTAUpdate::reset()
{
  OTAUpdate::numberOfBlocks = 0;
  OTAUpdate::version = 0;
  OTAUpdate::expectedBlockIndex = 0;
  OTAUpdate::isRunning = false;
}
