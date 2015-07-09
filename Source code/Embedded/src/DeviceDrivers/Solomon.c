/*
 * Solomon.c
 *
 * Created: 9/7/2013 11:34:25 PM
 *  Author: Sensics
 */

// driver for Solomon MIPI bridge chip

#include <asf.h>
#include "Solomon.h"
#include "board.h"
#include "spi_master.h"
#include "my_hardware.h"
#include "nxp\my_bit.h"
#include "delay.h"
#include "Console.h"

#define Solomon_CLOCK_SPEED 1000000 // todo: consider increasing

// useful registers
#define Solomon_LRR			0xd4 //  SPI read register - default value is 0xFA
#define Solomon_LRR_code	0xFA // default value of LRR
#define Solomon_DIR			0xb0 // device ID register - should return value of 0x2828

struct spi_device devices[2];

SPI_t *spi[2];

bool Display1On=false;
bool Display2On=false;

// uint16_t Solomon_CSN[2]; todo: can we remove?

bool init_solomon_spi(uint8_t deviceID);
void select_solomon(uint8_t channel);
void deselect_solomon(void);

bool init_solomon_spi(uint8_t deviceID)

{
    spi_flags_t spi_flags = SPI_MODE_0;

    spi_master_init(spi[deviceID]);
    spi_master_setup_device(spi[deviceID], &devices[deviceID], spi_flags,
                            Solomon_CLOCK_SPEED, devices[deviceID].id);
    spi_enable(spi[deviceID]); // todo: is this necessary?
    return true;
}

void powercycle_display(uint8_t deviceID)

// power cycles display connected to the specific device

{

    write_solomon(deviceID,0xBF,0x0028); // display on
    delay_ms(120);
    write_solomon(deviceID,0xBF,0x0010); // sleep out

    delay_ms(1000);


    write_solomon(deviceID,0xBF,0x0029); // display on
    delay_ms(120);
    write_solomon(deviceID,0xBF,0x0011); // sleep out
}

bool SolomonInitialized=false;

bool init_solomon_device(uint8_t deviceID)

