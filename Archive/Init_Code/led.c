#include "Common.h"
#include "msp.h" 
#include "uart.h" 

void LED1_On(void){
    P1->OUT |= BIT0;
}
void LED1_Off(void){
    P1->OUT &= ~BIT0;  
}

void LED2_On(void){
    int c, d;  // Move the declaration to the top of the function
    P2->OUT |= BIT0;
    P2->OUT &= ~BIT1;
    P2->OUT &= ~BIT2;

    for(c = 1; c <= 1000; c++) {
        for(d = 1; d <= 700; d++) {}
    }

    P2->OUT |= BIT1;
    P2->OUT &= ~BIT0;
    P2->OUT &= ~BIT2;

    for(c = 1; c <= 1000; c++) {
        for(d = 1; d <= 700; d++) {}
    }

    P2->OUT |= BIT2;
    P2->OUT &= ~BIT0;
    P2->OUT &= ~BIT1;

    for(c = 1; c <= 1000; c++) {
        for(d = 1; d <= 700; d++) {}
    }
}

void LED2_Off(void){
    P2->OUT &= ~BIT0;  
}

void LED1_Init(void) {
    // configure PortPin for LED1 as port I/O 
    P1->SEL1 &= ~BIT0;
    P1->SEL0 &= ~BIT0;
    
    // make built-in LED1 LED high drive strength
    P1->DS |= BIT0;
    
    // make built-in LED1 out
    P1->DIR |= BIT0;

    // turn off LED
    P1->OUT &= ~BIT0;
}

void LED2_Init(void) {
    // configure PortPin for LED2 as port I/O 
    P2->SEL1 &= ~7;
    P2->SEL0 &= ~7;

    // make built-in LED2 LEDs high drive strength
    P2->DS |= 7;

    // make built-in LED2 out
    P2->DIR |= 7;

    // turn off LED
    P2->OUT &= ~7;
}

//------------Switch_Input------------
// Read and return the status of Switch1
// Input: none
// return: TRUE if pressed
//         FALSE if not pressed
BOOLEAN Switch1_Pressed(void) {
    BOOLEAN retVal = FALSE;
    // check if pressed
    if ((P1->IN & BIT1) == 0)
        retVal = TRUE;
    else
        retVal = FALSE;

    return retVal;  // return TRUE(pressed) or FALSE(not pressed)
}

//------------Switch_Input------------
// Read and return the status of Switch2
// Input: none
// return: TRUE if pressed
//         FALSE if not pressed
BOOLEAN Switch2_Pressed(void) {
    BOOLEAN retVal = FALSE;
    // check if pressed
    if ((P1->IN & BIT4) == 0)
        retVal = TRUE;
    else
        retVal = FALSE;

    return retVal;  
}
