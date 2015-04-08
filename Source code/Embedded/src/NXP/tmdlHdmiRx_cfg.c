/**
 * Copyright (C) 2006 NXP N.V., All Rights Reserved.
 * This source code and any compilation or derivative thereof is the proprietary
 * information of NXP N.V. and is confidential in nature. Under no circumstances
 * is this software to be  exposed to or placed under an Open Source License of
 * any type without the expressed written permission of NXP N.V.
 *
 * \file          tmdlHdmiRx_cfg.c
 *
 * \version       Revision: 1
 *
 * \date          Date: 13/12/07 10:32
 *
 * \brief         devlib driver component API for the TDA1997X HDMI Receiver
 *
 * \section refs  Reference Documents
 * HDMI Rx Driver - FRS.doc,
 *
 * \section info  Change Information
 *
 * \verbatim

   History:       tmdlHdmiRx_cfg.c
 *

   \endverbatim
 *
*/

/*============================================================================*/
/*                             INCLUDE FILES                                  */
/*============================================================================*/
#include "GlobalOptions.h"
#include <asf.h>
#include "AVRHDMI.h"
#include "tmdlHdmiRx_IW.h"
#include "tmNxTypes.h"
#include "tmdlHdmiRx.h"
#include "tmdlHdmiRx_cfg.h"
#include "tmbslTDA1997X_Functions.h"
#include "Console.h"


#include <asf.h>
#include "conf_usb.h"
#include "ui.h"
#include "uart.h"

#ifdef HDMI_DEBUG
    //#define x
    //#define x	Write("dlcfg-"); PRINTIF(999,__LINE__);
#else
    //#define x	;
#endif

/*****************************************************************************/
/*****************************************************************************/
/*                 THIS PART CAN BE MODIFIED BY CUSTOMER                     */
/*****************************************************************************/
/*****************************************************************************/

/* The following includes are used by I2C access function for ARM7. If    */
/* you need to rewrite these functions for your own SW infrastructure, then   */
/* it can be removed                                                          */
#include "I2C.h"


#ifdef TMFL_TDA19972_FAMILY
    /* I2C adress of the unit                                                     */
    /* Put there the I2C slave address of the Rx transmitter IC                    */
    #define UNIT_I2C_ADDRESS_0 0x48
    #define UNIT_I2C_ADDRESS_1 0x49

    /* I2C adress of the unit                                                     */
    /* Put there the I2C slave address of the Rx transmitter IC                    */
    #define CEC_I2C_ADDRESS_0 0x34
    #define CEC_I2C_ADDRESS_1 0x35

#endif


/* Priority of the command task                                               */
/* Command task is an internal task that handles incoming event from the IC   */
/* put there a value that will ensure a response time of ~20ms in your system */
#define COMMAND_TASK_PRIORITY_0  10

/* Stack size of the command tasks */
/* This value depends of the type of CPU used, and also from the length of    */
/* the customer callbacks. Increase this value if you are making a lot of     */
/* processing (function calls & local variables) and that you experience      */
/* stack overflows                                                            */
#define COMMAND_TASK_STACKSIZE_0 128

/* Size of the message queues for command tasks                               */
/* This value defines the size of the message queue used to link the          */
/* the tmdlHdmiTxHandleInterrupt function and the command task. The default   */
/* value below should fit any configuration                                   */
#define COMMAND_TASK_QUEUESIZE_0 128

/* HDCP key seed                                                              */
/* HDCP key are stored encrypted into the IC, this value allows the IC to     */
/* decrypt them. This value is provided to the customer by NXP customer       */
/* support team. Value below is a dummy value and must be replaced by a       */
/* correct one.                                                               */
#define KEY_DESCRIPTION_SEED 0

/* HDCP seed table, arranged as pairs of 16-bit integers: lookup value, seed value.
 * If no table is programmed or if KEY_SEED in config file is null, HDCP will be disabled */

const tmdlHdmiRxCfgSeed_t kRxSeedTable[RX_SEED_TABLE_LEN] = {
    {0xF0, 0x1234},
    {0xF1, 0xDBE6},
    {0xF2, 0xDBE6},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0}
};


