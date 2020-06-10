#include "EPS.h"

// I2C busses
DWire I2Cinternal(0);
DWire SolarPanelsBus(1);
DWire BatteryBoardBus(2);
// battery board
// Battery gas gauge
// Battery capacity: 1500mAh
// Rsense: 33 mOhm
LTC2942 gasGauge(BatteryBoardBus, 1500, 33);
INA226 battery(BatteryBoardBus, 0x40);
// internal power busses
INA226 internalBus(I2Cinternal, 0x48);
INA226 unregulatedBus(I2Cinternal, 0x4A);
// external power busses
INA226 bus1(I2Cinternal, 0x40);
INA226 bus2(I2Cinternal, 0x41);
INA226 bus3(I2Cinternal, 0x43);
INA226 bus4(I2Cinternal, 0x42);
// Solar arrays
INA226 SAYp(SolarPanelsBus, 0x40);
INA226 SAYm(SolarPanelsBus, 0x41);
INA226 SAXp(SolarPanelsBus, 0x42);
INA226 SAXm(SolarPanelsBus, 0x43);
// SP's are on the batteryBoard, Current per Solar Panel.
INA226 SPYm(SolarPanelsBus, 0x4A);
INA226 SPYp(SolarPanelsBus, 0x48);
INA226 SPXp(SolarPanelsBus, 0x4C);
INA226 SPXm(SolarPanelsBus, 0x4E);

TMP100 tempYp(SolarPanelsBus, 0x4B);
TMP100 tempYm(SolarPanelsBus, 0x4F);
TMP100 tempXp(SolarPanelsBus, 0x4D);
TMP100 tempXm(SolarPanelsBus, 0x49);

// SPI bus
DSPI spi(3);
MB85RS fram(spi, GPIO_PORT_P1, GPIO_PIN0 );

// HardwareMonitor
HWMonitor hwMonitor(&fram);

// Bootloader
Bootloader bootLoader = Bootloader(fram);

// CDHS bus handler
PQ9Bus pq9bus(3, GPIO_PORT_P10, GPIO_PIN0);

// services running in the system
HousekeepingService<EPSTelemetryContainer> hk;
PingService ping;
ResetService reset( GPIO_PORT_P5, GPIO_PIN0 );

#ifndef SW_VERSION
SoftwareUpdateService SWupdate(fram);
#else
SoftwareUpdateService SWupdate(fram, (uint8_t*)xtr(SW_VERSION));
#endif


TestService test;
PowerBusHandler busHandler;
Service* services[] = { &hk, &ping, &reset, &SWupdate, &busHandler, &test };

// EPS board tasks
CommandHandler<PQ9Frame,PQ9Message> cmdHandler(pq9bus, services, 6);
PeriodicTask timerTask(1000, periodicTask);
PeriodicTask* periodicTasks[] = {&timerTask};
PeriodicTaskNotifier taskNotifier = PeriodicTaskNotifier(periodicTasks, 1);
Task* tasks[] = { &cmdHandler, &timerTask };

// system uptime
unsigned long uptime = 0;

// TODO: remove when bug in CCS has been solved
void receivedCommand(DataFrame &newFrame)
{
    cmdHandler.received(newFrame);
}

void validCmd(void)
{
    reset.kickInternalWatchDog();
}

