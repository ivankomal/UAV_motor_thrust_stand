#include <Arduino.h>
#include "HX711.h"

#define DT 22
#define SCK 23

HX711 scale;

const float CALIBRATION_WEIGHT_KG = 0.800; 
const float CORRECTION_K = 3.265;

void setup() {
  Serial.begin(9600);
  scale.begin(DT, SCK);

  Serial.println("=== HX711 CALIBRATION ===");
  delay(2000);
  scale.tare();

  long raw = scale.read_average(20); 
  float scale_factor = raw / CALIBRATION_WEIGHT_KG;

  scale.set_scale(scale_factor);
}
void loop() {
  if (scale.is_ready()) {
    float weight_raw = scale.get_units(10);   
    float weight_kg  = weight_raw * CORRECTION_K;

    Serial.print("Вага (кг): ");
    Serial.println(weight_kg, 3);
  }
  delay(500);
}