/* VSync delay                                                                */
/* This value defines the delay (in pixel clock periods) to apply to VS       */
/* signal.                                                                    */
/* 0  = [datapath latency] - 9 clkpix periods                                 */
/* 15 = [datapath latency] + 6 clkpix periods                                 */
#define VS_SYNC_DELAY           0x08U

/* HSync delay                                                                */
/* This value defines the delay (in pixel clock periods) to apply to HS       */
/* signal.                                                                    */
/* 0  = [datapath latency] - 9 clkpix periods                                 */
/* 15 = [datapath latency] + 6 clkpix periods                                 */
#define HS_SYNC_DELAY           0x08U



/* Clock pulse delay                                                          */
/* (De)activates delay added to the output clock pulse                        */
#define CLOCK_PULSE_DELAY       True

/* clock pulse delay value                                                    */
#ifdef TMFL_TDA19972_FAMILY
    #define CLOCK_PULSE_DELAY_VALUE 0x03U
#else
    #define CLOCK_PULSE_DELAY_VALUE 0x04U
#endif







/* Array gathering general information for each supported video mode          */
/* This array provides width in pixel, height in pixel, interlaced boolean    */
/* and vertical frequency.                                                    */
//#ifdef SUPPORT_3D_FP
//#define RES_SUPPORTED_NB_0 60
//#else
//#define RES_SUPPORTED_NB_0 48
//#endif
//const tmdlHdmiRxCfgResolution_t resolutionInfo[RES_SUPPORTED_NB_0]={

#define RES_SUPPORTED_NB_0 TMDL_HDMIRX_VIDEORES_UNKNOWN

const tmdlHdmiRxCfgResolution_t resolutionInfo[TMDL_HDMIRX_VIDEORES_UNKNOWN]= {
    /* Low TV */

    /* 60 Hz TV */
    {TMDL_HDMIRX_VIDEORES_720_480p_60HZ,   {720, 480, False, 60}},
    {TMDL_HDMIRX_VIDEORES_1280_720p_60HZ,  {1280, 720, False, 60}},
    {TMDL_HDMIRX_VIDEORES_1920_1080i_60HZ, {1920, 1080, True, 60}},
    {TMDL_HDMIRX_VIDEORES_720_480i_60HZ,   {720, 480, True, 60}},
    {TMDL_HDMIRX_VIDEORES_1920_1080p_60HZ, {1920, 1080, False, 60}},

    /* 50 Hz TV */

#ifdef SUPPORT_3D_FP
    /* 3D Frame Packing */
    {TMDL_HDMIRX_VIDEORES_720_480p_60HZ_FP,   {720, 1005, False, 60}},
    {TMDL_HDMIRX_VIDEORES_1280_720p_60HZ_FP,  {1280, 1470, False, 60}},
#endif

    /* 60 Hz PC */
    {TMDL_HDMIRX_VIDEORES_640_480p_60HZ,   { 640, 480, False, 60}}, /* quantum 882 DMT0660 */
    {TMDL_HDMIRX_VIDEORES_800_600p_60HZ,   { 800, 600, False, 60}}, /* quantum 882 DMT0860 */
    {TMDL_HDMIRX_VIDEORES_1024_768p_60HZ,  {1024, 768, False, 60}}, /* quantum 882 DMT01060 */
    {TMDL_HDMIRX_VIDEORES_1280_768p_60HZ,  {1280, 768, False, 60}}, /* quantum 882 CVR1260E */
    {TMDL_HDMIRX_VIDEORES_1280_960p_60HZ,  {1280, 960, False, 60}}, /* quantum 882 DMT1260A */
    {TMDL_HDMIRX_VIDEORES_1280_1024p_60HZ, {1280, 1024, False, 60}}, /* quantum 882 DMT1260G */
    {TMDL_HDMIRX_VIDEORES_1440_900p_60HZ,  {1440, 900, False, 60}}, /*  quantum 882 CVT1460D */
    {TMDL_HDMIRX_VIDEORES_1600_1200p_60HZ, {1600,1200, False, 60}}, /* quantum 882 DMT1660 */
    {TMDL_HDMIRX_VIDEORES_1680_1050p_60HZ_RB, {1680,1050, False, 60}}, /* quantum 882 CVR1660D */
    {TMDL_HDMIRX_VIDEORES_1920_1200p_60HZ_RB, {1920,1200, False, 60}}, /* quantum 882 CVR1960D */

    /* 75 HZ PC */
    /* Other resolutions */
    {TMDL_HDMIRX_VIDEORES_720_240p_60HZ_M1,   { 720, 240, False, 60}},
    {TMDL_HDMIRX_VIDEORES_720_240p_60HZ_M2,   { 720, 240, False, 60}},


    {TMDL_HDMIRX_VIDEORES_1360_768p_60HZ,     {1360, 768, False, 60}},
    {TMDL_HDMIRX_VIDEORES_1400_1050p_60HZ,    {1400,1050, False, 60}},
    {TMDL_HDMIRX_VIDEORES_1400_1050p_60HZ_RB, {1400,1050, False, 60}},


    {TMDL_HDMIRX_VIDEORES_1080_1920p_60HZ,	 { 1080, 1920, False, 60}}, // added by Sensics

};

