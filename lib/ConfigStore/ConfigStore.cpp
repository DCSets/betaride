#include "ConfigStore.h"

ConfigStore::ConfigStore()
{
    if (DEBUG)
        Serial.println("ConfigStore initialization started");

    _prefs.begin(NVS_NAMESPACE, false);

    this->loadMotorsConfig();
    this->loadBrushlessMotorsConfig();
    this->loadController();
    this->loadControllerRulesConfig();

    if (DEBUG)
    {
        Serial.println("ConfigStore initialization finished");
        this->printResourcesConfgs();
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
    Serial.println("=== Brushless Motors ===");
    this->printConfigs(_brushlessMotorsConfig, MAX_BRUSHLESS_MOTORS);
    
    Serial.println("=== Motors ===");
    this->printConfigs(_motorsConfig, MAX_MOTORS);
    
    Serial.println("=== Controller Rules ===");
    this->printConfigs(_controllerRules, MAX_RULES);
    
    Serial.println("=== Controller ===");
    ControllerConfig* controllerConfig = this->getControllerConfig();
    if (controllerConfig != nullptr)
    {
        String json;
        controllerConfig->toJson(json);
        Serial.println(json);
    }
    else
    {
        Serial.println("No controller config found");
    }
    
    
}

ControllerConfig* ConfigStore::getControllerConfig()
{
    // Check if ELRS config exists (has non-empty id)
    if (strlen(_elrsConfig[0].id) > 0)
    {
        return &_elrsConfig[0];
    }
    
    // Check if PS5 config exists (has non-empty id)
    if (strlen(_ps5Config[0].id) > 0)
    {
        return &_ps5Config[0];
    }
    
    return nullptr;
}

void ConfigStore::loadController()
{
    // Use default constructors instead of memset to avoid vtable corruption
    _elrsConfig[0] = ELRSConfig();
    _ps5Config[0] = PS5ControllerConfig();

    String ids[1];
    this->getIds(TYPE_CONTROLLER, ids, 1);
    
    if (ids[0].isEmpty())
    {
        if (DEBUG)
            Serial.println("No controller id found");
        return;
    }

    if (DEBUG)
        Serial.printf("Loading controller config for %s\n", ids[0].c_str());
    
    // Load JSON string from NVS
    String jsonString = _prefs.getString(ids[0].c_str(), "");
    if (jsonString.isEmpty())
    {
        if (DEBUG)
            Serial.printf("No JSON data found for controller config %s\n", ids[0].c_str());
        return;
    }

    // Parse JSON to determine controller type
    JsonDocument doc;
    deserializeJson(doc, jsonString);

    // Check controller type and load appropriate config
    int controllerType = doc["controllerType"];
    
    if (controllerType == ControllerType::ELRS)
    {
        _elrsConfig[0] = ELRSConfig(jsonString);
    }
    else if (controllerType == ControllerType::PS5)
    {
        _ps5Config[0] = PS5ControllerConfig(jsonString);
    }
    else
    {
        if (DEBUG)
            Serial.printf("Unknown controller type: %d\n", controllerType);
        return;
    }

    if (DEBUG)
    {
        Serial.println("Controller config loaded: " + ids[0] + " (Type: " + controllerType + ")");
    }
}