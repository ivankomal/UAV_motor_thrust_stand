#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {
  Wire.begin(); 
  lcd.init();  
  lcd.backlight(); 
  lcd.clear();
}

void loop() {
  static String counter = "Laba valyt";

  lcd.setCursor(0, 0); 
  lcd.print(counter);
  delay(1000);
}