/* Array providing caracteristics of video mode for resolution detection by   */
/* the driver.                                                                */
//#ifdef SUPPORT_3D_FP
//#define RES_TO_BE_DETECTED_NB_0 60
//#else
//#define RES_TO_BE_DETECTED_NB_0 48
//#endif
//const tmdlHdmiRxCfgResolutionDetect_t resolutionToBeDetectedList[RES_TO_BE_DETECTED_NB_0]={

#define RES_TO_BE_DETECTED_NB_0 TMDL_HDMIRX_VIDEORES_UNKNOWN

const tmdlHdmiRxCfgResolutionDetect_t resolutionToBeDetectedList[TMDL_HDMIRX_VIDEORES_UNKNOWN]= {
    /* Low TV */
    /* 60 Hz TV */
    {TMDL_HDMIRX_VIDEORES_720_480p_60HZ,    446870, 453626,  850,  865,  60, 63},
    {TMDL_HDMIRX_VIDEORES_1280_720p_60HZ,   446870, 453626,  594,  605,  13, 15},
    {TMDL_HDMIRX_VIDEORES_1920_1080i_60HZ,  446870, 453626,  793,  807,  14, 17},
    {TMDL_HDMIRX_VIDEORES_720_480i_60HZ,    446870, 453626, 1701, 1729, 122, 125},
    {TMDL_HDMIRX_VIDEORES_1920_1080p_60HZ,  446870, 453626,  396,  404,   6,  9},

    /* 50 Hz TV */

#ifdef SUPPORT_3D_FP
    /* 3D Frame Packing */
    {TMDL_HDMIRX_VIDEORES_720_480p_60HZ_FP,    446870,  453626,  424,  432,  29,  31},
    {TMDL_HDMIRX_VIDEORES_1280_720p_60HZ_FP,   446870,  453626,  297,  303,   6,   8},
#endif

    /* 60 HZ PC */
    {TMDL_HDMIRX_VIDEORES_640_480p_60HZ,    446870, 453626, 850, 865, 101, 104},
    {TMDL_HDMIRX_VIDEORES_800_600p_60HZ,    444523, 450791, 708, 718,  84,  88},
    {TMDL_HDMIRX_VIDEORES_1024_768p_60HZ,   446842, 453142, 554, 562,  54,  58},
    {TMDL_HDMIRX_VIDEORES_1280_768p_60HZ,   447842, 454156, 561, 569,  41,  46},
    {TMDL_HDMIRX_VIDEORES_1280_960p_60HZ,   446872, 453172, 447, 453,  26,  30},
    {TMDL_HDMIRX_VIDEORES_1280_1024p_60HZ,  446723, 453021, 419, 425,  26,  30},
    {TMDL_HDMIRX_VIDEORES_1440_900p_60HZ,   446723, 453021, 478, 486,  35,  40},
    {TMDL_HDMIRX_VIDEORES_1600_1200p_60HZ,  446872, 453172, 357, 363,  30,  34},
    {TMDL_HDMIRX_VIDEORES_1680_1050p_60HZ_RB,  447745, 454058, 415, 420,   5,  9},
    {TMDL_HDMIRX_VIDEORES_1920_1200p_60HZ_RB,  447235, 453550, 362, 367,   4,  8},

    /* 75 HZ PC */
    /* Other resolutions */
    {TMDL_HDMIRX_VIDEORES_720_240p_60HZ_M1,   446017, 452305, 1702, 1726, 122, 126},
    {TMDL_HDMIRX_VIDEORES_720_240p_60HZ_M2,   447723, 454035, 1702, 1726, 122, 126},

    {TMDL_HDMIRX_VIDEORES_1360_768p_60HZ,     446760, 453059, 562, 570, 33, 37},
    {TMDL_HDMIRX_VIDEORES_1400_1050p_60HZ,    447036, 453338, 411, 416, 30, 34},
    {TMDL_HDMIRX_VIDEORES_1400_1050p_60HZ_RB, 447260, 453565, 414, 420,  7,  11},

    {TMDL_HDMIRX_VIDEORES_1080_1920p_60HZ,    440046, 454046, 209, 249, 1, 4} // V 6e5ce e5 HS 2 - was 450046
};

