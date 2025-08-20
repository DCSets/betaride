#include "ConfiguratorSerial.h"

void ConfiguratorSerial::loop()
{
    if (Serial.available())
    {
        String input = Serial.readStringUntil('\n');
        String chunks[MAX_CHUNKS];

        int count = parseCommand(input, chunks, MAX_CHUNKS);

        if (count > 0)
        {
            return this->processCommand(chunks, count);
        }
    }
    if (this->isConnected() && !this->_configTransfer && this->_resources.size() > 0)
    {
        if (DEBUG)
        {
            Serial.println("Processing stored configs: ");
        }
        // Clear all entities before saving new ones to avoid complex update logic
        this->_store->clearEntities();
        
        static MotorConfig motors[MAX_MOTORS];
        static BrushlessMotorConfig brushlessMotors[MAX_BRUSHLESS_MOTORS];
        static ServoConfig servos[MAX_SERVOS];
        static ControllerRule controllerRules[MAX_RULES];

        int servosCount = 0;
        int motorsCount = 0;
        int brushlessCount = 0;
        int rulesCount = 0;

        memset(motors, 0, sizeof(motors));
        memset(brushlessMotors, 0, sizeof(brushlessMotors));
        memset(servos, 0, sizeof(servos));
        memset(controllerRules, 0, sizeof(controllerRules));

        static ControllerConfig *controllerConfig;
        bool hasControllerConfig = false;


        for (const auto &kv : this->_resources)
        {
            size_t pos = kv.first.find('_');
            if (pos == std::string::npos)
            {
                continue;
            }

            std::string type = kv.first.substr(0, pos);
            const char *jsonCStr = kv.second.c_str();
            String json = String(jsonCStr);

            Serial.printf("Processing stored configs: %s %s \n", type.c_str(), kv.first.c_str());
            if (DEBUG)
            {
                Serial.println("ID - " + String(kv.first.c_str()));
                Serial.println("Data - " + String(kv.second.c_str()));
            }

            if (type == TYPE_MOTORS && motorsCount < MAX_MOTORS)
            {
                motors[motorsCount++] = MotorConfig(json);
            }
            if (type == TYPE_BRUSHLESS_MOTORS && motorsCount < MAX_BRUSHLESS_MOTORS)
            {
                brushlessMotors[brushlessCount++] = BrushlessMotorConfig(json);
            }
            if (type == TYPE_SERVOS && servosCount < MAX_SERVOS)
            {
                servos[servosCount++] = ServoConfig(json);
            }
            if (type == TYPE_CONTROLLER && !hasControllerConfig)
            {
                JsonDocument doc;
                validateJsonHelper(json.c_str(), doc, "Controller config");
                int controllerTypeValue = doc["controllerType"] | -1;
                if (controllerTypeValue == ControllerType::ELRS)
                {
                    controllerConfig = new ELRSConfig(json);
                }
                if (controllerTypeValue == ControllerType::PS5)
                {
                    controllerConfig = new PS5ControllerConfig(json);
                }
                hasControllerConfig = true;
            }

            if (type == TYPE_CONTROLLER_RULES && rulesCount < MAX_RULES)
            {
                controllerRules[rulesCount++] = ControllerRule(json);
            }
        }

        if (motorsCount > 0)
            _store->saveMotorsConfig(motors, motorsCount);
        if (brushlessCount > 0)
            _store->saveBrushlessMotorsConfig(brushlessMotors, brushlessCount);
        if (servosCount > 0)
            _store->saveServosConfig(servos, servosCount);
        if (hasControllerConfig) {
            _store->saveControllerConfig(controllerConfig);
        }
        if (rulesCount > 0) {
            Serial.println("Saving rules");
            _store->saveControllerRulesConfig(controllerRules, rulesCount);
        }
        this->clear();
    }
}

void ConfiguratorSerial::processCommand(String chunks[], int count)
{
    String baseCommand = chunks[0];
    if (baseCommand == _CMD_SERIAL)
    {
        this->_isConnected = (chunks[1] == "1") ? true : false;
        if (this->_isConnected) {
            // Pong to let configurator know that the serial is connected
            Serial.printf("[%s@%s]\n", _CMD_SERIAL, _PONG_HASH );
        }
        if (DEBUG)
        {
            Serial.println("Serial connected: " + String(this->_isConnected));
        }
    }
    else if (baseCommand == _CMD_TRANSFER)
    {
        this->_configTransfer = (chunks[1] == "1") ? true : false;
        if (DEBUG)
        {
            Serial.println("Serial transfer: " + String(this->_configTransfer));
        }
    }
    else if (baseCommand == _CMD_CLEAR_RESOURCES)
    {
        this->_store->clearEntities();
        if (DEBUG)
        {
            Serial.println("Clear all resources");
        }
    }
    else if (baseCommand == _CMD_RESTART)
    {
        if (DEBUG)
        {
            Serial.println("Restarting...");
        }
        delay(500);
        ESP.restart();
    }
    else if (baseCommand == _CMD_REQUEST_RESOURCES)
    {
        if (DEBUG)
        {
            Serial.println("Requested resources:");
        }
        _store->printResourcesConfgs();
        Serial.printf("[%s@1]\n", this->_CMD_REQUEST_RESOURCES);
    }
    else if (baseCommand == _CMD_STORE_RESOURCES)
    {
        String resourceId = chunks[1];
        String resourceConfig = chunks[2];
        this->addResource(resourceId.c_str(), resourceConfig.c_str());
    }
    else if (baseCommand == _CMD_TEST_CONTROLLER)
    {
        this->_controllerTesting = (chunks[1] == "1") ? true : false;
        if (DEBUG)
        {
            Serial.println("Controller testing: " + String(this->_controllerTesting));
        }
    }
}

int ConfiguratorSerial::parseCommand(String received, String outputChunks[], int maxChunks)
{
    received.trim();

    if (!received.startsWith("[") || !received.endsWith("]"))
    {
        return 0;
    }

    received = received.substring(1, received.length() - 1);
    int chunkIndex = 0;

    while (received.length() > 0 && chunkIndex < maxChunks)
    {
        int sep = received.indexOf('@');
        if (sep == -1)
        {
            outputChunks[chunkIndex++] = received;
            break;
        }
        outputChunks[chunkIndex++] = received.substring(0, sep);
        received = received.substring(sep + 1);
    }

    return chunkIndex;
}

void ConfiguratorSerial::printElrsChannels(int channels[16])
{
    for (int i = 0; i < 16; i++)
    {
        // rx@CHANNEL_ID@CHANNEL_VALUE
        Serial.printf("[rx@%d@%d]\n", i + 1, channels[i]);
    }
}
