#include "msp.h" 

#include "Common.h"


void Switches_Init(void)
{
	// configure PortPin for Switch 1 and Switch2 as port I/O 
	P1->SEL1&=~BIT1; /*P1.1*/
	P1->SEL0&=~BIT1;
	P1->REN|=BIT4;
	P1->REN|=BIT1;
	P1->OUT|=BIT4;
	P1->OUT|=BIT1;
	
	P1->SEL1&=~BIT4;/*P1.4*/
	P1->SEL0&=~BIT4;
	// configure as input
	P1->DIR&=~BIT1;
	P1->DIR&=~BIT4;
              
}
