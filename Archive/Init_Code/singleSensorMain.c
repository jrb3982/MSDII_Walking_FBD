/*
* Rochester Institute of Technology
* Department of Computer Engineering
* CMPE 497
* Author: Jackson R. Baker
*/
#include <stdio.h>
#include <stdlib.h>

#include "msp.h"
#include "uart.h"
#include "led.h"
#include "switches.h"
#include "Timer32.h"
#include "CortexM.h"
#include "Common.h"
#include "ADC14.h"
#include "PID.h"
#include "LED_prog.h"
#include "oled.h"
#include "i2c.h"
// The sprintf function seemed to cause a hange in the interrupt service routine.
// I think if we increase the HEAP size, it will work
// change to Heap_Size       EQU     0x00000200 in startup_msp432p401r_uvision.s


BOOLEAN Timer1RunningFlag = FALSE;
BOOLEAN Timer2RunningFlag = FALSE;

unsigned long MillisecondCounter = 0;

void PORT1_IRQHandler(void)
{ 
	// Check if it came from Switch1
  if(P1->IFG & BIT1)  // we start a timer to toggle the read in analog signals from the MCU every 1/2 second.
	{
    // acknowledge P1.1 is pressed, by setting BIT1 to zero - remember P1.1 is switch 1
		// clear flag, acknowledge
    P1->IFG &= ~(BIT1);
    
    // XOR to disable/enable Timer32-1
    TIMER32_CONTROL1 = TIMER32_CONTROL1 ^ BIT5; 
  }
}

// Interrupt Service Routine for Timer32-1
void Timer32_1_ISR(void)
{
  unsigned int adc_In = ADC_In();
  float digVoltage = (float)adc_In * (2.5/16384.0); // 14-bit ADC digital representation of voltage
  //float forceVal = //Implement some operation here to convert digital volatage value to corresponding force. Dependent on sensor used.
  //corrected_force= computePID(intended_value,forceval) //Compute PID error val. (if even needed)
  //LEDInterpolate(forceVal, FILL IN NUMBER OF INDIVIDUAL AVAILABLE LEDS HERE) //interpolate active LEDs based on force applied.

}

void Switch1_Interrupt_Init(void)
{
	// disable interrupts
	DisableInterrupts();
  
	// initialize the Switch as per previous lab
	Switch1_Init();
	
	//7-0 PxIFG RW 0h Port X interrupt flag
	//0b = No interrupt is pending.
	//1b = Interrupt is pending.  
	// clear flag1 (reduce possibility of extra interrupt)	
  P1->IFG &= ~(BIT1); 

	//7-0 PxIE RW 0h Port X interrupt enable
	//0b = Corresponding port interrupt disabled
	//1b = Corresponding port interrupt enabled	
	// arm interrupt on  P1.1	
  P1->IE |= BIT1;  

	//7-0 PxIES RW Undefined Port X interrupt edge select
  //0b = PxIFG flag is set with a low-to-high transition.
  //1b = PxIFG flag is set with a high-to-low transition
	// now set the pin to cause falling edge interrupt event
	// P1.1 is falling edge event
  P1->IES |= BIT1; 
	
	// now set the pin to cause falling edge interrupt event
  NVIC_IPR8 = (NVIC_IPR8 & 0x00FFFFFF)|0x40000000; // priority 2
	
	// enable Port 1 - interrupt 35 in NVIC	
  NVIC_ISER1 = 0x00000008;  
	
	// enable interrupts  (// clear the I bit	)
  EnableInterrupts();              
}

// main
int main(void)
{
	//initializations
	uart0_init();
	uart0_put("\r\nLab5 ADC demo\r\n");
    Timer32_1_Init(&Timer32_1_ISR, SystemCoreClock/2, T32DIV1);
	LED1_Init();
	Switch1_Init();
    PID_Init();
    OLED_Init();
    i2c0_Init();
    Switch1_Interrupt_Init();
	ADC0_InitSWTriggerCh6();
	EnableInterrupts();
  while(1)
	{
		WaitForInterrupt();
  }
}

