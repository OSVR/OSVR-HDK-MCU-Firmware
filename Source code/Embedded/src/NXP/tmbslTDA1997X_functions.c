/**

 * Copyright (C) 2006 NXP N.V., All Rights Reserved.
 * Exposed under Apache 2.0 Open Source License with permission of NXP N.V.
 *
 * \file          tmbslTDA1997X_functions.c
 *
 * \version       Revision: 1
 *
 * \date          Date: 22/11/07 10:30
 *
 * \brief         BSL driver component API for the TDA1997X HDMI Receiver
 *
 * \section refs  Reference Documents
 * HDMI Rx Driver - FRS.doc,
 * HDMI Rx Driver - tmbslTDA1997X - SCS.doc
 *
 * \section info  Change Information
 *
 * \verbatim

   History: tmbslTDA1997X_functions.c
 *

   \endverbatim
 *
*/

/*============================================================================*/
/*                       INCLUDE FILES                                        */
/*============================================================================*/

// Options header
#include "GlobalOptions.h"

// SVR_HDMI_VERY_VERBOSE implies HDMI_VERBOSE
#if defined(SVR_HDMI_VERY_VERBOSE) && (!defined(HDMI_VERBOSE))
#define HDMI_VERBOSE
#endif

#ifdef SVR_HAVE_NXP
// Interface header
#include "DeviceDrivers/VideoInput.h" // for PortraitMode

// Internal headers for NXP driver
#include "AVRHDMI.h"
#include "tmbslTDA1997X_Functions.h"
#include "tmbslTDA1997X_local.h"
#include "i2c.h"

// application headers
#include "Console.h"
#include "my_hardware.h"

// standard headers
#include <stdio.h>

/*============================================================================*/
/*                     TYPES DECLARATIONS                                     */
/*============================================================================*/
#ifdef TMFL_OS_WINDOWS
    #define DUMMY_ACCESS(x) (x)
#else
    #define DUMMY_ACCESS(x)
#endif

uint8_t CECInterrupt;

#ifdef HDMI_DEBUG
    //#define x
    //#define x	{Write("bs-"); NXP_Private_PRINTIF(999,__LINE__); }

#else
    //#define x	;
#endif




/*============================================================================*/
/*                       CONSTANTS DECLARATIONS                               */
/*============================================================================*/
static const tmbslHdmiRxIRQSource_t tabIrqSource72[INTERRUPT_CLR_FLG_REG_NUM][NBR_BIT_INT_FLAGS] =
{
    /* SUS */
    {   BSLHDMIRX_IRQSOURCE_CONFIG_MTP,        BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_NOT_USED,
        BSLHDMIRX_IRQSOURCE_HDMI_LOCK,  BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_NOT_USED
    },

    /* DDC */
#ifdef TMFL_HDMI_OUT
    {   BSLHDMIRX_IRQSOURCE_EDID_MTP,  BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_EDID_READ,
#else
    {   BSLHDMIRX_IRQSOURCE_EDID_MTP,  BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_NOT_USED,
#endif
        BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_NOT_USED
    },

    /* RATE */
    {   BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_RXDB,  BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_NOT_USED,
        BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_RXDA,  BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_NOT_USED
    },

    /* MODE */
    {   BSLHDMIRX_IRQSOURCE_FLAGS,  BSLHDMIRX_IRQSOURCE_GAMUT,     BSLHDMIRX_IRQSOURCE_ISRC2,     BSLHDMIRX_IRQSOURCE_ISRC1,
        BSLHDMIRX_IRQSOURCE_ACP,    BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_DEEP_COLOR_MODE_CHANGED
    },

    /* INFO */
    {   BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_MPS_IF,    BSLHDMIRX_IRQSOURCE_AUD_IF,    BSLHDMIRX_IRQSOURCE_SPD_IF,
        BSLHDMIRX_IRQSOURCE_AVI_IF,    BSLHDMIRX_IRQSOURCE_VS_OTHER_BK2_IF,  BSLHDMIRX_IRQSOURCE_VS_OTHER_BK1_IF,  BSLHDMIRX_IRQSOURCE_VS_IF
    },

    /* AUDIO */
    {   BSLHDMIRX_IRQSOURCE_NOT_USED,    BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_NOT_USED,            BSLHDMIRX_IRQSOURCE_AUDIO_PACKET,
        BSLHDMIRX_IRQSOURCE_AUDIO_MUTE,  BSLHDMIRX_IRQSOURCE_AUDIO_CHANNEL_STATUS,  BSLHDMIRX_IRQSOURCE_AUDIO_FIFO_UNMUTED,  BSLHDMIRX_IRQSOURCE_NOT_USED
    },

    /* HDCP */
    {   BSLHDMIRX_IRQSOURCE_HDCP_MTP,  BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_NOT_USED,
        BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_STATE_C5,  BSLHDMIRX_IRQSOURCE_NOT_USED
    },

    /* AFE */
    {   BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_NOT_USED,
        BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_NOT_USED
    }

#ifdef TMFL_HDMI_OUT
    /* CEC block used for RXS and HPD*/
    ,{
        BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_HPD,
        BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_RXS,  BSLHDMIRX_IRQSOURCE_NOT_USED,  BSLHDMIRX_IRQSOURCE_NOT_USED
    }
#endif

};

/*============================================================================*/
/*                       VARIABLES DECLARATIONS                               */
/*============================================================================*/
tmbslTDA1997XConfigUnit_t   RxHdmiConfig[MAX_UNIT]=
{   {   False, 						/* Bool initUnit */
        E_PAGE_INVALID,				/* tmbslTDA1997XPage_t currentPage */
        0, 							/* UInt8 uHwAddress */
        0, 							/* UInt8 uCECAddress */
        Null, 						/* ptmbslHdmiRxSysFunc_t sysFuncWrite */
        Null, 						/* ptmbslHdmiRxSysFunc_t sysFuncRead */
        Null, 						/* ptmbslHdmiRxCallbackFunc_t callbackFunc */
        Null, 						/* ptmbslHdmiRxSysFuncTimer_t sysFuncTimer */
        BSLHDMIRX_TDA_UNKNOWN, 		/* tmbslHdmiRxVersion_t version */
        {   /* tmbslHdmiChipConfiguration_t	chipConfiguration */
            BSLHDMIRX_TMDS_B_CLK,		/* tmbslHdmiRxBClk_t B_Clk */
            BSLHDMIRX_TMDS_B_SOC,		/* tmbslHdmiRxBSoc_t B_Soc */
            0,							/* UInt8 configuration */
            0,							/* UInt8 revision (N1=0/N2 otherwise)*/
            BSLHDMIRX_TMDS_VP_24,		/* tmbslHdmiVP_t videoPort */
            BSLHDMIRX_OUT_SUP_3_3,		/* tmbslHdmiOutSupply outputSupply */
            BSLHDMIRX_CEC_DISABLED		/* tmbslHdmiCECEnabled_t CECEnabled */
        }, 							/* tmbslHdmiChipConfiguration_t	chipConfiguration */
        0, 							/* UInt8 writePageErrorFlag */
        0,							/* UInt8 currentAudioFreq */
        False,						/* Bool	MPTRWInProgress */
        /* WORKAROUND START: PR1296 */ 0 /* WORKAROUND END: PR1296 */,	/* UInt8 audioTestModeUsed */
        0							/* UInt8 hdmiFlagsStatus */
        ,False						/* Bool VSIReceived */
#ifdef TMFL_CALIBRATION_OPT
        ,/* WORKAROUND START: PR1633 */ False /* WORKAROUND END: PR1633 */ /* Bool susResetBySW */
#endif
    }
};


static UInt8	FakeOTPAddress;

Bool I2CGuard = False;

//extern UInt16 timer;

/*============================================================================*/
/*                       FUNCTION PROTOTYPES                                  */
/*============================================================================*/

tmErrorCode_t
bslTDA1997XSetPixelAndLineCounters
(
    tmUnitSelect_t unit,
    UInt16         pixelCountPreset,
    UInt16         pixelCountNumber,
    UInt16         lineCountPreset,
    UInt16         lineCountNumber
);

tmErrorCode_t
bslTDA1997XSetVHRefValues
(
    tmUnitSelect_t unit,
    tmbslHdmiRxVHRefValues_t   *pVHRefValues
);

tmErrorCode_t
bslTDA1997XSetVHSyncValues
(
    tmUnitSelect_t unit,
    tmbslHdmiRxVHSyncValues_t *pVHSyncValues
);

/*============================================================================*/
/*
    \brief Initialize the equalizer for an instance of an HDMI receiver.
           The register pages that configure the equalizer are written with
           the proper values.

    \param Unit            Receiver unit number

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is already initialised

******************************************************************************/
static tmErrorCode_t
bslTDA1997XInitTMDS
(
    tmUnitSelect_t      unit
);

/*============================================================================*/
/*
    \brief Read the status registers for interupts on the chip.

    \param Unit            Receiver unit number
    \param regStatus       Byte with the status of each interrupt

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is already initialised
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus

******************************************************************************/
static tmErrorCode_t
bslTDA1997XReadInterruptStatusRegisters
(
    tmUnitSelect_t          unit,
    UInt8                   *pRegStatus
);

/*============================================================================*/
/*
    \brief Callback the activity detection interrupts.

    \param Unit            Receiver unit number
    \param IrqSource       Interrupt to be processed
    \param regStatus       Byte with the status of each interrupt

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is already initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range

******************************************************************************/
static tmErrorCode_t
bslTDA1997XActivityDetectionInterrupts
(
    tmUnitSelect_t          unit,
    tmbslHdmiRxIRQSource_t  irqSource,
    UInt8                   regStatus
);

/*============================================================================*/
/*
    \brief This function checks periodically the audio frequency.

    \param Unit            Receiver unit number

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is already initialised
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus

******************************************************************************/
static tmErrorCode_t
bslTDA1997XCheckAudioFrequency
(
    tmUnitSelect_t  unit
);

/*============================================================================*/
/*
    \brief This function checks periodically the VSI update counter.

    \param Unit            Receiver unit number

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is already initialised
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus

******************************************************************************/
static tmErrorCode_t
bslTDA1997XCheckVSIUpdate
(
    tmUnitSelect_t  unit
);

/*============================================================================*/
/*
    \brief This function checks periodically the HDMI flags status.

    \param Unit            Receiver unit number

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is already initialised
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus

******************************************************************************/
static tmErrorCode_t
bslTDA1997XCheckHdmiFlags
(
    tmUnitSelect_t  unit
);

/* WORKAROUND START: PR1308 */
/*============================================================================*/
/*
    \brief This function changes the audio Fifo latency management according
           to the layout.

    \param Unit            Receiver unit number

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is already initialised
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus

******************************************************************************/
static tmErrorCode_t
bslTDA1997XChangeAudioFifoLatency
(
    tmUnitSelect_t  unit
);
/* WORKAROUND END: PR1308 */

/*============================================================================*/
/*
    \brief This function restart the audio regulation in case of audio FIFO
           failure (FIFO read and write pointers are crossed).

    \param Unit            Receiver unit number

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is already initialised
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus

******************************************************************************/
static tmErrorCode_t
bslTDA1997XRestartRegulation
(
    tmUnitSelect_t  unit
);

/* WORKAROUND START: PR1633 */
#ifdef TMFL_CALIBRATION_OPT
/*============================================================================*/
/*
    \brief This function changes the word locker mode.

    \param Unit            Receiver unit number

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is already initialised
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus

******************************************************************************/
static tmErrorCode_t
bslTDA1997XForceEqualizerGain
(
    tmUnitSelect_t  unit
);
#endif
/* WORKAROUND END: PR1633 */






/*============================================================================*/
/**
    \brief        Writing on I2C bus

    \param        unit          Unit number
    \param        firstRegister First register to write
    \param        lengthData    Length
    \param        * pBuffer     Pointer of data to be written

    \return     - TM_OK  : the call was successful
                - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
                  the receiver instance is already initialised
                - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing the I2C bus
                - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
                  inconsistent

 ******************************************************************************/

/// @todo Note: referenced in AVRHDMI.c as well.
tmErrorCode_t
bslTDA1997XWriteI2C
(
tmUnitSelect_t          unit,
UInt16                  firstRegister,
UInt8                   lengthData,
UInt8                   *pBuffer
); // definition for I2C routine


tmErrorCode_t
bslTDA1997XWriteI2C
(
    tmUnitSelect_t          unit,
    UInt16                  firstRegister,
    UInt8                   lengthData,
    UInt8                   *pBuffer
)
{
    tmErrorCode_t          errCode = TM_OK;
    tmbslTDA1997XPage_t    requestedPage;
    tmbslHdmiRxSysArgs_t   sysArgs;
    tmbslHdmiRxSysArgs_t   *pSysArgs= &sysArgs;
    UInt8				   oldSlaveAddr;


    /* Test pBuffer <> NULL */
    RETIF(pBuffer == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    if ((firstRegister >> 8) != E_PAGE_CEC) /* HDMI I2C register */
    {

        pSysArgs->slaveAddr = RxHdmiConfig[unit].uHwAddress;

        requestedPage = (tmbslTDA1997XPage_t)(firstRegister >> 8);

        //sprintf(Msg,"Page: %d",requestedPage);
        //WriteLn(Msg);
//
        if ( (requestedPage  != RxHdmiConfig[unit].currentPage) || (RxHdmiConfig[unit].writePageErrorFlag == 1) ) // comment out if you want to write page register any time
        {






            /* write the page register */
            /* The current page address register is the same on the hardware for all the pages,
               there is no need to check the current page to write this register */
            pSysArgs->firstRegister  = (UInt8)(CURPAGE_ADR_00H);
            pSysArgs->lenData        = 1;
            pSysArgs->pData          = &requestedPage;


            /* Call the external I2C write function with its parameters */
            errCode =  (RxHdmiConfig[unit].sysFuncWrite) (pSysArgs);



            RETIF(errCode != TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

            if(errCode != TM_OK)
            {

                RxHdmiConfig[unit].writePageErrorFlag = 1;
                return TMBSL_ERR_BSLHDMIRX_I2C_WRITE;
            }
            else
            {
                RxHdmiConfig[unit].writePageErrorFlag = 0;
            }

            RxHdmiConfig[unit].currentPage = requestedPage;
        }


        if ( (firstRegister != CURPAGE_ADR_00H) && (firstRegister != CURPAGE_ADR_01H) && (firstRegister != CURPAGE_ADR_02H) &&
                (firstRegister != CURPAGE_ADR_12H) && (firstRegister != CURPAGE_ADR_13H) &&
                (firstRegister != CURPAGE_ADR_20H) && (firstRegister != CURPAGE_ADR_21H) &&
                (firstRegister != CURPAGE_ADR_22H) && (firstRegister != CURPAGE_ADR_23H) &&
                (firstRegister != CURPAGE_ADR_30H) &&
                (firstRegister != CURPAGE_ADR_40H) && (firstRegister != CURPAGE_ADR_41H) && (firstRegister != CURPAGE_ADR_42H))
        {

            /* special action when SLAVE_ADDR is written: needs to update RxHdmiConfig[unit].uHwAddress and RxHdmiConfig[unit].uCECAddress */
            if ((firstRegister <= SLAVE_ADDR) && ((firstRegister + lengthData -1) >= SLAVE_ADDR))
            {
                /* write values up to SLAVE_ADDR */
                if (firstRegister != SLAVE_ADDR)
                {
                    pSysArgs->firstRegister  = (UInt8)firstRegister;
                    pSysArgs->lenData        = (SLAVE_ADDR - firstRegister);
                    pSysArgs->pData          = pBuffer;

                    errCode =  (RxHdmiConfig[unit].sysFuncWrite) (pSysArgs);
                    RETIF(errCode != TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
                }

                /* read old SLAVE_ADDR */
                pSysArgs->firstRegister  = (UInt8)SLAVE_ADDR;
                pSysArgs->lenData        = 1;
                pSysArgs->pData          = &oldSlaveAddr;



                errCode =  (RxHdmiConfig[unit].sysFuncRead) (pSysArgs);
                RETIF(errCode != TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

                /* write new SLAVE_ADDR */
                pSysArgs->firstRegister  = (UInt8)SLAVE_ADDR;
                pSysArgs->lenData        = 1;
                pSysArgs->pData          = (UInt8 *)((uintptr_t)pBuffer + (SLAVE_ADDR - firstRegister));



                errCode =  (RxHdmiConfig[unit].sysFuncWrite) (pSysArgs);
                RETIF(errCode != TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

#ifdef HDMI_DEBUG
                WriteLn("Address update 1");
#endif

                /* update RxHdmiConfig[unit].uHwAddress and RxHdmiConfig[unit].uCECAddres */
                RxHdmiConfig[unit].uHwAddress -= (oldSlaveAddr & 0x7);
                RxHdmiConfig[unit].uHwAddress += (*(pSysArgs->pData) &  0x7);
                RxHdmiConfig[unit].uCECAddress -= ((oldSlaveAddr >>4) & 0x3);
                RxHdmiConfig[unit].uCECAddress += ((*(pSysArgs->pData) >>4) &  0x3);

                if (unit==0)
                {
                    actualNXP_1_ADDR=RxHdmiConfig[unit].uHwAddress;
                    actualCEC_1_ADDR=RxHdmiConfig[unit].uCECAddress;
                }
#ifdef SVR_HAVE_NXP2
                else if (unit==1)
                {
                    actualNXP_2_ADDR=RxHdmiConfig[unit].uHwAddress;
                    actualCEC_2_ADDR=RxHdmiConfig[unit].uCECAddress;
                }
#endif
                /* update slaveAddr */
                pSysArgs->slaveAddr = RxHdmiConfig[unit].uHwAddress;

                /* write end of buffer */
                if ((firstRegister + lengthData -1) != SLAVE_ADDR)
                {
                    pSysArgs->firstRegister  = (UInt8)(SLAVE_ADDR + 1);
                    pSysArgs->lenData        = (lengthData -(SLAVE_ADDR - firstRegister) -1);
                    pSysArgs->pData          = pBuffer;



                    errCode =  (RxHdmiConfig[unit].sysFuncWrite) (pSysArgs);
                    RETIF(errCode != TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

                }
            }
            /* end of special action*/
            else
            {
                /* write in registers requested through I2C */
                pSysArgs->firstRegister  = (UInt8)firstRegister;
                pSysArgs->lenData        = lengthData;
                pSysArgs->pData          = pBuffer;



                errCode =  (RxHdmiConfig[unit].sysFuncWrite) (pSysArgs);
                RETIF(errCode != TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
            }

        }

    }
    else /* CEC register */
    {

        pSysArgs->slaveAddr = RxHdmiConfig[unit].uCECAddress;
        /* no page to program*/
        /* write in registers requested through I2C */
        pSysArgs->firstRegister  = (UInt8)firstRegister;
        pSysArgs->lenData        = lengthData;
        pSysArgs->pData          = pBuffer;

        errCode =  (RxHdmiConfig[unit].sysFuncWrite) (pSysArgs);
        RETIF(errCode != TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    }


    return TM_OK;
}



tmErrorCode_t
tmbslTDA1997XWriteI2C
(
    tmUnitSelect_t          unit,
    UInt16                  firstRegister,
    UInt8                   lengthData,
    UInt8                   *pBuffer
)
{

    tmErrorCode_t          errCode = TM_OK;
    tmbslTDA1997XPage_t    requestedPage;
    tmbslHdmiRxSysArgs_t   sysArgs;
    tmbslHdmiRxSysArgs_t   *pSysArgs= &sysArgs;
    UInt8				   oldSlaveAddr;

    /* Test pBuffer <> NULL */
    RETIF(pBuffer == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)



    if ((firstRegister >> 8) != E_PAGE_CEC) /* HDMI I2C register */
    {


        pSysArgs->slaveAddr = RxHdmiConfig[unit].uHwAddress;

        requestedPage = (tmbslTDA1997XPage_t)(firstRegister >> 8);


        //sprintf(Msg,"Page: %d",requestedPage);
        //Writ(Msg);

        if ( (requestedPage  != RxHdmiConfig[unit].currentPage) || (RxHdmiConfig[unit].writePageErrorFlag == 1) ) // comment out if you want to write page every time
        {





            /* write the page register */
            /* The current page address register is the same on the hardware for all the pages,
               there is no need to check the current page to write this register */
            pSysArgs->firstRegister  = (UInt8)(CURPAGE_ADR_00H);
            pSysArgs->lenData        = 1;
            pSysArgs->pData          = (UInt8 *)&requestedPage;

            /* Call the external I2C write function with its parameters */
            errCode =  (RxHdmiConfig[unit].sysFuncWrite) (pSysArgs);

            RETIF(errCode != TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
            if(errCode != TM_OK)
            {
                RxHdmiConfig[unit].writePageErrorFlag = 1;
                return TMBSL_ERR_BSLHDMIRX_I2C_WRITE;
            }
            else
            {
                RxHdmiConfig[unit].writePageErrorFlag = 0;
            }

            RxHdmiConfig[unit].currentPage = requestedPage;
        }


        if ( (firstRegister != CURPAGE_ADR_00H) && (firstRegister != CURPAGE_ADR_01H) && (firstRegister != CURPAGE_ADR_02H) &&
                (firstRegister != CURPAGE_ADR_12H) && (firstRegister != CURPAGE_ADR_13H) &&
                (firstRegister != CURPAGE_ADR_20H) && (firstRegister != CURPAGE_ADR_21H) &&
                (firstRegister != CURPAGE_ADR_22H) && (firstRegister != CURPAGE_ADR_23H) &&
                (firstRegister != CURPAGE_ADR_30H) &&
                (firstRegister != CURPAGE_ADR_40H) && (firstRegister != CURPAGE_ADR_41H) && (firstRegister != CURPAGE_ADR_42H))
        {


            /* special action when SLAVE_ADDR is written: needs to update RxHdmiConfig[unit].uHwAddress and RxHdmiConfig[unit].uCECAddress */
            if ((firstRegister <= SLAVE_ADDR) && ((firstRegister + lengthData -1) >= SLAVE_ADDR))
            {
                /* write values up to SLAVE_ADDR */
                if (firstRegister != SLAVE_ADDR)
                {
                    pSysArgs->firstRegister  = (UInt8)firstRegister;
                    pSysArgs->lenData        = (SLAVE_ADDR - firstRegister);
                    pSysArgs->pData          = pBuffer;

                    errCode =  (RxHdmiConfig[unit].sysFuncWrite) (pSysArgs);
                    RETIF(errCode != TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
                }

                /* read old SLAVE_ADDR */
                pSysArgs->firstRegister  = (UInt8)SLAVE_ADDR;
                pSysArgs->lenData        = 1;
                pSysArgs->pData          = &oldSlaveAddr;



                errCode =  (RxHdmiConfig[unit].sysFuncRead) (pSysArgs);
                RETIF(errCode != TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

                /* write new SLAVE_ADDR */
                pSysArgs->firstRegister  = (UInt8)SLAVE_ADDR;
                pSysArgs->lenData        = 1;
                pSysArgs->pData          = (UInt8 *)((uintptr_t)pBuffer + (SLAVE_ADDR - firstRegister));



                errCode =  (RxHdmiConfig[unit].sysFuncWrite) (pSysArgs);
                RETIF(errCode != TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

                /* update RxHdmiConfig[unit].uHwAddress and RxHdmiConfig[unit].uCECAddres */
#ifdef HDMI_DEBUG
                WriteLn("Address update");
#endif
                RxHdmiConfig[unit].uHwAddress -= (oldSlaveAddr & 0x7);
                RxHdmiConfig[unit].uHwAddress += (*(pSysArgs->pData) &  0x7);
                RxHdmiConfig[unit].uCECAddress -= ((oldSlaveAddr >>4) & 0x3);
                RxHdmiConfig[unit].uCECAddress += ((*(pSysArgs->pData) >>4) &  0x3);
#ifdef HDMI_DEBUG
                sprintf(Msg,"u %d, i %d, c %d",unit,RxHdmiConfig[unit].uHwAddress ,RxHdmiConfig[unit].uCECAddress);
                WriteLn(Msg);
#endif

                if (unit==0)
                {
                    actualNXP_1_ADDR=RxHdmiConfig[unit].uHwAddress;
                    actualCEC_1_ADDR=RxHdmiConfig[unit].uCECAddress;
                }
#ifndef	OSVRHDK
                else if (unit==1)
                {
                    actualNXP_2_ADDR=RxHdmiConfig[unit].uHwAddress;
                    actualCEC_2_ADDR=RxHdmiConfig[unit].uCECAddress;
                }
#endif

                /* update slaveAddr */
                pSysArgs->slaveAddr = RxHdmiConfig[unit].uHwAddress;

                /* write end of buffer */
                if ((firstRegister + lengthData -1) != SLAVE_ADDR)
                {
                    pSysArgs->firstRegister  = (UInt8)(SLAVE_ADDR + 1);
                    pSysArgs->lenData        = (lengthData -(SLAVE_ADDR - firstRegister) -1);
                    pSysArgs->pData          = pBuffer;



                    errCode =  (RxHdmiConfig[unit].sysFuncWrite) (pSysArgs);
                    RETIF(errCode != TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

                }
            }
            /* end of special action*/
            else
            {

                /* write in registers requested through I2C */
                pSysArgs->firstRegister  = (UInt8)firstRegister;
                pSysArgs->lenData        = lengthData;
                pSysArgs->pData          = pBuffer;



                errCode =  (RxHdmiConfig[unit].sysFuncWrite) (pSysArgs);
                RETIF(errCode != TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
            }

        }

    }
    else /* CEC register */
    {


        sprintf(Msg,"CEC: %x",RxHdmiConfig[unit].uCECAddress);
        WriteLn(Msg);
        pSysArgs->slaveAddr = RxHdmiConfig[unit].uCECAddress;
        /* no page to program*/
        /* write in registers requested through I2C */
        pSysArgs->firstRegister  = (UInt8)firstRegister;
        pSysArgs->lenData        = lengthData;
        pSysArgs->pData          = pBuffer;

        errCode =  (RxHdmiConfig[unit].sysFuncWrite) (pSysArgs);
        RETIF(errCode != TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    }


    return TM_OK;

}




/*============================================================================*/
/**
    \brief        Reading on I2C bus

    \param        unit          Unit number
    \param        firstRegister First register
    \param        lenghtData    Length
    \param        * pBuffer     Pointer of data to be read

    \return     - TM_OK  : the call was successful
                - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
                  the receiver instance is already initialised
                - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing the I2C bus
                - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus
                - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
                  inconsistent

 ******************************************************************************/
tmErrorCode_t
bslTDA1997XReadI2C
(
    tmUnitSelect_t          unit,
    UInt16                  firstRegister,
    UInt8                   lengthData,
    UInt8                   *pBuffer
)
{
    tmErrorCode_t           errCode = TM_OK;
    tmbslTDA1997XPage_t     requestedPage;
    tmbslHdmiRxSysArgs_t    sysArgs;
    tmbslHdmiRxSysArgs_t    *pSysArgs = &sysArgs;
    UInt8                   regValue;


    /* Test pBuffer <> NULL */
    if (pBuffer == Null)
    {
        return (TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS);
    }

    /* test the unit number */
    if (unit >= MAX_UNIT)
    {
        return (TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER);
    }

    if ((firstRegister >> 8) != E_PAGE_CEC) /* HDMI I2C register */
    {
        requestedPage = (tmbslTDA1997XPage_t)(firstRegister >> 8);

        if (requestedPage != RxHdmiConfig[unit].currentPage) // comment out if you want to write page value every time
        {
            regValue = (UInt8)requestedPage;

            switch (regValue)
            {
            case E_PAGE_00:
                errCode  =  bslTDA1997XWriteI2C(unit, (UInt16)CURPAGE_ADR_00H, 1, &regValue);
                break;
            case E_PAGE_01:
                errCode  =  bslTDA1997XWriteI2C(unit, (UInt16)CURPAGE_ADR_01H, 1, &regValue);
                break;
            case E_PAGE_02:
                errCode  =  bslTDA1997XWriteI2C(unit, (UInt16)CURPAGE_ADR_02H, 1, &regValue);
                break;
            case E_PAGE_12:
                errCode  =  bslTDA1997XWriteI2C(unit, (UInt16)CURPAGE_ADR_12H, 1, &regValue);
                break;
            case E_PAGE_13:
                errCode  =  bslTDA1997XWriteI2C(unit, (UInt16)CURPAGE_ADR_13H, 1, &regValue);
                break;
            case E_PAGE_14:
                errCode  =  bslTDA1997XWriteI2C(unit, (UInt16)CURPAGE_ADR_14H, 1, &regValue);
                break;
            case E_PAGE_20:
                errCode  =  bslTDA1997XWriteI2C(unit, (UInt16)CURPAGE_ADR_20H, 1, &regValue);
                break;
            case E_PAGE_21:
                errCode  =  bslTDA1997XWriteI2C(unit, (UInt16)CURPAGE_ADR_21H, 1, &regValue);
                break;
            case E_PAGE_22:
                errCode  =  bslTDA1997XWriteI2C(unit, (UInt16)CURPAGE_ADR_22H, 1, &regValue);
                break;
            case E_PAGE_23:
                errCode  =  bslTDA1997XWriteI2C(unit, (UInt16)CURPAGE_ADR_23H, 1, &regValue);
                break;
            case E_PAGE_30:
                errCode  =  bslTDA1997XWriteI2C(unit, (UInt16)CURPAGE_ADR_30H, 1, &regValue);
                break;
            case E_PAGE_40:
                errCode  =  bslTDA1997XWriteI2C(unit, (UInt16)CURPAGE_ADR_40H, 1, &regValue);
                break;
            case E_PAGE_41:
                errCode  =  bslTDA1997XWriteI2C(unit, (UInt16)CURPAGE_ADR_41H, 1, &regValue);
                break;
            case E_PAGE_42:
                errCode  =  bslTDA1997XWriteI2C(unit, (UInt16)CURPAGE_ADR_42H, 1, &regValue);
                break;
            default:
                errCode  = TMBSL_ERR_BSLHDMIRX_I2C_WRITE;
                break;
            }

            if (errCode != TM_OK)
            {
                WriteLn("Page err");
                return (errCode);
            }

        }

        pSysArgs->slaveAddr      = RxHdmiConfig[unit].uHwAddress;
        pSysArgs->firstRegister  = (UInt8)firstRegister;
        pSysArgs->lenData        = lengthData;
        pSysArgs->pData          = pBuffer;

        /* Call the external I2C read function with its parameters */
        errCode =  (RxHdmiConfig[unit].sysFuncRead) (pSysArgs);

        if (errCode != TM_OK)
        {
            return (TMBSL_ERR_BSLHDMIRX_I2C_READ);
        }

    }
    else /* CEC register */
    {
        pSysArgs->slaveAddr = RxHdmiConfig[unit].uCECAddress;
        /* no page to program*/
        /* write in registers requested through I2C */
        pSysArgs->firstRegister  = (UInt8)firstRegister;
        pSysArgs->lenData        = lengthData;
        pSysArgs->pData          = pBuffer;

        errCode =  (RxHdmiConfig[unit].sysFuncRead) (pSysArgs);

        if (errCode != TM_OK)
        {
            sprintf(Msg,"CEErr: %" TM_ERROR_CODE_FORMAT, errCode);
            WriteLn(Msg);
            return (TMBSL_ERR_BSLHDMIRX_I2C_READ);
        }

    }

    return TM_OK;
}

tmErrorCode_t
tmbslTDA1997XReadI2C
(
    tmUnitSelect_t          unit,
    UInt16                  firstRegister,
    UInt8                   lengthData,
    UInt8                   *pBuffer
)
{
    tmErrorCode_t          errCode = TM_OK;

    if (I2CGuard)
    {
        RETIF(I2CGuard, TMBSL_ERR_BSLHDMIRX_BUSY)
    }
    else
    {
        I2CGuard = True;
    }

    errCode |= bslTDA1997XReadI2C(unit, firstRegister, lengthData, pBuffer);

    I2CGuard = False;
    if (errCode != TM_OK)
    {
        sprintf(Msg,"CEErr: %" TM_ERROR_CODE_FORMAT, errCode);
        WriteLn(Msg);
    }
    return errCode;
}


/*============================================================================*/
/**
    \brief      Create an instance of an HDMI Receiver: initialize the
                driver, identify and reset the receiver device

    \param unit            Receiver unit number
    \param uHwAddress      Device I2C slave address
    \param sysFuncWrite    System function to write I2C
    \param sysFuncRead     System function to read I2C
    \param funcIntCallback Pointer to interrupt callback function.
    \param sysFuncTimer    Pointer to timer function

    \return     The call result:
                - TM_OK: the call was successful
                - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
                  the receiver instance is already initialised
                - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
                  of range
                - TMBSL_ERR_BSLHDMIRX_INIT_FAILED: the unit instance is already
                  initialised
                - TMBSL_ERR_BSLHDMIRX_COMPATIBILITY: the driver is not compatible
                  with the internal device version code
                - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
                  bus
                - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus
                - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
                  inconsistent

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XInit
(
    tmUnitSelect_t              unit,
    UInt8                       uHwAddress,
    UInt8						CECHwAddress,
    ptmbslHdmiRxSysFunc_t       sysFuncWrite,
    ptmbslHdmiRxSysFunc_t       sysFuncRead,
    ptmbslHdmiRxCallbackFunc_t  callbackFunc,
    ptmbslHdmiRxSysFuncTimer_t  sysFuncTimer
)
{
    tmErrorCode_t               errCode = TM_OK;
    UInt8                       regValue;
    UInt8                       pTabRegValue[NBR_REG_INT_FLAGS];

    WriteLn("init");
    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* test the unit is already initialised*/
    RETIF(RxHdmiConfig[unit].initUnit == True ,TMBSL_ERR_BSLHDMIRX_INIT_FAILED)


    /* test function I2C write */
    RETIF(sysFuncWrite == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* test function I2C read */
    RETIF(sysFuncRead == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* test timer function */
    RETIF(sysFuncTimer == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* Save the parameters */
    RxHdmiConfig[unit].initUnit             = True;
    RxHdmiConfig[unit].uHwAddress           = uHwAddress;
    RxHdmiConfig[unit].uCECAddress          = CECHwAddress;
    RxHdmiConfig[unit].sysFuncWrite         = sysFuncWrite;
    RxHdmiConfig[unit].sysFuncRead          = sysFuncRead;
    RxHdmiConfig[unit].callbackFunc         = callbackFunc;
    RxHdmiConfig[unit].sysFuncTimer         = sysFuncTimer;

    /* Initialization of the other parameters */
    RxHdmiConfig[unit].currentPage          = E_PAGE_INVALID;



    /* apply WA to get correct I2C access to Rx HDMI*/
    regValue = (UInt8)E_PAGE_13;

    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)CURPAGE_ADR_00H, 1, &regValue);

    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)


    regValue = (UInt8)0x24;
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)0xe8, 1, &regValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)


    /* Read chip version*/

    /* Get chip configuration */
    errCode = tmbslTDA1997XReadI2C(unit, (UInt16)CMTP_REG10, 2, pTabRegValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)


    /* Fill chipConfiguration structure */
    RxHdmiConfig[unit].chipConfiguration.B_Clk = (tmbslHdmiRxBClk_t)((pTabRegValue[0] >> 6) & 0x01);
    RxHdmiConfig[unit].chipConfiguration.B_Soc = (tmbslHdmiRxBSoc_t)((pTabRegValue[0] >> 5) & 0x01);
    RxHdmiConfig[unit].chipConfiguration.outputSupply = (tmbslHdmiOutSupply)((pTabRegValue[0] >> 1) & 0x01);
    RxHdmiConfig[unit].chipConfiguration.revision = pTabRegValue[1];
    switch ((UInt8)((pTabRegValue[0] >> 4) & 0x03))
    {
    case 0x00:
        RxHdmiConfig[unit].chipConfiguration.configuration = TDA19971_CONFIG_ID;
        if (RxHdmiConfig[unit].chipConfiguration.revision == 0)
        {
            RxHdmiConfig[unit].version = BSLHDMIRX_TDA19971;
        }
        else
        {
            RxHdmiConfig[unit].version = BSLHDMIRX_TDA19971N2;
        }
        break;
    case 0x01:
        RxHdmiConfig[unit].chipConfiguration.configuration = TDA19972_CONFIG_ID;
        RxHdmiConfig[unit].version = BSLHDMIRX_TDA19972;
        break;
    case 0x02:
    case 0x03:
        RxHdmiConfig[unit].chipConfiguration.configuration = TDA19973_CONFIG_ID;
        if (RxHdmiConfig[unit].chipConfiguration.B_Soc == BSLHDMIRX_TMDS_B_SOC)
        {
            if (RxHdmiConfig[unit].chipConfiguration.revision == 0)
            {
                RxHdmiConfig[unit].version = BSLHDMIRX_TDA19973_SOC_IN;
            }
            else
            {
                RxHdmiConfig[unit].version = BSLHDMIRX_TDA19973N2_SOC_IN;
            }
        }
        else
        {
            if (RxHdmiConfig[unit].chipConfiguration.revision == 0)
            {
                RxHdmiConfig[unit].version = BSLHDMIRX_TDA19973_CONNECTOR_IN;
            }
            else
            {
                RxHdmiConfig[unit].version = BSLHDMIRX_TDA19973N2_CONNECTOR_IN;
            }
        }
        break;
    }


    RxHdmiConfig[unit].chipConfiguration.videoPort = (tmbslHdmiVP_t)((pTabRegValue[0] >> 2) & 0x03);
    RxHdmiConfig[unit].chipConfiguration.CECEnabled = (tmbslHdmiCECEnabled_t)((pTabRegValue[0] >> 1) & 0x01);


    /* if N2 version, reset @0xe8 */
    if (RxHdmiConfig[unit].chipConfiguration.revision != 0)
    {
        regValue = (UInt8)0x00;
        errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)0xe8, 1, &regValue);
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
    }


    /* Init the Receiver driver */
    /* init page register to 00 */
    regValue = (UInt8)(E_PAGE_00);
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)CURPAGE_ADR_00H, 1, &regValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
    /* if N2 version, reset compdel_bp as it may generate some small pixel shifts in case of embedded sync/or delay lower than 4  */
    if (RxHdmiConfig[unit].chipConfiguration.revision != 0)
    {
        regValue = (UInt8)0x1F;
        errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)VDP_CTRL, 1, &regValue);
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
    }


    /* Here, the CEC I2C address is not yet correct. We need to take into account possible config setting in SLAVE_ADDR register.*/
    /* However, the Hw I2C address provided by the devLib (and defined in the cfg file) is assumed to already take into account this setting */
    /* So we need to read this register, extract a0 and a1 address bit values and update CEC address*/
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)SLAVE_ADDR, 1, &regValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)
    RxHdmiConfig[unit].uCECAddress += ((regValue >> 4) & 0x3);
    sprintf(Msg,"U: %d CEC: %d",unit, RxHdmiConfig[unit].uCECAddress);
    if (unit==0)
        actualCEC_1_ADDR=RxHdmiConfig[unit].uCECAddress;
#ifndef OSVRHDK
    else if (unit==1)
        actualCEC_2_ADDR=RxHdmiConfig[unit].uCECAddress;
#endif
    WriteLn(Msg);
    /* CEC I2C address is now uptodate */

    /* update page 20 */
    regValue = (UInt8)0x08;
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)HPD_AUTO_CTRL, 1, &regValue);

    if (RxHdmiConfig[unit].chipConfiguration.revision == 0)
    {
        /* update page 13 */
        regValue = (UInt8)0x24;
        errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)MAN_SUS_HDMI_SEL, 1, &regValue);
        regValue = (UInt8)0x08;
        errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)CGU_DEBUG_SEL, 1, &regValue);
    }

    /* update page 13 to reset infoframe at end of SUS*/
    regValue = (UInt8)0x06;
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)SUS_SET_RGB2, 1, &regValue);
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)SUS_SET_RGB3, 1, &regValue);


    /* update page 0 */
    regValue = (UInt8)0x43;
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)RT_MAN_CTRL, 1, &regValue);
    regValue = (UInt8)0xe0;
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)OF_CTRL, 1, &regValue);

    /* CEC page */
    /* enable sync measurement timing */
    regValue = (UInt8)0x04;
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)PWR_CONTROL, 1, &regValue);
    /* adjust CEC clock divider */
    regValue = (UInt8)0x03;
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)OSC_DIVIDER, 1, &regValue);
    regValue = (UInt8)0xA0;
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)EN_OSC_PERIOD_LSB, 1, &regValue);

    regValue = (UInt8)0x54;
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)TIMER_D, 1, &regValue);


