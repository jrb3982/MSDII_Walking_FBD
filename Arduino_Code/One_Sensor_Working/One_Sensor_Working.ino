// C++ code
//
//#include <Adafruit_NeoPixel.h>
//#include <Adafruit_LiquidCrystal.h>
#include <FastLED.h>
#include "math.h"

int seconds = 0;



#define NUM_LEDS	20
#define MAX_SENSOR 	100.0f
#define MIN_SENSOR 	10.0f
#define MIN_LEDS 	1.0f
#define LED_PIN 4

int delayval = 100;

int sensor;

CRGB leds[NUM_LEDS];

int numLedsLUT[(int)MAX_SENSOR];

void setup()
{
  Serial.begin(9600);
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
 // lcd_1.begin(16, 2);
  
  //lcd_1.print("sensor value: ");


 
  pinMode(A0, INPUT);
  
  //pixels1.begin();
  //pixels2.begin();
  //pixels3.begin();
  //pixels4.begin();
  //populate lookup table with #LEDs on for a given force
  for (int i = 0; i < MAX_SENSOR; i++){
    //num LEDs on determined via interpolation scaling
    float float_leds_on = ((NUM_LEDS - MIN_LEDS) / (MAX_SENSOR - MIN_SENSOR)) * (i - MIN_SENSOR) + MIN_LEDS;
    numLedsLUT[i] = (int) round(float_leds_on); //populate LUT
  }
 
}

void loop()
{
  int getSmooth = 0;
  int temp;
  while (getSmooth < 10 ){
    temp = analogRead(A0);
    if (temp > MIN_SENSOR){
      sensor += temp;
      getSmooth++;
    }
  }

  sensor = (int)(sensor/10);
  //Display LEDs/OLED according to sensor value
  processSensorData(numLedsLUT[sensor]);
  sensor = 0;
}

void processSensorData(int number_leds_on){
  for (int i = 0; i < NUM_LEDS; i++){ //for each LED in the strip
    if (i < number_leds_on){ //if our current LED should be ON
      if ( (i+1) <  (NUM_LEDS/4)){ //if in the bottom quarter of LEDs, color BLUE
        leds[i] = CRGB(0,0,255);
        FastLED.show();
        delay(2);
      } else if ( (i+1) >= (NUM_LEDS/4) && (i+1) <= (NUM_LEDS/2)) { // between bottom quarter and half, color GREEN
        leds[i] = CRGB(0,255,0);
        FastLED.show();
        delay(2);
      } else { //top 50% color red (we can change these percentages later
        leds[i] = CRGB(255,0,0);
        FastLED.show();
        delay(2);
      }
    } else { //outside of range of ON LEDs, turn off
        leds[i] = CRGB(0,0,0);
        FastLED.show();
        delay(2);
    }
  }  
    //lcd_1.clear(); //clear previous sensor reading
  	Serial.print("\r\nsensor: \r\n"); //display sensor reading
  	Serial.print(sensor);
}