// TimerA.c

#include "msp.h"
#include <stdint.h>
#include <stdio.h>
#include "TimerA.h"
#include "uart.h"

static uint32_t DEFAULT_PERIOD_A0[5] = {0,0,0,0,0};
static uint32_t DEFAULT_PERIOD_A2[5] = {0,0,0,0,0};

int TIMER_A0_PWM_Init(uint16_t period, double percentDutyCycle, uint16_t pin) {
    uint16_t dutyCycle;  // Moved declaration to top

    if (pin == 1) {
        P2->DIR |= BIT4;
        P2->SEL0 |= BIT4;
        P2->SEL1 &= ~BIT4;
    } else if (pin == 2) {
        P2->DIR |= BIT5;
        P2->SEL0 |= BIT5;
        P2->SEL1 &= ~BIT5;      
    } else if (pin == 3) {
        P2->DIR |= BIT6;
        P2->SEL0 |= BIT6;
        P2->SEL1 &= ~BIT6;             
    } else if (pin == 4) {
        P2->DIR |= BIT7;
        P2->SEL0 |= BIT7;
        P2->SEL1 &= ~BIT7;              
    } else {
        return -2;
    }

    DEFAULT_PERIOD_A0[pin] = period;
    TIMER_A0->CCR[0] = period;
    TIMER_A0->CCTL[pin] = (BIT5 | BIT6 | BIT7);
    
    dutyCycle = (uint16_t) (percentDutyCycle * (double)DEFAULT_PERIOD_A0[pin]);
    TIMER_A0->CCR[pin] = dutyCycle;
    TIMER_A0->CTL = BIT9 | BIT4; 
    return 0;
}

void TIMER_A0_PWM_DutyCycle(double percentDutyCycle, uint16_t pin) {
    TIMER_A0->CCR[pin] = percentDutyCycle * TIMER_A0->CCR[0];
}

int TIMER_A2_PWM_Init(uint16_t period, double percentDutyCycle, uint16_t pin) {
    uint16_t dutyCycle;  // Moved declaration to top

    if (pin == 1) {
        P5->DIR |= BIT6;
        P5->SEL0 |= BIT6;
        P5->SEL1 &= ~BIT6;
    } else {
        return -2;
    }

    DEFAULT_PERIOD_A2[pin] = period;
    TIMER_A2->CCR[0] = period;
    TIMER_A2->CCTL[pin] = (BIT5 | BIT6 | BIT7);

    dutyCycle = (uint16_t) (percentDutyCycle * (double)DEFAULT_PERIOD_A2[pin]);
    TIMER_A2->CCR[pin] = dutyCycle;

    TIMER_A2->CTL = (BIT7 | BIT6 | BIT9 | BIT4);
    TIMER_A2->EX0 = (BIT2 | BIT0 | BIT1);
    return 0;
}

void TIMER_A2_PWM_DutyCycle(double percentDutyCycle, uint16_t pin) {
    TIMER_A2->CCR[pin] = percentDutyCycle * TIMER_A2->CCR[0];
}
