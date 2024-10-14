/*
   -------------------------------------------------------------------------------------
   HX711_ADC
   Arduino library for HX711 24-Bit Analog-to-Digital Converter for Weight Scales
   Olav Kallhovd sept2017
   -------------------------------------------------------------------------------------
*/

/*/*
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

#define GRAVITY     9.80f
#define NUM_LEDS	40
#define MAX_SENSOR  1500.0f
#define MIN_SENSOR 	0.0f
#define MIN_LEDS 	1.0f
#define LED_PIN 12



#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

int delayval = 100;

int sensor;

CRGB leds[NUM_LEDS];

int numLedsLUT[(int)MAX_SENSOR];
enum Pattern { REGULAR, BLUE_GREEN, RED_YELLOW };
Pattern currentPattern = REGULAR;


//pins:
const int HX711_dout_1 = 4; //mcu > HX711 no 1 dout pin
const int HX711_sck_1 = 5; //mcu > HX711 no 1 sck pin
const int HX711_dout_2 = 6; //mcu > HX711 no 2 dout pin
const int HX711_sck_2 = 7; //mcu > HX711 no 2 sck pin

//HX711 constructor (dout pin, sck pin)
HX711_ADC LoadCell_1(HX711_dout_1, HX711_sck_1); //HX711 1
HX711_ADC LoadCell_2(HX711_dout_2, HX711_sck_2); //HX711 2

const int calVal_eepromAdress_1 = 0; // eeprom adress for calibration value load cell 1 (4 bytes)
const int calVal_eepromAdress_2 = 4; // eeprom adress for calibration value load cell 2 (4 bytes)
unsigned long t = 0;

void setup() {
  Serial.begin(57600); delay(10);
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  // pinMode(BUTTON_REGULAR_PIN, INPUT_PULLUP);
  // pinMode(BUTTON_BLUE_GREEN_PIN, INPUT_PULLUP);
  // pinMode(BUTTON_RED_YELLOW_PIN, INPUT_PULLUP);
  //populate lookup table with #LEDs on for a given force
  for (int i = 0; i < MAX_SENSOR; i++){
    //num LEDs on determined via interpolation scaling
    float float_leds_on = ((NUM_LEDS - MIN_LEDS) / (MAX_SENSOR - MIN_SENSOR)) * (i - MIN_SENSOR) + MIN_LEDS;
    numLedsLUT[i] = (int) round(float_leds_on); //populate LUT
  }

  float calibrationValue_1; // calibration value load cell 1
  float calibrationValue_2; // calibration value load cell 2

  calibrationValue_1 = -97.02; // uncomment this if you want to set this value in the sketch
  calibrationValue_2 = -74.65; // uncomment this if you want to set this value in the sketch
#if defined(ESP8266) || defined(ESP32)
  //EEPROM.begin(512); // uncomment this if you use ESP8266 and want to fetch the value from eeprom
#endif
  //EEPROM.get(calVal_eepromAdress_1, calibrationValue_1); // uncomment this if you want to fetch the value from eeprom
  //EEPROM.get(calVal_eepromAdress_2, calibrationValue_2); // uncomment this if you want to fetch the value from eeprom

  LoadCell_1.begin();
  LoadCell_2.begin();
  //LoadCell_1.setReverseOutput();
  //LoadCell_2.setReverseOutput();
  unsigned long stabilizingtime = 2000; // tare preciscion can be improved by adding a few seconds of stabilizing time
  boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
  byte loadcell_1_rdy = 0;
  byte loadcell_2_rdy = 0;
  while ((loadcell_1_rdy + loadcell_2_rdy) < 2) { //run startup, stabilization and tare, both modules simultaniously
    if (!loadcell_1_rdy) loadcell_1_rdy = LoadCell_1.startMultiple(stabilizingtime, _tare);
    if (!loadcell_2_rdy) loadcell_2_rdy = LoadCell_2.startMultiple(stabilizingtime, _tare);
  }
  if (LoadCell_1.getTareTimeoutFlag()) {
   // Serial.println("Timeout, check MCU>HX711 no.1 wiring and pin designations");
  }
  if (LoadCell_2.getTareTimeoutFlag()) {
    //Serial.println("Timeout, check MCU>HX711 no.2 wiring and pin designations");
  }
  LoadCell_1.setCalFactor(calibrationValue_1); // user set calibration value (float)
  LoadCell_2.setCalFactor(calibrationValue_2); // user set calibration value (float)
 // Serial.println("Startup is complete");
  
}

void loop() {
  static boolean newDataReady = 0;
  const int serialPrintInterval = 0; //increase value to slow down serial print activity
  int getSmooth = 0;
  int temp;
  float c;

  c=load_cell_data(c);
  //if (digitalRead(BUTTON_REGULAR_PIN) == LOW) {
        currentPattern = REGULAR;
    //    Serial.println("Regular gradient selected.");
    //    delay(200); // Debounce delay
   // }

    // // Check if the blue-green pattern button is pressed
    // if (digitalRead(BUTTON_BLUE_GREEN_PIN) == LOW) {
    //     currentPattern = BLUE_GREEN;
    //     Serial.println("Blue-green gradient selected.");
    //     delay(200); // Debounce delay
    // }

    // // Check if the red-yellow pattern button is pressed
    // if (digitalRead(BUTTON_RED_YELLOW_PIN) == LOW) {
    //     currentPattern = RED_YELLOW;
    //     Serial.println("Red-yellow gradient selected.");
    //     delay(200); // Debounce delay
    // }
  sensor=c;

  sensor = (int)(sensor);
  Serial.println(sensor);
  //Display LEDs/OLED according to sensor value
  switch (currentPattern) {
        case REGULAR:
            
            processSensorData(((NUM_LEDS - MIN_LEDS) / (MAX_SENSOR - MIN_SENSOR)) * (sensor - MIN_SENSOR) + MIN_LEDS);
            break;
        case BLUE_GREEN:
            processSensorDataBlueGreen(((NUM_LEDS - MIN_LEDS) / (MAX_SENSOR - MIN_SENSOR)) * (sensor - MIN_SENSOR) + MIN_LEDS);
            break;
        case RED_YELLOW:
            processSensorDataRedYellow(((NUM_LEDS - MIN_LEDS) / (MAX_SENSOR - MIN_SENSOR)) * (sensor - MIN_SENSOR) + MIN_LEDS);
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

float load_cell_data(float c){
static boolean newDataReady = 0;
  const int serialPrintInterval = 0; //increase value to slow down serial print activity

  // check for new data/start next conversion:
  if (LoadCell_1.update()) newDataReady = true;
  LoadCell_2.update();

  //get smoothed value from data set
  //if ((newDataReady)) {
    //if (millis() > t + serialPrintInterval) {
      
      c = (LoadCell_2.getData()+LoadCell_1.getData())/2;
      // Serial.print(F("Load_cell 1 output val: "));
      // Serial.println(LoadCell_2.getData());
      // Serial.println(F("    Load_cell 2 output val: "));
      // Serial.println(LoadCell_1.getData());
      // Serial.println(F("    Load_cell Average output val: "));
      // Serial.println(c);
      newDataReady = 0;
      t = millis();
    //}
  //}

  // receive command from serial terminal, send 't' to initiate tare operation:
  if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') {
      LoadCell_1.tareNoDelay();
      LoadCell_2.tareNoDelay();
    }
  }

  //check if last tare operation is complete
  if (LoadCell_1.getTareStatus() == true) {
    Serial.println("Tare load cell 1 complete");
  }
  if (LoadCell_2.getTareStatus() == true) {
    Serial.println("Tare load cell 2 complete");
  }
  return c;
}