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
#include "DSerial.h"

#define COMMAND_SERVICE            1
#define COMMAND_ERROR              0
#define COMMAND_REQUEST            1
#define COMMAND_RESPONSE           2

#define VBUS_LOW_THRESHOLD          3000
#define VBUS_HIGH_THRESHOLD         3200

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
    virtual bool process( PQ9Frame &command, PQ9Bus &interface, PQ9Frame &workingBbuffer );
    void setPowerBus(unsigned char bus, unsigned char status);
};

#endif /* POWERBUSHANDLER_H_ */
