#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define BUTTON_PIN 4  // Button on GPIO 4

LiquidCrystal_I2C lcd(0x27, 20, 4);

// UI States
enum UIState {
  STATE_WELCOME,
  STATE_MENU,
  STATE_MANUAL_TEST,
  STATE_ALGORITHM_TEST
};

// Button states
enum ButtonState {
  BTN_RELEASED,
  BTN_PRESSED,
  BTN_LONG_PRESS
};

UIState currentState = STATE_WELCOME;
int selectedOption = 1;  // 1 or 2
unsigned long buttonPressStart = 0;
bool buttonWasPressed = false;
const unsigned long LONG_PRESS_TIME = 3000;  // 3 seconds
const unsigned long DEBOUNCE_DELAY = 50;     // 50ms debounce

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

void displayManualTest() {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Manual motor test");
  lcd.setCursor(0, 2);
  lcd.print("Starting...");
}

void displayAlgorithmTest() {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Thrust test");
  lcd.setCursor(0, 2);
  lcd.print("algorithm");
  lcd.setCursor(0, 3);
  lcd.print("Starting...");
}

ButtonState checkButton() {
  bool buttonPressed = (digitalRead(BUTTON_PIN) == LOW);  // Assuming active LOW

  if (buttonPressed && !buttonWasPressed) {
    // Button just pressed
    buttonPressStart = millis();
    buttonWasPressed = true;
    delay(DEBOUNCE_DELAY);
    return BTN_PRESSED;
  }

  if (!buttonPressed && buttonWasPressed) {
    // Button released
    unsigned long pressDuration = millis() - buttonPressStart;
    buttonWasPressed = false;
    delay(DEBOUNCE_DELAY);

    if (pressDuration >= LONG_PRESS_TIME) {
      return BTN_LONG_PRESS;
    } else {
      return BTN_RELEASED;
    }
  }

  // Check if still pressed and duration exceeded
  if (buttonPressed && buttonWasPressed) {
    unsigned long pressDuration = millis() - buttonPressStart;
    if (pressDuration >= LONG_PRESS_TIME) {
      // Visual feedback for long press
      lcd.setCursor(19, 3);
      lcd.print("!");
    }
  }

  return BTN_PRESSED;
}

void setup() {
  Serial.begin(9600);
  delay(1000);

  Serial.println("\n=== UI Test ===");

  // Configure button pin
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // Using internal pull-up

  // Initialize LCD
  Wire.begin();
  lcd.init();
  lcd.backlight();

  // Show welcome screen
  displayWelcomeScreen();
  Serial.println("Welcome screen displayed");

  delay(2000);  // Show welcome for 2 seconds

  // Move to menu
  currentState = STATE_MENU;
  displayMenu();
  Serial.println("Menu displayed");
}

void loop() {
  ButtonState btnState = checkButton();

  switch (currentState) {
    case STATE_WELCOME:
      // Auto-transition to menu (handled in setup)
      break;

    case STATE_MENU:
      if (btnState == BTN_RELEASED) {
        // Short press - switch option
        selectedOption = (selectedOption == 1) ? 2 : 1;
        displayMenu();

        Serial.print("Option selected: ");
        Serial.println(selectedOption);
      }
      else if (btnState == BTN_LONG_PRESS) {
        // Long press - select option
        Serial.print("Choosing option: ");
        Serial.println(selectedOption);

        if (selectedOption == 1) {
          currentState = STATE_MANUAL_TEST;
          displayManualTest();
        } else {
          currentState = STATE_ALGORITHM_TEST;
          displayAlgorithmTest();
        }
      }
      break;

    case STATE_MANUAL_TEST:
      // In manual test mode - display info
      Serial.println("Manual test mode active");
      delay(1000);
      break;

    case STATE_ALGORITHM_TEST:
      // In algorithm test mode - display info
      Serial.println("Algorithm test mode active");
      delay(1000);
      break;
  }

  delay(50);  // Small delay for loop stability
}
