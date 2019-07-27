/*
 * EPS.h
 *
 *  Created on: 23 Jul 2019
 *      Author: stefanosperett
 */

#ifndef EPS_H_
#define EPS_H_

#include <driverlib.h>
#include "msp.h"
#include "PQ9Bus.h"
#include "PQ9Frame.h"
#include "DWire.h"
#include "INA226.h"
#include "DSerial.h"
#include "PQ9CommandHandler.h"
#include "Service.h"
#include "PingService.h"
#include "ResetService.h"
#include "SoftwareUpdateService.h"

#define FCLOCK 48000000

#endif /* EPS_H_ */
