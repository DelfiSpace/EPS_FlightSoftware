/*
 * EPSTelemetryService.cpp
 *
 *  Created on: 4 Aug 2019
 *      Author: stefanosperett
 */

#include "EPSHousekeepingService.h"

extern DSerial serial;

TelemetryContainer* EPSHousekeepingService::getContainerToRead()
{
    return &(telemetryContainer[(telemetryIndex + 1) % 2]);
}

TelemetryContainer* EPSHousekeepingService::getContainerToWrite()
{
    return &(telemetryContainer[telemetryIndex]);
}
