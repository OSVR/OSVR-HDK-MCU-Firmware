/*
 * NBO070.c
 *
 * Created: 10/30/2014 3:04:17 PM
 *  Author: Sensics Boger
 */

#include <asf.h>
#include "string.h"

#include "GlobalOptions.h"
#include "my_hardware.h"
#include "Console.h"

#include <ioport.h>
#include "BNO070.h"
#include "util/delay.h"

#include "twi_master.h"

bool BNO070Active=false;

#ifdef BNO070

//#define USE_GAME_ROTATION // if defined, uses game rotation vector, otherwise regular rotation vector

bool TWI_BNO070_PORT_initialized=false; // true if already initialized
uint8_t BNO070_Report[USB_REPORT_SIZE];

twi_package_t packet_received = {
    .addr[0]      = 1, //regNum,      // TWI slave memory address data
    .addr[1]      = 0, //regNum,      // TWI slave memory address data
    .addr_length  = sizeof (uint8_t)*2,     // TWI slave memory address data size
    .chip         = BNO070_ADDR,      // TWI slave bus address
    //.buffer       = &BNO070_Status,        // transfer data destination buffer
    .length       = 30   // transfer data size (bytes)
};

bool BNO070_operation_done(bool StateToExitFrom);
bool BNO070_packet_write(uint8_t Addr, void *DataToWrite, unsigned int Datalength);

bool BNO070_operation_done(bool StateToExitFrom)
// waits for BNO to complete the operation, as signified by interrupt going high or low. Waits to interrupt to exit from "state to exit from" mode
// returns TRUE if finished (normal mode), FALSE if error

{
	uint8_t timeout=0;

	while (ioport_get_value(Int_BNO070)==StateToExitFrom) // wait for interrupt to go high
	{
		timeout++;
		if (timeout>100)
		return false; // exit with error if another 100 mSec passed without INT going high
		delay_ms(1);
	}
	return true;
};

bool BNO070_packet_write(uint8_t Addr, void *DataToWrite, unsigned int Datalength)

// writes packet to BNO

{
    twi_package_t packet_write = {
	    .addr[0]      = Addr, //regNum,      // TWI slave memory address data
	    .addr[1]      = 0, //regNum,      // TWI slave memory address data
	    .addr_length  = sizeof (uint16_t),    // TWI slave memory address data size
	    .chip         = BNO070_ADDR,      // TWI slave bus address
	    .buffer       = DataToWrite, // transfer data source buffer
	    .length       = Datalength  // transfer data size (bytes)
    };
    if (twi_master_write(TWI_BNO070_PORT, &packet_write)!=STATUS_OK)
	    return false;
	else
		return true;
}

