# Beta Ride

<div align="center">
  <img src="beta-ride-sm.png" alt="Beta Ride Logo" width="200"/>
</div>

## Overview

Beta Ride is an advanced ESP32-based car control firmware that provides flexible and configurable remote control capabilities for RC vehicles. The firmware supports multiple controller types, brushless motors, servos, and customizable control rules.

## Features

- **Multi-Controller Support**: Compatible with ELRS (ExpressLRS) (other controllers in development)
- **Brushless Motor Control**: Full support for ESC (Electronic Speed Controller with Blheli_S firmware) with 3D capability
- **Servo Control**: Precise servo positioning and control
- **Configurable Rules Engine**: Create custom control mappings between channels and outputs
- **Serial Configuration Interface**: Real-time configuration through web interface via serial connection
- **Persistent Configuration Storage**: Settings saved to ESP32's NVS (Non-Volatile Storage)

## Supported Boards

- ESP32 development board

## Installation Guide

### Prerequisites

1. **Install PlatformIO**: Download and install [PlatformIO](https://platformio.org/install) or use the PlatformIO extension in VS Code
2. **ESP32 Board Support**: Ensure ESP32 board support is installed in PlatformIO

### Method 1: Using PlatformIO (Recommended)

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/DCSets/betaride betaride
   cd betaride
   ```

2. **Install Dependencies**:
   ```bash
   pio pkg install
   ```

3. **Build the Project**:
   ```bash
   pio run
   ```

4. **Upload to ESP32**:
   ```bash
   pio run --target upload
   ```

### Method 2: Download specific release

1. **Download Release Archive**: Visit the [Releases page](https://github.com/DCSets/betaride/releases) and download the latest release files

2. Unzip archive and flash the same way as in previous method    

### Configuration

After flashing, connect to the ESP32 via serial monitor (115200 baud) to verify there are no issues. 

By pressing reboot button you'll see some empty output of firmware.

Once that done you can visit [Configurator page](https://dcsets.com/beta-ride) to configure your ESP. 
Before connect make sure you closed the serial monitor terminal window


## Project Structure

```
esp32-car-firmware/
├── src/                    # Main source code
│   ├── main.cpp           # Entry point
│   ├── App.cpp            # Main application logic
│   └── App.h              # Application header
├── lib/                    # Library components
│   ├── BrushlessMotor/    # Brushless motor control
│   ├── Controller/        # Controller implementations
│   ├── ConfigStore/       # Configuration storage
│   ├── ControllerRules/   # Control rules engine
│   ├── Motor/             # Generic motor control
│   └── Servo/             # Servo control
├── include/               # Header files
│   ├── constants.h        # Global constants
│   ├── enums.h           # Enumerations
│   └── structs.h         # Data structures
└── platformio.ini        # PlatformIO configuration
```

## Development

You are not limited by web configurator. If you want you can freelly create any structures manually (check the related libraries for implementation) and populate resources inside main.cpp script

### Debugging
The firmware includes debug output which can be enabled by providing DEBUG_MODE=1 in build flags (check platformio.ini file)
For additional debug level switch build_type from release to debug


## Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Support

- **Issues**: Report bugs and request features on [GitHub Issues](https://dcsets.com/beta-ride/issues) or in related Discord topic
- **Discussions**: Join the community on [Discord](https://discord.gg/wapS8Huuff)

## Acknowledgments

- [ExpressLRS](https://github.com/ExpressLRS/ExpressLRS) for ELRS protocol support
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson) for JSON handling
- [AlfredoCRSF](https://github.com/alfredosystems/AlfredoCRSF) for CRSF protocol support

---

<div align="center">
  <strong>Built with ❤️ for the RC community</strong>
</div>
