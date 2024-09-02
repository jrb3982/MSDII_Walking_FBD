/*
   -------------------------------------------------------------------------------------
   HX711_ADC
   Arduino library for HX711 24-Bit Analog-to-Digital Converter for Weight Scales
   Olav Kallhovd sept2017
   -------------------------------------------------------------------------------------
*/

/*
   Settling time (number of samples) and data filtering can be adjusted in the config.h file
   For calibration and storing the calibration value in eeprom, see example file "Calibration.ino"

   The update() function checks for new data and starts the next conversion. In order to acheive maximum effective
   sample rate, update() should be called at least as often as the HX711 sample rate; >10Hz@10SPS, >80Hz@80SPS.
   If you have other time consuming code running (i.e. a graphical LCD), consider calling update() from an interrupt routine,
   see example file "Read_1x_load_cell_interrupt_driven.ino".

   This is an example sketch on how to use this library
   Can safely Handle 150% overload (max sensor reading=175)
*/

#include <HX711_ADC.h>
#include <FastLED.h>
#include "math.h"

#define NUM_LEDS	40
#define MAX_SENSOR 	75000.0f
#define MIN_SENSOR 	0.0f
#define MIN_LEDS 	1.0f
#define LED_PIN 4

#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

int seconds = 0;
//pins (might have to change dout pin):
const int HX711_dout = 4; //mcu > HX711 dout pin
const int HX711_sck = 5; //mcu > HX711 sck pin

//HX711 constructor:
HX711_ADC LoadCell(HX711_dout, HX711_sck);

const int calVal_eepromAdress = 0;
unsigned long t = 0;

void setup() {
  Serial.begin(57600); delay(10);
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  pinMode(BUTTON_REGULAR_PIN, INPUT_PULLUP);
  pinMode(BUTTON_BLUE_GREEN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_RED_YELLOW_PIN, INPUT_PULLUP);
  //populate lookup table with #LEDs on for a given force
  for (int i = 0; i < MAX_SENSOR; i++){
    //num LEDs on determined via interpolation scaling
    float float_leds_on = ((NUM_LEDS - MIN_LEDS) / (MAX_SENSOR - MIN_SENSOR)) * (i - MIN_SENSOR) + MIN_LEDS;
    numLedsLUT[i] = (int) round(float_leds_on); //populate LUT
  }

  Serial.println();
  Serial.println("Starting...");

  LoadCell.begin();
  //LoadCell.setReverseOutput(); //uncomment to turn a negative output value to positive
  float calibrationValue; // calibration value (see example file "Calibration.ino")
  calibrationValue = 696.0; // uncomment this if you want to set the calibration value in the sketch
#if defined(ESP8266)|| defined(ESP32)
  //EEPROM.begin(512); // uncomment this if you use ESP8266/ESP32 and want to fetch the calibration value from eeprom
#endif
  //EEPROM.get(calVal_eepromAdress, calibrationValue); // uncomment this if you want to fetch the calibration value from eeprom

  unsigned long stabilizingtime = 2000; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
  boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1);
  }
  else {
    LoadCell.setCalFactor(calibrationValue); // set calibration value (float)
    Serial.println("Startup is complete");
  }
}

void loop() {
  static boolean newDataReady = 0;
  const int serialPrintInterval = 0; //increase value to slow down serial print activity
  int getSmooth = 0;
  int temp;
  if (digitalRead(BUTTON_REGULAR_PIN) == LOW) {
        currentPattern = REGULAR;
        Serial.println("Regular gradient selected.");
        delay(200); // Debounce delay
    }

    // Check if the blue-green pattern button is pressed
    if (digitalRead(BUTTON_BLUE_GREEN_PIN) == LOW) {
        currentPattern = BLUE_GREEN;
        Serial.println("Blue-green gradient selected.");
        delay(200); // Debounce delay
    }

    // Check if the red-yellow pattern button is pressed
    if (digitalRead(BUTTON_RED_YELLOW_PIN) == LOW) {
        currentPattern = RED_YELLOW;
        Serial.println("Red-yellow gradient selected.");
        delay(200); // Debounce delay
    }
  while (getSmooth < 10){
    temp = LoadCell.getData();
    if (temp > MIN_SENSOR){
      sensor += temp;
      getSmooth++;
    }
  }

  sensor = (int)(sensor/10);
  //Display LEDs/OLED according to sensor value
  switch (currentPattern) {
        case REGULAR:
            processSensorData(numLedsLUT[sensor]);
            break;
        case BLUE_GREEN:
            processSensorDataBlueGreen(numLedsLUT[sensor]);
            break;
        case RED_YELLOW:
            processSensorDataRedYellow(numLedsLUT[sensor]);
            break;
    }
  sensor = 0;

}
/*
Displays Sensor Value on LEDs
Input: number of LEDs on (From LUT)
*/
void processSensorData(int number_leds_on) {
    for (int i = 0; i < NUM_LEDS; i++) { // for each LED in the strip
        if (i < number_leds_on) { // if our current LED should be ON
            if ((i + 1) <= (NUM_LEDS * 0.05)) { // bottom 5%, color BLUE
                leds[i] = CRGB(0, 0, 255);
            } else if ((i + 1) <= (NUM_LEDS * 0.10)) { // next 5%, color GREEN-BLUE
                leds[i] = CRGB(0, 128, 128);
            } else if ((i + 1) <= (NUM_LEDS * 0.20)) { // next 10%, color GREEN
                leds[i] = CRGB(0, 255, 0);
            } else if ((i + 1) <= (NUM_LEDS * 0.30)) { // next 10%, color YELLOW-GREEN
                leds[i] = CRGB(128, 255, 0);
            } else if ((i + 1) <= (NUM_LEDS * 0.40)) { // next 10%, color YELLOW
                leds[i] = CRGB(255, 255, 0);
            } else if ((i + 1) <= (NUM_LEDS * 0.60)) { // next 20%, color ORANGE
                leds[i] = CRGB(255, 165, 0);
            } else { // top 40%, color RED
                leds[i] = CRGB(255, 0, 0);
            }
        } else { // outside of range of ON LEDs, turn off
            leds[i] = CRGB(0, 0, 0);
        }
    }
    FastLED.show();
}

void processSensorDataBlueGreen(int number_leds_on) {
    for (int i = 0; i < NUM_LEDS; i++) { // for each LED in the strip
        if (i < number_leds_on) { // if our current LED should be ON
            leds[i] = CRGB(0, (255 * i) / NUM_LEDS, (255 * (NUM_LEDS - i)) / NUM_LEDS);
        } else { // outside of range of ON LEDs, turn off
            leds[i] = CRGB(0, 0, 0);
        }
    }
    FastLED.show();
}

void processSensorDataRedYellow(int number_leds_on) {
    for (int i = 0; i < NUM_LEDS; i++) { // for each LED in the strip
        if (i < number_leds_on) { // if our current LED should be ON
            if ((i + 1) <= (NUM_LEDS * 0.30)) { // bottom 30%, color YELLOW
                leds[i] = CRGB(255, 255, 0);
            } else if ((i + 1) <= (NUM_LEDS * 0.60)) { // middle 40%, color ORANGE
                leds[i] = CRGB(255, 165, 0);
            } else { // top 30%, color RED
                leds[i] = CRGB(255, 0, 0);
            }
        } else { // outside of range of ON LEDs, turn off
            leds[i] = CRGB(0, 0, 0);
        }
    }
    FastLED.show();
}












