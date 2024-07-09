
#include "PID.h"
#include "math.h"

float setPoint;
float prevError1;
float prevError2;

float errors[ERROR_HISTORY_COUNT];

void PID_Init() {
  for (int i = 0; i < ERROR_HISTORY_COUNT; i++) {
    errors[i] = 0.0;
  }
  setPoint = 0.0;
}

float computePID(float intended_position, float actual_position) {
  float error = intended_position - actual_position;

  // grab errors from the array
  prevError1 = errors[1];
  prevError2 = errors[2];
    
  setPoint = KP * error +
            // KI * (error + prevError1) * 0.5f +
             KD * (error - 2.0f * prevError1 + prevError2);
  
  errors[2] = errors[1];
  errors[1] = errors[0];
  errors[0] = error;

  return setPoint;

}
