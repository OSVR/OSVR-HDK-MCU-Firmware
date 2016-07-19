/*
 * NXP_AVR_Internal.h
 *
 * Created: 7/19/2016 1:32:27 PM
 *  Author: Sensics
 */ 


#ifndef NXP_AVR_INTERNAL_H_
#define NXP_AVR_INTERNAL_H_


#include <stdint.h>

/// Call for use internal to the NXP code.
void NXP_Private_PRINTIF(uint16_t errCode, uint16_t lineNumber);

#endif /* NXP_AVR_INTERNAL_H_ */