/* Array providing timings values of video mode for resolution sync timing    */
/* configuration.                                                             */
//const tmdlHdmiRxCfgResolutionTimings_t resolutionTimingsList[RES_SUPPORTED_NB_0]={
const tmdlHdmiRxCfgResolutionTimings_t resolutionTimingsList[TMDL_HDMIRX_VIDEORES_UNKNOWN]= {
    /* Low TV */
    /* 60 Hz TV */
    {   TMDL_HDMIRX_VIDEORES_720_480p_60HZ, 1, 858, 1, 525,
        {123, 843, 516, 45, 0, 0, 1, BSLHDMIRX_FREF_FIELD1_LOW_FIELD2_HIGH, 0},//OK scope
    },
    {   TMDL_HDMIRX_VIDEORES_1280_720p_60HZ, 1, 1650, 1, 750,
        {261, 1541, 745, 30, 0, 0, 1, BSLHDMIRX_FREF_FIELD1_LOW_FIELD2_HIGH, 0},//OK scope
    },
    {   TMDL_HDMIRX_VIDEORES_1920_1080i_60HZ, 1, 2200, 1, 1125,
        {193, 2113, 1123, 22, 560, 23, 1, BSLHDMIRX_FREF_FIELD1_LOW_FIELD2_HIGH, 563},//OK scope
    },
    {   TMDL_HDMIRX_VIDEORES_720_480i_60HZ, 1, 858, 1, 525,
        {120, 840, 521, 22, 258, 23, 1, BSLHDMIRX_FREF_FIELD1_LOW_FIELD2_HIGH, 263},//OK scope
    },
    {   TMDL_HDMIRX_VIDEORES_1920_1080p_60HZ, 1, 2200, 1, 1125,
        {193, 2113, 1121, 45, 0, 0, 1, BSLHDMIRX_FREF_FIELD1_LOW_FIELD2_HIGH, 0},//OK scope
    },

    /* 50 Hz TV */
#ifdef SUPPORT_3D_FP
    /* 3D Frame Packing */
    {   TMDL_HDMIRX_VIDEORES_720_480p_60HZ_FP, 1, 858, 1, 1050,
        {123, 843, 1032, 45, 0, 0, 1, BSLHDMIRX_FREF_FIELD1_LOW_FIELD2_HIGH, 0},
    },
    {   TMDL_HDMIRX_VIDEORES_1280_720p_60HZ_FP,  1, 1650, 1, 1500,
        {261, 1541, 1490, 30, 0, 0, 1, BSLHDMIRX_FREF_FIELD1_LOW_FIELD2_HIGH, 0},
    },
    // Following video formats have lineCountNb and VREF_F1_Start values greater than 11-bit registers
    // Don't use Blanking codes and Timing code with these video formats
#endif

    /* 60 Hz PC */
    {   TMDL_HDMIRX_VIDEORES_640_480p_60HZ, 1, 800, 1, 525,
        {145, 785, 515, 45, 0, 0, 1, BSLHDMIRX_FREF_FIELD1_LOW_FIELD2_HIGH, 0},//VGA DMT0660//OK scope + monitor
    },
    {   TMDL_HDMIRX_VIDEORES_800_600p_60HZ, 1, 1056, 1, 628,
        {217, 1017, 627, 28, 0, 0, 0, BSLHDMIRX_FREF_FIELD1_LOW_FIELD2_HIGH, 0},//SVGA DMT0860 //OK scope + monitor
    },
    {   TMDL_HDMIRX_VIDEORES_1024_768p_60HZ, 1, 1344, 1, 806,
        {297,   1321,    803,   38, 0, 0, 0, BSLHDMIRX_FREF_FIELD1_LOW_FIELD2_HIGH, 0},//XGA DMT1060 //OK scope + monitor
    },
    {   TMDL_HDMIRX_VIDEORES_1280_768p_60HZ, 1, 1440, 1, 790,
        {321,   1601,    795,   30, 0, 0, 0, BSLHDMIRX_FREF_FIELD1_LOW_FIELD2_HIGH, 0},//CVT1260E ? but no DE (probably bad from Quantum)
    },
    {   TMDL_HDMIRX_VIDEORES_1280_960p_60HZ, 1, 1800, 1, 1000,
        {425,   1705,    999,   40, 0, 0, 0, BSLHDMIRX_FREF_FIELD1_LOW_FIELD2_HIGH, 0},//DMT1260A //OK scope + monitor
    },
    {   TMDL_HDMIRX_VIDEORES_1280_1024p_60HZ, 1, 1688, 1, 1066,
        {361,   1641,   1065,   42, 0, 0, 0, BSLHDMIRX_FREF_FIELD1_LOW_FIELD2_HIGH, 0},//SXGA DMT1260G //OK scope + monitor
    },
    {   TMDL_HDMIRX_VIDEORES_1440_900p_60HZ, 1, 1904, 1, 934,
        {385,   1825,    931,   34, 0, 0, 0, BSLHDMIRX_FREF_FIELD1_LOW_FIELD2_HIGH, 0},//CVT1460D //OK scope + monitor
    },
    {   TMDL_HDMIRX_VIDEORES_1600_1200p_60HZ, 1, 2160, 1, 1250,
        {497, 2097, 1249, 50, 0, 0, 0, BSLHDMIRX_FREF_FIELD1_LOW_FIELD2_HIGH, 0},//UXGA DMT1660 //OK scope + monitor
    },
    {   TMDL_HDMIRX_VIDEORES_1680_1050p_60HZ_RB, 1, 1840, 1, 1080,
        {113, 1793, 1077, 30, 0, 0, 0, BSLHDMIRX_FREF_FIELD1_LOW_FIELD2_HIGH, 0},//CVR1660D //OK scope (not supported by monitor)
    },
    {   TMDL_HDMIRX_VIDEORES_1920_1200p_60HZ_RB, 1, 2080, 1, 1235,
        {113, 2033, 1232, 35, 0, 0, 0, BSLHDMIRX_FREF_FIELD1_LOW_FIELD2_HIGH, 0},//CVR1960D //OK scope (not supported by monitor)
    },

    /* 75 HZ PC */
    /* Other resolutions */

    {   TMDL_HDMIRX_VIDEORES_720_240p_60HZ_M1, 1, 858, 1, 262,
        {120, 840, 258, 22, 0, 0, 0, BSLHDMIRX_FREF_FIELD1_LOW_FIELD2_HIGH, 0},// NT unknown...
    },
    {   TMDL_HDMIRX_VIDEORES_720_240p_60HZ_M2, 1, 858, 1, 263,
        {120, 840, 258, 23, 0, 0, 0, BSLHDMIRX_FREF_FIELD1_LOW_FIELD2_HIGH, 0},// NT unknown...
    },

    {   TMDL_HDMIRX_VIDEORES_1360_768p_60HZ, 1, 1792, 1, 795,
        {369, 1729, 792, 27, 0, 0, 0, BSLHDMIRX_FREF_FIELD1_LOW_FIELD2_HIGH, 0},//DMT1360H //OK scope + monitor
    },
    {   TMDL_HDMIRX_VIDEORES_1400_1050p_60HZ, 1, 1864, 1, 1089,
        {377, 1777, 1086, 39, 0, 0, 0, BSLHDMIRX_FREF_FIELD1_LOW_FIELD2_HIGH, 0},//CVT1460 //OK scope + monitor
    },
    {   TMDL_HDMIRX_VIDEORES_1400_1050p_60HZ_RB, 1, 1560, 1, 1080,
        {113, 1513, 1077, 30, 0, 0, 0, BSLHDMIRX_FREF_FIELD1_LOW_FIELD2_HIGH, 0},//CVR1460 //? but no DE (probably bad from Quantum)
    },
    {   TMDL_HDMIRX_VIDEORES_1080_1920p_60HZ, 1, 1168, 1, 1961,
        {193, 2113, 1121, 45, 0, 0, 1, BSLHDMIRX_FREF_FIELD1_LOW_FIELD2_HIGH, 0},//OK scope !! todo: change to actual values
    }

};








