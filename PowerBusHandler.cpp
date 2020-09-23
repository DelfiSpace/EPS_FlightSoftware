/*
 * PowerBusHandler.cpp
 *
 *  Created on: 17 Aug 2019
 *      Author: stefanosperett
 */

#include "PowerBusHandler.h"

PowerBusHandler* _powerBusStub;

void PowerBusHandler::underVoltageTrip(){

    uint16_t status = MAP_GPIO_getInterruptStatus(GPIO_PORT_P2, GPIO_PIN7);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P2, PIN_ALL16);
    if(status & GPIO_PIN7)
    {
        Console::log("PowerBusHandler: Under-voltage TRIP");
        MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P8, GPIO_PIN0 );
        MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P8, GPIO_PIN1 );
        MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P8, GPIO_PIN2 );
        MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P8, GPIO_PIN3 );

        if ( !undervoltageProtection )
        {
            Console::log("PowerBusHandler: Under-voltage protection ON");
        }
        undervoltageProtection = true;
        //disable this interrupt to avoid hysteresis
        MAP_GPIO_disableInterrupt(GPIO_PORT_P2,GPIO_PIN7);

    }

}

PowerBusHandler::PowerBusHandler()
{
    _powerBusStub = this;
    // always start with the busses in protection
    undervoltageProtection = true;

    MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P8, GPIO_PIN0 );
    MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P8, GPIO_PIN1 );
    MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P8, GPIO_PIN2 );
    MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P8, GPIO_PIN3 );

    MAP_GPIO_setAsOutputPin( GPIO_PORT_P8, GPIO_PIN0 );
    MAP_GPIO_setAsOutputPin( GPIO_PORT_P8, GPIO_PIN1 );
    MAP_GPIO_setAsOutputPin( GPIO_PORT_P8, GPIO_PIN2 );
    MAP_GPIO_setAsOutputPin( GPIO_PORT_P8, GPIO_PIN3 );

    // initialize the bus status bits
    MAP_GPIO_setAsInputPin( GPIO_PORT_P3, GPIO_PIN0 );
    MAP_GPIO_setAsInputPin( GPIO_PORT_P3, GPIO_PIN1 );
    MAP_GPIO_setAsInputPin( GPIO_PORT_P3, GPIO_PIN2 );
    MAP_GPIO_setAsInputPin( GPIO_PORT_P3, GPIO_PIN3 );

    //enable unregulated INA interrupt for undervoltage detection
    //enable interrupt for P2.7
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P2, GPIO_PIN7);
    MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P2, GPIO_PIN7, GPIO_HIGH_TO_LOW_TRANSITION);
    MAP_GPIO_registerInterrupt(GPIO_PORT_P2,[](){_powerBusStub->underVoltageTrip();});

}

