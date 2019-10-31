
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

// power busses handler
PowerBusHandler pb;

// debug console handler
DSerial serial;

// services running in the system
EPSHousekeepingService hk;
PingService ping;
ResetService reset( GPIO_PORT_P5, GPIO_PIN0 );
SoftwareUpdateService SWUpdate;
TestService test;
PowerBusHandler busHandler;
Service* services[] = { &hk, &ping, &reset, &SWUpdate, &busHandler, &test };

// EPS board tasks
PQ9CommandHandler cmdHandler(pq9bus, services, 6);
PeriodicTask timerTask(FCLOCK, periodicTask);
Task* tasks[] = { &cmdHandler, &timerTask };

// system uptime
unsigned long uptime = 0;

void periodicTask()
{
    // increase the timer, this happens every second
    uptime++;

    // collect telemetry
    EPSTelemetryContainer *tc = static_cast<EPSTelemetryContainer*>(hk.getContainerToWrite());

    // acquire telemetry
    acquireTelemetry(tc);

    pb.checkBussesStatus(tc);
    // handle power busses

    // telemetry collected, store the values and prepare for next collection
    hk.stageTelemetry();

    // refresh the watch-dog configuration to make sure that, even in case of internal
    // registers corruption, the watch-dog is capable of recovering from an error
    reset.refreshConfiguration();

    // watch-dog time window
    // kick hardware watch-dog after every telemetry collection happens
    reset.kickExternalWatchDog();
}

void acquireTelemetry(EPSTelemetryContainer *tc)
{
    unsigned short v, c;
    signed short i, t;

    // set uptime in telemetry
    tc->setUpTime(uptime);

    // measure the battery board
    tc->setBattStatus(!gasGauge.getVoltage(v));
    tc->setBattVoltage(v);
    tc->setBattStatus(!gasGauge.getTemperature(t));
    tc->setBattTemperature(t);
    tc->setBattStatus(!gasGauge.getAvailableCapacity(c));
    tc->setBattCapacity(c);

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
}

/**
 * main.c
 */
void main(void)
{
    // initialize the MCU:
    // - clock source
    // - clock tree
    DelfiPQcore::initMCU();

    // initialize the reset handler:
    // - prepare the watch-dog
    // - initialize the pins for the hardware watch-dog
    // prepare the pin for power cycling the system
    reset.init();

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

    // link the command handler to the PQ9 bus:
    // every time a new command is received, it will be forwarded to the command handler
    pq9bus.setReceiveHandler([](PQ9Frame &newFrame){ cmdHandler.received(newFrame); });

    // every time a command is correctly processed, call the watch-dog
    cmdHandler.onValidCommand([]{ reset.kickInternalWatchDog(); });

    gasGauge.init(750, 21, 1500);            // Battery capacity: 750mAh
                                             // Rsense: 21mOhm
                                             // Imax: 1500mA

    serial.println("EPS booting...");

    // start the Task Manager: all activities from now on
    // will be managed from a dedicated task
    DelfiPQcore::startTaskManager(tasks, 2);
}
