# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1.1] - 16 Sep 2025

### Firmware
- BT Classic scanner (supports onDeviceFound, onStart, onStop events)
- Support for PS5 Controller with 28 channels mapping
- Automatic BT device scan in configurator
- "Click" event in controller rules
- Multiple resources per effect

### Technical
- Added huge_app.csv partition for more efficient utilisation of ESP flash memory
- SetTimeout helper in firmare
- Improved configurator store management
- Compare firmware & configurator version alert

## [0.0.1] - 1 Sep 2025

### Added
- Initial release of BetaRide Car Firmware
- Basic implementation for brushed and DC motors controll
- Servo control
- ELRS receiver support
- Web-based configuration interface
- Basic controller rule engine
- Configuration storage in flash memory

### Features
- Configurable motor direction and speed limits
- Servo angle calibration and limits
- Controller input mapping and scaling
- Real-time configuration updates

### Technical
- Built with PlatformIO framework
- Supports ESP32-WROOM-32 and ESP32-WROVER modules
- Modular architecture with separate libraries for each component
