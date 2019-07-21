#include <driverlib.h>
#include "msp.h"
#include "PQ9Bus.h"
#include "DWire.h"
#include "INA226.h"
#include "DSerial.h"
#define FCLOCK 48000000

DWire i2c0(0);
INA226 INAbus0(i2c0, 0x40);
INA226 INAbus1(i2c0, 0x49);
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

    // Initialize I2C master
    i2c0.setFastMode();
    i2c0.begin();

    // initialize the shunt resistor
    INAbus0.setShuntResistor(0.04);

    // initialize the shunt resistor
    INAbus1.setShuntResistor(0.04);

    serial.begin( );        //baud rate: 9600 bps
    uart.begin(115200, 5);  //baud rate: 115200 bps
    uart.onReceive(&onReceive);

    for (int i = 0; i < BUFFER_LENGTH; i++)
    {
        Txdata[i] = 'A';
    }

    serial.println("Hello World");

    unsigned short v = 0;
    signed short i = 0;
    unsigned short p = 0;

    while(true)
    {
        if (dataReceived)
        {
            RxData[6] = 2;
            uart.transmit(RxData[2], RxData[0], &RxData[3], RxData[1]);
            dataReceived = false;

            if (!INAbus0.getVoltage(v))
            {
                serial.print("Voltage: ");
                serial.print(v, DEC);
                serial.println(" mV");
            }
            else
            {
                serial.println("Error reading voltage bus 0");
            }

            if (!INAbus0.getCurrent(i))
            {
                serial.print("Current: ");
                serial.print(i, DEC);
                serial.println(" mA");
            }
            else
            {
                serial.println("Error reading current bus 0");
            }

            if (!INAbus0.getPower(p))
            {
                serial.print("Power: ");
                serial.print(p, DEC);
                serial.println(" mW");
            }
            else
            {
                serial.println("Error reading power bus 0");
            }

            if (!INAbus1.getVoltage(v))
            {
                serial.print("Voltage: ");
                serial.print(v, DEC);
                serial.println(" mV");
            }
            else
            {
                serial.println("Error reading voltage bus 1");
            }
            serial.println();
        }
    }
}
