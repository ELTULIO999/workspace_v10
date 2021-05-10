//*****************************************************************************
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Blinky (blinky)</h1>
//!
//! A very simple example that blinks the on-board LED.
//
//*****************************************************************************

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
    while(1);
}
#endif

//*****************************************************************************
//
// Blink the on-board LED.
//
//*****************************************************************************

uint8_t flag;
int
main(void)
{

    volatile uint32_t ui32Loop;
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)){
    }
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);
    // Loop forever.
    while(1)
    {
        if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)==0){
                    flag=1;}
                else {
                    if(flag==1){
                        //green
                        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
                        SysCtlDelay(2000000);
                        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
                        SysCtlDelay(2000000);

                        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
                        SysCtlDelay(2000000);
                        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
                        SysCtlDelay(2000000);
                        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
                        SysCtlDelay(2000000);
                        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
                        SysCtlDelay(200000);
                        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
                        SysCtlDelay(2000000);
                        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
                        SysCtlDelay(200000);
                        //yel
                        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
                        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
                        SysCtlDelay(2000000);
                        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
                        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
                        SysCtlDelay(2000000);
                        //red
                        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
                        SysCtlDelay(2000000);
                        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
                        SysCtlDelay(2000000);
                        flag=0;
                        }
                    }

    }

}
