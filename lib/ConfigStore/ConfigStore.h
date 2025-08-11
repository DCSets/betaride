#ifndef CAR_CONFIG_H
#define CAR_CONFIG_H

#include <Arduino.h>
#include <Preferences.h>
#include <Motor.h>
#include <BrushlessMotor.h>
#include <Controller.h>
#include <ELRSController.h>
#include <PS5Controller.h>
#include <Servo.h>
#include <constants.h>
#include <ControllerRules.h>

// todo: add key to get active controller. currently elrs only supported
class ConfigStore
{
public:
    ConfigStore();

    template <typename T>
    void saveResources(T *resources, int count, const char *entity, int maxCount);
    template <typename T, size_t N>
    T *loadConfig(T (&configArray)[N], const char *entityKey, const char *debugName = "config");
    
    // Method to load controller config (supports different controller types)
    void loadController();

    void saveMotorsConfig(MotorConfig *motorsConfig, int count) { this->saveResources(motorsConfig, count, TYPE_MOTORS, MAX_MOTORS); };
    void saveBrushlessMotorsConfig(BrushlessMotorConfig *brushlessMotorsConfig, int count) { this->saveResources(brushlessMotorsConfig, count, TYPE_BRUSHLESS_MOTORS, MAX_BRUSHLESS_MOTORS); };
    void saveELRSConfig(ELRSConfig *elrsConfig, int count) { this->saveResources(elrsConfig, count, TYPE_CONTROLLER, 1); };
    void savePS5Config(PS5ControllerConfig *ps5Config, int count) { this->saveResources(ps5Config, count, TYPE_CONTROLLER, 1); };
    void saveControllerRulesConfig(ControllerRule *rules, int count) { this->saveResources(rules, count, TYPE_CONTROLLER_RULES, MAX_RULES); };

    
    MotorConfig *loadMotorsConfig() { return this->loadConfig(_motorsConfig, TYPE_MOTORS, "motors"); };
    BrushlessMotorConfig *loadBrushlessMotorsConfig() { return this->loadConfig(_brushlessMotorsConfig, TYPE_BRUSHLESS_MOTORS, "brushless motors"); };
    ELRSConfig *loadELRSConfig() { this->loadController(); return &_elrsConfig; };
    ControllerRule *loadControllerRulesConfig() { return this->loadConfig(_controllerRules, TYPE_CONTROLLER_RULES, "Controller rules"); };

    MotorConfig *getMotorsConfig() { return _motorsConfig; };
    BrushlessMotorConfig *getBrushlessMotorsConfig() { return _brushlessMotorsConfig; };
    ControllerRule *getControllerRulesConfig() { return _controllerRules; };

    ELRSConfig *getELRSConfig() { return &_elrsConfig; };
    PS5ControllerConfig *getPS5Config() { return &_ps5Config; };
    // Generic method to get the actual stored controller config
    ControllerConfig* getControllerConfig();

    void printResourcesConfgs();

private:
    Preferences _prefs;
    void cleanEntity(const char *entity, int maxCount);
    // Array members to store the data
    MotorConfig _motorsConfig[MAX_MOTORS];
    BrushlessMotorConfig _brushlessMotorsConfig[MAX_BRUSHLESS_MOTORS];
    ControllerRule _controllerRules[MAX_RULES];
    ELRSConfig _elrsConfig;  // Single ELRS config
    PS5ControllerConfig _ps5Config;  // Single PS5 config
    

    void getIds(const char *entity, String *idsArray, int maxCount);
    String joinIds(const String *ids, int maxCount);

    template <typename T>
    void printConfigs(T *configArray, int maxCount);
};

// ===== Template Implementations =====
// These must be visible to all translation units that use them
// to avoid undefined references during linking.

template <typename T>
inline void ConfigStore::saveResources(T *resources, int count, const char *entity, int maxCount)
{
    this->cleanEntity(entity, maxCount);

    // Build a comma-separated list of IDs while saving each resource payload
    String idsJoined;
    for (int i = 0; i < count && i < maxCount; i++)
    {
        T *cfg = &resources[i];

        if (idsJoined.length() > 0)
        {
            idsJoined += ",";
        }
        idsJoined += cfg->id;

        // Convert struct to JSON string
        String jsonString;
        cfg->toJson(jsonString);

        // Save JSON string instead of binary data
        _prefs.putString(cfg->id, jsonString);
    }

    _prefs.putString(entity, idsJoined);
    if (DEBUG)
        Serial.print(String(entity) + ". IDs: " + idsJoined);
}

template <typename T, size_t N>
inline T *ConfigStore::loadConfig(T (&configArray)[N], const char *entityKey, const char *debugName)
{
    memset(configArray, 0, sizeof(configArray));

    String ids[N];
    this->getIds(entityKey, ids, N);
    if (DEBUG)
    {
        Serial.println("Loading config: " + String(debugName) + " ids: " + this->joinIds(ids, N));
    }

    for (size_t i = 0; i < N; ++i)
    {
        if (ids[i].isEmpty())
            continue;

        if (DEBUG)
            Serial.printf("Loading %s config for %s\n", debugName, ids[i].c_str());
        // Load JSON string from NVS
        String jsonString = _prefs.getString(ids[i].c_str(), "");
        if (jsonString.isEmpty())
        {
            if (DEBUG)
                Serial.printf("No JSON data found for %s %s\n", debugName, ids[i].c_str());

            continue;
        }

        // Parse JSON and create config object
        T config(jsonString);

        // Copy the parsed config to the array
        configArray[i] = config;
    }

    if (DEBUG)
    {
        Serial.println("Config loaded: " + String(debugName) + " ids: " + this->joinIds(ids, N));
    }

    return configArray;
}

// Helper to print arrays of configs that have `id` and `toJson(String&)`
template <typename T>
inline void ConfigStore::printConfigs(T *configArray, int maxCount)
{
    for (int i = 0; i < maxCount; i++)
    {
        if (strlen(configArray[i].id) > 0)
        {
            String json;
            configArray[i].toJson(json);
            Serial.println(json);
        }
    }
}

#endif