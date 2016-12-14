/*
 * TimingDebug.c
 *
 * Created: 10/15/2015 12:00:52 PM
 *  Author: YSB
 */

#include <asf.h>
#include "GlobalOptions.h"
#include "TimingDebug.h"
#include "Console.h"
#include <stdio.h>

#if defined(MeasurePerformance) || defined (HDK_20)

bool CollectData = true;
uint8_t TimeLogIndex = 0;
uint16_t TimeLog1[256];
uint16_t TimeLog2[256];
uint16_t TimeLog3[256];
uint16_t TimingCounter[256];
uint32_t tick1, tick2, tick3;

#define TIMER_EXAMPLE TCC2
#define TIMER_EXAMPLE_PERIOD 31250

void TimingDebug_init(void)

{
	// rtc_init();
	// rtc_set_time(0);
	tc_enable(&TIMER_EXAMPLE);
	tc_set_wgm(&TIMER_EXAMPLE, TC_WG_NORMAL);
	// tc_write_period(&TIMER_EXAMPLE, TIMER_EXAMPLE_PERIOD);
	tc_write_period(&TIMER_EXAMPLE, 65535);
	// tc_set_resolution(&TIMER_EXAMPLE, TIMER_EXAMPLE_PERIOD);
	tc_set_resolution(&TIMER_EXAMPLE, 1000000);
	tick1 = 0;
	tick2 = 0;
	tick3 = 0;
};

void TimingDebug_enable(void)  // enable data collection
{
	CollectData = true;
	tick1 = 0;
	tick2 = 0;
	tick3 = 0;
}

void TimingDebug_disable(void)  // disable data collection
{
	CollectData = false;
}

void TimingDebug_event1(void)  // indicates when event 1 in a sequence of up to 3 happened
{
	if (CollectData)
	{
		TimeLogIndex = (TimeLogIndex + 1) & 0xFF;
		// TimeLogIndex=BNOSequenceNumber;
		TimeLog1[TimeLogIndex] = tc_read_count(&TIMER_EXAMPLE);
		TimeLog2[TimeLogIndex] = 0;
		TimeLog3[TimeLogIndex] = 0;
		TimingCounter[TimeLogIndex] = 0;
		tick1++;
	}
};

void TimingDebug_event2(void)  // indicates when event 2 in a sequence of up to 3 happened
{
	if (CollectData)
	{
		TimeLog2[TimeLogIndex] = tc_read_count(&TIMER_EXAMPLE);
		tick2++;
	}
};

void TimingDebug_event3(void)  // indicates when event 3 in a sequence of up to 3 happened
{
	if (CollectData)
	{
		TimeLog3[TimeLogIndex] = tc_read_count(&TIMER_EXAMPLE);
		tick3++;
	}
};

void TimingDebug_RecordEventType(uint8_t Event) { TimingCounter[TimeLogIndex] = Event; }
void TimingDebug_output(void)  // outputs results of timing collection
{
	char msg[30];

	TimingDebug_disable();
	for (int i = 0; i < 50; i++)
	{
		sprintf(msg, "%d: %u %u %u %d", i, TimeLog1[i], TimeLog2[i], TimeLog3[i], TimingCounter[i]);
		WriteLn(msg);
	}
	sprintf(msg, "%ld %ld %ld", tick1, tick2, tick3);
	WriteLn(msg);
	TimingDebug_enable();
};

#endif
