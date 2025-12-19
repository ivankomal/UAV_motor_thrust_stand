#include <Arduino.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "HX711.h"

#define MOTOR_PIN 19  // PWM pin for motor ESC

// Load cell pins
#define DT 18
#define SCK 23

// PWM range for ESC (INVERTED: lower PWM = faster)
#define MIN_PWM 1210   // Maximum speed (fastest)
#define MAX_PWM 1340   // Minimum speed (slowest)

// Ramp settings
#define PWM_STEP 10      // PWM change per step
#define STEP_DELAY 2000  // Delay between steps (ms)

// Load cell calibration
const float CALIBRATION_WEIGHT_KG = 0.800;
const float CORRECTION_K = 3.265;

// Drone payload calculation
const float DRONE_WEIGHT_KG = 0.500;          // Average drone weight (hardcoded)
const int NUM_MOTORS = 4;                      // Quadcopter
const float THRUST_TO_WEIGHT_RATIO = 2.0;      // For stable flight and maneuvers (2:1 ratio)

Servo esc;
LiquidCrystal_I2C lcd(0x27, 20, 4);
HX711 scale;

bool testCompleted = false;
float maxThrustKg = 0.0;  // Track maximum thrust during test

void setup() {
  Serial.begin(9600);
  delay(1000);

  Serial.println("\n=== Motor Algorithm Test ===");
  Serial.println("Automatic ramping: 1340us <-> 1210us\n");

  // Initialize LCD
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Algorithm Test");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");

  // Attach and arm ESC
  esc.attach(MOTOR_PIN, 1000, 2000);
  Serial.println("Arming ESC at 1340us (stopped)...");
  esc.writeMicroseconds(MAX_PWM);
  delay(2000);
  Serial.println("ESC armed!");

  // Initialize and calibrate load cell
  Serial.println("\nCalibrating load cell...");
  lcd.setCursor(0, 1);
  lcd.print("Calibrating...");

  scale.begin(DT, SCK);
  delay(1000);
  scale.tare();

  long raw = scale.read_average(20);
  float scale_factor = raw / CALIBRATION_WEIGHT_KG;
  scale.set_scale(scale_factor);

  Serial.println("Load cell calibrated!");

  Serial.println("\nStarting ramp sequence...");
  Serial.println("PWM (us) | Throttle % | Thrust (kg)");
  Serial.println("=========================================\n");

  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Processing...");
}

void loop() {
  if (testCompleted) {
    // Test already completed, do nothing
    return;
  }

  // Calculate total steps for progress tracking
  int stepsDown = (MAX_PWM - MIN_PWM) / PWM_STEP;
  int stepsUp = (MAX_PWM - MIN_PWM) / PWM_STEP;
  int totalSteps = stepsDown + stepsUp;
  int currentStep = 0;

  // Ramp DOWN from 1340us to 1210us (SPEEDING UP on inverted ESC)
  Serial.println("=== Speeding up: 1340us -> 1210us ===");
  for (int pwm = MAX_PWM; pwm >= MIN_PWM; pwm -= PWM_STEP) {
    esc.writeMicroseconds(pwm);

    // Read thrust from load cell
    float thrust_kg = 0.0;
    if (scale.is_ready()) {
      float weight_raw = scale.get_units(10);
      thrust_kg = weight_raw * CORRECTION_K;
    }

    // Track maximum thrust
    if (thrust_kg > maxThrustKg) {
      maxThrustKg = thrust_kg;
    }

    int throttlePercent = map(pwm, MAX_PWM, MIN_PWM, 0, 100);
    int progressPercent = (currentStep * 100) / totalSteps;

    Serial.print(pwm);
    Serial.print("us\t| ");
    Serial.print(throttlePercent);
    Serial.print("%\t| ");
    Serial.print(thrust_kg, 3);
    Serial.print(" kg\t| Progress: ");
    Serial.print(progressPercent);
    Serial.println("%");

    // Update LCD
    lcd.setCursor(0, 1);
    lcd.print("Progress: ");
    lcd.print(progressPercent);
    lcd.print("%   ");

    lcd.setCursor(0, 2);
    lcd.print("Thrust: ");
    lcd.print(thrust_kg, 3);
    lcd.print(" kg   ");

    currentStep++;
    delay(STEP_DELAY);
  }

  Serial.println("\n[HOLD] At maximum speed (1210us) for 2 seconds...\n");
  delay(2000);

  // Ramp UP from 1210us to 1340us (SLOWING DOWN on inverted ESC)
  Serial.println("=== Slowing down: 1210us -> 1340us ===");
  for (int pwm = MIN_PWM; pwm <= MAX_PWM; pwm += PWM_STEP) {
    esc.writeMicroseconds(pwm);

    // Read thrust from load cell
    float thrust_kg = 0.0;
    if (scale.is_ready()) {
      float weight_raw = scale.get_units(10);
      thrust_kg = weight_raw * CORRECTION_K;
    }

    // Track maximum thrust
    if (thrust_kg > maxThrustKg) {
      maxThrustKg = thrust_kg;
    }

    int throttlePercent = map(pwm, MAX_PWM, MIN_PWM, 0, 100);
    int progressPercent = (currentStep * 100) / totalSteps;

    Serial.print(pwm);
    Serial.print("us\t| ");
    Serial.print(throttlePercent);
    Serial.print("%\t| ");
    Serial.print(thrust_kg, 3);
    Serial.print(" kg\t| Progress: ");
    Serial.print(progressPercent);
    Serial.println("%");

    // Update LCD
    lcd.setCursor(0, 1);
    lcd.print("Progress: ");
    lcd.print(progressPercent);
    lcd.print("%   ");

    lcd.setCursor(0, 2);
    lcd.print("Thrust: ");
    lcd.print(thrust_kg, 3);
    lcd.print(" kg   ");

    currentStep++;
    delay(STEP_DELAY);
  }

  Serial.println("\n[TEST COMPLETE] Motor stopped.\n");

  // Stop motor
  esc.writeMicroseconds(MAX_PWM);

  // Calculate payload capacity
  float totalThrust = maxThrustKg * NUM_MOTORS;  // Total thrust from 4 motors
  float maxTotalWeight = totalThrust / THRUST_TO_WEIGHT_RATIO;  // Max weight for stable flight
  float payloadCapacity = maxTotalWeight - DRONE_WEIGHT_KG;  // Available payload

  // Display results on Serial Monitor
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

  Serial.print("Max total weight: ");
  Serial.print(maxTotalWeight, 3);
  Serial.println(" kg");

  Serial.print("\n>>> PAYLOAD CAPACITY: ");
  Serial.print(payloadCapacity, 3);
  Serial.println(" kg <<<\n");
  Serial.println("=========================================\n");

  // Display results on LCD
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

  testCompleted = true;
}
