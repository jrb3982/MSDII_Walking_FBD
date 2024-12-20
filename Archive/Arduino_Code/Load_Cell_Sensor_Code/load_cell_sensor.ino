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
#define NUM_LEDS	20
#define MAX_SENSOR  3000.0f
//#define MIN_SENSOR 	20.0f
#define MIN_LEDS 	1.0f
#define LED_PIN 12
#define LED_PIN_2 13
#define LED_PIN_3 11
#define CALIBRATION_PIN 10
#define BUTTON_REGULAR_PIN 14
#define BUTTON_BLUE_GREEN_PIN 15
#define BUTTON_RED_YELLOW_PIN 16
#define BUTTON_COOL_SCHEME_PIN 17



#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

int delayval = 100;
int user_weight=0;
int sensor;
int sensor_2;
int sensor_3;


CRGB leds1[NUM_LEDS];
CRGB leds2[NUM_LEDS];
CRGB leds3[NUM_LEDS];

int numLedsLUT[(int)MAX_SENSOR];
enum Pattern { REGULAR, BLUE_GREEN, RED_YELLOW,COOL_SCHEME };
Pattern currentPattern = REGULAR;
float MIN_SENSOR=20.0f;

//pins:
const int HX711_dout_1 = 4; //mcu > HX711 no 1 dout pin
const int HX711_sck_1 = 5; //mcu > HX711 no 1 sck pin
const int HX711_dout_2 = 6; //mcu > HX711 no 2 dout pin
const int HX711_sck_2 = 7; //mcu > HX711 no 2 sck pin
const int HX711_dout_3 = 2; //mcu > HX711 no 1 dout pin
const int HX711_sck_3 = 3; //mcu > HX711 no 1 sck pin
const int HX711_dout_4 = 8; //mcu > HX711 no 2 dout pin
const int HX711_sck_4 = 9; //mcu > HX711 no 2 sck pin

//HX711 constructor (dout pin, sck pin)
HX711_ADC LoadCell_1(HX711_dout_1, HX711_sck_1); //HX711 1
HX711_ADC LoadCell_2(HX711_dout_2, HX711_sck_2); //HX711 2
HX711_ADC LoadCell_3(HX711_dout_3, HX711_sck_3); //HX711 1
HX711_ADC LoadCell_4(HX711_dout_4, HX711_sck_4); //HX711 2



const int calVal_eepromAdress_1 = 0; // eeprom adress for calibration value load cell 1 (4 bytes)
const int calVal_eepromAdress_2 = 4; // eeprom adress for calibration value load cell 2 (4 bytes)
const int calVal_eepromAdress_3 = 8; // eeprom adress for calibration value load cell 2 (4 bytes)
const int calVal_eepromAdress_4 = 12; // eeprom adress for calibration value load cell 2 (4 bytes)


unsigned long t = 0;

void setup() {
  Serial.begin(57600); delay(10);
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds1, NUM_LEDS);
  FastLED.addLeds<WS2812, LED_PIN_2, GRB>(leds2, NUM_LEDS);
  FastLED.addLeds<WS2812, LED_PIN_3, GRB>(leds3, NUM_LEDS);
   
  pinMode(BUTTON_REGULAR_PIN, INPUT_PULLUP);
  pinMode(BUTTON_BLUE_GREEN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_RED_YELLOW_PIN, INPUT_PULLUP);
  pinMode(BUTTON_COOL_SCHEME_PIN,INPUT_PULLUP);
  pinMode(CALIBRATION_PIN,INPUT_PULLUP);
  //populate lookup table with #LEDs on for a given force
  for (int i = 0; i < MAX_SENSOR; i++){
    //num LEDs on determined via interpolation scaling
    float float_leds_on = ((NUM_LEDS - MIN_LEDS) / (MAX_SENSOR - MIN_SENSOR)) * (i - MIN_SENSOR) + MIN_LEDS;
    numLedsLUT[i] = (int) round(float_leds_on); //populate LUT
  }

  float calibrationValue_1; // calibration value load cell 1
  float calibrationValue_2; // calibration value load cell 2
  float calibrationValue_3;
  float calibrationValue_4;

  calibrationValue_1 = -97.02; // uncomment this if you want to set this value in the sketch
  calibrationValue_2 = -74.65; // uncomment this if you want to set this value in the sketch
  calibrationValue_3 = -97.02; // uncomment this if you want to set this value in the sketch
  calibrationValue_4 = -74.65; // uncomment this if you want to set this value in the sketch
  
#if defined(ESP8266) || defined(ESP32)
  //EEPROM.begin(512); // uncomment this if you use ESP8266 and want to fetch the value from eeprom
