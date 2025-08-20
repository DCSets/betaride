#include "ConfigStore.h"

ConfigStore::ConfigStore()
{
    if (DEBUG)
        Serial.println("ConfigStore initialization started");

    _prefs.begin(NVS_NAMESPACE, false);

    this->loadMotorsConfig();
    this->loadBrushlessMotorsConfig();
    this->loadControllerConfig();
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

void ConfigStore::clearEntities()
{
    this->cleanEntity(TYPE_MOTORS, MAX_MOTORS);
    this->cleanEntity(TYPE_BRUSHLESS_MOTORS, MAX_BRUSHLESS_MOTORS);
    this->cleanEntity(TYPE_CONTROLLER, 1);
    this->cleanEntity(TYPE_CONTROLLER_RULES, MAX_RULES);
}

void ConfigStore::printResourcesConfgs()
{
    Serial.println("=== Brushless Motors ===");
    this->printConfigs(_brushlessMotorsConfig, MAX_BRUSHLESS_MOTORS);
    
    Serial.println("=== Motors ===");
    this->printConfigs(_motorsConfig, MAX_MOTORS);
    
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
    
    // It should be last because effect relay on other resources
    // TODO: probably the separate command/indicator that resources are printed should be added as well
    Serial.println("=== Controller Rules ===");
    this->printConfigs(_controllerRules, MAX_RULES);
}

void ConfigStore::saveControllerConfig(ControllerConfig *controllerConfig) {
    this->cleanEntity(TYPE_CONTROLLER, 1);
    String idsJoined;
    {
        ControllerConfig *cfg  = controllerConfig;
        if (idsJoined.length() > 0)
        {
            idsJoined += ",";
        }
        idsJoined += cfg->id;
        // Convert struct to JSON string and save it to prefs
        String jsonString;
        cfg->toJson(jsonString);
        if(DEBUG) {
            Serial.println("Saving config: " + String(cfg->id) + " " + jsonString);
        }
        _prefs.putString(cfg->id, jsonString);
    }

    _prefs.putString(TYPE_CONTROLLER, idsJoined);
    if (DEBUG)
        Serial.print(String(TYPE_CONTROLLER) + ". IDs: " + idsJoined);
}

ControllerConfig* ConfigStore::loadControllerConfig()
{
    String ids[1];
    this->getIds(TYPE_CONTROLLER, ids, 1);
    
    if (ids[0].isEmpty())
    {
        if (DEBUG)
            Serial.println("No controller id found");
        return _controllerConfig;
    }

    if (DEBUG)
        Serial.printf("Loading controller config for %s\n", ids[0].c_str());
    
    // Load JSON string from NVS
    String jsonString = _prefs.getString(ids[0].c_str(), "");
    if (jsonString.isEmpty())
    {
        if (DEBUG)
            Serial.printf("No JSON data found for controller config %s\n", ids[0].c_str());
        return _controllerConfig;
    }

    // Parse JSON to determine controller type
    JsonDocument doc;
    deserializeJson(doc, jsonString);

    // Check controller type and load appropriate config
    int controllerType = doc["controllerType"] | -1;
    
    if (controllerType == ControllerType::ELRS)
    {
        _controllerConfig = new ELRSConfig(jsonString);
    }
    else if (controllerType == ControllerType::PS5)
    {
        _controllerConfig = new PS5ControllerConfig(jsonString);
    }
    else
    {
        if (DEBUG)
            Serial.printf("Unknown controller type: %d\n", controllerType);

        return _controllerConfig;
    }

    if (DEBUG)
    {
        Serial.println("Controller config loaded: " + ids[0] + " (Type: " + controllerType + ")");
    }

    return _controllerConfig;
}