/* This is the table used by tmdlHdmiRxSetColorConversionMatrix function for  */
/* the predefined color conversion matrix. Change it to fit your specific     */
/* needs.                                                                     */
#define PREDEFINED_COLOR_MATRIX_NB_0 2
const tmdlHdmiRxColorMatrixCoefs_t predefinedColorMatrixList[PREDEFINED_COLOR_MATRIX_NB_0]=
{
    {
        -256,   -256,   -256,                /*Input Offset*/
        2404,   1225,    467,
        -1754,   2095,   -341,
        -1388,   -707,   2095,                /*Matrix: RGB limited range => ITU-601 YCbCr limited range */
        256,   2048,   2048                 /*Output Offset*/
    },
    {
        -256,   -2048,  -2048,                /*Input Offset*/
        4096,   -2860,  -1378,
        4096,    5615,      0,
        4096,       0,   7097,                /*Matrix:ITU-601 YCbCr limited range =>  RGB limited range */
        256,     256,    256                 /*Output Offset*/
    }
};

/* Structure listing IC capabilities as they will be reported by              */
/* tmdlHdmiRxGetCapabilitiesM.                                                */
tmdlHdmiRxCapabilities_t capabilitiesList0 = {
    TMDL_HDMIRX_DEVICE_UNKNOWN,    /* Version of the IC device */
    True,                          /* Repeater mode supported */
    False,                         /* Fast I2C supported */
    TMDL_HDMIRX_HDMI_VERSION_1_3a, /* HDMI protocol version supported */
    {   False,                        /* HBR supported */
        False,                        /* DST supported */
        False
    },                       /* OBA supported */
    True,                          /* fast re-auth supported */
    False,                         /* analog input supported */
    False,                         /* HDMI 1.1 features supported */
    True
};                         /* Internal EDID feature supported */



