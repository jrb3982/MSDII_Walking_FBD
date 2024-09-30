#include <FastLED.h>
#include "math.h"

#define NUM_LEDS 20
#define MAX_SENSOR_FOR_LEDS 50.0f
#define ABS_MAX 150.0f
#define MIN_SENSOR 0.0f
#define MIN_LEDS 1.0f
#define LED_PIN 4
#define BUTTON_REGULAR_PIN 5
#define BUTTON_BLUE_GREEN_PIN 6
#define BUTTON_RED_YELLOW_PIN 7
#define NUM_SENSORS 3

int sensor[NUM_SENSORS];
int sensorSum;
float forceSum;
int sensorAvg;
float forceAvg;
int sel[] = {8,9,10,11};
CRGB leds[NUM_LEDS];
int numLedsLUT[(int)MAX_SENSOR_FOR_LEDS];
float forceLUT[(int)ABS_MAX];
int seconds = 0;
int selPattern = 0; 

enum Pattern { REGULAR, BLUE_GREEN, RED_YELLOW };
Pattern currentPattern = REGULAR;

void setup() {
    Serial.begin(9600);
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
    pinMode(A0, INPUT);
    pinMode(BUTTON_REGULAR_PIN, INPUT_PULLUP);
    pinMode(BUTTON_BLUE_GREEN_PIN, INPUT_PULLUP);
    pinMode(BUTTON_RED_YELLOW_PIN, INPUT_PULLUP);
    for (int i = 0; i < NUM_SENSORS; i++) {
      pinMode(sel[i], OUTPUT);  // Set each pin as an output
    }

    // populate lookup table with #LEDs on for a given force
    for (int i = 0; i < (int)MAX_SENSOR_FOR_LEDS; i++) {
        // num LEDs on determined via interpolation scaling
        float float_leds_on = ((NUM_LEDS - MIN_LEDS) / (MAX_SENSOR_FOR_LEDS - MIN_SENSOR)) * (i - MIN_SENSOR) + MIN_LEDS;
        numLedsLUT[i] = (int) round(float_leds_on); // populate LUT
    }
    for (int i = 0; i < (int)ABS_MAX; i++){
        forceLUT[i] = (float)determineForce(i);
    }
}

void loop() {
    int getSmooth = 0;
    int temp;  

    //Check if the regular pattern button is pressed
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

    for (int i = 0; i < NUM_SENSORS; i++){
      multiplexOut(i);
      // Read sensor value
      // while (getSmooth < 10) {
      //     temp = analogRead(A0);
      //     if (temp > MIN_SENSOR) {
      //         sensor[i] += temp;
      //         getSmooth++;
      //     }
      // }
      // sensor[i] = (int)(sensor[i] / 10);
      temp = analogRead(A0);
      forceSum += forceLUT[temp];
      sensorSum += temp;
     // getSmooth = 0;
    }

    sensorAvg = (int)(sensorSum / NUM_SENSORS);
    Serial.println("Force =");
    Serial.print(forceSum);
    Serial.println(" N");
    // Display LEDs according to sensor value with the selected gradient
    if (sensorAvg >= MAX_SENSOR_FOR_LEDS){
        sensorAvg = MAX_SENSOR_FOR_LEDS-1;
    } 
    switch (currentPattern) {
        case REGULAR:
            processSensorData(numLedsLUT[sensorAvg]);
            break;
        case BLUE_GREEN:
            processSensorDataBlueGreen(numLedsLUT[sensorAvg]);
            break;
        case RED_YELLOW:
            processSensorDataRedYellow(numLedsLUT[sensorAvg]);
            break;
    }
    sensorSum = 0;
    sensorAvg = 0;
    forceSum = 0;
    forceAvg =0;
    delay(500);
    // if (selPattern < NUM_SENSORS - 1){
    //   selPattern++;
    // } else {
    //   selPattern = 0;
    // }
}

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

void multiplexOut(int selPattern) {
  for (int i = 0; i < NUM_SENSORS; i++) {
    if (selPattern & (1 << i)) {
      digitalWrite(sel[i], HIGH);  // Set the pin high if the corresponding bit in selPattern is 1
    } else {
      digitalWrite(sel[i], LOW);   // Set the pin low if the corresponding bit in selPattern is 0
    }
  }
}

float determineForce(int sensor) {
  float forceVal;
  float gram_force;
  if (sensor > 1){
    gram_force = 62*exp(0.0254f*sensor);//see plot of g vs sensor out
    forceVal  = (9.8 * (gram_force*0.001f)); //f = m(kg)*9.8, correction factor of 0.62 to offset  
  } else {
    forceVal = 0.0;
  }
  return forceVal;

}

