/*
 * FPGA.h
 *
 * Created: 9/1/2013 10:29:39 PM
 *  Author: Sensics
 */

#ifndef FPGA_H_
#define FPGA_H_

extern bool SideBySideMode;

void FPGA_reset(void);
void FPGA_write(uint8_t FPGANun, uint8_t Command, uint8_t Value);
uint8_t FPGA_read(uint8_t FPGANun, uint8_t Command);

#endif /* FPGA_H_ */