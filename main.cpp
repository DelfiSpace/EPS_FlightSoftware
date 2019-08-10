
#include "EPS.h"

// I2C busses
DWire I2Cinternal(0);
DWire SolarPanelsBus(1);
DWire BatteryBoardBus(2);

// Battery bus
LTC2942 gasGauge(BatteryBoardBus);

// internal power busses
INA226 internalBus(I2Cinternal, 0x48);
INA226 unregulatedBus(I2Cinternal, 0x4A);

// external power busses
INA226 bus1(I2Cinternal, 0x40);
INA226 bus2(I2Cinternal, 0x41);
INA226 bus3(I2Cinternal, 0x43);
INA226 bus4(I2Cinternal, 0x42);

// solar arrays
INA226 SAYp(SolarPanelsBus, 0x40);
INA226 SAYm(SolarPanelsBus, 0x41);
INA226 SAXp(SolarPanelsBus, 0x42);
INA226 SAXm(SolarPanelsBus, 0x43);

TMP100 tempYp(SolarPanelsBus, 0x4B);
TMP100 tempYm(SolarPanelsBus, 0x4F);
TMP100 tempXp(SolarPanelsBus, 0x4D);
TMP100 tempXm(SolarPanelsBus, 0x49);

// CDHS bus handler
PQ9Bus pq9bus(3, GPIO_PORT_P10, GPIO_PIN0);

// debug console handler
DSerial serial;
// services running in the system
EPSHousekeepingService hk;
PingService ping;
ResetService reset;
SoftwareUpdateService SWUpdate;
TestService test;
Service* services[] = { &hk, &ping, &reset, &SWUpdate, &test };

