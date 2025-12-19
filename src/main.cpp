#include <Arduino.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "HX711.h"

// Pin definitions
#define MOTOR_PIN 19      // PWM pin for motor ESC
#define POT_PIN 34        // Potentiometer analog input (ADC1_CH6)
#define BUTTON_PIN 4      // Button for menu navigation
#define DT 18             // Load cell data pin
#define SCK 23            // Load cell clock pin

// PWM range for ESC (INVERTED: lower PWM = faster)
#define MIN_PWM 1200      // Maximum speed (fastest)
#define MAX_PWM 1340      // Minimum spinning speed (slowest)

// Algorithm test settings
#define MIN_PWM_ALGO 1210
#define MAX_PWM_ALGO 1340
#define PWM_STEP 10
#define STEP_DELAY 2000

// Load cell calibration
const float CALIBRATION_WEIGHT_KG = 0.800;
const float CORRECTION_K = 3.265;

// Drone payload calculation
const float DRONE_WEIGHT_KG = 0.500;
const int NUM_MOTORS = 4;
const float THRUST_TO_WEIGHT_RATIO = 2.0;

// UI States
enum UIState {
  STATE_WELCOME,
  STATE_MENU,
  STATE_MANUAL_TEST,
  STATE_ALGORITHM_TEST
};

// Hardware objects
Servo esc;
HX711 scale;
LiquidCrystal_I2C lcd(0x27, 20, 4);

// State variables
UIState currentState = STATE_WELCOME;
int selectedOption = 1;
bool buttonWasPressed = false;
unsigned long buttonPressStart = 0;
const unsigned long LONG_PRESS_TIME = 3000;
const unsigned long DEBOUNCE_DELAY = 50;

// Algorithm test variables
bool algorithmTestCompleted = false;
float maxThrustKg = 0.0;
int algorithmStep = 0;
int totalAlgorithmSteps = 0;

// Function prototypes
void displayWelcomeScreen();
void displayMenu();
void setupManualTest();
void runManualTest();
void setupAlgorithmTest();
void runAlgorithmTest();
bool checkButtonPress();
bool checkButtonLongPress();

void displayWelcomeScreen() {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Motor Thrust Stand");
}

void displayMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Choose option:");

  lcd.setCursor(0, 1);
  if (selectedOption == 1) {
    lcd.print("> ");
  } else {
    lcd.print("  ");
  }
  lcd.print("1) Manual test");

  lcd.setCursor(0, 2);
  if (selectedOption == 2) {
    lcd.print("> ");
  } else {
    lcd.print("  ");
  }
  lcd.print("2) Algorithm test");
}

bool checkButtonPress() {
  bool buttonPressed = (digitalRead(BUTTON_PIN) == LOW);

  if (buttonPressed && !buttonWasPressed) {
    buttonPressStart = millis();
    buttonWasPressed = true;
    delay(DEBOUNCE_DELAY);
    return false;
  }

  if (!buttonPressed && buttonWasPressed) {
    unsigned long pressDuration = millis() - buttonPressStart;
    buttonWasPressed = false;
    delay(DEBOUNCE_DELAY);

    if (pressDuration < LONG_PRESS_TIME) {
      return true;  // Short press
    }
  }

  return false;
}

bool checkButtonLongPress() {
  if (buttonWasPressed) {
    unsigned long pressDuration = millis() - buttonPressStart;
    if (pressDuration >= LONG_PRESS_TIME) {
      buttonWasPressed = false;
      delay(DEBOUNCE_DELAY);
      return true;
    }
  }
  return false;
}

void setupManualTest() {
  Serial.println("\n=== Manual Test Mode ===");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Throttle:");
  lcd.setCursor(0, 2);
  lcd.print("Thrust:");

  Serial.println("Throttle % | PWM (us) | Thrust (kg)");
  Serial.println("========================================");
}

void runManualTest() {
  // Check for long press to exit
  if (checkButtonLongPress()) {
    Serial.println("\nExiting manual test...");
    esc.writeMicroseconds(1360);  // Stop motor
    delay(500);
    currentState = STATE_MENU;
    displayMenu();
    Serial.println("Returned to menu\n");
    return;
  }

  // Read potentiometer and map to PWM
  int potValue = analogRead(POT_PIN);
  int pwmValue = map(potValue, 0, 4095, MIN_PWM, MAX_PWM);

  // Send PWM to motor
  esc.writeMicroseconds(pwmValue);

  // Calculate throttle percentage
  int throttlePercent = map(pwmValue, MAX_PWM, MIN_PWM, 0, 100);

  // Read load cell
  float thrust_kg = 0.0;
  if (scale.is_ready()) {
    float weight_raw = scale.get_units(10);
    thrust_kg = weight_raw * CORRECTION_K;
  }

  // Display data on Serial Monitor
  Serial.print(throttlePercent);
  Serial.print("%\t| ");
  Serial.print(pwmValue);
  Serial.print("us\t| ");
  Serial.print(thrust_kg, 3);
  Serial.println(" kg");

  // Display data on LCD
  lcd.setCursor(0, 1);
  lcd.print("   ");
  lcd.setCursor(0, 1);
  lcd.print(throttlePercent);
  lcd.print("%");

  lcd.setCursor(0, 3);
  lcd.print("         ");
  lcd.setCursor(0, 3);
  lcd.print(thrust_kg, 3);
  lcd.print(" kg");

  delay(100);
}

