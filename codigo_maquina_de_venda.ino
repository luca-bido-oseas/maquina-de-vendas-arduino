#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Pin definitions
const int CANCEL_PIN = 3;
const int KITKAT_SELECT = 5;
const int MM_SELECT = 4;
const int CONFIRM_PIN = 6;
const int MOTOR_KITKAT_IN1 = 8;
const int MOTOR_KITKAT_IN2 = 9;
const int MOTOR_MM_IN1 = 10;
const int MOTOR_MM_IN2 = 11;

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// State machine
enum State { ST_IDLE, ST_SELECTING, ST_DISPENSING };
State currentState = ST_IDLE;

// Variables
int selectedProduct = 0; // 1 for KitKat, 2 for MM
unsigned long motorStartTime = 0;
const unsigned long MOTOR_RUN_TIME = 1500; // 5 seconds

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.print("Vending Machine");
  
  pinMode(CANCEL_PIN, INPUT_PULLUP);
  pinMode(KITKAT_SELECT, INPUT_PULLUP);
  pinMode(MM_SELECT, INPUT_PULLUP);
  pinMode(CONFIRM_PIN, INPUT_PULLUP);
  pinMode(MOTOR_KITKAT_IN1, OUTPUT);
  pinMode(MOTOR_KITKAT_IN2, OUTPUT);
  pinMode(MOTOR_MM_IN1, OUTPUT);
  pinMode(MOTOR_MM_IN2, OUTPUT);
  
  // Ensure motors are off
  digitalWrite(MOTOR_KITKAT_IN1, LOW);
  digitalWrite(MOTOR_KITKAT_IN2, LOW);
  digitalWrite(MOTOR_MM_IN1, LOW);
  digitalWrite(MOTOR_MM_IN2, LOW);
  
  // Wait for serial monitor
  delay(1000);
  Serial.println("Vending Machine Ready");
  Serial.println("Select product: 3 for KitKat, 4 for M&M");
}

void loop() {
  // Read buttons (active low due to INPUT_PULLUP)
  bool cancelPressed = digitalRead(CANCEL_PIN) == LOW;
  bool kitkatPressed = digitalRead(KITKAT_SELECT) == LOW;
  bool mmPressed = digitalRead(MM_SELECT) == LOW;
  bool confirmPressed = digitalRead(CONFIRM_PIN) == LOW;
  
  switch (currentState) {
    case ST_IDLE:
      if (kitkatPressed) {
        selectedProduct = 1;
        currentState = ST_SELECTING;
        lcd.clear();
        lcd.print("KitKat selected");
        lcd.setCursor(0, 1);
        lcd.print("Press 4 to confirm");
        Serial.println("KitKat selected. Press 5 to confirm.");
      } else if (mmPressed) {
        selectedProduct = 2;
        currentState = ST_SELECTING;
        lcd.clear();
        lcd.print("M&M selected");
        lcd.setCursor(0, 1);
        lcd.print("Press 4 to confirm");
        Serial.println("M&M selected. Press 4 to confirm.");
      }
      break;
      
    case ST_SELECTING:
      if (cancelPressed) {
        currentState = ST_IDLE;
        lcd.clear();
        lcd.print("Cancelled");
        delay(1000);
        lcd.clear();
        lcd.print("Vending Machine");
        Serial.println("Selection cancelled");
      } else if (confirmPressed) {
        currentState = ST_DISPENSING;
        motorStartTime = millis();
        lcd.clear();
        lcd.print("Dispensing...");
        // Start the appropriate motor
        if (selectedProduct == 1) {
          // KitKat motor forward (adjust polarity if needed)
          digitalWrite(MOTOR_KITKAT_IN1, HIGH);
          digitalWrite(MOTOR_KITKAT_IN2, LOW);
          Serial.println("KitKat motor ON");
        } else if (selectedProduct == 2) {
          // MM motor forward
          digitalWrite(MOTOR_MM_IN1, HIGH);
          digitalWrite(MOTOR_MM_IN2, LOW);
          Serial.println("M&M motor ON");
        }
      }
      break;
      
    case ST_DISPENSING:
      // Non-blocking timer
      if (millis() - motorStartTime >= MOTOR_RUN_TIME) {
        // Stop motors
        digitalWrite(MOTOR_KITKAT_IN1, LOW);
        digitalWrite(MOTOR_KITKAT_IN2, LOW);
        digitalWrite(MOTOR_MM_IN1, LOW);
        digitalWrite(MOTOR_MM_IN2, LOW);
        Serial.println("Motors OFF");
        
        lcd.clear();
        lcd.print("Take product");
        delay(2000);
        lcd.clear();
        lcd.print("Vending Machine");
        currentState = ST_IDLE;
        selectedProduct = 0;
        Serial.println("Back to idle");
      }
      break;
  }
}