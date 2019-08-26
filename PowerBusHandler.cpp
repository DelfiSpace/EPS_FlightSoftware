/*
 * PowerBusHandler.cpp
 *
 *  Created on: 17 Aug 2019
 *      Author: stefanosperett
 */

#include <PowerBusHandler.h>

PowerBusHandler::PowerBusHandler()
{
    // always start with the busses in protection
    undervoltageProtection = true;

    // set power busses enable pins to GPIO mode
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin( GPIO_PORT_P4, GPIO_PIN0,
                                             GPIO_PRIMARY_MODULE_FUNCTION );
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin( GPIO_PORT_P4, GPIO_PIN1,
                                             GPIO_PRIMARY_MODULE_FUNCTION );
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin( GPIO_PORT_P4, GPIO_PIN2,
                                             GPIO_PRIMARY_MODULE_FUNCTION );
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin( GPIO_PORT_P4, GPIO_PIN3,
                                             GPIO_PRIMARY_MODULE_FUNCTION );

    // initialize the pins to control the power busses
    MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN0 );
    MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN1 );
    MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN2 );
    MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN3 );

    MAP_GPIO_setAsOutputPin( GPIO_PORT_P4, GPIO_PIN0 );
    MAP_GPIO_setAsOutputPin( GPIO_PORT_P4, GPIO_PIN1 );
    MAP_GPIO_setAsOutputPin( GPIO_PORT_P4, GPIO_PIN2 );
    MAP_GPIO_setAsOutputPin( GPIO_PORT_P4, GPIO_PIN3 );

}

void PowerBusHandler::checkBussesStatus( EPSTelemetryContainer *tc )
{
    // check if Unregulated Bus voltage is below threshold
    // if so, switch all buses off
    if (tc->getURBVoltage() < VBUS_LOW_THRESHOLD)
    {
        undervoltageProtection = true;
        MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN0 );
        MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN1 );
        MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN2 );
        MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN3 );
    }
    else
    {
        if ((undervoltageProtection) && (tc->getURBVoltage() >= VBUS_HIGH_THRESHOLD))
        {
            undervoltageProtection = false;
            if (DEFAULT & BUS1)
            {
                MAP_GPIO_setOutputHighOnPin( GPIO_PORT_P4, GPIO_PIN0 );
            }
            if (DEFAULT & BUS2)
            {
                MAP_GPIO_setOutputHighOnPin( GPIO_PORT_P4, GPIO_PIN1 );
            }
            if (DEFAULT & BUS3)
            {
                MAP_GPIO_setOutputHighOnPin( GPIO_PORT_P4, GPIO_PIN3 );
            }
            if (DEFAULT & BUS4)
            {
                MAP_GPIO_setOutputHighOnPin( GPIO_PORT_P4, GPIO_PIN4 );
            }
        }
    }
}

unsigned char PowerBusHandler::getStatus( void )
{
    return MAP_GPIO_getInputPinValue( GPIO_PORT_P4, GPIO_PIN0 ) |
          (MAP_GPIO_getInputPinValue( GPIO_PORT_P4, GPIO_PIN1 ) < 1) |
          (MAP_GPIO_getInputPinValue( GPIO_PORT_P4, GPIO_PIN2 ) < 2) |
          (MAP_GPIO_getInputPinValue( GPIO_PORT_P4, GPIO_PIN3 ) < 3);
}
