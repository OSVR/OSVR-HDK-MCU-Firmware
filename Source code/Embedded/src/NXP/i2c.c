/* device-specific i2c module, Sensics

Implements I2C communications for NXP HDMI controllers */

#include "GlobalOptions.h"
#ifdef SVR_USING_NXP
#include "Console.h"

#include "i2c.h"
#include "tmNxCompId.h"
#include "my_hardware.h"

#include "conf_usb.h"
#include "ui.h"
#include "uart.h"

#include "NXP/AVRHDMI.h" // includes hardware definition

// ASF Includes
#include <twi_master.h>

#define I2C_TIMEOUT				1 	// msec


bool NXP_HDMIShadow=true; // if true, a write to one HDMI causes a write to both

#ifdef SVR_HAVE_TMDS442
    bool TWI_TMDS442_PORT_initialized=false; // true if already initialized
#endif // SVR_HAVE_TMDS442

// function prototypes

bool i2cWriteRegister(uint8_t addr, uint8_t RegNum, uint8_t TxByte);


uint8_t message[1];

bool i2cWriteRegister(uint8_t addr, uint8_t RegNum, uint8_t TxByte)

{


#ifdef BYPASS_I2C
    return 0;
#endif

#ifdef No_NXP
    return 0;
#endif

    message[0]=TxByte;

    twi_package_t packet = {
        .addr[0]      = RegNum,      // TWI slave memory address data MSB
        .addr_length  = sizeof (uint8_t),     // TWI slave memory address data size
        .chip         = addr,      // TWI slave bus address
        .buffer       = /*(void *)*/message, // transfer data source buffer
        .length       = sizeof(message),   // transfer data size (bytes)
        .no_wait	  = true // return immediately if not available
    };

#ifdef HDMI_DEBUG
    //sprintf(Msg, "w reg %x %x %x",addr,RegNum,TxByte);
    //dWriteLn(Msg,debugI2CReadMask);
#endif

    bool Success;

    // Perform a multi-byte write access then check the result.
    //if (addr==NXP_1_ADDR)
    if (addr==actualNXP_1_ADDR)
    {
        Success = (twi_master_write(TWI_NXP1_PORT, &packet) == TWI_SUCCESS);
#ifdef SVR_HAVE_NXP2
        if (NXP_HDMIShadow)
		{
			//packet.chip=actualNXP_1_ADDR;
            Success = (twi_master_write(TWI_NXP2_PORT, &packet) == TWI_SUCCESS);
		}
#endif // SVR_HAVE_NXP2
    }
    //else if (addr==(CEC_1_ADDR+1))
    else if (addr==actualCEC_1_ADDR)
    {
        WriteLn("CEC 1 match");
        packet.chip=actualCEC_1_ADDR;
        Success = (twi_master_write(TWI_NXP1_PORT, &packet) == TWI_SUCCESS);
#ifdef SVR_HAVE_NXP2
        if (NXP_HDMIShadow)
		{
			//packet.chip=actualCEC_1_ADDR;
            Success = (twi_master_write(TWI_NXP2_PORT, &packet) == TWI_SUCCESS);
		}
#endif // SVR_HAVE_NXP2
    }
    //else if (addr==NXP_2_ADDR)


    else
        Success = false;

#ifdef HDMI_DEBUG
    if (Success==false)
        dWriteLn("write failed",debugI2CReadMask);
#endif
    return (Success);
}




uint8_t NXPReadRegister(uint8_t addr, uint8_t RegNum)

{

    uint8_t data_received[1];
    status_code_t Outcome;
    bool Result;


#ifdef BYPASS_I2C
    return 0;
#endif

#ifdef No_NXP
    return 0;
#endif

#ifdef HDMI_DEBUG
    //sprintf(Msg, "r reg %x %x",addr,RegNum);
    //dWriteLn(Msg,debugI2CReadMask);
#endif

    twi_package_t packet_received = {
        .addr[0]      = RegNum,      // TWI slave memory address data MSB
        .addr_length  = sizeof (uint8_t),     // TWI slave memory address data size
        .chip         = addr,      // TWI slave bus address
        .buffer       = data_received,        // transfer data destination buffer
        .length       = sizeof(data_received)   // transfer data size (bytes)
    };

    // Perform a multi-byte read access then check the result.
    if (addr==actualNXP_1_ADDR)
    {
        Outcome=twi_master_read(TWI_NXP1_PORT, &packet_received);
        Result= (Outcome == TWI_SUCCESS);
    }
    else if (addr==actualCEC_1_ADDR)
    {
        packet_received.chip=CEC_1_ADDR;
        Outcome=twi_master_read(TWI_NXP1_PORT, &packet_received);
        Result= (Outcome == TWI_SUCCESS);
    }
    else
    {
        Outcome=0;
        Result=false;
    }

    if (Result)
    {
#ifdef HDMI_DEBUG
        //sprintf(Msg,"%x:%x",RegNum,data_received[0]);
        //dWriteLn(Msg,debugI2CReadMask);
#endif
        return data_received[0];
    }
    else
    {
#ifdef HDMI_DEBUG
        //sprintf(Msg,"read err: %x",Outcome);
        //dWriteLn(Msg,debugI2CReadMask);
#endif
        return 0;
    }
}




uint8_t NXPReadMulti(uint8_t addr,uint8_t RegNum,uint8_t len,UInt8 *pdat) // read multiple bytes. Return status

