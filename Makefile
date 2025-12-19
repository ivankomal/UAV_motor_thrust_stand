# UAV Motor Thrust Stand - Makefile
ENV=esp32dev

.PHONY: help build upload monitor run clean test-motor test-motor-2 test-motor-manual test-tenzo test-lcd test-algorithm test-ui all

all: build

help:
	@echo "UAV Motor Thrust Stand - Available commands:"
	@echo ""
	@echo "  make build          - Build the project"
	@echo "  make upload         - Upload to device"
	@echo "  make monitor        - Start serial monitor"
	@echo "  make run            - Build, upload and monitor"
	@echo "  make clean          - Clean build files"
	@echo ""
	@echo "  make test-motor        - Build and upload motor PWM test"
	@echo "  make test-motor-2      - Build and upload motor ramp test"
	@echo "  make test-motor-manual - Build and upload motor manual control (potentiometer)"
	@echo "  make test-tenzo        - Build and upload load cell test"
	@echo "  make test-lcd          - Build and upload LCD I2C test"
	@echo "  make test-algorithm    - Build and upload algorithm test (automatic ramping)"
	@echo "  make test-ui           - Build and upload UI test (button and LCD menu)"
	@echo ""
	@echo "  ENV=esp32-s3-devkitm-1 make build  - Build for different board"
	@echo ""

build:
	pio run -e $(ENV)

upload:
	pio run -e $(ENV) --target upload

monitor:
	pio device monitor


clean:
	pio run -e $(ENV) --target clean

test-motor:
	pio run -e test_motor --target upload
	pio device monitor

test-motor-2:
	pio run -e test_motor_2 --target upload
	pio device monitor

test-motor-manual:
	pio run -e test_motor_manual --target upload
	pio device monitor

test-tenzo:
	pio run -e test_loadcell --target upload
	pio device monitor

test-lcd:
	pio run -e test_lcd --target upload
	pio device monitor

test-algorithm:
	pio run -e test_algorithm --target upload
	pio device monitor

test-ui:
	pio run -e test_ui --target upload
	pio device monitor
