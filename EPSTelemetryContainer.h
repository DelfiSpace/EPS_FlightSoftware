//File Generated By TelemetryFromXTCE.py

#ifndef EPSTELEMETRYCONTAINER_H_
#define EPSTELEMETRYCONTAINER_H_

#include "TelemetryContainer.h"

#define EPS_CONTAINER_SIZE  112

class EPSTelemetryContainer : public TelemetryContainer
{
protected:
unsigned char telemetry[EPS_CONTAINER_SIZE];

public:
    virtual int size();
    virtual unsigned char * getArray();

    char getStatus();
    void setStatus(char var);

    char getBootCounter();
    void setBootCounter(char var);

    long getResetCause();
    void setResetCause(long var);

    unsigned long getUptime();
    void setUptime(unsigned long var);

    unsigned long getTotalUptime();
    void setTotalUptime(unsigned long var);

    char getVersionNumber();
    void setVersionNumber(char var);

    short getMCUTemp();
    void setMCUTemp(short var);

    bool getBatteryINAStatus();
    void setBatteryINAStatus(bool var);

    bool getBatteryGGStatus();
    void setBatteryGGStatus(bool var);

    bool getInternalINAStatus();
    void setInternalINAStatus(bool var);

    bool getUnregulatedINAStatus();
    void setUnregulatedINAStatus(bool var);

    bool getBus1INAStatus();
    void setBus1INAStatus(bool var);

    bool getBus2INAStatus();
    void setBus2INAStatus(bool var);

    bool getBus3INAStatus();
    void setBus3INAStatus(bool var);

    bool getBus4INAStatus();
    void setBus4INAStatus(bool var);

    bool getBus4Error();
    void setBus4Error(bool var);

    bool getBus3Error();
    void setBus3Error(bool var);

    bool getBus2Error();
    void setBus2Error(bool var);

    bool getBus1Error();
    void setBus1Error(bool var);

    bool getBus4State();
    void setBus4State(bool var);

    bool getBus3State();
    void setBus3State(bool var);

    bool getBus2State();
    void setBus2State(bool var);

    bool getBus1State();
    void setBus1State(bool var);

    bool getPanelYpINAStatus();
    void setPanelYpINAStatus(bool var);

    bool getPanelYmINAStatus();
    void setPanelYmINAStatus(bool var);

    bool getPanelXpINAStatus();
    void setPanelXpINAStatus(bool var);

    bool getPanelXmINAStatus();
    void setPanelXmINAStatus(bool var);

    bool getPanelYpTMPStatus();
    void setPanelYpTMPStatus(bool var);

    bool getPanelYmTMPStatus();
    void setPanelYmTMPStatus(bool var);

    bool getPanelXpTMPStatus();
    void setPanelXpTMPStatus(bool var);

    bool getPanelXmTMPStatus();
    void setPanelXmTMPStatus(bool var);

    bool getMpptYpINAStatus();
    void setMpptYpINAStatus(bool var);

    bool getMpptYmINAStatus();
    void setMpptYmINAStatus(bool var);

    bool getMpptXpINAStatus();
    void setMpptXpINAStatus(bool var);

    bool getMpptXmINAStatus();
    void setMpptXmINAStatus(bool var);

    bool getCellYpINAStatus();
    void setCellYpINAStatus(bool var);

    bool getCellYmINAStatus();
    void setCellYmINAStatus(bool var);

    bool getCellXpINAStatus();
    void setCellXpINAStatus(bool var);

    bool getCellXmINAStatus();
    void setCellXmINAStatus(bool var);

    short getInternalINACurrent();
    void setInternalINACurrent(short var);

    short getInternalINAVoltage();
    void setInternalINAVoltage(short var);

    short getUnregulatedINACurrent();
    void setUnregulatedINACurrent(short var);

    short getUnregulatedINAVoltage();
    void setUnregulatedINAVoltage(short var);

    short getBatteryGGVoltage();
    void setBatteryGGVoltage(short var);

    short getBatteryINAVoltage();
    void setBatteryINAVoltage(short var);

    short getBatteryINACurrent();
    void setBatteryINACurrent(short var);

    short getBatteryGGCapacity();
    void setBatteryGGCapacity(short var);

    short getBatteryGGTemperature();
    void setBatteryGGTemperature(short var);

    short getBatteryTMP20Temperature();
    void setBatteryTMP20Temperature(short var);

    short getBus4Current();
    void setBus4Current(short var);

    short getBus3Current();
    void setBus3Current(short var);

    short getBus2Current();
    void setBus2Current(short var);

    short getBus1Current();
    void setBus1Current(short var);

    short getBus4Voltage();
    void setBus4Voltage(short var);

    short getBus3Voltage();
    void setBus3Voltage(short var);

    short getBus2Voltage();
    void setBus2Voltage(short var);

    short getBus1Voltage();
    void setBus1Voltage(short var);

    short getPanelYpCurrent();
    void setPanelYpCurrent(short var);

    short getPanelYmCurrent();
    void setPanelYmCurrent(short var);

    short getPanelXpCurrent();
    void setPanelXpCurrent(short var);

    short getPanelXmCurrent();
    void setPanelXmCurrent(short var);

    short getPanelYpVoltage();
    void setPanelYpVoltage(short var);

    short getPanelYmVoltage();
    void setPanelYmVoltage(short var);

    short getPanelXpVoltage();
    void setPanelXpVoltage(short var);

    short getPanelXmVoltage();
    void setPanelXmVoltage(short var);

    short getPanelYpTemperature();
    void setPanelYpTemperature(short var);

    short getPanelYmTemperature();
    void setPanelYmTemperature(short var);

    short getPanelXpTemperature();
    void setPanelXpTemperature(short var);

    short getPanelXmTemperature();
    void setPanelXmTemperature(short var);

    short getMpptYpCurrent();
    void setMpptYpCurrent(short var);

    short getMpptYmCurrent();
    void setMpptYmCurrent(short var);

    short getMpptXpCurrent();
    void setMpptXpCurrent(short var);

    short getMpptXmCurrent();
    void setMpptXmCurrent(short var);

    short getMpptYpVoltage();
    void setMpptYpVoltage(short var);

    short getMpptYmVoltage();
    void setMpptYmVoltage(short var);

    short getMpptXpVoltage();
    void setMpptXpVoltage(short var);

    short getMpptXmVoltage();
    void setMpptXmVoltage(short var);

    short getCellYpCurrent();
    void setCellYpCurrent(short var);

    short getCellYmCurrent();
    void setCellYmCurrent(short var);

    short getCellXpCurrent();
    void setCellXpCurrent(short var);

    short getCellXmCurrent();
    void setCellXmCurrent(short var);

    short getCellYpVoltage();
    void setCellYpVoltage(short var);

    short getCellYmVoltage();
    void setCellYmVoltage(short var);

    short getCellXpVoltage();
    void setCellXpVoltage(short var);

    short getCellXmVoltage();
    void setCellXmVoltage(short var);

};

#endif /* EPSTELEMETRYCONTAINER_H_ */
