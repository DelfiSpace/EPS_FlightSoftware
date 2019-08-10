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

bool EPSTelemetryContainer::getB1Status()
{
    return ((telemetry[8] & 0x10) != 0);
}

void EPSTelemetryContainer::setB1Status(bool bval)
{
    telemetry[8] &= (~0x10);
    telemetry[8] |= bval ? 0x10 : 0x00;
}

bool EPSTelemetryContainer::getB2Status()
{
    return ((telemetry[8] & 0x20) != 0);
}

void EPSTelemetryContainer::setB2Status(bool bval)
{
    telemetry[8] &= (~0x20);
    telemetry[8] |= bval ? 0x20 : 0x00;
}

bool EPSTelemetryContainer::getB3Status()
{
    return ((telemetry[8] & 0x40) != 0);
}

void EPSTelemetryContainer::setB3Status(bool bval)
{
    telemetry[8] &= (~0x40);
    telemetry[8] |= bval ? 0x40 : 0x00;
}

bool EPSTelemetryContainer::getB4Status()
{
    return ((telemetry[8] & 0x80) != 0);
}

void EPSTelemetryContainer::setB4Status(bool bval)
{
    telemetry[8] &= (~0x80);
    telemetry[8] |= bval ? 0x80 : 0x00;
}

bool EPSTelemetryContainer::getIntBStatus()
{
    return ((telemetry[7] & 0x01) != 0);
}

void EPSTelemetryContainer::setIntBStatus(bool bval)
{
    telemetry[7] &= (~0x01);
    telemetry[7] |= bval ? 0x01 : 0x00;
}

bool EPSTelemetryContainer::getURBStatus()
{
    return ((telemetry[7] & 0x02) != 0);
}

void EPSTelemetryContainer::setURBStatus(bool bval)
{
    telemetry[7] &= (~0x02);
    telemetry[7] |= bval ? 0x02 : 0x00;
}

bool EPSTelemetryContainer::getSAYpStatus()
{
    return ((telemetry[7] & 0x04) != 0);
}

void EPSTelemetryContainer::setSAYpStatus(bool bval)
{
    telemetry[7] &= (~0x04);
    telemetry[7] |= bval ? 0x04 : 0x00;
}

bool EPSTelemetryContainer::getSAYmStatus()
{
    return ((telemetry[7] & 0x08) != 0);
}

void EPSTelemetryContainer::setSAYmStatus(bool bval)
{
    telemetry[7] &= (~0x08);
    telemetry[7] |= bval ? 0x08 : 0x00;
}

bool EPSTelemetryContainer::getSAXpStatus()
{
    return ((telemetry[7] & 0x10) != 0);
}

void EPSTelemetryContainer::setSAXpStatus(bool bval)
{
    telemetry[7] &= (~0x10);
    telemetry[7] |= bval ? 0x10 : 0x00;
}

bool EPSTelemetryContainer::getSAXmStatus()
{
    return ((telemetry[7] & 0x20) != 0);
}

void EPSTelemetryContainer::setSAXmStatus(bool bval)
{
    telemetry[7] &= (~0x20);
    telemetry[7] |= bval ? 0x20 : 0x00;
}

bool EPSTelemetryContainer::getBattStatus()
{
    return ((telemetry[7] & 0x40) != 0);
}

void EPSTelemetryContainer::setBattStatus(bool bval)
{
    telemetry[7] &= (~0x40);
    telemetry[7] |= bval ? 0x40 : 0x00;
}

bool EPSTelemetryContainer::getSAYpTmpStatus()
{
    return ((telemetry[8] & 0x01) != 0);
}

void EPSTelemetryContainer::setSAYpTmpStatus(bool bval)
{
    telemetry[8] &= (~0x01);
    telemetry[8] |= bval ? 0x01 : 0x00;
}

bool EPSTelemetryContainer::getSAYmTmpStatus()
{
    return ((telemetry[8] & 0x02) != 0);
}

void EPSTelemetryContainer::setSAYmTmpStatus(bool bval)
{
    telemetry[8] &= (~0x02);
    telemetry[8] |= bval ? 0x02 : 0x00;
}

bool EPSTelemetryContainer::getSAXpTmpStatus()
{
    return ((telemetry[8] & 0x04) != 0);
}