/* The following function must be rewritten by the customer to fit its own    */
/* SW infrastructure. This function allows reading through I2C bus.           */
/* tmbslHdmiRxSysArgs_t definition is located into tmbslHdmiRx_type.h file.   */
tmErrorCode_t RtxRxI2cReadFunction(tmbslHdmiRxSysArgs_t *pSysArgs)
{
    tmErrorCode_t errCode;

    //WriteLn("read");
    errCode = i2cRead(reg_TDA997X, (tmbslHdmiSysArgs_t *) pSysArgs);

    return errCode;
}

/* The following function must be rewritten by the customer to fit its own    */
/* SW infrastructure. This function allows writing through I2C bus.           */
/* tmbslHdmiRxSysArgs_t definition is located into tmbslHdmiRx_type.h file.   */
tmErrorCode_t RtxRxI2cWriteFunction(tmbslHdmiRxSysArgs_t *pSysArgs)
{
    tmErrorCode_t errCode;


    errCode = i2cWrite(reg_TDA997X,(tmbslHdmiSysArgs_t *) pSysArgs);


    return errCode;
}




/* BSL function list                                                          */
/* This table is the list of BSL functions that devlib will use. This table   */
/* should not be modified unless you would like to use another BSL and        */
/* another IC than TDA9978                                                    */
const tmbslHdmiRxFunctionsList_t bslHdmiRxFunctionsList= {
    tmbslTDA1997XInit,
    tmbslTDA1997XDeInit,
    tmbslTDA1997XGetChipInfo,
    tmbslTDA1997XHandleInterrupt,
    tmbslTDA1997XGetInterruptStatus,
    tmbslTDA1997XHeartBeat,
    tmbslTDA1997XSetInterruptMask,
    tmbslTDA1997XConfigureInput,
    tmbslTDA1997XSetClockPulseDelay,
    tmbslTDA1997XConfigurePixelClockGenerator,
    tmbslTDA1997XGetSyncTimings,
    tmbslTDA1997XConfigureColorSpaceConversion,
    tmbslTDA1997XSetPixelAndLineCounters,
    tmbslTDA1997XConfigureVHRef,
    tmbslTDA1997XSetVHRefValues,
    tmbslTDA1997XSetVHSyncValues,
    tmbslTDA1997XConfigureFrameDetectionWindow,
    tmbslTDA1997XGetFrameMeasurements,
    tmbslTDA1997XSetVHSyncDelay,
    tmbslTDA1997XSetBlankingCodes,
    tmbslTDA1997XConfigurePreFilter,
    tmbslTDA1997XConfigureRangeControl,
    tmbslTDA1997XConfigureVideoOutput,
    tmbslTDA1997XConfigureUpDownSampler,
    tmbslTDA1997XConfigureInternalVHSync,
    tmbslTDA1997XConfigureSyncOutput,
    tmbslTDA1997XConfigureOutputPolarity,
    tmbslTDA1997XConfigureVideoPort,
    tmbslTDA1997XConfigureDataEnable,
    tmbslTDA1997XConfigureHDMI,
    tmbslTDA1997XConfigureHDMIClock,
    tmbslTDA1997XConfigureAudioFormatter,
    tmbslTDA1997XConfigurePixelRepeater,
    tmbslTDA1997XConfigureAudioSelection,
    tmbslTDA1997XConfigurePacketTypeScan,
    tmbslTDA1997XAudioAutoMute,
    tmbslTDA1997XGetHDMIStatus,
    tmbslTDA1997XGetAudioStatus,
    tmbslTDA1997XGetAudioClockRegPacket,
    tmbslTDA1997XGetDataIslandErrorCorrection,
    tmbslTDA1997XGetAudioReadWriteDelta,
    tmbslTDA1997XGetAudioContentProtectionUpdate,
    tmbslTDA1997XConfigureOTP,
    tmbslTDA1997XSetOTPAddress,
    tmbslTDA1997XReadOTP,
    tmbslTDA1997XConfigureHDCP,
    tmbslTDA1997XConfigureReceiverCaps,
    tmbslTDA1997XSetHDCPRepeaterStatus,
    tmbslTDA1997XSetHDCPRepeaterControl,
    tmbslTDA1997XSetKeySelectionVector,
    tmbslTDA1997XSetPrivateKeyIndex,
    tmbslTDA1997XSetPrivateKey,
    tmbslTDA1997XSetKSVIndex,
    tmbslTDA1997XSetKSVValue,
    tmbslTDA1997XConfigureHDCPErrorProtection,
    tmbslTDA1997XGetDisplayData,
    tmbslTDA1997XGetInfoframe,
    tmbslTDA1997XGetACPPacket,
    tmbslTDA1997XReadISRC1Packet,
    tmbslTDA1997XReadISRC2Packet,
    tmbslTDA1997XReadGBDPacket,
    tmbslTDA1997XHDMISoftReset,
    tmbslTDA1997XGetCapabilities,
    tmbslTDA1997XConfigureAudioClkMode,
    tmbslTDA1997XReadGCP,
    tmbslTDA1997XWriteI2C,
    tmbslTDA1997XReadI2C,
    tmbslTDA1997XGetTMDSFrequency,
    tmbslTDA1997XGetAudioFrequency,
    tmbslTDA1997XConfigureEDID,
    tmbslTDA1997XLoadEDIDData,
    tmbslTDA1997XGetDeepColorMode,
    tmbslTDA1997XSetPowerState,
    tmbslTDA1997XActivateTestPattern,
    tmbslTDA1997XGetChannelStatus
#ifdef TMFL_HDMI_OUT
    ,tmbslTDA1997XSetHdmiOutMode
#endif
};