bool init_BNO070(void)
{

    int result;

    uint8_t BNO070_Status[30];

    packet_received.buffer       = &BNO070_Status;        // transfer data destination buffer

    // do a dummy read to clear initial interrupt

    ioport_configure_pin(BNO_070_Reset_Pin,IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH); // reset is active low
    ioport_configure_pin(Int_BNO070, IOPORT_DIR_INPUT|IOPORT_MODE_PULLUP);


    if (!TWI_BNO070_PORT_initialized)
    {
        twi_master_options_t opt_BNO070;

        opt_BNO070.speed = BNO_TWI_SPEED;
        opt_BNO070.chip  = BNO070_ADDR;
        result=twi_master_setup(TWI_BNO070_PORT, &opt_BNO070);
        if (result==STATUS_OK)
        {
            TWI_BNO070_PORT_initialized=true;
            //WriteLn("Init success");
        }
        else
            return false;

    }

    ioport_set_pin_low(BNO_070_Reset_Pin);
    delay_ms(3);
    ioport_set_pin_high(BNO_070_Reset_Pin);
    delay_ms(100);
    
	if (!BNO070_operation_done(true))
        return false; // exit with error if another 100 mSec passed without INT going low

    packet_received.addr_length=0; // do a read with length 0 which does not write any address bytes
    packet_received.length       = 2;   // transfer data size (bytes)


    if (twi_master_read(TWI_BNO070_PORT, &packet_received)!=STATUS_OK)
        return false; // read failed, something not working right

    // restore normal setting

    packet_received.addr[0]=1;
    packet_received.addr_length=2;
    packet_received.length       = 30;   // transfer data size (bytes)


	if (!BNO070_operation_done(false))
		return false; // exit with error if another 100 mSec passed without INT going high

    // read HID descriptor
    if (twi_master_read(TWI_BNO070_PORT, &packet_received) != STATUS_OK)
    {
        return false;
    }

    delay_ms(3);

    // configure the reporting requirements

#ifdef USE_GAME_ROTATION
    // option 1 from Roy: 
	//const uint8_t rotation_vector_command[] = {0x3f,0x03/* turn on game rotation vector */ ,0x08 ,0x06 ,0x00 ,0x11 ,0x00 ,0x00 ,0x00 ,0x00 ,0x88 ,0x13 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00};
		// option 2 from Roy:
	const uint8_t rotation_vector_command[] = {0x3f,0x03/* turn on game  rotation vector */ ,0x08 ,0x06 ,0x00 ,0x0d,0x00 ,0x00 ,0x00 ,0x00 ,0x88 ,0x07 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00};
	//{0x3f,0x03/* turn on game rotation vector */ ,0x08 ,0x00 ,0x35 ,0x03 ,0x06 ,0x00 ,0x11 ,0x00 ,0x00 ,0x00 ,0x00 ,0x40 ,0x0D ,0x03 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00};
#else
    // option 1 from Roy: 
	//const uint8_t rotation_vector_command[] = {0x3f,0x03/* turn on game rotation vector */ ,0x05 ,0x06 ,0x00 ,0x11 ,0x00 ,0x00 ,0x00 ,0x00 ,0x88 ,0x13 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00};
		// option 2 from Roy:
	const uint8_t rotation_vector_command[] = {0x3f,0x03/* turn on game  rotation vector */ ,0x05 ,0x06 ,0x00 ,0x0d,0x00 ,0x00 ,0x00 ,0x00 ,0x88 ,0x07 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00};
    //const uint8_t rotation_vector_command[] = {0x3f,0x03/* turn on rotation vector */ ,0x05 ,0x00 ,0x35 ,0x03 ,0x06 ,0x00 ,0x11 ,0x00 ,0x00 ,0x00 ,0x00 ,0x40 ,0x0D ,0x03 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00};
#endif

	if (!BNO070_packet_write(5,(void *)rotation_vector_command,sizeof(rotation_vector_command)))
		return false;

	
#ifdef USE_GAME_ROTATION
    const uint8_t set_accl_report[] = {0x3f,0x03,/* game orientation register */ 0x08, 0x06, 0x00, 0x0d,0x00,0x00,0x00,0x00,0x88,0x07,0x00,0x00,0x00,0x00,0x00,0x00}; // /set for 200 Hz read
#else
    const uint8_t set_accl_report[] = {0x3f,0x03,/* orientation register */ 0x05, 0x06, 0x00, 0x0d,0x00,0x00,0x00,0x00,0x88,0x07,0x00,0x00,0x00,0x00,0x00,0x00}; // /set for 200 Hz read
#endif

    if (!BNO070_packet_write(5,(void *)set_accl_report, sizeof(set_accl_report)))
        return false;

    const uint8_t new_report[]= {0x3f, 0x02, 0x05, 0x06,0x00}; // overcome a bug. Normally, read back is not needed

    if (!BNO070_packet_write(5,(void *)new_report, sizeof(new_report)))
        return false;




	// add AR/VR smoothing
	
	// Issue an FRS Write Request – this initiates the protocol
#ifdef USE_GAME_ROTATION
	const uint8_t initiate_write_request[]= {/*0x3f, 0x03, 0x05,0x00,*/0x2F,0x03,0x82,0x06,0x00,0x07,0x00,0x00,0x04,0x00,0x2E,0x3E};
#else
	const uint8_t initiate_write_request[]= {/*0x3f, 0x03, 0x05,0x00,*/0x2F,0x03,0x82,0x06,0x00,0x07,0x00,0x00,0x04,0x00,0x2D,0x3E};
#endif

	if (!BNO070_packet_write(5,(void *)initiate_write_request,sizeof(initiate_write_request)))
		return false;

	if (!BNO070_operation_done(true))
		return false; // exit with error if another 100 mSec passed without INT going low

	// read FRS write response
    packet_received.addr_length=0; // do a read with length 0 which does not write any address bytes
    packet_received.length       = 6;   // transfer data size (bytes)
    if (twi_master_read(TWI_BNO070_PORT, &packet_received) != STATUS_OK)
    {
	    return false;
    }
	
	// Write the first two words of the FRS record
	/* Set report ID 0x83
		Length = 0xd
		Offset = 0
		Scale = 0.2
		Max rotation = 7.3 degrees */

	const uint8_t first_two_words[]={/*0x3f, 0x03,0x05,0x00,*/0x2F,0x03,0x83,0x06,0x00,0x0D,0x00,0x5A,0x00,0x00,0x31,0x08,0xAC,0x0C,0x75,0x93,0x18,0x04};

	if (!BNO070_packet_write(5,(void *)first_two_words,sizeof(first_two_words)))
		return false;

	if (!BNO070_operation_done(true))
		return false; // exit with error if another 100 mSec passed without INT going low


	// read FRS write response - acknowledges words received)
    packet_received.addr_length=0; // do a read with length 0 which does not write any address bytes
    packet_received.length       = 6;   // transfer data size (bytes)
	if (twi_master_read(TWI_BNO070_PORT, &packet_received) != STATUS_OK)
	{
		return false;
	}

	// write last two words
	//const uint8_t last_two_words[]={/*0x3f,0x03,0x05,0x00,*/0x2F,0x03,0x83,0x06,0x00,0x0D,0x00,0x5A,0x02,0x00,0x85,0xEB,0x51,0x08,0x0B,0x41,0x0E,0x00};
	const uint8_t last_two_words[]={/*0x3f,0x03,0x05,0x00,*/0x2F,0x03,0x83,0x06,0x00,0x0D,0x00,0x5A,0x02,0x00,0x85,0xEB,0x51,0x08,0x00,0x00,0x00,0x00};

	if  (!BNO070_packet_write(5,(void *)last_two_words,sizeof(last_two_words)))
		return false;

	if (!BNO070_operation_done(true))
		return false; // exit with error if another 100 mSec passed without INT going low


	// read FRS write response - acknowledges words received
    packet_received.addr_length=0; // do a read with length 0 which does not write any address bytes
    packet_received.length       = 6;   // transfer data size (bytes)
	if (twi_master_read(TWI_BNO070_PORT, &packet_received) != STATUS_OK)
	{
		return false;
	}
	
	// read FRS write response (indicates record is good)
    packet_received.addr_length=0; // do a read with length 0 which does not write any address bytes
    packet_received.length       = 6;   // transfer data size (bytes)
	if (twi_master_read(TWI_BNO070_PORT, &packet_received) != STATUS_OK)
	{
		return false;
	}

	// read write response will be provided (indicates record is written)
    packet_received.addr_length=0; // do a read with length 0 which does not write any address bytes
    packet_received.length       = 6;   // transfer data size (bytes)
	if (twi_master_read(TWI_BNO070_PORT, &packet_received) != STATUS_OK)
	{
		return false;
	}




	// Change max fusion period to 1 mSec
	
	// Issue an FRS Write Request – this initiates the protocol
	const uint8_t initiate_write_request_max_fusion[]= {/*0x3f, 0x03, 0x05,0x00,*/0x2F,0x03,0x82,0x06,0x00,0x07,0x00,0x00,0x01,0x00,0xD7,0xD7};

	if (!BNO070_packet_write(5,(void *)initiate_write_request_max_fusion,sizeof(initiate_write_request_max_fusion)))
		return false;

	if (!BNO070_operation_done(true))
		return false; // exit with error if another 100 mSec passed without INT going low

	// read FRS write response
    packet_received.addr_length=0; // do a read with length 0 which does not write any address bytes
    packet_received.length       = 6;   // transfer data size (bytes)
    if (twi_master_read(TWI_BNO070_PORT, &packet_received) != STATUS_OK)
    {
	    return false;
    }
	
	// Write the first two words of the FRS record
	/* Set report ID 0x83
		Length = 0xd
		Offset = 0
		Max fusion period = 1000us */

	const uint8_t first_two_words_max_fusion[]={/*0x3f, 0x03,0x05,0x00,*/0x2F,0x03,0x83,0x06,0x00,0x0D,0x00,0x5A,0x00,0x00,0xE8,0x03,0x00,0x00,0x00,0x00,0x00,0x00};

	if (!BNO070_packet_write(5,(void *)first_two_words_max_fusion,sizeof(first_two_words_max_fusion)))
		return false;

	if (!BNO070_operation_done(true))
		return false; // exit with error if another 100 mSec passed without INT going low


	// read FRS write response - acknowledges words received)
    packet_received.addr_length=0; // do a read with length 0 which does not write any address bytes
    packet_received.length       = 6;   // transfer data size (bytes)
	if (twi_master_read(TWI_BNO070_PORT, &packet_received) != STATUS_OK)
	{
		return false;
	}

	
	// read FRS write response (indicates record is good)
    packet_received.addr_length=0; // do a read with length 0 which does not write any address bytes
    packet_received.length       = 6;   // transfer data size (bytes)
	if (twi_master_read(TWI_BNO070_PORT, &packet_received) != STATUS_OK)
	{
		return false;
	}

	// read write response will be provided (indicates record is written)
    packet_received.addr_length=0; // do a read with length 0 which does not write any address bytes
    packet_received.length       = 6;   // transfer data size (bytes)
	if (twi_master_read(TWI_BNO070_PORT, &packet_received) != STATUS_OK)
	{
		return false;
	}





    // setup report
    BNO070_Report[0]=1; // this indicates the version number of the report
    BNO070_Report[1]=0; // this indicates the sequence number


    return true;

}