void EPSTelemetryContainer::setSAXpTmpStatus(bool bval)
{
    telemetry[8] &= (~0x04);
    telemetry[8] |= bval ? 0x04 : 0x00;
}

bool EPSTelemetryContainer::getSAXmTmpStatus()
{
    return ((telemetry[8] & 0x08) != 0);
}

void EPSTelemetryContainer::setSAXmTmpStatus(bool bval)
{
    telemetry[8] &= (~0x08);
    telemetry[8] |= bval ? 0x08 : 0x00;
}

signed short EPSTelemetryContainer::getIntBCurrent()
{
    unsigned short ushort;
    ((unsigned char *)&ushort)[1] = telemetry[58];
    ((unsigned char *)&ushort)[0] = telemetry[59];
    return ushort;
}

void EPSTelemetryContainer::setIntBCurrent(signed short ushort)
{
    telemetry[58] = ((unsigned char *)&ushort)[1];
    telemetry[59] = ((unsigned char *)&ushort)[0];
}

unsigned short EPSTelemetryContainer::getIntBVoltage()
{
    unsigned short ushort;
    ((unsigned char *)&ushort)[1] = telemetry[60];
    ((unsigned char *)&ushort)[0] = telemetry[61];
    return ushort;
}

void EPSTelemetryContainer::setIntBVoltage(unsigned short ushort)
{
    telemetry[60] = ((unsigned char *)&ushort)[1];
    telemetry[61] = ((unsigned char *)&ushort)[0];
}

signed short EPSTelemetryContainer::getURBCurrent()
{
    unsigned short ushort;
    ((unsigned char *)&ushort)[1] = telemetry[62];
    ((unsigned char *)&ushort)[0] = telemetry[63];
    return ushort;
}

void EPSTelemetryContainer::setURBCurrent(signed short ushort)
{
    telemetry[62] = ((unsigned char *)&ushort)[1];
    telemetry[63] = ((unsigned char *)&ushort)[0];
}

unsigned short EPSTelemetryContainer::getURBVoltage()
{
    unsigned short ushort;
    ((unsigned char *)&ushort)[1] = telemetry[64];
    ((unsigned char *)&ushort)[0] = telemetry[65];
    return ushort;
}

void EPSTelemetryContainer::setURBVoltage(unsigned short ushort)
{
    telemetry[64] = ((unsigned char *)&ushort)[1];
    telemetry[65] = ((unsigned char *)&ushort)[0];
}

signed short EPSTelemetryContainer::getB1Current()
{
    unsigned short ushort;
    ((unsigned char *)&ushort)[1] = telemetry[33];
    ((unsigned char *)&ushort)[0] = telemetry[34];
    return ushort;
}

void EPSTelemetryContainer::setB1Current(signed short ushort)
{
    telemetry[33] = ((unsigned char *)&ushort)[1];
    telemetry[34] = ((unsigned char *)&ushort)[0];
}

unsigned short EPSTelemetryContainer::getB1Voltage()
{
    unsigned short ushort;
    ((unsigned char *)&ushort)[1] = telemetry[41];
    ((unsigned char *)&ushort)[0] = telemetry[42];
    return ushort;
}

void EPSTelemetryContainer::setB1Voltage(unsigned short ushort)
{
    telemetry[41] = ((unsigned char *)&ushort)[1];
    telemetry[42] = ((unsigned char *)&ushort)[0];
}

signed short EPSTelemetryContainer::getB2Current()
{
    unsigned short ushort;
    ((unsigned char *)&ushort)[1] = telemetry[35];
    ((unsigned char *)&ushort)[0] = telemetry[36];
    return ushort;
}

void EPSTelemetryContainer::setB2Current(signed short ushort)
{
    telemetry[35] = ((unsigned char *)&ushort)[1];
    telemetry[36] = ((unsigned char *)&ushort)[0];
}

unsigned short EPSTelemetryContainer::getB2Voltage()
{
    unsigned short ushort;
    ((unsigned char *)&ushort)[1] = telemetry[43];
    ((unsigned char *)&ushort)[0] = telemetry[44];
    return ushort;
}

void EPSTelemetryContainer::setB2Voltage(unsigned short ushort)
{
    telemetry[43] = ((unsigned char *)&ushort)[1];
    telemetry[44] = ((unsigned char *)&ushort)[0];
}