#ifndef TMFL_HDMI_OUT
    /* change video port mapping */
    regValue = (UInt8)0xc8;
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)VP35_32_CTRL, 1, &regValue);
    regValue = (UInt8)0xe7;
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)VP31_28_CTRL, 1, &regValue);
    regValue = (UInt8)0x00;
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)VP27_24_CTRL, 1, &regValue);
    regValue = (UInt8)0xe2;
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)VP23_20_CTRL, 1, &regValue);
    regValue = (UInt8)0xe1;
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)VP19_16_CTRL, 1, &regValue);
    regValue = (UInt8)0x00;
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)VP15_12_CTRL, 1, &regValue);
    regValue = (UInt8)0xe5;
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)VP11_8_CTRL, 1, &regValue);
    regValue = (UInt8)0xe4;
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)VP7_4_CTRL, 1, &regValue);
    regValue = (UInt8)0x00;
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)VP3_0_CTRL, 1, &regValue);

#endif

    /* WA enable power switch  - SRAM content is always valid (in case E-MTP is not or bad programmed)*/
#if 0
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)CONTROL, 1, &regValue);
    regValue |= 0x20;
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)CONTROL, 1, &regValue);
#endif
    /* wait during 50 ms */
#if 0
    errCode = RxHdmiConfig[unit].sysFuncTimer(50);
    RETIF(errCode != TM_OK, errCode)
#endif
    /* Read the chip version */
    WriteLn("bef");
    sprintf(Msg,"unit %d",unit);
    WriteLn(Msg);
    errCode = tmbslTDA1997XReadI2C(unit, (UInt16)VERSION, 1, &regValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)
    RETIF(((regValue !=TDA19972A_CORE_ID)&&(regValue !=TDA19972B_CORE_ID)) , TMBSL_ERR_BSLHDMIRX_COMPATIBILITY)
    WriteLn("aft");

    /* Get chip configuration */
    errCode = tmbslTDA1997XReadI2C(unit, (UInt16)CMTP_REG10, 1, &regValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)


    /* Initialization of the common interrupt masks (only VAI_FLAGS6 is different and set above) */
    pTabRegValue[0]     = INTERRUPT_MASK_DEF_TOP;
    pTabRegValue[1]     = INTERRUPT_MASK_DEF_SUS;
    pTabRegValue[2]     = INTERRUPT_MASK_DEF_DDC;
    pTabRegValue[3]     = INTERRUPT_MASK_DEF_RATE;
    pTabRegValue[4]     = INTERRUPT_MASK_DEF_MODE;
    pTabRegValue[5]     = INTERRUPT_MASK_DEF_INFO;
    pTabRegValue[6]     = INTERRUPT_MASK_DEF_AUDIO;
    pTabRegValue[7]     = INTERRUPT_MASK_DEF_HDCP;
    pTabRegValue[8]     = INTERRUPT_MASK_DEF_AFE;

    /* Write the INT_MASKs to init the interrupt masks*/
    errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)INT_MASK_TOP, NBR_REG_INT_FLAGS, &(pTabRegValue[0]) );
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /* Clear all the interrupts */

    pTabRegValue[0]     = INTERRUPT_CLEAR_VAL;
    pTabRegValue[1]     = INTERRUPT_CLEAR_VAL;
    pTabRegValue[2]     = INTERRUPT_CLEAR_VAL;
    pTabRegValue[3]     = INTERRUPT_CLEAR_VAL;
    pTabRegValue[4]     = INTERRUPT_CLEAR_VAL;
    pTabRegValue[5]     = INTERRUPT_CLEAR_VAL;
    pTabRegValue[6]     = INTERRUPT_CLEAR_VAL;
    pTabRegValue[7]     = INTERRUPT_CLEAR_VAL;
    pTabRegValue[8]     = INTERRUPT_CLEAR_VAL;

    /* Clear the interrupt flags */
    errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)INT_FLG_CLR_TOP, NBR_REG_INT_FLAGS, &(pTabRegValue[0]) );
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

#ifdef TMFL_HDMI_OUT
    /* enable HPD and RXS interrupts */
    pTabRegValue[0]		= INTERRUPT_ENA_UTILRXSHPD;
    errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)ENABLE_INTERRUPT, 1, &(pTabRegValue[0]) );
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
#endif


    /* Initialization of the equalizer */
    errCode = bslTDA1997XInitTMDS(unit);
    RETIF(errCode !=TM_OK, errCode)

    /* Disable test pattern */
    pTabRegValue[0]		= 0;
    errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)SERVICE_MODE, 1, &(pTabRegValue[0]) );
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /* Update HDMI INFO CTRL */
    regValue = 0xFF;
    errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)INFO_CTRL, 1, &regValue );
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /* Write HDMI INFO EXCEED value */
    pTabRegValue[0]		= HDMI_INFO_EXCEED;
    errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)INFO_EXCEED, 1, &(pTabRegValue[0]) );
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    if (RxHdmiConfig[unit].chipConfiguration.revision == 0)
    {
        /* Clear HDMI mode flag in BCAPS (WA for N1)*/
        regValue = 0x03;
        errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)CLK_CFG, 1, &regValue );
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
        regValue = 0x01;
        errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)PON_OVR_EN, 1, &regValue );
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
        regValue = 0x01;
        errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)PON_CBIAS, 1, &regValue );
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
        regValue = 0x01;
        errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)PON_PLL, 1, &regValue );
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

        errCode = tmbslTDA1997XReadI2C(unit, (UInt16)MODE_RECOVER_CFG1, 1, &regValue);
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)
        regValue &= ~0x06;
        regValue |= 0x02;
        errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)MODE_RECOVER_CFG1, 1, &regValue );
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

        regValue = 0x00;
        errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)CLK_CFG, 1, &regValue );
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
        regValue = 0x00;
        errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)PON_OVR_EN, 1, &regValue );
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

        errCode = tmbslTDA1997XReadI2C(unit, (UInt16)MODE_RECOVER_CFG1, 1, &regValue);
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)
        regValue &= ~0x06;
        errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)MODE_RECOVER_CFG1, 1, &regValue );
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
        /* End of Clear HDMI mode flag in BCAPS (WA for N1)*/
    }


    return TM_OK;
}

/*============================================================================*/
/**
    \brief Reset an instance of an HDMI receiver. This function is called by
           the device library to completely shut down an external device and
           release all resources that were allocated for it.

    \param Unit            Receiver unit number

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is already initialised
            - TMBSL_ERR_BSLHDMIRX_RESOURCE_NOT_OWNED: the caller does not own
              the resource

******************************************************************************/
tmErrorCode_t
tmbslTDA1997XDeInit
(
    tmUnitSelect_t              unit
)
{

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* check if the unit has been initialized */
    RETIF(RxHdmiConfig[unit].initUnit == False, TMBSL_ERR_BSLHDMIRX_RESOURCE_NOT_OWNED);

    /* Reset the RxHdmiConfig structure */
    RxHdmiConfig[unit].initUnit             = False;
    RxHdmiConfig[unit].currentPage          = E_PAGE_00;
    RxHdmiConfig[unit].uHwAddress           = 0;
    RxHdmiConfig[unit].sysFuncWrite         = 0;
    RxHdmiConfig[unit].sysFuncRead          = 0;
    RxHdmiConfig[unit].callbackFunc         = 0;
    RxHdmiConfig[unit].version              = BSLHDMIRX_TDA_UNKNOWN;

    return TM_OK;
}
/*============================================================================*/
/**
    \brief        Returns the version of the HDMI RX chip identified during
                  initialization of the driver

    \param Version Version of the device

    \return     - TM_OK  : the call was successful
                - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
                  the receiver instance is not initialised
                - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
                  inconsistent

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XGetChipInfo
(
    tmUnitSelect_t        unit,
    tmbslHdmiRxVersion_t  *pVersion
)
{

    /* Test pVersion <> NULL */
    RETIF(pVersion == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    *pVersion = RxHdmiConfig[unit].version;

    return TM_OK;
}
/*============================================================================*/
/**
    \brief        Returns the software version of tmbslTDA1997X

    \param        pVersionSoft :pointer of software version

    \return     - TM_OK  : the call was successful
                - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
                  the receiver instance is not initialised
                - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
                  inconsistent

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XGetSWVersion
(
    tmUnitSelect_t      unit,
    tmSWVersion_t       *pVersionSoft
)
{
    /* Test pVersion <> NULL */
    RETIF(pVersionSoft == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    pVersionSoft->compatibilityNr = COMPATIBILITY_BSL;
    pVersionSoft->majorVersionNr  = MAJOR_VERSION_BSL;
    pVersionSoft->minorVersionNr  = MINOR_VERSION_BSL;

    return TM_OK;
}
/*============================================================================*/
/**
    \brief Make driver handle an incoming interrupt. This function is used by
           application to tell the driver that the TDA1997X sent an interrupt.
           The driver will then check TDA1997X status and callback the
           application with the various events, if necessary.

    \param Unit Receiver unit number

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
                bus

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XHandleInterrupt
(
    tmUnitSelect_t  unit
)
{
    tmErrorCode_t                         errCode = TM_OK;
    UInt8                                 j,maskBit;
    UInt8								  interrupt_flags_top;
#ifdef TMFL_HDMI_OUT
    UInt8								  interrupt_rxshpd;
    UInt8								  rxs_level;
    UInt8								  utilityhpd_level;
    tmbslHdmiRxHotPlug_t                  hpdStatus;  /* HPD status */
    Bool                    sendEdidCallback;
#endif
    UInt8                                 pTabRegValue[INTERRUPT_CLR_FLG_REG_NUM];
    const tmbslHdmiRxIRQSource_t          (*tabIrqSource)[NBR_BIT_INT_FLAGS];
    UInt8                                 interruptStatusReg;
    UInt8								  regValue;

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    tabIrqSource = tabIrqSource72;

#ifdef TMFL_HDMI_OUT
    /* Check RXS or HPD IT */
    errCode = tmbslTDA1997XReadI2C(unit, (UInt16)INTERRUPT, 1, &interrupt_rxshpd);
    RETIF(errCode != TM_OK,errCode)
    /* read RXS level */
    errCode = tmbslTDA1997XReadI2C(unit, (UInt16)STATUS_RX_SENSE, 1, &rxs_level);
    RETIF(errCode != TM_OK,errCode)
    /* read utility and HPD level */
    errCode = tmbslTDA1997XReadI2C(unit, (UInt16)DETECT_5V_HPD, 1, &utilityhpd_level);
    RETIF(errCode != TM_OK,errCode)

    /* keep only RXS, Utility and HPD int information */
    interrupt_rxshpd &= INTERRUPT_MASK_DEF_UTILRXSHPD;

    /* keep only Utility and and HPD out information */
    utilityhpd_level &= INTERRUPT_MASK_DEF_UTILHPD;

    /* clear RXS, Utility and HPD if any */
    errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)INTERRUPT, 1, &interrupt_rxshpd);
    RETIF(errCode != TM_OK,errCode)

    if (interrupt_rxshpd != 0)
    {
        /* call Hdmiout_Edid int management routine */
        tmbslHdmiRxHdmioutHandleRXSHPD(unit, interrupt_rxshpd, (utilityhpd_level | rxs_level), &pTabRegValue[INTERRUPT_CEC_IDX]);

        /* call callbacks */
        maskBit = 0x80U;
        {
            for(j = 0; j < NBR_BIT_INT_FLAGS; j++)
            {
                if (pTabRegValue[INTERRUPT_CEC_IDX] & maskBit)
                {
                    if (tabIrqSource[INTERRUPT_CEC_IDX][j] == BSLHDMIRX_IRQSOURCE_HPD)
                    {
                        x
                        errCode = tmbslHdmiRxHdmioutHotPlugGetStatus(unit,
                                  &hpdStatus, False);
                        if (hpdStatus == HDMIRX_HOTPLUG_INACTIVE)
                        {
                            errCode = tmbslHdmiRxHdmioutClearEdidRequest(unit);
                        }

                    }

                    if((RxHdmiConfig[unit].callbackFunc != Null) && (tabIrqSource[INTERRUPT_CEC_IDX][j] != BSLHDMIRX_IRQSOURCE_NOT_USED))
                    {
                        x
                        /* Callback for all the interrupt sources except "hdmi_lock" and "hdmi_flags" */
                        RxHdmiConfig[unit].callbackFunc(tabIrqSource[INTERRUPT_CEC_IDX][j], 1);
                    }
                }
                maskBit = maskBit >> 1;
            }
        }

    }
#endif


    do
    {
        /* Read interrupt flags top*/
        errCode = tmbslTDA1997XReadI2C(unit, (UInt16)INT_FLG_CLR_TOP, 1, &interrupt_flags_top);
        RETIF(errCode != TM_OK,errCode)
#ifdef SVR_HDMI_VERY_VERBOSE
        static const bool shouldPrint = true;
#elif defined(HDMI_VERBOSE)
        const bool shouldPrint = (interrupt_flags_top != 0);
#endif


#ifdef HDMI_VERBOSE
        if (shouldPrint)
        {
            sprintf(Msg,"Int top %x",interrupt_flags_top);
            WriteLn(Msg);
            if (interrupt_flags_top == 0)
                WriteLn("No activity");
        }
#endif // HDMI_VERBOSE

        /* if no interrupt to handle, exit*/
        RETIF_SILENT(interrupt_flags_top == 0, TM_OK)


        //tmbslTDA1997XReadI2C(unit, (UInt16)ACP_UPDATE,1, &regValue);
        //sprintf(Msg,"Rx Beg %x",regValue);
        //WriteLn(Msg);
        //_delay_ms(1);

//		printf("IT RX begin processing at %d\n", regValue);

        /* clear pTabRegValue */
        for (j=0; j<8 ; j++)
        {
            pTabRegValue[j] = 0;
        }

        /* Read the useful registers in page 13 (CLK_x_STATUS, SUS_STATUS) */
        /* itStatus: */
        /*   - bit 7 to 5: not used   */
        /*   - bit 4: 1 => sus state = 5, 0 => sus state != 5 */
        /*   - bit 3: 1 => clock stable on input D, 0 => clock not stable on input D */
        /*   - bit 2: 1 => clock stable on input C, 0 => clock not stable on input C */
        /*   - bit 1: 1 => clock stable on input B, 0 => clock not stable on input B */
        /*   - bit 0: 1 => clock stable on input A, 0 => clock not stable on input A */
        errCode  = bslTDA1997XReadInterruptStatusRegisters(unit, &interruptStatusReg);
        RETIF(errCode != TM_OK,errCode)

        /* Callback for HDCP interrupt source */
        if (interrupt_flags_top & INTERRUPT_HDCP)
        {

            //WriteLn("$HDCP");
            /* Read interrupt flags registers*/
            errCode = tmbslTDA1997XReadI2C(unit, (UInt16)INT_FLG_CLR_HDCP, 1, &pTabRegValue[INTERRUPT_HDCP_IDX]);
            RETIF(errCode != TM_OK,errCode)
            /* Clear the interrupt flags */
            errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)INT_FLG_CLR_HDCP, 1, &pTabRegValue[INTERRUPT_HDCP_IDX]);
            RETIF(errCode != TM_OK,errCode)

            /* reset MTP in use flag if set */
            if ((RxHdmiConfig[unit].MPTRWInProgress == True) && (pTabRegValue[INTERRUPT_HDCP_IDX] & MASK_MPT_BIT))
            {
                RxHdmiConfig[unit].MPTRWInProgress = False;
            }

            sprintf(Msg,"iHDCP %x",(pTabRegValue[INTERRUPT_HDCP_IDX]));
            WriteLn(Msg);

            /* init maskBit for the first flag of VAI_FLAGSx */
            maskBit = 0x80U;

            {
                for(j = 0; j < NBR_BIT_INT_FLAGS; j++)
                {
                    if (pTabRegValue[INTERRUPT_HDCP_IDX] & maskBit)
                    {
                        if((RxHdmiConfig[unit].callbackFunc != Null) && (tabIrqSource[INTERRUPT_HDCP_IDX][j] != BSLHDMIRX_IRQSOURCE_NOT_USED))
                        {
                            /* Callback for all the interrupt sources except "hdmi_lock" and "hdmi_flags" */
                            //if (j==6) printf("C5 IT detected at %d\n", timer);
                            //if (j==6)
                            //{
                            //tmbslTDA1997XReadI2C(unit, (UInt16)ACP_UPDATE,1, &regValue);
                            //printf("C5 IT detected at %d\n", regValue);
                            //}
                            /* WA REPEATER: mask AUDIO and IF interrupts to avoid IF during authentication */

                            if (j==6)
                            {
                                tmbslTDA1997XReadI2C(unit, (UInt16)INT_MASK_TOP,1, &regValue);
                                regValue &= ~0x30;
                                tmbslTDA1997XWriteI2C(unit, (UInt16)INT_MASK_TOP,1, &regValue);
                                interrupt_flags_top &= ~0x30;
                                //printf("------------ IF BLOCKED\n");
                            }
                            /* TRY WA REPEATER: force SR_AUDIO to avoid IF during authentication */
                            RxHdmiConfig[unit].callbackFunc(tabIrqSource[INTERRUPT_HDCP_IDX][j], 1);
                        }
                    }
                    maskBit = maskBit >> 1;
                }
            }

        }

        /* Callback for RATE interrupt source */
        else if (interrupt_flags_top & INTERRUPT_RATE)
        {

            //WriteLn("$rate");
            /* Read interrupt flags registers*/
            errCode = tmbslTDA1997XReadI2C(unit, (UInt16)INT_FLG_CLR_RATE, 1, &pTabRegValue[INTERRUPT_RATE_IDX]);
            RETIF(errCode != TM_OK,errCode)
            /* Clear the interrupt flags */
            errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)INT_FLG_CLR_RATE, 1, &pTabRegValue[INTERRUPT_RATE_IDX]);
            RETIF(errCode != TM_OK,errCode)
            /* read clock status register again until INT_FLG_CLR_RATE is still 0 after the read to make sure clock status is the last one*/
            regValue = pTabRegValue[INTERRUPT_RATE_IDX];
            while (regValue!=0)
            {
                errCode  = bslTDA1997XReadInterruptStatusRegisters(unit, &interruptStatusReg);
                RETIF(errCode != TM_OK,errCode)
                errCode = tmbslTDA1997XReadI2C(unit, (UInt16)INT_FLG_CLR_RATE, 1, &regValue);
                RETIF(errCode != TM_OK,errCode)
                errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)INT_FLG_CLR_RATE, 1, &regValue);
                RETIF(errCode != TM_OK,errCode)
                pTabRegValue[INTERRUPT_RATE_IDX] |=regValue;
            }

            //sprintf(Msg,"IRate %x",(pTabRegValue[INTERRUPT_RATE_IDX]));
            //WriteLn(Msg);

            /* init maskBit for the first flag of VAI_FLAGSx */
            maskBit = 0x80U;

            for(j = 0; j < NBR_BIT_INT_FLAGS; j++)
            {
                if (pTabRegValue[INTERRUPT_RATE_IDX] & maskBit)
                {
                    /* WORKAROUND START: PR1633 */
#ifdef TMFL_CALIBRATION_OPT
                    if( (RxHdmiConfig[unit].callbackFunc != Null) &&
                            (tabIrqSource[INTERRUPT_RATE_IDX][j] != BSLHDMIRX_IRQSOURCE_NOT_USED) &&
                            (!RxHdmiConfig[unit].susResetBySW) )
#else
                    /* WORKAROUND END: PR1633 */
                    if((RxHdmiConfig[unit].callbackFunc != Null) && (tabIrqSource[INTERRUPT_RATE_IDX][j] != BSLHDMIRX_IRQSOURCE_NOT_USED))
#endif
                    {
                        /* Callback for each activity detection interrupt source in a specific API */
                        errCode  = bslTDA1997XActivityDetectionInterrupts(unit, tabIrqSource[INTERRUPT_RATE_IDX][j], interruptStatusReg);
                        RETIF(errCode != TM_OK,errCode)
                    }
                }
                maskBit = maskBit >> 1;
            }/* end for */
        }

        /* Callback for SUS interrupt source */
        else if (interrupt_flags_top & INTERRUPT_SUS)
        {

            //WriteLn("$sus");
            /* Read interrupt flags registers*/
            errCode = tmbslTDA1997XReadI2C(unit, (UInt16)INT_FLG_CLR_SUS, 1, &pTabRegValue[INTERRUPT_SUS_IDX]);
            RETIF(errCode != TM_OK,errCode)
            /* Clear the interrupt flags */
            errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)INT_FLG_CLR_SUS, 1, &pTabRegValue[INTERRUPT_SUS_IDX]);
            RETIF(errCode != TM_OK,errCode)

            /* reset MTP in use flag if set */
            if ((RxHdmiConfig[unit].MPTRWInProgress == True) && (pTabRegValue[INTERRUPT_SUS_IDX] & MASK_MPT_BIT))
            {
                RxHdmiConfig[unit].MPTRWInProgress = False;
            }

            /* WA audio reset audio FIFO */
            if (pTabRegValue[INTERRUPT_SUS_IDX] & MASK_SUS_END_BIT)
            {
                tmbslTDA1997XReadI2C(unit, (UInt16)HDMI_INFO_RST,1, &regValue);
                regValue |= MASK_SR_FIFO_FIFO_CTRL;
                tmbslTDA1997XWriteI2C(unit, (UInt16)HDMI_INFO_RST,1, &regValue);
                regValue &= ~MASK_SR_FIFO_FIFO_CTRL;
                tmbslTDA1997XWriteI2C(unit, (UInt16)HDMI_INFO_RST,1, &regValue);
                printf("RESET AUDIO SUS_END\n");

                /* reset HDMI flags memory and VSIReceived flag*/
                RxHdmiConfig[unit].hdmiFlagsStatus = 0;
                RxHdmiConfig[unit].VSIReceived = False;

                ///* reset audio ITs */
                //if (interrupt_flags_top & INTERRUPT_AUDIO)
                //{
                //interrupt_flags_top &= ~INTERRUPT_AUDIO;
                ///* Read interrupt flags registers*/
                //errCode = tmbslTDA1997XReadI2C(unit, (UInt16)INT_FLG_CLR_AUDIO, 1, &pTabRegValue[INTERRUPT_AUDIO_IDX]);
                //RETIF(errCode != TM_OK,errCode)
                ///* Clear the interrupt flags */
                //errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)INT_FLG_CLR_AUDIO, 1, &pTabRegValue[INTERRUPT_AUDIO_IDX]);
                //RETIF(errCode != TM_OK,errCode)
                //}


            }

            //sprintf(Msg,"ISus %x",(pTabRegValue[INTERRUPT_SUS_IDX]));
            //WriteLn(Msg);

            /* init maskBit for the first flag of VAI_FLAGSx */
            maskBit = 0x80U;

            {
                for(j = 0; j < NBR_BIT_INT_FLAGS; j++)
                {
                    if (pTabRegValue[INTERRUPT_SUS_IDX] & maskBit)
                    {
                        if((RxHdmiConfig[unit].callbackFunc != Null) && (tabIrqSource[INTERRUPT_SUS_IDX][j] != BSLHDMIRX_IRQSOURCE_NOT_USED))
                        {
                            /* Callback for all the interrupt sources except "hdmi_lock" and "hdmi_flags" */

                            RxHdmiConfig[unit].callbackFunc(tabIrqSource[INTERRUPT_SUS_IDX][j], 1);
                        }
                    }
                    maskBit = maskBit >> 1;
                }
            }
        }

        /* Callback for DDC interrupt source. Currently empty if no Hdmiout as all interrupts are BSLHDMIRX_IRQSOURCE_NOT_USED */
        else if (interrupt_flags_top & INTERRUPT_DDC)
        {

            //WriteLn("$ddc");
            /* Read interrupt flags registers*/
            errCode = tmbslTDA1997XReadI2C(unit, (UInt16)INT_FLG_CLR_DDC, 1, &pTabRegValue[INTERRUPT_DDC_IDX]);
            RETIF(errCode != TM_OK,errCode)
            /* Clear the interrupt flags */
            errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)INT_FLG_CLR_DDC, 1, &pTabRegValue[INTERRUPT_DDC_IDX]);
            RETIF(errCode != TM_OK,errCode)

            /* reset MTP in use flag if set */
            if ((RxHdmiConfig[unit].MPTRWInProgress == True) && (pTabRegValue[INTERRUPT_DDC_IDX] & MASK_MPT_BIT))
            {
                RxHdmiConfig[unit].MPTRWInProgress = False;
            }

#ifdef TMFL_HDMI_OUT
            /* init maskBit for the first flag of VAI_FLAGSx */
            maskBit = 0x80U;

            {
                for(j = 0; j < NBR_BIT_INT_FLAGS; j++)
                {
                    if (pTabRegValue[INTERRUPT_DDC_IDX] & maskBit)
                    {
                        if (tabIrqSource[INTERRUPT_DDC_IDX][j]==BSLHDMIRX_IRQSOURCE_EDID_READ)
                        {
                            tmbslHdmiRxHdmioutEdidBlockAvailable(unit,&sendEdidCallback);
                            if ((sendEdidCallback == True) && (RxHdmiConfig[unit].callbackFunc != Null))
                            {
                                /* Read EDID finished call the callback */
                                RxHdmiConfig[unit].callbackFunc(tabIrqSource[INTERRUPT_DDC_IDX][j], 1);
                            }

                        }
                        else
                        {
                            if((RxHdmiConfig[unit].callbackFunc != Null) && (tabIrqSource[INTERRUPT_DDC_IDX][j] != BSLHDMIRX_IRQSOURCE_NOT_USED))
                            {
                                /* Callback for all the interrupt sources except "hdmi_lock" and "hdmi_flags" */
                                RxHdmiConfig[unit].callbackFunc(tabIrqSource[INTERRUPT_DDC_IDX][j], 1);
                            }
                        }
                    }
                    maskBit = maskBit >> 1;
                }
            }

            hpdStatus = HDMIRX_HOTPLUG_INVALID;

            /* Get Hot Plug status */
            errCode = tmbslHdmiRxHdmioutHotPlugGetStatus(unit,
                      &hpdStatus, True);
            if (errCode != TM_OK) return errCode;

            ///* Has hot plug changed to Active? */
            //if (hpdStatus == HDMIRX_HOTPLUG_ACTIVE)
#endif

        }

        /* Callback for MODE interrupt source, special action for BSLHDMIRX_IRQSOURCE_FLAGS*/
        else if (interrupt_flags_top & INTERRUPT_MODE)
        {

            //WriteLn("$Interrupt mode");
            /* Read interrupt flags registers*/
            errCode = tmbslTDA1997XReadI2C(unit, (UInt16)INT_FLG_CLR_MODE, 1, &pTabRegValue[INTERRUPT_MODE_IDX]);
            RETIF(errCode != TM_OK,errCode)
            /* Clear the interrupt flags */
            errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)INT_FLG_CLR_MODE, 1, &pTabRegValue[INTERRUPT_MODE_IDX]);
            RETIF(errCode != TM_OK,errCode)


            sprintf(Msg,"Imode %x",(pTabRegValue[INTERRUPT_MODE_IDX]));
            WriteLn(Msg);
            /* init maskBit for the first flag of VAI_FLAGSx */
            maskBit = 0x40U;

            {
                for(j = 1; j < NBR_BIT_INT_FLAGS; j++)
                {
                    if (pTabRegValue[INTERRUPT_MODE_IDX] & maskBit)
                    {

                        if((RxHdmiConfig[unit].callbackFunc != Null) && (tabIrqSource[INTERRUPT_MODE_IDX][j] != BSLHDMIRX_IRQSOURCE_NOT_USED))
                        {

                            /* Callback for all the interrupt sources except "hdmi_lock" and "hdmi_flags" */
                            RxHdmiConfig[unit].callbackFunc(tabIrqSource[INTERRUPT_MODE_IDX][j], 1);
                        }
                    }
                    maskBit = maskBit >> 1;
                }
            }

        }

        /* Callback for INFO interrupt source */
        else if (interrupt_flags_top & INTERRUPT_INFO)
        {

            //WriteLn("$info");
            /* Read interrupt flags registers*/
            errCode = tmbslTDA1997XReadI2C(unit, (UInt16)INT_FLG_CLR_INFO, 1, &pTabRegValue[INTERRUPT_INFO_IDX]);
            RETIF(errCode != TM_OK,errCode)
            /* Clear the interrupt flags */
            errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)INT_FLG_CLR_INFO, 1, &pTabRegValue[INTERRUPT_INFO_IDX]);
            RETIF(errCode != TM_OK,errCode)

            //sprintf(Msg,"Iinfo %x",(pTabRegValue[INTERRUPT_INFO_IDX]));
            //WriteLn(Msg);

            /* init maskBit for the first flag of VAI_FLAGSx */
            maskBit = 0x80U;

            {
                for(j = 0; j < NBR_BIT_INT_FLAGS; j++)
                {
                    if (pTabRegValue[INTERRUPT_INFO_IDX] & maskBit)
                    {
                        if((RxHdmiConfig[unit].callbackFunc != Null) && (tabIrqSource[INTERRUPT_INFO_IDX][j] != BSLHDMIRX_IRQSOURCE_NOT_USED))
                        {
                            /* Callback for all the interrupt sources except "hdmi_lock" and "hdmi_flags" */
                            RxHdmiConfig[unit].callbackFunc(tabIrqSource[INTERRUPT_INFO_IDX][j], 1);
                        }
                    }
                    maskBit = maskBit >> 1;
                }
            }

            /* memorize if VSI received */
            if ((pTabRegValue[INTERRUPT_INFO_IDX] & 0x07) != 0x0) RxHdmiConfig[unit].VSIReceived = True;

        }

        /* Callback for AUDIO interrupt source */
        else if (interrupt_flags_top & INTERRUPT_AUDIO)
        {

            //WriteLn("$audio");
            /* Read interrupt flags registers*/
            errCode = tmbslTDA1997XReadI2C(unit, (UInt16)INT_FLG_CLR_AUDIO, 1, &pTabRegValue[INTERRUPT_AUDIO_IDX]);
            RETIF(errCode != TM_OK,errCode)
            /* Clear the interrupt flags */
            errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)INT_FLG_CLR_AUDIO, 1, &pTabRegValue[INTERRUPT_AUDIO_IDX]);
            RETIF(errCode != TM_OK,errCode)

            /* WA audio reset audio FIFO */
            tmbslTDA1997XReadI2C(unit, (UInt16)SUS_STATUS,1, &regValue);

            if ((pTabRegValue[INTERRUPT_AUDIO_IDX] & (MASK_FIFO_ERR | MASK_MUTE_FLAG)) && ((regValue & MASK_SUS_STATE_VALUE) == LAST_STATE_REACHED))
            {
                tmbslTDA1997XReadI2C(unit, (UInt16)HDMI_INFO_RST,1, &regValue);
                regValue |= MASK_SR_FIFO_FIFO_CTRL;
                tmbslTDA1997XWriteI2C(unit, (UInt16)HDMI_INFO_RST,1, &regValue);
                regValue &= ~MASK_SR_FIFO_FIFO_CTRL;
                tmbslTDA1997XWriteI2C(unit, (UInt16)HDMI_INFO_RST,1, &regValue);

                /* in that case reset channel status IT if present */
                pTabRegValue[INTERRUPT_AUDIO_IDX] &= ~(MASK_CHAN_STATUS);
            }

            //sprintf(Msg,"Iaudio %x",(pTabRegValue[INTERRUPT_AUDIO_IDX]));
            //WriteLn(Msg);

            /* init maskBit for the first flag of VAI_FLAGSx */
            maskBit = 0x80U;

            {
                for(j = 0; j < NBR_BIT_INT_FLAGS; j++)
                {
                    if (pTabRegValue[INTERRUPT_AUDIO_IDX] & maskBit)
                    {
                        if((RxHdmiConfig[unit].callbackFunc != Null) && (tabIrqSource[INTERRUPT_AUDIO_IDX][j] != BSLHDMIRX_IRQSOURCE_NOT_USED))
                        {
                            /* Callback for all the interrupt sources except "hdmi_lock" and "hdmi_flags" */
                            RxHdmiConfig[unit].callbackFunc(tabIrqSource[INTERRUPT_AUDIO_IDX][j], 1);
                        }
                    }
                    maskBit = maskBit >> 1;
                }
            }

        }

        /* Callback for AFE interrupt source. Currently empty as all interrupts are BSLHDMIRX_IRQSOURCE_NOT_USED */
        else if (interrupt_flags_top & INTERRUPT_AFE)
        {
            //WriteLn("$AFE");

        }

    }
    while (interrupt_flags_top != 0);

    //tmbslTDA1997XReadI2C(unit, (UInt16)ACP_UPDATE,1, &regValue);
    //sprintf(Msg,"Rx end %x",regValue);
    //WriteLn(Msg);
