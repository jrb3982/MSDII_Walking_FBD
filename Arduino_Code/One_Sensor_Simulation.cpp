// C++ code
//
#include <Adafruit_NeoPixel.h>
#include <Adafruit_LiquidCrystal.h>
#include "math.h"

int seconds = 0;

Adafruit_LiquidCrystal lcd_1(0);


#define LIGHT1 4
#define LIGHT2 5
#define LIGHT3 6
#define LIGHT4 7

#define NUMPIXELS	8
#define MAX_SENSOR 	466.0f
#define MIN_SENSOR 	0.0f
#define MIN_LEDS 	1.0f

int delayval = 100;

Adafruit_NeoPixel pixels1 = Adafruit_NeoPixel(NUMPIXELS, LIGHT1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels2 = Adafruit_NeoPixel(NUMPIXELS, LIGHT2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels3 = Adafruit_NeoPixel(NUMPIXELS, LIGHT3, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels4 = Adafruit_NeoPixel(NUMPIXELS, LIGHT4, NEO_GRB + NEO_KHZ800);



int redColor = 0;
int greenColor = 0;
int blueColor = 0;

int sensor = 0;

int numLedsLUT[(int)MAX_SENSOR];

void setup()
{
  Serial.begin(9600);
  
  lcd_1.begin(16, 2);
  
  //lcd_1.print("sensor value: ");


 
  pinMode(A0, INPUT);
  
  pixels1.begin();
  //pixels2.begin();
  //pixels3.begin();
  //pixels4.begin();
  //populate lookup table with #LEDs on for a given force
  for (int i = 0; i < MAX_SENSOR; i++){
    //num LEDs on determined via interpolation scaling
    float float_leds_on = ((NUMPIXELS - MIN_LEDS) / (MAX_SENSOR - MIN_SENSOR)) * (i - MIN_SENSOR) + MIN_LEDS;
    numLedsLUT[i] = (int) round(float_leds_on); //populate LUT
  }
 
}

void loop()
{
  sensor = analogRead(A0);
  //Serial.print("Analog value: ");
  //Serial.println(sensor);
  
  pixels1.show();
  pixels2.show();
  pixels3.show();
  pixels4.show();
  //Display LEDs/OLED according to sensor value
  processSensorData(numLedsLUT[sensor]);
  /**
  if ((sensor< 200) && (sensor>100))
  {
   
    for (int i=0; i < NUMPIXELS; i++) {
      pixels1.setPixelColor(i, pixels1.Color(0, 0, 255));
      pixels2.setPixelColor(i, pixels3.Color(0,0,0));
      pixels3.setPixelColor(i, pixels3.Color(0,0,0));
      pixels4.setPixelColor(i, pixels3.Color(0,0,0));
      pixels1.show();
      pixels2.show();
      pixels3.show();
      pixels4.show();
    }
      
  }
  
  else if (sensor<300)
  {
    
    for (int i=0; i <NUMPIXELS; i++) {
      pixels1.setPixelColor(i, pixels1.Color(0,0,255));
      pixels2.setPixelColor(i, pixels2.Color(0,255,0));
      pixels3.setPixelColor(i, pixels3.Color(0,0,0));
      pixels4.setPixelColor(i, pixels3.Color(0,0,0));
      pixels1.show();
      pixels2.show();
      pixels3.show();
      pixels4.show();
    }

  }
  
  else if (sensor<400)
  {
    
    for (int i=0; i <NUMPIXELS; i++) {
      pixels1.setPixelColor(i, pixels1.Color(0,0,255));
      pixels2.setPixelColor(i, pixels2.Color(0,255,0));
      pixels3.setPixelColor(i, pixels3.Color(0,255,0));
      pixels4.setPixelColor(i, pixels3.Color(0,0,0));
      pixels1.show();
      pixels2.show();
      pixels3.show();
      pixels4.show();
    }
  }
  
  else if (sensor < 500)
  {
   
    for (int i=0; i <NUMPIXELS; i++) {
      pixels1.setPixelColor(i, pixels1.Color(0,0,255));
      pixels2.setPixelColor(i, pixels2.Color(0,255,0));
      pixels3.setPixelColor(i, pixels3.Color(0,255,0));
      pixels4.setPixelColor(i, pixels4.Color(255,0,0));
      pixels1.show();
      pixels2.show();
      pixels3.show();
      pixels4.show();
    }
  }
  lcd_1.clear();
  lcd_1.print("sensor: ");
  lcd_1.print(sensor);
**/
}

void processSensorData(int number_leds_on){
  for (int i = 0; i < NUMPIXELS; i++){ //for each LED in the strip
    if (i < number_leds_on){ //if our current LED should be ON
      if ( (i+1) <  (NUMPIXELS/4)){ //if in the bottom quarter of LEDs, color BLUE
        pixels1.setPixelColor(i, pixels1.Color(0,0,255));
      } else if ( (i+1) >= (NUMPIXELS/4) && (i+1) <= (NUMPIXELS/2)) { // between bottom quarter and half, color GREEN
        pixels1.setPixelColor(i, pixels1.Color(0,255,0));
      } else { //top 50% color red (we can change these percentages later
        pixels1.setPixelColor(i, pixels1.Color(255,00,0));
      }
    } else { //outside of range of ON LEDs, turn off
      pixels1.setPixelColor(i, pixels1.Color(0,0,0));
    }
  }  
    lcd_1.clear(); //clear previous sensor reading
  	lcd_1.print("sensor: "); //display sensor reading
  	lcd_1.print(sensor);
}

