#include "msp.h"
#include "uart.h"
#include "Common.h"

#define BAUD_RATE 9600      // default baud rate 
#define BAUD_RATE2 115200   // default baud rate 
extern uint32_t SystemCoreClock;  // clock rate of MCU

void uart2_init() {
    int Mod2 = SystemCoreClock / BAUD_RATE2;  // Moved declaration to the top

    EUSCI_A2->CTLW0 |= BIT0;
    EUSCI_A2->CTLW0 |= (BIT7 | BIT6);

    EUSCI_A2->BRW = Mod2;
    EUSCI_A2->MCTLW &= ~0xFFF0;

    P3->SEL0 |= 0x0C;
    P3->SEL1 &= ~0x0C;

    EUSCI_A2->CTLW0 &= ~BIT0;
    EUSCI_A2->IE &= ~(BIT0 | BIT1 | BIT2 | BIT3);
}

void uart0_init() {
    int Mod = SystemCoreClock / BAUD_RATE;  // Moved declaration to the top

    EUSCI_A0->CTLW0 |= BIT0;
    EUSCI_A0->CTLW0 |= (BIT7 | BIT6);

    EUSCI_A0->BRW = Mod;
    EUSCI_A0->MCTLW &= ~0xFFF0;

    P1->SEL0 |= 0x0C;
    P1->SEL1 &= ~0x0C;

    EUSCI_A0->CTLW0 &= ~BIT0;
    EUSCI_A0->IE &= ~(BIT0 | BIT1 | BIT2 | BIT3);
}

BYTE uart2_getchar() {
    BYTE inChar;
    while((EUSCI_A2->IFG & BIT0) == 0) {}
    inChar = (BYTE)EUSCI_A2->RXBUF;
    return inChar;
}

BYTE uart0_getchar() {
    BYTE inChar;
    while((EUSCI_A0->IFG & BIT0) == 0) {}
    inChar = (BYTE)EUSCI_A0->RXBUF;
    return inChar;
}

BOOLEAN uart0_dataAvailable() {
    return (EUSCI_A0->IFG & BIT0) != 0;
}

BOOLEAN uart2_dataAvailable() {
    return (EUSCI_A2->IFG & BIT0) != 0;
}

void uart2_putchar(char ch) {
    while((EUSCI_A2->IFG & BIT1) == 0) {}
    EUSCI_A2->TXBUF = ch;
}

void uart0_putchar(char ch) {
    while((EUSCI_A0->IFG & BIT1) == 0) {}
    EUSCI_A0->TXBUF = ch;
}

void uart2_put(char *ptr_str) {
    while(*ptr_str != '\0') {
        uart2_putchar(*ptr_str++);
    }
}

void uart0_put(char *ptr_str) {
    while(*ptr_str != '\0') {
        uart0_putchar(*ptr_str++);
    }
}
