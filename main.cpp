#include "EPS.h"
//
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
INA226 bus3(I2Cinternal, 0x42);
INA226 bus4(I2Cinternal, 0x43);
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
MB85RS fram(spi, GPIO_PORT_P1, GPIO_PIN0, MB85RS::MB85RS1MT );

// HardwareMonitor
HWMonitor hwMonitor(&fram);
// Bootloader
Bootloader bootLoader = Bootloader(fram);
// CDHS bus handler
PQ9Bus pq9bus(3, GPIO_PORT_P9, GPIO_PIN0);
// services running in the system
HousekeepingService<EPSTelemetryContainer> hk;
PingService ping;
ResetService reset( GPIO_PORT_P4, GPIO_PIN0, GPIO_PORT_P4, GPIO_PIN2 );

#ifndef SW_VERSION
SoftwareUpdateService SWupdate(fram);
#else
SoftwareUpdateService SWupdate(fram, (uint8_t*)xtr(SW_VERSION));
#endif

TestService test;
PowerBusHandler busHandler;
FRAMService framService(fram);
Service* services[] = { &hk, &ping, &reset, &SWupdate, &busHandler, &framService, &test };
// EPS board tasks
CommandHandler<PQ9Frame,PQ9Message> cmdHandler(pq9bus, services, 6);
PeriodicTask timerTask(1000, periodicTask);
PeriodicTask* periodicTasks[] = {&timerTask};
PeriodicTaskNotifier taskNotifier = PeriodicTaskNotifier(periodicTasks, 1);
Task* tasks[] = { &cmdHandler, &timerTask };
// system uptime
unsigned long uptime = 0;
FRAMVar<unsigned long> totalUptime;

void periodicTask()
{
    // increase the timer, this happens every second
    totalUptime += 1;
    uptime += 1;
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
    // make sure we dont reset on silent bus.
    reset.kickInternalWatchDog();
}

