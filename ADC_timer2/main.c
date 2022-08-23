/*
 Procesamiento de Señales - Laboratorio 11                              Sección 21
 Julio Lopez 18211
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

volatile float x_n;            // its the term of x[n]
float y_n,                     // its the term of y[n]
     x_n_1 = 0,             // its the term of x{n-1]
     y_n_1 = 0;             // its the term of y[n-1]
volatile uint16_t flag = 0,    // its a variable for the UART communication
        func = 0;              // Define que ecuacion se va emplearse
uint16_t now = 16,             // Var for the push boton
         late = 16;            // Var for the push boton


//*****************************************************************************
//
// The interrupt handler for the  timer 0 interrupt.
//
//*****************************************************************************
void
Timer0IntHandler(void)
{
    // Notar que ahora necesitamos dos espacios para las conversiones.
    uint32_t pui32ADC0Value[1];
    // Clear the timer interrupt. Necesario para lanzar la próxima interrupción.
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    // Trigger the ADC conversion.
    ADCProcessorTrigger(ADC0_BASE, 2);  // Notar el cambio de "secuencia" (2 en lugar de 3).
    // Wait for conversion to be completed.
    while(!ADCIntStatus(ADC0_BASE, 2, false))  // Notar el cambio de "secuencia".
    {
    }
    // Clear the ADC interrupt flag.
    ADCIntClear(ADC0_BASE, 2);  // Notar el cambio de "secuencia".
    ADCSequenceDataGet(ADC0_BASE, 2, pui32ADC0Value);  // Notar el cambio de "secuencia".
    x_n = pui32ADC0Value[0];  // Convertir a voltios
    flag = 1;

}
//*****************************************************************************
//
// Interrupcion para generar la señal cuadrada y el antirebote.
//
//*****************************************************************************
void Timer1IntHandler(void)
{
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0,!GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_0));
    late = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4);
}
void InitConsole(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTStdioConfig(0, 115200, 16000000);
}
void Timer_ADC_Conf(void)
{   //esto fue dado por el catadratico
    uint16_t freq_muestreo = 1000;                                                   // En Hz
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);                                    // setting up of pin PE3
    ADCSequenceConfigure(ADC0_BASE, 2, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 2, 0,ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 2);
    ADCIntClear(ADC0_BASE, 2);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A,(uint32_t) (SysCtlClockGet() / freq_muestreo));
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}
void Timer1_Signalandpushb_conf(void)
{
    uint16_t signalfreq = 10; //Frecuencia de la señal cuadrada

    //Configuracion de SW1 para ser pull-up
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //enable the periferial
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4); //setting up the output pin
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_8MA,GPIO_PIN_TYPE_STD_WPU); //pull-up of the push-but

    //setting up of the pin that will be the pwd (PB0)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); //enable the periferial
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_0); //setting up the output pin
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0, 1);

    //setting timer1
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1); //enable the periferial
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER1_BASE, TIMER_A,(uint32_t) (SysCtlClockGet() / (2 * signalfreq)));
    IntEnable(INT_TIMER1A);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
}

int
main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ| SYSCTL_OSC_MAIN); //40MHz
    uint32_t clock = SysCtlClockGet();
    InitConsole();
    IntMasterEnable();
    Timer_ADC_Conf();
    Timer1_Signalandpushb_conf();
    // Enable the timers.
    TimerEnable(TIMER0_BASE, TIMER_A);
    TimerEnable(TIMER1_BASE, TIMER_A);
    while(1)
    {
        if (now == 0 && late == 16) {func++;}
        if (func == 3) {func = 0; now = late;}
        if (flag == 1) //when the buffer gets new data
        {

            switch (func)
            {
            case 1: //Ecu 1
                y_n = 0.1 * x_n + 0.9 * y_n_1;
                y_n_1 = y_n;
                break;
            case 2: //Ecu 2
                y_n = 0.9 * (x_n - x_n_1 + y_n_1);
                y_n_1 = y_n;
                x_n_1 = x_n;
                break;
            default: //Estado con el que se comienza
                y_n = x_n;
            }

            UARTprintf("%d, %d\n", (int) x_n, (int) y_n);// to print the data being proc
            flag = 0;
    }
}
}
