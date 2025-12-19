#include <Arduino.h>
#include <ESP32Servo.h>

#define MOTOR_PIN 19      // PWM pin for motor ESC
#define POT_PIN 34        // Potentiometer analog input (ADC1_CH6)

// PWM range for ESC (INVERTED: lower PWM = faster)
#define MIN_PWM 1200   // Maximum speed (fastest)
#define MAX_PWM 1500   // Minimum spinning speed (slowest)

Servo esc;

void setup() {
  Serial.begin(9600);
  delay(1000);

  Serial.println("\n=== Motor Manual Control via Potentiometer ===");

  // Configure potentiometer pin
  pinMode(POT_PIN, INPUT);

  // Attach ESC to pin
  esc.attach(MOTOR_PIN, 1000, 2000);

  // Arm ESC without spinning motor
  Serial.println("Arming ESC at 1500us (stopped)...");
  esc.writeMicroseconds(1500);
  delay(2000);

  Serial.println("ESC armed!");

  Serial.println("\nTurn potentiometer to control motor speed");
  Serial.println("Potentiometer range: 0-4095");
  Serial.println("PWM range: 1200-1500us (inverted ESC)");
  Serial.println("=======================================\n");
}

void loop() {
  // Read potentiometer value (0-4095 on ESP32)
  int potValue = analogRead(POT_PIN);

  // Map potentiometer to PWM range (1000-2000us)
  int pwmValue = map(potValue, 0, 4095, MIN_PWM, MAX_PWM);

  // Send PWM to motor
  esc.writeMicroseconds(pwmValue);

  // Display current values
  Serial.print("Pot: ");
  Serial.print(potValue);
  Serial.print(" | PWM: ");
  Serial.print(pwmValue);
  Serial.print("us (");
  Serial.print(map(pwmValue, MIN_PWM, MAX_PWM, 0, 100));
  Serial.println("%)");

  delay(100);  // Update 10 times per second
}
