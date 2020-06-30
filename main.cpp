#include "EPS.h"

// I2C busses
DWire I2Cinternal(0);
DWire SolarPanelsBus(1);
DWire BatteryBoardBus(2);
// battery board
// Battery gas gauge
// Battery capacity: 1500mAh
// Rsense: 33 mOhm
LTC2942 batteryGG(BatteryBoardBus, 1500, 33);
INA226 batteryINA(BatteryBoardBus, 0x40);
// internal power busses
INA226 internalBus(I2Cinternal, 0x48);
INA226 unregulatedBus(I2Cinternal, 0x4A);
// external power busses
INA226 bus1(I2Cinternal, 0x40);
INA226 bus2(I2Cinternal, 0x41);
INA226 bus3(I2Cinternal, 0x43);
INA226 bus4(I2Cinternal, 0x42);

// Solar Cells
INA226 cellOutYp(SolarPanelsBus, 0x40);
INA226 cellOutYm(SolarPanelsBus, 0x41);
INA226 cellOutXp(SolarPanelsBus, 0x42);
INA226 cellOutXm(SolarPanelsBus, 0x43);

//Solar Arrays MPPT
INA226 mpptOutYp(SolarPanelsBus, 0x44);
INA226 mpptOutYm(SolarPanelsBus, 0x45);
INA226 mpptOutXp(SolarPanelsBus, 0x46);
INA226 mpptOutXm(SolarPanelsBus, 0x47);

// SP's are on the batteryBoard, Current per Solar Panel.
INA226 panelOutYp(SolarPanelsBus, 0x48);
INA226 panelOutYm(SolarPanelsBus, 0x4A);
INA226 panelOutXp(SolarPanelsBus, 0x4C);
INA226 panelOutXm(SolarPanelsBus, 0x4E);

TMP100 tempYp(SolarPanelsBus, 0x4B);
TMP100 tempYm(SolarPanelsBus, 0x4F);
TMP100 tempXp(SolarPanelsBus, 0x4D);
TMP100 tempXm(SolarPanelsBus, 0x49);

int batteryTemp = 0; //Register on ADCManager  (P8.4 / A21)

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
    tc->setBattStatus((!batteryGG.getVoltage(v)) &
                      (!batteryGG.getTemperature(t)) &
                      (!batteryGG.getAvailableCapacity(c)));
    tc->setBattVoltage(v);
    tc->setBattTemperature(t);
    tc->setBattCapacity(c);

    // INA on battery
    tc->setBattINAStatus((!batteryINA.getVoltage(v)) & (!batteryINA.getCurrent(i)));
    tc->setBattVoltage1(v);
    tc->setBattCurrent(i);

    // INAS on the output of solar panels
    tc->setSPYmStatus((!panelOutYm.getVoltage(v)) & (!panelOutYm.getCurrent(i)));
    tc->setSPYmVoltage(v);
    tc->setSPYmCurrent(i);

    tc->setSPYpStatus((!panelOutYp.getVoltage(v)) & (!panelOutYp.getCurrent(i)));
    tc->setSPYpVoltage(v);
    tc->setSPYpCurrent(i);

    tc->setSPXpStatus((!panelOutXp.getVoltage(v)) & (!panelOutXp.getCurrent(i)));
    tc->setSPXpVoltage(v);
    tc->setSPXpCurrent(i);

    tc->setSPXmStatus((!panelOutXm.getVoltage(v)) & (!panelOutXm.getCurrent(i)));
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

    // INAS on the output of solar cells
    tc->setSAYpStatus((!cellOutYp.getVoltage(v)) & (!cellOutYp.getCurrent(i)));
    tc->setSAYpVoltage(v);
    tc->setSAYpCurrent(i);
    tc->setSAYpTmpStatus(!tempYp.getTemperature(t));
    tc->setSAYpTemperature(t);

    tc->setSAYmStatus((!cellOutYm.getVoltage(v)) & (!cellOutYm.getCurrent(i)));
    tc->setSAYmVoltage(v);
    tc->setSAYmCurrent(i);
    tc->setSAYmTmpStatus(!tempYm.getTemperature(t));
    tc->setSAYmTemperature(t);

    tc->setSAXpStatus((!cellOutXp.getVoltage(v)) & (!cellOutXp.getCurrent(i)));
    tc->setSAXpVoltage(v);
    tc->setSAXpCurrent(i);
    tc->setSAXpTmpStatus(!tempXp.getTemperature(t));
    tc->setSAXpTemperature(t);

    tc->setSAXmStatus((!cellOutXm.getVoltage(v)) & (!cellOutXm.getCurrent(i)));
    tc->setSAXmVoltage(v);
    tc->setSAXmCurrent(i);
    tc->setSAXmTmpStatus(!tempXm.getTemperature(t));
    tc->setSAXmTemperature(t);

    //Added MPPT INA226:
    tc->setMPPTYpStatus((!mpptOutYp.getVoltage(v)) & (!mpptOutYp.getCurrent(i)));
    tc->setMPPTYpVoltage(v);
    tc->setMPPTYpCurrent(i);

    tc->setMPPTYmStatus((!mpptOutYm.getVoltage(v)) & (!mpptOutYm.getCurrent(i)));
    tc->setMPPTYmVoltage(v);
    tc->setMPPTYmCurrent(i);

    tc->setMPPTXpStatus((!mpptOutXp.getVoltage(v)) & (!mpptOutXp.getCurrent(i)));
    tc->setMPPTXpVoltage(v);
    tc->setMPPTXpCurrent(i);

    tc->setMPPTXmStatus((!mpptOutXm.getVoltage(v)) & (!mpptOutXm.getCurrent(i)));
    tc->setMPPTXmVoltage(v);
    tc->setMPPTXmCurrent(i);

    // power bus status
    tc->setBusStatus(busHandler.getStatus());
    tc->setBusErrorStatus(busHandler.getErrorStatus());

    int temperature_mV = ADCManager::getMeasurementVolt(batteryTemp);
    int temperature_C = (temperature_mV - 1886.3)/11.69;
    if(temperature_C > 0){
        Console::log("TMP20 Voltage: %d mV || %d C", temperature_mV, temperature_C);
    }else{
        Console::log("TMP20 Voltage: %d mV || -%d C", temperature_mV, -temperature_C);
    }
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
    cellOutYp.setShuntResistor(40);
    cellOutYm.setShuntResistor(40);
    cellOutXp.setShuntResistor(40);
    cellOutXm.setShuntResistor(40);
    panelOutYp.setShuntResistor(33);
    panelOutYm.setShuntResistor(33);
    panelOutXp.setShuntResistor(33);
    panelOutXm.setShuntResistor(33);
    mpptOutYp.setShuntResistor(40);
    mpptOutYm.setShuntResistor(40);
    mpptOutXp.setShuntResistor(40);
    mpptOutXm.setShuntResistor(40);
    batteryINA.setShuntResistor(33);

    // initialize temperature sensors
    tempYp.init();
    tempYm.init();
    tempXp.init();
    tempXm.init();
    batteryTemp = ADCManager::registerADC(ADC_INPUT_A22); //Register TMP20

    // initialize GasGauge
    batteryGG.init();

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

    Console::log("EPS booting...SLOT: %d", (int) Bootloader::getCurrentSlot());

    if(HAS_SW_VERSION == 1){
        Console::log("SW_VERSION: %s", (const char*)xtr(SW_VERSION));
    }
    // start the Task Manager: all activities from now on
    // will be managed from a dedicated task
    TaskManager::start(tasks, 2);
}
