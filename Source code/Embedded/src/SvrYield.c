/*
 * SvrYield.c
 *
 * Created: 7/21/2016 8:41:29 AM
 *  Author: Sensics
 */

#include "SvrYield.h"
#include "GlobalOptions.h"
#ifdef BNO070
#include "DeviceDrivers/BNO070.h"
#endif

#include <delay.h>

void svr_yield_ms(uint16_t ms)
// delays for specified number of ms, while yielding to BNO
{
	while (ms > 0)
	{
		delay_ms(1);
#ifdef BNO070
		/// @todo account for the time taken in the BNO_Yield in the overall delay time.
		BNO_Yield();
#endif
		ms--;
	}
}