bool Check_BNO070(void)

{

#ifdef USE_GAME_ROTATION
#define BNOOrientationMessageLength 14
#else
#define BNOOrientationMessageLength 16
#endif

    uint8_t BNO070_incoming[BNOOrientationMessageLength];

    if (ioport_get_value(Int_BNO070)==false)
    {
        // data is available, so read it
		
		//ioport_set_pin_high(Side_by_side_B); // !!! for debug
		
        packet_received.addr_length=0; // do a read with length 0 which does not write any address bytes
        packet_received.length=BNOOrientationMessageLength; // need to read just 16 total bytes for orientation report
        packet_received.buffer       = &BNO070_incoming;        // transfer data destination buffer
        twi_master_read(TWI_BNO070_PORT, &packet_received);

		//ioport_set_pin_low(Side_by_side_B); // !!! for debug

        BNO070_Report[1]=BNO070_incoming[1+2]; // sequence number
        memcpy(&BNO070_Report[2],&BNO070_incoming[4+2],8); // copy quaternion data
        udi_hid_generic_send_report_in(BNO070_Report);
		

        while (ioport_get_value(Int_BNO070)==false); // wait for INTN to go back up after read
		

        return true;
    }
    return false;
}

bool Tare_BNO070(void)
{
    // execute tare commands

    // In HID parlance, this is a write to the command register, where the command is “set output report.” The ‘07’ byte is the axes (P2) and the preceding ‘00’ byte is the tare basis.

    const uint8_t tare_now[] = {0x3f,0x03, 0x05 ,0x00 ,0x2F ,0x03 ,0x87 ,0x06 ,0x00 ,0x0D ,0x00 ,0x00 ,0x03 ,0x00 ,0x07 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00};

    twi_package_t packet_write = {
        .addr[0]      = 5, //regNum,      // TWI slave memory address data
        .addr[1]      = 0, //regNum,      // TWI slave memory address data
        .addr_length  = sizeof (uint16_t),    // TWI slave memory address data size
        .chip         = BNO070_ADDR,      // TWI slave bus address
        .buffer       = (void *)tare_now, // transfer data source buffer
        .length       = sizeof(tare_now)  // transfer data size (bytes)
    };

    if (twi_master_write(TWI_BNO070_PORT, &packet_write)!=STATUS_OK)
        return false;

    const uint8_t persist_tare[] = {0x3f,0x03 ,0x05 ,0x00 ,0x2F ,0x03 ,0x87 ,0x06 ,0x00 ,0x0D ,0x00 ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00};

    packet_write.addr[0]      = 5; //regNum,      // TWI slave memory address data
    packet_write.addr[1]      = 0; //regNum,      // TWI slave memory address data
    packet_write.addr_length  = sizeof (uint16_t);    // TWI slave memory address data size
    packet_write.chip         = BNO070_ADDR;      // TWI slave bus address
    packet_write.buffer       = (void *)persist_tare; // transfer data source buffer
    packet_write.length       = sizeof(persist_tare);  // transfer data size (bytes)

    if (twi_master_write(TWI_BNO070_PORT, &packet_write)!=STATUS_OK)
        return false;

    WriteLn("Tare completed");
    return true;
}


