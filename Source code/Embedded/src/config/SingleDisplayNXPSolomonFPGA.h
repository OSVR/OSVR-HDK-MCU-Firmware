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
#define SVR_HAVE_NXP 1
#define SVR_HAVE_SOLOMON 1
#define SVR_HAVE_FPGA
#define SVR_HAVE_SIDEBYSIDE

#define SVR_NUM_LOGICAL_DISPLAYS 1

#endif /* SINGLEDISPLAYNXPSOLOMONFPGA_H_ */