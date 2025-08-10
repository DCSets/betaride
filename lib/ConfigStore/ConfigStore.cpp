#include "ConfigStore.h"

ConfigStore::ConfigStore()
{
    if (DEBUG)
        Serial.println("ConfigStore initialization started");

    _prefs.begin(NVS_NAMESPACE, false);

    this->loadMotorsConfig();
    this->loadBrushlessMotorsConfig();
    this->loadELRSConfig();
    this->loadControllerRulesConfig();

    if (DEBUG)
    {
        Serial.println("ConfigStore initialization finished");
        Serial.println("Brushless motors: ");
        this->printConfigs(_brushlessMotorsConfig, MAX_BRUSHLESS_MOTORS);
        Serial.println("Controller config: ");
        this->printConfigs(_elrsConfig, MAX_CONTROLLERS);
    }
}

void ConfigStore::cleanEntity(const char *entity, int maxCount)
{
    String ids[static_cast<size_t>(maxCount)];
    this->getIds(entity, ids, maxCount);
    for (int i = 0; i < maxCount; i++)
    {
        if (!ids[i].isEmpty())
        {
            _prefs.remove(ids[i].c_str());
        }
    }
    _prefs.putString(entity, "");
}

void ConfigStore::getIds(const char *entity, String *idsArray, int maxCount)
{
    String idsString = "";
    // Avoid noisy error logs for missing keys
    if (_prefs.isKey(entity))
    {
        idsString = _prefs.getString(entity, "");
    }

    // Clear the array first
    for (int i = 0; i < maxCount; i++)
    {
        idsArray[i] = "";
    }

    // Parse comma-separated string
    int index = 0;
    int startPos = 0;
    int commaPos = idsString.indexOf(',');

    while (commaPos != -1 && index < maxCount)
    {
        idsArray[index] = idsString.substring(startPos, commaPos);
        idsArray[index].trim(); // Remove whitespace
        startPos = commaPos + 1;
        commaPos = idsString.indexOf(',', startPos);
        index++;
    }

    // Get the last element if there's still content
    if (index < maxCount && startPos < idsString.length())
    {
        idsArray[index] = idsString.substring(startPos);
        idsArray[index].trim(); // Remove whitespace
    }
}

String ConfigStore::joinIds(const String *ids, int maxCount)
{
    String result = "";
    bool first = true;
    for (int i = 0; i < maxCount; i++)
    {
        if (ids[i].isEmpty())
        {
            break;
        }
        if (!first)
        {
            result += ",";
        }
        result += ids[i];
        first = false;
    }
    return result;
}

void ConfigStore::printResourcesConfgs()
{
    this->printConfigs(_brushlessMotorsConfig, MAX_BRUSHLESS_MOTORS);
    this->printConfigs(_motorsConfig, MAX_MOTORS);
}