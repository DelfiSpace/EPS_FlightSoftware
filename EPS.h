/*
 * EPS.h
 *
 *  Created on: 23 Jul 2019
 *      Author: stefanosperett
 */

#ifndef EPS_H_
#define EPS_H_

#include <driverlib.h>
#include "SLOT_SELECT.h"
#include "CommandHandler.h"
#include "msp.h"
#include "DelfiPQcore.h"
#include "PQ9Bus.h"
#include "PQ9Frame.h"
#include "PQ9Message.h"
#include "DWire.h"
#include "INA226.h"
#include "LTC2942.h"
#include "TMP100.h"
#include "DSPI.h"
#include "MB85RS.h"
#include "Console.h"
#include "Task.h"
#include "PeriodicTask.h"
#include "TaskManager.h"
#include "PowerBusHandler.h"
#include "Service.h"
#include "HousekeepingService.h"
#include "EPSTelemetryContainer.h"
#include "PingService.h"
#include "ResetService.h"
#include "SoftwareUpdateService.h"
#include "TestService.h"
#include "PowerBusHandler.h"
#include "PeriodicTaskNotifier.h"
#include "Bootloader.h"
#include "HWMonitor.h"
#include "ADCManager.h"

#define FCLOCK 48000000
#define SCLOCK 32768

#define EPS_ADDRESS     2

// callback functions
void acquireTelemetry(EPSTelemetryContainer *tc);
void periodicTask();

#endif /* EPS_H_ */
