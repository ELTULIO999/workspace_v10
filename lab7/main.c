#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"


/**
 * main.c
 */
char a;
int b;

void UARTIntHandler(void);
void Timer0IntHandler(void);

int main(void)
{
a = 0;


SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));

//timer0
IntMasterEnable();
SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));
TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()/2 - 1);
TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
TimerIntRegister(TIMER0_BASE,TIMER_A, Timer0IntHandler);
TimerEnable(TIMER0_BASE, TIMER_A);
IntEnable(INT_TIMER0A);

//UART
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));
SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))
GPIOPinConfigure(0x00000001);
GPIOPinConfigure(0x00000401);
GPIOPinTypeUART(GPIO_PORTA_BASE,GPIO_PIN_0|GPIO_PIN_1);
UARTConfigSetExpClk(UART0_BASE,SysCtlClockGet(),115200,(UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE));
UARTFIFOEnable(INT_UART0);
IntEnable(INT_UART0);
UARTIntEnable(UART0_BASE,UART_INT_RX | UART_INT_RT);
UARTIntClear(UART0_BASE,UART_INT_RX | UART_INT_RT);
UARTIntRegister(UART0_BASE, UARTIntHandler);
UARTEnable(UART0_BASE);


//leds
GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

while (1){
    a = UARTCharGet(UART0_BASE);
    UARTCharPut(UART0_BASE, a);
    if (a == 'r'){
        if (b == 2){b = 0;}
        else {b = 2;}
    }
    else if (a == 'g'){
        if (b == 8){b = 0;}
        else {b = 8;}
    }
    else if (a == 'b'){
        if (b == 4){b = 0;}
        else {b = 4;}
    }

}
}
void Timer0IntHandler(void){
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    if (GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_1)){
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0);
        }
    else if (GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_2)){
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0);
        }
    else if (GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_3)){
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0);
            }
    else{
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, b);
        }
    }

void UARTIntHandler(void){
    UARTIntClear(UART0_BASE, UART_INT_RX | UART_INT_RT);
    a = UARTCharGet(UART0_BASE);
}
