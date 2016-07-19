/*
 * Solomon.h
 *
 * Created: 9/7/2013 11:35:02 PM
 *  Author: Sensics
 */


#ifndef SOLOMON_H_
#define SOLOMON_H_

#include "GlobalOptions.h"
#include <stdbool.h>

#ifdef SVR_HAVE_SOLOMON1
#define Solomon1	0
#define Solomon1_CSN            IOPORT_CREATE_PIN(PORTC, 4)
#define Solomon1_AddrData       IOPORT_CREATE_PIN(PORTB, 1)
#define Solomon1_VOUT_Shutdown	IOPORT_CREATE_PIN(PORTA,1) // when low, Solomon is activated
#define Solomon1_Reset			IOPORT_CREATE_PIN(PORTA,2)
#endif // SVR_HAVE_SOLOMON1

#ifdef SVR_HAVE_SOLOMON2
#define Solomon2	1
// todo: map to actual one and enable
//#define Solomon2_CSN            IOPORT_CREATE_PIN(PORTF, 3)
#define Solomon2_AddrData       IOPORT_CREATE_PIN(PORTB, 3) // todo: map to actual one and enable
#define Solomon2_VOUT_Shutdown	IOPORT_CREATE_PIN(PORTA,3)
#define Solomon2_Reset			IOPORT_CREATE_PIN(PORTA,4)
/* When Mux_OE is low, Mux_select low is Solomon1, Mux_select high is Solomon2 */
#define SPI_Mux_OE				IOPORT_CREATE_PIN(PORTC,2)
#define SPI_Mux_Select			IOPORT_CREATE_PIN(PORTC,3)
#endif // SVR_HAVE_SOLOMON2


void init_solomon(void);
bool init_solomon_device(uint8_t deviceID);
void DisplayOn(uint8_t deviceID);
void DisplayOff(uint8_t deviceID);

void powercycle_display(uint8_t deviceID);
uint16_t read_solomon(uint8_t channel, uint8_t address);
void write_solomon(uint8_t channel, uint8_t address, uint16_t data);
void write_solomon_pair(uint8_t channel, uint8_t address, uint16_t data1, uint16_t data2);
void raise_sdc(uint8_t channel);
void lower_sdc(uint8_t channel);
void Solomon_Reset(uint8_t SolomonNum);
void set_strobing(uint8_t deviceID, uint8_t refresh, uint8_t percentage);


// read the solomon ID
uint16_t read_Solomon_ID(uint8_t channel);

// strobing settings
extern uint8_t Strobing_rate;
extern uint8_t Strobing_percent;


#endif /* SOLOMON_H_ */

