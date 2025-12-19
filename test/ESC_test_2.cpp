#include <Arduino.h>
#include <ESP32Servo.h>

#define MOTOR_PIN 19  // PWM pin for motor ESC

Servo esc;

void setup() {
  Serial.begin(9600);
  delay(1000);

  Serial.println("\n=== Motor PWM Ramp Test ===");
  Serial.println("Waiting 15 seconds before starting...");

  // Attach ESC to pin
  esc.attach(MOTOR_PIN, 1000, 2000);

  // ESC Calibration
  Serial.println("Calibrating ESC - MAX throttle");
  esc.writeMicroseconds(2000);
  delay(2000);

  Serial.println("Calibrating ESC - MIN throttle");
  esc.writeMicroseconds(1000);
  delay(3000);

  Serial.println("ESC armed!");

  // Start motor at 1100us (FAST on reversed ESC)
  Serial.println("Starting motor at 1100us (fast)...");
  esc.writeMicroseconds(1400);
  delay(3000);

  Serial.println("Motor spinning! Waiting 10 seconds before ramp...");
  delay(10000);

  Serial.println("Starting ramp sequence...\n");
}

void loop() {
  // ESC is INVERTED: low PWM = fast, high PWM = slow
  // Ramp from FAST to SLOW: 1400us -> 1700us
  Serial.println("=== Slowing down: 1400us -> 1700us ===");
  for (int us = 1400; us <= 1700; us += 100) {
    esc.writeMicroseconds(us);
    Serial.print("[SLOW DOWN] PWM: ");
    Serial.print(us);
    Serial.print("us - Motor getting SLOWER");
    Serial.println();
    delay(2000);
  }

  // Wait 3 seconds at 1700us (slowest on inverted ESC)
  Serial.println("\n[HOLD] At slowest speed (1700us) for 3 seconds...");
  esc.writeMicroseconds(1700);
  delay(3000);

  // Ramp from SLOW to FAST: 1700us -> 1100us
  Serial.println("\n=== Speeding up: 1700us -> 1100us ===");
  for (int us = 1700; us >= 1100; us -= 100) {
    esc.writeMicroseconds(us);
    Serial.print("[SPEED UP] PWM: ");
    Serial.print(us);
    Serial.print("us - Motor getting FASTER");
    Serial.println();
    delay(2000);
  }

  Serial.println("\n[CYCLE COMPLETE] Restarting...\n");
  delay(1000);
}
