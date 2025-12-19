#include <Arduino.h>
#include <ESP32Servo.h>

#define MOTOR_PIN 19  // PWM pin for motor ESC

Servo esc;
int currentThrottle = 1000;

void logThrottle(int throttle) {
  Serial.print("[THROTTLE] Sending: ");
  Serial.print(throttle);
  Serial.print("us (");
  Serial.print(map(throttle, 1000, 2000, 0, 100));
  Serial.println("%)");
}

void setup() {
  Serial.begin(9600);
  delay(1000);

  Serial.println("\n\n=== Motor PWM Diagnostic Test ===");
  Serial.print("Motor Pin: ");
  Serial.println(MOTOR_PIN);
  Serial.println("Waiting 3 seconds for battery connection...");
  delay(3000);

  Serial.println("\n[INIT] Attaching ESC to pin...");
  if (esc.attach(MOTOR_PIN, 1000, 2000)) {
    Serial.println("ESC attached to pin");
  } else {
    Serial.println("Failed to attach ESC!");
  }

  Serial.println("\n[CALIBRATION] Starting ESC calibration sequence");
  Serial.println("----------------------------------------");

  Serial.println("[CAL-1] Sending MAX throttle (2000us)...");
  currentThrottle = 2000;
  esc.writeMicroseconds(currentThrottle);
  logThrottle(currentThrottle);
  Serial.println("Listening for beeps (3 sec)...");
  delay(3000);

  Serial.println("\n[CAL-2] Sending MIN throttle (1000us)...");
  currentThrottle = 1000;
  esc.writeMicroseconds(currentThrottle);
  logThrottle(currentThrottle);
  Serial.println("Waiting for arming beeps (5 sec)...");
  delay(5000);

  Serial.println("\n[ARMED] ESC should be calibrated and armed");
  Serial.println("----------------------------------------");
  Serial.println("\n[READY] Starting throttle sequence");
}

void loop() {
  // 1700us for 2 seconds
  currentThrottle = 1700;
  esc.writeMicroseconds(currentThrottle);
  Serial.print("[LOOP] Throttle: ");
  Serial.print(currentThrottle);
  Serial.print("us (");
  Serial.print(map(currentThrottle, 1000, 2000, 0, 100));
  Serial.println("%)");
  delay(2000);

  // 1600us for 2 seconds
  currentThrottle = 1600;
  esc.writeMicroseconds(currentThrottle);
  Serial.print("[LOOP] Throttle: ");
  Serial.print(currentThrottle);
  Serial.print("us (");
  Serial.print(map(currentThrottle, 1000, 2000, 0, 100));
  Serial.println("%)");
  delay(2000);

  // 1500us for 2 seconds
  currentThrottle = 1500;
  esc.writeMicroseconds(currentThrottle);
  Serial.print("[LOOP] Throttle: ");
  Serial.print(currentThrottle);
  Serial.print("us (");
  Serial.print(map(currentThrottle, 1000, 2000, 0, 100));
  Serial.println("%)");
  delay(2000);

  // 1400us for 2 seconds
  currentThrottle = 1400;
  esc.writeMicroseconds(currentThrottle);
  Serial.print("[LOOP] Throttle: ");
  Serial.print(currentThrottle);
  Serial.print("us (");
  Serial.print(map(currentThrottle, 1000, 2000, 0, 100));
  Serial.println("%)");
  delay(2000);

  // 1300us for 10 seconds
  currentThrottle = 1300;
  esc.writeMicroseconds(currentThrottle);
  Serial.print("[LOOP] Throttle: ");
  Serial.print(currentThrottle);
  Serial.print("us (");
  Serial.print(map(currentThrottle, 1000, 2000, 0, 100));
  Serial.println("%) - MAINTAINING for 10 seconds");
  delay(10000);

  Serial.println("[LOOP] Cycle complete, restarting...\n");
}

