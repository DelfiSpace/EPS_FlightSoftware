/*
 * EPSTelemetryService.h
 *
 *  Created on: 4 Aug 2019
 *      Author: stefanosperett
 */

#ifndef EPSHOUSEKEEPINGSERVICE_H_
#define EPSHOUSEKEEPINGSERVICE_H_

#include "HousekeepingService.h"
#include "EPSTelemetryContainer.h"
#include "DSerial.h"

class EPSHousekeepingService: public HousekeepingService
{
protected:
    EPSTelemetryContainer telemetryContainer[2];

public:
    virtual TelemetryContainer* getContainerToRead();
    virtual TelemetryContainer* getContainerToWrite();
};
#endif /* EPSHOUSEKEEPINGSERVICE_H_ */
