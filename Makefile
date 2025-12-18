# UAV Motor Thrust Stand - Makefile
ENV=esp32dev

.PHONY: help build upload monitor run clean test-motor test-tenzo test-lcd all

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
	@echo "  make test-motor     - Build and upload motor PWM test"
	@echo "  make test-loadcell  - Build and upload load cell test"
	@echo "  make test-lcd       - Build and upload LCD I2C test"
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

test-tenzo:
	pio run -e test_loadcell --target upload
	pio device monitor

test-lcd:
	pio run -e test_lcd --target upload
	pio device monitor