#include "sensorhub.h"
extern sensorhub_t sensorhub;

#if 0
#define DFU_MAJOR 1
#define DFU_MINOR 7
#define DFU_PATCH 0
#include "bno-hostif/1000-3251_1.7.0.384.c"

#else 
#define DFU_MAJOR 1
#define DFU_MINOR 2
#define DFU_PATCH 5
#include "bno-hostif/1000-3251_1.2.5.c"
#endif

static sensorhub_ProductID_t readProductId()
{
    sensorhub_ProductID_t id;
    memset(&id, 0, sizeof(id));

    sensorhub.debugPrintf("Requesting product ID...\r\n");
    int rc = sensorhub_getProductID(&sensorhub, &id);
    if (rc != SENSORHUB_STATUS_SUCCESS) {
        debugPrintf("readProductId received error: %d\r\n", rc);
        return id;
    }

    sensorhub.debugPrintf("  Version %d.%d.%d\r\n", id.swVersionMajor, id.swVersionMinor, id.swVersionPatch);
    sensorhub.debugPrintf("  Part number: %d\r\n", id.swPartNumber);
    sensorhub.debugPrintf("  Build number: %d\r\n", id.swBuildNumber);

    return id;
}

bool dfu_BNO070(void) {
    

    sensorhub.debugPrintf("Performing DFU . . . \r\n");
    sensorhub_ProductID_t id = readProductId();
    if ((id.swVersionMajor != DFU_MAJOR) ||
    (id.swVersionMinor != DFU_MINOR) ||
    (id.swVersionPatch != DFU_PATCH)) {
        sensorhub.debugPrintf("BNO is not at %d.%d.%d.  Performing DFU . . . \r\n", DFU_MAJOR, DFU_MINOR, DFU_PATCH);

        int rc = sensorhub_dfu(&sensorhub, sensorhub_dfu_stream, sizeof(sensorhub_dfu_stream));
        if (rc != SENSORHUB_STATUS_SUCCESS) {
            sensorhub.debugPrintf("dfu received error: %d\r\n", rc);
            return;
        }
        sensorhub.debugPrintf("DFU Completed Successfully\r\n");
        // Re-probe:
        sensorhub_probe(&sensorhub);

        // Get the updated version number
        readProductId();
    }
    
}


#endif