#endif
  //EEPROM.get(calVal_eepromAdress_1, calibrationValue_1); // uncomment this if you want to fetch the value from eeprom
  //EEPROM.get(calVal_eepromAdress_2, calibrationValue_2); // uncomment this if you want to fetch the value from eeprom

  LoadCell_1.begin();
  LoadCell_2.begin();
  LoadCell_3.begin();
  LoadCell_4.begin();
  //LoadCell_1.setReverseOutput();
  //LoadCell_2.setReverseOutput();
  unsigned long stabilizingtime = 2000; // tare preciscion can be improved by adding a few seconds of stabilizing time
  boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
  byte loadcell_1_rdy = 0;
  byte loadcell_2_rdy = 0;
  byte loadcell_3_rdy = 0;
  byte loadcell_4_rdy = 0;
  while ((loadcell_1_rdy + loadcell_2_rdy +loadcell_3_rdy +loadcell_4_rdy) < 4) { //run startup, stabilization and tare, both modules simultaniously
    if (!loadcell_1_rdy) loadcell_1_rdy = LoadCell_1.startMultiple(stabilizingtime, _tare);
    if (!loadcell_2_rdy) loadcell_2_rdy = LoadCell_2.startMultiple(stabilizingtime, _tare);
    if (!loadcell_3_rdy) loadcell_3_rdy = LoadCell_3.startMultiple(stabilizingtime, _tare);
    if (!loadcell_4_rdy) loadcell_4_rdy = LoadCell_4.startMultiple(stabilizingtime, _tare);
    
  }
 
 
  LoadCell_1.setCalFactor(calibrationValue_1); // user set calibration value (float)
  LoadCell_2.setCalFactor(calibrationValue_2); // user set calibration value (float)
  LoadCell_3.setCalFactor(calibrationValue_3); // user set calibration value (float)
  LoadCell_4.setCalFactor(calibrationValue_4); // user set calibration value (float)
 // Serial.println("Startup is complete");
  
}

