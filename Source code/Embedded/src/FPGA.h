/*
 * FPGA.h
 *
 * Created: 9/1/2013 10:29:39 PM
 *  Author: Sensics
 */

#ifndef FPGA_H_
#define FPGA_H_

#include <stdint.h>

extern bool SideBySideMode;

/// Start reset of FPGA.
void FPGA_start_reset(void);
/// End reset of FPGA
void FPGA_end_reset(void);
void FPGA_reset(void);

#if 0
void FPGA_write(uint8_t FPGANun, uint8_t Command, uint8_t Value);
uint8_t FPGA_read(uint8_t FPGANun, uint8_t Command);
#endif

#endif /* FPGA_H_ */