void acquireTelemetry(EPSTelemetryContainer *tc)
{
    unsigned short v, c;
    signed short i, t;
    unsigned char uc;
    unsigned long ul;
    //Set Telemetry:

    //HouseKeeping Header:
    tc->setStatus(Bootloader::getCurrentSlot());
    fram.read(FRAM_RESET_COUNTER + Bootloader::getCurrentSlot(), &uc, 1);
    tc->setBootCounter(uc);
    tc->setResetCause(hwMonitor.getResetStatus());
    tc->setUptime(uptime);
    tc->setTotalUptime((unsigned long) totalUptime);
    tc->setVersionNumber(2);
    tc->setMCUTemp(hwMonitor.getMCUTemp());

    // BatteryBoard Telemetry
    tc->setBatteryINAStatus((!batteryINA.getVoltage(v)) & (!batteryINA.getCurrent(i)));
    tc->setBatteryINAVoltage(v);
    tc->setBatteryINACurrent(i);
    tc->setBatteryGGStatus((!batteryGG.getVoltage(v)) &
                    (!batteryGG.getTemperature(t)) &
                    (!batteryGG.getAvailableCapacity(c)));
    tc->setBatteryGGVoltage(v);
    tc->setBatteryGGTemperature(t);
    tc->setBatteryGGCapacity(c);
    int temperature_C = (int)(10*((ADCManager::getMeasurementVolt(batteryTemp) - 1886.3)/(-11.69)));
    //Console::log("TMP20 = %d C || GG = %d C", temperature_C, tc->getBattTemperature());
    //Console::log("MCUTemp: %d", hwMonitor.getMCUTemp());
    tc->setBatteryTMP20Temperature(temperature_C);

    // Bus Telemetry
    tc->setBus1INAStatus((!bus1.getVoltage(v)) & (!bus1.getCurrent(i)));
    tc->setBus1Voltage(v);
    tc->setBus1Current(i);
    tc->setBus1State(busHandler.getStatus() & (0x01));
    tc->setBus1Error(busHandler.getErrorStatus() & (0x01));

    tc->setBus2INAStatus((!bus2.getVoltage(v)) & (!bus2.getCurrent(i)));
    tc->setBus2Voltage(v);
    tc->setBus2Current(i);
    tc->setBus2State(busHandler.getStatus() & (0x01 << 1));
    tc->setBus2Error(busHandler.getErrorStatus() & (0x01 << 1));

    tc->setBus3INAStatus((!bus3.getVoltage(v)) & (!bus3.getCurrent(i)));
    tc->setBus3Voltage(v);
    tc->setBus3Current(i);
    tc->setBus3State(busHandler.getStatus() & (0x01 << 2));
    tc->setBus3Error(busHandler.getErrorStatus() & (0x01 << 2));

    tc->setBus4INAStatus((!bus4.getVoltage(v)) & (!bus4.getCurrent(i)));
    tc->setBus4Voltage(v);
    tc->setBus4Current(i);
    tc->setBus4State(busHandler.getStatus() & (0x01 << 3));
    tc->setBus4Error(busHandler.getErrorStatus() & (0x01 << 3));

    // INAS on the output of solar panels
    tc->setCellXmINAStatus((!cellOutXm.getVoltage(v)) & (!cellOutXm.getCurrent(i)));
    tc->setCellXmCurrent(i);
    tc->setCellXmVoltage(v);

    tc->setCellXpINAStatus((!cellOutXp.getVoltage(v)) & (!cellOutXp.getCurrent(i)));
    tc->setCellXpCurrent(i);
    tc->setCellXpVoltage(v);

    tc->setCellYmINAStatus((!cellOutYm.getVoltage(v)) & (!cellOutYm.getCurrent(i)));
    tc->setCellYmCurrent(i);
    tc->setCellYmVoltage(v);

    tc->setCellYpINAStatus((!cellOutYp.getVoltage(v)) & (!cellOutYp.getCurrent(i)));
    tc->setCellYpCurrent(i);
    tc->setCellYpVoltage(v);

    tc->setPanelXmTMPStatus(!tempXm.getTemperature(t));
    tc->setPanelXmTemperature(t);
    tc->setPanelXpTMPStatus(!tempXp.getTemperature(t));
    tc->setPanelXpTemperature(t);
    tc->setPanelYmTMPStatus(!tempYm.getTemperature(t));
    tc->setPanelYmTemperature(t);
    tc->setPanelYpTMPStatus(!tempYp.getTemperature(t));
    tc->setPanelYpTemperature(t);

    tc->setMpptXmINAStatus((!mpptOutXm.getVoltage(v)) & (!mpptOutXm.getCurrent(i)));
    tc->setMpptXmCurrent(i);
    tc->setMpptXmVoltage(v);

    tc->setMpptXpINAStatus((!mpptOutXp.getVoltage(v)) & (!mpptOutXp.getCurrent(i)));
    tc->setMpptXpCurrent(i);
    tc->setMpptXpVoltage(v);

    tc->setMpptYmINAStatus((!mpptOutYm.getVoltage(v)) & (!mpptOutYm.getCurrent(i)));
    tc->setMpptYmCurrent(i);
    tc->setMpptYmVoltage(v);

    tc->setMpptYpINAStatus((!mpptOutYp.getVoltage(v)) & (!mpptOutYp.getCurrent(i)));
    tc->setMpptYpCurrent(i);
    tc->setMpptYpVoltage(v);

    tc->setPanelXmINAStatus((!panelOutXm.getVoltage(v)) & (!panelOutXm.getCurrent(i)));
    tc->setPanelXmCurrent(i);
    tc->setPanelXmVoltage(v);

    tc->setPanelXpINAStatus((!panelOutXp.getVoltage(v)) & (!panelOutXp.getCurrent(i)));
    tc->setPanelXpCurrent(i);
    tc->setPanelXpVoltage(v);

    tc->setPanelYmINAStatus((!panelOutYm.getVoltage(v)) & (!panelOutYm.getCurrent(i)));
    tc->setPanelYmCurrent(i);
    tc->setPanelYmVoltage(v);

    tc->setPanelYpINAStatus((!panelOutYp.getVoltage(v)) & (!panelOutYp.getCurrent(i)));
    tc->setPanelYpCurrent(i);
    tc->setPanelYpVoltage(v);

    // Internal and Unregulated Bus:
    tc->setInternalINAStatus((!internalBus.getVoltage(v)) & (!internalBus.getCurrent(i)));
    tc->setInternalINACurrent(i);
    tc->setInternalINAVoltage(v);

    tc->setUnregulatedINAStatus((!unregulatedBus.getVoltage(v)) & (!unregulatedBus.getCurrent(i)));
    tc->setUnregulatedINACurrent(i);
    tc->setUnregulatedINAVoltage(v);
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
    //   /* Configuring GPIOs (8.4 A21) */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P8, GPIO_PIN4,
                                                GPIO_TERTIARY_MODULE_FUNCTION);
    batteryTemp = ADCManager::registerADC(ADC_INPUT_A21);
    // Initialize I2C masters
    I2Cinternal.setFastMode();
    I2Cinternal.begin();
    SolarPanelsBus.setFastMode();
    SolarPanelsBus.begin();
    BatteryBoardBus.setFastMode();
    BatteryBoardBus.begin();

    // initialize the shunt resistors
    internalBus.setShuntResistor(33);
    unregulatedBus.setShuntResistor(33);
    bus1.setShuntResistor(33);
    bus2.setShuntResistor(33);
    bus3.setShuntResistor(33);
    bus4.setShuntResistor(33);
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

    // initialize GasGauge
    batteryGG.init();

    // Initialize SPI master
    spi.initMaster(DSPI::MODE0, DSPI::MSBFirst, 1000000);

    // Initialize fram and fram-variables
    fram.init();
    totalUptime.init(fram, FRAM_TOTAL_UPTIME);



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
    pq9bus.setReceiveHandler([](DataFrame &newFrame){ cmdHandler.received(newFrame); });

    // every time a command is correctly processed, call the watch-dog
    cmdHandler.onValidCommand([]{ reset.kickInternalWatchDog(); });

    Console::log("EPS booting...SLOT: %d", (int) Bootloader::getCurrentSlot());
    if(HAS_SW_VERSION == 1)
    {
        Console::log("SW_VERSION: %s", (const char*)xtr(SW_VERSION));
    }

    // start the Task Manager: all activities from now on
    // will be managed from a dedicated task
    TaskManager::start(tasks, 2);
}
