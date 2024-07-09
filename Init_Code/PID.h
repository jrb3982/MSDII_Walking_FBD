// PID parameters- Needs Tuning
#define KP 1.55f
#define KI 0.0f
#define KD 0.045f

#define ERROR_HISTORY_COUNT 3

void PID_Init(void);
float computePID(float intended_position, float actual_position);
