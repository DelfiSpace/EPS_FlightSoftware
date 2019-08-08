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

    unsigned short getIntBCurrent();
    void setIntBCurrent(unsigned short ushort);
    unsigned short getIntBVoltage();
    void setIntBVoltage(unsigned short ushort);
    bool getIntBStatus();
    void setIntBStatus(bool bval);

    unsigned short getURBCurrent();
    void setURBCurrent(unsigned short ushort);
    unsigned short getURBVoltage();
    void setURBVoltage(unsigned short ushort);
    bool getURBStatus();
    void setURBStatus(bool bval);

    unsigned short getB1Current();
    void setB1Current(unsigned short ushort);
    unsigned short getB1Voltage();
    void setB1Voltage(unsigned short ushort);
    bool getB1Status();
    void setB1Status(bool bval);

    unsigned short getB2Current();
    void setB2Current(unsigned short ushort);
    unsigned short getB2Voltage();
    void setB2Voltage(unsigned short ushort);
    bool getB2Status();
    void setB2Status(bool bval);

    unsigned short getB3Current();
    void setB3Current(unsigned short ushort);
    unsigned short getB3Voltage();
    void setB3Voltage(unsigned short ushort);
    bool getB3Status();
    void setB3Status(bool bval);

    unsigned short getB4Current();
    void setB4Current(unsigned short ushort);
    unsigned short getB4Voltage();
    void setB4Voltage(unsigned short ushort);
    bool getB4Status();
    void setB4Status(bool bval);

    unsigned short getSAYpCurrent();
    void setSAYpCurrent(unsigned short ushort);
    unsigned short getSAYpVoltage();
    void setSAYpVoltage(unsigned short ushort);
    bool getSAYpStatus();
    void setSAYpStatus(bool bval);

    unsigned short getSAYmCurrent();
    void setSAYmCurrent(unsigned short ushort);
    unsigned short getSAYmVoltage();
    void setSAYmVoltage(unsigned short ushort);
    bool getSAYmStatus();
    void setSAYmStatus(bool bval);

    unsigned short getSAXpCurrent();
    void setSAXpCurrent(unsigned short ushort);
    unsigned short getSAXpVoltage();
    void setSAXpVoltage(unsigned short ushort);
    bool getSAXpStatus();
    void setSAXpStatus(bool bval);

    unsigned short getSAXmCurrent();
    void setSAXmCurrent(unsigned short ushort);
    unsigned short getSAXmVoltage();
    void setSAXmVoltage(unsigned short ushort);
    bool getSAXmStatus();
    void setSAXmStatus(bool bval);

    unsigned short getGGVoltage();
    void setGGVoltage(unsigned short ushort);
    bool getGGStatus();
    void setGGStatus(bool bval);
};

#endif /* EPSTELEMETRYCONTAINER_H_ */