{

    //uint8_t data_received[1];

    bool Result;

    twi_package_t packet_received = {
        .addr[0]      = RegNum,      // TWI slave memory address data MSB
        .addr_length  = sizeof (uint8_t),     // TWI slave memory address data size
        .chip         = addr,      // TWI slave bus address
        .buffer       = pdat,        // transfer data destination buffer
        .length       = len   // transfer data size (bytes)
    };

    // Perform a multi-byte read access then check the result.
    if (addr==NXP_1_ADDR)
        Result=(twi_master_read(TWI_NXP1_PORT, &packet_received) == TWI_SUCCESS);
    else if (addr==CEC_1_ADDR)
        Result=(twi_master_read(TWI_NXP1_PORT, &packet_received) == TWI_SUCCESS);
#ifdef SVR_HAVE_NXP2
    else if (addr==NXP_2_ADDR)
    {
        packet_received.chip=NXP_1_ADDR;
        Result=(twi_master_read(TWI_NXP2_PORT, &packet_received) == TWI_SUCCESS);
    }
    else if (addr==CEC_2_ADDR)
    {
        packet_received.chip=CEC_1_ADDR;
        Result=(twi_master_read(TWI_NXP2_PORT, &packet_received) == TWI_SUCCESS);
    }
#endif // SVR_HAVE_NXP2
    else
        Result=false;

    if (Result)
        return TM_OK;
    else
        return 0;

}



/*
    UInt8 slaveAddr;
    UInt8 firstRegister;
    UInt8 lenData;
    UInt8 *pData;
*/


tmErrorCode_t Init_i2c(void)

{
#ifdef BYPASS_I2C
    return(TM_OK);
#endif
    dWriteLn("I2C init",debugI2CReadMask);
    twi_master_options_t opt_NXP;

    opt_NXP.speed = NXP_TWI_SPEED;
    opt_NXP.chip  = NXP_1_ADDR;
    twi_master_setup(TWI_NXP1_PORT, &opt_NXP);
#ifdef SVR_HAVE_TMDS442
    if (!TWI_TMDS442_PORT_initialized)
    {
        // determine if port 2 has already been initialized with HDMI switch or needs a separate init
        twi_master_setup(TWI_NXP2_PORT, &opt_NXP);
        TWI_TMDS442_PORT_initialized=true;
    }
#endif // SVR_HAVE_TMDS442

//	ioport_configure_pin(TWIE_SDA, ioport_dir
    return(TM_OK);
};


tmErrorCode_t  i2cWrite(i2cRegisterType_t type_register,tmbslHdmiSysArgs_t *pSysArgs)

{
    UInt8 slave = pSysArgs->slaveAddr;
    UInt8 first = pSysArgs->firstRegister;
    UInt8 len= pSysArgs->lenData;
    UInt8 *pdat = pSysArgs->pData;

    UInt8 i;

    //char msg[10];


#ifdef BYPASS_I2C
    return(TM_OK);
#endif

#ifdef No_NXP
    return(TM_OK);
#endif



    if (slave!=slaveAddressDriverHdmiCEC)
    {
        //sprintf(Msg,"Sla:%x",slave);
        //WriteLn(Msg);
        for (i=0; i<len; i++)
        {

            i2cWriteRegister(NXP_1_ADDR, first, *pdat);

#ifdef SVR_HAVE_NXP2
            if (NXP_HDMIShadow)
            {
                //i2cWriteRegister(NXP_2_ADDR, first, *pdat);
            }
#endif // SVR_HAVE_NXP2
            //_delay_ms(5);
            first++;
            pdat++;
        }
    }
    else
    {
        //sprintf(Msg,"Sla:%x",slave);
        //WriteLn(Msg);
        for (i=0; i<len; i++)
        {

            i2cWriteRegister(slave, first, *pdat);
//#ifndef OSVRHDK
//
            //if (NXP_HDMIShadow)
            //{
	            //i2cWriteRegister(NXP_2_ADDR, first, *pdat);
            //}
//#endif
            first++;
            pdat++;
        }
    }
    return(TM_OK);
};



tmErrorCode_t  i2cRead(i2cRegisterType_t type_register,tmbslHdmiSysArgs_t *pSysArgs)

{
    UInt8 slave = pSysArgs->slaveAddr;
    UInt8 first = pSysArgs->firstRegister;
    UInt8 len= pSysArgs->lenData;
    UInt8 *pdat = pSysArgs->pData;

    UInt8 read;



    if (len==1)
    {
        read=NXPReadRegister(slave, first);
        *pdat=read;
        return(TM_OK);
    }
    else
        return(NXPReadMulti(slave,first,len,pdat));


};


uint8_t I2CReadRegister(uint8_t addr, uint8_t RegNum)

{
    uint8_t Status;

    twi_package_t packet_received = {
        .addr[0]      = RegNum,      // TWI slave memory address data
        .addr_length  = sizeof (uint8_t),     // TWI slave memory address data size
        .chip         = addr,      // TWI slave bus address
        .buffer       = &Status,        // transfer data destination buffer
        .length       = 1   // transfer data size (bytes)
    };

    // Perform a read access then check the result.
    if (twi_master_read(TWI_NXP1_PORT, &packet_received) == TWI_SUCCESS)
    {
        return true;
    }
    else
        return false;
}

#endif // SVR_USING_NXP
