/*
 * EPS.h
 *
 *  Created on: 23 Jul 2019
 *      Author: stefanosperett
 */

#ifndef EPS_H_
#define EPS_H_

#include <driverlib.h>
#include "CommandHandler.h"
#include "msp.h"
#include "DelfiPQcore.h"
#include "PQ9Bus.h"
#include "PQ9Frame.h"
#include "DWire.h"
#include "INA226.h"
#include "LTC2942.h"
#include "TMP100.h"
#include "DSerial.h"
#include "Task.h"
#include "PeriodicTask.h"
#include "TaskManager.h"
#include "PowerBusHandler.h"
#include "Service.h"
#include "HousekeepingService.h"
#include "EPSHousekeepingService.h"
#include "EPSTelemetryContainer.h"
#include "PingService.h"
#include "ResetService.h"
#include "SoftwareUpdateService.h"
#include "TestService.h"
#include "PowerBusHandler.h"

#define FCLOCK 48000000

#define EPS_ADDRESS     2

// callback functions
void acquireTelemetry(EPSTelemetryContainer *tc);
void periodicTask();

#endif /* EPS_H_ */