void PowerBusHandler::checkBussesStatus( EPSTelemetryContainer *tc )
{
    // check if Unregulated Bus voltage is below threshold
    // if so, switch all buses off

    int curBatteryVoltage = 0;

    if(tc->getBatteryINAStatus()){
        curBatteryVoltage = tc->getBatteryINAVoltage();
        Console::log("PowerBusHandler: INA VOLTAGE %d mV",  curBatteryVoltage);
    }else if(tc->getBatteryGGStatus()){
        curBatteryVoltage = tc->getBatteryGGVoltage();
        Console::log("PowerBusHandler: GG VOLTAGE %d mV",  curBatteryVoltage);
    }else{
        //both sensors are dead. Just set voltage high enough to turn on the bus.
        curBatteryVoltage = BATTERY_HIGH_THRESHOLD;
        Console::log("PowerBusHandler: NO SENSOR, ASSUMING: %d mV",  curBatteryVoltage);
    }

    if (curBatteryVoltage < BATTERY_LOW_THRESHOLD)
    {
        MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P8, GPIO_PIN0 );
        MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P8, GPIO_PIN1 );
        MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P8, GPIO_PIN2 );
        MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P8, GPIO_PIN3 );

        if ( !undervoltageProtection )
        {
            Console::log("PowerBusHandler: Under-voltage protection ON");
        }
        undervoltageProtection = true;
    }
    else
    {
        if ((undervoltageProtection) && (curBatteryVoltage >= BATTERY_HIGH_THRESHOLD))
        {
            Console::log("PowerBusHandler: Under-voltage protection OFF");
            undervoltageProtection = false;
            //turn undervoltage trip detection back on.
            MAP_GPIO_clearInterruptFlag(GPIO_PORT_P2, PIN_ALL16);
            MAP_GPIO_enableInterrupt(GPIO_PORT_P2,GPIO_PIN7);

            if (DEFAULT & BUS1)
            {
                MAP_GPIO_setOutputHighOnPin( GPIO_PORT_P8, GPIO_PIN0 );
            }
            else
            {
                MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P8, GPIO_PIN0 );
            }
            if (DEFAULT & BUS2)
            {
                MAP_GPIO_setOutputHighOnPin( GPIO_PORT_P8, GPIO_PIN1 );
            }
            else
            {
                MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P8, GPIO_PIN1 );
            }
            if (DEFAULT & BUS3)
            {
                MAP_GPIO_setOutputHighOnPin( GPIO_PORT_P8, GPIO_PIN2 );
            }
            else
            {
                MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P8, GPIO_PIN2 );
            }
            if (DEFAULT & BUS4)
            {
                MAP_GPIO_setOutputHighOnPin( GPIO_PORT_P8, GPIO_PIN3 );
            }
            else
            {
                MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P8, GPIO_PIN3 );
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
            case 1:
            if (status)
            {
                MAP_GPIO_setOutputHighOnPin( GPIO_PORT_P8, GPIO_PIN0 );
            }
            else
            {
                MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P8, GPIO_PIN0 );
            }
            break;

            case 2:
            if (status)
            {
                MAP_GPIO_setOutputHighOnPin( GPIO_PORT_P8, GPIO_PIN1 );
            }
            else
            {
                MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P8, GPIO_PIN1 );
            }
            break;

            case 3:
            if (status)
            {
                MAP_GPIO_setOutputHighOnPin( GPIO_PORT_P8, GPIO_PIN2 );
            }
            else
            {
                MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P8, GPIO_PIN2 );
            }
            break;

            case 4:
            if (status)
            {
                MAP_GPIO_setOutputHighOnPin( GPIO_PORT_P8, GPIO_PIN3 );
            }
            else
            {
                MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P8, GPIO_PIN3 );
            }
            break;
        }
    }
    else
    {
        MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P8, GPIO_PIN0 );
        MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P8, GPIO_PIN1 );
        MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P8, GPIO_PIN2 );
        MAP_GPIO_setOutputLowOnPin( GPIO_PORT_P8, GPIO_PIN3 );
    }
}

unsigned char PowerBusHandler::getStatus( void )
{
    return ((MAP_GPIO_getInputPinValue( GPIO_PORT_P8, GPIO_PIN0 ) == GPIO_INPUT_PIN_HIGH) |
           ((MAP_GPIO_getInputPinValue( GPIO_PORT_P8, GPIO_PIN1 ) == GPIO_INPUT_PIN_HIGH) << 1) |
           ((MAP_GPIO_getInputPinValue( GPIO_PORT_P8, GPIO_PIN2 ) == GPIO_INPUT_PIN_HIGH) << 2) |
           ((MAP_GPIO_getInputPinValue( GPIO_PORT_P8, GPIO_PIN3 ) == GPIO_INPUT_PIN_HIGH) << 3));
}

unsigned char PowerBusHandler::getErrorStatus( void )
{
    unsigned char tmp = (((MAP_GPIO_getInputPinValue( GPIO_PORT_P3, GPIO_PIN1 ) == GPIO_INPUT_PIN_HIGH) << 1) |
           ((MAP_GPIO_getInputPinValue( GPIO_PORT_P3, GPIO_PIN2 ) == GPIO_INPUT_PIN_HIGH) << 2) |
           ((MAP_GPIO_getInputPinValue( GPIO_PORT_P3, GPIO_PIN3 ) == GPIO_INPUT_PIN_HIGH) << 3));

   if (MAP_GPIO_getInputPinValue( GPIO_PORT_P8, GPIO_PIN0 ) == GPIO_INPUT_PIN_HIGH)
   {
       // if BUS 1 is ON, read the status bit
       tmp |= (MAP_GPIO_getInputPinValue( GPIO_PORT_P3, GPIO_PIN0 ) == GPIO_INPUT_PIN_HIGH);
   }
   else
   {
       // otherwise, if OFF report there is no error
       tmp |= 1;
   }

    return tmp;
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
            if(command.getPayloadSize() == 1 && command.getDataPayload()[0] == 5){

                Console::log("Toggle BAT_RST ON!");
                this->setPostFunc([](){MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN6);
                    MAP_GPIO_setAsOutputPin( GPIO_PORT_P8, GPIO_PIN6 );});
                workingBuffer.setPayloadSize(1);
                workingBuffer.getDataPayload()[0] = command.getDataPayload()[0];
            }else{
                Console::log("Uknown command / Wrong command length");
                // unknown request
                workingBuffer.setPayloadSize(1);
                workingBuffer.getDataPayload()[0] = COMMAND_ERROR_PBUS;
            }
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
