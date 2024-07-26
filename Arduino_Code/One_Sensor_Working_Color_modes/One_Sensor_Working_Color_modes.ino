#include <FastLED.h>
#include "math.h"

int seconds = 0;

#define NUM_LEDS 40
#define MAX_SENSOR 430.0f
#define MIN_SENSOR 0.0f
#define MIN_LEDS 1.0f
#define LED_PIN 4
#define BUTTON_REGULAR_PIN 5
#define BUTTON_BLUE_GREEN_PIN 6
#define BUTTON_RED_YELLOW_PIN 7

int delayval = 100;
int sensor;

CRGB leds[NUM_LEDS];
int numLedsLUT[(int)MAX_SENSOR];

enum Pattern { REGULAR, BLUE_GREEN, RED_YELLOW };
Pattern currentPattern = REGULAR;

void setup() {
    Serial.begin(9600);
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
    pinMode(A0, INPUT);
    pinMode(BUTTON_REGULAR_PIN, INPUT_PULLUP);
    pinMode(BUTTON_BLUE_GREEN_PIN, INPUT_PULLUP);
    pinMode(BUTTON_RED_YELLOW_PIN, INPUT_PULLUP);

    // populate lookup table with #LEDs on for a given force
    for (int i = 0; i < MAX_SENSOR; i++) {
        // num LEDs on determined via interpolation scaling
        float float_leds_on = ((NUM_LEDS - MIN_LEDS) / (MAX_SENSOR - MIN_SENSOR)) * (i - MIN_SENSOR) + MIN_LEDS;
        numLedsLUT[i] = (int) round(float_leds_on); // populate LUT
    }
}

void loop() {
    // Check if the regular pattern button is pressed
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

    // Read sensor value
    int getSmooth = 0;
    int temp;
    while (getSmooth < 10) {
        temp = analogRead(A0);
        if (temp > MIN_SENSOR) {
            sensor += temp;
            getSmooth++;
        }
    }
    sensor = (int)(sensor / 10);

    // Display LEDs according to sensor value with the selected gradient
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
            if ((i + 1) <= (NUM_LEDS * 0.50)) { // bottom 50%, color RED
                leds[i] = CRGB(255, 0, 0);
            } else { // top 50%, color YELLOW
                leds[i] = CRGB(255, 255, 0);
            }
        } else { // outside of range of ON LEDs, turn off
            leds[i] = CRGB(0, 0, 0);
        }
    }
    FastLED.show();
}