/*****************************************************************************/
/*****************************************************************************/
/*                THIS PART MUST NOT BE MODIFIED BY CUSTOMER                 */
/*****************************************************************************/
/*****************************************************************************/

/* DO NOT MODIFY, this table is used for transmission of the configuration to */
/* the core driver         */


/* DO NOT MODIFY, this table is used for transmission of the configuration to */
/* the core driver                                                            */
tmdlHdmiRxDriverConfigTable_t driverConfigTable[MAX_UNITS] = {
    {
        COMMAND_TASK_PRIORITY_0,
        COMMAND_TASK_STACKSIZE_0,
        COMMAND_TASK_QUEUESIZE_0,
        UNIT_I2C_ADDRESS_0,
#ifdef TMFL_TDA19972_FAMILY
        CEC_I2C_ADDRESS_0,
#endif
        RtxRxI2cReadFunction,
        RtxRxI2cWriteFunction,
        //RES_SUPPORTED_NB_0,
        TMDL_HDMIRX_VIDEORES_UNKNOWN, //it is the size of the enum type for resolutions, so also the number of supported resolutions
        (ptmdlHdmiRxCfgResolution_t) resolutionInfo,
        //RES_TO_BE_DETECTED_NB_0,
        TMDL_HDMIRX_VIDEORES_UNKNOWN, //it is the size of the enum type for resolutions, so also the number of resolutions to be detected
        (ptmdlHdmiRxCfgResolutionDetect_t) resolutionToBeDetectedList,
        (tmdlHdmiRxColorMatrixCoefs_t *) predefinedColorMatrixList,
        PREDEFINED_COLOR_MATRIX_NB_0,
        (ptmbslHdmiRxFunctionsList_t) &bslHdmiRxFunctionsList,
        (ptmdlHdmiRxCfgResolutionTimings_t) resolutionTimingsList,
        &capabilitiesList0,
        KEY_DESCRIPTION_SEED,
        VS_SYNC_DELAY,
        HS_SYNC_DELAY,
        (Bool) CLOCK_PULSE_DELAY,
        CLOCK_PULSE_DELAY_VALUE
    }


};


/* macro for quick error handling */
//#define RETIF(cond, rslt) if ((cond)){WriteLn("Back rx-cfg"); return (rslt);}
#define RETIF(cond, rslt)       if ((cond)) \
	{Write("CFG:"); PRINTIF(1998,__LINE__); \
	return (rslt);\
	}

tmErrorCode_t tmdlHdmiRxCfgGetConfig
(
    tmUnitSelect_t                 unit,
    tmdlHdmiRxDriverConfigTable_t *pConfig
)
{
    /* check if unit number is in range */
//	sprintf(msg,"%d %d",unit,MAX_UNITS);
//	WriteLn(msg);


    dWriteLn("GetConf",debugSolomonMask);
    RETIF((unit < 0) || (unit >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_UNIT_NUMBER)

    /* check if pointer is Null */
    //if (pConfig==Null)
    //	WriteLn("Null");


    RETIF(pConfig == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)

    *pConfig = driverConfigTable[unit];

    return(TM_OK);
};

/*============================================================================*/
/*                            END OF FILE                                     */
/*============================================================================*/