void loop() {
  static boolean newDataReady = 0;
  const int serialPrintInterval = 0; //increase value to slow down serial print activity
  int getSmooth = 0;
  int temp;
  float c;
  float d;
  float n;
  
  c=load_cell_data(c);
  d=load_cell_data_2(d);
  if(digitalRead(CALIBRATION_PIN)==LOW){
      n=c+d;
      MIN_SENSOR=n;

  }
  sendValues(c,d);
  if (digitalRead(BUTTON_REGULAR_PIN) == LOW) {
       currentPattern = REGULAR;
       //Serial.println("Regular gradient selected.");
       delay(100); // Debounce delay
   }

  // Check if the blue-green pattern button is pressed
  if (digitalRead(BUTTON_BLUE_GREEN_PIN) == LOW) {
        currentPattern = BLUE_GREEN;
        //Serial.println("Blue-green gradient selected.");
        delay(100); // Debounce delay
  }

  // Check if the red-yellow pattern button is pressed
  if (digitalRead(BUTTON_RED_YELLOW_PIN) == LOW) {
        currentPattern = RED_YELLOW;
        //Serial.println("Red-yellow gradient selected.");
        delay(100); // Debounce delay
  }

   if (digitalRead(BUTTON_COOL_SCHEME_PIN) == LOW) {
        currentPattern = BUTTON_COOL_SCHEME_PIN;
        //Serial.println("Red-yellow gradient selected.");
        delay(100); // Debounce delay
    }

  

  sensor=c;
  sensor_2=d;
  sensor_3=n;
  sensor = (int)(sensor);
  sensor_2=(int)(sensor_2);
  sensor_3=(int)(sensor_3);
 // Serial.print(F("Foot 1 output val: "));
 // Serial.println(sensor);
  // Serial.print(F("Foot 2output val: "));
  //Serial.println(sensor_2);
  //Display LEDs/OLED according to sensor value
  switch (currentPattern) {
        case REGULAR:
            
            processSensorData((((NUM_LEDS - MIN_LEDS) / (MAX_SENSOR - MIN_SENSOR)) * (sensor - MIN_SENSOR) + MIN_LEDS),leds1);
            processSensorData((((NUM_LEDS - MIN_LEDS) / (MAX_SENSOR - MIN_SENSOR)) * (sensor_2 - MIN_SENSOR) + MIN_LEDS),leds2);
            processSensorData((((NUM_LEDS - MIN_LEDS) / (MAX_SENSOR - MIN_SENSOR)) * (sensor_3 - MIN_SENSOR) + MIN_LEDS),leds3);
            break;
        case BLUE_GREEN:
            processSensorDataBlueGreen((((NUM_LEDS - MIN_LEDS) / (MAX_SENSOR - MIN_SENSOR)) * (sensor - MIN_SENSOR) + MIN_LEDS),leds1);
            processSensorDataBlueGreen((((NUM_LEDS - MIN_LEDS) / (MAX_SENSOR - MIN_SENSOR)) * (sensor_2 - MIN_SENSOR) + MIN_LEDS),leds2);
            processSensorDataBlueGreen((((NUM_LEDS - MIN_LEDS) / (MAX_SENSOR - MIN_SENSOR)) * (sensor_3 - MIN_SENSOR) + MIN_LEDS),leds3);
            break;
        case RED_YELLOW:
            processSensorDataRedYellow((((NUM_LEDS - MIN_LEDS) / (MAX_SENSOR - MIN_SENSOR)) * (sensor - MIN_SENSOR) + MIN_LEDS),leds1);
            processSensorDataRedYellow((((NUM_LEDS - MIN_LEDS) / (MAX_SENSOR - MIN_SENSOR)) * (sensor_2 - MIN_SENSOR) + MIN_LEDS),leds2);
            processSensorDataRedYellow((((NUM_LEDS - MIN_LEDS) / (MAX_SENSOR - MIN_SENSOR)) * (sensor_3 - MIN_SENSOR) + MIN_LEDS),leds3);
            break;
        case COOL_SCHEME:
            processSensorDataCool((((NUM_LEDS - MIN_LEDS) / (MAX_SENSOR - MIN_SENSOR)) * (sensor - MIN_SENSOR) + MIN_LEDS),leds1);
            processSensorDataCool((((NUM_LEDS - MIN_LEDS) / (MAX_SENSOR - MIN_SENSOR)) * (sensor_2 - MIN_SENSOR) + MIN_LEDS),leds2);
            processSensorDataCool((((NUM_LEDS - MIN_LEDS) / (MAX_SENSOR - MIN_SENSOR)) * (sensor_3 - MIN_SENSOR) + MIN_LEDS),leds3);
            break;
    }
  sensor = 0;
  sensor_2=0;

}
/*
Displays Sensor Value on LEDs
Input: number of LEDs on (From LUT)
*/
void processSensorData(int number_leds_on, CRGB * leds) {
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

void processSensorDataBlueGreen(int number_leds_on, CRGB * leds) {
    for (int i = 0; i < NUM_LEDS; i++) { // for each LED in the strip
        if (i < number_leds_on) { // if our current LED should be ON
            leds[i] = CRGB(0, (255 * i) / NUM_LEDS, (255 * (NUM_LEDS - i)) / NUM_LEDS);
        } else { // outside of range of ON LEDs, turn off
            leds[i] = CRGB(0, 0, 0);
        }
    }
    FastLED.show();
}

void processSensorDataRedYellow(int number_leds_on, CRGB * leds) {
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

void processSensorDataCool(int number_leds_on, CRGB * leds) {
    for (int i = 0; i < NUM_LEDS; i++) { // for each LED in the strip
        if (i < number_leds_on) { // if our current LED should be ON
            if ((i + 1) <= (NUM_LEDS * 0.25)) { // bottom 25%, color TEAL
                leds[i] = CRGB(0, 128, 128);
            } else if ((i + 1) <= (NUM_LEDS * 0.50)) { // next 25%, color BLUE
                leds[i] = CRGB(0, 0, 255);
            } else if ((i + 1) <= (NUM_LEDS * 0.75)) { // next 25%, color PURPLE
                leds[i] = CRGB(128, 0, 128);
            } else { // top 25%, color LIGHT BLUE
                leds[i] = CRGB(173, 216, 230);
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

  
      
      c = (LoadCell_2.getData()+LoadCell_1.getData())/2;
      
      newDataReady = 0;
      t = millis();
    

  // receive command from serial terminal, send 't' to initiate tare operation:
  if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') {
      LoadCell_1.tareNoDelay();
      LoadCell_2.tareNoDelay();
    }
  }

 
  return c;
}
float load_cell_data_2(float d){
static boolean newDataReady = 0;
  const int serialPrintInterval = 0; //increase value to slow down serial print activity

  // check for new data/start next conversion:
  if (LoadCell_3.update()) newDataReady = true;
  LoadCell_4.update();

  
      
      d = (LoadCell_3.getData()+LoadCell_4.getData())/2;
     
      newDataReady = 0;
      t = millis();
   

  // receive command from serial terminal, send 't' to initiate tare operation:
  if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') {
      LoadCell_3.tareNoDelay();
      LoadCell_4.tareNoDelay();
    }
  }

  return d;
}
void sendValues(float forceL, float forceR) {
    forceL=(0.001* 9.98*forceL);
    forceR=(.001* 9.98*forceR);
    // Send Left Value
    Serial.print("S"); // Identifier for Left
    Serial.print(",");
    Serial.print(forceL); // Send the float value
    Serial.print("\n"); // End the transmission for Left

    // Send Right Value
    Serial.print(""); // Identifier for Right
    Serial.print(",");
    Serial.print(forceR); // Send the float value
    Serial.print("\n"); // End the transmission for Right
}