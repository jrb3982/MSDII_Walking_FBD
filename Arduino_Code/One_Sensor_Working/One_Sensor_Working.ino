// C++ code
//
//#include <Adafruit_NeoPixel.h>
//#include <Adafruit_LiquidCrystal.h>
#include <FastLED.h>
#include "math.h"

int seconds = 0;



#define NUM_LEDS	40
#define MAX_SENSOR 	430.0f
#define MIN_SENSOR 	0.0f
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
/**
void loop(){
  //use this to gather your highest and lowest values. Use these to update min and max sensor macros.
  int temp = analogRead(A0);
  Serial.print("\r\nsensor: \r\n"); //display sensor reading
  Serial.print(temp);
  delay(100);
}
**/

void loop()
{
  int getSmooth = 0;
  int temp;
  while (getSmooth < 10){
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
  for (int i = 0; i < NUM_LEDS; i++){ // for each LED in the strip
    if (i < number_leds_on){ // if our current LED should be ON
      if ((i+1) <= (NUM_LEDS * 0.05)){ // bottom 5%, color BLUE
        leds[i] = CRGB(0, 0, 255);
      } else if ((i+1) <= (NUM_LEDS * 0.10)) { // next 5%, color GREEN-BLUE
        leds[i] = CRGB(0, 128, 128);
      } else if ((i+1) <= (NUM_LEDS * 0.20)) { // next 10%, color GREEN
        leds[i] = CRGB(0, 255, 0);
      } else if ((i+1) <= (NUM_LEDS * 0.30)) { // next 10%, color YELLOW-GREEN
        leds[i] = CRGB(128, 255, 0);
      } else if ((i+1) <= (NUM_LEDS * 0.40)) { // next 10%, color YELLOW
        leds[i] = CRGB(255, 255, 0);
      } else if ((i+1) <= (NUM_LEDS * 0.60)) { // next 20%, color ORANGE
        leds[i] = CRGB(255, 165, 0);
      } else { // top 40%, color RED
        leds[i] = CRGB(255, 0, 0);
      }
      FastLED.show();
      delay(2);
    } else { // outside of range of ON LEDs, turn off
      leds[i] = CRGB(0, 0, 0);
      FastLED.show();
      delay(2);
    }
  }  
  
  // Clear previous sensor reading on LCD
  //lcd_1.clear();
  
  // Display sensor reading
  Serial.print("\r\nsensor: \r\n");
  Serial.print(sensor);
}