{
	//if (SolomonInitialized==true)
		//return true;
	delay_ms(500);
	SolomonInitialized=true;
	
    // check if Solomon returns ID. If not, can't initialize
    if (read_Solomon_ID(deviceID)!=0x2828)
    {
        WriteLn("Solomon init failed");
        return false;
    }

    // ssd 2828 initialize

    write_solomon(deviceID,0xB1,0x0216); // VSA=2, HSA=22

#ifndef H546DLT01
    write_solomon(deviceID,0xB2,0x0630); // VBP=4, HBP=26
    //write_solomon(deviceID,0xB2,0x0100); // VBP=4, HBP=26

    write_solomon(deviceID,0xB3,0x288C); // VFP=40, HFP=140
    //write_solomon(deviceID,0xB3,0x0100); // VFP=40, HFP=140
#else
    write_solomon(deviceID,0xB2,0x069c); // VBP=4, HBP=156
    //write_solomon(deviceID,0xB2,0x0100); // VBP=4, HBP=26

    write_solomon(deviceID,0xB3,0x2820); // VFP=40, HFP=32
    //write_solomon(deviceID,0xB3,0x0100); // VFP=40, HFP=140
#endif

    write_solomon(deviceID,0xB4,0x0438); // HACT=1080
    write_solomon(deviceID,0xB5,0x0780); // VACT=1920

    write_solomon(deviceID,0xB6,0x2007); // non burst with sync events, 24bpp

    write_solomon(deviceID,0xC9,0x140A); // HS prepare delay
    write_solomon(deviceID,0xDE,0x0003); // no of lane
    write_solomon(deviceID,0xD6,0x0004); // packet number in blanking period
    write_solomon(deviceID,0xBA,0xC030); // lane speed=960Mbps
    write_solomon(deviceID,0xBB,0x0008); // LP clock
    write_solomon(deviceID,0xB9,0x0001); // enable PLL
    write_solomon(deviceID,0xC4,0x0001); // auto BTA

    delay_ms(50);
    // module panel initialization
    write_solomon(deviceID,0xB7,0x0302); // LP generic write
    write_solomon(deviceID,0xB8,0x0000); // VC

#ifdef LS055T1SX01 // sharp 5.5"
    write_solomon(deviceID,0xBC,0x0003); // no of bytes to send
    write_solomon_pair(deviceID,0xBF,0x0F51,0x00FF); // cmd=51, data1=0F

    write_solomon(deviceID,0xBC,0x0002); // no of byte send
    write_solomon(deviceID,0xBF,0x0453); // cmd=53, data=04
    write_solomon(deviceID,0xBF,0x0055); // cmd=55, data=00

    write_solomon(deviceID,0xB7,0x0342); // LP DCS write
    write_solomon(deviceID,0xB8,0x0000); // VC


    write_solomon(deviceID,0xBC,0x0001); //
    write_solomon(deviceID,0xBF,0x0029); // display on
    delay_ms(120);
    write_solomon(deviceID,0xBF,0x0011); // sleep out
#endif

#ifdef LS050T1SX01 // sharp 5"
    // from LS050T1SX01 data sheet
    write_solomon(deviceID,0xBC,0x0002); // no of byte send
    write_solomon(deviceID,0xBF,0x04B0); // cmd=B0, data=04
    write_solomon(deviceID,0xBF,0x01D6); // cmd=D6, data=01
    write_solomon(deviceID,0xB7,0x0342); // LP DCS write
    write_solomon(deviceID,0xB8,0x0000); // VC
    write_solomon(deviceID,0xBC,0x0003); // no of byte send

    write_solomon_pair(deviceID,0xBF,0x0F51,0x00FF); // cmd=51, data1=0F

    write_solomon(deviceID,0xBC,0x0002); //
    //write_solomon(deviceID,0xBB,0x0008); // LP clock BC 0002
    write_solomon(deviceID,0xBF,0x0453); // cmd=53, data=04
    write_solomon(deviceID,0xBC,0x0001); //
    write_solomon(deviceID,0xBF,0x0029); // display on
    delay_ms(120);
    write_solomon(deviceID,0xBF,0x0011); // sleep out

    // end of LS050T1SX01 data sheet
#endif

#ifdef H546DLT01 // AUO 5.46" OLED
    // from LS050T1SX01 data sheet
    write_solomon(deviceID,0xBC,0x0002); // no of byte send
	delay_ms(16);
    write_solomon(deviceID,0xBF,0x04B0); // cmd=B0, data=04
	delay_ms(16);
    write_solomon(deviceID,0xBF,0x01D6); // cmd=D6, data=01
	delay_ms(16);
    write_solomon(deviceID,0xB7,0x0341); // LP DCS write
	delay_ms(16);
    write_solomon(deviceID,0xB8,0x0000); // VC
	delay_ms(16);

    write_solomon(deviceID,0xBC,0x0002); // number of bytes to write
	delay_ms(100);

#ifdef LOW_PERSISTENCE
    write_solomon(deviceID,0xBF,0x08FE); // cmd=FE, data=08
    delay_ms(16);
    write_solomon(deviceID,0xBF,0x4003); // cmd=FE, data=08
    delay_ms(16);
    write_solomon(deviceID,0xBF,0x1A07); // cmd=FE, data=08
    delay_ms(16);
    write_solomon(deviceID,0xBF,0x00FE); // cmd=FE, data=08
    delay_ms(16);
    write_solomon(deviceID,0xBF,0xFF51); // cmd=FE, data=08
    delay_ms(16);
#else
    write_solomon(deviceID,0xBF,0x04FE); // cmd=FE, data=04
	delay_ms(16);
    write_solomon(deviceID,0xBF,0x005E);
	delay_ms(16);
    write_solomon(deviceID,0xBF,0x4744);
	delay_ms(16);
    write_solomon(deviceID,0xBF,0x07FE);
	delay_ms(16);
    write_solomon(deviceID,0xBF,0x6AA9);
	delay_ms(16);
    write_solomon(deviceID,0xBF,0x0AFE);
	delay_ms(16);
    write_solomon(deviceID,0xBF,0x5214);
	delay_ms(16);
    write_solomon(deviceID,0xBF,0x00FE);
	delay_ms(16);
    write_solomon(deviceID,0xBF,0x0135);
	delay_ms(16);
    write_solomon(deviceID,0xBF,0x0055);
	delay_ms(16);
#endif
	/*
    write_solomon(deviceID,0xBC,0x0001); //
	delay_ms(16);
    write_solomon(deviceID,0xBF,0x0011); // sleep out
    delay_ms(170); // delay > 10 frames
    write_solomon(deviceID,0xBF,0x0029); // display on
    delay_ms(20); // delay > 1 frames

    write_solomon(deviceID,0xBC,0x0002); // number of bytes to write

    write_solomon(deviceID,0xBF,0x07FE); // cmd=FE, data=BF
    delay_ms(100); // delay > 5 frames
    write_solomon(deviceID,0xBF,0xFAA9);
	delay_ms(16);
    write_solomon(deviceID,0xBF,0x00FE);
	delay_ms(16);

    write_solomon(deviceID,0xBC,0x0001); //
	delay_ms(16);*/

#endif

#ifndef H546DLT01 // AUO 5.46" OLED
    // video mode on
	delay_ms(250);
    write_solomon(deviceID,0xB7,0x034B); // video mode on
	delay_ms(100);
#endif
	
    return true;
}