signed short EPSTelemetryContainer::getB3Current()
{
    unsigned short ushort;
    ((unsigned char *)&ushort)[1] = telemetry[37];
    ((unsigned char *)&ushort)[0] = telemetry[38];
    return ushort;
}

void EPSTelemetryContainer::setB3Current(signed short ushort)
{
    telemetry[37] = ((unsigned char *)&ushort)[1];
    telemetry[38] = ((unsigned char *)&ushort)[0];
}

unsigned short EPSTelemetryContainer::getB3Voltage()
{
    unsigned short ushort;
    ((unsigned char *)&ushort)[1] = telemetry[45];
    ((unsigned char *)&ushort)[0] = telemetry[46];
    return ushort;
}

void EPSTelemetryContainer::setB3Voltage(unsigned short ushort)
{
    telemetry[45] = ((unsigned char *)&ushort)[1];
    telemetry[46] = ((unsigned char *)&ushort)[0];
}

signed short EPSTelemetryContainer::getB4Current()
{
    unsigned short ushort;
    ((unsigned char *)&ushort)[1] = telemetry[39];
    ((unsigned char *)&ushort)[0] = telemetry[40];
    return ushort;
}

void EPSTelemetryContainer::setB4Current(signed short ushort)
{
    telemetry[39] = ((unsigned char *)&ushort)[1];
    telemetry[40] = ((unsigned char *)&ushort)[0];
}

unsigned short EPSTelemetryContainer::getB4Voltage()
{
    unsigned short ushort;
    ((unsigned char *)&ushort)[1] = telemetry[47];
    ((unsigned char *)&ushort)[0] = telemetry[48];
    return ushort;
}

void EPSTelemetryContainer::setB4Voltage(unsigned short ushort)
{
    telemetry[47] = ((unsigned char *)&ushort)[1];
    telemetry[48] = ((unsigned char *)&ushort)[0];
}

signed short EPSTelemetryContainer::getSAYpCurrent()
{
    unsigned short ushort;
    ((unsigned char *)&ushort)[1] = telemetry[17];
    ((unsigned char *)&ushort)[0] = telemetry[18];
    return ushort;
}

void EPSTelemetryContainer::setSAYpCurrent(signed short ushort)
{
    telemetry[17] = ((unsigned char *)&ushort)[1];
    telemetry[18] = ((unsigned char *)&ushort)[0];
}

unsigned short EPSTelemetryContainer::getSAYpVoltage()
{
    unsigned short ushort;
    ((unsigned char *)&ushort)[1] = telemetry[25];
    ((unsigned char *)&ushort)[0] = telemetry[26];
    return ushort;
}

void EPSTelemetryContainer::setSAYpVoltage(unsigned short ushort)
{
    telemetry[25] = ((unsigned char *)&ushort)[1];
    telemetry[26] = ((unsigned char *)&ushort)[0];
}

signed short EPSTelemetryContainer::getSAYmCurrent()
{
    unsigned short ushort;
    ((unsigned char *)&ushort)[1] = telemetry[19];
    ((unsigned char *)&ushort)[0] = telemetry[20];
    return ushort;
}

void EPSTelemetryContainer::setSAYmCurrent(signed short ushort)
{
    telemetry[19] = ((unsigned char *)&ushort)[1];
    telemetry[20] = ((unsigned char *)&ushort)[0];
}

unsigned short EPSTelemetryContainer::getSAYmVoltage()
{
    unsigned short ushort;
    ((unsigned char *)&ushort)[1] = telemetry[27];
    ((unsigned char *)&ushort)[0] = telemetry[28];
    return ushort;
}

void EPSTelemetryContainer::setSAYmVoltage(unsigned short ushort)
{
    telemetry[27] = ((unsigned char *)&ushort)[1];
    telemetry[28] = ((unsigned char *)&ushort)[0];
}

signed short EPSTelemetryContainer::getSAXpCurrent()
{
    unsigned short ushort;
    ((unsigned char *)&ushort)[1] = telemetry[21];
    ((unsigned char *)&ushort)[0] = telemetry[22];
    return ushort;
}

void EPSTelemetryContainer::setSAXpCurrent(signed short ushort)
{
    telemetry[21] = ((unsigned char *)&ushort)[1];
    telemetry[22] = ((unsigned char *)&ushort)[0];
}

