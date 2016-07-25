/*
 * SvrYield.c
 *
 * Created: 7/21/2016 8:41:29 AM
 *  Author: Sensics
 */

#include "SvrYield.h"

// Options header
#include "GlobalOptions.h"

// Application headers
#ifdef BNO070
#include "DeviceDrivers/BNO070.h"
#endif

// asf header
#include <delay.h>

/// Body that should run during a yield.
__attribute__((always_inline)) static void svr_yield_impl(void);
__attribute__((always_inline)) inline static void svr_yield_impl()
{
#ifdef BNO070
	/// @todo account for the time taken in the BNO_Yield in the overall delay time, when we are given a delay time - or
	/// switch to an RTOS.
	BNO_Yield();
#endif
}

void svr_yield(void) { svr_yield_impl(); }
void svr_yield_ms(uint16_t ms)
// delays for specified number of ms, while yielding to BNO
{
	while (ms > 0)
	{
		delay_ms(1);
		svr_yield_impl();
		ms--;
	}
}