
#include "EPS.h"

DWire i2c0(0);
INA226 INAbus0(i2c0, 0x40);
INA226 INAbus1(i2c0, 0x49);
// CDHS bus handler
PQ9Bus pq9bus(3, GPIO_PORT_P9, GPIO_PIN0);
// debug console handler
DSerial serial;
// services running in the system
PingService ping;
ResetService reset;
SoftwareUpdateService SWUpdate;
Service* services[] = { &ping, &reset, &SWUpdate };
// command handler, dealing with all CDHS requests and responses
PQ9CommandHandler cmdHandler(pq9bus, services, 3);


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
    pq9bus.begin(115200, 5);  //baud rate: 115200 bps

    // initialize the command handler: from now on, commands can be processed
    cmdHandler.init();

    serial.println("Hello World");

    unsigned short v = 0;
    signed short i = 0;
    unsigned short p = 0;

    while(true)
    {
        cmdHandler.commandLoop();

            /*if (!INAbus0.getVoltage(v))
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
            */

    }
}
