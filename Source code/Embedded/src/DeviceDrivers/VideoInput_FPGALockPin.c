/*
 * VideoInput_FPGALockPin.c
 *
 * Created: 7/20/2016 10:49:30 AM
 *  Author: Sensics
 */

#include "GlobalOptions.h"

#ifdef SVR_HAVE_FPGA_VIDEO_LOCK_PIN

#include "VideoInput.h"
#include "VideoInput_Protected.h"
#include "my_hardware.h"
#include <ioport.h>

bool VideoInput_Poll_Status()
{
	bool newStatus = !ioport_get_pin_level(FPGA_unlocked);
	VideoInput_Protected_Report_Status(newStatus);
	return newStatus;
}
#endif  // SVR_HAVE_FPGA_VIDEO_LOCK_PIN