void periodicTask()
{
    // increase the timer, this happens every second
    uptime++;

    // collect telemetry
    hk.acquireTelemetry(acquireTelemetry);

    // handle power busses:
    // verify if the internal bus is high enough to allow the power busses to be ON
    busHandler.checkBussesStatus(hk.getTelemetry());

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
    tc->setMCUTemperature(hwMonitor.getMCUTemp());

    // measure the battery board
    tc->setBattStatus((!gasGauge.getVoltage(v)) &
                      (!gasGauge.getTemperature(t)) &
                      (!gasGauge.getAvailableCapacity(c)));
    tc->setBattVoltage(v);
    tc->setBattTemperature(t);
    tc->setBattCapacity(c);

    // INA on battery
    tc->setBattINAStatus((!battery.getVoltage(v)) & (!battery.getCurrent(i)));
    tc->setBattVoltage1(v);
    tc->setBattCurrent(i);

    // INAS on the output of solar panels
    tc->setSPYmStatus((!SPYm.getVoltage(v)) & (!SPYm.getCurrent(i)));
    tc->setSPYmVoltage(v);
    tc->setSPYmCurrent(i);

    tc->setSPYpStatus((!SPYp.getVoltage(v)) & (!SPYp.getCurrent(i)));
    tc->setSPYpVoltage(v);
    tc->setSPYpCurrent(i);

    tc->setSPXpStatus((!SPXp.getVoltage(v)) & (!SPXp.getCurrent(i)));
    tc->setSPXpVoltage(v);
    tc->setSPXpCurrent(i);

    tc->setSPXmStatus((!SPXm.getVoltage(v)) & (!SPXm.getCurrent(i)));
    tc->setSPXmVoltage(v);
    tc->setSPXmCurrent(i);

    // measure the internal bus
    tc->setIntBStatus((!internalBus.getVoltage(v)) & (!internalBus.getCurrent(i)));
    tc->setIntBVoltage(v);
    tc->setIntBCurrent(i);

    // measure the unregulated bus
    tc->setURBStatus((!unregulatedBus.getVoltage(v)) & (!unregulatedBus.getCurrent(i)));
    tc->setURBVoltage(v);
    tc->setURBCurrent(i);

    // measure bus 1
    tc->setB1Status((!bus1.getVoltage(v)) & (!bus1.getCurrent(i)));
    tc->setB1Voltage(v);
    tc->setB1Current(i);

    // measure bus 2
    tc->setB2Status((!bus2.getVoltage(v)) & (!bus2.getCurrent(i)));
    tc->setB2Voltage(v);
    tc->setB2Current(i);

    // measure bus3
    tc->setB3Status((!bus3.getVoltage(v)) & (!bus3.getCurrent(i)));
    tc->setB3Voltage(v);
    tc->setB3Current(i);

    // measure bus 4
    tc->setB4Status((!bus4.getVoltage(v)) & (!bus4.getCurrent(i)));
    tc->setB4Voltage(v);
    tc->setB4Current(i);

    // measure solar array Yp
    tc->setSAYpStatus((!SAYp.getVoltage(v)) & (!SAYp.getCurrent(i)));
    tc->setSAYpVoltage(v);
    tc->setSAYpCurrent(i);
    tc->setSAYpTmpStatus(!tempYp.getTemperature(t));
    tc->setSAYpTemperature(t);

    // measure solar array Ym
    tc->setSAYmStatus((!SAYm.getVoltage(v)) & (!SAYm.getCurrent(i)));
    tc->setSAYmVoltage(v);
    tc->setSAYmCurrent(i);
    tc->setSAYmTmpStatus(!tempYm.getTemperature(t));
    tc->setSAYmTemperature(t);

    // measure solar array Xp
    tc->setSAXpStatus((!SAXp.getVoltage(v)) & (!SAXp.getCurrent(i)));
    tc->setSAXpVoltage(v);
    tc->setSAXpCurrent(i);
    tc->setSAXpTmpStatus(!tempXp.getTemperature(t));
    tc->setSAXpTemperature(t);

    // measure solar array Xm
    tc->setSAXmStatus((!SAXm.getVoltage(v)) & (!SAXm.getCurrent(i)));
    tc->setSAXmVoltage(v);
    tc->setSAXmCurrent(i);
    tc->setSAXmTmpStatus(!tempXm.getTemperature(t));
    tc->setSAXmTemperature(t);

    // power bus status
    tc->setBusStatus(busHandler.getStatus());
    tc->setBusErrorStatus(busHandler.getErrorStatus());
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

    // initialize the ADC
    // - ADC14 and FPU Module
    // - MEM0 for internal temperature measurements
    ADCManager::initADC();

    // Initialize I2C masters
    I2Cinternal.setFastMode();
    I2Cinternal.begin();
    SolarPanelsBus.setFastMode();
    SolarPanelsBus.begin();
    BatteryBoardBus.setFastMode();
    BatteryBoardBus.begin();

    // initialize the shunt resistors
    internalBus.setShuntResistor(40);
    unregulatedBus.setShuntResistor(40);
    bus1.setShuntResistor(40);
    bus2.setShuntResistor(40);
    bus3.setShuntResistor(40);
    bus4.setShuntResistor(40);
    SAYp.setShuntResistor(40);
    SAYm.setShuntResistor(40);
    SAXp.setShuntResistor(40);
    SAXm.setShuntResistor(40);
    SPYp.setShuntResistor(33);
    SPYm.setShuntResistor(33);
    SPXp.setShuntResistor(33);
    SPXm.setShuntResistor(33);
    battery.setShuntResistor(33);

    // initialize temperature sensors
    tempYp.init();
    tempYm.init();
    tempXp.init();
    tempXm.init();

    // Initialize SPI master
    spi.initMaster(DSPI::MODE0, DSPI::MSBFirst, 1000000);
    fram.init();

    Console::init( 115200 );                // baud rate: 115200 bps
    pq9bus.begin(115200, EPS_ADDRESS);      // baud rate: 115200 bps
                                            // address EPS (2)

    //InitBootLoader!
    bootLoader.JumpSlot();

    // initialize the reset handler:
    // - prepare the watch-dog
    // - initialize the pins for the hardware watch-dog
    // - prepare the pin for power cycling the system
    reset.init();

    // initialize Task Notifier
    taskNotifier.init();

    // initialize HWMonitor readings
    hwMonitor.readResetStatus();
    hwMonitor.readCSStatus();


    // link the command handler to the PQ9 bus:
    // every time a new command is received, it will be forwarded to the command handler
    // TODO: put back the lambda function after bug in CCS has been fixed
    //pq9bus.setReceiveHandler([](PQ9Frame &newFrame){ cmdHandler.received(newFrame); });
    pq9bus.setReceiveHandler(receivedCommand);

    // every time a command is correctly processed, call the watch-dog
    // TODO: put back the lambda function after bug in CCS has been fixed
    //cmdHandler.onValidCommand([]{ reset.kickInternalWatchDog(); });
    cmdHandler.onValidCommand(validCmd);

    gasGauge.init();

    Console::log("EPS booting...SLOT: %d", (int) Bootloader::getCurrentSlot());

    if(HAS_SW_VERSION == 1){
        Console::log("SW_VERSION: %s", (const char*)xtr(SW_VERSION));
    }
    // start the Task Manager: all activities from now on
    // will be managed from a dedicated task
    TaskManager::start(tasks, 2);
}
