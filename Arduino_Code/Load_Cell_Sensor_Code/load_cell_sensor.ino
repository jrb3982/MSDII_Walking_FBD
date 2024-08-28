#include <FastLED.h>
#include "math.h"

#define NUM_LEDS 60

//have to determine maximum weight applied 
#define MAX_SENSOR 
#define MIN_SENSOR 0.0f
#define SCK_PIN 4

int sensor;

CRGB leds[NUM_LEDS];
int numLedsLUT[(int)MAX_SENSOR];

enum Pattern { REGULAR, BLUE_GREEN, RED_YELLOW };
Pattern currentPattern = REGULAR;
/*
Sets up code for Load Cells 

*/
void setup(){
  Serial.begin(9600);
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);

    //DOUT
    pinMode(A0, INPUT);

    //Channel A negative Input
    pinMode(A1, OUTPUT);
    //Channel A positive input 
    pinMode(A2, OUTPUT);

    pinMode(BUTTON_REGULAR_PIN, INPUT_PULLUP);
    pinMode(BUTTON_BLUE_GREEN_PIN, INPUT_PULLUP);
    pinMode(BUTTON_RED_YELLOW_PIN, INPUT_PULLUP);

  Serial.begin(9600);

}

void loop() {



    
}


