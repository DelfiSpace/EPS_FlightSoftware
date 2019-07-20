#include <driverlib.h>
#include "msp.h"
#include "PQ9Bus.h"
#include "DSerial.h"
#define FCLOCK 48000000

PQ9Bus uart(3, GPIO_PORT_P9, GPIO_PIN0);
DSerial serial;

#define BUFFER_LENGTH 8

unsigned char Txdata[BUFFER_LENGTH];

unsigned char RxData[261];
uint8_t addr;
volatile bool dataReceived = false;

void onReceive( unsigned char *buffer)
{
    for (int i = 0; i < buffer[1] + 5; i++)
    {
        RxData[i] = buffer[i];
    }
    dataReceived = true;
}

/**
 * main.c
 */
void main(void)
{
    // Disabling the Watchdog timer
    MAP_WDT_A_holdTimer( );

    // Configuring pins for HF XTAL
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ,
            GPIO_PIN3 | GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);

    // Starting HFXT in non-bypass mode without a timeout. Before we start
    // we have to change VCORE to 1 to support the 48MHz frequency
    MAP_CS_setExternalClockSourceFrequency(0, FCLOCK);
    MAP_PCM_setCoreVoltageLevel(PCM_VCORE1);
    MAP_FlashCtl_setWaitState(FLASH_BANK0, 2);
    MAP_FlashCtl_setWaitState(FLASH_BANK1, 2);
    MAP_CS_startHFXT(false);

    // Configure clocks that we need
    MAP_CS_initClockSignal(CS_MCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_SMCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_4);
    MAP_CS_initClockSignal(CS_HSMCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_2);

    serial.begin( );        //baud rate: 9600 bps
    uart.begin(115200, 4);  //baud rate: 115200 bps
    uart.onReceive(&onReceive);

    for (int i = 0; i < BUFFER_LENGTH; i++)
    {
        Txdata[i] = 'A';
    }

    serial.println("Hello World");

    while(true)
    {
        if (dataReceived)
        {
            RxData[6] = 2;
            uart.transmit(RxData[2], RxData[0], &RxData[3], RxData[1]);
            dataReceived = false;
        }
    }
}
