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

        serial.println("PowerBusHandler: Under-voltage protection ON");
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
                MAP_GPIO_setOutputHighOnPin( GPIO_PORT_P4, GPIO_PIN2 );
            }
            else
            {
                MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN2 );
            }
            if (DEFAULT & BUS4)
            {
                MAP_GPIO_setOutputHighOnPin( GPIO_PORT_P4, GPIO_PIN3 );
            }
            else
            {
                MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN3 );
            }

            serial.println("PowerBusHandler: Under-voltage protection OFF");
        }
    }
}

void PowerBusHandler::setPowerBus(unsigned char bus, unsigned char status)
{
    if (!undervoltageProtection)
    {
        switch(bus)
        {
            case 1:
            if (status)
            {
                MAP_GPIO_setOutputHighOnPin( GPIO_PORT_P4, GPIO_PIN0 );
            }
            else
            {
                MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN0 );
            }
            break;

            case 2:
            if (status)
            {
                MAP_GPIO_setOutputHighOnPin( GPIO_PORT_P4, GPIO_PIN1 );
            }
            else
            {
                MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN1 );
            }
            break;

            case 3:
            if (status)
            {
                MAP_GPIO_setOutputHighOnPin( GPIO_PORT_P4, GPIO_PIN2 );
            }
            else
            {
                MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN2 );
            }
            break;

            case 4:
            if (status)
            {
                MAP_GPIO_setOutputHighOnPin( GPIO_PORT_P4, GPIO_PIN3 );
            }
            else
            {
                MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN3 );
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
    return ((MAP_GPIO_getInputPinValue( GPIO_PORT_P4, GPIO_PIN0 ) == GPIO_INPUT_PIN_HIGH) |
           ((MAP_GPIO_getInputPinValue( GPIO_PORT_P4, GPIO_PIN1 ) == GPIO_INPUT_PIN_HIGH) << 1) |
           ((MAP_GPIO_getInputPinValue( GPIO_PORT_P4, GPIO_PIN2 ) == GPIO_INPUT_PIN_HIGH) << 2) |
           ((MAP_GPIO_getInputPinValue( GPIO_PORT_P4, GPIO_PIN3 ) == GPIO_INPUT_PIN_HIGH) << 3));
}

unsigned char PowerBusHandler::getErrorStatus( void )
{
    return ((MAP_GPIO_getInputPinValue( GPIO_PORT_P3, GPIO_PIN0 ) == GPIO_INPUT_PIN_HIGH) |
           ((MAP_GPIO_getInputPinValue( GPIO_PORT_P3, GPIO_PIN1 ) == GPIO_INPUT_PIN_HIGH) << 1) |
           ((MAP_GPIO_getInputPinValue( GPIO_PORT_P3, GPIO_PIN2 ) == GPIO_INPUT_PIN_HIGH) << 2) |
           ((MAP_GPIO_getInputPinValue( GPIO_PORT_P3, GPIO_PIN3 ) == GPIO_INPUT_PIN_HIGH) << 3));
}

bool PowerBusHandler::process(PQ9Frame &command, PQ9Sender &interface, PQ9Frame &workingBuffer)
{
    if (command.getPayload()[0] == COMMAND_SERVICE)
    {
        serial.print("PowerBusHandler: Set Bus ");
        // prepare response frame
        workingBuffer.setDestination(command.getSource());
        workingBuffer.setSource(interface.getAddress());
        workingBuffer.setPayloadSize(4);
        workingBuffer.getPayload()[0] = COMMAND_SERVICE;
        workingBuffer.getPayload()[2] = command.getPayload()[2];
        workingBuffer.getPayload()[3] = command.getPayload()[3];

        if ((command.getPayloadSize() == 4) && (command.getPayload()[1] == COMMAND_REQUEST))
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
                    serial.print(command.getPayload()[2], DEC);
                    serial.print(" ");
                    serial.println(command.getPayload()[3] ? "ON" : "OFF");
                    break;

                default:
                    serial.println("error");
                    workingBuffer.getPayload()[1] = COMMAND_ERROR;
                    break;
            }
        }
        else
        {
            serial.println("error");
            // unknown request
            workingBuffer.getPayload()[1] = COMMAND_ERROR;
        }

        // send response
        interface.transmit(workingBuffer);

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
