/*
 * PowerBusHandler.cpp
 *
 *  Created on: 17 Aug 2019
 *      Author: stefanosperett
 */

#include <PowerBusHandler.h>

extern DSerial serial;

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

    // initialize the bus status bits
    MAP_GPIO_setAsInputPin( GPIO_PORT_P3, GPIO_PIN0 );
    MAP_GPIO_setAsInputPin( GPIO_PORT_P3, GPIO_PIN1 );
    MAP_GPIO_setAsInputPin( GPIO_PORT_P3, GPIO_PIN2 );
    MAP_GPIO_setAsInputPin( GPIO_PORT_P3, GPIO_PIN3 );

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
            else
            {
                MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN0 );
            }
            if (DEFAULT & BUS2)
            {
                MAP_GPIO_setOutputHighOnPin( GPIO_PORT_P4, GPIO_PIN1 );
            }
            else
            {
                MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN1 );
            }
            if (DEFAULT & BUS3)
            {
                MAP_GPIO_setOutputHighOnPin( GPIO_PORT_P4, GPIO_PIN3 );
            }
            else
            {
                MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN3 );
            }
            if (DEFAULT & BUS4)
            {
                MAP_GPIO_setOutputHighOnPin( GPIO_PORT_P4, GPIO_PIN4 );
            }
            else
            {
                MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN4 );
            }
        }
    }
}

void PowerBusHandler::setPowerBus(unsigned char bus, unsigned char status)
{
    if (!undervoltageProtection)
    {
        switch(bus)
        {
            case BUS1:
            if (status)
            {
                MAP_GPIO_setOutputHighOnPin( GPIO_PORT_P4, GPIO_PIN0 );
            }
            else
            {
                MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN0 );
            }
            break;

            case BUS2:
            if (status)
            {
                MAP_GPIO_setOutputHighOnPin( GPIO_PORT_P4, GPIO_PIN1 );
            }
            else
            {
                MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN1 );
            }
            break;

            case BUS3:
            if (status)
            {
                MAP_GPIO_setOutputHighOnPin( GPIO_PORT_P4, GPIO_PIN3 );
            }
            else
            {
                MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN3 );
            }
            break;

            case BUS4:
            if (status)
            {
                MAP_GPIO_setOutputHighOnPin( GPIO_PORT_P4, GPIO_PIN4 );
            }
            else
            {
                MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN4 );
            }
            break;
        }
    }
    else
    {
        MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN0 );
        MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN1 );
        MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN2 );
        MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN3 );
    }
}

unsigned char PowerBusHandler::getStatus( void )
{
    return MAP_GPIO_getInputPinValue( GPIO_PORT_P4, GPIO_PIN0 ) |
          (MAP_GPIO_getInputPinValue( GPIO_PORT_P4, GPIO_PIN1 ) << 1) |
          (MAP_GPIO_getInputPinValue( GPIO_PORT_P4, GPIO_PIN2 ) << 2) |
          (MAP_GPIO_getInputPinValue( GPIO_PORT_P4, GPIO_PIN3 ) << 3);
}

unsigned char PowerBusHandler::getErrorStatus( void )
{
    return MAP_GPIO_getInputPinValue( GPIO_PORT_P3, GPIO_PIN0 ) |
          (MAP_GPIO_getInputPinValue( GPIO_PORT_P3, GPIO_PIN1 ) << 1) |
          (MAP_GPIO_getInputPinValue( GPIO_PORT_P3, GPIO_PIN2 ) << 2) |
          (MAP_GPIO_getInputPinValue( GPIO_PORT_P3, GPIO_PIN3 ) << 3);
}

bool PowerBusHandler::process(PQ9Frame &command, PQ9Bus &interface, PQ9Frame &workingBuffer)
{
    if (command.getPayload()[0] == COMMAND_SERVICE)
    {
        serial.println("PowerBusHandler: Execute");
        // prepare response frame
        workingBuffer.setDestination(command.getSource());
        workingBuffer.setSource(interface.getAddress());
        workingBuffer.setPayloadSize(3);
        workingBuffer.getPayload()[0] = COMMAND_SERVICE;

        if (command.getPayloadSize() == 4)
        {
            workingBuffer.getPayload()[2] = command.getPayload()[2];
            switch(command.getPayload()[2])
            {
                case 1:
                case 2:
                case 3:
                case 4:
                    setPowerBus(command.getPayload()[2], command.getPayload()[3]);
                    workingBuffer.getPayload()[1] = COMMAND_RESPONSE;
                    break;

                default:
                    workingBuffer.getPayload()[1] = COMMAND_ERROR;
                    break;
            }

            // send response
            interface.transmit(workingBuffer);
            // command processed
        }
        else
        {
            // unknown request
            workingBuffer.getPayload()[1] = COMMAND_ERROR;
            // send response: doing it here to make sure
            // a response is sent before reset but not 2
            interface.transmit(workingBuffer);
        }

        // command processed
        return true;
    }
    else
    {
        // this command is related to another service,
        // report the command was not processed
        return false;
    }
}

