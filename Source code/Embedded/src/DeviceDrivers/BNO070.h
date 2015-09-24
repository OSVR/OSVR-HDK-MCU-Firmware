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

struct BNO070_Stats_s {
	uint32_t resets;
	uint32_t events;
	uint32_t empty_events;
};
typedef struct BNO070_Stats_s BNO070_Stats_t;

extern bool BNO070Active;
extern sensorhub_ProductID_t BNO070id;

bool init_BNO070(void);
void SimReset_BNO070(void);
bool Check_BNO070(void);
bool Tare_BNO070(void);
bool SetDcdEn_BNO070(uint8_t flags);
bool SaveDcd_BNO070(void);
bool ClearDcd_BNO070(void);
bool MagSetEnable_BNO070(bool enabled);
uint8_t MagStatus_BNO070(void);  // 0 - Unreliable, 1 - Low, 2 - Medium, 3 - High Accuracy.
void GetStats_BNO070(BNO070_Stats_t *stats);
void SetDebugPrintEvents_BNO070(bool);
bool ReInit_BNO070(void);
bool Reset_BNO070(void);
bool dfu_BNO070(void);

#endif /* BNO070_H_ */