//
//    tmbslTDA1997XReadI2C(unit, (UInt16)ACP_UPDATE,1, &regValue);
//	printf("IT RX end processing at %d\n", regValue);

    return TM_OK;
}
/*============================================================================*/
/**
    \brief This function allows the application to read an interrupt source

    \param Unit      Receiver unit number
    \param IrqSource Interrupt to be read

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XGetInterruptStatus
(
    tmUnitSelect_t          unit,
    tmbslHdmiRxIRQSource_t  irqSource
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           interruptStatusReg;

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)


    /* Read the useful registers in page 13 (CLK_x_STATUS, SUS_STATUS) */
    /* itStatus: */
    /*   - bit 7 to 5: not used   */
    /*   - bit 4: 1 => sus state = 5, 0 => sus state != 5 */
    /*   - bit 3: 1 => clock stable on input D, 0 => clock not stable on input D */
    /*   - bit 2: 1 => clock stable on input C, 0 => clock not stable on input C */
    /*   - bit 1: 1 => clock stable on input B, 0 => clock not stable on input B */
    /*   - bit 0: 1 => clock stable on input A, 0 => clock not stable on input A */
    errCode  = bslTDA1997XReadInterruptStatusRegisters(unit, &interruptStatusReg);
    RETIF(errCode != TM_OK,errCode)

    if(RxHdmiConfig[unit].callbackFunc != Null)
    {

        switch(irqSource)
        {
        case BSLHDMIRX_IRQSOURCE_RXDA:
        case BSLHDMIRX_IRQSOURCE_RXDB:
            /* WORKAROUND START: PR1633 */
#ifdef TMFL_CALIBRATION_OPT
            if(!RxHdmiConfig[unit].susResetBySW)
            {
#endif
                /* WORKAROUND END: PR1633 */
                /* Callback for the activity detection interrupt source in a specific API */
                errCode  = bslTDA1997XActivityDetectionInterrupts(unit, irqSource, interruptStatusReg);
                RETIF(errCode != TM_OK,errCode)
                /* WORKAROUND START: PR1633 */
#ifdef TMFL_CALIBRATION_OPT
            }
#endif
            /* WORKAROUND END: PR1633 */
            break;

        case BSLHDMIRX_IRQSOURCE_HDMI_LOCK:

            if( (interruptStatusReg & MASK_SUS_STATE_BIT) >> 4 )
            {
                /* WORKAROUND START: PR1633 */
#ifdef TMFL_CALIBRATION_OPT
                errCode = bslTDA1997XForceEqualizerGain(unit);
                RETIF(errCode != TM_OK, errCode)
#endif
                /* WORKAROUND END: PR1633 */
            }
            /* WORKAROUND START: PR1633 */

#ifdef TMFL_CALIBRATION_OPT
            if(!RxHdmiConfig[unit].susResetBySW)
            {
#endif
                /* WORKAROUND END: PR1633 */
                /* Callback for the HDMI_LOCK IRQ source */
                RxHdmiConfig[unit].callbackFunc(irqSource, ( (interruptStatusReg & MASK_SUS_STATE_BIT) >> 4) );
                /* WORKAROUND START: PR1633 */
#ifdef TMFL_CALIBRATION_OPT
            }
#endif
            /* WORKAROUND END: PR1633 */

            break;

        case BSLHDMIRX_IRQSOURCE_FLAGS:
            /* Callback for the HDMI_FLAGS IRQ source */
            RxHdmiConfig[unit].callbackFunc(irqSource, 1);
            break;

        case BSLHDMIRX_IRQSOURCE_DEEP_COLOR_MODE_CHANGED:
            /* Callback for the DEEP_COLOR_MODE_CHANGED IRQ source */
            RxHdmiConfig[unit].callbackFunc(irqSource, 1);
            break;

        case BSLHDMIRX_IRQSOURCE_AUDIO_PACKET:
            /* Callback for the BSLHDMIRX_IRQSOURCE_AUDIO_PACKET IRQ source */
            RxHdmiConfig[unit].callbackFunc(irqSource, 1);
            break;
#ifdef TMFL_HDMI_OUT
        case BSLHDMIRX_IRQSOURCE_EDID_READ:
            /* Callback for the BSLHDMIRX_IRQSOURCE_EDID_READ IRQ source */
            RxHdmiConfig[unit].callbackFunc(irqSource, 1);
            break;
#endif
        default:
            errCode = TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER;
        }
    }

    return errCode;
}
/*============================================================================*/
/**
    \brief This function must be called by BSL client every 50ms. It is used
           by the BSL driver to handle all time repetitive tasks required by
           the receiver. This function makes the BSL driver independent of the
           OS infrastructure (timer allocation).

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XHeartBeat
(
    tmUnitSelect_t  unit
)
{
    tmErrorCode_t   errCode;
    static UInt8    counter = 0;

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Check periodically the audio frequency and VSI present*/
    if(counter % 4 == 0)
    {
        errCode = bslTDA1997XCheckHdmiFlags(unit);
        RETIF(errCode !=TM_OK, errCode)

        errCode = bslTDA1997XCheckAudioFrequency(unit);
        RETIF(errCode !=TM_OK, errCode)

        errCode = bslTDA1997XCheckVSIUpdate(unit);
        RETIF(errCode !=TM_OK, errCode)
    }

    counter++;

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Set an interrupt mask. this function allows the application to
           define which interrupt source will actually trigger an interrupt.
           A value of 1 means that the interrupt source is enabled, 0 is
           disabled.

    \param Unit      Receiver unit number
    \param IrqSource Interrupt to be enabled/disabled
    \param maskValue Interrupt mask value (0 = disabled, 1 = enabled)

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XSetInterruptMask
(
    tmUnitSelect_t          unit,
    tmbslHdmiRxIRQSource_t  irqSource,
    UInt8                   maskValue
)
{
    tmErrorCode_t                         errCode;
    UInt8                                 i,j,maskBit;
    const tmbslHdmiRxIRQSource_t          (*tabIrqSource)[NBR_BIT_INT_FLAGS];
    UInt8                                 find = 0;
    UInt8                                 pTabRegValue[INTERRUPT_CLR_FLG_REG_NUM];

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    tabIrqSource = tabIrqSource72;

    /* Read the interrupt masks */
    errCode = tmbslTDA1997XReadI2C(unit, (UInt16)INT_MASK_SUS, INTERRUPT_CLR_FLG_REG_NUM, &(pTabRegValue[0]) );
    RETIF(errCode != TM_OK,errCode)

    for (i=0; i<INTERRUPT_CLR_FLG_REG_NUM; i++)
    {
        /* init maskBit for the first flag of VAI_FLAGSx */
        maskBit = 0x80U;

        for (j=0; j<NBR_BIT_INT_FLAGS; j++)
        {
            if(tabIrqSource[i][j] == irqSource)
            {
                find = 1;
                if (maskValue == 0)
                {
                    /* Clear the corresponding bit */
                    pTabRegValue[i] &= (UInt8) ~maskBit;
                }
                else
                {
                    /* Set the corresponding bit */
                    pTabRegValue[i] |= (UInt8) maskBit;
                }
            }

            maskBit = maskBit >> 1;
        }/* end for */
    }/* end for */

    RETIF(find == 0, TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)

    /* Write the interrupt masks registers */
    errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)INT_MASK_SUS, INTERRUPT_CLR_FLG_REG_NUM, &(pTabRegValue[0]) );
    RETIF(errCode != TM_OK,errCode)

    return TM_OK;
}



/*============================================================================*/
/**
    \brief Configures the input of the HDMI RX chip. With this function, you can
           (de)activate the color bar generator, choose its format (NTSC/PAL),
           select the source of the sync timing measurement module for mode
           identification and select the physical digital or analog input to be
           processed.

    \param Unit               Receiver unit number
    \param SyncType           Analog or digital sync input       (ignored with TDA1997X)
    \param DigitalSyncMode    Digital sync mode (auto or manual) (ignored by TDA19972 family)
    \param DigitalInputSource Digital input source
    \param AnDigSelect        Analog or digital video input      (ignored with TDA1997X)
    \param AnInputSelect      Analog video input source          (ignored with TDA1997X)
    \param vdpResetMode       VDP reset mode (auto/manual)       (ignored by TDA19972 family)
    \param vdpReset           VDP manual reset

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading from the I2C
              bus

*******************************************************************************/
tmErrorCode_t
tmbslTDA1997XConfigureInput
(
    tmUnitSelect_t                           unit,
    tmbslHdmiRxInputSelSyncType_t            syncSignal,            /* ignored with TDA1997X */
    tmbslHdmiRxInputSelDigitalMode_t         digitalSyncMode,		/* ignored by TDA19972 family */
    tmbslHdmiRxInputSelDigitalSource_t       digitalInput,
    tmbslHdmiRxInputSelVideoSource_t         anDigSelect,           /* ignored with TDA1997X */
    tmbslHdmiRxInputSelAnalogVideoSource_t   anInputSelect,         /* ignored with TDA1997X */
    tmbslHdmiRxInputSelVDPResetMode_t        vdpResetMode,			/* ignored by TDA19972 family */
    tmbslHdmiRxInputSelVDPReset_t            vdpReset
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           regValue;
    UInt8           digitalInputInternalUse = (UInt8) digitalInput;

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* check the consistency between chip version and parameter values */
    RETIF( ( ((RxHdmiConfig[unit].version == BSLHDMIRX_TDA19971)||(RxHdmiConfig[unit].version == BSLHDMIRX_TDA19971N2)) && (digitalInput != BSLHDMIRX_DIGITALINPUT_A)) ||
           (( (RxHdmiConfig[unit].version == BSLHDMIRX_TDA19972) ||
              (RxHdmiConfig[unit].version == BSLHDMIRX_TDA19973_SOC_IN) ||
              (RxHdmiConfig[unit].version == BSLHDMIRX_TDA19973N2_SOC_IN) ||
              (RxHdmiConfig[unit].version == BSLHDMIRX_TDA19973_CONNECTOR_IN) ||
              (RxHdmiConfig[unit].version == BSLHDMIRX_TDA19973N2_CONNECTOR_IN) ) &&
            ((digitalInput == BSLHDMIRX_DIGITALINPUT_C) || (digitalInput == BSLHDMIRX_DIGITALINPUT_D) )), TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED)

    /* Test on the digitalInput parameter (the enums do not correspond to
       the values of the bits) */
    if(digitalInput == BSLHDMIRX_DIGITALINPUT_A)
    {
        digitalInputInternalUse = SELECT_INPUT_A;
    }

    if(digitalInput == BSLHDMIRX_DIGITALINPUT_B)
    {
        digitalInputInternalUse = SELECT_INPUT_B;
    }

    /* read INPUT_SEL register */
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)INPUT_SEL, 1, &regValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

    /* keep loop mode in case of TDA19973 */
    if ((RxHdmiConfig[unit].version == BSLHDMIRX_TDA19973_SOC_IN) ||
            (RxHdmiConfig[unit].version == BSLHDMIRX_TDA19973N2_SOC_IN) ||
            (RxHdmiConfig[unit].version == BSLHDMIRX_TDA19973_CONNECTOR_IN) ||
            (RxHdmiConfig[unit].version == BSLHDMIRX_TDA19973N2_CONNECTOR_IN))
    {
        regValue = regValue & ~MASK_DIG_INPUT_VDPR_FMTR;
    }
    else
    {
        regValue = regValue & ~(MASK_DIG_INPUT_VDPR_FMTR | MASK_HDMIOUTMODE);

    }

    /* update INPUT_SEL according selected input and vdpReset */
    if (vdpReset == BSLHDMIRX_VDP_RESET_ON)
    {
        //regValue = regValue | ((digitalInputInternalUse & MASK_DIG_INPUT) | RESET_VDP | RESET_FTM);
        regValue = regValue | ((digitalInputInternalUse & MASK_DIG_INPUT)  | RESET_FTM);
    }
    else
    {
        regValue = regValue | (digitalInputInternalUse & MASK_DIG_INPUT) ;
    }

    /* Write INPUT_SEL register */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)INPUT_SEL, 1, &regValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /* To avoid some warnings */
    DUMMY_ACCESS(syncSignal);
    DUMMY_ACCESS(anDigSelect);
    DUMMY_ACCESS(anInputSelect);
    DUMMY_ACCESS(digitalSyncMode);
    DUMMY_ACCESS(vdpResetMode);

    return TM_OK;
}


#ifdef TMFL_HDMI_OUT
/*============================================================================*/
/**
    \brief Configures the HDMI out of the HDMI RX chip. This functionnality
    	   is only available on a few Rx devices as the TDA19973.

    \param Unit               Receiver unit number
    \param outmode            Either loop or bypass mode

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED: not supported by the driven
              Rx chip model.
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading from the I2C
              bus

*******************************************************************************/
tmErrorCode_t
tmbslTDA1997XSetHdmiOutMode
(
    tmUnitSelect_t                           unit,
    tmbslHdmiRxHdmiOutMode_t            	 outmode
)
{
    tmErrorCode_t   errCode;
    UInt8           regValue;

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* check the coherence between chip version and parameter values */
    RETIF(((RxHdmiConfig[unit].version != BSLHDMIRX_TDA19973_SOC_IN) &&
           (RxHdmiConfig[unit].version != BSLHDMIRX_TDA19973N2_SOC_IN) &&
           (RxHdmiConfig[unit].version != BSLHDMIRX_TDA19973_CONNECTOR_IN) &&
           (RxHdmiConfig[unit].version != BSLHDMIRX_TDA19973N2_CONNECTOR_IN)),
          TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED)

    /* read INPUT_SEL register */
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)INPUT_SEL, 1, &regValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

    /* keep all but loop mode in case of TDA19973 */
    regValue = regValue & ~MASK_HDMIOUTMODE;

    /* update INPUT_SEL according selected outmode */
    regValue |= (outmode & MASK_HDMIOUTMODE);

    /* TO DO for N2, enable/disable PON in LOOP mode (LOOP/LOOP_PON)*/

    /* Write INPUT_SEL register */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)INPUT_SEL, 1, &regValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /* TO DO and to be discussed with Guillaume*/
    /* EDID management*/

    return TM_OK;
}
#endif

/*============================================================================*/
/**
    \brief Configures the clock pulse delay. If enabled, a delay is added to
           the output clock signal. This is useful to ensure proper data
           sampling by the connected device.

    \param Unit               Receiver unit number
    \param clockPulseDelay    Clock pulse delay (on/off)
    \param delayValue         Clock pulse delay value

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus

*******************************************************************************/
tmErrorCode_t
tmbslTDA1997XSetClockPulseDelay
(
    tmUnitSelect_t                       unit,
    tmbslHdmiRxInputSelClockPulseDelay_t clockPulseDelay,
    UInt8                                delayValue
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           regValue;

#ifdef HDMI_DEBUG
    // SENSICS_ADDITION
    sprintf(Msg,"PD %d Val %d",clockPulseDelay,delayValue);
    WriteLn(Msg);
#endif
    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Test the delayValue value (MAX_VAL_CLK_DEL = 0x07) */
    RETIF(delayValue > MAX_VAL_CLK_DEL, TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)

    /* change delayValue is clockPulseDelay = off*/
    if (clockPulseDelay == BSLHDMIRX_CLOCK_PULSE_DELAY_OFF) delayValue = 0;

    /* clockPulseDelay bit is in the CLKOUT_GEN register => read the value */
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)CLKOUT_CTRL, 1, &regValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

    /* Apply the new value for delayValue */
    regValue = (UInt8) ( (regValue & ~MASK_CLK_DELAY) | (delayValue << 4) );

    /* Write CLKOUT_CTRL register */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)CLKOUT_CTRL, 1, &regValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Configure the pixel clock generator. This function is used to set
           the configuration of CLKOUT, CLKPIX and CLKFOR that are the heartbeat
           of all the pixel processing chain. You can choose the source of each
           clock depending on you output mode (4:2:2, CCIR656, etc.).

    \param Unit              Receiver unit number
    \param ClockOutToggle    Clock out toggle mode (ON/OFF)
    \param ClockOutSelection Clock out source selection
    \param ClockForSelection Clock FOR source selection
    \param ClockPixSelection Clock PIX source selection

    \return The call result:
            - TM_OK  : the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XConfigurePixelClockGenerator
(
    tmUnitSelect_t                     unit,
    tmbslHdmiRxPixClkToggle_t         clockOutToggle,
    tmbslHdmiRxOutputFormat_t         outputFormat,
    tmbslHdmiRxOutClockEdgeMode_t     clockOutEdgeMode
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           regValue;

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* clockOutToggle bit and clockOutSelection are in the CLKOUT_CTRL register => read the value */
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)CLKOUT_CTRL, 1, &regValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

    /* select clockOutSelection value to program in RX device*/
    if (clockOutEdgeMode == BSLHDMIRX_OUT_CLOCK_SINGLE_EDGE) /* single edge */
    {
        switch (outputFormat)
        {
        case BSLHDMIRX_OUTPUT_FORMAT_444:
        case BSLHDMIRX_OUTPUT_FORMAT_422_SMPT:
        case BSLHDMIRX_OUTPUT_FORMAT_422_CCIR_DIV2:
            regValue = (UInt8) ((regValue & ~MASK_CLK_SEL) | PIX_CLOCK );
            break;
        case BSLHDMIRX_OUTPUT_FORMAT_422_CCIR:
            regValue = (UInt8) ((regValue & ~MASK_CLK_SEL) | PIX_CLOCK_X2 );
            break;
        case BSLHDMIRX_OUTPUT_FORMAT_422_SMPT_DIV2:
        case BSLHDMIRX_OUTPUT_FORMAT_422_CCIR_DIV4:
            regValue = (UInt8) ((regValue & ~MASK_CLK_SEL) | PIX_CLOCK_DIV2 );
            break;
        default:
            break;
        }
    }
    else /* dual edge */
    {
        switch (outputFormat)
        {
        case BSLHDMIRX_OUTPUT_FORMAT_444:
        case BSLHDMIRX_OUTPUT_FORMAT_422_SMPT:
        case BSLHDMIRX_OUTPUT_FORMAT_422_CCIR_DIV2:
            regValue = (UInt8) ((regValue & ~MASK_CLK_SEL) | PIX_CLOCK_DIV2 );
            break;
        case BSLHDMIRX_OUTPUT_FORMAT_422_CCIR:
            regValue = (UInt8) ((regValue & ~MASK_CLK_SEL) | PIX_CLOCK );
            break;
        case BSLHDMIRX_OUTPUT_FORMAT_422_SMPT_DIV2:
        case BSLHDMIRX_OUTPUT_FORMAT_422_CCIR_DIV4:
            regValue = (UInt8) ((regValue & ~MASK_CLK_SEL) | PIX_CLOCK_DIV4 );
            break;
        default:
            break;
        }
    }

    /* Update with clockOutToggle */
    regValue = (UInt8) ((regValue & ~MASK_CLK_TOG) | (clockOutToggle >> 5) );

    /* Write CLKOUT_CTRL register */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)CLKOUT_CTRL, 1, &regValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    return TM_OK;
}
/*============================================================================*/
/**
    \brief Get sync timing measurements. This function allow to retrieve all
           the measurements related to sync timings. Depending on the sync
           detection configuration (function "ConfigureSyncDetection"), it will
           measure either from digital or analog input. This is the entry point
           for properly detecting the video mode received at input.

    \param Unit                Receiver unit number
    \param VerticalPeriod      Period of a frame (or two fields) in MCLK cycles
    \param HorizontalPeriod    Period of a line in MCLK cycles
    \param HorizontalSyncWidth Width of the horizontal sync pulse in MCLK cycles
    \param pFormatMeasurements Structure containing parameters for format timing
                               measurements.

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading from the I2C
              bus

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XGetSyncTimings
(
    tmUnitSelect_t  unit,
    UInt32                      *pVerticalPeriod,
    UInt16                      *pHorizontalPeriod,
    UInt16                      *pHorizontalSyncWidth,
    tmbslHdmiRxFormatMeas_t     *pFormatMeasurements
)
{
    tmErrorCode_t   errCode = TM_OK;
    /* To store reg values from start addr 0x19 to end addr 0x1F. */
    UInt8           pTabRegValue[7];

    /* To store reg values from start addr 0x20 to end addr 0x33. */
    UInt8           pData[20];

    /* Test pointers <> NULL */
    RETIF(pVerticalPeriod == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pHorizontalPeriod == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pHorizontalSyncWidth == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pFormatMeasurements == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Read the timing measurement registers */

    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)V_PER_MSB, 7, pTabRegValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

#ifdef SVR_DEBUG_TIMING_MEASUREMENT
    // SENSICS_ADDITION
    sprintf(Msg,"T2 %x %x %x",pTabRegValue[0] & MASK_VPER_MSB,pTabRegValue[1],pTabRegValue[2]);
    WriteLn(Msg);
#endif // SVR_DEBUG_TIMING_MEASUREMENT

    *pVerticalPeriod      = ( ((UInt32)(pTabRegValue[0] & MASK_VPER_MSB) << 16 ) |
                              ((UInt32)(pTabRegValue[1]) <<  8 ) |
                              ((UInt32)(pTabRegValue[2]) ) );

#ifdef SVR_DEBUG_TIMING_MEASUREMENT
    // SENSICS_ADDITION
    sprintf(Msg,"T3 %x %x",pTabRegValue[3] & MASK_HPER_MSB,pTabRegValue[4]);
    WriteLn(Msg);
#endif // SVR_DEBUG_TIMING_MEASUREMENT

    *pHorizontalPeriod    = ( ( (UInt16)(pTabRegValue[3] & MASK_HPER_MSB) <<  8 ) |
                              ( (UInt16)(pTabRegValue[4]) ) );

#ifdef SVR_DEBUG_TIMING_MEASUREMENT
    // SENSICS_ADDITION
    sprintf(Msg,"T4 %x %x",pTabRegValue[5] & MASK_HSWIDTH_MSB,pTabRegValue[6]);
    WriteLn(Msg);
#endif // SVR_DEBUG_TIMING_MEASUREMENT

    *pHorizontalSyncWidth = ( ( (UInt16)(pTabRegValue[5] & MASK_HSWIDTH_MSB) <<  8 ) |
                              ( (UInt16)(pTabRegValue[6]) ) );

#ifdef SVR_DEBUG_TIMING_MEASUREMENT
    // SENSICS_ADDITION
    sprintf(Msg,"V %lx %x HS %x",*pVerticalPeriod,*pHorizontalPeriod,*pHorizontalSyncWidth);
    WriteLn(Msg	);
#endif // SVR_DEBUG_TIMING_MEASUREMENT

    pFormatMeasurements->vsPolarity  = (tmbslHdmiRxFmtMeasVSPol_t)(pTabRegValue[0] & 0x80);
    pFormatMeasurements->hsPolarity  = (tmbslHdmiRxFmtMeasHSPol_t)(pTabRegValue[3] & 0x80);
    pFormatMeasurements->videoFormat = (tmbslHdmiRxFmtMeasInterlaced_t)(pTabRegValue[5] & 0x80);


#ifdef SVR_DEBUG_TIMING_MEASUREMENT
    // SENSICS_ADDITION
    sprintf(Msg,"Vp %x Hp %x Fmt %x",pFormatMeasurements->vsPolarity,
        pFormatMeasurements->hsPolarity,pFormatMeasurements->videoFormat);
    WriteLn(Msg);
#endif // SVR_DEBUG_TIMING_MEASUREMENT

    /* Read the FMT registers */
    errCode = tmbslTDA1997XReadI2C(unit, (UInt16)FMT_H_TOT_MSB, 20, pData);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

    /* Read the FMT_H_TOT_* registers */
    pFormatMeasurements->horizontalTotalPeriod =
        (UInt16)(pData[0] & 0x3F) << 8 | (UInt16)pData[1];

    /* Read the FMT_H_ACT_* registers */
    pFormatMeasurements->horizontalVideoActiveWidth =
        (UInt16)(pData[2] & 0x3F) << 8 | (UInt16)pData[3];

    /* Read the FMT_H_FRONT_* registers */
    pFormatMeasurements->horizontalFrontPorchWidth =
        (UInt16)(pData[4] & 0x3F) << 8 | (UInt16)pData[5];

    /* Read the FMT_H_SYNC_* registers */
    pFormatMeasurements->horizontalSyncWidthPixClk =
        (UInt16)(pData[6] & 0x3F) << 8 | (UInt16)pData[7];

    /* Read the FMT_H_BACK_* registers */
    pFormatMeasurements->horizontalBackPorchWidth =
        (UInt16)(pData[8] & 0x3F) << 8 | (UInt16)pData[9];

    /* Read the FMT_V_TOT_* registers */
    pFormatMeasurements->verticalTotalPeriod =
        (UInt16)(pData[10] & 0x3F) << 8 | (UInt16)pData[11];

    /* Read the FMT_V_ACT_* registers */
    pFormatMeasurements->verticalVideoActiveWidth =
        (UInt16)(pData[12] & 0x3F) << 8 | (UInt16)pData[13];

    /* Read the FMT_V_FRONT_F1 register */
    pFormatMeasurements->verticalFrontPorchWidthF1 = pData[14];

    /* Read the FMT_V_FRONT_F2 register */
    pFormatMeasurements->verticalFrontPorchWidthF2 = pData[15];

    /* Read the FMT_V_SYNC register */
    pFormatMeasurements->verticalSyncWidth = pData[16];

    /* Read the FMT_V_BACK_F1 register */
    pFormatMeasurements->verticalBackPorchWidthF1 = pData[17];

    /* Read the FMT_V_BACK_F2 register */
    pFormatMeasurements->verticalBackPorchWidthF2 = pData[18];

    /* Read the FMT_DE_ACT register */
    pFormatMeasurements->dataEnablePresent =
        (tmbslHdmiRxFmtMeasDEPresent_t)(pData[19] & 0x01);

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Configure color space conversion. With this function, you can change
           any parameter of the color space conversion module located before
           the digital output. You can define each coefficient and offset of
           the conversion matrix or bypass it. Each coefficient is an Int16
           integer that is equal to 4096*Cnp, where Cnp is the actual matrix
           coefficient.

    \param Unit         Receiver unit number
    \param Bypass       Matrix bypass mode (ON/OFF)
    \param Coefficients Matrix coefficients and offsets

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XConfigureColorSpaceConversion
(
    tmUnitSelect_t                          unit,
    tmbslHdmiRxColorSpaceBypass_t           bypass,
    tmbslHdmiRxColorSpaceCoefficients_t     *pCoefficients
)
{
    tmErrorCode_t                           errCode = TM_OK;
    UInt8                                   pTabRegValue[31];
    Int16                                   *pTabCoeff = &(pCoefficients->offInt1);
    UInt8                                   i, j;

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    if (bypass == BSLHDMIRX_COLORSP_MXBYPASS_ON)
    {
        /* Read VDP_CTRL register */
        errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)VDP_CTRL, 1, pTabRegValue);
        RETIF(errCode != TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

        /* Write mat_bp field */
        pTabRegValue[0] = (UInt8) ((pTabRegValue[0]& ~MASK_MAT_BP)|(bypass >> 2));

        /* Write back VDP_CTRL register */
        errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)VDP_CTRL, 1, pTabRegValue);
        RETIF(errCode != TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
    }
    else
    {
        /* Test pCoefficients <> NULL */
        RETIF(pCoefficients == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

        /* Test on the range of the coefficients */
        /* Matrix Offsets at input */
        for(i = 0; i < OFFSET_LOOP_NB; i++)
        {
            for(j = 0; j < MAT_OFFSET_NB; j++)
            {
                RETIF( (pTabCoeff[j] < MIN_VAL_OFFSET) || (pTabCoeff[j] > MAX_VAL_OFFSET), TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)
            }
            pTabCoeff = &(pCoefficients->offOut1);
        }

        pTabCoeff = &(pCoefficients->P11Coef);

        for(i = 0; i < MAT_COEFF_NB; i++)
        {
            RETIF( (pTabCoeff[i] < MIN_VAL_COEFF) || (pTabCoeff[i] > MAX_VAL_COEFF), TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)
        }

        /* Read VDP_CTRL register */
        errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)VDP_CTRL, 1, pTabRegValue);
        RETIF(errCode != TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

        /* MAT_CTRL register value */
        pTabRegValue[0] = (UInt8) ((pTabRegValue[0]& ~MASK_MAT_BP)|(bypass >> 2));


        /* Offset input 1 value */
        pTabRegValue[1] = (UInt8) ( (UInt16)(pCoefficients->offInt1) >> 8 );
        pTabRegValue[2] = (UInt8) (pCoefficients->offInt1 & MASK_MAT_COEFF_LSB);

        /* Offset input 2 value */
        pTabRegValue[3] = (UInt8) ( (UInt16)(pCoefficients->offInt2) >> 8 );
        pTabRegValue[4] = (UInt8) (pCoefficients->offInt2 & MASK_MAT_COEFF_LSB);

        /* Offset input 3 value */
        pTabRegValue[5] = (UInt8) ( (UInt16)(pCoefficients->offInt3) >> 8 );
        pTabRegValue[6] = (UInt8) (pCoefficients->offInt3 & MASK_MAT_COEFF_LSB);

        /* Coefficient (1,1) value */
        pTabRegValue[7] = (UInt8) ( (UInt16)(pCoefficients->P11Coef) >> 8 );
        pTabRegValue[8] = (UInt8) (pCoefficients->P11Coef & MASK_MAT_COEFF_LSB);

        /* Coefficient (1,2) value */
        pTabRegValue[9]  = (UInt8) ( (UInt16)(pCoefficients->P12Coef) >> 8 );
        pTabRegValue[10] = (UInt8) (pCoefficients->P12Coef & MASK_MAT_COEFF_LSB);

        /* Coefficient (1,3) value */
        pTabRegValue[11] = (UInt8) ( (UInt16)(pCoefficients->P13Coef) >> 8 );
        pTabRegValue[12] = (UInt8) (pCoefficients->P13Coef & MASK_MAT_COEFF_LSB);

        /* Coefficient (2,1) value */
        pTabRegValue[13] = (UInt8) ( (UInt16)(pCoefficients->P21Coef) >> 8 );
        pTabRegValue[14] = (UInt8) (pCoefficients->P21Coef & MASK_MAT_COEFF_LSB);

        /* Coefficient (2,2) value */
        pTabRegValue[15] = (UInt8) ( (UInt16)(pCoefficients->P22Coef) >> 8 );
        pTabRegValue[16] = (UInt8) (pCoefficients->P22Coef & MASK_MAT_COEFF_LSB);

        /* Coefficient (2,3) value */
        pTabRegValue[17] = (UInt8) ( (UInt16)(pCoefficients->P23Coef) >> 8 );
        pTabRegValue[18] = (UInt8) (pCoefficients->P23Coef & MASK_MAT_COEFF_LSB);

        /* Coefficient (3,1) value */
        pTabRegValue[19] = (UInt8) ( (UInt16)(pCoefficients->P31Coef) >> 8 );
        pTabRegValue[20] = (UInt8) (pCoefficients->P31Coef & MASK_MAT_COEFF_LSB);

        /* Coefficient (3,2) value */
        pTabRegValue[21] = (UInt8) ( (UInt16)(pCoefficients->P32Coef) >> 8 );
        pTabRegValue[22] = (UInt8) (pCoefficients->P32Coef & MASK_MAT_COEFF_LSB);

        /* Coefficient (3,3) value */
        pTabRegValue[23] = (UInt8) ( (UInt16)(pCoefficients->P33Coef) >> 8 );
        pTabRegValue[24] = (UInt8) (pCoefficients->P33Coef & MASK_MAT_COEFF_LSB);

        /* Offset output 1 value */
        pTabRegValue[25] = (UInt8) ( (UInt16)(pCoefficients->offOut1) >> 8 );
        pTabRegValue[26] = (UInt8) (pCoefficients->offOut1 & MASK_MAT_COEFF_LSB);

        /* Offset output 2 value */
        pTabRegValue[27] = (UInt8) ( (UInt16)(pCoefficients->offOut2) >> 8 );
        pTabRegValue[28] = (UInt8) (pCoefficients->offOut2 & MASK_MAT_COEFF_LSB);

        /* Offset output 3 value */
        pTabRegValue[29] = (UInt8) ( (UInt16)(pCoefficients->offOut3) >> 8 );
        pTabRegValue[30] = (UInt8) (pCoefficients->offOut3 & MASK_MAT_COEFF_LSB);

        /* Write the VDP_CTRL register and all the coefficients */
        errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)VDP_CTRL, 31, pTabRegValue);
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
    }

    return TM_OK;
}
/*============================================================================*/
/**
    \brief Set the pixel and line counters

    \param Unit             Receiver unit number
    \param PixelCountPreset Preset number of the pixel counter
    \param PixelCountNumber Maximum value of line counter before roll-over
    \param LineCountPreset  Preset number of the line counter
    \param LineCountNumber  Maximum value of line counter before roll-over

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XSetPixelAndLineCounters
(
    tmUnitSelect_t unit,
    UInt16         pixelCountPreset,
    UInt16         pixelCountNumber,
    UInt16         lineCountPreset,
    UInt16         lineCountNumber
)
{
    tmErrorCode_t   errCode = TM_OK;


    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    errCode = bslTDA1997XSetPixelAndLineCounters(unit,pixelCountPreset, pixelCountNumber, lineCountPreset, lineCountNumber);

    return errCode;
}

/*============================================================================*/
/**
    \brief Configure the VHRef timing generator. The VHREF generator is
           responsible for rebuilding all horizontal and vertical
           synchronisation and reference signals from its input. With this
           function, you can configure its automatic detection algorithms and
           force (or not) some predefined modes (480i & 576i).

    \param Unit               Receiver unit number
    \param InterlaceDetection VHRef interlace detection method
    \param FrameVsync         VHRef frame vsync detection method
    \param StandardDetection  VHRef standard selection
    \param VrefProg           Vref timings programmation
    \param HrefProg           Href timings programmation
    \param CSyncPol           CSync polarity

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XConfigureVHRef
(
    tmUnitSelect_t                       unit,
    tmbslHdmiRxVhrefInterlaceDetection_t interlaceDetection,
    tmbslHdmiRxVhrefFrameVsync_t         frameVsync,
    tmbslHdmiRxVhrefStandardDetection_t  standardDetection,
    tmbslHdmiRxVhrefVrefProg_t           vrefProg,
    tmbslHdmiRxVhrefHrefProg_t           hrefProg,
    tmbslHdmiRxVhrefCSyncPol_t           cSyncPol /* not used by TDA19972 family */
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           regValue;


    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Read VHREF_CTRL register */
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)VHREF_CTRL, 1, &regValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

    /* VHREF_CTRL register value */
    regValue = (UInt8)( (regValue & MASK_IHS_SEL) | interlaceDetection | frameVsync | standardDetection |
                        vrefProg | hrefProg);

    /* Write VHREF_CTRL register */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)VHREF_CTRL, 1, &regValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    return TM_OK;
}
/*============================================================================*/
/**
    \brief Configure the VHRef timing values. In case the VHREF generator has
           been configured in manual mode by the "ConfigureVHRef" function,
           this function will allow to manually set all horizontal and vertical
           reference values (do define active pixel area) of the generator. It
           also allows setting the frame reference parameters. These values
           typically depend on the video mode that is beeing processed.

    \param Unit        Receiver unit number
    \param VHRefValues Structure containing all reference values

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XSetVHRefValues
(
    tmUnitSelect_t             unit,
    tmbslHdmiRxVHRefValues_t   *pVHRefValues
)
{
    tmErrorCode_t   errCode = TM_OK;

    /* Test pVHRefValues <> NULL */
    RETIF(pVHRefValues == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    errCode = bslTDA1997XSetVHRefValues(unit, pVHRefValues);

    return errCode;
}
/*============================================================================*/
/**
    \brief Configure the VHSync timing values. In case the VHREF generator has
           been configured in manual mode by the "ConfigureVHRef" function,
           this function allows to manually set all horizontal and vertical
           synchronization values of the generator. These values typically
           depend on the video mode that is beeing processed.

    \param Unit         Receiver unit number
    \param VHSyncValues Structure containing all sync values

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XSetVHSyncValues
(
    tmUnitSelect_t            unit,
    tmbslHdmiRxVHSyncValues_t *pVHSyncValues
)
{
    tmErrorCode_t   errCode = TM_OK;

    /* Test pVHSyncValues <> NULL */
    RETIF(pVHSyncValues == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    errCode = bslTDA1997XSetVHSyncValues(unit, pVHSyncValues);

    return errCode;
}

