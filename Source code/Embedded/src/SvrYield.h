/*
 * SvrYield.h
 * Replacement for delay.h that provides delays that correctly service other peripherals during the delay time as
 * required.
 * Created: 7/21/2016 8:40:01 AM
 *  Author: Sensics
 */

#ifndef SVRYIELD_H_
#define SVRYIELD_H_

#include <stdint.h>

/// Allow any regular tasks to run.
void svr_yield(void);

/// Wait at least ms milliseconds, allowing regular tasks to run.
void svr_yield_ms(uint16_t ms);

#endif /* SVRYIELD_H_ */