/*
 * EPSTelemetryContainer.cpp
 *
 *  Created on: 5 Aug 2019
 *      Author: stefanosperett
 */

#include <EPSTelemetryContainer.h>

int EPSTelemetryContainer::size()
{
    return EPS_CONTAINER_SIZE;
}

unsigned char* EPSTelemetryContainer::getArray()
{
    return &telemetry[0];
}

unsigned short EPSTelemetryContainer::getIntCurrent()
{
    unsigned short ushort;
    ((unsigned char *)&ushort)[1] = telemetry[58];
    ((unsigned char *)&ushort)[0] = telemetry[59];
    return ushort;
}

void EPSTelemetryContainer::setIntCurrent(unsigned short ushort)
{
    // 59 58
    telemetry[58] = ((unsigned char *)&ushort)[1];
    telemetry[59] = ((unsigned char *)&ushort)[0];
}

unsigned short EPSTelemetryContainer::getIntVoltage()
{
    unsigned short ushort;
    ((unsigned char *)&ushort)[1] = telemetry[60];
    ((unsigned char *)&ushort)[0] = telemetry[61];
    return ushort;
}

void EPSTelemetryContainer::setIntVoltage(unsigned short ushort)
{
    // 61 60
    telemetry[60] = ((unsigned char *)&ushort)[1];
    telemetry[61] = ((unsigned char *)&ushort)[0];
}