void DisplayOn(uint8_t deviceID)

{
#if 0
	if (deviceID==Solomon1)
	{
		if (Display1On)
			return;
		else
			Display1On=true;
	}
	#ifdef Solomon2
	else if (deviceID==Solomon2)
	{
		if (Display2On)
		return;
		else
		Display2On=true;
	}
	#endif
#endif

#ifdef H546DLT01 // AUO 5.46" OLED
		//delay_ms(500);
		WriteLn("Turning display on");
				
		//display power on
		delay_ms(20);
		//active display reset (pull RESX up)
		//ioport_set_pin_high(LCD_avdd_en);
		//delay_ms(20);
				
		
		//initial setting
		write_solomon(deviceID,0xBC,0x0002); // no of byte send
		write_solomon(deviceID,0xBF,0x04B0); // cmd=B0, data=04
		write_solomon(deviceID,0xBF,0x01D6); // cmd=D6, data=01
		write_solomon(deviceID,0xB7,0x0341); // LP DCS write
		write_solomon(deviceID,0xB8,0x0000); // VC
		delay_ms(16);

		//write_solomon(deviceID,0xBC,0x0035); //set TE output
		write_solomon(deviceID,0xBF,0x0011); // sleep out
		delay_ms(160);    //>10 frame
		write_solomon(deviceID,0xBF,0x0029); // display on
		delay_ms(16); //> 1 frame

		
		//EM signal on
		write_solomon(deviceID,0xBC,0x0002); // number of bytes to write
		delay_ms(16);
		write_solomon(deviceID,0xBF,0x07FE);
		delay_ms(80); //> 5 frame
		write_solomon(deviceID,0xBF,0xEAA9);
		delay_ms(16);
		write_solomon(deviceID,0xBF,0x00FE);

		
		write_solomon(deviceID,0xBF,0x0011); // sleep out
		delay_ms(160);
		write_solomon(deviceID,0xBF,0x0029); // display on
		delay_ms(16); //> 1 frame
		write_solomon(deviceID,0xBF,0x07FE);
		delay_ms(80); //> 5 frame
		write_solomon(deviceID,0xBF,0xFAA9);
		delay_ms(16);

		//auo setting
		write_solomon(deviceID,0xBF,0x08FE);
		write_solomon(deviceID,0xBF,0x4003);
		write_solomon(deviceID,0xBF,0x1A07);
		write_solomon(deviceID,0xBF,0x00FE);
		write_solomon(deviceID,0xBF,0xFF51);
		write_solomon(deviceID,0xBC,0x0001); //
		delay_ms(16);

	    write_solomon(deviceID,0xBC,0x0001); //
	    delay_ms(32);
		write_solomon(deviceID,0xB7,0x034B); // video mode on

#endif

		//ioport_set_pin_high(FPGA_Reset_Pin);	// release FPGA reset


}