void setupAlgorithmTest() {
  Serial.println("\n=== Algorithm Test Mode ===");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Algorithm Test");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");

  delay(1000);

  // Calculate total steps
  int stepsDown = (MAX_PWM_ALGO - MIN_PWM_ALGO) / PWM_STEP;
  int stepsUp = (MAX_PWM_ALGO - MIN_PWM_ALGO) / PWM_STEP;
  totalAlgorithmSteps = stepsDown + stepsUp;
  algorithmStep = 0;
  maxThrustKg = 0.0;
  algorithmTestCompleted = false;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Processing...");

  Serial.println("PWM (us) | Throttle % | Thrust (kg) | Progress");
  Serial.println("=============================================");
}

void runAlgorithmTest() {
  if (algorithmTestCompleted) {
    return;  // Test already complete
  }

  bool exitRequested = false;

  // Ramp DOWN from MAX to MIN (speeding up)
  Serial.println("=== Speeding up ===");
  for (int pwm = MAX_PWM_ALGO; pwm >= MIN_PWM_ALGO; pwm -= PWM_STEP) {
    // Check for exit request
    if (checkButtonLongPress()) {
      exitRequested = true;
      break;
    }

    esc.writeMicroseconds(pwm);

    // Read thrust
    float thrust_kg = 0.0;
    if (scale.is_ready()) {
      float weight_raw = scale.get_units(10);
      thrust_kg = weight_raw * CORRECTION_K;
    }

    // Track maximum
    if (thrust_kg > maxThrustKg) {
      maxThrustKg = thrust_kg;
    }

    int throttlePercent = map(pwm, MAX_PWM_ALGO, MIN_PWM_ALGO, 0, 100);
    int progressPercent = (algorithmStep * 100) / totalAlgorithmSteps;

    // Serial output
    Serial.print(pwm);
    Serial.print("us\t| ");
    Serial.print(throttlePercent);
    Serial.print("%\t| ");
    Serial.print(thrust_kg, 3);
    Serial.print(" kg\t| ");
    Serial.print(progressPercent);
    Serial.println("%");

    // LCD update
    lcd.setCursor(0, 1);
    lcd.print("Progress: ");
    lcd.print(progressPercent);
    lcd.print("%   ");

    lcd.setCursor(0, 2);
    lcd.print("Thrust: ");
    lcd.print(thrust_kg, 3);
    lcd.print(" kg   ");

    algorithmStep++;
    delay(STEP_DELAY);
  }

  if (exitRequested) {
    Serial.println("\nExiting algorithm test...");
    esc.writeMicroseconds(1360);  // Stop motor
    delay(500);
    currentState = STATE_MENU;
    displayMenu();
    Serial.println("Returned to menu\n");
    return;
  }

  Serial.println("\n[HOLD] At maximum speed for 2 seconds\n");
  delay(2000);

  // Ramp UP from MIN to MAX (slowing down)
  Serial.println("=== Slowing down ===");
  for (int pwm = MIN_PWM_ALGO; pwm <= MAX_PWM_ALGO; pwm += PWM_STEP) {
    // Check for exit request
    if (checkButtonLongPress()) {
      exitRequested = true;
      break;
    }

    esc.writeMicroseconds(pwm);

    // Read thrust
    float thrust_kg = 0.0;
    if (scale.is_ready()) {
      float weight_raw = scale.get_units(10);
      thrust_kg = weight_raw * CORRECTION_K;
    }

    // Track maximum
    if (thrust_kg > maxThrustKg) {
      maxThrustKg = thrust_kg;
    }

    int throttlePercent = map(pwm, MAX_PWM_ALGO, MIN_PWM_ALGO, 0, 100);
    int progressPercent = (algorithmStep * 100) / totalAlgorithmSteps;

    // Serial output
    Serial.print(pwm);
    Serial.print("us\t| ");
    Serial.print(throttlePercent);
    Serial.print("%\t| ");
    Serial.print(thrust_kg, 3);
    Serial.print(" kg\t| ");
    Serial.print(progressPercent);
    Serial.println("%");

    // LCD update
    lcd.setCursor(0, 1);
    lcd.print("Progress: ");
    lcd.print(progressPercent);
    lcd.print("%   ");

    lcd.setCursor(0, 2);
    lcd.print("Thrust: ");
    lcd.print(thrust_kg, 3);
    lcd.print(" kg   ");

    algorithmStep++;
    delay(STEP_DELAY);
  }

  if (exitRequested) {
    Serial.println("\nExiting algorithm test...");
    esc.writeMicroseconds(1360);  // Stop motor
    delay(500);
    currentState = STATE_MENU;
    displayMenu();
    Serial.println("Returned to menu\n");
    return;
  }

  // Stop motor
  esc.writeMicroseconds(MAX_PWM_ALGO);

  // Calculate payload
  float totalThrust = maxThrustKg * NUM_MOTORS;
  float maxTotalWeight = totalThrust / THRUST_TO_WEIGHT_RATIO;
  float payloadCapacity = maxTotalWeight - DRONE_WEIGHT_KG;

  // Serial output
  Serial.println("\n========== PAYLOAD CALCULATION ==========");
  Serial.print("Max single motor thrust: ");
  Serial.print(maxThrustKg, 3);
  Serial.println(" kg");
  Serial.print("Total thrust (4 motors): ");
  Serial.print(totalThrust, 3);
  Serial.println(" kg");
  Serial.print("Drone weight: ");
  Serial.print(DRONE_WEIGHT_KG, 3);
  Serial.println(" kg");
  Serial.print("Thrust-to-weight ratio: ");
  Serial.print(THRUST_TO_WEIGHT_RATIO, 1);
  Serial.println(":1");
  Serial.print("\n>>> PAYLOAD CAPACITY: ");
  Serial.print(payloadCapacity, 3);
  Serial.println(" kg <<<\n");
  Serial.println("=========================================\n");

  // LCD display results
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Test Complete!");
  lcd.setCursor(0, 1);
  lcd.print("Max thrust: ");
  lcd.print(maxThrustKg, 2);
  lcd.print("kg");
  lcd.setCursor(0, 2);
  lcd.print("UAV thrust: ");
  lcd.print(totalThrust, 2);
  lcd.print("kg");
  lcd.setCursor(0, 3);
  lcd.print("Payload: ");
  lcd.print(payloadCapacity, 2);
  lcd.print("kg");

  algorithmTestCompleted = true;
}

