#include <Adafruit_Fingerprint.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// Create fingerprint sensor connection
SoftwareSerial mySerial(2, 3); // RX, TX
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// Create LCD object with I2C address 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// LED pins
const int greenLEDPin = 7;
const int redLEDPin = 8;

void setup() {
  // Start serial communication
  Serial.begin(9600);
  while (!Serial); // For Leonardo/Micro/Zero

  // Initialize LED pins
  pinMode(greenLEDPin, OUTPUT);
  pinMode(redLEDPin, OUTPUT);
  digitalWrite(greenLEDPin, LOW);
  digitalWrite(redLEDPin, LOW);

  // Initialize LCD
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");

  // Initialize fingerprint sensor
  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sensor found");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sensor not found");
    while (1) { delay(1); }
  }

  finger.getTemplateCount();
  Serial.print("Sensor contains ");
  Serial.print(finger.templateCount);
  Serial.println(" templates");
  Serial.println("Waiting for valid finger...");

  lcd.setCursor(0, 1);
  lcd.print("Waiting...");
}

void loop() {
  // Check if a fingerprint is available
  uint8_t result = getFingerprintID();
  if (result == FINGERPRINT_OK) {
    Serial.println("Fingerprint matched!");
    digitalWrite(greenLEDPin, HIGH);
    digitalWrite(redLEDPin, LOW);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Accepted");
    Serial.println("Door Unlocked, Welcome!");
  } else if (result == FINGERPRINT_NOTFOUND) {
    Serial.println("Fingerprint not matched");
    digitalWrite(greenLEDPin, LOW);
    digitalWrite(redLEDPin, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Rejected");
    Serial.println("Rejected");
  } else {
    Serial.println("Error reading fingerprint");
    digitalWrite(greenLEDPin, LOW);
    digitalWrite(redLEDPin, LOW);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error");
  }
  delay(1000); // Wait a bit before trying again
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Waiting...");
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return p;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return p;

  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.print("Found ID #"); Serial.print(finger.fingerID);
    Serial.print(" with confidence of "); Serial.println(finger.confidence);
    return FINGERPRINT_OK;
  } else {
    return FINGERPRINT_NOTFOUND;
  }
}