unsigned short EPSTelemetryContainer::getSAXpVoltage()
{
    unsigned short ushort;
    ((unsigned char *)&ushort)[1] = telemetry[29];
    ((unsigned char *)&ushort)[0] = telemetry[30];
    return ushort;
}

void EPSTelemetryContainer::setSAXpVoltage(unsigned short ushort)
{
    telemetry[29] = ((unsigned char *)&ushort)[1];
    telemetry[30] = ((unsigned char *)&ushort)[0];
}

signed short EPSTelemetryContainer::getSAXmCurrent()
{
    unsigned short ushort;
    ((unsigned char *)&ushort)[1] = telemetry[23];
    ((unsigned char *)&ushort)[0] = telemetry[24];
    return ushort;
}

void EPSTelemetryContainer::setSAXmCurrent(signed short ushort)
{
    telemetry[23] = ((unsigned char *)&ushort)[1];
    telemetry[24] = ((unsigned char *)&ushort)[0];
}

unsigned short EPSTelemetryContainer::getSAXmVoltage()
{
    unsigned short ushort;
    ((unsigned char *)&ushort)[1] = telemetry[31];
    ((unsigned char *)&ushort)[0] = telemetry[32];
    return ushort;
}

void EPSTelemetryContainer::setSAXmVoltage(unsigned short ushort)
{
    telemetry[31] = ((unsigned char *)&ushort)[1];
    telemetry[32] = ((unsigned char *)&ushort)[0];
}

unsigned short EPSTelemetryContainer::getBattVoltage()
{
    unsigned short ushort;
    ((unsigned char *)&ushort)[1] = telemetry[49];
    ((unsigned char *)&ushort)[0] = telemetry[50];
    return ushort;
}

void EPSTelemetryContainer::setBattVoltage(unsigned short ushort)
{
    telemetry[49] = ((unsigned char *)&ushort)[1];
    telemetry[50] = ((unsigned char *)&ushort)[0];
}

signed short EPSTelemetryContainer::getBattTemperature()
{
    unsigned short ushort;
    ((unsigned char *)&ushort)[1] = telemetry[53];
    ((unsigned char *)&ushort)[0] = telemetry[54];
    return ushort;
}

void EPSTelemetryContainer::setBattTemperature(signed short ushort)
{
    telemetry[53] = ((unsigned char *)&ushort)[1];
    telemetry[54] = ((unsigned char *)&ushort)[0];
}

signed short EPSTelemetryContainer::getSAYpTemperature()
{
    unsigned short ushort;
    ((unsigned char *)&ushort)[1] = telemetry[9];
    ((unsigned char *)&ushort)[0] = telemetry[10];
    return ushort;
}

void EPSTelemetryContainer::setSAYpTemperature(signed short ushort)
{
    telemetry[9] = ((unsigned char *)&ushort)[1];
    telemetry[10] = ((unsigned char *)&ushort)[0];
}

signed short EPSTelemetryContainer::getSAYmTemperature()
{
    unsigned short ushort;
    ((unsigned char *)&ushort)[1] = telemetry[11];
    ((unsigned char *)&ushort)[0] = telemetry[12];
    return ushort;
}

void EPSTelemetryContainer::setSAYmTemperature(signed short ushort)
{
    telemetry[11] = ((unsigned char *)&ushort)[1];
    telemetry[12] = ((unsigned char *)&ushort)[0];
}

signed short EPSTelemetryContainer::getSAXpTemperature()
{
    unsigned short ushort;
    ((unsigned char *)&ushort)[1] = telemetry[13];
    ((unsigned char *)&ushort)[0] = telemetry[14];
    return ushort;
}

void EPSTelemetryContainer::setSAXpTemperature(signed short ushort)
{
    telemetry[13] = ((unsigned char *)&ushort)[1];
    telemetry[14] = ((unsigned char *)&ushort)[0];
}

signed short EPSTelemetryContainer::getSAXmTemperature()
{
    unsigned short ushort;
    ((unsigned char *)&ushort)[1] = telemetry[15];
    ((unsigned char *)&ushort)[0] = telemetry[16];
    return ushort;
}

void EPSTelemetryContainer::setSAXmTemperature(signed short ushort)
{
    telemetry[15] = ((unsigned char *)&ushort)[1];
    telemetry[16] = ((unsigned char *)&ushort)[0];
}