/*============================================================================*/
/**
    \brief Configure the frame detection window. This function defines the
           horizontal area where the VHREF modules consider that receiving a
           VSYNC pulse indicates a new frame. These values typically
           depend on the video mode that is beeing processed.

    \param Unit     Receiver unit number
    \param FdwStart Start position of the frame detection window
    \param FdwEnd   End position of the frame detection window

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XConfigureFrameDetectionWindow
(
    tmUnitSelect_t unit,
    UInt16         fdwStart,
    UInt16         fdwEnd
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           pTabRegValue[4];


    /* Test the unit number */
    RETIF(unit >= MAX_UNIT, TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Test the FdwStart value (KMaxValFdw = 0xFFF) */
    RETIF(fdwStart > MAX_VAL_FDW, TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)

    /* Test the FdwStart value (KMaxValFdw = 0xFFF) */
    RETIF(fdwEnd > MAX_VAL_FDW, TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)


    /* The FdwStart and FdwEnd values must be written on three registers,
       two for the LSB and one for the MSB => using a mask is necessary */

    /* FDW_S_MSB register value */
    pTabRegValue[0] = (UInt8) (fdwStart >> 8);

    /* FDW_S_LSB register value */
    pTabRegValue[1] = (UInt8) (fdwStart & MASK_FDW_LSB);

    /* FDW_E_MSB register value */
    pTabRegValue[2] = (UInt8) (fdwEnd >> 8);

    /* FDW_E_LSB register value */
    pTabRegValue[3] = (UInt8) (fdwEnd & MASK_FDW_LSB);

    /* Write FDW_S_MSB, FDW_S_LSB, FDW_E_MSB and FDW_E_LSB registers */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)FDW_S_MSB, 4, pTabRegValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Get frame measurements. This function is used to retrieve the values
           measured by the VHREF modules (number of lines, pixels, type of
           interlaced mode, etc.). It is typically used after the call of
           function "GetSyncTimings" to check that the programmed video mode is
           the right one. It is useful to discriminate two modes that have very
           close sync timings.

    \param Unit        Receiver unit number
    \param Interlaced  Detected frame type (interlaced or progressive)
    \param LineMatch   Line vs Standard detection state
    \param FrameFormat Detected frame format (525/625)
    \param Lines       Number of measured lines
    \param Pixels      Number of measured pixels

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading from the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XGetFrameMeasurements
(
    tmUnitSelect_t                      unit,
    tmbslHdmiRxAsdMeasureInterlaced_t   *pInterlaced,
    tmbslHdmiRxVhrefAsdLineStandard_t   *pLineMatch,
    tmbslHdmiRxVhrefAsdMeaslin525_t     *pFrameFormat,
    UInt16                              *pLines,
    UInt16                              *pPixels
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           pTabRegValue[5];

    /* Test pointers <> NULL */
    RETIF(pInterlaced == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pLineMatch == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pFrameFormat == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pLines == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pPixels == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Read Out of frame measurement registers */
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)MEASLIN_MSB, 5, pTabRegValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

    *pInterlaced = (tmbslHdmiRxAsdMeasureInterlaced_t)(pTabRegValue[4] & MASK_INTERLACE_TYPE);

    *pLineMatch = (tmbslHdmiRxVhrefAsdLineStandard_t )(pTabRegValue[4] & MASK_LINE_MATCH);

    *pFrameFormat = (tmbslHdmiRxVhrefAsdMeaslin525_t)(pTabRegValue[4] & MASK_FRAME_FORMAT);

    *pLines  = (UInt16)(((UInt16)((pTabRegValue[0]) & MASK_NB_LINE) << 8 ) | pTabRegValue[1]);

    *pPixels = (UInt16)(((UInt16)(pTabRegValue[2] & MASK_NB_PIX) <<  8 ) | pTabRegValue[3]);

    /// SENSICS_ADDITION
	PortraitMode=(*pLines > * pPixels);
    return TM_OK;
}

/*============================================================================*/
/**
    \brief Configure the vertical and horizontal sync delays.

    \param Unit           Receiver unit number
    \param VertSyncDelay  Vertical sync delay
    \param HorSyncDelay   Horizontal sync delay

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading from the I2C
              bus

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XSetVHSyncDelay
(
    tmUnitSelect_t unit,
    UInt8          vertSyncDelay,
    UInt8          horSyncDelay
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           regValue;


    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Test the Vertical Sync delay value (KMaxVsDel = 0x0F) */
    RETIF(vertSyncDelay > MAX_VS_DEL, TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)

    /* Test the Horizontal Sync delay value (KMaxHsDel = 0x0F) */
    RETIF(horSyncDelay > MAX_HS_DEL, TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)

    if ((vertSyncDelay <4) ||(horSyncDelay<4))
    {
        /* Read VDP_CTRL register */
        errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)VDP_CTRL, 1, &regValue);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
        /* disable compdel BP, so activate compdel ...!!!*/
        regValue = ((regValue) & ~0x20);
        /* Write VDP_CTRL register */
        errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)VDP_CTRL, 1, &regValue);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
    }


    /* Read HS_HREF_SEL register*/
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)HS_HREF_SEL, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_READ)

    /* update hs delay */
    regValue = (regValue & MASK_HS_DEL) | (horSyncDelay << 4);

    /* Write Hor Sync_delay*/
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)HS_HREF_SEL, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /* Read VS_VREF_SEL register*/
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)VS_VREF_SEL, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_READ)

    /* update vs delay */
    regValue = (regValue & MASK_VS_DEL) | (vertSyncDelay << 4);

    /* Write Hor Sync_delay*/
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)VS_VREF_SEL, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Set the blanking codes

    \param Unit           Receiver unit number
    \param BlankingCodeGy Blanking code of G/Y channel
    \param BlankingCodeBu Blanking code of B/Cb channel
    \param BlankingCodeRv Blanking code of R/Cr channel

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XSetBlankingCodes
(
    tmUnitSelect_t unit,
    UInt16         blankingCodeGy,
    UInt16         blankingCodeBu,
    UInt16         blankingCodeRv
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           pTabRegValue[6];

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Test the BlankingCodeGy value (KMaxBlkCode = 0x3FF) */
    RETIF(blankingCodeGy > MAX_BLK_CODE, TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)

    /* Test the BlankingCodeBu value (KMaxBlkCode = 0x3FF) */
    RETIF(blankingCodeBu > MAX_BLK_CODE, TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)

    /* Test the BlankingCodeRv value (KMaxBlkCode = 0x3FF) */
    RETIF(blankingCodeRv > MAX_BLK_CODE, TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)

    /* The Blanking Code values must be written on four registers,
       three for the MSB and one for the LSB => using a mask is necessary */

    /* BLK_GY_MSB register value */
    pTabRegValue[0] = (UInt8) (blankingCodeGy >> 8);

    /* BLK_GY_LSB register value */
    pTabRegValue[1] = (UInt8) (blankingCodeGy & MASK_BLK_CODE_LSB);

    /* BLK_BU_MSB register value */
    pTabRegValue[2] = (UInt8) (blankingCodeBu >> 8);

    /* BLK_BU_LSB register value */
    pTabRegValue[3] = (UInt8) (blankingCodeBu & MASK_BLK_CODE_LSB);

    /* BLK_RV_MSB register value */
    pTabRegValue[4] = (UInt8) (blankingCodeRv >> 8);

    /* BLK_RV_LSB register value */
    pTabRegValue[5] = (UInt8) (blankingCodeRv & MASK_BLK_CODE_LSB);

    /* Write BLK_GY_MSB, BLK_GY_LSB, BLK_BU_MSB, BLK_BU_LSB, BLK_RV_MSB and BLK_RV_LSB registers */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)BLK_GY_MSB, 6, pTabRegValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Configure pre-filter. The pre-filter can downsample the incoming
           video from 4:4:4 to 4:2:2. This function allows you to choose the
           downsampling algorithm for each channel or to bypass it.

    \param Unit           Receiver unit number
    \param FilterConfigBu Pre-filter configuration for B/Cb channel
    \param FilterConfigRv Pre-filter configuration for R/Cr channel

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XConfigurePreFilter
(
    tmUnitSelect_t               unit,
    tmbslHdmiRxPreFilterConfig_t filterConfigBu,
    tmbslHdmiRxPreFilterConfig_t filterConfigRv
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           regValue;


    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Update with the new values for filterConfigBu and filterConfigRv parameters */
    regValue = (UInt8) ( (filterConfigBu << 2) |
                         filterConfigRv  );

    /* Write FILTERS_CTRL register */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)FILTERS_CTRL, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    if ((filterConfigBu == 0) && (filterConfigRv == 0))
    {
        /* Write VDP_CTRL register */
        errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)VDP_CTRL, 1, &regValue);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
        /* activate prefilter BP */
        regValue = ((regValue) | 0x2);
        /* Write VDP_CTRL register */
        errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)VDP_CTRL, 1, &regValue);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
    }
    else
    {
        /* Write VDP_CTRL register */
        errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)VDP_CTRL, 1, &regValue);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
        /* disable prefilter BP */
        regValue = ((regValue) & ~0x2);
        /* Write VDP_CTRL register */
        errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)VDP_CTRL, 1, &regValue);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    }

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Configure range control. Range control truncates the incoming video
           data to the given range (ceiling and floor).

    \param unit          Receiver unit number
    \param chromaCeiling Maximum level that can be reached by chroma channels
                         (16 bit value, extra LSB will be ignored)
    \param chromaFloor   Minimum level that can be reached by chroma channels
                         (16 bit value, extra LSB will be ignored)
    \param lumaCeiling   Maximum level that can be reached by luma channels
                         (16 bit value, extra LSB will be ignored)
    \param lumaFloor     Minimum level that can be reached by luma channels
                         (16 bit value, extra LSB will be ignored)

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XConfigureRangeControl
(
    tmUnitSelect_t unit,
    UInt16         chromaCeiling,
    UInt16         chromaFloor,
    UInt16         lumaCeiling,
    UInt16         lumaFloor
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           pTabRegValue[8];


    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* C_CEIL_MSB register value (be careful, the value is 12bit left aligned on a 16bit word)*/
    pTabRegValue[0] = (UInt8)(chromaCeiling >> 12);

    /* C_CEIL_LSB register value */
    pTabRegValue[1] = (UInt8)((chromaCeiling >> 4) & MASK_CEIL_FLOOR_LSB);

    /* C_FLOOR_MSB register value (be careful, the value is 12bit left aligned on a 16bit word)*/
    pTabRegValue[2] = (UInt8)(chromaFloor >> 12);

    /* C_FLOOR_LSB register value */
    pTabRegValue[3] = (UInt8)((chromaFloor >> 4) & MASK_CEIL_FLOOR_LSB);

    /* Y_CEIL_MSB register value (be careful, the value is 12bit left aligned on a 16bit word)*/
    pTabRegValue[4] = (UInt8)(lumaCeiling >> 12);

    /* Y_CEIL_LSB register value */
    pTabRegValue[5] = (UInt8)((lumaCeiling >> 4) & MASK_CEIL_FLOOR_LSB);

    /* Y_FLOOR_MSB register value (be careful, the value is 12bit left aligned on a 16bit word)*/
    pTabRegValue[6] = (UInt8)(lumaFloor >> 12);

    /* Y_FLOOR_LSB register value */
    pTabRegValue[7] = (UInt8)((lumaFloor >> 4) & MASK_CEIL_FLOOR_LSB);

    /* Write C_CEIL, C_FLOOR, Y_CEIL, Y_FLOOR and CEIL_FLOOR_LSB registers */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)C_CEIL_MSB, 8, pTabRegValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Configure video output formatter. This function allows to control all
           the parameters of the output formatter : impedance, video format,
           timing, codes, blanking codes, etc.

    \param Unit          Receiver unit number
    \param OutputControl Output formatter control
    \param VideoPort     Video port configuration
    \param RefOutput     VHRef output configuration
    \param BlankingCode  Blanking codes output configuration
    \param TimingRefs    Timing refs output configuration
    \param OutputFormat  Output format selection

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XConfigureVideoOutput
(
    tmUnitSelect_t                   unit,
    tmbslHdmiRxOutputControl_t       outputControl,
    tmbslHdmiRxOutputVideoPort_t     videoPort,
    tmbslHdmiRxOutputRef_t           refOutput, /* not used on the TDA19972*/
    tmbslHdmiRxOutputBlankingCodes_t blankingCode,
    tmbslHdmiRxOutputTimingRefs_t    timingRefs,
    tmbslHdmiRxOutputFormat_t        outputFormat
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           regValue;


    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* OF_CTRL register value */
    regValue = (UInt8)( outputControl | videoPort |
                        blankingCode | timingRefs |
                        outputFormat);

    /* Write OF_CTRL register */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)OF_CTRL, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /* select bypass or not of the output formatter */
    if ((outputFormat == BSLHDMIRX_OUTPUT_FORMAT_444) &&
            (blankingCode == BSLHDMIRX_OUTPUT_BLANKING_REMOVE) &&
            (timingRefs == BSLHDMIRX_OUTPUT_TIMINGREF_REMOVE))
    {
        /* Read VDP_CTRL register */
        errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)VDP_CTRL, 1, &regValue);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
        /* activate formatter BP (disable formatter, compdel not to be changed...)*/
        regValue = ((regValue) | 0x10);
        /* Write VDP_CTRL register */
        errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)VDP_CTRL, 1, &regValue);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
    }
    else
    {
        /* Read VDP_CTRL register */
        errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)VDP_CTRL, 1, &regValue);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
        /* disable formatter BP and compdel BP, so activate formatter and compdel ...!!!*/
        regValue = ((regValue) & ~0x30);
        /* Write VDP_CTRL register */
        errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)VDP_CTRL, 1, &regValue);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    }

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Configure up/down sampler. This function allows to enable or disable
           the up and downsamplers of the system. This is used to produce a
           different video output format than the input (e.g. 4:2:2 CCIR i.s.o
           4:4:4).

    \param Unit       Receiver unit number
    \param DownConfig Downsampler configuration
    \param UpConfig   Upsampler configuration

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XConfigureUpDownSampler
(
    tmUnitSelect_t           unit,
    tmbslHdmiRxDownSampler_t downConfig,	/* not used in TDA19972 */
    tmbslHdmiRxUpSampler_t   upConfig
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           regValue;


    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Read the PIX_REPEAT register to recover pix_rep bits */
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)PIX_REPEAT, 1, &regValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

    /* Update with the new value for upConfig parameter */
    regValue = (UInt8) ( (regValue & ~MASK_UP_SEL) | (upConfig << 4) );

    /* Write PIX_REPEAT register */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)PIX_REPEAT, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Configure internal VHSync. This function allows to change the sync
           signal used by the VHRef modules to build its reference signals.

    \param Unit  Receiver unit number
    \param VSync Internal VSync selection
    \param HSync Internal HSync selection

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED: function not supported on the
              active chip.

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XConfigureInternalVHSync
(
    tmUnitSelect_t             unit,
    tmbslHdmiRxInternalVSync_t vSync,		/* ignored by TDA19972 */
    tmbslHdmiRxInternalHSync_t hSync
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           regValue;


    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* check the coherence between chip version and parameter values */
    RETIF( (hSync == BSLHDMIRX_INTERNALHSYNC_PLL) ||
           (hSync == BSLHDMIRX_INTERNALHSYNC_SDRS)
           ,TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED)

    /* Read VHREF_CTRL register */
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)VHREF_CTRL, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_READ)

    /* IHS_SEL fiel value */
    regValue = (regValue & ~MASK_IHS_SEL) | (hSync & MASK_IHS_SEL) ;

    /* Write IVSHS_SEL register */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)VHREF_CTRL, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Configure sync output. This function allows choosing the source for
           each sync output (composite, horizontal, vertical). There are various
           possible sources depending on the signal : SDRS, VHREF, HDMI and PLL
           output.

    \param Unit  Receiver unit number
    \param CSSelection Composite sync output selection
    \param VSSelection Vertical sync output selection
    \param HSSelection Horizontal sync output selection

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED: function not supported on the
              active chip.

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XConfigureSyncOutput
(
    tmUnitSelect_t                     unit,
    tmbslHdmiRxSyncOutputCSSelection_t csSelection,		/* ignored by TDA19972 */
    tmbslHdmiRxSyncOutputVSSelection_t vsSelection,
    tmbslHdmiRxSyncOutputHSSelection_t hsSelection
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           regValue;
    UInt8           vsSelInternalUse = (UInt8) vsSelection;
    UInt8           hsSelInternalUse = (UInt8) hsSelection;

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* check the coherence between chip version and parameter values */
    RETIF( ( (vsSelection == BSLHDMIRX_SYNCOUTPUT_VSYNC_SDRS) || (vsSelection == BSLHDMIRX_SYNCOUTPUT_VSYNC_HDMI_FILTERED) ||
             (hsSelection == BSLHDMIRX_SYNCOUTPUT_HSYNC_PLL)  || (hsSelection == BSLHDMIRX_SYNCOUTPUT_HSYNC_SDRS) ||
             (hsSelection == BSLHDMIRX_SYNCOUTPUT_FREF_VHREF) )
           ,TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED)

    switch (vsSelection)
    {
    case BSLHDMIRX_SYNCOUTPUT_VSYNC_VHREF:
        vsSelInternalUse = (UInt8) 0x00;
        break;

    case BSLHDMIRX_SYNCOUTPUT_VREF_VHREF:
        vsSelInternalUse = (UInt8) 0x01;
        break;

    case BSLHDMIRX_SYNCOUTPUT_VSYNC_HDMI:
    default:
        vsSelInternalUse = (UInt8) 0x02;
        break;
    }

    switch (hsSelection)
    {
    case BSLHDMIRX_SYNCOUTPUT_HSYNC_VHREF:
        hsSelInternalUse = (UInt8) 0x00;
        break;

    case BSLHDMIRX_SYNCOUTPUT_HREF_VHREF:
        hsSelInternalUse = (UInt8) 0x01;
        break;

    case BSLHDMIRX_SYNCOUTPUT_HSYNC_HDMI:
    default:
        hsSelInternalUse = (UInt8) 0x02;
        break;
    }

    /*Read VS_VREF_SEL register */
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)VS_VREF_SEL, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_READ)

    regValue = (UInt8)( (regValue & ~MASK_VS_SEL) | (vsSelInternalUse & MASK_VS_SEL) );

    /* Write VS_VREF_SEL register */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)VS_VREF_SEL, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /*Read HS_HREF_SEL register */
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)HS_HREF_SEL, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_READ)

    regValue = (UInt8)( (regValue & ~MASK_HS_SEL) | (hsSelInternalUse & MASK_HS_SEL) );

    /* Write HS_HREF_SEL register */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)HS_HREF_SEL, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Configure output polarity. This function gives control on all output
           signal polarity, allowing adaptation of the TDA1997X to the chips
           processing its output.

    \param Unit                     Receiver unit number
    \param DataEnPolarity           DataEn output polarity
    \param CSyncPolarity            Composite sync output polarity
    \param HSyncPolarity            Horizontal sync output polarity
    \param VSyncPolarity            Vertical sync output polarity
    \param FieldRefPolarity         Field reference output polarity
    \param HRefPolarity             Horizontal reference output polarity
    \param VRefPolarity             Vertical reference output polarity
    \param AutomaticSyncPolarity    Automatic polarity search for HDMI sync (on/off)

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XConfigureOutputPolarity
(
    tmUnitSelect_t                      unit,
    tmbslHdmiRxPolarityDataEnOut_t      dataEnPolarity,
    tmbslHdmiRxPolarityCSyncOut_t       cSyncPolarity,		/* no used by the TDA19972 */
    tmbslHdmiRxPolarityHSyncOut_t       hSyncPolarity,
    tmbslHdmiRxPolarityVSyncOut_t       vSyncPolarity,
    tmbslHdmiRxPolarityFieldrefOut_t    fieldRefPolarity,	/* no used by the TDA19972 */
    tmbslHdmiRxPolarityHRefOut_t        hRefPolarity,		/* no used by the TDA19972 */
    tmbslHdmiRxPolarityVRefOut_t        vRefPolarity,		/* no used by the TDA19972 */
    tmbslHdmiRxAutomaticSyncPolarity_t  hsVsSelection		/* no used by the TDA19972 */
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           regValue;
    UInt8           dePolInternalUse = (UInt8) dataEnPolarity;
    UInt8           hsPolInternalUse = (UInt8) hSyncPolarity;
    UInt8           vsPolInternalUse = (UInt8) vSyncPolarity;


    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    switch (dataEnPolarity)
    {
    case BSLHDMIRX_POLARITY_DATAENOUT_NEGATIVE:
        dePolInternalUse = (UInt8) MASK_POL;
        break;

    case BSLHDMIRX_POLARITY_DATAENOUT_POSITIVE:
    default:
        dePolInternalUse = (UInt8) 0x00;
        break;
    }

    switch (hSyncPolarity)
    {
    case BSLHDMIRX_POLARITY_HSYNCOUT_NEGATIVE:
        hsPolInternalUse = (UInt8) MASK_POL;
        break;

    case BSLHDMIRX_POLARITY_HSYNCOUT_POSITIVE:
    default:
        hsPolInternalUse = (UInt8) 0x00;
        break;
    }

    switch (vSyncPolarity)
    {
    case BSLHDMIRX_POLARITY_VSYNCOUT_NEGATIVE:
        vsPolInternalUse = (UInt8) MASK_POL;
        break;

    case BSLHDMIRX_POLARITY_VSYNCOUT_POSITIVE:
    default:
        vsPolInternalUse = (UInt8) 0x00;
        break;
    }

    /*Read DE_FREF_SEL register */
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)DE_FREF_SEL, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_READ)

    regValue = (UInt8)( (regValue & ~MASK_POL) | dePolInternalUse );

    /* Write VS_VREF_SEL register */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)DE_FREF_SEL, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /*Read HS_HREF_SEL register */
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)HS_HREF_SEL, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_READ)

    regValue = (UInt8)( (regValue & ~MASK_POL) | hsPolInternalUse );

    /* Write HS_HREF_SEL register */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)HS_HREF_SEL, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /*Read VS_VREF_SEL register */
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)VS_VREF_SEL, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_READ)

    regValue = (UInt8)( (regValue & ~MASK_POL) | vsPolInternalUse );

    /* Write VS_VREF_SEL register */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)VS_VREF_SEL, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Configure video port output. Video ports are located after the
           output formatter and this function allows to configure each of them
           to output the desired video format (YUV, RGB, etc.).

    \param Unit                Receiver unit number
    \param VideoPortFormat     Video port output format
    \param VideoPortASelection Video port A selection
    \param VideoPortBSelection Video port B selection
    \param VideoPortCSelection Video port C selection

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XConfigureVideoPort
(
    tmUnitSelect_t                  unit,
    tmbslHdmiRxVideoPortFormat_t    videoPortFormat,
    tmbslHdmiRxVideoPortSelection_t videoPortASelection,
    tmbslHdmiRxVideoPortSelection_t videoPortBSelection,
    tmbslHdmiRxVideoPortSelection_t videoPortCSelection
)

/* Legacy function to help switching from a TDA19978 to a TDA 19971
 * Assumption:
 * - Only TDA19971 in 24bit configuration is supported (no 72 planned yet, and 73 not considered as a replacement for the 77/78
 * - VideoPortFormat = 10bit, not supported as 3*10bit not supported by the 71 and other YUV 10 bits configuration are
 * either not achievable by the 71 (2*10bits) or equivalent to 12 bits (1*10bits)
 * - If videoPortFormat is 12bit, at least one of the VideoPort must be set to HighImpedance
 */
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           regValue;
    /* VP to Component table. For each value of tmbslHdmiRxVideoPortSelection_t,
     * give the values to program in in three VPxx_xx_CTRL registers
     */
    /*UInt8			VP_Comp_8[4][3] = {	{0x82, 0x81, 0x40},
    									{0x85, 0x84, 0x40},
    									{0x88, 0x87, 0x40},
    									{0x40, 0x40, 0x40} };*/
    UInt8			VP_Comp_8[4][3] = {	{0xe2, 0xe1, 0x00},
        {0xe5, 0xe4, 0x00},
        {0xc8, 0xe7, 0x00},
        {0x00, 0x00, 0x00}
    };
    UInt8			VP_Comp_12[4][4]= { {0x82, 0x81, 0x40, 0x80},
        {0x40, 0x40, 0x40, 0x40},
        {0x88, 0x40, 0x87, 0x86},
        {0x40, 0x40, 0x40, 0x40}
    };


    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Test chip is TDA19971 24bits*/
    RETIF((RxHdmiConfig[unit].version != BSLHDMIRX_TDA19971) ||(RxHdmiConfig[unit].chipConfiguration.videoPort != BSLHDMIRX_TMDS_VP_24),
          TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED)

    /* Test videoPortFormat is not 10bit*/
    RETIF(videoPortFormat == BSLHDMIRX_VIDEOPORT_10BITS, TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED)

    /* If videoPortFormat is 12bit, check one of VP is HiZ*/
    RETIF((videoPortFormat == BSLHDMIRX_VIDEOPORT_12BITS) &&
          (videoPortASelection != BSLHDMIRX_VIDEOPORT_HIGHIMPEDANCE) &&
          (videoPortBSelection != BSLHDMIRX_VIDEOPORT_HIGHIMPEDANCE) &&
          (videoPortCSelection != BSLHDMIRX_VIDEOPORT_HIGHIMPEDANCE),
          TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED)


    if (videoPortFormat == BSLHDMIRX_VIDEOPORT_8BITS)
    {
        /*RGB444 or YUV 444*/
        /* on TDA19978, VPA is VP[6..13], VPB is VP[14..21] and VPC is VP[22..29]
         * keep the same mapping on a TDA19971 */
        /* Write VP_CTRL registers */
        /* VPC */
        errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)VP35_32_CTRL, 3, VP_Comp_8[videoPortCSelection]);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
        /* VPB */
        errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)VP23_20_CTRL, 3, VP_Comp_8[videoPortBSelection]);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
        /* VPA */
        errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)VP11_8_CTRL, 3, VP_Comp_8[videoPortASelection]);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)



    }
    else /* BSLHDMIRX_VIDEOPORT_12BITS as BSLHDMIRX_VIDEOPORT_10BITS has been rejected before */
    {
        /* YUV 422 SMP or CCIR 12 bits. In that case VPA must be BSLHDMIRX_VIDEOPORT_HIGHIMPEDANCE, VPB and VPC are to be programmed,
         * bits 0..3 of the VP are HiZ
         */
        RETIF(videoPortASelection != BSLHDMIRX_VIDEOPORT_HIGHIMPEDANCE, TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)
        /* VPC */
        errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)VP35_32_CTRL, 4, VP_Comp_12[videoPortCSelection]);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
        /* VPB */
        errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)VP19_16_CTRL, 4, VP_Comp_12[videoPortCSelection]);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
        /* VP0..3 */
        regValue = 0x40;
        errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)VP3_0_CTRL, 1, &regValue);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)


    }

    return errCode;
}

/*============================================================================*/
/**
    \brief Define video port output mapping. For each 4bit (or 2bit in case of 30bit video port) video port output pin group,
    choose the associated color 4bit group .

    \param Unit                Receiver unit number
    \param VideoPortFormat     Video port output format
    \param VideoPortASelection Video port A selection
    \param VideoPortBSelection Video port B selection
    \param VideoPortCSelection Video port C selection

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XDefineVideoPort
(
    tmUnitSelect_t                  unit,
    tmbslHdmiRxVideoPortPinGroup_t    videoPortPinGroup,
    tmbslHdmiRxVideoColorQuartet_t    videoColor
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt16 vp_ctrl_register = 0;

    /* check VideoPortPinGroup against Video port size */
    if ((((videoPortPinGroup & VP_MASK) == VP36BIT) && (RxHdmiConfig[unit].chipConfiguration.videoPort != BSLHDMIRX_TMDS_VP_36)) ||
            (((videoPortPinGroup & VP_MASK) == VP30BIT) && (RxHdmiConfig[unit].chipConfiguration.videoPort != BSLHDMIRX_TMDS_VP_30)) ||
            (((videoPortPinGroup & VP_MASK) == VP24BIT) && (RxHdmiConfig[unit].chipConfiguration.videoPort != BSLHDMIRX_TMDS_VP_24)) )
        errCode =  TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER;

    /* select the register to program */

    switch (videoPortPinGroup)
    {
    case BSLHDMIRX_VP36_G4_3_0:
    case BSLHDMIRX_VP30_G2_1_0:
        vp_ctrl_register = VP3_0_CTRL;
        break;

    case BSLHDMIRX_VP36_G4_7_4:
    case BSLHDMIRX_VP30_G4_5_2:
    case BSLHDMIRX_VP24_G4_3_0:
        vp_ctrl_register = VP7_4_CTRL;
        break;

    case BSLHDMIRX_VP36_G4_11_8:
    case BSLHDMIRX_VP30_G4_9_6:
    case BSLHDMIRX_VP24_G4_7_4:
        vp_ctrl_register = VP11_8_CTRL;
        break;

    case BSLHDMIRX_VP36_G4_15_12:
    case BSLHDMIRX_VP30_G2_11_10:
        vp_ctrl_register = VP15_12_CTRL;
        break;

    case BSLHDMIRX_VP36_G4_19_16:
    case BSLHDMIRX_VP30_G4_15_12:
    case BSLHDMIRX_VP24_G4_11_8:
        vp_ctrl_register = VP19_16_CTRL;
        break;

    case BSLHDMIRX_VP36_G4_23_20:
    case BSLHDMIRX_VP30_G4_19_16:
    case BSLHDMIRX_VP24_G4_15_12:
        vp_ctrl_register = VP23_20_CTRL;
        break;

    case BSLHDMIRX_VP36_G4_27_24:
    case BSLHDMIRX_VP30_G2_21_20:
        vp_ctrl_register = VP27_24_CTRL;
        break;

    case BSLHDMIRX_VP36_G4_31_28:
    case BSLHDMIRX_VP30_G4_25_22:
    case BSLHDMIRX_VP24_G4_19_16:
        vp_ctrl_register = VP31_28_CTRL;
        break;

    case BSLHDMIRX_VP36_G4_35_32:
    case BSLHDMIRX_VP30_G4_29_26:
    case BSLHDMIRX_VP24_G4_23_20:
        vp_ctrl_register = VP35_32_CTRL;
        break;

    default:
        errCode |=  TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER;
        break;
    }

    /* write the value to register*/
    errCode |=  tmbslTDA1997XWriteI2C(unit, vp_ctrl_register, 1, (UInt8 *)&videoColor);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)


    return (errCode);
}

