#ifndef BR_PS5_CONTROLLER_H
#define BR_PS5_CONTROLLER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <enums.h>
#include <ConfiguratorHelpers.h>
#include <structs.h>
#include <Controller.h>
#include <ps5Controller.h>

struct PS5ControllerConfig : public ControllerConfig
{
    String address;
    PS5ControllerConfig() = default;
    PS5ControllerConfig(String json)
    {
        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, json);

        if (err)
        {
            Serial.println(F("JSON parse error - PS5 controller config"));
            return;
        }

        this->isNew = false;
        this->controllerType = ControllerType::PS5;
        strncpy(this->id, doc["id"], sizeof(this->id));
        strncpy(this->name, doc["name"], sizeof(this->name));
        strncpy(this->type, doc["type"], sizeof(this->type));

        this->address = doc["address"] | "";
    }

    void toJson(String &outJson) const
    {
        JsonDocument doc;

        doc["id"] = this->id;
        doc["name"] = this->name;
        doc["controllerType"] = this->controllerType;
        doc["type"] = this->type;
        doc["address"] = this->address;
        doc["isNew"] = this->isNew;

        serializeJson(doc, outJson);
    }
};

class PS5_Controller : public Controller
{
private:
    ps5Controller ps5;
    PS5ControllerConfig _config;
    boolean _ready = false;
    unsigned long drift = 0; // TODO: Add specific drift per each channel. Sticks has aroud +-6 while buttons has 0
    
    void retryConnection();
    // Channel mapping structure
    struct ChannelMapping {
        int channelIndex;
        int (PS5_Controller::*getValue)();  // Member function pointer to get the value
        int minValue;
        int maxValue;
    };
    
    // Channel mappings from frontend channel definitions
    ChannelMapping _channelMappings[28] = {
        // Analog Sticks (int8_t: -128 to 127)
        {0, nullptr, -128, 127},   // LStickX
        {1, nullptr, -128, 127},   // LStickY
        {2, nullptr, -128, 127},   // RStickX
        {3, nullptr, -128, 127},   // RStickY
        
        // Face Buttons (bool: 0 to 1)
        {4, nullptr, 0, 1},        // Square
        {5, nullptr, 0, 1},        // Cross
        {6, nullptr, 0, 1},        // Triangle
        {7, nullptr, 0, 1},        // Circle
        
        // D-Pad Buttons (bool: 0 to 1)
        {8, nullptr, 0, 1},        // Left
        {9, nullptr, 0, 1},        // Right
        {10, nullptr, 0, 1},       // Up
        {11, nullptr, 0, 1},       // Down
        
        // Shoulder Buttons (bool: 0 to 1)
        {12, nullptr, 0, 1},       // L1
        {13, nullptr, 0, 1},       // L2
        {14, nullptr, 0, 1},       // R1
        {15, nullptr, 0, 1},       // R2
        
        // Other Buttons (bool: 0 to 1)
        {16, nullptr, 0, 1},       // Share
        {17, nullptr, 0, 1},       // Options
        {18, nullptr, 0, 1},       // L3
        {19, nullptr, 0, 1},       // R3
        {20, nullptr, 0, 1},       // PSButton
        {21, nullptr, 0, 1},       // Touchpad
        
        // Trigger Values (uint8_t: 0 to 255)
        {22, nullptr, 0, 255},     // L2Value
        {23, nullptr, 0, 255},     // R2Value
        
        // D-Pad Diagonal Buttons (bool: 0 to 1)
        {24, nullptr, 0, 1},       // UpRight
        {25, nullptr, 0, 1},       // DownRight
        {26, nullptr, 0, 1},       // UpLeft
        {27, nullptr, 0, 1}        // DownLeft
    };
    
    std::map<int, boolean> _buttonClicks = {
        {4, false},
        {5, false},
        {6, false},
        {7, false},
        {8, false},
        {9, false},
        {10, false},
        {11, false},
        {12, false},
        {13, false},
        {14, false},
        {15, false},
        {16, false},
        {17, false},
        {18, false},
        {19, false},
        {20, false},
        {21, false},
        {24, false},
        {25, false},
        {26, false},
        {27, false}
    };

    void initializeChannelMappings();
    int getChannelValue(int channelIndex);
    
    // Individual channel getter methods
    int getLStickX() { return ps5.LStickX(); }
    int getLStickY() { return ps5.LStickY(); }
    int getRStickX() { return ps5.RStickX(); }
    int getRStickY() { return ps5.RStickY(); }
    
    int getSquare() { return ps5.Square() ? 1 : 0; }
    int getCross() { return ps5.Cross() ? 1 : 0; }
    int getTriangle() { return ps5.Triangle() ? 1 : 0; }
    int getCircle() { return ps5.Circle() ? 1 : 0; }
    
    int getLeft() { return ps5.Left() ? 1 : 0; }
    int getRight() { return ps5.Right() ? 1 : 0; }
    int getUp() { return ps5.Up() ? 1 : 0; }
    int getDown() { return ps5.Down() ? 1 : 0; }
    
    int getL1() { return ps5.L1() ? 1 : 0; }
    int getL2() { return ps5.L2() ? 1 : 0; }
    int getR1() { return ps5.R1() ? 1 : 0; }
    int getR2() { return ps5.R2() ? 1 : 0; }
    
    int getShare() { return ps5.Share() ? 1 : 0; }
    int getOptions() { return ps5.Options() ? 1 : 0; }
    int getL3() { return ps5.L3() ? 1 : 0; }
    int getR3() { return ps5.R3() ? 1 : 0; }
    int getPSButton() { return ps5.PSButton() ? 1 : 0; }
    int getTouchpad() { return ps5.Touchpad() ? 1 : 0; }
    
    int getL2Value() { return ps5.L2Value(); }
    int getR2Value() { return ps5.R2Value(); }
    
    int getUpRight() { return ps5.UpRight() ? 1 : 0; }
    int getDownRight() { return ps5.DownRight() ? 1 : 0; }
    int getUpLeft() { return ps5.UpLeft() ? 1 : 0; }
    int getDownLeft() { return ps5.DownLeft() ? 1 : 0; }

public:
    PS5_Controller(PS5ControllerConfig config)
        : _config(config),
          ps5() {}

    void begin() override;
    void loop() override;
    void printAllChannels() override;
    int getChannelPercent(int channel) override;
    int getChannelPercent(int channel, int min, int max) override;
    bool isConnected() override { 
        if(!_ready) {
            return false;
        }
        return ps5.isConnected(); 
    }
};

#endif
