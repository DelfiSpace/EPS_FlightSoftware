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

#define VBUS_LOW_THRESHOLD          3000
#define VBUS_HIGH_THRESHOLD         3200

#define BUS1        1
#define BUS2        2
#define BUS3        4
#define BUS4        8

#define DEFAULT     BUS1

class PowerBusHandler
{
private:
    unsigned char busDesiredStatus;
    bool undervoltageProtection;

public:
    PowerBusHandler();
    void checkBussesStatus( EPSTelemetryContainer *tc );
    unsigned char getStatus( void );
};

#endif /* POWERBUSHANDLER_H_ */