void DisplayOff(uint8_t deviceID)

{
#if 0
		if (deviceID==Solomon1)
		{
			if (!Display1On)
				return;
			else
				Display1On=false;
		}
		#ifdef Solomon2
		else if (deviceID==Solomon2)
		{
			if (!Display2On)
				return;
			else
				Display2On=false;
		}
		#endif
#endif
#ifdef H546DLT01 // AUO 5.46" OLED

        WriteLn("Turing display off");

        write_solomon(deviceID,0xB7,0x0341); // video mode off
        delay_ms(16);
        write_solomon(deviceID,0xBC,0x0001); //

        delay_ms(16);

        write_solomon(deviceID,0xBF,0x0028); // display off
        delay_ms(16); // delay > 10 frames

		//EM signal off
		write_solomon(deviceID,0xBC,0x0002); // number of bytes to write
		delay_ms(16);
		write_solomon(deviceID,0xBF,0x07FE);
		delay_ms(80); //> 5 frame
		write_solomon(deviceID,0xBF,0x6AA9);
		delay_ms(16);
		write_solomon(deviceID,0xBF,0x00FE);

        write_solomon(deviceID,0xBF,0x0010); // sleep in

        delay_ms(20); // delay > 1 frames
		 
		//Solomon_Reset(1);
		//delay_ms(16);
 		//init_solomon_device(Solomon1);
		 
		//ioport_set_pin_low(LCD_avdd_en);	//RESX need jump wired
      
#endif
}
void init_solomon(void)

{

#ifdef Solomon1_SPI
    devices[Solomon1].id = Solomon1_CSN;
    devices[Solomon1].sdc = Solomon1_AddrData;
    spi[Solomon1]=&Solomon1_SPI;
#endif

#ifdef Solomon2_SPI
    devices[Solomon2].id = Solomon1_CSN;
    devices[Solomon2].sdc= Solomon2_AddrData;
    spi[Solomon2]=&Solomon1_SPI;
#endif

    //Solomon_CSN[Solomon1]=Solomon1_CSN; // todo: can we remove?
    //Solomon_CSN[Solomon2]=Solomon2_CSN;

    init_solomon_spi(Solomon1); // no need to do repeat for Solomon2 because both share the same SPI port

#ifdef Solomon1_SPI
    init_solomon_device(Solomon1); // todo: add back after debug of board
#endif

#ifdef Solomon2_SPI
    init_solomon_device(Solomon2);
#endif
}

/* write data to solomon
	channel: solomon1 or solomon2
	address: address to read
	returns 16-bit data
*/

void select_solomon(uint8_t channel)

// enable the mux and select the correct output

{
#ifdef OSVRHDK
    return;
#else
    ioport_set_pin_low(SPI_Mux_OE);
    if (channel==Solomon1)
        ioport_set_pin_low(SPI_Mux_Select);
    else
        ioport_set_pin_high(SPI_Mux_Select);
#endif
}

void deselect_solomon(void)

// deselect the mux

{
    // ioport_set_pin_high(SPI_Mux_OE); // leave permanently low per Zeev
}

uint16_t read_solomon(uint8_t channel, uint8_t address)

