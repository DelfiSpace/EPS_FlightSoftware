/*
 * TestService.cpp
 *
 *  Created on: 6 Aug 2019
 *      Author: stefanosperett
 */

#include "TestService.h"
#include "LTC2942.h"

extern DSerial serial;
extern LTC2942 gasGauge;

bool TestService::process(PQ9Frame &command, PQ9Bus &interface, PQ9Frame &workingBuffer)
{
    if (command.getPayload()[0] == 0)
    {
        serial.println("TestService");

        if (command.getPayload()[1] == 1)
        {
            unsigned short coulomb_charge;
            if (!gasGauge.getAvailableCapacity(coulomb_charge))
            {
                serial.print(coulomb_charge, DEC);
                serial.println();

            }
            else
            {
                serial.println("gas gauge failed");
            }

        }
        else if (command.getPayload()[1] == 2)
        {
            gasGauge.setRawCharge(0);
        }
        else if (command.getPayload()[1] == 3)
        {
            gasGauge.setRawCharge(0xFFFF);
        }

        return true;
    }
    return false;
}


