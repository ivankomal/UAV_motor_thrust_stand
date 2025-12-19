# Test Files

This folder contains individual test programs for component testing and development. Each test isolates specific hardware components or features for verification and calibration.

## Quick Start

Run any test using the Makefile commands:
```bash
make test-motor          # Basic motor PWM test
make test-motor-2        # Motor ramp test
make test-motor-manual   # Manual control with potentiometer
make test-tenzo          # Load cell calibration
make test-lcd            # LCD display test
make test-algorithm      # Automated algorithm test
make test-ui             # UI menu system test
```

## Test Programs

### Motor Tests

#### `ESC_test.cpp`
Basic motor PWM test with calibration sequence.
- Tests throttle range from 1300us to 1700us
- Displays throttle percentage and PWM values
- Continuous cycle through different speeds

**Run:** `make test-motor`

#### `ESC_test_2.cpp`
Motor ramp test for inverted ESC.
- Ramps from 1400us → 1700us (slowing down)
- Ramps from 1700us → 1100us (speeding up)
- Continuous cycling

**Run:** `make test-motor-2`

#### `ESC_manual_control.cpp`
Potentiometer-based manual motor control.
- Control motor speed with potentiometer
- PWM range: 1200-1340us (inverted ESC)
- Real-time throttle display

**Run:** `make test-motor-manual`

### Sensor Tests

#### `tenzo_test.cpp` (Load Cell Test)
Load cell (HX711) calibration and testing.
- Calibrates with 0.8kg reference weight
- Displays real-time weight in kg
- Applies correction factor: 3.265
- Continuous measurement output

**Run:** `make test-tenzo`

**Hardware:** Load cell connected to DT (GPIO 18/22) and SCK (GPIO 23)

#### `lcd_test.cpp` (LCD Display Test)
LCD I2C (20x4) display test.
- Tests LCD at I2C address 0x27
- Displays test message on all 4 rows
- Verifies backlight and character display
- Tests cursor positioning

**Run:** `make test-lcd`

**Hardware:** 20x4 I2C LCD on default I2C pins

### Integrated Tests

#### `test_algorithm.cpp`
Automated motor ramping with payload calculation.
- Automatic PWM ramping: 1340us ↔ 1210us
- Real-time thrust measurement
- Calculates UAV payload capacity
- Displays progress on LCD
- Single iteration, stops automatically

**Features:**
- Tracks maximum thrust
- Calculates total thrust for 4 motors
- Computes payload capacity with 2:1 thrust-to-weight ratio
- Displays results on LCD and Serial Monitor

**Run:** `make test-algorithm`

#### `UI_test.cpp`
Button-controlled LCD menu system.
- Welcome screen
- Menu navigation with button (GPIO 4)
- Short press: switch options
- Long press: select option
- Two options: Manual test / Algorithm test

**Run:** `make test-ui`

## Hardware Configuration

All tests use:
- **ESP32 DevKit**
- **Motor ESC:** GPIO 19
- **Potentiometer:** GPIO 34 (manual tests)
- **Button:** GPIO 4 (UI test)
- **Load cell DT:** GPIO 18 or 22 (varies by test)
- **Load cell SCK:** GPIO 23
- **LCD I2C:** Default I2C pins (SDA/SCL)

## Important Notes

### ESC Configuration
- **INVERTED ESC**: Lower PWM = Faster motor speed
- Standard ESC range: 1000-2000µs
- This project uses: 1200-1340µs (inverted range)
- Arming: 1360µs for 2 seconds

### Calibration Values
- **Load cell**: 0.8kg reference weight
- **Correction factor**: 3.265 (adjust based on measurements)
- **LCD I2C address**: 0x27 (try 0x3F if not found)

### Serial Monitor
All tests output data to serial monitor at **9600 baud**. Make sure to open the serial monitor to see test results and debugging information.

## Test Development

When creating new tests:
1. Add test file to `test/` directory
2. Create corresponding environment in `platformio.ini`
3. Add build flags and source filter
4. Add Makefile target in root `Makefile`
5. Document the test in this README

Example platformio.ini environment:
```ini
[env:test_new_feature]
board = esp32dev
build_flags =
    ${env.build_flags}
    -DTEST_NEW_FEATURE
build_src_filter = +<*> -<main.cpp> +<../test/new_feature_test.cpp>
```