/*============================================================================*/
/**
    \brief Configure data enable signal. This function allows a specific control
           on the data enable output signal. You can expand it to include
           SAV/EAV codes, choose to set it to low during specific transitions,
           etc.

    \param Unit             Receiver unit number
    \param DataEnHrefExpand Data enable HRef pixel expansion
    \param DataEnHRefForce  Data enable HRef force to low
    \param DataEnExpand     Data enable pixel expansion
    \param DataEnSelection  Data enable signal selection
    \param DataEnDelay      Data enable signal delay

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XConfigureDataEnable
(
    tmUnitSelect_t                  unit,
    tmbslHdmiRxDataEnHRefExpand_t   dataEnHrefExpand,
    tmbslHdmiRxDataEnHRefForceLow_t dataEnHRefForce,		/* not used by TDA19972 */
    tmbslHdmiRxDataEnExpand_t       dataEnExpand,
    tmbslHdmiRxDataEnSelection_t    dataEnSelection,
    UInt8                           dataEnDelay
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           regValue;


    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Test the DataEnDelay value (KMaxValDEDel = 0x0F) */
    RETIF(dataEnDelay > MAX_VAL_DE_DEL, TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)

    /* change value of dataEnSelection */
    switch (dataEnSelection)
    {
    case BSLHDMIRX_DATAEN_SELECTION_HDMI:
        dataEnSelection = 0x02;
        break;
    case BSLHDMIRX_DATAEN_FREF_VHREF:
        dataEnSelection = 0x01;
        break;
    case BSLHDMIRX_DATAEN_SELECTION_VHREF:
    default:
        dataEnSelection = 0x00;
        break;
    }

    /*Read HS_HREF_SEL register */
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)HS_HREF_SEL, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_READ)

    regValue = (UInt8)( (regValue & ~MASK_PXQ) | (dataEnHrefExpand >> 4) );

    /* Write HS_HREF_SEL register */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)HS_HREF_SEL, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /*Read DE_FREF_SEL register */
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)DE_FREF_SEL, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_READ)

    regValue = (UInt8)( (regValue & MASK_POL) | (dataEnDelay << 4) | (dataEnExpand >> 2) | dataEnSelection );

    /* Write DE_FREF_SEL register */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)DE_FREF_SEL, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Configure HDMI. This function allows the configuration of the high
           level parameters of the digital part of the chip, including HDCP
           module : Mute, encryption mode, protocol, etc.

    \param Unit              Receiver unit number
    \param HdmiMute          HDMI mute mode
    \param HdcpMode          HDCP encryption type
    \param HdmiProtocol      HDMI protocol type
    \param VsyncPolarityMode Vertical sync polarity detection mode
    \param VsyncToggle       Vertical sync toggle

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XConfigureHDMI
(
    tmUnitSelect_t                 unit,
    tmbslHdmiRxHdmiMute_t          hdmiMute,
    tmbslHdmiRxHdcpMode_t          hdcpMode,
    tmbslHdmiRxHdmiProtocol_t      hdmiProtocol,
    tmbslHdmiRxHdmiVsyncPolarity_t vsyncPolarityMode, 	/* ignored by TDA19972 */
    tmbslHdmiRxHdmiVsyncToggle_t   vsyncToggle			/* ignored by TDA19972 */
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           regValue;


    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* HDMI_CTRL register value
    regValue = (UInt8) (hdmiMute | hdcpMode | hdmiProtocol |
                        vsyncPolarityMode | vsyncToggle);*/

    /* HDMI_CTRL register value */
    regValue = (UInt8) ((hdmiMute | hdcpMode ) >>4);

    /* Write HDMI_CTRL register */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)HDMI_CTRL, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)



    return TM_OK;
}

/*============================================================================*/
/**
    \brief Configure HDMI Clock. This function allows to set the clock source
           of the HDMI part. In normal operation clock is taken from input
           signal. However, it is necessary to take another clock source (MCLK)
           at startup to allow key HDCP key download while no signal is present
           at input.

    \param Unit      Receiver unit number
    \param HdmiClock HDMI clock source

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XConfigureHDMIClock /* not used by TDA19972 */
(
    tmUnitSelect_t          unit,
    tmbslHdmiRxHdmiClock_t  hdmiClock
)
{
    return TM_OK;
}

/*============================================================================*/
/**
    \brief Configure audio formatter. All the parameters concerning audio
           reception and output are handled by this function. You can choose
           the layout of the channels, activate test tone, choose the audio
           output format, etc.

    \param Unit               Receiver unit number
    \param spFlag             SP flag mode (used or ignored by audio FIFO control)
    \param ChannelAssignement Audio channels assignement (See EIA/CEA-861B
                              specification for more details)
    \param LayoutMode         Audio path layout mode
    \param Forcedlayout       Audio path forced layout
    \param FifoLatency        Audio FIFO initial latency
    \param TestTone           Test tone mode
    \param OutputFormat       Audio output format
    \param ForceWordSelect    Force WordSelect output   (ignored with TDA1997X)
    \param ForceAudioPort7    Force audio port 7 output
    \param ForceAudioPort6    Force audio port 6 output
    \param ForceAudioPort5    Force audio port 5 output
    \param ForceAudioPort4    Force audio port 4 output
    \param ForceAudioPort3    Force audio port 3 output
    \param ForceAudioPort2    Force audio port 2 output
    \param ForceAudioPort1    Force audio port 1 output
    \param ForceAudioPort0    Force audio port 0 output
    \param ForceAudioClock    Force audio clock output

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XConfigureAudioFormatter
(
    tmUnitSelect_t                     unit,
    tmbslHdmiRxAudioPathSpFlagMode_t   spFlagMode,
    UInt8                              channelAssignement,
    tmbslHdmiRxAudioPathLayoutMode_t   layoutMode,
    tmbslHdmiRxAudioPathForcedLayout_t forcedLayout,
    UInt8                              fifoLatency,
    tmbslHdmiRxAudioTestTone_t         testTone,
    tmbslHdmiRxAudioOutputFormat_t     outputFormat,
    tmbslHdmiRxAudioOutputForce_t      forceWordSelect,     /* ignored with TDA1997X */
    tmbslHdmiRxAudioOutputForce_t      forceAudioPort7,		/* ignored with TDA19972 */
    tmbslHdmiRxAudioOutputForce_t      forceAudioPort6,		/* ignored with TDA19972 */
    tmbslHdmiRxAudioOutputForce_t      forceAudioPort5,		/* ignored with TDA19972 */
    tmbslHdmiRxAudioOutputForce_t      forceAudioPort4,		/* ignored with TDA19972 */
    tmbslHdmiRxAudioOutputForce_t      forceAudioPort3,		/* ignored with TDA1997X */
    tmbslHdmiRxAudioOutputForce_t      forceAudioPort2,		/* ignored with TDA1997X */
    tmbslHdmiRxAudioOutputForce_t      forceAudioPort1,		/* ignored with TDA1997X */
    tmbslHdmiRxAudioOutputForce_t      forceAudioPort0,		/* ignored with TDA1997X */
    tmbslHdmiRxAudioOutputForce_t      forceAudioClock		/* ignored with TDA1997X */
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           pTabRegValue[3];
    UInt8           regValue;


    /* Test the unit number */
    RETIF(unit >= MAX_UNIT, TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Test the Channel assignement value (KMaxValChlAsgt = 0x1F) */
    RETIF(channelAssignement > MAX_VAL_CH_ASGT, TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)

    /* AUDIO_PATH register value */
    regValue = (UInt8) channelAssignement;

    /* AUDIO_FORM register value
    pTabRegValue[2] = (UInt8) ( (forceAudioPort7 << 7) | (forceAudioPort6 << 6) |
                                (forceAudioPort5 << 5) | (forceAudioPort4 << 4) |
                                (forceAudioPort3 << 3) | (forceAudioPort2 << 2) |
                                (forceAudioPort1 << 1) | forceAudioPort0 );*/

    /* Write AUDIO_PATH*/
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)AUDIO_PATH, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /* AUDIO_LAYOUT register value */
    regValue = (UInt8) ( (spFlagMode >>5) | layoutMode | forcedLayout);

    /* Write AUDIO_LAYOUT*/
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)AUDIO_LAYOUT, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /* FIFO_LATENCY register value */
    regValue = (UInt8) fifoLatency;

    /* Write FIFO_LATENCY_VALUE*/
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)FIFO_LATENCY_VALUE, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)



    /* testTone, outputFormat and forceAudioClock bits are in the AUDIO_SEL register => read the value */
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)AUDIO_SEL, 1, &regValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

    /* Apply the new value for testTone, outputFormat and forceAudioClock parameters */
    regValue &= ~(MASK_TEST_TONE | MASK_I2S_SPDIF);
    regValue |= ((testTone >> 1) | (outputFormat >> 1));

    /* Write AUDIO_SEL register */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)AUDIO_SEL, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)


    /* enable AP: if spFlagMode is BSLHDMIRX_AUDIOPATH_SP_FLAG_IGNORED_BY_FIFO we assume that all AP
     * shall be used (that is the case for HBR, DST and other special audio packets..
     */
    if (spFlagMode == BSLHDMIRX_AUDIOPATH_SP_FLAG_IGNORED_BY_FIFO)
    {
        regValue = 0x0f;
    }
    else
    {

        /* enable AP depending of channel allocation */
        /* AP0 always enabled */
        regValue = 1;
        /* channelAssigmenents greater than 1 need also AP1 */
        if (channelAssignement >=0x01) regValue |= 2;
        /* channelAssigmenents greater than 4 need also AP2 */
        if (channelAssignement >=0x04) regValue |= 4;
        /* channelAssigmenents greater than 12 need also AP3 */
        if (channelAssignement >=0x0C) regValue |= 8;
        /* specific cases where AP1 is not used */
        if ((channelAssignement== 0x04)||
                (channelAssignement== 0x08)||
                (channelAssignement== 0x0C)||
                (channelAssignement== 0x10)||
                (channelAssignement== 0x14)||
                (channelAssignement== 0x18)||
                (channelAssignement== 0x1C)) regValue &= ~2;
        /* specific cases where AP2 is not used */
        if ((channelAssignement>= 0x14) && (channelAssignement<= 0x17)) regValue &= ~4;

    }

    /* enable AP, ACLK and WS if needed */
    if (outputFormat == BSLHDMIRX_AUDIOOUTPUT_I2S)
    {
        regValue |= 0x30;
    }
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)AUDIO_OUT_ENABLE, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /* reset test mode to normal audio frequency automatic selection */
    pTabRegValue[0] = (UInt8) 0x00;
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)TEST_MODE, 1, pTabRegValue);





    /* Set audio frequency to 48KHz for test tone TO BE CHECKED FOR TDA19972*/
    if(testTone == BSLHDMIRX_TESTTONE_ON)
    {
        /* set audio test frequency for 48kHz*/
        pTabRegValue[0] = (UInt8) 0x0B;
        errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)TEST_AUDIO_FREQ, 1, pTabRegValue);

        /* set test mode to bypass audio frequency automatic selection */
        pTabRegValue[0] = (UInt8) 0x03;
        errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)TEST_MODE, 1, pTabRegValue);

    }
    else if(regValue & 0x18)
    {
        /* Reset sw_ncts_en & ncts_en */
        errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)TEST_NCTS_CTRL, 1, pTabRegValue);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_READ)
        pTabRegValue[0] = (UInt8)(pTabRegValue[0] & ~0x03);
        errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)TEST_NCTS_CTRL, 1, pTabRegValue);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
    }

    DUMMY_ACCESS(forceWordSelect);

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Configure pixel repeater. With this function, you can activate the
           pixel repeater and repeat n-times each pixel.

    \param Unit            Receiver unit number
    \param PixelRepetition Number of times of pixel must be repeated (0..9)

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XConfigurePixelRepeater
(
    tmUnitSelect_t unit,
    UInt8          pixelRepetition
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           regValue;


    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Test the Pixel repetition value (KMaxValPixRepeat = 0x09) */
    RETIF(pixelRepetition > MAX_VAL_PIX_REPEAT, TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)

    /* Read PIX_REPEAT register */
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)PIX_REPEAT, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_READ)

    /* PIX_REPEAT register value */
    regValue = (UInt8) ((regValue & ~MASK_PIX_REP) | pixelRepetition);

    /* Write PIX_REPEAT register */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)PIX_REPEAT, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Configure audio selection. This function allows setting a various set
           of parameters related to audio : PLL input selection, I2S resolution,
           audio PLL mode, audio packet mode selection and HBR output mode.

    \param Unit                     Receiver unit number
    \param AudioPacketMode          Audio packet mode selection
    \param PllInputRef              Audio PLL input reference       (ignored with TDA1997X)
    \param I2SResolution            I2S resolution
    \param PllInputSelection        PLL input signal                (ignored with TDA1997X)
    \param outputModeHBR            HBR output mode selection

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XConfigureAudioSelection
(
    tmUnitSelect_t                          unit,
    tmbslHdmiRxAudioPacketMode_t            packetMode,
    tmbslHdmiRxAudioPLLInputRef_t           pllInputRef,        /* ignored with TDA1997X */
    tmbslHdmiRxAudioI2SResolution_t         i2SResolution,
    tmbslHdmiRxAudioPllSelection_t          pllInputSelection,   /* ignored with TDA1997X */
    tmbslHdmiRxHBROutputMode_t              outputModeHBR
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           packetModeInternalUse = (UInt8) packetMode;
    UInt8           regValue;

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* AUDIO_SEL register value */
    if(packetMode == BSLHDMIRX_ONE_BIT_AUDIO_PACKET_MODE)
    {
        packetModeInternalUse = (UInt8) 0x02;
    }

    /* Read the AUDIO_SEL register to recover testTone, outputFormat, forceAudioClock and hw mute bits */
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)AUDIO_SEL, 1, &regValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

    /* Update with the new value for packetMode, i2SResolution and outputModeHBR parameters */
    regValue &= ~(MASK_I2S_16_32 | MASK_HBR_DEMUX | MASK_AUDIO_TYPE);
    regValue |= ((i2SResolution << 2) | outputModeHBR | packetModeInternalUse) ;

    /* Write AUDIO_SEL register */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)AUDIO_SEL, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    DUMMY_ACCESS(pllInputRef);
    DUMMY_ACCESS(pllInputSelection);

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Configure packet type scan. This function is used to specify the
           packet type value of data island packet stored in SPD infoframe
           registers.

    \param Unit           Receiver unit number
    \param PacketTypeScan Packet type value of data island packet stored in SPD
                          infoframe registers

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XConfigurePacketTypeScan
(
    tmUnitSelect_t                  unit,
    tmbslHdmiRxPacketTypeScan_t     packetTypeScan
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           regValue;


    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* PACKET_TYPE_SCAN register value */
    regValue = (UInt8) packetTypeScan;

    /* Write PACKET_TYPE_SCAN register */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)PACKET_TYPE_SCAN, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Enable automatic mute/unmute of audio. Automatic mute is a special
           feature of the receiver that triggers an audio mute when audio FIFO
           is near failure (emptyness or fullness). The only way to un-mute
           the audio is to do a FIFO reset through tmbslTDA1997XHDMISoftReset
           API.

    \param Unit     Receiver unit number
    \param AutoMute Sets the audio mute status

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED: this function is not supported on the
              current configured IC.

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XAudioAutoMute
(
    tmUnitSelect_t              unit,
    tmbslHdmiRxAudioAutoMute_t  autoMute
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           regValue;

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)


    /* autoMute bit is in the AUDIO_SEL register => read the value */
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)AUDIO_SEL, 1, &regValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

    /* Apply the new value for audio mute */
    regValue = (UInt8) ( (regValue & ~MASK_AUDIO_AUTO_MUTE) | (autoMute << 3) );

    /* Write AUDIO_SEL register */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)AUDIO_SEL, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Get HDMI status. This function gives the current status of the HDMI
           subsystem of the chip. It is typically used each time an HDMI status
           change is signalled through the callback registered during
           initialisation of the driver. Application can use
           BSLHDMIRX_HDMISTATUS_* defines to parse the status value.

    \param Unit   Receiver unit number
    \param Status pointer to the structure that will receive HDMI status

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XGetHDMIStatus
(
    tmUnitSelect_t  unit,
    UInt32          *pStatus
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           regValue;

    /* Test pStatus <> NULL */
    RETIF(pStatus == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Read HDMI_FLAGS register */
    errCode = tmbslTDA1997XReadI2C(unit, (UInt16)HDMI_FLAGS, 1, &regValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

    *pStatus = (UInt32)(regValue) << 24;

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Get audio status. This function gives the current status of the audio
           subsystem of the chip. It is typically used each time an audio status
           change is signalled through the callback registered during
           initialisation of the driver. Application can use
           BSLHDMIRX_AUDIO_STATUS_* defines to parse the status value.

    \param Unit   Receiver unit number
    \param Status pointer to the structure that will receive HDMI status

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XGetAudioStatus
(
    tmUnitSelect_t  unit,
    UInt32          *pStatus
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           regValue;

    /* Test pStatus <> NULL */
    RETIF(pStatus == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Read HDMI_FLAGS register */
    errCode = tmbslTDA1997XReadI2C(unit, (UInt16)AUDIO_FLAGS, 1, &regValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

    *pStatus = (UInt32)(regValue) << 24;

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Get audio clock regeneration packets.

    \param Unit    Receiver unit number
    \param AcrpCts Pointer to the integer that will receive the audio clock
                   regeneration cycle time stamp
    \param AcrpN   Pointer to the integer that will receive the audio clock
                   regeneration N divider

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XGetAudioClockRegPacket
(
    tmUnitSelect_t  unit,
    UInt32          *pAcrpCts,
    UInt32          *pAcrpN
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           pTabRegValue[6];

    /* Test pointers <> NULL */
    RETIF(pAcrpCts == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pAcrpN == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Read audio clock regeneration packet register */
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)ACRP_CTS_MSB, 6, pTabRegValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

    *pAcrpCts = (UInt32) (((UInt32)(pTabRegValue[0]) << 16) | ((UInt32)(pTabRegValue[1]) << 8) | pTabRegValue[2]);

    *pAcrpN = (UInt32) (((UInt32)(pTabRegValue[3]) << 16) | ((UInt32)(pTabRegValue[4]) << 8) | pTabRegValue[5]);

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Get data island error correction information. This function retrieves
           statistics about data error correction algorithm (erros per frames
           and distance between two errors).

    \param Unit                Receiver unit number
    \param ErrorPerframe       Pointer to the integer that will receive the
                               number of errors per frame
    \param FramesBetweenErrors Pointer to the integer that will receive the
                               number of frames between two errors

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XGetDataIslandErrorCorrection
(
    tmUnitSelect_t  unit,
    UInt16          *pErrorPerframe,
    UInt16          *pFramesBetweenErrors
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           pTabRegValue[4];

    /* Test pointers <> NULL */
    RETIF(pErrorPerframe == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pFramesBetweenErrors == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)ERR_FR_MSB, 4, pTabRegValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

    *pErrorPerframe       = ((UInt16)(pTabRegValue[0]) << 8) | (UInt16)(pTabRegValue[1]);

    *pFramesBetweenErrors = ((UInt16)(pTabRegValue[2]) << 8) | (UInt16)(pTabRegValue[3]);

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Get delta between write and read pointer of audio FIFO. This function
           is used to check the status of the the audio FIFO. Too close write
           and read pointer is a sign of bad input data rate or
           misconfiguration.

    \param Unit  Receiver unit number
    \param Delta Delta between write and read pointer

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XGetAudioReadWriteDelta
(
    tmUnitSelect_t  unit,
    UInt8           *pDelta
)
{
    tmErrorCode_t   errCode = TM_OK;

    /* Test pDelta <> NULL */
    RETIF(pDelta == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)DELTA_WR_RD_PNTR, 1, pDelta);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Get audio content protection update period. This gives the update
           period of audio content protection packet, measured in vertical
           sync pulses.

    \param Unit      Receiver unit number
    \param AcpUpdate Audio content protection update period measured in vertical
                     sync

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XGetAudioContentProtectionUpdate
(
    tmUnitSelect_t  unit,
    UInt8           *pAcpUpdate
)
{
    tmErrorCode_t   errCode = TM_OK;

    /* Test pAcpUpdate <> NULL */
    RETIF(pAcpUpdate == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)ACP_UPDATE, 1, pAcpUpdate);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Originally, on TD19978, configured OTP memory. On TDA19972, OTP has been replaced
    by a MTP. This function has been kept, but now it manages HDCP MTP.

    \param Unit       Receiver unit number
    \param OtpCommand Command to be send to OTP memory
    \param BchEnable  Enable/disable BCH error correction code.

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus

 ******************************************************************************/
tmErrorCode_t
/*tmbslTDA19972ConfigureOTP*/
tmbslTDA1997XConfigureOTP
(
    tmUnitSelect_t          unit,
    tmbslHdmiRxOtpCommand_t otpCommand,
    tmbslHdmiRxBchEnable_t  bchEnable
)
{

    tmErrorCode_t   errCode = TM_OK;
    UInt8           regValue;
    //UInt8           regMValue[5];

    /* UInt16 				i;*/

    /*test the unit number*/
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    switch (otpCommand)
    {
    case BSLHDMIRX_OTP_NO_COMMAND:
        break;

    case BSLHDMIRX_OTP_START_DOWNLOAD:

        if (RxHdmiConfig[unit].chipConfiguration.revision == 0)
        {
            /* 00 (added) disable termination and enable HDCP block*/
            regValue = (UInt8)0x00;
            errCode |=  tmbslTDA1997XWriteI2C(unit, (UInt16)RT_MAN_CTRL, 1, &regValue);
            regValue = (UInt8)0x24;
            errCode |=  tmbslTDA1997XWriteI2C(unit, (UInt16)MAN_SUS_HDMI_SEL, 1, &regValue);



            /*0. Enable Clock on TMDS PLL by usiong FRO */
            regValue = (UInt8)0x03;
            errCode |=  tmbslTDA1997XWriteI2C(unit, (UInt16)CLK_CFG, 1, &regValue);
            regValue = (UInt8)0x01;
            errCode |=  tmbslTDA1997XWriteI2C(unit, (UInt16)PON_CBIAS, 1, &regValue);
            regValue = (UInt8)0x01;
            errCode |=  tmbslTDA1997XWriteI2C(unit, (UInt16)PON_PLL, 1, &regValue);
            regValue = (UInt8)0x01;
            errCode |=  tmbslTDA1997XWriteI2C(unit, (UInt16)PON_OVR_EN, 1, &regValue);

            regValue = (UInt8)0x00;
            errCode |=  tmbslTDA1997XWriteI2C(unit, (UInt16)CGU_DEBUG_SEL, 1, &regValue);

            /*0bis make a reset of HDCP block (added: enable HDCP block again)*/
            regValue = (UInt8)0x24;
            errCode |=  tmbslTDA1997XWriteI2C(unit, (UInt16)MAN_SUS_HDMI_SEL, 1, &regValue);
            regValue = (UInt8)0x04;
            errCode |=  tmbslTDA1997XWriteI2C(unit, (UInt16)MAN_HDMI_SET, 1, &regValue);
            regValue = (UInt8)0x00;
            errCode |=  tmbslTDA1997XWriteI2C(unit, (UInt16)MAN_HDMI_SET, 1, &regValue);

            /*0ter (added) remove force */
            regValue = (UInt8)0x00;
            errCode |=  tmbslTDA1997XWriteI2C(unit, (UInt16)PON_OVR_EN, 1, &regValue);
            regValue = (UInt8)0x00;
            errCode |=  tmbslTDA1997XWriteI2C(unit, (UInt16)CLK_CFG, 1, &regValue);

            /*1. Copy byte KEY_0(39) (page 40h, offset 0x02) into private_area (page 42h, offset 0x5F)*/

            errCode |=  tmbslTDA1997XReadI2C(unit, (UInt16)MTP_KEY39_LSB, 1, &regValue);
            errCode |= tmbslTDA1997XWriteI2C(unit, (UInt16)MTP_PRIVATE_AREA, 1, &regValue);
        }

        /*1bis Enable HDCP (added: reset HDMI_INFO_RST)*/
        regValue = (UInt8)0x00;
        errCode |=  tmbslTDA1997XWriteI2C(unit, (UInt16)HDMI_INFO_RST, 1, &regValue);
        regValue = (UInt8)0x03;
        errCode |=  tmbslTDA1997XWriteI2C(unit, (UInt16)HDCP_CTRL, 1, &regValue);


        /*2. Clear flag hdcp_dlmtp (register INT_FLG_CLR_HDCP, page 00h, offset 0x15, bit 4)*/
        /*3. Clear flag hdcp_dlram (register INT_FLG_CLR_HDCP, page 00h, offset 0x15, bit 3)*/
        regValue = 0x18;
        errCode |= tmbslTDA1997XWriteI2C(unit, (UInt16)INT_FLG_CLR_HDCP, 1, &regValue);


        /*4. Read Seed_index (page 40h, offset 0x00)
        5. Check Seed value (page 00h, offset 0x61 & 0x62)*/

        /*6. Download KEY into HDCP engine: registre HDCP_KEY_CTRL (page 00h, offset 0x64)*/
        regValue = 0x01;
        errCode |= tmbslTDA1997XWriteI2C(unit, (UInt16)HDCP_KEY_CTRL, 1, &regValue);

        /*7. Check flag hdcp_dlram (register INT_FLG_CLR_HDCP, page 00h, offset 0x15, bit 3)*/
        regValue = 0x00;
        while ((regValue & 0x08) != 0x08)
        {
            errCode |=  tmbslTDA1997XReadI2C(unit, (UInt16)INT_FLG_CLR_HDCP, 1, &regValue);

        }

        /*8. Download MTP into SRAM: hmtp_dl_all (page 13h, offset 0x7A).*/
        regValue = 0x01;
        errCode |= tmbslTDA1997XWriteI2C(unit, (UInt16)HMTP_CTRL, 1, &regValue);

        /*9. Check flag hdcp_dlmtp (register INT_FLG_CLR_HDCP, page 00h, offset 0x15, bit 4)*/
        regValue = 0x00;
        while ((regValue & 0x10) != 0x10)
        {
            errCode |=  tmbslTDA1997XReadI2C(unit, (UInt16)INT_FLG_CLR_HDCP, 1, &regValue);

        }
        if (RxHdmiConfig[unit].chipConfiguration.revision == 0)
        {
            regValue = (UInt8)0x08;
            errCode |=  tmbslTDA1997XWriteI2C(unit, (UInt16)CGU_DEBUG_SEL, 1, &regValue);
        }

        /* 10 clear HDCP interrupt status bits that may have been raised during this process */
        regValue = (UInt8)0x07;
        errCode |=  tmbslTDA1997XWriteI2C(unit, (UInt16)INT_FLG_CLR_HDCP, 1, &regValue);


        if (RxHdmiConfig[unit].chipConfiguration.revision == 0)
        {
            /* added: restore termination */
            regValue = (UInt8)0x03;
            errCode |=  tmbslTDA1997XWriteI2C(unit, (UInt16)RT_MAN_CTRL, 1, &regValue);
        }


        break;

    case BSLHDMIRX_OTP_START_READ:
        /*2. Clear flag hdcp_dlmtp (register INT_FLG_CLR_HDCP, page 00h, offset 0x15, bit 4)*/
        regValue = 0x10;
        errCode |= tmbslTDA1997XWriteI2C(unit, (UInt16)INT_FLG_CLR_HDCP, 1, &regValue);

        /*8. Download MTP into SRAM: hmtp_dl_all (page 13h, offset 0x7A).*/
        regValue = 0x01;
        errCode |= tmbslTDA1997XWriteI2C(unit, (UInt16)HMTP_CTRL, 1, &regValue);

        /*9. Check flag hdcp_dlmtp (register INT_FLG_CLR_HDCP, page 00h, offset 0x15, bit 4)*/
        regValue = 0x00;
        while ((regValue & 0x10) != 0x10)
        {
            errCode |=  tmbslTDA1997XReadI2C(unit, (UInt16)INT_FLG_CLR_HDCP, 1, &regValue);

        }
        break;
    }


    return errCode;
}

/*============================================================================*/
/**
    \brief Set OTP address. This function allows defining at which address of
           the OTP memory will begin the next OTP command.

    \param Unit    Receiver unit number
    \param address 8 bit address

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED: this function is not supported
              on the current configured IC.

 ******************************************************************************/
tmErrorCode_t
/*tmbslTDA19972SetOTPAddress*/
tmbslTDA1997XSetOTPAddress
(
    tmUnitSelect_t  unit,
    UInt8           address
)
{

    FakeOTPAddress = address;

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Read OTP memory. This function allows to read the OTP memory. It is
           used together with "ConfigureOTP" function. The end of the read
           process is indicated by the OTP status byte accessible through
           GetInterruptStatus function.

    \param Unit    Receiver unit number
    \param OtpData Pointer to the integer that will receive the last 32 bits
                   of data read from OTP memory.

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

 ******************************************************************************/
tmErrorCode_t
/*tmbslTDA19972ReadOTP*/
tmbslTDA1997XReadOTP(
    tmUnitSelect_t    unit,
    UInt32            *pOtpData
)
{

    UInt16 FakeReadAddress;
    tmErrorCode_t   errCode = TM_OK;
    UInt8           pTabRegValue[3];

    FakeReadAddress = (0x40 << 8) | FakeOTPAddress;

    /*errCode =  tmbslTDA1997XReadI2C(unit, FakeReadAddress, 3, pTabRegValue);

    *pOtpData = ((UInt32)(pTabRegValue[0]) << 16) | ((UInt32)(pTabRegValue[1]) << 8) |
                 (UInt32)(pTabRegValue[2]);*/

    errCode =  tmbslTDA1997XReadI2C(unit, FakeReadAddress, 1, pTabRegValue);

    *pOtpData = (UInt32)pTabRegValue[0];


    return errCode;
}

/*============================================================================*/
/**
    \brief Configure HDCP. This function sets the basic configuration of the
           HDCP module (enable/disable, key encryption,DDC address, key
           description seed).

    \param Unit               Receiver unit number
    \param DecryptKeys        Key internal decryption (on/off)
    \param HdcpEnable         Enable/disable HDCP function
    \param DdcI2cAddress      Display data channel I2C address
    \param KeyDescriptionSeed Key decryption seed

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XConfigureHDCP
(
    tmUnitSelect_t               unit,
    tmbslHdmiRxHdcpDecryptKey_t  decryptKeys,
    tmbslHdmiRxHdcpEnable_t      hdcpEnable,
    UInt8                        ddcI2cAddress,
    UInt16                       keyDescriptionSeed
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           pTabRegValue[3];
    UInt8			regValue;


    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Write HDCP control */
    pTabRegValue[0] = (UInt8)(decryptKeys | hdcpEnable);

    /* Write keys description seed MSB */
    pTabRegValue[1] = (UInt8)(keyDescriptionSeed >> 8);

    /* Write keys description seed LSB */
    pTabRegValue[2] = (UInt8)(keyDescriptionSeed & MASK_KEYS_DESC);

    if (RxHdmiConfig[unit].chipConfiguration.revision == 0)
    {
        /*0. WA Enable Clock on TMDS PLL by using FRO*/
        regValue = (UInt8)0x03;
        errCode |=  tmbslTDA1997XWriteI2C(unit, (UInt16)CLK_CFG, 1, &regValue);
        regValue = (UInt8)0x01;
        errCode |=  tmbslTDA1997XWriteI2C(unit, (UInt16)PON_CBIAS, 1, &regValue);
        regValue = (UInt8)0x01;
        errCode |=  tmbslTDA1997XWriteI2C(unit, (UInt16)PON_PLL, 1, &regValue);
        regValue = (UInt8)0x01;
        errCode |=  tmbslTDA1997XWriteI2C(unit, (UInt16)PON_OVR_EN, 1, &regValue);
    }


    /* Write HDCP_CTRL register */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)HDCP_CTRL , 3, pTabRegValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /* Write I2C address */
    pTabRegValue[0] = ddcI2cAddress;

    /* Write HDCP_DDC_ADDR register */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)HDCP_DDC_ADDR , 1, pTabRegValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    if (RxHdmiConfig[unit].chipConfiguration.revision == 0)
    {
        /*0. WA Disable force Clock on TMDS PLL by usiong FRO*/
        regValue = (UInt8)0x00;
        errCode |=  tmbslTDA1997XWriteI2C(unit, (UInt16)CLK_CFG, 1, &regValue);
        regValue = (UInt8)0x00;
        errCode |=  tmbslTDA1997XWriteI2C(unit, (UInt16)PON_OVR_EN, 1, &regValue);
        /* WA Restore clock*/
        regValue = (UInt8)0x08;
        errCode |=  tmbslTDA1997XWriteI2C(unit, (UInt16)CGU_DEBUG_SEL, 1, &regValue);

        /* Clear HDMI mode flag in BCAPS (WA for N1)*/
        regValue = 0x03;
        errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)CLK_CFG, 1, &regValue );
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
        regValue = 0x01;
        errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)PON_OVR_EN, 1, &regValue );
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
        regValue = 0x01;
        errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)PON_CBIAS, 1, &regValue );
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
        regValue = 0x01;
        errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)PON_PLL, 1, &regValue );
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

        errCode = tmbslTDA1997XReadI2C(unit, (UInt16)MODE_RECOVER_CFG1, 1, &regValue);
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)
        regValue &= ~0x06;
        regValue |= 0x02;
        errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)MODE_RECOVER_CFG1, 1, &regValue );
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

        regValue = 0x00;
        errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)CLK_CFG, 1, &regValue );
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
        regValue = 0x00;
        errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)PON_OVR_EN, 1, &regValue );
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

        errCode = tmbslTDA1997XReadI2C(unit, (UInt16)MODE_RECOVER_CFG1, 1, &regValue);
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)
        regValue &= ~0x06;
        errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)MODE_RECOVER_CFG1, 1, &regValue );
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
        /* End of Clear HDMI mode flag in BCAPS (WA for N1)*/
    }


    /* clear HDCP interrupt status bits that may have been raised during this process */
    regValue = (UInt8)0x07;
    errCode |=  tmbslTDA1997XWriteI2C(unit, (UInt16)INT_FLG_CLR_HDCP, 1, &regValue);

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Configure receiver capabilities. This function allows enabling or
           disabling some of the receiver capabilities (1.1 features, fast I2C,
           fast reauthentification).

    \param Unit       Receiver unit number
    \param repeater   Enable/disable repeater capability
    \param FastI2c    Enable/disable fast I2C capability
    \param Hdmi11     Enable/disable HDMI 1.1 extensions
    \param FastReauth Enable/disable fast reauthentication capability

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XConfigureReceiverCaps
(
    tmUnitSelect_t              unit,
    tmbslHdmiRxCapsRepeater_t   repeater,
    tmbslHdmiRxCapsFastI2c_t    fastI2c,
    tmbslHdmiRxCapsHdmi11_t     hdmi11,
    tmbslHdmiRxCapsFastReauth_t fastReauth
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           regValue;


    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Receiver capabilities register value */
    regValue = (UInt8)(DEF_VAL_CAPS | repeater | fastI2c | hdmi11 | fastReauth);

    /* Write HDCP_BCAPS register */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)HDCP_BCAPS, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Set HDCP RepeaterStatus. This function allows the caller to define
           what state will be reported to the Hdmi Tx w.r.t HDCP repeater
           status.

    \param Unit             Receiver unit number
    \param maxDevStatus     Has the maximum number of attached device been
                            reached ? (true/false)
    \param attachedDevices  Number of attached devices
    \param maxCascadeStatus Has the maximum level of cascaded devices been
                            reached ? (true/false)
    \param depth            Depth of the device cascade

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED: this function is not supported
              on the current configured IC.

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XSetHDCPRepeaterStatus
(
    tmUnitSelect_t                       unit,
    tmbslHdmiRxHDCPRepStatusMaxDev_t     maxDevStatus,
    UInt8                                attachedDevices,
    tmbslHdmiRxHDCPRepStatusMaxCascade_t maxCascadeStatus,
    UInt8                                depth
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           pTabRegValue[2];


    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    if (attachedDevices > MAX_VAL_DEVICE_COUNT)
    {
        attachedDevices = MAX_VAL_DEVICE_COUNT;
    }

    /* Test the depth value (MAX_VAL_DEPTH = 0x07) */
    if(depth > MAX_VAL_DEPTH)
    {
        depth = MAX_VAL_DEPTH;
    }

    /* HDCP_BSTATUS_MSB register value */
    pTabRegValue[0] = (UInt8) (maxCascadeStatus | depth);

    /* HDCP_BSTATUS_LSB register value */
    pTabRegValue[1] = (UInt8) (maxDevStatus | attachedDevices);

    /* Write HDCP_BSTATUS registers */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)HDCP_BSTATUS_MSB, 2, pTabRegValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Set HDCP Repeater control information. This function allows the
           caller to set the hardware state of the repeater accordingly to
           the status of the negociations with downstream devices.

    \param Unit             Receiver unit number
    \param dpUnauth         Downstream port unauthenticated (True/False)
    \param wdEnd            5s Watchdog reached (True/False)
    \param ksvReady         KSV list is ready (True/false)

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED: this function is not supported
              on the current configured IC.

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XSetHDCPRepeaterControl
(
    tmUnitSelect_t unit,
    UInt8          dpUnauth,
    UInt8          wdEnd,
    UInt8          ksvReady
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           regValue = 0;


    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* HDCP_RPT_CTRL register value */
    if (dpUnauth != 0)
    {
        regValue = (UInt8) (regValue | K_DP_UNAUTH);
    }

    if (wdEnd != 0)
    {
        regValue = (UInt8) (regValue | K_WD_END);
    }

    if (ksvReady != 0)
    {
        regValue = (UInt8) (regValue | K_KSV_READY);
    }

    regValue |= (UInt8) MASK_AUTO_RESET;

    /* Write HDCP_RPT_CTRL register */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)HDCP_RPT_CTRL, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /*WA blocking AVI and AUD info frames during authentication */
    /* unmask sr_audio and sr_if */
    tmbslTDA1997XReadI2C(unit, (UInt16)INT_MASK_TOP,1, &regValue);
    regValue |= 0x30;
    tmbslTDA1997XWriteI2C(unit, (UInt16)INT_MASK_TOP,1, &regValue);
    //printf("------------ IF UNBLOCKED\n");
    /*WA blocking AVI and AUD info frames during authentication */


    return TM_OK;

}
/*============================================================================*/
/**
    \brief Set HDCP key selection vector. This function sets the key selection
           vector that will be used by the receiver HDCP module.

    \param Unit               Receiver unit number
    \param KeySelectionVector Pointer to the 5 byte selection vector (MSB first)

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XSetKeySelectionVector
(
    tmUnitSelect_t unit,
    UInt8          pKeySelectionVector[5]
)
{
    tmErrorCode_t   errCode = TM_OK;

    /* Test pKeySelectionVector <> NULL */
    RETIF(pKeySelectionVector == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Write key selection vector register */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)HDCP_BKSV4, 5, pKeySelectionVector);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Set private key index. This function is used to set the index of
           the next private key that will be set/downloaded to the chip.

    \param Unit     Receiver unit number
    \param KeyIndex Index of the key to be set

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XSetPrivateKeyIndex
(
    tmUnitSelect_t unit,
    UInt8          keyIndex
)
{
    tmErrorCode_t   errCode = TM_OK;

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Write HDCP_KICX register */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)HDCP_KIDX, 1, &keyIndex);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Set private key. This function is used to set a private 56-bit key
           into the receiver HDCP module. This function is generally used
           after a call to SetKeyIndex

    \param Unit Receiver unit number
    \param pKey Pointer to the 7 bytes key (MSB first)

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XSetPrivateKey
(
    tmUnitSelect_t unit,
    UInt8          pKey[7]
)
{
    tmErrorCode_t   errCode = TM_OK;

    /* Test pKey <> NULL */
    RETIF(pKey == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Write HDCP private key registers */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)HDCP_KEY6, 7, pKey);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Set KSV index. This function is used to set the index of
           the next KSV that will be stored in the KSV FIFO for the repeater
           functionality.

    \param Unit     Receiver unit number
    \param KeyIndex Index of the key to be set

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED: this function is not supported
              on the current configured IC.

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XSetKSVIndex
(
    tmUnitSelect_t unit,
    UInt8          keyIndex
)
{
    tmErrorCode_t   errCode = TM_OK;

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Write HDCP_KSVX register */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)FIFO_KSV_IDX, 1, &keyIndex);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    return TM_OK;

}

