#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal.h>

// Fingerprint sensor pins
#define rxPin 2
#define txPin 3

// Fingerprint sensor setup
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// LCD setup
LiquidCrystal lcd(7, 8, 9, 10, 11, 12); // Adjust pins as per your wiring

// Motor Driver Pins
#define IN1 5
#define IN2 6
#define ENA 9

// Button pin
#define buttonPin 8

void setup() {
  // Initialize fingerprint sensor
  finger.begin(57600);
  if (finger.verifyPassword()) {
    lcd.setCursor(0, 0);
    lcd.print("Sensor found");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Sensor not found");
    while (1);
  }

  // Initialize LCD
  lcd.begin(16, 2);

  // Set up motor driver pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);

  // Set up button pin
  pinMode(buttonPin, INPUT_PULLUP);

  // Display ready message
  lcd.setCursor(0, 1);
  lcd.print("Place Finger");
}

void loop() {
  // Wait for user to press the button
  if (digitalRead(buttonPin) == LOW) {
    delay(500); // Debounce delay
    uint8_t id = getFingerprintID();
    if (id != -1) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Success: ID ");
      lcd.print(id);
      activateMotor();
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Failed");
      stopMotor();
    }
  }
}

// Function to get fingerprint ID
uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.fingerSearch();
  if (p != FINGERPRINT_OK) return -1;

  return finger.fingerID;
}

// Function to activate motor
void activateMotor() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 255);
}

// Function to stop motor
void stopMotor() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);
}
