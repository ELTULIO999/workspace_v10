
/******************************************************

 * ***************************************************/
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

volatile float x_n;             // Representa al termino x[n]
float y_n,                      // Representa al termino y[n]
        x_n_1 = 0,              // Representa al termino x{n-1]
        y_n_1 = 0;              // Representa al termino y[n-1]
volatile uint16_t flag = 0,  // Habilia el calculo y el envio via uart
        func = 0;            // Define que ecuacion se va emplearse
uint16_t ant = 16,          // Variable que almacena el valor anterior del boton
        act = 16;            // Variable que almacena el valor actual del boton


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

    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0,
                 !GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_0));
    act = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4);
}

//*****************************************************************************
//
// This function sets up UART0 to be used for a console to display information
// as the example is running.
//
//*****************************************************************************
void InitConsole(void)
{
    // Enable GPIO port A which is used for UART0 pins.
    // TODO: change this to whichever GPIO port you are using.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Configure the pin muxing for UART0 functions on port A0 and A1.
    // This step is not necessary if your part does not support pin muxing.
    // TODO: change this to select the port/pin you are using.
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    // Enable UART0 so that we can configure the clock.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Use the internal 16MHz oscillator as the UART clock source.
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    // Select the alternate (UART) function for these pins.
    // TODO: change this to select the port/pin you are using.
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Initialize the UART for console I/O.
    UARTStdioConfig(0, 115200, 16000000);
}
void Timer0_ADC_Config(void)
{

    uint16_t freq_muestreo = 1000;    // En Hz

    // The ADC0 peripheral must be enabled for use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    // For this example ADC0 is used with AIN0 and AIN1.
    // The actual port and pins used may be different on your part, consult
    // the data sheet for more information.  GPIO port E needs to be enabled
    // so these pins can be used.
    // TODO: change this to whichever GPIO port you are using.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    // Select the analog ADC function for these pins.
    // Consult the data sheet to see which functions are allocated per pin.
    // TODO: change this to select the port/pin you are using.
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);  // Configura el pin PE3
    // Se configura la secuencia 2, que permitiría hasta cuatro muestras (aunque
    // se usarán dos).
    ADCSequenceConfigure(ADC0_BASE, 2, ADC_TRIGGER_PROCESSOR, 0);
    // Step 0 en la secuencia 2: Canal 0 (ADC_CTL_CH0) en modo single-ended (por defecto).
    ADCSequenceStepConfigure(ADC0_BASE, 2, 0,
    ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
    // Since sample sequence 2 is now configured, it must be enabled.
    ADCSequenceEnable(ADC0_BASE, 2);  // Notar el cambio de "secuencia".
    // Clear the interrupt status flag.  This is done to make sure the
    // interrupt flag is cleared before we sample.
    ADCIntClear(ADC0_BASE, 2);  // Notar el cambio de "secuencia".

    // Enable the peripherals used by this example.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    // Configure the two 32-bit periodic timers.
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    // El tercer argumento determina la frecuencia. El reloj se puede obtener
    // con SysCtlClockGet().
    // La frecuencia está dada por SysCtlClockGet()/(valor del 3er argumento).
    // Ejemplos: Si se pone SysCtlClockGet(), la frecuencia será de 1 Hz.
    //           Si se pone SysCtlClockGet()/1000, la frecuencia será de 1 kHz
    //TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet());
    TimerLoadSet(TIMER0_BASE, TIMER_A,
                 (uint32_t) (SysCtlClockGet() / freq_muestreo));
    // Setup the interrupt for the timer timeout.
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

}
void Timer1_Signal_SW_Config(void)
{
    uint16_t signalfreq = 10; //Frecuencia de la señal cuadrada
    //Configuracion de SW1 para ser pull-up
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //Se habilita el periferico
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4); //se configura el pin como entrada
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_8MA,
    GPIO_PIN_TYPE_STD_WPU); //con pull-upp

    //Configuracion del pin encargado de generar la señal cuadrada (PB0)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); //Se habilita el periferico
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_0); //Se configura como pin de salida
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0, 1); //Se define el estado de inicio de la señal
                                                  //cuadrada
    //Se configura el Timer 1 para que genere una interrupcion cada 10Hz
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1); //se habilita el periferico
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER1_BASE, TIMER_A,
                 (uint32_t) (SysCtlClockGet() / (2 * signalfreq)));
    IntEnable(INT_TIMER1A);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
}
//*****************************************************************************
//
// Configure ADC0 for a single-ended input and a single sample.  Once the
// sample is ready, an interrupt flag will be set.  Using a polling method,
// the data will be read then displayed on the console via UART0.
//
//*****************************************************************************
int
main(void)
{
    // Set the clocking to run at 80 MHz (200 MHz / 2.5) using the PLL.  When
    // using the ADC, you must either use the PLL or supply a 16 MHz clock source.
    // TODO: The SYSCTL_XTAL_ value must be changed to match the value of the
    // crystal on your board.
    //SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
    //                 SYSCTL_XTAL_16MHZ); // 20 MHz
    SysCtlClockSet(
            SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ
                    | SYSCTL_OSC_MAIN); //40MHz
    uint32_t clock = SysCtlClockGet();
    // Set up the serial console to use for displaying messages.  This is
    // just for this example program and is not needed for ADC operation.
    InitConsole();
    // Enable processor interrupts.
    IntMasterEnable();
    //Se configura el Timer 0
    Timer0_ADC_Config();
    //Se configura el Timer 1, la señal cuadrada y el boton
    Timer1_Signal_SW_Config();

    // Enable the timers.
    TimerEnable(TIMER0_BASE, TIMER_A);
    TimerEnable(TIMER1_BASE, TIMER_A);

    while(1)
    {

        if (ant == 0 && act == 16)
            func++; //Cada vez que se presiona, y suelta, el boton se cambia de opcion
        if (func == 3)
            func = 0; //Se asegura que existan solo 3 opciones
        ant = act;
        if (flag == 1) //Bandera que se habilita cada vez que se obtiene una nueva muestra
        {
            switch (func)
            {
            case 1: //Ecuacion en diferencias 1
                y_n = 0.1 * x_n + 0.9 * y_n_1;
                y_n_1 = y_n;
                break;
            case 2: //Ecuacion en diferencias 2
                y_n = 0.9 * (x_n - x_n_1 + y_n_1);
                y_n_1 = y_n;
                x_n_1 = x_n;
                break;
            default: //Estado con el que se comienza
                y_n = x_n;
            }

            UARTprintf("x[n] %d, y[n] %d\n", (int) x_n, (int) y_n);
            flag = 0;
    }
}
}