/*============================================================================*/
/**
    \brief Set KSV value. This function is used to store a KSV value
           into the KSV FIFO for HDCP repeater functionality.

    \param Unit Receiver unit number
    \param pKSV Pointer to the 5 bytes KSV (MSB first)

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED: this function is not supported
              on the current configured IC.
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XSetKSVValue
(
    tmUnitSelect_t unit,
    UInt8          pKSV[5]
)
{
    tmErrorCode_t   errCode = TM_OK;

    /* Test pKSV <> NULL */
    RETIF(pKSV == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Write key selection vector registers */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)FIFO_KSV4, 5, pKSV);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Configure HDCP error protection. This function is used to set the
           parameters that affects HDCP error protection mechanisms :
           Error detection and error filtering.

    \param Unit                 Receiver unit number
    \param delockDelay          Delay before delocking the word locker
    \param DEMeasureMode        DE measurement mode
    \param DERegenerationMode   Enable/disable DE regeneration
    \param DEFilterSensivity    DE filter sensivity
    \param DECompMode           DE composition mode
    \param CTLFilterSensivity   CTL filter sensivity
    \param VSFilterSensivity    VS filter sensivity
    \param HSFilterSensivity    HS filter sensivity

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XConfigureHDCPErrorProtection
(
    tmUnitSelect_t                  unit,
    UInt8                           delockDelay,
    tmbslHdmiRxDEMeasurementMode_t  DEMeasureMode,
    tmbslHdmiRxDERegeneration_t     DERegenerationMode,
    tmbslHdmiRxDEFilterSensivity_t  DEFilterSensivity,
    tmbslHdmiRxDECompositionMode_t  DECompMode,
    tmbslHdmiRxCTLFilterSensivity_t CTLFilterSensivity,
    tmbslHdmiRxVSFilterSensivity_t  VSFilterSensivity,
    tmbslHdmiRxHSFilterSensivity_t  HSFilterSensivity
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           pTabRegValue[2];

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* test delockDelay <= 7 */
    RETIF(delockDelay > MAX_VAL_DELOCK_DEL,TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)

    pTabRegValue[0] = delockDelay;

    /* Write delockDelay */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)DELOCK_DELAY, 1, pTabRegValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    pTabRegValue[0] = (UInt8)(DEMeasureMode | DERegenerationMode | DEFilterSensivity | DECompMode);

    pTabRegValue[1] = (UInt8)(CTLFilterSensivity | VSFilterSensivity | HSFilterSensivity);

    /* Write HDCP private key registers */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)HDCP_DE_CTRL, 2, pTabRegValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Get display data. This function allows reading the display data
           received through the DDC channel.

    \param Unit                     Receiver unit number
    \param LinkVerification         Link verification response
    \param Aksv                     Transmitter key selection vector
    \param Rand                     Session random number
    \param EnhancedLinkVerification Enhanced link verification response
    \param CipherMode               HDCP cipher mode

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XGetDisplayData
(
    tmUnitSelect_t  unit,
    UInt16          *pLinkVerification,
    UInt8           pAksv[5],
    UInt8           pRand[8],
    UInt8           *pEnancedLinkVerification,
    UInt8           *pCipherMode
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           pTabRegValue[14];
    UInt8           index;

    /* Test pointers <> NULL */
    RETIF(pLinkVerification == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pAksv == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pRand == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pEnancedLinkVerification == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pCipherMode == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)HDCP_RI_MSB, 3, pTabRegValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

    *pLinkVerification  = ((UInt16)(pTabRegValue[0]) << 8) | pTabRegValue[1];
    *pEnancedLinkVerification = pTabRegValue[2];


    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)HDCP_AKSV4, 15, pTabRegValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

    for (index=0; index< 5; index++)
    {
        pAksv[index] = pTabRegValue[index];
    }
    for (index=0; index< 8; index++)
    {
        pRand[index] = pTabRegValue[index+5];
    }
    *pCipherMode              = pTabRegValue[13];

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Get infoframe data. This function is used to read data coming from
           an infoframe. It is typically used after beeing notified that an
           infoframe as been received by HW. The amount of data that will be
           written by the function depend on the type of infoframe that is
           requested. With this function, you can read AUDIO, AVI, SPD and MPS
           infoframes.

    \param Unit          Receiver unit number
    \param InfoFrameType Type of infoframe to read
    \param ReadType      Type read in the infoframe itself as in EIA/CEA-861B
    \param Version       Infoframe version
    \param Length        Infoframe length
    \param CheckSum      Infoframe checksum
    \param Data          Infoframe data array (maximum: 31 bytes)

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XGetInfoframe
(
    tmUnitSelect_t           unit,
    tmbslHdmiRxInfoFrame_t   infoFrame,
    UInt8                    *pReadType,
    UInt8                    *pVersion,
    UInt8                    *pLength,
    UInt8                    *pCheckSum,
    UInt8                    *pData
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           pTabRegValue[MAX_VAL_IF_NB_DATA];
    UInt8           index;
    UInt8           nbrData;

    /* Test pointers <> NULL */
    RETIF(pReadType == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pVersion == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pLength == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pCheckSum == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pData == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    switch(infoFrame)
    {
    case BSLHDMIRX_INFOFRAME_AVI :
        nbrData = AVI_IF_NB_DATA;
        errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)AVI_IF_TYPE, nbrData, pTabRegValue);
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

        /* read update flag and store at the end */
        //nbrData += 1;
        //errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)AVI_IF_UPDATE, 1, &pTabRegValue[AVI_IF_NB_DATA]);
        //RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)
        break;

    case BSLHDMIRX_INFOFRAME_SPD :
        nbrData = SPD_IF_NB_DATA;
        errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)SPD_IF_TYPE, nbrData, pTabRegValue);
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

        /* read update flag and store at the end */
        //nbrData += 1;
        //errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)SPD_IF_UPDATE, 1, &pTabRegValue[SPD_IF_NB_DATA]);
        //RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)
        break;

    case BSLHDMIRX_INFOFRAME_AUD :
        nbrData = AUD_IF_NB_DATA;
        errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)AUD_IF_TYPE, nbrData, pTabRegValue);
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

        /* read update flag and store at the end */
        //nbrData += 1;
        //errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)AUD_IF_UPDATE, 1, &pTabRegValue[AUD_IF_NB_DATA]);
        //RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)
        break;

    case BSLHDMIRX_INFOFRAME_MPS :
        nbrData = MPS_IF_NB_DATA;
        errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)MPS_IF_TYPE, nbrData, pTabRegValue);
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

        /* read update flag and store at the end */
        //nbrData += 1;
        //errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)MPS_IF_UPDATE, 1, &pTabRegValue[MPS_IF_NB_DATA]);
        //RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)
        break;

    case BSLHDMIRX_INFOFRAME_VS :
        nbrData = VS_IF_NB_DATA;
        errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)VS_IF_HDMI_TYPE, nbrData, pTabRegValue);
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

        /* read update flag and store at the end */
        nbrData += 1;
        errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)VS_IF_HDMI_UPDATE, 1, &pTabRegValue[VS_IF_NB_DATA]);
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)
        break;

    case BSLHDMIRX_INFOFRAME_VS_OTHER_BK1 :
        nbrData = VS_IF_NB_DATA;
        errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)VS_IF_OTHER_BK1_TYPE, nbrData, pTabRegValue);
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

        /* read update flag and store at the end */
        nbrData += 1;
        errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)VS_IF_OTHER_BK1_UPDATE, 1, &pTabRegValue[VS_IF_NB_DATA]);
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)
        break;

    case BSLHDMIRX_INFOFRAME_VS_OTHER_BK2 :
        nbrData = VS_IF_NB_DATA;
        errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)VS_IF_OTHER_BK2_TYPE, nbrData, pTabRegValue);
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

        /* read update flag and store at the end */
        nbrData += 1;
        errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)VS_IF_OTHER_BK2_UPDATE, 1, &pTabRegValue[VS_IF_NB_DATA]);
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)
        break;

    default :
        nbrData = 0;
        return TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER;
    }

    *pReadType  = pTabRegValue[0];
    *pVersion   = pTabRegValue[1];
    *pLength    = pTabRegValue[2] & MASK_IF_LENGTH;
    *pCheckSum  = pTabRegValue[3];

    //if (pTabRegValue[VS_IF_NB_DATA] > HDMI_INFO_EXCEED) return TM_OK;

    for(index = 0; index < nbrData - IF_HEAD_CHECK_LENGTH; index++)
    {
        pData[index] = pTabRegValue[index + IF_HEAD_CHECK_LENGTH];
    }

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Get audio content protection packet. This function is used to read an
           ACP packet after beeing notified that an ACP packet as been received
           by HW.

    \param Unit       Receiver unit number
    \param PacketType ACP packet type
    \param AcpType    ACP type
    \param Data       ACP data array (28 bytes)

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XGetACPPacket
(
    tmUnitSelect_t  unit,
    UInt8           *pPacketType,
    UInt8           *pAcpType,
    UInt8           pData[28]
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           pTabRegValue[ACP_PKT_HEADER_LENGTH + ACP_PKT_DATA_LENGTH];
    UInt8           index;

    /* Test pointers <> NULL */
    RETIF(pPacketType == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pAcpType == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pData == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)ACP_PACKET_TYPE, ACP_PKT_HEADER_LENGTH + ACP_PKT_DATA_LENGTH, pTabRegValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

    *pPacketType = pTabRegValue[0];
    *pAcpType    = pTabRegValue[1];

    for (index = 0; index < ACP_PKT_DATA_LENGTH; index++)
    {
        pData[index] = pTabRegValue[index + ACP_PKT_HEADER_LENGTH];
    }

    for(index = ACP_PKT_DATA_LENGTH; index < 28; index++)
    {
        pData[index] = 0;
    }

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Get ISRC1 packet. This function is used to read an ISRC1 packet
           after beeing notified that an ISRC1 packet as been received by HW.

    \param Unit       Receiver unit number
    \param PacketType ISRC packet type
    \param Control    ISRC control value
    \param Data       ISRC packet data array (16 bytes)

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XReadISRC1Packet
(
    tmUnitSelect_t  unit,
    UInt8           *pPacketType,
    UInt8           *pControl,
    UInt8           pData[16]
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           pTabRegValue[ISRC_PKT_HEADER_LENGTH + ISRC_PKT_DATA_LENGTH];
    UInt8           index;

    /* Test pointers <> NULL */
    RETIF(pPacketType == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pControl == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pData == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)ISRC1_PACKET_TYPE, ISRC_PKT_HEADER_LENGTH + ISRC_PKT_DATA_LENGTH, pTabRegValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

    *pPacketType = pTabRegValue[0];
    *pControl = pTabRegValue[1];

    for (index = 0; index < ISRC_PKT_DATA_LENGTH; index++)
    {
        pData[index] = pTabRegValue[index + ISRC_PKT_HEADER_LENGTH];
    }

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Get ISRC2 packet. This function is used to read an ISRC2 packet
           after beeing notified that an ISRC2 packet as been received by HW.

    \param Unit       Receiver unit number
    \param PacketType ISRC packet type
    \param Data       ISRC packet data array (16 bytes)

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XReadISRC2Packet
(
    tmUnitSelect_t  unit,
    UInt8           *pPacketType,
    UInt8           pData[16]
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           pTabRegValue[ISRC_PKT_HEADER_LENGTH + ISRC_PKT_DATA_LENGTH];
    UInt8           index;

    /* Test pointers <> NULL */
    RETIF(pPacketType == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pData == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)ISRC2_PACKET_TYPE, ISRC_PKT_HEADER_LENGTH + ISRC_PKT_DATA_LENGTH, pTabRegValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

    *pPacketType = pTabRegValue[0];

    for (index = 0; index < ISRC_PKT_DATA_LENGTH; index++)
    {
        pData[index] = pTabRegValue[index + ISRC_PKT_HEADER_LENGTH];
    }

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Get Gamut Metadata packet. This function is used to read a Gamut
           Metadata packet after beeing notified that it has been received by HW.

    \param Unit       Receiver unit number
    \param PacketType Gamut Boundary Description (GBD) packet type
    \param Header     Gamut Boundary Description (GBD) header (2 bytes)
    \param Data       Gamut Boundary Description (GBD) data array (28 bytes)

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

******************************************************************************/
tmErrorCode_t
tmbslTDA1997XReadGBDPacket
(
    tmUnitSelect_t  unit,
    UInt8           *pPacketType,
    UInt8           pHeader[2],
    UInt8           pData[28]
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           pTabRegValue[GAMUT_PKT_HEADER_LENGTH + GAMUT_PKT_DATA_LENGTH];
    UInt8           index;

    /* Test pointers <> NULL */
    RETIF(pPacketType == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pHeader == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pData == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)GBD_PACKET_TYPE, GAMUT_PKT_HEADER_LENGTH + GAMUT_PKT_DATA_LENGTH, pTabRegValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

    *pPacketType = pTabRegValue[0];
    pHeader[0] = pTabRegValue[1];
    pHeader[1] = pTabRegValue[2];

    for (index = 0; index < GAMUT_PKT_DATA_LENGTH; index++)
    {
        pData[index] = pTabRegValue[index + GAMUT_PKT_HEADER_LENGTH];
    }

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Reset HDMI parts by software. Each module of the HDMI digital part
           can be reseted through this function.

    \param unit           Receiver unit number
    \param nackHdcp       Enable/disable EDID acknowledgment
    \param resetAi        Reset advanced infomation module (yes/no)
    \param resetInfoframe Reset infoframe module (yes/no)
    \param resetAudioFifo Reset audio FIFO (yes/no)
    \param resetGamut     Reset gamut metadata (yes/no)
    \param resetFifoCtrl  Reset audio FIFO control (yes/no)
    \param resetSus       Reset Start-Up Sequencer (yes/no)
    \param resetDc        Reset deep color module (yes/no)

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XHDMISoftReset
(
    tmUnitSelect_t                  unit,
    tmbslHdmiRxHdmiNackHdcp_t       nackHdcp,
    tmbslHdmiRxHdmiResetAi_t        resetAi,
    tmbslHdmiRxHdmiResetInfoframe_t resetInfoframe,
    tmbslHdmiRxHdmiResetAudioFifo_t resetAudioFifo,
    tmbslHdmiRxHdmiResetGamut_t     resetGamut,
    tmbslHdmiRxHdmiResetFifoCtrl_t  resetFifoCtrl,
    tmbslHdmiRxHdmiResetSus_t       resetSus,
    tmbslHdmiRxHdmiResetDc_t        resetDc
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           pTabRegValue[2];

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* HDMI_SOFT_RST register value */
    pTabRegValue[0] = (UInt8) resetDc;

    /* HDMI_INFO_RST register value */
    pTabRegValue[1] = (UInt8) ( nackHdcp | resetAi | resetInfoframe |
                                resetAudioFifo | resetGamut | resetFifoCtrl);

    /* check */
    if (nackHdcp == BSLHDMIRX_DONTNACK_HDCP)
    {
        nackHdcp = BSLHDMIRX_DONTNACK_HDCP;
    }
    else
    {
        nackHdcp = BSLHDMIRX_NACK_HDCP;
    }

    /* Write HDMI_SOFT_RST and HDMI_INFO_RST registers */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)HDMI_SOFT_RST, 2, pTabRegValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /* Write SUS_RESET register (bit hdcp_ddc_man does not exist on TDA19972)*/
    switch (resetSus)
    {
    case BSLHDMIRX_DONTRESET_SUS:
        errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)RATE_CTRL, 1, pTabRegValue);
        pTabRegValue[0] |= RATE_REFTIM_ENABLE;
        errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)RATE_CTRL, 1, pTabRegValue);
        break;
    case BSLHDMIRX_RESET_SUS:
        errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)RATE_CTRL, 1, pTabRegValue);
        pTabRegValue[0] &= ~RATE_REFTIM_ENABLE;
        errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)RATE_CTRL, 1, pTabRegValue);
        break;
    default:
        errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)RATE_CTRL, 1, pTabRegValue);
        break;
    }

    return TM_OK;
}

/******************************************************************************
    \brief Get the capabilities of a unit.


    \param pCapabilities Pointer to the capabilities structure.

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range

******************************************************************************/
tmErrorCode_t
tmbslTDA1997XGetCapabilities
(
    tmUnitSelect_t            unit,
    tmbslHdmiRxCapabilities_t *pCapabilities
)
{
    /* Test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER);

    /* Test the pointer */
    RETIF(pCapabilities == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS);

    pCapabilities->deviceVersion = RxHdmiConfig[unit].version;

    switch (RxHdmiConfig[unit].version)
    {
    case BSLHDMIRX_TDA19971:
    case BSLHDMIRX_TDA19972:
    case BSLHDMIRX_TDA19973_SOC_IN:
    case BSLHDMIRX_TDA19973_CONNECTOR_IN:
    case BSLHDMIRX_TDA19971N2:
    case BSLHDMIRX_TDA19973N2_SOC_IN:
    case BSLHDMIRX_TDA19973N2_CONNECTOR_IN:
        /* HW device is a TDA1997X */
        pCapabilities->repeater = True;
        pCapabilities->fastI2C = False;
        pCapabilities->hdmiVersion = BSLHDMIRX_HDMI_VERSION_1_4a;
        pCapabilities->audioPacket.HBR = True;
        pCapabilities->audioPacket.DST = True;
        pCapabilities->audioPacket.oneBitAudio = True;
        pCapabilities->fastReauth = True;
        pCapabilities->analogInput = False;
        pCapabilities->hdmi11Features = False;
        pCapabilities->internalEdid = True;
        break;

    default:
        return TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER;
    }

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Configure audio clocks. All audio clocks parameters
           (internal and sysclk) are modified through this function.

    \param Unit             Receiver unit number
    \param clocksPowerMode  Set the power mode of the clock block
    \param clocksRefMode    Set the clock reference (TMDS or XTAL)  (ignored with TDA1997X)
    \param clocksRefFreq    Set the reference clock frequency
    \param clocksAudioMode  Set the internal audio clock mode
    \param clocksSysMode    Set the SYSCLK clock mode

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_TDA1997X_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_TDA1997X_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_TDA1997X_I2C_WRITE: failed when writing to the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED: this function is not supported on the
              current configured IC.

******************************************************************************/
tmErrorCode_t
tmbslTDA1997XConfigureAudioClkMode
(
    tmUnitSelect_t                          unit,
    tmbslHdmiRxAudioClocksModePowerMode_t   clocksPowerMode,	/* ignored by TDA19972 */
    tmbslHdmiRxAudioClocksModeRefMode_t     clocksRefMode,      /* ignored with TDA1997X */
    tmbslHdmiRxAudioClocksModeRefFreq_t     clocksRefFreq,		/* only 27Mhz supported by TDA19972*/
    tmbslHdmiRxAudioClocksAudioMode_t       clocksAudioMode,
    tmbslHdmiRxAudioClocksSysMode_t         clocksSysMode		/* ignored by TDA19972*/
)
{
    tmErrorCode_t   errCode = TM_OK;


    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* check clocksRefFreq is BSLHDMIRX_CLOCKS_MODE_REFFREQ_27 */
    RETIF(clocksRefFreq != BSLHDMIRX_CLOCKS_MODE_REFFREQ_27, TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)

    /* WA to be implemented to select FRo (not selected correctly by default)*/


    /* Write CLOCKS_MODE register */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)AUDIO_CLOCK_MODE, 1, (UInt8*)&clocksAudioMode);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    DUMMY_ACCESS(clocksPowerMode);
    DUMMY_ACCESS(clocksRefMode);
    DUMMY_ACCESS(clocksSysMode);

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Get the General Control Packets. This function is used to read a
           General Control Packet after beeing notified by HW that the deep
           color mode has changed.

    \param Unit       Receiver unit number
    \param PacketType General Control Packet (GCP) packet type
    \param Header     General Control Packet (GCP) header (2 bytes)
    \param Data       General Control Packet (GCP) data array (7 bytes)

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

******************************************************************************/
tmErrorCode_t
tmbslTDA1997XReadGCP
(
    tmUnitSelect_t  unit,
    UInt8           *pPacketType,
    UInt8           pHeader[2],
    UInt8           pData[7]
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           pTabRegValue[GCP_HEADER_LENGTH + GCP_DATA_LENGTH];
    UInt8           index;

    /* Test pointers <> NULL */
    RETIF(pPacketType == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pHeader == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pData == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Read the GCP registers */
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)GCP_PACKET_TYPE, GCP_HEADER_LENGTH + GCP_DATA_LENGTH, pTabRegValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

    *pPacketType = pTabRegValue[0];
    pHeader[0] = pTabRegValue[1];
    pHeader[1] = pTabRegValue[2];

    for (index = 0; index < GCP_DATA_LENGTH; index++)
    {
        pData[index] = pTabRegValue[index + GCP_HEADER_LENGTH];
    }

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Get TMDS frequency of a given input. The value is given in kHz.

    \param Unit     Receiver unit number
    \param inputSel Input to retreive the frequency of
    \param pFreq    Pointer on the integer that will receive the frequency of the
                    selected input

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XGetTMDSFrequency
(
    tmUnitSelect_t                      unit,
    tmbslHdmiRxInputSelDigitalSource_t  inputSel,
    UInt32                              *pFreq
)
{
    tmErrorCode_t   errCode = TM_OK;

    UInt8           freq[3];

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Test pointers <> NULL */
    RETIF(pFreq == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* Check the input chosen */
    if(inputSel == BSLHDMIRX_DIGITALINPUT_A)
    {
        /* Read the Clk_A_rate register */
        errCode = tmbslTDA1997XReadI2C(unit, CLK_A_RATE_MSB, 3, freq);
    }
    else if (inputSel == BSLHDMIRX_DIGITALINPUT_B)
    {
        /* Read the Clk_B_rate register */
        errCode = tmbslTDA1997XReadI2C(unit, CLK_B_RATE_MSB, 3, freq);
    }
    else
    {
        /* This should not happen */
        errCode = TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER;
    }

    *pFreq = ((UInt32)freq[0] << 16 |(UInt32)freq[1] << 8 | freq[2]);

    return errCode;
}

/*============================================================================*/
/**
    \brief Get audio sample frequency from audio PLL.

    \param unit      Receiver unit number
    \param dstRate   DST single/double rate
    \param audioFreq Audio sample frequency

    \return The call result:
            - TM_OK  : the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading to the I2C bus
            - TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED: this function is not supported on the
              current configured IC.

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XGetAudioFrequency
(
    tmUnitSelect_t                unit,
    tmbslHdmiRxAudioDstRate_t     *pDstRate,
    tmbslHdmiRxAudioSampleFreq_t  *pAudioFreq
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           regValue;

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Test pointers <> NULL */
    RETIF(pDstRate == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pAudioFreq == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* Read the audio frequency register */
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)AUDIO_FREQ, 1, &regValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

    *pDstRate   = (tmbslHdmiRxAudioDstRate_t)(regValue & MASK_AUDIO_DST_RATE);
    *pAudioFreq = (tmbslHdmiRxAudioSampleFreq_t)(regValue & MASK_AUDIO_FREQ);

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Get current deep color mode

    \param Unit               Receiver unit number
    \param pPixelPackingPhase Pointer to the pixel packing phase
    \param pDeepColorMode     Pointer to the deep color mode

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent
            - TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED: this function is not supported on the
              current configured IC.

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XGetDeepColorMode
(
    tmUnitSelect_t             unit,
    UInt8                      *pPixelPackingPhase,
    tmbslHdmiRxDeepColorMode_t *pDeepColorMode
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           regValue;

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Test pointers <> NULL */
    RETIF(pPixelPackingPhase == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pDeepColorMode == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* Read the audio frequency register */
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)DEEP_COLOR_MODE, 1, &regValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

    *pPixelPackingPhase = (UInt8)(regValue & MASK_DC_PIXEL_PHASE);
    *pDeepColorMode     = (tmbslHdmiRxDeepColorMode_t)(regValue & MASK_DC_COLOR_DEPTH);

    return TM_OK;
}

/******************************************************************************
    \brief Set the power state of the HDMI receiver. ON state
           correspond to a fully supplied, up and running device. Other modes
           correspond to the powerdown states of the device.

    \param unit       Receiver unit number
    \param powerState Power state to set.

    \return The call result:
            - TM_OK  : the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED: this function is not supported on the
              current configured IC.

******************************************************************************/
/*tmbslTDA19972SetPowerState*/

tmErrorCode_t tmbslTDA1997XSetPowerState
(
    tmUnitSelect_t unit,
    tmPowerState_t powerState
)
{
    /*
        tmErrorCode_t   errCode = TM_OK;
        UInt8           regValue;

         test the unit number
        RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

        if( (powerState == tmPowerOn) || (powerState == tmPowerStandby) )
        {
             Power on sequence

             Disable low power mode
            errCode = tmbslTDA1997XReadI2C(unit, (UInt16)EDID_POWER, 1, &regValue);
            RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

            regValue = (UInt8) (regValue & ~MASK_LOW_PW_EDID);
            errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)EDID_POWER, 1, &regValue);
            RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

             Automatic control of TMDS
            regValue = (UInt8) TMDS_AUTO_PON;
            errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)PON_OVR_EN, 1, &regValue);
            RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

             Enable current bias unit
            regValue = (UInt8) CBIAS_PON;
            errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)CFG1, 1, &regValue);
            RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

             Enable TMDS clock in the digital equalizer
            errCode = tmbslTDA1997XReadI2C(unit, (UInt16)SUS_RESET, 1, &regValue);
            RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

            regValue = (UInt8) (regValue & ~MASK_TMDS_CLK_DIS);
            errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)SUS_RESET, 1, &regValue);
            RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

             Enable Xtal oscillator
            errCode = tmbslTDA1997XReadI2C(unit, (UInt16)XOSC_CFG, 1, &regValue);
            RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

            regValue = (UInt8) (regValue & ~MASK_XTAL_OSC_PD);
            errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)XOSC_CFG, 1, &regValue);
            RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

             Enable deep color PLL
            regValue = (UInt8) DC_PLL_PON;
            errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)DEEP_PLL7, 1, &regValue);
            RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

             Enable audio PLL
            errCode = tmbslTDA1997XReadI2C(unit, (UInt16)CLOCKS_MODE, 1, &regValue);
            RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

            regValue = (UInt8) (regValue & ~MASK_AUDIO_PLL_PD);
            errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)CLOCKS_MODE, 1, &regValue);
            RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

             Output buffers active
            errCode = tmbslTDA1997XReadI2C(unit, (UInt16)OF_CTRL, 1, &regValue);
            RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

            regValue = (UInt8) (regValue & ~MASK_OF_CTRL_OUT_HIZ);
            errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)OF_CTRL, 1, &regValue);
            RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)
        }
        else
        {
             Power-down EDID mode sequence

             Output buffers in HiZ
            errCode = tmbslTDA1997XReadI2C(unit, (UInt16)OF_CTRL, 1, &regValue);
            RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

            regValue |= (UInt8) MASK_OF_CTRL_OUT_HIZ;
            errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)OF_CTRL, 1, &regValue);
            RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

             Disable audio PLL
            errCode = tmbslTDA1997XReadI2C(unit, (UInt16)CLOCKS_MODE, 1, &regValue);
            RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

            regValue |= (UInt8) MASK_AUDIO_PLL_PD;
            errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)CLOCKS_MODE, 1, &regValue);
            RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

             Disable deep color PLL
            regValue = (UInt8) DC_PLL_PD;
            errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)DEEP_PLL7, 1, &regValue);
            RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

             Disable Xtal oscillator
            errCode = tmbslTDA1997XReadI2C(unit, (UInt16)XOSC_CFG, 1, &regValue);
            RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

            regValue |= (UInt8) MASK_XTAL_OSC_PD;
            errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)XOSC_CFG, 1, &regValue);
            RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

             Disable TMDS clock in the digital equalizer
            errCode = tmbslTDA1997XReadI2C(unit, (UInt16)SUS_RESET, 1, &regValue);
            RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

            regValue |= (UInt8) MASK_TMDS_CLK_DIS;
            errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)SUS_RESET, 1, &regValue);
            RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

             Disable current bias unit
            regValue = (UInt8) CBIAS_POFF;
            errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)CFG1, 1, &regValue);
            RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

             Manual control of TMDS
            regValue = (UInt8) TMDS_MAN_PON;
            errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)PON_OVR_EN, 1, &regValue);
            RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

             Enable low power mode
            errCode = tmbslTDA1997XReadI2C(unit, (UInt16)EDID_POWER, 1, &regValue);
            RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

            regValue |= (UInt8) MASK_LOW_PW_EDID;
            errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)EDID_POWER, 1, &regValue);
            RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)
        }

    */
    return TM_OK;
}

/******************************************************************************
    \brief Configure the HDMI receiver to enable/disable test.

    \param unit              Receiver unit number
    \param testPatternSel    Activation of the color-bar test pattern
    \param testPatternFormat Video mode of the color-bar test pattern

    \return The call result:
            - TM_OK  : the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus
            - TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED: this function is not supported on the
              current configured IC.

******************************************************************************/
tmErrorCode_t tmbslTDA1997XActivateTestPattern
(
    tmUnitSelect_t unit,
    tmbslHdmiRxTestPatternSel_t     testPatternSel,
    tmbslHdmiRxTestPatternFormat_t  testPatternFormat
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           RegVal, VsPolSel, HsPolSel, DEPolSel, ServiceMode;
    tmbslHdmiRxVHRefValues_t VHRefValues;
    tmbslHdmiRxVHSyncValues_t VHSyncValues;

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    if((testPatternSel == BSLHDMIRX_TESTPATTERN_ON) || (testPatternSel == BSLHDMIRX_TESTPATTERN_RAMP))
    {
        ServiceMode = testPatternSel;

        switch (testPatternFormat)
        {
        case BSLHDMIRX_TESTPATTERN_480P:

            /* Set pixel and line number */
            /* 1, 858, 1, 525 for 480p60 see tmdlHdmiRx_cfg.c*/
            /* line preset is set to 1 instead of 7 for line counter free running */
            bslTDA1997XSetPixelAndLineCounters (unit, 1, 858, 1, 525);

            /* VHSyncValues */
            VHSyncValues.hsync_start = 1;
            VHSyncValues.hsync_end = 63;
            VHSyncValues.vsync_f1_pix_start = 1;
            VHSyncValues.vsync_f1_pix_end = 1;
            VHSyncValues.vsync_f1_start = 7;
            VHSyncValues.vsync_f1_width = 6;
            VHSyncValues.vsync_f2_pix_start = 0;
            VHSyncValues.vsync_f2_pix_end = 0;
            VHSyncValues.vsync_f2_start = 0;
            VHSyncValues.vsync_f2_width = 0;

            bslTDA1997XSetVHSyncValues(unit, &VHSyncValues);


            /* VHRefValues */
            /* {123, 843, 516, 45, 0, 0, 1, BSLHDMIRX_FREF_FIELD1_LOW_FIELD2_HIGH, 0} for 480p60 see tmdlHdmiRx_cfg.c*/
            VHRefValues.href_start = 123;
            VHRefValues.href_end = 843;
            VHRefValues.vref_f1_start = 523;
            VHRefValues.vef_f1_width = 45;
            VHRefValues.vref_f2_start = 0;
            VHRefValues.vef_f2_width = 0;
            VHRefValues.fieldref_f1_start = 7;
            VHRefValues.fieldref_f2_start = 0;
            VHRefValues.fieldPolarity = BSLHDMIRX_FREF_FIELD1_LOW_FIELD2_HIGH;

            bslTDA1997XSetVHRefValues (unit, &VHRefValues );

            ServiceMode |= (CLKX2_XTAL|CLK_XTAL|STD_NTSC|INTPRO_PROG);

            break;

        case BSLHDMIRX_TESTPATTERN_576P:

            /* Set pixel and line number */
            /* 1, 864, 1, 625 for 480p60 see tmdlHdmiRx_cfg.c */
            bslTDA1997XSetPixelAndLineCounters (unit, 1, 864, 1, 625);

            /* VHSyncValues */
            VHSyncValues.hsync_start = 1;
            VHSyncValues.hsync_end = 65;
            VHSyncValues.vsync_f1_pix_start = 1;
            VHSyncValues.vsync_f1_pix_end = 1;
            VHSyncValues.vsync_f1_start = 1;
            VHSyncValues.vsync_f1_width = 5;
            VHSyncValues.vsync_f2_pix_start = 0;
            VHSyncValues.vsync_f2_pix_end = 0;
            VHSyncValues.vsync_f2_start = 0;
            VHSyncValues.vsync_f2_width = 0;

            bslTDA1997XSetVHSyncValues(unit, &VHSyncValues);


            /* VHRefValues */
            /* {133, 853, 620, 49, 0, 0, 1, BSLHDMIRX_FREF_FIELD1_LOW_FIELD2_HIGH, 0} for 576p50 see tmdlHdmiRx_cfg.c */
            VHRefValues.href_start = 133;
            VHRefValues.href_end = 853;
            VHRefValues.vref_f1_start = 621; /* seems there is an error in values in tmdlHdmiRx_cfg.c */
            VHRefValues.vef_f1_width = 49;
            VHRefValues.vref_f2_start = 0;
            VHRefValues.vef_f2_width = 0;
            VHRefValues.fieldref_f1_start = 1;
            VHRefValues.fieldref_f2_start = 0;
            VHRefValues.fieldPolarity = BSLHDMIRX_FREF_FIELD1_LOW_FIELD2_HIGH;

            bslTDA1997XSetVHRefValues (unit, &VHRefValues );

            ServiceMode |= (CLKX2_XTAL|CLK_XTAL|STD_PAL|INTPRO_PROG);

            break;

        case BSLHDMIRX_TESTPATTERN_480I:

            /* Set pixel and line number */
            /* 1, 858, 1, 525 for 480i60 see tmdlHdmiRx_cfg.c*/
            /* line preset is set to 1 instead of 4 for line counter free running */
            bslTDA1997XSetPixelAndLineCounters (unit, 1, 858, 1, 525);

            /* VHSyncValues */
            VHSyncValues.hsync_start = 1;
            VHSyncValues.hsync_end = 63;
            VHSyncValues.vsync_f1_pix_start = 1;
            VHSyncValues.vsync_f1_pix_end = 1;
            VHSyncValues.vsync_f1_start = 5;
            VHSyncValues.vsync_f1_width = 3;
            VHSyncValues.vsync_f2_pix_start = 429;
            VHSyncValues.vsync_f2_pix_end = 429;
            VHSyncValues.vsync_f2_start = 267;
            VHSyncValues.vsync_f2_width = 3;

            bslTDA1997XSetVHSyncValues(unit, &VHSyncValues);


            /* VHRefValues */
            /* {120, 840, 521, 22, 258, 23, 1, BSLHDMIRX_FREF_FIELD1_LOW_FIELD2_HIGH, 263} for 480i60 see tmdlHdmiRx_cfg.c*/
            VHRefValues.href_start = 120;
            VHRefValues.href_end = 840;
            VHRefValues.vref_f1_start = 1; 		/* WA to have VREF signal. Be careful that active video is one line earlier than DE */
            VHRefValues.vef_f1_width = 22;
            VHRefValues.vref_f2_start = 263;
            VHRefValues.vef_f2_width = 23;
            VHRefValues.fieldref_f1_start = 5;
            VHRefValues.fieldref_f2_start = 267;
            VHRefValues.fieldPolarity = BSLHDMIRX_FREF_FIELD1_LOW_FIELD2_HIGH;

            bslTDA1997XSetVHRefValues (unit, &VHRefValues );

            ServiceMode |= (CLKX2_XTAL|CLK_XTAL_DIV2|STD_NTSC|INTPRO_INT);

            break;

        case BSLHDMIRX_TESTPATTERN_576I:

            /* Set pixel and line number */
            /* 1, 864, 1, 625 for 480i60 see tmdlHdmiRx_cfg.c */
            bslTDA1997XSetPixelAndLineCounters (unit, 1, 864, 1, 625);

            /* VHSyncValues */
            VHSyncValues.hsync_start = 1;
            VHSyncValues.hsync_end = 64;
            VHSyncValues.vsync_f1_pix_start = 1;
            VHSyncValues.vsync_f1_pix_end = 1;
            VHSyncValues.vsync_f1_start = 1;
            VHSyncValues.vsync_f1_width = 3;
            VHSyncValues.vsync_f2_pix_start = 432;
            VHSyncValues.vsync_f2_pix_end = 432;
            VHSyncValues.vsync_f2_start = 313;
            VHSyncValues.vsync_f2_width = 3;

            bslTDA1997XSetVHSyncValues(unit, &VHSyncValues);


            /* VHRefValues */
            /* {133, 853, 623, 24, 310, 25, 1, BSLHDMIRX_FREF_FIELD1_LOW_FIELD2_HIGH, 313 } for 576p50 see tmdlHdmiRx_cfg.c */
            VHRefValues.href_start = 133;
            VHRefValues.href_end = 853;
            VHRefValues.vref_f1_start = 624; 	/* seems there is an error in values in tmdlHdmiRx_cfg.c */
            VHRefValues.vef_f1_width = 24;
            VHRefValues.vref_f2_start = 311;	/* seems there is an error in values in tmdlHdmiRx_cfg.c */
            VHRefValues.vef_f2_width = 25;
            VHRefValues.fieldref_f1_start = 1;
            VHRefValues.fieldref_f2_start = 313;
            VHRefValues.fieldPolarity = BSLHDMIRX_FREF_FIELD1_LOW_FIELD2_HIGH;

            bslTDA1997XSetVHRefValues (unit, &VHRefValues );

            ServiceMode |= (CLKX2_XTAL|CLK_XTAL_DIV2|STD_PAL|INTPRO_INT);

            break;

        default:
            break;
        }

        /* HS and VS from VHREF */
        //csVsHsSel = HS_VS_FROM_VHREF;
        HsPolSel = SYNC_POL_NEG | SYNC_FROM_VHREF;
        VsPolSel = SYNC_POL_NEG | SYNC_FROM_VHREF;
        DEPolSel = SYNC_POL_POS | SYNC_FROM_VHREF;

    }
    else
    {
        /* HS start, HS end, */
        /* VS field 1 pix start and width */
        /* VS field 1 line start and width */
        /* VS field 2 pix start and width */
        /* VS field 2 line start and width */
        /* skipped because they are not used => HS and VS from HDMI */

        /* HS and VS from HDMI */
        HsPolSel = SYNC_POL_POS | SYNC_FROM_HDMI;
        VsPolSel = SYNC_POL_POS | SYNC_FROM_HDMI;
        DEPolSel = SYNC_POL_POS | SYNC_FROM_HDMI;

        /* Disable service_mode */
        ServiceMode = SM_OFF;

    }

    /* Write HS_HREF_SEL, VS_VREF_SEL and DE_FREF_SEL registers */
    errCode = tmbslTDA1997XReadI2C(unit, (UInt16)HS_HREF_SEL, 1, &RegVal);
    RegVal = ((RegVal & 0xF8) | HsPolSel);
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)HS_HREF_SEL, 1, &RegVal);

    errCode = tmbslTDA1997XReadI2C(unit, (UInt16)VS_VREF_SEL, 1, &RegVal);
    RegVal = ((RegVal & 0xF8) | VsPolSel);
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)VS_VREF_SEL, 1, &RegVal);

    errCode = tmbslTDA1997XReadI2C(unit, (UInt16)DE_FREF_SEL, 1, &RegVal);
    RegVal = ((RegVal & 0xF8) | DEPolSel);
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)DE_FREF_SEL, 1, &RegVal);


    /* Write SERVICE_MODE register */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)SERVICE_MODE, 1, &ServiceMode);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    return TM_OK;
}

