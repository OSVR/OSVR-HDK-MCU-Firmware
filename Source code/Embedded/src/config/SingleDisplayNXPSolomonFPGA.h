/*
 * SingleDisplayNXPSolomonFPGA.h
 *
 * Created: 7/19/2016 10:12:25 AM
 *  Author: Sensics
 */

#ifndef SINGLEDISPLAYNXPSOLOMONFPGA_H_
#define SINGLEDISPLAYNXPSOLOMONFPGA_H_

// Shared configuration options for the family of devices
// with a single display, using a Solomon MIPI bridge, NXP receiver,
// and FPGA.

// Interval in number of mainloop cycles that should elapse between polling for video status.
#define SVR_VIDEO_INPUT_POLL_INTERVAL 1000

// Single NXP TDA-19971 HDMI receiver
#define SVR_HAVE_NXP 1

// Single Solomon SSD2828 MIPI bridge chip
#define SVR_HAVE_SOLOMON 1

// Video-processing FPGA in the signal path between HDMI receiver and MIPI bridge
#define SVR_HAVE_FPGA

// Input pin from FPGA indicating that it has a lock on a video signal
#define SVR_HAVE_FPGA_VIDEO_LOCK_PIN

// Side-by-side video transform functionality provided by FPGA
#define SVR_HAVE_SIDEBYSIDE

// Single display to control
#define SVR_NUM_LOGICAL_DISPLAYS 1

#endif /* SINGLEDISPLAYNXPSOLOMONFPGA_H_ */