/*
 * EPSTelemetryContainer.h
 *
 *  Created on: 5 Aug 2019
 *      Author: stefanosperett
 */

#ifndef EPSTELEMETRYCONTAINER_H_
#define EPSTELEMETRYCONTAINER_H_

#include "TelemetryContainer.h"

#define EPS_CONTAINER_SIZE  66
class EPSTelemetryContainer : public TelemetryContainer
{
protected:
    unsigned char telemetry[EPS_CONTAINER_SIZE];

public:
    virtual int size();
    virtual unsigned char * getArray();

    unsigned short getIntCurrent();
    void setIntCurrent(unsigned short ushort);
    unsigned short getIntVoltage();
    void setIntVoltage(unsigned short ushort);
};

#endif /* EPSTELEMETRYCONTAINER_H_ */
