# UAV Motor Thrust Stand

ESP32-based motor thrust measurement system for UAV/drone motor testing and payload capacity calculation.

## Overview

This project provides a complete solution for measuring UAV motor thrust using a load cell, with an interactive LCD menu system and automated testing capabilities. It supports both manual testing with real-time throttle control and automated algorithm-based testing with payload calculations.

## Features

- **Manual Test Mode**: Real-time motor control using a potentiometer with live thrust readings
- **Algorithm Test Mode**: Automated PWM ramping with comprehensive data collection
- **Payload Calculation**: Automatic calculation of UAV payload capacity based on thrust measurements
- **Interactive UI**: LCD menu system with button navigation
- **Load Cell Integration**: HX711-based precision weight measurement
- **Inverted ESC Support**: Configured for ESCs where lower PWM = higher speed

## Hardware Requirements

### Components
- **Microcontroller**: ESP32 DevKit or ESP32-S3 DevKit
- **Motor ESC**: Standard brushless motor ESC (inverted configuration)
- **Load Cell**: HX711-based load cell sensor
- **Display**: 20x4 I2C LCD (address 0x27)
- **Potentiometer**: 10k potentiometer for manual control
- **Button**: Push button for menu navigation

### Pin Configuration

| Component | GPIO Pin | Notes |
|-----------|----------|-------|
| Motor ESC | 19 | PWM signal |
| Potentiometer | 34 | Analog input |
| Button | 4 | Internal pull-up |
| Load Cell DT | 18 | Data pin |
| Load Cell SCK | 23 | Clock pin |
| LCD I2C | Default | SDA/SCL pins |

## Software Requirements

- [PlatformIO](https://platformio.org/)
- Arduino framework for ESP32

### Dependencies (auto-installed)
- `bogde/HX711` - Load cell amplifier library
- `madhephaestus/ESP32Servo` - Servo/ESC control
- `marcoschwartz/LiquidCrystal_I2C` - LCD display driver

## Installation

1. Clone this repository:
```bash
git clone <repository-url>
cd UAV_motor_thrust_stand
```

2. Install PlatformIO if not already installed

3. Build the project:
```bash
make build
```

4. Upload to your ESP32:
```bash
make upload
```

## Usage

### Main Program

The main program provides a complete UI with two testing modes:

```bash
make run
```

**Menu Navigation:**
- **Short press** (< 3 seconds): Switch between menu options
- **Long press** (≥ 3 seconds): Select current option
- **Exit test**: Long press during any test to return to menu

**Test Modes:**
1. **Manual Test**: Use potentiometer to control motor speed, view real-time thrust
2. **Algorithm Test**: Automated PWM sweep with payload capacity calculation

### Running Tests

Individual component tests are available in the `test/` directory:

```bash
make test-motor          # Basic motor PWM test
make test-motor-2        # Motor ramp test
make test-motor-manual   # Manual control test
make test-tenzo          # Load cell calibration
make test-lcd            # LCD display test
make test-algorithm      # Automated testing algorithm
make test-ui             # Menu system test
```

See [test/README.md](test/README.md) for detailed test descriptions.

## Configuration

### Motor PWM Range

Edit `src/main.cpp` to adjust PWM values for your ESC:

```cpp
#define MIN_PWM 1200        // Minimum PWM (µs) - Maximum speed
#define MAX_PWM 1340        // Maximum PWM (µs) - Stop
```

### Load Cell Calibration

```cpp
const float CALIBRATION_WEIGHT_KG = 0.800;  // Reference weight
const float CORRECTION_K = 3.265;            // Calibration factor
```

### UAV Parameters

```cpp
const float DRONE_WEIGHT_KG = 0.500;        // Drone weight (kg)
const int NUM_MOTORS = 4;                    // Number of motors
const float THRUST_TO_WEIGHT_RATIO = 2.0;   // Desired T/W ratio
```

## Building for Different Boards

For ESP32-S3:
```bash
ENV=esp32-s3-devkitm-1 make build
```

## Project Structure

```
UAV_motor_thrust_stand/
├── src/
│   └── main.cpp           # Main application code
├── test/
│   ├── ESC_test.cpp       # Basic motor tests
│   ├── ESC_test_2.cpp     # Motor ramp test
│   ├── ESC_manual_control.cpp
│   ├── test_algorithm.cpp # Automated testing
│   ├── UI_test.cpp        # Menu system test
│   └── README.md          # Test documentation
├── platformio.ini         # PlatformIO configuration
├── Makefile              # Build commands
└── README.md             # This file
```

## Output Data

### Serial Monitor Output

The system outputs detailed data to the serial monitor (9600 baud):

**Manual Test:**
```
Throttle % | PWM (us) | Thrust (kg)
========================================
45%        | 1277us   | 0.234 kg
```

**Algorithm Test:**
```
PWM (us) | Throttle % | Thrust (kg) | Progress
=============================================
1340us   | 0%         | 0.000 kg    | 0%
...
1210us   | 100%       | 0.456 kg    | 100%

========== PAYLOAD CALCULATION ==========
Max single motor thrust: 0.456 kg
Total thrust (4 motors): 1.824 kg
Drone weight: 0.500 kg
Thrust-to-weight ratio: 2.0:1

>>> PAYLOAD CAPACITY: 0.412 kg <<<
=========================================
```

### LCD Display

**Manual Test:**
```
Throttle:
45%

Thrust:
0.234 kg
```

**Algorithm Test:**
```
Test Complete!
Max thrust: 0.46kg
UAV thrust: 1.82kg
Payload: 0.41kg
```

