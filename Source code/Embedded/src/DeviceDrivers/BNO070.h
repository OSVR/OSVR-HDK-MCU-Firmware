/*
 * BNO070.h
 *
 * Created: 10/30/2014 3:04:28 PM
 *  Author: Sensics Boger
 */


#ifndef BNO070_H_
#define BNO070_H_

#include "GlobalOptions.h"
#include "DeviceDrivers/bno-hostif/src/sensorhub.h"

extern bool BNO070Active;
extern sensorhub_ProductID_t BNO070id;

bool init_BNO070(void);
bool Check_BNO070(void);
bool Tare_BNO070(void);

bool dfu_BNO070(void);


#endif /* BNO070_H_ */