// command handler, dealing with all CDHS requests and responses
PQ9CommandHandler cmdHandler(pq9bus, services, 5);


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

    // Initialize I2C masters
    I2Cinternal.setFastMode();
    I2Cinternal.begin();
    SolarPanelsBus.setFastMode();
    SolarPanelsBus.begin();
    BatteryBoardBus.setFastMode();
    BatteryBoardBus.begin();

    // initialize the shunt resistor
    internalBus.setShuntResistor(0.04);

    // initialize the shunt resistor
    unregulatedBus.setShuntResistor(0.04);

    // initialize the shunt resistor
    bus1.setShuntResistor(0.04);
    bus2.setShuntResistor(0.04);
    bus3.setShuntResistor(0.04);
    bus4.setShuntResistor(0.04);

    // initialize the shunt resistor
    SAYp.setShuntResistor(0.04);
    SAYm.setShuntResistor(0.04);
    SAXp.setShuntResistor(0.04);
    SAXm.setShuntResistor(0.04);

    tempYp.init(RESOLUTION_12_BIT);
    tempYm.init(RESOLUTION_12_BIT);
    tempXp.init(RESOLUTION_12_BIT);
    tempXm.init(RESOLUTION_12_BIT);

    serial.begin( );                        // baud rate: 9600 bps
    pq9bus.begin(115200, EPS_ADDRESS);      // baud rate: 115200 bps
                                            // address EPS (2)

    // initialize the command handler: from now on, commands can be processed
    cmdHandler.init();

    gasGauge.init(750, 5, 1500);            //Battery capacity: 750mAh, Rsense: 5mOhm, Imax: 1500mA

    MAP_GPIO_setAsPeripheralModuleFunctionInputPin( GPIO_PORT_P4, GPIO_PIN0,
                                             GPIO_PRIMARY_MODULE_FUNCTION );
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin( GPIO_PORT_P4, GPIO_PIN1,
                                             GPIO_PRIMARY_MODULE_FUNCTION );

    MAP_GPIO_setOutputHighOnPin( GPIO_PORT_P4, GPIO_PIN0 );
    MAP_GPIO_setAsOutputPin( GPIO_PORT_P4, GPIO_PIN0 );
    MAP_GPIO_setOutputHighOnPin( GPIO_PORT_P4, GPIO_PIN1 );
    MAP_GPIO_setAsOutputPin( GPIO_PORT_P4, GPIO_PIN1 );

    serial.println("Hello World");

    int counter = 0;

    while(true)
    {
        cmdHandler.commandLoop();

        // hack to simulate timer to acquire telemetry approximately once per second
        if (counter >= 1400000)
        {
            counter = 0;
            serial.println("Acquiring telemetry....");

            EPSTelemetryContainer *tc = static_cast<EPSTelemetryContainer*>(hk.getContainerToWrite());

            unsigned short v = 0;
            signed short i = 0;
            signed short t;

            // measure the battery board
            tc->setBattStatus(!gasGauge.getVoltage(v));
            tc->setBattVoltage(v);
            tc->setBattStatus(!gasGauge.getTemperature(i));
            tc->setBattTemperature(i);

            // measure the internal bus
            tc->setIntBStatus(!internalBus.getVoltage(v));
            tc->setIntBVoltage(v);
            tc->setIntBStatus(!internalBus.getCurrent(i));
            tc->setIntBCurrent(i);

            // measure the unregulated bus
            tc->setURBStatus(!unregulatedBus.getVoltage(v));
            tc->setURBVoltage(v);
            tc->setURBStatus(!unregulatedBus.getCurrent(i));
            tc->setURBCurrent(i);

            // measure bus 1
            tc->setB1Status(!bus1.getVoltage(v));
            tc->setB1Voltage(v);
            tc->setB1Status(!bus1.getCurrent(i));
            tc->setB1Current(i);

            // measure bus 2
            tc->setB2Status(!bus2.getVoltage(v));
            tc->setB2Voltage(v);
            tc->setB2Status(!bus2.getCurrent(i));
            tc->setB2Current(i);

            // measure bus3
            tc->setB3Status(!bus3.getVoltage(v));
            tc->setB3Voltage(v);
            tc->setB3Status(!bus3.getCurrent(i));
            tc->setB3Current(i);

            // measure bus 4
            tc->setB4Status(!bus4.getVoltage(v));
            tc->setB4Voltage(v);
            tc->setB4Status(!bus4.getCurrent(i));
            tc->setB4Current(i);

            // measure solar array Yp
            tc->setSAYpStatus(!SAYp.getVoltage(v));
            tc->setSAYpVoltage(v);
            tc->setSAYpStatus(!SAYp.getCurrent(i));
            tc->setSAYpCurrent(i);
            tc->setSAYpTmpStatus(!tempYp.getTemperature(t));
            tc->setSAYpTemperature(t);

            // measure solar array Ym
            tc->setSAYmStatus(!SAYm.getVoltage(v));
            tc->setSAYmVoltage(v);
            tc->setSAYmStatus(!SAYm.getCurrent(i));
            tc->setSAYmCurrent(i);
            tc->setSAYmTmpStatus(!tempYm.getTemperature(t));
            tc->setSAYmTemperature(t);

            // measure solar array Xp
            tc->setSAXpStatus(!SAXp.getVoltage(v));
            tc->setSAXpVoltage(v);
            tc->setSAXpStatus(!SAXp.getCurrent(i));
            tc->setSAXpCurrent(i);
            tc->setSAXpTmpStatus(!tempXp.getTemperature(t));
            tc->setSAXpTemperature(t);

            // measure solar array Xm
            tc->setSAXmStatus(!SAXm.getVoltage(v));
            tc->setSAXmVoltage(v);
            tc->setSAXmStatus(!SAXm.getCurrent(i));
            tc->setSAXmCurrent(i);
            tc->setSAXmTmpStatus(!tempXm.getTemperature(t));
            tc->setSAXmTemperature(t);

            // telemetry collected, store the values and prepare for next collection
            hk.stageTelemetry();
        }

        counter++;

    }
}
