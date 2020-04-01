/*
 * PowerBusHandler.h
 *
 *  Created on: 17 Aug 2019
 *      Author: stefanosperett
 */

#ifndef POWERBUSHANDLER_H_
#define POWERBUSHANDLER_H_

#include <driverlib.h>
#include "EPSTelemetryContainer.h"
#include "Service.h"
#include "Console.h"

#define COMMAND_SERVICE_PBUS            1
#define COMMAND_ERROR_PBUS                0
#define COMMAND_REQUEST_PBUS              1
#define COMMAND_RESPONSE_PBUS             2

#define BATTERY_LOW_THRESHOLD          3000    // mV
#define BATTERY_HIGH_THRESHOLD         3600    // mV

#define BUS1        1
#define BUS2        2
#define BUS3        4
#define BUS4        8

#define DEFAULT     BUS1

class PowerBusHandler: public Service
{
private:
    bool undervoltageProtection;

public:
    PowerBusHandler();
    void checkBussesStatus( EPSTelemetryContainer *tc );
    unsigned char getStatus( void );
    unsigned char getErrorStatus( void );
    virtual bool process( DataMessage &command, DataMessage &workingBbuffer );
    void setPowerBus(unsigned char bus, unsigned char status);
};

#endif /* POWERBUSHANDLER_H_ */