/*============================================================================*/
/*
    \brief Initialize the equalizer for an instance of an HDMI receiver.
           The register pages that configure the equalizer are written with
           the proper values.

    \param Unit            Receiver unit number

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is already initialised
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus

******************************************************************************/
static tmErrorCode_t
bslTDA1997XInitTMDS
(
    tmUnitSelect_t  unit
)
{
    /********************************************************
     *
     *          A  T  T  E  N  T  I  O  N
     *
     *
     *          Please do not change the default value
     *          (0x6978 => 1ms) of the register rate_reftim.
     *
     ********************************************************/

    tmErrorCode_t   errCode = TM_OK;
    UInt8           regValue;

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* TDA19972 WA for HW CR388*/
    if (RxHdmiConfig[unit].chipConfiguration.revision == 0)
    {
        regValue = (UInt8) 0x08 ;
        errCode = tmbslTDA1997XWriteI2C(unit, (UInt16) CGU_DEBUG_SEL, 1, &regValue);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
    }
    /* end of TDA19972 WA for HW CR388*/

    /* CLK_MIN_RATE_MSB register */
    regValue = (UInt8) CLK_MIN_RATE_MSB_VAL;
    errCode = tmbslTDA1997XWriteI2C(unit, (UInt16) CLK_MIN_RATE_MSB, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /* CLK_MIN_RATE_ISB register */
    regValue = (UInt8) CLK_MIN_RATE_ISB_VAL;
    errCode = tmbslTDA1997XWriteI2C(unit, (UInt16) CLK_MIN_RATE_ISB, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /* CLK_MIN_RATE_LSB register */
    regValue = (UInt8) CLK_MIN_RATE_LSB_VAL;
    errCode = tmbslTDA1997XWriteI2C(unit, (UInt16) CLK_MIN_RATE_LSB, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /* CLK_MAX_RATE_MSB register */
    regValue = (UInt8) CLK_MAX_RATE_MSB_VAL;
    errCode = tmbslTDA1997XWriteI2C(unit, (UInt16) CLK_MAX_RATE_MSB, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /* CLK_MAX_RATE_ISB register */
    regValue = (UInt8) CLK_MAX_RATE_ISB_VAL;
    errCode = tmbslTDA1997XWriteI2C(unit, (UInt16) CLK_MAX_RATE_ISB, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /* CLK_MAX_RATE_LSB register */
    regValue = (UInt8) CLK_MAX_RATE_LSB_VAL;
    errCode = tmbslTDA1997XWriteI2C(unit, (UInt16) CLK_MAX_RATE_LSB, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    if (RxHdmiConfig[unit].chipConfiguration.revision == 0)
    {
        /* WORKAROUND START: PR1308 */
        /* FIFO_LATENCY_CTRL register TO BE CHECKED IF REALLY NEEDED FOR N1...*/
        regValue = (UInt8) FIFO_LATENCY_CTRL_VAL;
        errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)FIFO_LATENCY_CTRL, 1, &regValue);
        RETIF(errCode != TM_OK, errCode)
        /* WORKAROUND END: PR1308 */
        /* WDL_CFG registers TO BE CHECKED IF REALLY NEEDED FOR N1...*/
        regValue = (UInt8) WDL_CFG_VAL;
        errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)WDL_CFG, 1, &regValue);
        RETIF(errCode != TM_OK, errCode)
    }
    else
    {
        /* WORKAROUND START: */
        /* FIFO_LATENCY_CTRL register setting to get audio quickly...*/
        regValue = (UInt8) FIFO_LATENCY_CTRL_VAL;
        errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)FIFO_LATENCY_CTRL, 1, &regValue);
        RETIF(errCode != TM_OK, errCode)
        /* WORKAROUND END: */

    }

    /* Page 11 */
    /* DC filter TDA19972 see page 13 dc_fil write 31 in */
    regValue = (UInt8) DC_FILTER_VAL;
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)DEEP_COLOR_CTRL, 1, &regValue);
    RETIF(errCode != TM_OK, errCode)

    return TM_OK;
}

/*============================================================================*/
/*
    \brief Read the status registers for interrupts on the chip.

    \param Unit            Receiver unit number
    \param regStatus       Byte with the status of each interrupt

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is already initialized
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus

******************************************************************************/
static tmErrorCode_t
bslTDA1997XReadInterruptStatusRegisters
(
    tmUnitSelect_t          unit,
    UInt8                   *pRegStatus
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           regValue;
    UInt8           regStatus = 0;
    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* regStatus byte: */
    /*   - bit 7 to 5: not used   */
    /*   - bit 4: 1 => sus state = 5, 0 => sus state != 5 */
    /*   - bit 3: 1 => clock stable on input D, 0 => clock not stable on input D */
    /*   - bit 2: 1 => clock stable on input C, 0 => clock not stable on input C */
    /*   - bit 1: 1 => clock stable on input B, 0 => clock not stable on input B */
    /*   - bit 0: 1 => clock stable on input A, 0 => clock not stable on input A */

    /* Activity detection must only be notified when stable_clk_x AND active_x bits are set to 1  */
    /* If only stable_clk_x bit is set to 1 but not active_x, it means that the TMDS clock is not */
    /* in the defined range, so activity detection must not be notified */
    /* => regStatus must be set to 0 in that case */
    /* If stable_clk_x bit is set to 0, regStatus must also be set to 0 */

    /* Read CLK_A_STATUS register */
    errCode = tmbslTDA1997XReadI2C(unit, (UInt16) CLK_A_STATUS, 1, &regValue);
    //sprintf(Msg,"clk-stat %x",regValue);
    //WriteLn(Msg);
    RETIF(errCode != TM_OK, errCode)

    /* When stable_clk_x is set to 1, check active_x bit */
    if( (regValue & MASK_CLK_STABLE) && !(regValue & MASK_CLK_ACTIVE) )
    {
        regValue = (UInt8) (regValue & ~MASK_CLK_STABLE);
    }

    regStatus |= (UInt8) ((regValue & MASK_CLK_STABLE) >> 2);

    /* Read CLK_B_STATUS register */
    errCode = tmbslTDA1997XReadI2C(unit, (UInt16) CLK_B_STATUS, 1, &regValue);
    RETIF(errCode != TM_OK, errCode)

    //sprintf(Msg,"clk-statB %x",regValue);
    //WriteLn(Msg);

    /* When stable_clk_x is set to 1, check active_x bit */
    if( (regValue & MASK_CLK_STABLE) && !(regValue & MASK_CLK_ACTIVE) )
    {
        regValue = (UInt8) (regValue & ~MASK_CLK_STABLE);
    }

    regStatus |= (UInt8) ((regValue & MASK_CLK_STABLE) >> 1);

    /* Read the SUS_STATUS register */
    errCode = tmbslTDA1997XReadI2C(unit, (UInt16)SUS_STATUS, 1, &regValue);
    RETIF(errCode !=TM_OK,errCode)

    //sprintf(Msg,"sus-stat %x",regValue);
    //WriteLn(Msg);

    /* If state = 5 => TMDS is locked */
    if( (regValue & MASK_SUS_STATE_VALUE) == LAST_STATE_REACHED)
    {
        regStatus |= MASK_SUS_STATE_BIT;
    }
    else
    {
        regStatus &= ~MASK_SUS_STATE_BIT;
    }

    *pRegStatus = regStatus;

    return TM_OK;
}

/*============================================================================*/
/*
    \brief Callback the activity detection interrupts.

    \param Unit            Receiver unit number
    \param IrqSource       Interrupt to be processed
    \param regStatus       Byte with the status of each interrupt

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is already initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range

******************************************************************************/
static tmErrorCode_t
bslTDA1997XActivityDetectionInterrupts
(
    tmUnitSelect_t          unit,
    tmbslHdmiRxIRQSource_t  irqSource,
    UInt8                   regStatus
)
{
    tmErrorCode_t   errCode = TM_OK;
    static UInt8	oldRegStatus = 0;
    UInt8           regValue;

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    switch(irqSource)
    {
    case BSLHDMIRX_IRQSOURCE_RXDA:
        if ((regStatus & MASK_RXDA_BIT)!= (oldRegStatus & MASK_RXDA_BIT))
        {
            if ((regStatus & MASK_RXDA_BIT)== 0) /* activity lost on input A */
            {
                if (RxHdmiConfig[unit].chipConfiguration.revision == 0)
                {
                    /* Clear HDMI mode flag in BCAPS (WA for N1)*/
                    regValue = 0x03;
                    errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)CLK_CFG, 1, &regValue );
                    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
                    regValue = 0x01;
                    errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)PON_OVR_EN, 1, &regValue );
                    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
                    regValue = 0x01;
                    errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)PON_CBIAS, 1, &regValue );
                    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
                    regValue = 0x01;
                    errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)PON_PLL, 1, &regValue );
                    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

                    errCode = tmbslTDA1997XReadI2C(unit, (UInt16)MODE_RECOVER_CFG1, 1, &regValue);
                    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)
                    regValue &= ~0x06;
                    regValue |= 0x02;
                    errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)MODE_RECOVER_CFG1, 1, &regValue );
                    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

                    regValue = 0x00;
                    errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)CLK_CFG, 1, &regValue );
                    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
                    regValue = 0x00;
                    errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)PON_OVR_EN, 1, &regValue );
                    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

                    errCode = tmbslTDA1997XReadI2C(unit, (UInt16)MODE_RECOVER_CFG1, 1, &regValue);
                    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)
                    regValue &= ~0x06;
                    errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)MODE_RECOVER_CFG1, 1, &regValue );
                    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
                    /* End of Clear HDMI mode flag in BCAPS (WA for N1)*/
                }
            }


            /* Callback with the "RXDA" IRQ source */
            RxHdmiConfig[unit].callbackFunc(BSLHDMIRX_IRQSOURCE_RXDA, (regStatus & MASK_RXDA_BIT));
            oldRegStatus = (oldRegStatus &~MASK_RXDA_BIT) | (regStatus & MASK_RXDA_BIT);
        }
        break;
    case BSLHDMIRX_IRQSOURCE_RXDB:
        if ((regStatus & MASK_RXDB_BIT)!= (oldRegStatus & MASK_RXDB_BIT))
        {
            if (((regStatus & MASK_RXDB_BIT) >> 1)== 0) /* activity lost on input A */
            {
                if (RxHdmiConfig[unit].chipConfiguration.revision == 0)
                {
                    /* Clear HDMI mode flag in BCAPS (WA for N1)*/
                    regValue = 0x03;
                    errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)CLK_CFG, 1, &regValue );
                    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
                    regValue = 0x01;
                    errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)PON_OVR_EN, 1, &regValue );
                    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
                    regValue = 0x01;
                    errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)PON_CBIAS, 1, &regValue );
                    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
                    regValue = 0x01;
                    errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)PON_PLL, 1, &regValue );
                    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

                    errCode = tmbslTDA1997XReadI2C(unit, (UInt16)MODE_RECOVER_CFG1, 1, &regValue);
                    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)
                    regValue &= ~0x06;
                    regValue |= 0x02;
                    errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)MODE_RECOVER_CFG1, 1, &regValue );
                    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

                    regValue = 0x00;
                    errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)CLK_CFG, 1, &regValue );
                    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
                    regValue = 0x00;
                    errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)PON_OVR_EN, 1, &regValue );
                    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

                    errCode = tmbslTDA1997XReadI2C(unit, (UInt16)MODE_RECOVER_CFG1, 1, &regValue);
                    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)
                    regValue &= ~0x06;
                    errCode = tmbslTDA1997XWriteI2C(unit, (UInt16)MODE_RECOVER_CFG1, 1, &regValue );
                    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)
                    /* End of Clear HDMI mode flag in BCAPS (WA for N1)*/
                }
            }
            /* Callback with the "RXDB" IRQ source */
            RxHdmiConfig[unit].callbackFunc(BSLHDMIRX_IRQSOURCE_RXDB, ( (regStatus & MASK_RXDB_BIT) >> 1) );
            oldRegStatus = (oldRegStatus &~MASK_RXDB_BIT) | (regStatus & MASK_RXDB_BIT);
        }
        break;
    default:
        errCode = TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER;
    }

    return errCode;
}

/*============================================================================*/
/*
    \brief This function checks periodically the audio frequency.

    \param Unit            Receiver unit number

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is already initialised
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus

******************************************************************************/
static tmErrorCode_t
bslTDA1997XCheckAudioFrequency
(
    tmUnitSelect_t  unit
)
{
    tmErrorCode_t   errCode;
    UInt8           susStatus, audioFreq;

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Since HW is not actually triggering an IRQ for an audio format change,
       it must be emulated by SW into the heartbeat function. Each heartbeat
       call, BSL will check if status changed and notify it with the
       BSLHDMIRX_IRQSOURCE_AUDIO_SAMPLE_FREQ_CHANGED IRQ source */

    /* Read the audio frequency register */
    /* WORKAROUND START: PR1296 */
    if(RxHdmiConfig[unit].audioTestModeUsed)
    {
        errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)TEST_AUDIO_FREQ, 1, &audioFreq);
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)
        audioFreq = (UInt8) ( (audioFreq & MASK_AUDIO_FREQ_P13) >> 1);
    }
    /* WORKAROUND END: PR1296 */
    else
    {
        errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)AUDIO_FREQ, 1, &audioFreq);
        RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)
        audioFreq = (UInt8) (audioFreq & MASK_AUDIO_FREQ);
    }

    if(audioFreq != RxHdmiConfig[unit].currentAudioFreq)
    {
        /* IRQ triggering must only be done if TMDS is locked */
        /* Read the SUS_STATUS register */
        errCode = tmbslTDA1997XReadI2C(unit, (UInt16)SUS_STATUS, 1, &susStatus);
        RETIF(errCode !=TM_OK,errCode)

        /* If state = 5 => TMDS is locked */
        if( (susStatus & MASK_SUS_STATE_VALUE) == LAST_STATE_REACHED )
        {
            /* Update with the new value */
            RxHdmiConfig[unit].currentAudioFreq = audioFreq;

            /* Callback for the AUDIO_SAMPLE_FREQ IRQ source */
            RxHdmiConfig[unit].callbackFunc(BSLHDMIRX_IRQSOURCE_AUDIO_SAMPLE_FREQ_CHANGED, 1);
        }
    }

    return TM_OK;
}

/*============================================================================*/
/*
    \brief This function checks periodically the VSI update counter.

    \param Unit            Receiver unit number

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is already initialised
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus

******************************************************************************/
static tmErrorCode_t
bslTDA1997XCheckVSIUpdate
(
    tmUnitSelect_t  unit
)
{
    tmErrorCode_t   errCode;
    UInt8           VSIUpdate, VSOther1Update, VSOther2Update;

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Read the Update registers */
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)VS_IF_HDMI_UPDATE, 1, &VSIUpdate);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)VS_IF_OTHER_BK1_UPDATE, 1, &VSOther1Update);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)VS_IF_OTHER_BK2_UPDATE, 1, &VSOther2Update);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

    /* No new VSI has been received if these 3 registers are greater than 3 (use 10 to take margin...) */
    if ((RxHdmiConfig[unit].VSIReceived) &&(VSIUpdate> 10) && (VSOther1Update >10) && (VSOther2Update>10))
    {
        RxHdmiConfig[unit].VSIReceived = False;
        /* in that case, fake VSI received */
        RxHdmiConfig[unit].callbackFunc(BSLHDMIRX_IRQSOURCE_VS_IF, 0);

    }

    return TM_OK;
}

/*============================================================================*/
/*
    \brief This function checks periodically the HDMI flags status.

    \param Unit            Receiver unit number

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is already initialised
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus

******************************************************************************/
static tmErrorCode_t
bslTDA1997XCheckHdmiFlags
(
    tmUnitSelect_t  unit
)
{
    tmErrorCode_t   errCode;
    UInt8           regValue, statusChange;

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Read the HDMI_FLAGS register */
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)HDMI_FLAGS, 1, &regValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

    /* Do not take into account fifo_fail and fifo_warning bits */
    regValue &= (UInt8) MASK_HDMI_FLAGS;

    statusChange = RxHdmiConfig[unit].hdmiFlagsStatus ^ regValue;

    if(statusChange)
    {
        /* WORKAROUND START: PR1308 */
        if(statusChange & MASK_LAYOUT_FLAG)
        {
            errCode = bslTDA1997XChangeAudioFifoLatency(unit);
            RETIF(errCode != TM_OK, errCode)
        }
        /* WORKAROUND END: PR1308 */

        /* Status has changed */
        RxHdmiConfig[unit].hdmiFlagsStatus = regValue;

        /* Callback for the BSLHDMIRX_IRQSOURCE_FLAGS IRQ source */
        RxHdmiConfig[unit].callbackFunc(BSLHDMIRX_IRQSOURCE_FLAGS, 1);
    }

    return TM_OK;
}

/* WORKAROUND START: PR1308 */
/*============================================================================*/
/*
    \brief This function changes the audio Fifo latency management according
           to the layout.

    \param Unit            Receiver unit number

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is already initialised
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus

******************************************************************************/
static tmErrorCode_t
bslTDA1997XChangeAudioFifoLatency
(
    tmUnitSelect_t  unit
)
{
    tmErrorCode_t   errCode;
    UInt8           regValue;

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Read the HDMI_FLAGS register to recover the layout */
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)HDMI_FLAGS, 1, &regValue);
    RETIF(errCode != TM_OK, errCode)

    /* Write the FIFO_LATENCY_CTRL register */
    regValue = (UInt8) ( !( (regValue & MASK_LAYOUT_FLAG) >> 2 ) | FIFO_LAT_CTRL_DEF_VAL);
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)FIFO_LATENCY_CTRL, 1, &regValue);
    RETIF(errCode != TM_OK, errCode)

    return TM_OK;
}
/* WORKAROUND END: PR1308 */

/*============================================================================*/
/*
    \brief This function restart the audio regulation in case of audio FIFO
           failure (FIFO read and write pointers are crossed).

    \param Unit            Receiver unit number

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is already initialised
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus

******************************************************************************/
static tmErrorCode_t
bslTDA1997XRestartRegulation
(
    tmUnitSelect_t  unit
)
{
    tmErrorCode_t   errCode;
    UInt8           hdmiInfoVal;

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Reset the audio FIFO */
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)HDMI_INFO_RST, 1, &hdmiInfoVal);
    RETIF(errCode != TM_OK, errCode)

    hdmiInfoVal |= RESET_AUDIO_FIFO;
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)HDMI_INFO_RST, 1, &hdmiInfoVal);

    RETIF(errCode != TM_OK, errCode)

    hdmiInfoVal = (UInt8) (hdmiInfoVal & ~RESET_AUDIO_FIFO);
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)HDMI_INFO_RST, 1, &hdmiInfoVal);

    RETIF(errCode != TM_OK, errCode)

    return TM_OK;
}


#ifdef TMFL_CALIBRATION_OPT
/*============================================================================*/
/*
    \brief This function forces the equalizer gain.

    \param Unit            Receiver unit number

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is already initialised
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus

******************************************************************************/
static tmErrorCode_t
bslTDA1997XForceEqualizerGain
(
    tmUnitSelect_t  unit
)
{
    tmErrorCode_t   errCode;
    UInt8           regValue, gainMode, gainMin = 7, i;
    UInt8           gainRes[3];

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Get the gain mode */
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)GAIN_OVR_EN, 1, &gainMode);
    RETIF(errCode != TM_OK, errCode)

    if(gainMode == GAIN_MODE_MANUAL)
    {
        /* Gain is in manual control mode */
        /* Set the gain in automatic calibration mode */
        regValue = GAIN_MODE_AUTO;
        errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)GAIN_OVR_EN, 1, &regValue);
        RETIF(errCode != TM_OK, errCode)

        /* Reset SUS */
        errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)RATE_CTRL, 1, &regValue);
        regValue &= ~RATE_REFTIM_ENABLE;
        errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)RATE_CTRL, 1, &regValue);

        /* Wait during 1 ms */
        errCode = RxHdmiConfig[unit].sysFuncTimer(1);
        RETIF(errCode != TM_OK, errCode)

        errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)RATE_CTRL, 1, &regValue);
        regValue |= RATE_REFTIM_ENABLE;
        errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)RATE_CTRL, 1, &regValue);

        RxHdmiConfig[unit].susResetBySW = True;
    }
    else
    {
        /* Gain is in automatic calibration mode */
        /* Read the gain for each channel */
        errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)GAIN_CH0, 3, gainRes);
        RETIF(errCode != TM_OK, errCode)

        for(i = 0; i < 3; i++)
        {
            if(gainRes[i] < gainMin)
            {
                gainMin = gainRes[i];
            }
        }

        gainRes[0] = gainMin;
        gainRes[1] = gainMin;
        gainRes[2] = gainMin;

        /* Write the gain for each channel */
        errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)GAIN_CH0_OVRD, 3, gainRes);
        RETIF(errCode != TM_OK, errCode)

        /* Set the gain in manual control mode */
        regValue = GAIN_MODE_MANUAL;
        errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)GAIN_OVR_EN, 1, &regValue);
        RETIF(errCode != TM_OK, errCode)

        RxHdmiConfig[unit].susResetBySW = False;
    }

    return TM_OK;
}
#endif
/* WORKAROUND END: PR1633 */


/*============================================================================*/
/**
    \brief Get audio channel status.

    \param unit        Receiver unit number
    \param pChanStatus Audio channel status

    \return The call result:
            - TM_OK  : the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading to the I2C bus
            - TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED: this function is not supported on the
              current configured IC.

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XGetChannelStatus
(
    tmUnitSelect_t  unit,
    UInt8           *pChanStatus
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           int_audio_flags;

    /* test the unit number */
    RETIF(unit >= MAX_UNIT, TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Test pointers <> NULL */
    RETIF(pChanStatus == Null, TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* read FIFO error audio flag before reading channel status*/
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)INT_FLG_CLR_AUDIO, 1, &int_audio_flags);
    RETIF(errCode !=TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_READ)
    RETIF((int_audio_flags & MASK_FIFO_ERR), TMBSL_ERR_BSLHDMIRX_BUSY)

    /* read channel status */
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)CH_STAT_BYTE0, 12, pChanStatus);
    RETIF(errCode !=TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_READ)

    /* read FIFO error audio flag again after have read channel status*/
    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)INT_FLG_CLR_AUDIO, 1, &int_audio_flags);
    RETIF(errCode !=TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_READ)
    RETIF((int_audio_flags & MASK_FIFO_ERR), TMBSL_ERR_BSLHDMIRX_BUSY)


    return TM_OK;
}

/*============================================================================*/
/**
    \brief Configure the internal EDID module. Allows to configure all
           internal EDID parameters : clocking, power modes, etc.

    \param Unit             Receiver unit number
    \param nackEdid         Enable/disable EDID acknowledgment
    \param resetEdid        Reset EDID module
    \param powerDownEdid    Powerdown EDID I2C access module
    \param powerDownEdidDDC Powerdown EDID DDC access module

    \return The call result:
            - TM_OK  : the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED: this function is not supported on the
              current configured IC.

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XConfigureEDID
(
    tmUnitSelect_t                unit,
    tmbslHdmiRxEDIDNack_t         nackEdid,
    tmbslHdmiRxEDIDReset_t        resetEdid,		/* ignored */
    tmbslHdmiRxEDIDPowerDown_t    powerDownEdid,    /* ignored */
    tmbslHdmiRxEDIDPowerDownDDC_t powerDownEdidDDC	/* ignored */
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           regValue = 0;

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Read EDID ENABLE register */
    errCode =  tmbslTDA1997XReadI2C(unit, EDID_ENABLE, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_READ)

    /* EDID ENABLE register */
    regValue &= ~0x83;
    regValue |= (nackEdid == BSLHDMIRX_EDID_NACK_ON) ? 0x00 : 0x83;
    //regValue |= (powerDownEdid == BSLHDMIRX_EDID_PD_ON) ? 0x00 : 0x40;

    /* Write EDID ENABLE register */
    errCode =  tmbslTDA1997XWriteI2C(unit, EDID_ENABLE, 1, &regValue);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    return errCode;
}

/*============================================================================*/
/**
    \brief Load EDID data into the internal EDID memory. Note that on TDA1997X.

    \param Unit       Receiver unit number
    \param commonEDID Pointer to the two block EDID (array of 256 bytes)
                      that will be common to all HDMI inputs.
    \param sPA        Pointer to the array of 4 source physical addresses (a
                      total of 8 bytes written in big endian order).
                      The first element of this array corresponds to HDMI input A.
    \param checksum   Pointer to the array of 5 checksums (a total of 5
                      bytes). The first element of this array corresponds
                      to the block 0 checksum and the following elements to
                      block 1 checksums for HDMI input A, B, C and D.
    \param sPAOffset  Offset of the first SPA byte inside EDID block 1
                      (offset is the same for all HDMI inputs: A, B, C and D).

    \return The call result:
            - TM_OK  : the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED: this function is not supported on the
              current configured IC.

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XLoadEDIDData
(
    tmUnitSelect_t unit,
    UInt8          *commonEDID,
    UInt16         *sPA,
    UInt8          *checksum,
    UInt8          sPAOffset
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           srcPhyAdd[4];

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* Write EDID data 0 to 126 registers */
    errCode =  tmbslTDA1997XWriteI2C(unit, EDID_IN_BYTE0, 127, commonEDID);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /* Write EDID data 127 (checksum) */
    errCode =  tmbslTDA1997XWriteI2C(unit, EDID_IN_BYTE127, 1, checksum);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /* Write EDID data 128 to 255 registers */
    errCode =  tmbslTDA1997XWriteI2C(unit, EDID_IN_BYTE128, 128, &commonEDID[128]);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    srcPhyAdd[0] = (UInt8)((sPA[0] & 0xFF00)>>8);
    srcPhyAdd[1] = (UInt8)(sPA[0] & 0x00FF);
    srcPhyAdd[2] = (UInt8)((sPA[1] & 0xFF00)>>8);
    srcPhyAdd[3] = (UInt8)(sPA[1] & 0x00FF);

    /* Write Source Physical Address subaddress register (position of the sPA in block 1) */
    errCode =  tmbslTDA1997XWriteI2C(unit, EDID_IN_SPA_SUB, 1, &sPAOffset);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /* Write Source Physical Address registers for input A*/
    errCode =  tmbslTDA1997XWriteI2C(unit, EDID_IN_SPA_AB_A, 2, srcPhyAdd);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /* Write Checksum register for input A */
    errCode =  tmbslTDA1997XWriteI2C(unit, EDID_IN_CHECKSUM_A, 1, &checksum[1]);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /* Write Source Physical Address registers for input B*/
    errCode =  tmbslTDA1997XWriteI2C(unit, EDID_IN_SPA_AB_B, 2, &srcPhyAdd[2]);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /* Write Checksum register for input B */
    errCode =  tmbslTDA1997XWriteI2C(unit, EDID_IN_CHECKSUM_B, 1, &checksum[2]);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    return errCode;
}

/*============================================================================*/
/*                         TDA19972 and TDA19973 only                         */
/*============================================================================*/



