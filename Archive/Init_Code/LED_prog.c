#include <stdio.h>
#include <stdlib.h>
#include "msp.h"
#include "LED_prog.h"
#include "Common.h"

#define MAX_FORCE 980.76f
#define MIN_FORCE 1.0f
#define MIN_LEDS 1.0f

void LEDInterpolate(float forceval, uint32_t AvailLEDs){
    float float_leds_on = ((targetNumLEDs - MIN_LEDs) / (MAX_FORCE-MIN_FORCE)) * (forceval - MINFORCE) + MIN_LEDS;
    if (float_leds_on > 1.0f){ //eliminating cases too small, less than MIN_FORCE
        uint32_t number_leds_on = (int) float_leds_on;
        //Based on method of programming LED strips, go from here.
    }

}