/*
 * PowerBusHandler.cpp
 *
 *  Created on: 17 Aug 2019
 *      Author: stefanosperett
 */

#include "PowerBusHandler.h"


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
    if (tc->getBattVoltage() < BATTERY_LOW_THRESHOLD)
    {
        MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN0 );
        MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN1 );
        MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN2 );
        MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN3 );

        if ( !undervoltageProtection )
        {
            Console::log("PowerBusHandler: Under-voltage protection ON");
        }
        undervoltageProtection = true;
    }
    else
    {
        if ((undervoltageProtection) && (tc->getBattVoltage() >= BATTERY_HIGH_THRESHOLD))
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

            Console::log("PowerBusHandler: Under-voltage protection OFF");
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

bool PowerBusHandler::process(DataMessage &command, DataMessage &workingBuffer)
{
    if (command.getService() == COMMAND_SERVICE_PBUS)
    {
        Console::log("PowerBusHandler: Set Bus ");
        // prepare response frame
        workingBuffer.setService(COMMAND_SERVICE_PBUS);
        workingBuffer.setMessageType(SERVICE_RESPONSE_REPLY);

        //Echo the requested bus/state back.
        workingBuffer.setPayloadSize(2);
        workingBuffer.getDataPayload()[0] = command.getDataPayload()[0];
        workingBuffer.getDataPayload()[1] = command.getDataPayload()[1];

        if ((command.getPayloadSize() == 2)) //make sure the bus message is exactly this length (targetBus + State)
        {
            switch(command.getDataPayload()[0]) //get Bus number
            {
                case 1:
                case 2:
                case 3:
                case 4:
                    setPowerBus(command.getDataPayload()[0], command.getDataPayload()[1]);
                    Console::log("%d - %s %s",command.getDataPayload()[0],command.getDataPayload()[1] ? "ON" : "OFF",undervoltageProtection ? "Undervoltage Protection Error" : "");
                    break;

                default:
                    Console::log("Bus does not exist!");
                    workingBuffer.getDataPayload()[0] = COMMAND_ERROR_PBUS; //set bus echo to '0' signifying an error.
                    break;
            }
        }
        else
        {
            Console::log("Uknown command / Wrong command length");
            // unknown request
            workingBuffer.setPayloadSize(1);
            workingBuffer.getDataPayload()[0] = COMMAND_ERROR_PBUS;
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
