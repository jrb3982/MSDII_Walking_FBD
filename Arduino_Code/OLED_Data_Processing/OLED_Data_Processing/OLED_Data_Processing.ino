#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
#define CALIB_BUTTON 5

U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/13, /* data=*/11, /* cs=*/10, /* dc=*/9, /* reset=*/8);

float highestValueL = 0, highestValueR = 0;  // Variables to store the highest values
float highestValueS = 0, highestValueT = 0;
unsigned long previousMillis = 0;  // Store the last time the highest value was reset
const long interval = 1000;        // Interval for capturing and displaying the highest value (1 second)

void setup() {
  Serial.begin(115200);
  pinMode(CALIB_BUTTON, INPUT_PULLUP);
  u8g2.begin();
}

void loop() {
  //Check if the regular pattern button is pressed
  if (digitalRead(CALIB_BUTTON) == LOW) {
    u8g2.begin();
    delay(200);  // Debounce delay
  }
  // Check if there is serial data to read
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');  // Read until newline
    parseData(data);                             // Parse the received data
  }

  // Handle the 1-second display logic
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    u8g2.clearBuffer();  // clear the internal memory

    u8g2.setFont(u8g2_font_ncenB08_tr);   // choose a suitable font
    u8g2.drawStr(0, 10, "Prof. Nebula");  // write something to the internal memory

    // Display Left Arm with highest value
    char leftArm[20];
    dtostrf(highestValueL, 6, 2, leftArm);  // Convert float to string with 2 decimal places
    u8g2.drawStr(0, 25, "Lert Arm (N): ");  // Label
    u8g2.drawStr(80, 25, leftArm);          // write Left Arm value to the display

    // Display Right Arm with highest value
    char rightArm[20];
    dtostrf(highestValueR, 6, 2, rightArm);  // Convert float to string with 2 decimal places
    u8g2.drawStr(0, 35, "Right Arm (N): ");  // Label
    u8g2.drawStr(80, 35, rightArm);          // write Right Arm value to the display

    char leftLeg[20];
    dtostrf(highestValueS, 6, 2, leftLeg);  // Convert float to string with 2 decimal places
    u8g2.drawStr(0, 45, "Left Leg (N): ");  // Placeholder text
    u8g2.drawStr(80, 45, leftLeg);          // write Right Arm value to the display

    char rightLeg[20];
    dtostrf(highestValueT, 6, 2, rightLeg);  // Convert float to string with 2 decimal places
    u8g2.drawStr(0, 55, "Right Leg (N): ");  // Placeholder text
    u8g2.drawStr(80, 55, rightLeg);          // write Right Arm value to the display

    u8g2.sendBuffer();  // transfer internal memory to the display

    // Reset the highest values for the next second
    highestValueL = 0;
    highestValueR = 0;
    highestValueS = 0;
    highestValueT = 0;
  }
}

void parseData(String data) {
  int commaIndex = data.indexOf(',');  // Find the comma
  if (commaIndex > 0) {
    String identifier = data.substring(0, 1);                // Get the identifier (L or R)
    float value = data.substring(commaIndex + 1).toFloat();  // Get the float value

    // Update the highest value based on the identifier
    if (identifier == "L") {
      if (value > highestValueL) {
        highestValueL = value;
      }
    } else if (identifier == "R") {
      if (value > highestValueR) {
        highestValueR = value;
      }
    } else if (identifier == "S") {
      if (value > highestValueS) {
        highestValueS = value;
      }
    } else if (identifier == "T") {
      if (value > highestValueT) {
        highestValueT = value;
      }
    }
  }
}