/*============================================================================*/
/**
    \brief Load secondary EDID data into the internal EDID memory (pages 0x22 & 0x23).

    \param Unit       Receiver unit number
    \param commonEDID Pointer to the two block EDID (array of 256 bytes)
                      that will be common to all HDMI inputs.
    \param sPA        Pointer to the array of 4 source physical addresses (a
                      total of 8 bytes written in big endian order).
                      The first element of this array corresponds to HDMI input A.
    \param checksum   Pointer to the array of 5 checksums (a total of 5
                      bytes). The first element of this array corresponds
                      to the block 0 checksum and the following elements to
                      block 1 checksums for HDMI input A, B, C and D.
    \param sPAOffset  Offset of the first SPA byte inside EDID block 1
                      (offset is the same for all HDMI inputs: A, B, C and D).

    \return The call result:
            - TM_OK  : the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED: this function is not supported on the
              current configured IC.

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XLoadSecondaryEDIDData
(
    tmUnitSelect_t unit,
    UInt8          *commonEDID,
    UInt16         *sPA,
    UInt8          *checksum,
    UInt8          sPAOffset
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           srcPhyAdd[4];

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)


    /* Write EDID data 0 to 126 registers */
    errCode =  tmbslTDA1997XWriteI2C(unit, EDID_OUT_BYTE0, 127, commonEDID);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /* Write EDID data 127 (checksum) */
    errCode =  tmbslTDA1997XWriteI2C(unit, EDID_OUT_BYTE127, 1, checksum);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /* Write EDID data 128 to 255 registers */
    errCode =  tmbslTDA1997XWriteI2C(unit, EDID_OUT_BYTE128, 128, &commonEDID[128]);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    srcPhyAdd[0] = (UInt8)((sPA[0] & 0xFF00)>>8);
    srcPhyAdd[1] = (UInt8)(sPA[0] & 0x00FF);
    srcPhyAdd[2] = (UInt8)((sPA[1] & 0xFF00)>>8);
    srcPhyAdd[3] = (UInt8)(sPA[1] & 0x00FF);

    /* Write Source Physical Address subaddress register (position of the sPA in block 1) */
    errCode =  tmbslTDA1997XWriteI2C(unit, EDID_OUT_SPA_SUB, 1, &sPAOffset);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /* Write Source Physical Address registers for input A*/
    errCode =  tmbslTDA1997XWriteI2C(unit, EDID_OUT_SPA_AB_A, 2, srcPhyAdd);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /* Write Checksum register for input A */
    errCode =  tmbslTDA1997XWriteI2C(unit, EDID_OUT_CHECKSUM_A, 1, &checksum[1]);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /* Write Source Physical Address registers for input A*/
    errCode =  tmbslTDA1997XWriteI2C(unit, EDID_OUT_SPA_AB_B, 2, &srcPhyAdd[2]);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /* Write Checksum register for input A */
    errCode =  tmbslTDA1997XWriteI2C(unit, EDID_OUT_CHECKSUM_B, 1, &checksum[2]);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    return errCode;
}

/*============================================================================*/
/**
    \brief Reloads page with Data from MTP.

    \param instance   Instance identifier.
    		page		Page to write: either 0x20 or 0x21

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent
            - TMDL_ERR_DLHDMIRX_OPERATION_NOT_PERMITTED: devlib is not
              configured to use internal EDID.

******************************************************************************/
tmErrorCode_t tmbslTDA1997XReLoadEdidDataFromMTP
(
    tmUnitSelect_t unit
)
{
    UInt8 regVal, subAddr;
    UInt32 timeout;
    tmErrorCode_t   errCode = TM_OK;

    if (RxHdmiConfig[unit].MPTRWInProgress != True)
    {
        /* Clear interrupt flags */
        regVal = 0xFF;
        for(subAddr = 0x0F; subAddr < 0x17; subAddr++)
        {
            errCode = tmbslTDA1997XWriteI2C(unit, ACC_REG( subAddr, 0x00), 1, &regVal);
            if (errCode) return TM_ERR_BAD_PARAMETER;
        }

        /* E-MTP Read All start */
        regVal = 0x01;
        errCode = tmbslTDA1997XWriteI2C(unit, EMTP_CTRL, 1, &regVal);
        if (errCode) return TM_ERR_BAD_PARAMETER;

        /* Wait interrupt flag */
        timeout = 0;
        subAddr = 0x10;
        do
        {
            errCode = tmbslTDA1997XReadI2C(unit, INT_FLG_CLR_DDC, 1, &regVal);
            if (errCode) return TM_ERR_BAD_PARAMETER;
            RxHdmiConfig[unit].sysFuncTimer(200);
            timeout += 200;
        }
        while ((!(regVal & 0x80)) && (timeout < 5000));
        if (timeout == 5000)
        {
            //PRNT("/!\\Read E-MTP Error: Timeout of waiting E-MTP flag\n");
            return TM_ERR_BAD_PARAMETER;
        }

        /* Clear interrupt flag */
        regVal = 0x80;
        errCode = tmbslTDA1997XWriteI2C(unit, INT_FLG_CLR_DDC, 1, &regVal);
    }
    else
    {
        errCode = TMBSL_ERR_BSLHDMIRX_BUSY;
    }

    return errCode;
}


/*============================================================================*/
/**
    \brief Save EDID data to MTP.

    \param 	instance   Instance identifier.
    		page		Page to write: either 0x20 or 0x21
    		tabvalue	Pointer to a tab containing the 140 values to write

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent
            - TMDL_ERR_DLHDMIRX_OPERATION_NOT_PERMITTED: devlib is not
              configured to use internal EDID.

******************************************************************************/
tmErrorCode_t tmbslTDA1997XSaveEDIDDataToMTP
(
    tmUnitSelect_t unit
)
{
    UInt8 regVal, subAddr;
    UInt32 timeout;
    tmErrorCode_t   errCode = TM_OK;

    if (RxHdmiConfig[unit].MPTRWInProgress != True)
    {
        /* Work-around Single Row Erase */
        regVal = 0x04;
        errCode = tmbslTDA1997XWriteI2C(unit, DDC_HIDDEN0_REGISTER, 1, &regVal);
        if (errCode) return TM_ERR_BAD_PARAMETER;

        /* Clear interrupt flags */
        regVal = 0xFF;
        for(subAddr = 0x0F; subAddr < 0x17; subAddr++)
        {
            errCode = tmbslTDA1997XWriteI2C(unit, ACC_REG( subAddr, 0x00), 1, &regVal);
            if (errCode) return TM_ERR_BAD_PARAMETER;
        }

        /* E-MTP Write All start */
        regVal = 0x02;
        errCode = tmbslTDA1997XWriteI2C(unit, EMTP_CTRL, 1, &regVal);
        if (errCode) return TM_ERR_BAD_PARAMETER;

        /* Wait interrupt flag */
        timeout = 0;
        do
        {
            errCode = tmbslTDA1997XReadI2C(unit, INT_FLG_CLR_DDC, 1, &regVal);
            if (errCode) return TM_ERR_BAD_PARAMETER;
            RxHdmiConfig[unit].sysFuncTimer(200);
            timeout += 200;
        }
        while ((!(regVal & 0x80)) && (timeout < 5000));
        if (timeout == 5000)
        {
            WriteLn("/!\\Write E-MTP Error: Timeout of waiting E-MTP flag\n");
            return TM_ERR_BAD_PARAMETER;
        }

        /* Clear interrupt flag */
        regVal = 0x80;
        errCode = tmbslTDA1997XWriteI2C(unit, INT_FLG_CLR_DDC, 1, &regVal);
        if (errCode) return TM_ERR_BAD_PARAMETER;
    }
    else
    {
        errCode = TMBSL_ERR_BSLHDMIRX_BUSY;
    }

    return errCode;
}

/*============================================================================*/
/**
    \brief Loads DDC and RT configuration data into embedded memory of receiver device.

    \param instance   Instance identifier.
    \param DDC_Config	Pointer to the DDC block configuration (array of 8 bytes)
    \param RT_Config	Pointer to the RT block configuration (array of 6 bytes)

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range


 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XLoadConfigData
(
    tmUnitSelect_t unit,
    UInt8          *DDC_Config,
    UInt8          *RT_Config
)
{
    tmErrorCode_t   errCode = TM_OK;

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)

    /* write each DDC config Byte of page 20h */
    errCode =  tmbslTDA1997XWriteI2C(unit, EDID_IN_VERSION, 8, DDC_Config);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    /* write each RT config Byte of page 30h */
    errCode =  tmbslTDA1997XWriteI2C(unit, RT_AUTO_CTRL, 6, RT_Config);
    RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    return errCode;
}

/*============================================================================*/
/**
    \brief Reloads page with Data from MTP.

    \param instance   Instance identifier.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent
            - TMDL_ERR_DLHDMIRX_OPERATION_NOT_PERMITTED: devlib is not
              configured to use internal EDID.

******************************************************************************/
tmErrorCode_t tmbslTDA1997XReLoadConfigDataFromMTP
(
    tmUnitSelect_t unit
)
{
    UInt8 regVal, subAddr;
    UInt32 timeout;
    tmErrorCode_t   errCode = TM_OK;

    if (RxHdmiConfig[unit].MPTRWInProgress != True)
    {
        /* Clear interrupt flags */
        regVal = 0xFF;
        for(subAddr = 0x0F; subAddr < 0x17; subAddr++)
        {
            errCode = tmbslTDA1997XWriteI2C(unit, ACC_REG( subAddr, 0x00), 1, &regVal);
            if (errCode) return TM_ERR_BAD_PARAMETER;
        }

        /* C-MTP Read All start */
        regVal = 0x01;
        errCode = tmbslTDA1997XWriteI2C(unit, ACC_REG( 0xFC, 0x00), 1, &regVal);
        if (errCode) return TM_ERR_BAD_PARAMETER;

        /* Wait interrupt flag */
        timeout = 0;
        do
        {
            errCode = tmbslTDA1997XReadI2C(unit, ACC_REG( 0x0F, 0x00), 1, &regVal);
            if (errCode) return TM_ERR_BAD_PARAMETER;
            RxHdmiConfig[unit].sysFuncTimer(200);
            timeout += 200;
        }
        while ((!(regVal & 0x80)) && (timeout < 5000));
        if (timeout == 5000)
        {
            WriteLn("/!\\Read C-MTP Error: Timeout of waiting C-MTP flag\n");
            return TM_ERR_BAD_PARAMETER;
        }

        /* Clear interrupt flag */
        regVal = 0x80;
        errCode = tmbslTDA1997XWriteI2C(unit, ACC_REG( 0x0F, 0x00), 1, &regVal);
    }
    else
    {
        errCode = TMBSL_ERR_BSLHDMIRX_BUSY;
    }

    return errCode;
}


/*============================================================================*/
/**
    \brief Save Control data to MTP.

    \param 	instance   Instance identifier.
    		tabvalue	Pointer to a tab containing the 16 values to write

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent
            - TMDL_ERR_DLHDMIRX_OPERATION_NOT_PERMITTED: devlib is not
              configured to use internal EDID.

******************************************************************************/
tmErrorCode_t tmbslTDA1997XSaveConfigDataToMTP
(
    tmUnitSelect_t unit
)
{
    UInt8 regVal, subAddr;
    UInt32 timeout;
    tmErrorCode_t   errCode = TM_OK;

    if (RxHdmiConfig[unit].MPTRWInProgress != True)
    {
        ///* Work-around Single Row Erase */
        //regVal = 0x04;
        //errCode = tmbslTDA1997XWriteI2C(unit, DDC_HIDDEN0_REGISTER, 1, &regVal);
        //if (errCode) return TM_ERR_BAD_PARAMETER;

        /* Clear interrupt flags */
        regVal = 0xFF;
        for(subAddr = 0x0F; subAddr < 0x17; subAddr++)
        {
            errCode = tmbslTDA1997XWriteI2C(unit, ACC_REG( subAddr, 0x00), 1, &regVal);
            if (errCode) return TM_ERR_BAD_PARAMETER;
        }

        /* C-MTP Write All start */
        regVal = 0x02;
        errCode = tmbslTDA1997XWriteI2C(unit, ACC_REG( 0xFC, 0x21), 1, &regVal);
        if (errCode) return TM_ERR_BAD_PARAMETER;

        /* Wait interrupt flag */
        timeout = 0;
        do
        {
            errCode = tmbslTDA1997XReadI2C(unit, ACC_REG( 0x0F, 0x21), 1, &regVal);
            if (errCode) return TM_ERR_BAD_PARAMETER;
            RxHdmiConfig[unit].sysFuncTimer(200);
            timeout += 200;
        }
        while ((!(regVal & 0x80)) && (timeout < 5000));
        if (timeout == 5000)
        {
            WriteLn("/!\\Write C-MTP Error: Timeout of waiting C-MTP flag\n");
            return TM_ERR_BAD_PARAMETER;
        }

        /* Clear interrupt flag */
        regVal = 0x80;
        errCode = tmbslTDA1997XWriteI2C(unit, ACC_REG( 0x0F, 0x21), 1, &regVal);
        if (errCode) return TM_ERR_BAD_PARAMETER;
    }
    else
    {
        errCode = TMBSL_ERR_BSLHDMIRX_BUSY;
    }

    return errCode;
}


/*============================================================================*/
/**
    \brief Read EDID data.

    \param instance   Instance identifier.
    \param EDIDData   Pointer to memory location where to write the two block EDID
    				  (array of 256 bytes). If null pointer is provided, only sPA data
    				  are returned.
    \param sPA        Pointer to the array of 2 source physical addresses (a
                      total of 4 bytes written in big endian order).
                      The first element of this array corresponds to HDMI input A.
    \param sPAOffset  Pointer to the offset of the first SPA byte inside EDID block 1
                      (offset is the same for the two inputs: A and B).

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent
            - TMDL_ERR_DLHDMIRX_OPERATION_NOT_PERMITTED: devlib is not
              configured to use internal EDID.

******************************************************************************/
tmErrorCode_t tmbslTDA1997XReadEdidData
(
    tmUnitSelect_t unit,
    UInt8         *commonEDID,
    UInt16        *sPA,
    UInt8         *checksum,
    UInt8         *sPAOffset
)

{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           srcPhyAdd[4];

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)


    if (commonEDID != Null)
    {
        /* Read EDID data 0 to 127 registers */
        errCode =  tmbslTDA1997XReadI2C(unit, EDID_IN_BYTE0, 128, commonEDID);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_READ)

        /* Read EDID data 128 to 255 registers */
        errCode =  tmbslTDA1997XReadI2C(unit, EDID_IN_BYTE128, 128, &commonEDID[128]);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_READ)
    }

    if (sPAOffset != Null)
    {
        /* Read Source Physical Address subaddress register (position of the sPA in block 1) */
        errCode =  tmbslTDA1997XReadI2C(unit, EDID_IN_SPA_SUB, 1, sPAOffset);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_READ)
    }

    if (sPA != Null)
    {
        /* Read Source Physical Address registers for input A*/
        errCode =  tmbslTDA1997XReadI2C(unit, EDID_IN_SPA_AB_A, 2, srcPhyAdd);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_READ)

        /* Read Source Physical Address registers for input A*/
        errCode =  tmbslTDA1997XReadI2C(unit, EDID_IN_SPA_AB_B, 2, &srcPhyAdd[2]);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_READ)

        sPA[0] = ((srcPhyAdd[0] << 8) | srcPhyAdd[1]);
        sPA[1] = ((srcPhyAdd[2] << 8) | srcPhyAdd[3]);
    }

    if (checksum != Null)
    {
        /* Read EDID data 127 (checksum) */
        errCode =  tmbslTDA1997XReadI2C(unit, EDID_IN_BYTE127, 1, checksum);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_READ)

        /* Read Checksum register for input A */
        errCode =  tmbslTDA1997XReadI2C(unit, EDID_IN_CHECKSUM_A, 1, &checksum[1]);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_READ)

        /* Read Checksum register for input A */
        errCode =  tmbslTDA1997XReadI2C(unit, EDID_IN_CHECKSUM_B, 1, &checksum[2]);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_READ)
    }

    return errCode;
}



/*============================================================================*/
/**
    \brief Read secondary EDID data.

    \param instance   Instance identifier.
    \param EDIDData   Pointer to memory location where to write the two block EDID
    				  (array of 256 bytes). If null pointer is provided, only sPA data
    				  are returned.
    \param sPA        Pointer to the array of 2 source physical addresses (a
                      total of 4 bytes written in big endian order).
                      The first element of this array corresponds to HDMI input A.
    \param sPAOffset  Pointer to the offset of the first SPA byte inside EDID block 1
                      (offset is the same for the two inputs: A and B).

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent
            - TMDL_ERR_DLHDMIRX_OPERATION_NOT_PERMITTED: devlib is not
              configured to use internal EDID.

******************************************************************************/
tmErrorCode_t tmbslTDA1997XReadSecondaryEdidData
(
    tmUnitSelect_t unit,
    UInt8         *commonEDID,
    UInt16        *sPA,
    UInt8         *checksum,
    UInt8         *sPAOffset
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           srcPhyAdd[4];

    /* test the unit number */
    RETIF(unit >= MAX_UNIT,TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER)


    if (commonEDID != Null)
    {
        /* Read EDID data 0 to 127 registers */
        errCode =  tmbslTDA1997XReadI2C(unit, EDID_OUT_BYTE0, 128, commonEDID);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_READ)

        /* Read EDID data 128 to 255 registers */
        errCode =  tmbslTDA1997XReadI2C(unit, EDID_OUT_BYTE128, 128, &commonEDID[128]);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_READ)
    }

    if (sPAOffset != Null)
    {
        /* Read Source Physical Address subaddress register (position of the sPA in block 1) */
        errCode =  tmbslTDA1997XReadI2C(unit, EDID_OUT_SPA_SUB, 1, sPAOffset);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_READ)
    }

    if (sPA != Null)
    {
        /* Read Source Physical Address registers for input A*/
        errCode =  tmbslTDA1997XReadI2C(unit, EDID_OUT_SPA_AB_A, 2, srcPhyAdd);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_READ)

        /* Read Source Physical Address registers for input A*/
        errCode =  tmbslTDA1997XReadI2C(unit, EDID_OUT_SPA_AB_B, 2, &srcPhyAdd[2]);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_READ)

        sPA[0] = ((srcPhyAdd[0] << 8) | srcPhyAdd[1]);
        sPA[1] = ((srcPhyAdd[2] << 8) | srcPhyAdd[3]);
    }

    if (checksum != Null)
    {
        /* Read EDID data 127 (checksum) */
        errCode =  tmbslTDA1997XReadI2C(unit, EDID_OUT_BYTE127, 1, checksum);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_READ)

        /* Read Checksum register for input A */
        errCode =  tmbslTDA1997XReadI2C(unit, EDID_OUT_CHECKSUM_A, 1, &checksum[1]);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_READ)

        /* Read Checksum register for input A */
        errCode =  tmbslTDA1997XReadI2C(unit, EDID_OUT_CHECKSUM_B, 1, &checksum[2]);
        RETIF(errCode != TM_OK, TMBSL_ERR_BSLHDMIRX_I2C_READ)
    }

    return errCode;
}



/*============================================================================*/
/**
    \brief Attach EDID data to input indicated. It can be attach to 0,
     	   one or two inputs.

    \param instance   Instance identifier.
    \param inputSel   Input to be attached to the seconadry Edid

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent
            - TMDL_ERR_DLHDMIRX_OPERATION_NOT_PERMITTED: devlib is not
              configured to use internal EDID.

******************************************************************************/
tmErrorCode_t tmbslTDA1997XAttachEdidToInput
(
    tmUnitSelect_t unit,
    tmbslHdmiRxInputSelDigitalSource_t  inputSel
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8 BlockSelect;

    switch (inputSel)
    {
    case BSLHDMIRX_DIGITALINPUT_A:
        errCode =  tmbslTDA1997XReadI2C(unit, EDID_BLOCK_SELECT, 1, &BlockSelect);
        if (errCode == TM_OK)
        {
            BlockSelect &= 0xF0;
            BlockSelect |= 0x04;
            errCode =  tmbslTDA1997XWriteI2C(unit, EDID_BLOCK_SELECT, 1, &BlockSelect);
        }
        break;
    case BSLHDMIRX_DIGITALINPUT_B:
        errCode =  tmbslTDA1997XReadI2C(unit, EDID_BLOCK_SELECT, 1, &BlockSelect);
        if (errCode == TM_OK)
        {
            BlockSelect &= 0x0F;
            BlockSelect |= 0x40;
            errCode =  tmbslTDA1997XWriteI2C(unit, EDID_BLOCK_SELECT, 1, &BlockSelect);
        }
        break;
    default:
        errCode = TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER;
        break;
    }

    return (errCode);
}


/*============================================================================*/
/**
    \brief Attach secondary EDID data to input indicated. It can be attach to 0,
     	   one or two inputs.

    \param instance   Instance identifier.
    \param inputSel   Input to be attached to the seconadry Edid

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent
            - TMDL_ERR_DLHDMIRX_OPERATION_NOT_PERMITTED: devlib is not
              configured to use internal EDID.

******************************************************************************/
tmErrorCode_t tmbslTDA1997XAttachSecondaryEdidToInput
(
    tmUnitSelect_t unit,
    tmbslHdmiRxInputSelDigitalSource_t  inputSel
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8 BlockSelect;

    switch (inputSel)
    {
    case BSLHDMIRX_DIGITALINPUT_A:
        errCode =  tmbslTDA1997XReadI2C(unit, EDID_BLOCK_SELECT, 1, &BlockSelect);
        if (errCode == TM_OK)
        {
            BlockSelect &= 0xF0;
            BlockSelect |= 0x0E;
            errCode =  tmbslTDA1997XWriteI2C(unit, EDID_BLOCK_SELECT, 1, &BlockSelect);
        }
        break;
    case BSLHDMIRX_DIGITALINPUT_B:
        errCode =  tmbslTDA1997XReadI2C(unit, EDID_BLOCK_SELECT, 1, &BlockSelect);
        if (errCode == TM_OK)
        {
            BlockSelect &= 0x0F;
            BlockSelect |= 0xE0;
            errCode =  tmbslTDA1997XWriteI2C(unit, EDID_BLOCK_SELECT, 1, &BlockSelect);
        }
        break;
    default:
        errCode = TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER;
        break;
    }

    return (errCode);
}

/*============================================================================*/
/**
    \brief Manual HPD control.

    \param instance   Instance identifier.
    \param inputSel   Input to be attached to the seconadry Edid

    \return The call result:
            - TM_OK: the call was successful

******************************************************************************/
tmErrorCode_t tmbslTDA1997XManualHPD
(
    tmUnitSelect_t unit,
    tmbslHdmiHPDManual_t  ManualHPD
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8 HPD_auto;
    UInt8 HPD_power;
    UInt8 HPD_man;

    errCode =  tmbslTDA1997XReadI2C(unit, HPD_AUTO_CTRL, 1, &HPD_auto);
    errCode =  tmbslTDA1997XReadI2C(unit, HPD_POWER, 1, &HPD_power);
    errCode =  tmbslTDA1997XReadI2C(unit, HPD_MAN_CTRL, 1, &HPD_man);
    HPD_man &= 0x87;

    switch (ManualHPD)
    {
    case BSLHDMIRX_HPD_LOW_BP:
        HPD_man &= ~0x03;
        HPD_power &= ~0x0C;
        /* write HPD_POWER so that HPD is low and write HPD_MAN ensure HPD pulse fo at least 100ms*/
        errCode |=  tmbslTDA1997XWriteI2C(unit, HPD_POWER, 1, &HPD_power);
        errCode |=  tmbslTDA1997XWriteI2C(unit, HPD_MAN_CTRL, 1, &HPD_man);
        break;
    case BSLHDMIRX_HPD_HIGH_BP:
        HPD_power &= ~0x0C;
        HPD_power |= 0x04;
        /* write HPD_POWER */
        errCode |=  tmbslTDA1997XWriteI2C(unit, HPD_POWER, 1, &HPD_power);
        break;
    case BSLHDMIRX_HPD_LOW_OTHER:
        HPD_man &= ~0x03;
        HPD_auto &= ~0x10;
        /* write HPD_AUTO so that HPD is low and write HPD_MAN ensure HPD pulse fo at least 100ms*/
        errCode |=  tmbslTDA1997XWriteI2C(unit, HPD_AUTO_CTRL, 1, &HPD_auto);
        errCode |=  tmbslTDA1997XWriteI2C(unit, HPD_MAN_CTRL, 1, &HPD_man);
        break;
    case BSLHDMIRX_HPD_HIGH_OTHER:
        HPD_auto |= 0x10;
        /* write HPD_AUTO */
        errCode |=  tmbslTDA1997XWriteI2C(unit, HPD_AUTO_CTRL, 1, &HPD_auto);
        break;
    case BSLHDMIRX_HPD_PULSE:
    default:
        HPD_man &= ~0x03;
        /* write HPD_MAN to have HPD low pulse*/
        errCode |=  tmbslTDA1997XWriteI2C(unit, HPD_MAN_CTRL, 1, &HPD_man);
        break;
    }

    return (errCode);
}


/*------------------------------------------------ Local functions ---------------------------------------------------------*/

/*******************************************************************************/
tmErrorCode_t
bslTDA1997XSetPixelAndLineCounters
(
    tmUnitSelect_t unit,
    UInt16         pixelCountPreset,
    UInt16         pixelCountNumber,
    UInt16         lineCountPreset,
    UInt16         lineCountNumber
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           pTabRegValue[8];


    if (RxHdmiConfig[unit].chipConfiguration.revision == 0)
    {
        /* WA add 3 pixels to pixelCountPreset */
        pixelCountPreset +=3;
    }

    /* WA add 1 line to lineCountPreset */
    /* not executed because VRef_start already dicreased by one in the resolutionTimingsList table in tmdlHdmiRx_cfg.c */
    //lineCountPreset +=1;

    /* Pixel counter preset value */
    RETIF(pixelCountPreset > MAX_COUNTER,TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)
    pTabRegValue[0] = (UInt8)(( pixelCountPreset & MASK_COUNT_MSB )>> 8);
    pTabRegValue[1] = (UInt8)( pixelCountPreset & MASK_COUNT_LSB );

    /* Pixel counter number value */
    RETIF(pixelCountNumber > MAX_COUNTER,TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)
    pTabRegValue[2] = (UInt8)(( pixelCountNumber & MASK_COUNT_MSB )>> 8);
    pTabRegValue[3] = (UInt8)( pixelCountNumber & MASK_COUNT_LSB );

    /* Line counter preset value */
    RETIF(lineCountPreset > MAX_COUNTER,TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)
    pTabRegValue[4] = (UInt8)(( lineCountPreset & MASK_COUNT_MSB )>> 8);
    pTabRegValue[5] = (UInt8)( lineCountPreset & MASK_COUNT_LSB );

    /* Line counter number value */
    RETIF(lineCountNumber > MAX_COUNTER,TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)
    pTabRegValue[6] = (UInt8)(( lineCountNumber & MASK_COUNT_MSB )>> 8);
    pTabRegValue[7] = (UInt8)( lineCountNumber & MASK_COUNT_LSB );

    /* Write the Pixel and Line Counter registers */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)PXCNT_PR_MSB, 8, pTabRegValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    return TM_OK;
}

/*******************************************************************************/
tmErrorCode_t
bslTDA1997XSetVHRefValues
(
    tmUnitSelect_t unit,
    tmbslHdmiRxVHRefValues_t   *pVHRefValues
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           pTabRegValue[6];

    /* Test pVHRefValues <> NULL */
    RETIF(pVHRefValues == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* Horizontal reference start MSB values */
    RETIF(pVHRefValues->href_start > MAX_REF_START_END,TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)
    pTabRegValue[0] = (UInt8)( (pVHRefValues->href_start & MASK_REF_START_END_MSB ) >> 8 );

    /* Horizontal reference start LSB value */
    pTabRegValue[1] = (UInt8)( pVHRefValues->href_start & MASK_REF_START_END_LSB );

    /* Horizontal reference end MSB values */
    RETIF(pVHRefValues->href_end > MAX_REF_START_END,TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)
    pTabRegValue[2] = (UInt8)( (pVHRefValues->href_end & MASK_REF_START_END_MSB ) >> 8 );

    /* Horizontal reference end LSB value */
    pTabRegValue[3] = (UInt8)( pVHRefValues->href_end & MASK_REF_START_END_LSB );

    /* Write HREF_S_MSB, HREF_S_LSB, HREF_E_MSB, HREF_E_LSB registers */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)HREF_S_MSB, 4, pTabRegValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)


    /* Vertical reference f1 start MSB value */
    RETIF(pVHRefValues->vref_f1_start > MAX_REF_FI_START,TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)
    pTabRegValue[0] = (UInt8)( pVHRefValues->vref_f1_start >> 8 );

    /* Vertical reference f1 start LSB value */
    pTabRegValue[1] = (UInt8)( pVHRefValues->vref_f1_start & MASK_REF_FI_START_LSB );

    /* Vertical reference f1 width value */
    pTabRegValue[2] = pVHRefValues->vef_f1_width;

    /* Vertical reference f2 start MSB value */
    RETIF(pVHRefValues->vref_f2_start > MAX_REF_FI_START,TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)
    pTabRegValue[3] = (UInt8)(pVHRefValues->vref_f2_start >> 8 );

    /* Vertical reference f2 start LSB value */
    pTabRegValue[4] = (UInt8)(pVHRefValues->vref_f2_start & MASK_REF_FI_START_LSB );

    /* Vertical reference f2 width value */
    pTabRegValue[5] = pVHRefValues->vef_f2_width;

    /* Write VREF_F1_S_MSB, VREF_F1_S_LSB, VREF_F1_WIDTH,
       VREF_F2_S_MSB, VREF_F2_S_LSB and VREF_F2_WIDTH registers */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)VREF_F1_S_MSB, 6, pTabRegValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)


    /* Field reference f1 start MSB value */
    RETIF(pVHRefValues->fieldref_f1_start > MAX_REF_FI_START,TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)
    pTabRegValue[0] = (UInt8)(( pVHRefValues->fieldref_f1_start & MASK_REF_FI_START_MSB ) >> 8);

    /* Fill field polarity bit */
    pTabRegValue[0] |=  pVHRefValues->fieldPolarity;

    /* Field reference f1 start LSB value */
    pTabRegValue[1] = (UInt8)( pVHRefValues->fieldref_f1_start & MASK_REF_FI_START_LSB );

    /* Field reference f2 start MSB value */
    RETIF(pVHRefValues->fieldref_f2_start > MAX_REF_FI_START,TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)
    pTabRegValue[2] = (UInt8)(( pVHRefValues->fieldref_f2_start & MASK_REF_FI_START_MSB ) >> 8);

    /* Field reference f2 start LSB value */
    pTabRegValue[3] = (UInt8)( pVHRefValues->fieldref_f2_start & MASK_REF_FI_START_LSB );

    /* Write FREF_F1_S_MSB, FREF_F1_S_LSB, Field Polarity, FREF_F2_S_MSB and FREF_F2_S_LSB registers */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)FREF_F1_S_MSB, 4, pTabRegValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    return TM_OK;
}

/*******************************************************************************/
tmErrorCode_t
bslTDA1997XSetVHSyncValues
(
    tmUnitSelect_t unit,
    tmbslHdmiRxVHSyncValues_t *pVHSyncValues
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           pTabRegValue[14];

    /* Test pVHSyncValues <> NULL */
    RETIF(pVHSyncValues == Null,TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS)

    /* Horizontal sync start MSB value */
    RETIF(pVHSyncValues->hsync_start > MAX_REF_START_END,TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)
    pTabRegValue[0] = (UInt8)( (pVHSyncValues->hsync_start & MASK_REF_START_END_MSB ) >> 8 );

    /* Horizontal sync start LSB value */
    RETIF(pVHSyncValues->hsync_start > MAX_REF_START_END,TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)
    pTabRegValue[1] = (UInt8)( pVHSyncValues->hsync_start & MASK_REF_START_END_LSB );

    /* Horizontal sync end MSB value */
    RETIF(pVHSyncValues->hsync_end > MAX_REF_START_END,TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)
    pTabRegValue[2] = (UInt8)( (pVHSyncValues->hsync_end & MASK_REF_START_END_MSB ) >> 8 );

    /* Horizontal sync end LSB value */
    RETIF(pVHSyncValues->hsync_end > MAX_REF_START_END,TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)
    pTabRegValue[3] = (UInt8)( pVHSyncValues->hsync_end & MASK_REF_START_END_LSB );

    /* Write HS_S_MSB, HS_LSB, HS_E_MSB and HS_E_LSB registers */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)HS_S_MSB, 4, pTabRegValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)


    /* Vertical sync line f1 start MSB value */
    RETIF(pVHSyncValues->vsync_f1_start > MAX_REF_FI_START,TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)
    pTabRegValue[0] = (UInt8)( pVHSyncValues->vsync_f1_start >> 8 );

    /* Vertical sync line f1 start LSB value */
    pTabRegValue[1] = (UInt8)( pVHSyncValues->vsync_f1_start & MASK_REF_FI_START_LSB );

    /* Vertical sync line f1 width value */
    pTabRegValue[2] = pVHSyncValues->vsync_f1_width;

    /* Vertical sync line f2 start MSB value */
    RETIF(pVHSyncValues->vsync_f2_start > MAX_REF_FI_START,TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)
    pTabRegValue[3] = (UInt8)( pVHSyncValues->vsync_f2_start >> 8 );

    /* Vertical sync line f2 start LSB value */
    pTabRegValue[4] = (UInt8)( pVHSyncValues->vsync_f2_start & MASK_REF_FI_START_LSB );

    /* Vertical sync line f2 width value */
    pTabRegValue[5] = pVHSyncValues->vsync_f2_width;

    /* Vertical sync pixel f1 start MSB value */
    RETIF(pVHSyncValues->vsync_f1_pix_start > MAX_REF_START_END,TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)
    pTabRegValue[6] = (UInt8)( pVHSyncValues->vsync_f1_pix_start >> 8 );

    /* Vertical sync pixel f1 start LSB value */
    pTabRegValue[7] = (UInt8)( pVHSyncValues->vsync_f1_pix_start & MASK_REF_START_END_LSB );

    /* Vertical sync pixel f1 end MSB value */
    RETIF(pVHSyncValues->vsync_f1_pix_end > MAX_REF_START_END,TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)
    pTabRegValue[8] = (UInt8)( pVHSyncValues->vsync_f1_pix_end >> 8 );

    /* Vertical sync pixel f1 end LSB value */
    pTabRegValue[9] = (UInt8)( pVHSyncValues->vsync_f1_pix_end & MASK_REF_START_END_LSB );

    /* Vertical sync pixel f2 start MSB value */
    RETIF(pVHSyncValues->vsync_f2_pix_start > MAX_REF_START_END,TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)
    pTabRegValue[10] = (UInt8)( pVHSyncValues->vsync_f2_pix_start >> 8 );

    /* Vertical sync pixel f2 start LSB value */
    pTabRegValue[11] = (UInt8)( pVHSyncValues->vsync_f2_pix_start & MASK_REF_START_END_LSB );

    /* Vertical sync pixel f2 end MSB value */
    RETIF(pVHSyncValues->vsync_f2_pix_end > MAX_REF_START_END,TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER)
    pTabRegValue[12] = (UInt8)( pVHSyncValues->vsync_f2_pix_end >> 8 );

    /* Vertical sync pixel f2 end LSB value */
    pTabRegValue[13] = (UInt8)( pVHSyncValues->vsync_f2_pix_end & MASK_REF_START_END_LSB );

    /* Write the VSync registers */
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)VS_F1_LINE_S_MSB, 14, pTabRegValue);
    RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_WRITE)

    return TM_OK;
}



/*============================================================================*/
/*                            END OF FILE                                     */
/*============================================================================*/


static void HZ5(void)
{
    // debug routine
    /* WA enable power switch  - SRAM content is always valid (in case E-MTP is not or bad programmed)*/
    int errCode;
    tmUnitSelect_t          unit=0;
    UInt8                   regValue;

    errCode =  tmbslTDA1997XReadI2C(unit, (UInt16)CONTROL, 1, &regValue);
    regValue |= 0x20;
    errCode =  tmbslTDA1997XWriteI2C(unit, (UInt16)CONTROL, 1, &regValue);
    /* wait during 50 ms */
}

#endif // SVR_HAVE_NXP