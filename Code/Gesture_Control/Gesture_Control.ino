/*

 * Project Name: Geasture Control Module 

 * Project Brief: Firmware for Geasture Control Module

 * Author: Jobit Joseph

 * Copyright © Jobit Joseph

 * Copyright © Semicon Media Pvt Ltd

 * Copyright © Circuitdigest.com

 * 

 * This program is free software: you can redistribute it and/or modify

 * it under the terms of the GNU General Public License as published by

 * the Free Software Foundation, in version 3.

 *

 * This program is distributed in the hope that it will be useful,

 * but WITHOUT ANY WARRANTY; without even the implied warranty of

 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the

 * GNU General Public License for more details.

 *

 * You should have received a copy of the GNU General Public License

 * along with this program. If not, see <http://www.gnu.org/licenses/&gt;.

 *

 */
#include <Wire.h>
#include <Adafruit_APDS9500.h>
#include <Keyboard.h>
#define Red 10
#define Blue 11
#define Green 12
Adafruit_APDS9500 apds;
const unsigned long onInterval = 100; 
const unsigned long offInterval = 1000;
unsigned long previousMillis = 0;
bool ledState = LOW;

unsigned long ledBlinkMillis = 0;
const unsigned long blinkInterval = 200; // Blink interval in milliseconds
bool ledBlinkState = LOW;
bool shouldBlink = false; //

void ledCB(bool numlock, bool capslock, bool scrolllock, bool compose, bool kana, void *cbData) {
  (void)numlock;
  (void)scrolllock;
  (void)compose;
  (void)kana;
  (void)cbData;
  digitalWrite(Green, capslock ? HIGH : LOW);
}


void setup(void) {
  pinMode(Red, OUTPUT);
  pinMode(Blue, OUTPUT);
  pinMode(Green, OUTPUT);
  digitalWrite(Red, 0);
  digitalWrite(Green, 0);
  digitalWrite(Blue, ledState);
  Serial.begin(115200);
  Keyboard.onLED(ledCB);
  Keyboard.begin();
  //while (!Serial) delay(10);     // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit APDS9500 test!");

  while (!apds.begin()) {
    Serial.println("Failed to find APDS9500 chip, retrying...");
    delay(1000);  // Delay for 1 second before retrying
  }
  Serial.println("APDS9500 Found!");
}

void loop() {

  uint16_t gesture_flags = apds.getDetectedGestures();
  // Check if sensor read was unsuccessful
  if (gesture_flags == (uint16_t)-1) {
    Serial.println("Sensor read failed, attempting to reinitialize...");
    while (!apds.begin()) {
      Serial.println("Failed to reinitialize APDS9500, retrying...");
      delay(1000);  // Delay for 1 second before retrying
    }
    Serial.println("APDS9500 reinitialized successfully.");
    return;  // Return to the start of the loop after reinitialization
  }
  if (gesture_flags != 0) { // If any gesture is detected, start blinking
    shouldBlink = true;
    ledBlinkMillis = millis(); // Reset the blink timer
  }

  if (shouldBlink) {
    unsigned long currentMillis = millis();
    if (currentMillis - ledBlinkMillis >= blinkInterval) {
      ledBlinkState = !ledBlinkState;
      digitalWrite(Green, ledBlinkState);
      digitalWrite(Red, ledBlinkState);
      ledBlinkMillis = currentMillis;
      if (!ledBlinkState) { // Stop blinking after one cycle
        shouldBlink = false;
      }
    }
  }

  if (gesture_flags & 0x01)
  {
    Serial.println("Down event detected");
    Keyboard.write(KEY_DOWN_ARROW);
  } 
  if (gesture_flags & 0x02){
     Serial.println("Up event detected");
    Keyboard.write(KEY_UP_ARROW);
  }
  if (gesture_flags & 0x04){
    Serial.println("Right event detected");
    Keyboard.write(KEY_RIGHT_ARROW);
  }
  if (gesture_flags & 0x08){
    Serial.println("Left event detected");
    Keyboard.write(KEY_LEFT_ARROW);
  }

  char key_space = ' ';
  if (gesture_flags & 0x10){
    Serial.println("FORWARD event detected");
    Keyboard.write(key_space);
  }
  if (gesture_flags & 0x20)
  {  
    Serial.println("BACKWARD event detected");
  } 

  if (gesture_flags & 0x40) Serial.println("CLOCKWISE event detected");
  if (gesture_flags & 0x80) Serial.println("COUNTERCLOCKWISE event detected");

  if ((gesture_flags >> 8) & 0x01) Serial.println("WAVE event detected");
  if ((gesture_flags >> 8) & 0x02) Serial.println("PROXIMITY event detected");
  if ((gesture_flags >> 8) & 0x04) Serial.println("HAS OBJECT event detected");
  if ((gesture_flags >> 8) & 0x08) Serial.println("WAKE UP TRIGGER event detected");
  if ((gesture_flags >> 8) & 0x80) Serial.println("NO OBJECT event detected");

  unsigned long currentMillis = millis();
  unsigned long interval = ledState ? onInterval : offInterval;
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(Blue, ledState);
  }
  delay(100);
}