void setup() {
  Serial.begin(9600);
  delay(1000);

  Serial.println("\n=== UAV Motor Thrust Stand ===\n");

  // Configure pins
  pinMode(POT_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Initialize LCD
  Wire.begin();
  lcd.init();
  lcd.backlight();

  // Show welcome screen
  displayWelcomeScreen();
  Serial.println("Welcome screen displayed");
  delay(2000);

  // Attach and arm ESC
  esc.attach(MOTOR_PIN, 1000, 2000);
  Serial.println("Arming ESC at 1360us (stopped)...");
  esc.writeMicroseconds(1360);
  delay(2000);
  Serial.println("ESC armed!");

  // Initialize and calibrate load cell
  Serial.println("\nCalibrating load cell...");
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Calibrating...");

  scale.begin(DT, SCK);
  delay(1000);
  scale.tare();

  long raw = scale.read_average(20);
  float scale_factor = raw / CALIBRATION_WEIGHT_KG;
  scale.set_scale(scale_factor);

  Serial.println("Load cell calibrated!");

  // Move to menu
  currentState = STATE_MENU;
  displayMenu();
  Serial.println("Menu displayed\n");
}

void loop() {
  // Check button inputs
  bool shortPress = checkButtonPress();
  bool longPress = checkButtonLongPress();

  switch (currentState) {
    case STATE_WELCOME:
      // Auto-transition handled in setup
      break;

    case STATE_MENU:
      if (shortPress) {
        // Toggle option
        selectedOption = (selectedOption == 1) ? 2 : 1;
        displayMenu();
        Serial.print("Option selected: ");
        Serial.println(selectedOption);
      }
      else if (longPress) {
        // Select option
        Serial.print("Choosing option: ");
        Serial.println(selectedOption);

        if (selectedOption == 1) {
          currentState = STATE_MANUAL_TEST;
          setupManualTest();
        } else {
          currentState = STATE_ALGORITHM_TEST;
          setupAlgorithmTest();
          runAlgorithmTest();  // Run once
        }
      }
      break;

    case STATE_MANUAL_TEST:
      runManualTest();
      break;

    case STATE_ALGORITHM_TEST:
      // Algorithm test runs once in setup
      break;
  }

  delay(10);
}