{

    uint8_t dataL,dataH;
    uint16_t data;

    select_solomon(channel);

    spi_select_device(spi[channel],&devices[channel]);

    ioport_set_pin_low(devices[channel].sdc); // lower sdc bit because this is command
    spi_write_single(spi[channel],address);
    while (!spi_is_rx_full(spi[channel])) {
    }
    spi_write_single(spi[channel],Solomon_LRR_code); // Component switches to read mode if this byte equals the contents of register LRR
    while (!spi_is_rx_full(spi[channel])) {
    }
    ioport_set_pin_high(devices[channel].sdc); // raise sdc bit because here comes the data
    spi_write_single(spi[channel],CONFIG_SPI_MASTER_DUMMY);
    while (!spi_is_rx_full(spi[channel])) {
    }
    spi_read_single(spi[channel],&dataL);
    spi_write_single(spi[channel],CONFIG_SPI_MASTER_DUMMY);
    while (!spi_is_rx_full(spi[channel])) {
    }
    spi_read_single(spi[channel],&dataH);
    spi_deselect_device(spi[channel],&devices[channel]);
    data=dataH;
    data=(data<<8)+dataL;

    deselect_solomon();
    return (data);
};

/* write data to solomon
	channel: solomon1 or solomon2
	address: address to write
	data: data to write
*/

void write_solomon(uint8_t channel, uint8_t address, uint16_t data)

{
    select_solomon(channel);

    spi_select_device(spi[channel],&devices[channel]);
    ioport_set_pin_low(devices[channel].sdc); // lower sdc bit because this is command
    spi_write_single(spi[channel],address);
    while (!spi_is_rx_full(spi[channel])) {
    }
    ioport_set_pin_high(devices[channel].sdc); // raise sdc bit because here comes the data
    spi_write_single(spi[channel],lo(data)); // low byte first. by default, MSB of each byte is sent first because DORD=0
    while (!spi_is_rx_full(spi[channel])) {
    }
    spi_write_single(spi[channel],hi(data));
    while (!spi_is_rx_full(spi[channel])) {
    }
    spi_deselect_device(spi[channel],&devices[channel]);

    deselect_solomon();
};

// writes a pair of data points
void write_solomon_pair(uint8_t channel, uint8_t address, uint16_t data1, uint16_t data2)

{
    select_solomon(channel);

    spi_select_device(spi[channel],&devices[channel]);
    ioport_set_pin_low(devices[channel].sdc); // lower sdc bit because this is command
    spi_write_single(spi[channel],address);
    while (!spi_is_rx_full(spi[channel])) {
    }
    ioport_set_pin_high(devices[channel].sdc); // raise sdc bit because here comes the data

    spi_write_single(spi[channel],lo(data1)); // low byte first. by default, MSB of each byte is sent first because DORD=0
    while (!spi_is_rx_full(spi[channel])) {
    }
    spi_write_single(spi[channel],hi(data1));
    while (!spi_is_rx_full(spi[channel])) {
    }

    spi_write_single(spi[channel],lo(data2)); // low byte first. by default, MSB of each byte is sent first because DORD=0
    while (!spi_is_rx_full(spi[channel])) {
    }
    spi_write_single(spi[channel],hi(data2));
    while (!spi_is_rx_full(spi[channel])) {
    }

    spi_deselect_device(spi[channel],&devices[channel]);

    deselect_solomon();
};

// read the solomon ID
uint16_t read_Solomon_ID(uint8_t channel)

{
    write_solomon(channel,0xd4,0xfa); // about to read dir register
    return(read_solomon(channel, Solomon_DIR));
};

void raise_sdc(uint8_t channel)
{
    ioport_set_pin_high(devices[channel].sdc); // // raise sdc bit because here comes the data
}

void lower_sdc(uint8_t channel)
{
    ioport_set_pin_low(devices[channel].sdc); // lower sdc bit because this is command
}

void Solomon_Reset(uint8_t SolomonNum)

{
    if (SolomonNum==1)
    {
        WriteLn("reset Sol1");
        ioport_set_pin_low(Solomon1_Reset);
        delay_ms(10);
        ioport_set_pin_high(Solomon1_Reset);
    }
    else if (SolomonNum==2)
    {
        WriteLn("reset Sol2");
        ioport_set_pin_low(Solomon2_Reset);
        delay_ms(1);
        ioport_set_pin_high(Solomon2_Reset);
    }
    else
        WriteLn("Wrong Sol num");
}