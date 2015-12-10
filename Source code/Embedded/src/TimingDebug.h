/*
 * TimingDebug.h
 *
 * Created: 10/15/2015 11:58:35 AM
 *  Author: YSB
 */ 


// set of routines to help debug timing of time-sensitive tasks

#ifndef TIMINGDEBUG_H_
#define TIMINGDEBUG_H_

void TimingDebug_init(void);
void TimingDebug_enable(void); // enable data collection
void TimingDebug_disable(void); // disable data collection

void TimingDebug_event1(void); // indicates when event 1 in a sequence of up to 3 happened
void TimingDebug_event2(void); // indicates when event 2 in a sequence of up to 3 happened
void TimingDebug_event3(void); // indicates when event 3 in a sequence of up to 3 happened
void TimingDebug_RecordEventType(uint8_t Event);


void TimingDebug_output(void); // outputs results of timing collection


#endif /* TIMINGDEBUG_H_ */