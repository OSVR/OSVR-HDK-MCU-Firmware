#include "GlobalOptions.h"



/**
 * Copyright (C) 2006 NXP N.V., All Rights Reserved.
 * This source code and any compilation or derivative thereof is the proprietary
 * information of NXP N.V. and is confidential in nature. Under no circumstances
 * is this software to be  exposed to or placed under an Open Source License of
 * any type without the expressed written permission of NXP N.V.
 *
 * \file          tmdlHdmiRx_bslAPItypes.h
 *
 * \version       $Revision: 1 $
 *
 * \date          $Date: 12/10/06 10:32 $
 *
 * \brief         BSL driver component inAPI for the TDA997x HDMI Receiver
 *
 * \section refs  Reference Documents
 * HDMI Rx Driver - FRS.doc,
 * HDMI Rx Driver - tmbslTDA997X - SCS.doc
 *
 * \section info  Change Information
 *
 * \verbatim

   $History: tmdlHdmiRx_bslAPItypes.h $
 *
 * *****************  Version 13  *****************
 * User: Demoment     Date: 13/10/06   Time: 10:32
 * Updated in $/Source/tmbslTDA997X/inc
 * initial version
 *

   \endverbatim
 *
*/

#ifndef TMDLHDMIRX_BSLAPITYPES_H
#define TMDLHDMIRX_BSLAPITYPES_H

/*============================================================================*/
/*                       INCLUDE FILES                                        */
/*============================================================================*/

#include "tmNxCompId.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Define the OK code if not defined already */
#ifndef TM_OK
#define TM_OK   0
#endif


/*============================================================================*/
/*                                DEFINES                                     */
/*============================================================================*/

/**< Error Codes */
#define TMBSL_ERR_BSLHDMIRX_BASE                      CID_BSL_HDMIRX
#define TMBSL_ERR_BSLHDMIRX_COMPATIBILITY             (TMBSL_ERR_BSLHDMIRX_BASE + TM_ERR_COMPATIBILITY)             /**< SW Interface compatibility   */
#define TMBSL_ERR_BSLHDMIRX_MAJOR_VERSION             (TMBSL_ERR_BSLHDMIRX_BASE + TM_ERR_MAJOR_VERSION)             /**< SW Major Version error       */
#define TMBSL_ERR_BSLHDMIRX_COMP_VERSION              (TMBSL_ERR_BSLHDMIRX_BASE + TM_ERR_COMP_VERSION)              /**< SW component version error   */
#define TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER           (TMBSL_ERR_BSLHDMIRX_BASE + TM_ERR_BAD_UNIT_NUMBER)           /**< Invalid device unit number   */
#define TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER             (TMBSL_ERR_BSLHDMIRX_BASE + TM_ERR_BAD_PARAMETER)             /**< Invalid input parameter      */
#define TMBSL_ERR_BSLHDMIRX_RESOURCE_NOT_OWNED        (TMBSL_ERR_BSLHDMIRX_BASE + TM_ERR_RESOURCE_NOT_OWNED)        /**< Caller does not own resource */
#define TMBSL_ERR_BSLHDMIRX_NO_RESOURCES        	  (TMBSL_ERR_BSLHDMIRX_BASE + TM_ERR_NO_RESOURCES)        		/**< Resource is not available */
#define TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS       (TMBSL_ERR_BSLHDMIRX_BASE + TM_ERR_INCONSISTENT_PARAMS)       /**< Inconsistent input params    */
#define TMBSL_ERR_BSLHDMIRX_NOT_INITIALIZED           (TMBSL_ERR_BSLHDMIRX_BASE + TM_ERR_NOT_INITIALIZED)           /**< Component is not initialized */
#define TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED             (TMBSL_ERR_BSLHDMIRX_BASE + TM_ERR_NOT_SUPPORTED)             /**< Function is not supported    */
#define TMBSL_ERR_BSLHDMIRX_INIT_FAILED               (TMBSL_ERR_BSLHDMIRX_BASE + TM_ERR_INIT_FAILED)               /**< Initialization failed        */
#define TMBSL_ERR_BSLHDMIRX_BUSY                      (TMBSL_ERR_BSLHDMIRX_BASE + TM_ERR_BUSY)                      /**< Component is busy            */
#define TMBSL_ERR_BSLHDMIRX_I2C_READ                  (TMBSL_ERR_BSLHDMIRX_BASE + TM_ERR_READ)                      /**< Read error                   */
#define TMBSL_ERR_BSLHDMIRX_I2C_WRITE                 (TMBSL_ERR_BSLHDMIRX_BASE + TM_ERR_WRITE)                     /**< Write error                  */
#define TMBSL_ERR_BSLHDMIRX_ASSERTION                 (TMBSL_ERR_BSLHDMIRX_BASE + TM_ERR_ASSERTION)                 /**< Assertion failure            */
#define TMBSL_ERR_BSLHDMIRX_OPERATION_NOT_PERMITTED   (TMBSL_ERR_BSLHDMIRX_BASE + TM_ERR_OPERATION_NOT_PERMITTED)   /**< corresponds to posix EPERM   */
#define TMBSL_ERR_BSLHDMIRX_NULL_CONNECTION   		  (TMBSL_ERR_BSLHDMIRX_BASE + TM_ERR_NULL_CONNECTION)   		/**< No connection   > */

/* Define to parse the HDMI status values */
#define BSLHDMIRX_HDMISTATUS_AUDIOFLAG 0x80000000U     /**< Audio packet detected in last videoframe (TDA997X only) */
#define BSLHDMIRX_HDMISTATUS_HDMI      0x40000000U     /**< HDMI mode detected */
#define BSLHDMIRX_HDMISTATUS_EESS      0x20000000U     /**< EESS mode detected */
#define BSLHDMIRX_HDMISTATUS_ENCRYPTED 0x10000000U     /**< Data stream is HDCP encrypted */
#define BSLHDMIRX_HDMISTATUS_AVMUTE    0x08000000U     /**< General Control Packet sends "Set_AVMUTE" command */
#define BSLHDMIRX_HDMISTATUS_LAY_ASP   0x04000000U     /**< Layout status Audio Sample Packet */
#define BSLHDMIRX_HDMISTATUS_FAIL      0x02000000U     /**< Fifo read and write pointers are crossed */
#define BSLHDMIRX_HDMISTATUS_WARNING   0x01000000U     /**< Fifo read pointer is closer than 2 samples from the write pointer */

/* Define to parse the audio status values */
#define BSLHDMIRX_AUDIO_STATUS_HARDWARE_MUTE        0x80000000U  /**< Audio is muted (hardware mute) (TDA1997X only)*/
#define BSLHDMIRX_AUDIO_STATUS_DST_PACKET           0x08000000U  /**< DST packets detected (TDA1997X only) */
#define BSLHDMIRX_AUDIO_STATUS_OBA_PACKET           0x04000000U  /**< OBA packets detected (TDA1997X only) */
#define BSLHDMIRX_AUDIO_STATUS_HBR_PACKET           0x02000000U  /**< HBR packets detected (TDA1997X only) */
#define BSLHDMIRX_AUDIO_STATUS_AUDIO_SAMPLE_PACKET  0x01000000U  /**< Audio sample packets detected (TDA1997X only) */

/* When this number of frames is reached consider no Infoframe received */
#define HDMI_INFO_EXCEED 3

/*============================================================================*/
/*                       ENUM OR TYPE DEFINITIONS                             */
/*============================================================================*/
typedef enum
{
    BSLHDMIRX_IRQSOURCE_ASD, //0
    BSLHDMIRX_IRQSOURCE_SOGD2,
    BSLHDMIRX_IRQSOURCE_DCSD2, // 2
    BSLHDMIRX_IRQSOURCE_DSSD2,
    BSLHDMIRX_IRQSOURCE_SOGD1, // 4
    BSLHDMIRX_IRQSOURCE_DCSD1,
    BSLHDMIRX_IRQSOURCE_DSSD1, // 6
    BSLHDMIRX_IRQSOURCE_PLL_LOCKED,
    BSLHDMIRX_IRQSOURCE_SOGD3, // 8
    BSLHDMIRX_IRQSOURCE_DCSD3,
    BSLHDMIRX_IRQSOURCE_DSSD3, // 0a
    BSLHDMIRX_IRQSOURCE_FLAGS,
    BSLHDMIRX_IRQSOURCE_ISRC2, // 0c
    BSLHDMIRX_IRQSOURCE_ISRC1,
    BSLHDMIRX_IRQSOURCE_ACP, // 0e
    BSLHDMIRX_IRQSOURCE_AVI_IF,
#ifdef TMFL_TDA19972_FAMILY
    BSLHDMIRX_IRQSOURCE_VS_OTHER_BK2_IF, // 10
    BSLHDMIRX_IRQSOURCE_VS_OTHER_BK1_IF,
    BSLHDMIRX_IRQSOURCE_VS_IF, // 12
#endif
    BSLHDMIRX_IRQSOURCE_SPD_IF,
    BSLHDMIRX_IRQSOURCE_AUD_IF, // 14
    BSLHDMIRX_IRQSOURCE_MPS_IF,
    BSLHDMIRX_IRQSOURCE_RXDB, // 16
    BSLHDMIRX_IRQSOURCE_RXDA,
    BSLHDMIRX_IRQSOURCE_HDMI_LOCK, // 18
    BSLHDMIRX_IRQSOURCE_STATE_C5,
#ifdef TMFL_TDA19972_FAMILY
    BSLHDMIRX_IRQSOURCE_CONFIG_MTP, // 1a
#else
    BSLHDMIRX_IRQSOURCE_OTP,
#endif
    BSLHDMIRX_IRQSOURCE_AUDIO_PACKET, /**< Audio packet detected */
    BSLHDMIRX_IRQSOURCE_AUDIO_MUTE,   /**< Audio mute status changed */ // 1c
    BSLHDMIRX_IRQSOURCE_RXDC,
    BSLHDMIRX_IRQSOURCE_RXDD, // 1e
    BSLHDMIRX_IRQSOURCE_GAMUT,
    BSLHDMIRX_IRQSOURCE_SUS_STATUS_CHANGED, // 20
    BSLHDMIRX_IRQSOURCE_DEEP_COLOR_MODE_CHANGED,
    BSLHDMIRX_IRQSOURCE_AUDIO_SAMPLE_FREQ_CHANGED, // 22
    BSLHDMIRX_IRQSOURCE_AUDIO_FIFO_UNMUTED,
    BSLHDMIRX_IRQSOURCE_AUDIO_CHANNEL_STATUS, // 24
#ifdef TMFL_TDA19972_FAMILY
    BSLHDMIRX_IRQSOURCE_EDID_MTP,
    BSLHDMIRX_IRQSOURCE_HDCP_MTP, // 26
#endif
#ifdef TMFL_HDMI_OUT
    BSLHDMIRX_IRQSOURCE_HPD,
    BSLHDMIRX_IRQSOURCE_RXS,
    BSLHDMIRX_IRQSOURCE_EDID_READ,
#endif
    BSLHDMIRX_IRQSOURCE_NOT_USED
} tmbslHdmiRxIRQSource_t;

typedef struct
{
    UInt8 slaveAddr;
    UInt8 firstRegister;
    UInt8 lenData;
    UInt8 *pData;
} tmbslHdmiRxSysArgs_t;

typedef tmErrorCode_t (*ptmbslHdmiRxSysFunc_t) (tmbslHdmiRxSysArgs_t *pSysArgs);
typedef void (*ptmbslHdmiRxCallbackFunc_t) (tmbslHdmiRxIRQSource_t IrqSource, UInt8 Status);

/**
 * \brief Timer function pointer type, to call an application timer
 * \param ms delay in milliseconds
 */
typedef tmErrorCode_t (*ptmbslHdmiRxSysFuncTimer_t) (UInt16 ms);

/*============================================================================*/

typedef enum
{
    BSLHDMIRX_TDA_UNKNOWN = 0x00,
    BSLHDMIRX_TDA19978,
    BSLHDMIRX_TDA19974,
    BSLHDMIRX_TDA19977,
    BSLHDMIRX_TDA19979,
    BSLHDMIRX_TDA19971,
    BSLHDMIRX_TDA19972,
    BSLHDMIRX_TDA19973_SOC_IN,
    BSLHDMIRX_TDA19973_CONNECTOR_IN,
    BSLHDMIRX_TDA19971N2,
    BSLHDMIRX_TDA19973N2_SOC_IN,
    BSLHDMIRX_TDA19973N2_CONNECTOR_IN
} tmbslHdmiRxVersion_t;

typedef enum
{
    BSLHDMIRX_SYNCSIGNAL_ANALOG  = 0x00,
    BSLHDMIRX_SYNCSIGNAL_DIGITAL = 0x20
} tmbslHdmiRxInputSelSyncType_t;

typedef enum
{
    BSLHDMIRX_INPUTSELECTION_MANUAL = 0x00,
    BSLHDMIRX_INPUTSELECTION_AUTO   = 0x10
} tmbslHdmiRxInputSelDigitalMode_t;

typedef enum
{
    BSLHDMIRX_DIGITALINPUT_A = 0x08,
    BSLHDMIRX_DIGITALINPUT_B = 0x00,
    BSLHDMIRX_DIGITALINPUT_C = 0x18,    /* TDA19978 only */
    BSLHDMIRX_DIGITALINPUT_D = 0x0C     /* TDA19978 only */
} tmbslHdmiRxInputSelDigitalSource_t;

typedef enum
{
    BSLHDMIRX_VIDEO_SOURCE_ANALOG  = 0x00,
    BSLHDMIRX_VIDEO_SOURCE_DIGITAL = 0x04
} tmbslHdmiRxInputSelVideoSource_t;

typedef enum
{
    BSLHDMIRX_VIDEOINPUT_1    = 0x00,
    BSLHDMIRX_VIDEOINPUT_2    = 0x01,
    BSLHDMIRX_VIDEOINPUT_3    = 0x02,
    BSLHDMIRX_VIDEOINPUT_AUTO = 0x03
} tmbslHdmiRxInputSelAnalogVideoSource_t;

typedef enum
{
    BSLHDMIRX_CLOCK_PULSE_DELAY_OFF = 0x00,
    BSLHDMIRX_CLOCK_PULSE_DELAY_ON  = 0x80
} tmbslHdmiRxInputSelClockPulseDelay_t;

typedef enum
{
    BSLHDMIRX_VDP_RESET_MODE_MANUAL = 0x00,
    BSLHDMIRX_VDP_RESET_MODE_AUTO   = 0x02
} tmbslHdmiRxInputSelVDPResetMode_t;

typedef enum
{
    BSLHDMIRX_VDP_RESET_ON  = 0x00,
    BSLHDMIRX_VDP_RESET_OFF = 0x01
} tmbslHdmiRxInputSelVDPReset_t;

typedef enum
{
    BSLHDMIRX_HDMI_OUT_LOOP   	= 0x00,
    BSLHDMIRX_HDMI_OUT_LOOP_PON	= 0x01,
    BSLHDMIRX_HDMI_OUT_BYPASS 	= 0x02
} tmbslHdmiRxHdmiOutMode_t;

typedef enum
{
    BSLHDMIRX_SOGBURSTFILTER_BYPASS = 0x00,
    BSLHDMIRX_SOGBURSTFILTER_ENABLE = 0x04
} tmbslHdmiRxSogBurst_t;

typedef enum
{
    BSLHDMIRX_SYNCDETECTION_NORMAL = 0x00,
    BSLHDMIRX_SYNCDETECTION_ATV    = 0x80
} tmbslHdmiRxSyncDetectAtv_t;

typedef enum
{
    BSLHDMIRX_CSDETECTION_1_4_3_4 = 0x00,
    BSLHDMIRX_CSDETECTION_LARGER  = 0x01
} tmbslHdmiRxSyncDetectComposite_t;

typedef enum
{
    BSLHDMIRX_SYNCDETECTION_DISABLE = 0x40,
    BSLHDMIRX_SYNCDETECTION_ENABLE  = 0x00
} tmbslHdmiRxSyncDetect_t;

typedef enum
{
    BSLHDMIRX_SOG_FORCED = 0x20,
    BSLHDMIRX_SOG_NORMAL = 0x00
} tmbslHdmiRxForceSog_t;

typedef enum
{
    BSLHDMIRX_DCS_FORCED = 0x10,
    BSLHDMIRX_DCS_NORMAL = 0x00
} tmbslHdmiRxSForceDcs_t;

typedef enum
{
    BSLHDMIRX_VSYNCPOLARITY_AUTO   = 0x00,
    BSLHDMIRX_VSYNCPOLARITY_MANUAL = 0x08
} tmbslHdmiRxSVsyncPolarityMode_t;

typedef enum
{
    BSLHDMIRX_VSYNCTOGGLE_DISABLE = 0x00,
    BSLHDMIRX_VSYNCTOGGLE_ENABLE  = 0x04
} tmbslHdmiRxSVsyncToggle_t;

typedef enum
{
    BSLHDMIRX_HSYNCPOLARITY_AUTO   = 0x00,
    BSLHDMIRX_HSYNCPOLARITY_MANUAL = 0x02
} tmbslHdmiRxSHsyncPolarityMode_t;

typedef enum
{
    BSLHDMIRX_HSYNCTOGGLE_DISABLE = 0x00,
    BSLHDMIRX_HSYNCTOGGLE_ENABLE  = 0x01
} tmbslHdmiRxHsyncToggle_t;

typedef enum
{
    BSLHDMIRX_PLLMDIV_1 = 0x00,
    BSLHDMIRX_PLLMDIV_2 = 0x40,
    BSLHDMIRX_PLLMDIV_4 = 0x80,
    BSLHDMIRX_PLLMDIV_8 = 0xC0
} tmbslHdmiRxPllMdiv_t;

typedef enum
{
    BSLHDMIRX_PLLSYNC_RISINGEDGE  = 0x00,
    BSLHDMIRX_PLLSYNC_FALLINGEDGE = 0x08
} tmbslHdmiRxPllEdgeSync_t;

typedef enum
{
    BSLHDMIRX_PLLCOAST_ENABLE  = 0x00,
    BSLHDMIRX_PLLCOAST_DISABLE = 0x04
} tmbslHdmiRxPllCoastMode_t;

typedef enum
{
    BSLHDMIRX_PLLCOASTSEL_SRDS_VHREF = 0x00,
    BSLHDMIRX_PLLCOASTSEL_SRDS       = 0x01,
    BSLHDMIRX_PLLCOASTSEL_VHREF      = 0x02,
    BSLHDMIRX_PLLCOASTSEL_PIN        = 0x03
} tmbslHdmiRxPllCoastSelection_t;

typedef enum
{
    BSLHDMIRX_ADCPOLARITY_POSITIVE = 0x00,
    BSLHDMIRX_ADCPOLARITY_NEGATIVE = 0x80
} tmbslHdmiRxADCPolarity_t;

typedef enum
{
    BSLHDMIRX_CLOCKOUTDELAY_OFF = 0x00,
    BSLHDMIRX_CLOCKOUTDELAY_ON  = 0x40
} tmbslHdmiRxADCClkDelay_t;

typedef enum
{
    BSLHDMIRX_CURRENT_BPB_25  = 0x00,
    BSLHDMIRX_CURRENT_BPB_50  = 0x10,
    BSLHDMIRX_CURRENT_BPB_100 = 0x20,
    BSLHDMIRX_CURRENT_BPB_150 = 0x30
} tmbslHdmiRxADCCurrentPBp_t;

typedef enum
{
    BSLHDMIRX_CURRENT_GY_25  = 0x00,
    BSLHDMIRX_CURRENT_GY_50  = 0x04,
    BSLHDMIRX_CURRENT_GY_100 = 0x08,
    BSLHDMIRX_CURRENT_GY_150 = 0x0C
} tmbslHdmiRxADCCurrentGy_t;

typedef enum
{
    BSLHDMIRX_CURRENT_RPR_25  = 0x00,
    BSLHDMIRX_CURRENT_RPR_50  = 0x01,
    BSLHDMIRX_CURRENT_RPR_100 = 0x02,
    BSLHDMIRX_CURRENT_RPR_150 = 0x03
} tmbslHdmiRxADCCurrentPRp_t;

typedef enum
{
    BSLHDMIRX_DLL_CLKOUTDIV_2 = 0x00,
    BSLHDMIRX_DLL_CLKOUTDIV_4 = 0x20,
    BSLHDMIRX_DLL_CLKOUTDIV_8 = 0x40
} tmbslHdmiRxDllDivClkOut_t;

typedef enum
{
    BSLHDMIRX_DLL_CLKPIXDIV_2 = 0x00,
    BSLHDMIRX_DLL_CLKPIXDIV_4 = 0x04,
    BSLHDMIRX_DLL_CLKPIXDIV_8 = 0x08
} tmbslHdmiRxDllDivClkPix_t;

typedef enum
{
    BSLHDMIRX_DLL_PRESETDELAY_OFF = 0x00,
    BSLHDMIRX_DLL_PRESETDELAY_ON  = 0x02
} tmbslHdmiRxDllDivPrDelay_t;

typedef enum
{
    BSLHDMIRX_DLL_PHASEEDGE_RAISING = 0x00,
    BSLHDMIRX_DLL_PHASEEDGE_FALLING = 0x01
} tmbslHdmiRxDllDivPhEdge_t;

typedef enum
{
    BSLHDMIRX_PIXCLK_OUT_TOGGLE_OFF = 0x00,
    BSLHDMIRX_PIXCLK_OUT_TOGGLE_ON  = 0x80
} tmbslHdmiRxPixClkToggle_t;

typedef enum
{
    BSLHDMIRX_PIXCLK_OUT_HDMICLOCK    = 0x20,
    BSLHDMIRX_PIXCLK_OUT_HDMICLOCK_X2 = 0x30,
    BSLHDMIRX_PIXCLK_OUT_AVICLKOUT    = 0x40,
    BSLHDMIRX_PIXCLK_OUT_AVICLKFOR    = 0x50,
    BSLHDMIRX_PIXCLK_OUT_AVICLKPIX    = 0x60
} tmbslHdmiRxPixClkOutSelection_t;

typedef enum
{
    BSLHDMIRX_OUT_CLOCK_SINGLE_EDGE,
    BSLHDMIRX_OUT_CLOCK_DUAL_EDGE
} tmbslHdmiRxOutClockEdgeMode_t;

typedef enum
{
    BSLHDMIRX_PIXCLK_FOR_AVICLKFOR    = 0x04,
    BSLHDMIRX_PIXCLK_FOR_CLOCKHDMI_X2 = 0x08,
    BSLHDMIRX_PIXCLK_FOR_ZERO         = 0x0C
} tmbslHdmiRxPixClkForSelection_t;

typedef enum
{
    BSLHDMIRX_PIXCLK_PIX_AVICLKPIX    = 0x02,
    BSLHDMIRX_PIXCLK_PIX_HDMICLOCK    = 0x03
} tmbslHdmiRxPixClkPixSelection_t;

typedef enum
{
    BSLHDMIRX_CLAMP_UPDATE_4096_FRAMES = 0x00,
    BSLHDMIRX_CLAMP_UPDATE_2048_FRAMES = 0x03,
    BSLHDMIRX_CLAMP_UPDATE_1024_FRAMES = 0x04,
    BSLHDMIRX_CLAMP_UPDATE_512_FRAMES  = 0x05,
    BSLHDMIRX_CLAMP_UPDATE_256_FRAMES  = 0x06,
    BSLHDMIRX_CLAMP_UPDATE_128_FRAMES  = 0x07,
    BSLHDMIRX_CLAMP_UPDATE_64_FRAMES   = 0x08,
    BSLHDMIRX_CLAMP_UPDATE_32_FRAMES   = 0x09,
    BSLHDMIRX_CLAMP_UPDATE_16_FRAMES   = 0x0A,
    BSLHDMIRX_CLAMP_UPDATE_8_FRAMES    = 0x0B,
    BSLHDMIRX_CLAMP_UPDATE_4_FRAMES    = 0x0C,
    BSLHDMIRX_CLAMP_UPDATE_2_FRAMES    = 0x0F
} tmbslHdmiRxClampUpdateRate_t;

typedef enum
{
    BSLHDMIRX_CLAMP_OFF_DIGITALADJUST = 0x00,
    BSLHDMIRX_CLAMP_ON_DIGITALADJUST  = 0x02
} tmbslHdmiRxClampDigitalAdjustment_t;

typedef enum
{
    BSLHDMIRX_CLAMP_PUSLE_VHREF  = 0x00,
    BSLHDMIRX_CLAMP_PULSE_EXTERN = 0x01
} tmbslHdmiRxClampPulseSelection_t;

typedef struct
{
    UInt16  Gain;
    UInt8   High;
    UInt8   Low;
} tmbslHdmiRxGainValue_t;

typedef enum
{
    BSLHDMIRX_GAIN_PULSE_VHREF = 0x00,
    BSLHDMIRX_GAIN_PULSE_EXTERN = 0x20
} tmbslHdmiRxGainPulseSelection_t;

typedef enum
{
    BSLHDMIRX_COLORSP_MXBYPASS_OFF = 0x00,
    BSLHDMIRX_COLORSP_MXBYPASS_ON  = 0x04
} tmbslHdmiRxColorSpaceBypass_t;

typedef enum
{
    BSLHDMIRX_COLORSPACE_SCALE_256  = 256,
    BSLHDMIRX_COLORSPACE_SCALE_512  = 512,
    BSLHDMIRX_COLORSPACE_SCALE_1024 = 1024
} tmbslHdmiRxColorSpaceScaleFactor_t;

typedef struct
{
    Int16   offInt1;
    Int16   offInt2;
    Int16   offInt3;
    Int16   P11Coef;
    Int16   P12Coef;
    Int16   P13Coef;
    Int16   P21Coef;
    Int16   P22Coef;
    Int16   P23Coef;
    Int16   P31Coef;
    Int16   P32Coef;
    Int16   P33Coef;
    Int16   offOut1;
    Int16   offOut2;
    Int16   offOut3;
} tmbslHdmiRxColorSpaceCoefficients_t;

typedef enum
{
    BSLHDMIRX_VHREF_INTERLACED_FRAMEFIELD = 0x00,
    BSLHDMIRX_VHREF_INTERLACED_ALTERNATE  = 0x80
} tmbslHdmiRxVhrefInterlaceDetection_t;

typedef enum
{
    BSLHDMIRX_VHREF_FRAMEVSYNC_AUTO = 0x00,
    BSLHDMIRX_VHREF_FRAMEVSYNC_FDW  = 0x20,
    BSLHDMIRX_VHREF_FRAMEVSYNC_EVEN = 0x40,
    BSLHDMIRX_VHREF_FRAMEVSYNC_ODD  = 0x60
} tmbslHdmiRxVhrefFrameVsync_t;

typedef enum
{
    BSLHDMIRX_VHREF_STANDARD_PAL  = 0x00,
    BSLHDMIRX_VHREF_STANDARD_NTSC = 0x08,
    BSLHDMIRX_VHREF_STANDARD_AUTO = 0x10,
    BSLHDMIRX_VHREF_STANDARD_OFF  = 0x18
} tmbslHdmiRxVhrefStandardDetection_t;

typedef enum
{
    BSLHDMIRX_VHREF_VREF_MANUAL        = 0x00,
    BSLHDMIRX_VHREF_VREF_FROM_STANDARD = 0x04
} tmbslHdmiRxVhrefVrefProg_t;

typedef enum
{
    BSLHDMIRX_VHREF_HREF_MANUAL        = 0x00,
    BSLHDMIRX_VHREF_HREF_FROM_STANDARD = 0x02
} tmbslHdmiRxVhrefHrefProg_t;

typedef enum
{
    BSLHDMIRX_VHREF_CSYNC_ACTIVE_LOW  = 0x00,
    BSLHDMIRX_VHREF_CSYNC_ACTIVE_HIGH = 0x01
} tmbslHdmiRxVhrefCSyncPol_t;

typedef enum
{
    BSLHDMIRX_FREF_FIELD1_LOW_FIELD2_HIGH = 0x00,
    BSLHDMIRX_FREF_FIELD1_HIGH_FIELD2_LOW = 0x80
} tmbslHdmiRxFrefPolarity_t;

typedef struct
{
    UInt16                    href_start;
    UInt16                    href_end;
    UInt16                    vref_f1_start;
    UInt8                     vef_f1_width;
    UInt16                    vref_f2_start;
    UInt8                     vef_f2_width;
    UInt16                    fieldref_f1_start;
    tmbslHdmiRxFrefPolarity_t fieldPolarity;
    UInt16                    fieldref_f2_start;
} tmbslHdmiRxVHRefValues_t;

typedef struct
{
    UInt16 hsync_start;
    UInt16 hsync_end;
    UInt16 vsync_f1_start;
    UInt8  vsync_f1_width;
    UInt16 vsync_f2_start;
    UInt8  vsync_f2_width;
    UInt16 vsync_f1_pix_start;
    UInt16 vsync_f1_pix_end;
    UInt16 vsync_f2_pix_start;
    UInt16 vsync_f2_pix_end;
} tmbslHdmiRxVHSyncValues_t;

typedef struct
{
    UInt16 coast_f1_start;
    UInt8  coast_f1_width;
    UInt16 coast_f2_start;
    UInt8  coast_f2_width;
} tmbslHdmiRxCoastSignalValues_t;

typedef struct
{
    UInt16 clamp_pix_start;
    UInt16 clamp_pix_end;
    UInt16 clamp_f1_line_start;
    UInt8  clamp_f1_line_width;
    UInt16 clamp_f2_line_start;
    UInt8  clamp_f2_line_width;
} tmbslHdmiRxClampSignalValues_t;

typedef enum
{
    BSLHDMIRX_ASD_FRAME_PROGRESSIVE = 0x00,
    BSLHDMIRX_ASD_FRAME_INTERLACED  = 0x80
} tmbslHdmiRxAsdMeasureInterlaced_t;

typedef enum
{
    BSLHDMIRX_ASD_LINES_DONTMATCH_STANDARD = 0x00,
    BSLHDMIRX_ASD_LINES_MATCH_STANDARD     = 0x40
} tmbslHdmiRxVhrefAsdLineStandard_t;

typedef enum
{
    BSLHDMIRX_625_ASD_PROGRESSIVE_LINES = 0x00,
    BSLHDMIRX_525_ASD_PROGRESSIVE_LINES = 0x20
} tmbslHdmiRxVhrefAsdMeaslin525_t;

typedef enum
{
    BSLHDMIRX_PREFILTER_OFF    = 0x00,
    BSLHDMIRX_PREFILTER_2TAPS  = 0x01,
    BSLHDMIRX_PREFILTER_7TAPS  = 0X02,
    BSLHDMIRX_PREFILTER_27TAPS = 0x03
} tmbslHdmiRxPreFilterConfig_t;

#ifdef TMFL_TDA19972_FAMILY
typedef enum
{
    BSLHDMIRX_OUTPUT_HIGHIMPEDANCE  = 0x00,
    BSLHDMIRX_OUTPUT_ACTIVE 		= 0x80
} tmbslHdmiRxOutputControl_t;

typedef enum
{
    BSLHDMIRX_OUTPUT_VIDEO_LOW           = 0x00,
    BSLHDMIRX_OUTPUT_VIDEO_HIGHIMPEDANCE = 0x40
} tmbslHdmiRxOutputVideoPort_t;

typedef enum
{
    BSLHDMIRX_OUTPUT_BLANKING_INSERT = 0x10,
    BSLHDMIRX_OUTPUT_BLANKING_REMOVE = 0x00
} tmbslHdmiRxOutputBlankingCodes_t;

typedef enum
{
    BSLHDMIRX_OUTPUT_TIMINGREF_INSERT = 0x08,
    BSLHDMIRX_OUTPUT_TIMINGREF_REMOVE = 0x00
} tmbslHdmiRxOutputTimingRefs_t;

#endif

typedef enum
{
    BSLHDMIRX_OUTPUT_VHREF_DISABLE = 0x00,
    BSLHDMIRX_OUTPUT_VHREF_ENABLE  = 0x20
} tmbslHdmiRxOutputRef_t;

typedef enum
{
    BSLHDMIRX_OUTPUT_FORMAT_444      = 0x00,
    BSLHDMIRX_OUTPUT_FORMAT_422_SMPT = 0x01,
#ifndef TMFL_DOWNSAMPLED_FORMATS
    BSLHDMIRX_OUTPUT_FORMAT_422_CCIR = 0x02
#else
    BSLHDMIRX_OUTPUT_FORMAT_422_CCIR = 0x02,
    BSLHDMIRX_OUTPUT_FORMAT_422_SMPT_DIV2 = 0x05,
    BSLHDMIRX_OUTPUT_FORMAT_422_CCIR_DIV2 = 0x06,
    BSLHDMIRX_OUTPUT_FORMAT_422_CCIR_DIV4 = 0x07
#endif
} tmbslHdmiRxOutputFormat_t;

typedef enum
{
    BSLHDMIRX_DOWNSAMPLE_BYPASS = 0x00,
    BSLHDMIRX_DOWNSAMPLE_2      = 0x04,
    BSLHDMIRX_DOWNSAMPLE_4      = 0x08
} tmbslHdmiRxDownSampler_t;

typedef enum
{
    BSLHDMIRX_UPSAMPLE_BYPASS       = 0x00,
    BSLHDMIRX_UPSAMPLE_REPEATCHROMA = 0x01,
    BSLHDMIRX_UPSAMPLE_INTERPOLATE  = 0x02
} tmbslHdmiRxUpSampler_t;

typedef enum
{
    BSLHDMIRX_INTERNALVSYNC_SDRS          = 0x00,
    BSLHDMIRX_INTERNALVSYNC_HDMI          = 0x04,
    BSLHDMIRX_INTERNALVSYNC_HDMI_FILTERED = 0x06
} tmbslHdmiRxInternalVSync_t;

typedef enum
{
    BSLHDMIRX_INTERNALHSYNC_PLL         = 0x00,
    BSLHDMIRX_INTERNALHSYNC_SDRS        = 0x01,
    BSLHDMIRX_INTERNALHSYNC_HDMI        = 0x02,
    BSLHDMIRX_INTERNALHSYNC_HDMI_DATAEN = 0x03
} tmbslHdmiRxInternalHSync_t;

typedef enum
{
    BSLHDMIRX_SYNCOUTPUT_CS_SDRS  = 0x00,
    BSLHDMIRX_SYNCOUTPUT_CS_HS_VS = 0x20,
    BSLHDMIRX_SYNCOUTPUT_CS_FREF  = 0x40
} tmbslHdmiRxSyncOutputCSSelection_t;

typedef enum
{
    BSLHDMIRX_SYNCOUTPUT_VSYNC_SDRS           = 0x00,
    BSLHDMIRX_SYNCOUTPUT_VSYNC_VHREF          = 0x08, /* do not use, VS values not programmed */
    BSLHDMIRX_SYNCOUTPUT_VSYNC_HDMI           = 0x10,
    BSLHDMIRX_SYNCOUTPUT_VSYNC_HDMI_FILTERED  = 0x18,
    BSLHDMIRX_SYNCOUTPUT_VREF_VHREF           = 0x28
} tmbslHdmiRxSyncOutputVSSelection_t;

typedef enum
{
    BSLHDMIRX_SYNCOUTPUT_HSYNC_PLL   = 0x00,
    BSLHDMIRX_SYNCOUTPUT_HSYNC_SDRS  = 0x02,
    BSLHDMIRX_SYNCOUTPUT_HSYNC_HDMI  = 0x03,
    BSLHDMIRX_SYNCOUTPUT_HSYNC_VHREF = 0x04,	/* do not use, HS values not programmed */
    BSLHDMIRX_SYNCOUTPUT_HREF_VHREF  = 0x05,
    BSLHDMIRX_SYNCOUTPUT_FREF_VHREF  = 0x07
} tmbslHdmiRxSyncOutputHSSelection_t;

typedef enum
{
    BSLHDMIRX_POLARITY_DATAENOUT_POSITIVE = 0x00,
    BSLHDMIRX_POLARITY_DATAENOUT_NEGATIVE = 0x40
} tmbslHdmiRxPolarityDataEnOut_t;

typedef enum
{
    BSLHDMIRX_POLARITY_CSYNCOUT_POSITIVE = 0x00,
    BSLHDMIRX_POLARITY_CSYNCOUT_NEGATIVE = 0x20
} tmbslHdmiRxPolarityCSyncOut_t;

typedef enum
{
    BSLHDMIRX_POLARITY_HSYNCOUT_POSITIVE = 0x00,
    BSLHDMIRX_POLARITY_HSYNCOUT_NEGATIVE = 0x10
} tmbslHdmiRxPolarityHSyncOut_t;

typedef enum
{
    BSLHDMIRX_POLARITY_VSYNCOUT_POSITIVE = 0x00,
    BSLHDMIRX_POLARITY_VSYNCOUT_NEGATIVE = 0x08
} tmbslHdmiRxPolarityVSyncOut_t;

typedef enum
{
    BSLHDMIRX_POSITIVE_POLARITY_FIELDREFOUT = 0x00,
    BSLHDMIRX_NEGATIVE_POLARITY_FIELDREFOUT = 0x04
} tmbslHdmiRxPolarityFieldrefOut_t;

typedef enum
{
    BSLHDMIRX_POLARITY_HREFOUT_POSITIVE = 0x00,
    BSLHDMIRX_POLARITY_HREFOUT_NEGATIVE = 0x02
} tmbslHdmiRxPolarityHRefOut_t;

typedef enum
{
    BSLHDMIRX_POLARITY_VREFOUT_POSITIVE = 0x00,
    BSLHDMIRX_POLARITY_VREFOUT_NEGATIVE = 0x01
} tmbslHdmiRxPolarityVRefOut_t;

typedef enum
{
    BSLHDMIRX_VIDEOPORT_12BITS = 0x00,
    BSLHDMIRX_VIDEOPORT_10BITS = 0x40,
    BSLHDMIRX_VIDEOPORT_8BITS  = 0x80
} tmbslHdmiRxVideoPortFormat_t;

typedef enum
{
    BSLHDMIRX_VIDEOPORT_RV            = 0x00,
    BSLHDMIRX_VIDEOPORT_BU            = 0x01,
    BSLHDMIRX_VIDEOPORT_GY            = 0x02,
    BSLHDMIRX_VIDEOPORT_HIGHIMPEDANCE = 0x03
} tmbslHdmiRxVideoPortSelection_t;

#ifdef TMFL_TDA19972_FAMILY

#define VP24BIT		0x00
#define VP30BIT 	0x40
#define VP36BIT 	0x80

#define VP_MASK		0xE0

#define G2_1_0		0x00
#define G4_3_0		0x01
#define G4_5_2		0x02
#define G4_7_4		0x03
#define G4_9_6		0x04
#define G4_11_8		0x05
#define G2_11_10	0x06
#define G4_15_12	0x07
#define G4_19_16	0x08
#define G2_21_20	0x09
#define G4_23_20	0x0A
#define G4_25_22	0x0B
#define G4_27_24	0x0C
#define G4_29_26	0x0D
#define G4_31_28	0x0E
#define G4_35_32	0x0F

#define G_MASK		0x0F

#define R_CR_CBCR_3_0 	0x80
#define R_CR_CBCR_7_4 	0x81
#define R_CR_CBCR_11_8 	0x82
#define B_CB_3_0		0x83
#define B_CB_7_4		0x84
#define B_CB_11_8		0x85
#define G_Y_3_0			0x86
#define G_Y_7_4			0x87
#define G_Y_11_8		0x88

#define SWAP		0x10


typedef enum
{
    BSLHDMIRX_VP24_G4_3_0			= (VP24BIT | G4_3_0),
    BSLHDMIRX_VP24_G4_7_4			= (VP24BIT | G4_7_4),
    BSLHDMIRX_VP24_G4_11_8			= (VP24BIT | G4_11_8),
    BSLHDMIRX_VP24_G4_15_12			= (VP24BIT | G4_15_12),
    BSLHDMIRX_VP24_G4_19_16			= (VP24BIT | G4_19_16),
    BSLHDMIRX_VP24_G4_23_20			= (VP24BIT | G4_23_20),
    BSLHDMIRX_VP30_G2_1_0			= (VP30BIT | G2_1_0),
    BSLHDMIRX_VP30_G4_5_2			= (VP30BIT | G4_5_2),
    BSLHDMIRX_VP30_G4_9_6			= (VP30BIT | G4_9_6),
    BSLHDMIRX_VP30_G2_11_10			= (VP30BIT | G2_11_10),
    BSLHDMIRX_VP30_G4_15_12			= (VP30BIT | G4_15_12),
    BSLHDMIRX_VP30_G4_19_16			= (VP30BIT | G4_19_16),
    BSLHDMIRX_VP30_G2_21_20			= (VP30BIT | G2_21_20),
    BSLHDMIRX_VP30_G4_25_22			= (VP30BIT | G4_25_22),
    BSLHDMIRX_VP30_G4_29_26			= (VP30BIT | G4_29_26),
    BSLHDMIRX_VP36_G4_3_0			= (VP36BIT | G4_3_0),
    BSLHDMIRX_VP36_G4_7_4			= (VP36BIT | G4_7_4),
    BSLHDMIRX_VP36_G4_11_8			= (VP36BIT | G4_11_8),
    BSLHDMIRX_VP36_G4_15_12			= (VP36BIT | G4_15_12),
    BSLHDMIRX_VP36_G4_19_16			= (VP36BIT | G4_19_16),
    BSLHDMIRX_VP36_G4_23_20			= (VP36BIT | G4_23_20),
    BSLHDMIRX_VP36_G4_27_24			= (VP36BIT | G4_27_24),
    BSLHDMIRX_VP36_G4_31_28			= (VP36BIT | G4_31_28),
    BSLHDMIRX_VP36_G4_35_32			= (VP36BIT | G4_35_32),
    BSLHDMIRX_UNDEFINED				= 0xFF
} tmbslHdmiRxVideoPortPinGroup_t;

typedef enum
{
    BSLHDMIRX_LOZ						= 0x00,
    BSLHDMIRX_HIZ						= 0x40,
    BSLHDMIRX_R_CR_CBCR_YCBCR_3_0		= R_CR_CBCR_3_0,
    BSLHDMIRX_R_CR_CBCR_YCBCR_7_4		= R_CR_CBCR_7_4,
    BSLHDMIRX_R_CR_CBCR_YCBCR_11_8		= R_CR_CBCR_11_8,
    BSLHDMIRX_B_CB_3_0					= B_CB_3_0,
    BSLHDMIRX_B_CB_7_4					= B_CB_7_4,
    BSLHDMIRX_B_CB_11_8					= B_CB_11_8,
    BSLHDMIRX_G_Y_3_0					= G_Y_3_0,
    BSLHDMIRX_G_Y_7_4					= G_Y_7_4,
    BSLHDMIRX_G_Y_11_8					= G_Y_11_8,
    BSLHDMIRX_R_CR_CBCR_YCBCR_3_0_S		= (R_CR_CBCR_3_0 | SWAP),
    BSLHDMIRX_R_CR_CBCR_YCBCR_7_4_S		= (R_CR_CBCR_7_4 | SWAP),
    BSLHDMIRX_R_CR_CBCR_YCBCR_11_8_S	= (R_CR_CBCR_11_8 | SWAP),
    BSLHDMIRX_B_CB_3_0_S				= (B_CB_3_0 | SWAP),
    BSLHDMIRX_B_CB_7_4_S				= (B_CB_7_4 | SWAP),
    BSLHDMIRX_B_CB_11_8_S				= (B_CB_11_8 | SWAP),
    BSLHDMIRX_G_Y_3_0_S					= (G_Y_3_0 | SWAP),
    BSLHDMIRX_G_Y_7_4_S					= (G_Y_7_4 | SWAP),
    BSLHDMIRX_G_Y_11_8_S				= (G_Y_11_8 | SWAP)
} tmbslHdmiRxVideoColorQuartet_t;
#endif

typedef enum
{
    BSLHDMIRX_HREF_EXPAND_OFF = 0x00,
    BSLHDMIRX_HREF_EXPAND_ON  = 0x80
} tmbslHdmiRxDataEnHRefExpand_t;

typedef enum
{
    BSLHDMIRX_DATAEN_HREF_DONTFORCE_LOW = 0x00,
    BSLHDMIRX_DATAEN_HREF_FORCE_LOW     = 0x40
} tmbslHdmiRxDataEnHRefForceLow_t;

typedef enum
{
    BSLHDMIRX_DATAEN_EXPAND_OFF = 0x00,
    BSLHDMIRX_DATAEN_EXPAND_ON  = 0x20
} tmbslHdmiRxDataEnExpand_t;

typedef enum
{
    BSLHDMIRX_DATAEN_SELECTION_HDMI  = 0x00,
#ifdef TMFL_TDA19972_FAMILY
    BSLHDMIRX_DATAEN_FREF_VHREF = 0x01,
#endif
    BSLHDMIRX_DATAEN_SELECTION_VHREF = 0x10
} tmbslHdmiRxDataEnSelection_t;

typedef enum
{
    BSLHDMIRX_TRCALIBRATION_AUTO   = 0x00,
    BSLHDMIRX_TRCALIBRATION_MANUAL = 0x40
} tmbslHdmiRxTRCalibrationMode_t;

typedef enum
{
    BSLHDMIRX_RESET_AUTO   = 0x00,
    BSLHDMIRX_RESET_MANUAL = 0x80
} tmbslHdmiRxResetMode_t;

typedef enum
{
    BSLHDMIRX_RESET_AVI     = 0x00,
    BSLHDMIRX_DONTRESET_AVI = 0x40
} tmbslHdmiRxResetAVI_t;

typedef enum
{
    BSLHDMIRX_GP_LOW  = 0x00,
    BSLHDMIRX_GP_HIGH = 0x01
} tmbslHdmiRxGPOutput_t;

typedef enum
{
    BSLHDMIRX_PLLOUT_LOCK_HIGH = 0x00,
    BSLHDMIRX_PLLOUT_LOCK_LOW  = 0x08
} tmbslHdmiRxPllOutputLock_t;

typedef enum
{
    BSLHDMIRX_PLLOUT_SIGNAL_PLL_LOCK  = 0x03,
    BSLHDMIRX_PLLOUT_SIGNAL_OSC_CLOCK = 0x05,
    BSLHDMIRX_PLLOUT_SIGNAL_HDMI_LOCK = 0x06
} tmbslHdmiRxPllOutputSignal_t;

typedef enum
{
    BSLHDMIRX_OUTOFRANGE_SGN_RPR_ADC      = 0x00,
    BSLHDMIRX_OUTOFRANGE_SGN_RCR          = 0x40,
    BSLHDMIRX_OUTOFRANGE_SGN_VHREF_GAIN   = 0x80
} tmbslHdmiRxOutOfRangeRCR_t;

typedef enum
{
    BSLHDMIRX_GY_ADC_OUTOFRANGE_SGN       = 0x00,
    BSLHDMIRX_GY_OUTOFRANGE_SGN           = 0x08,
    BSLHDMIRX_VHREF_CLAMP_OUTOFRANGE_SGN  = 0x10
} tmbslHdmiRxOutOfRangeGY_t;

typedef enum
{
    BSLHDMIRX_OUTOFRANGE_SGN_BPB_ADC = 0x00,
    BSLHDMIRX_OUTOFRANGE_SGN_BCB     = 0x01
} tmbslHdmiRxOutOfRangeBCB_t;

typedef enum
{
    BSLHDMIRX_HDMIMUTE_CTRL_PACKET = 0x00,
    BSLHDMIRX_HDMIMUTE_OFF            = 0x40,
    BSLHDMIRX_HDMIMUTE_ON             = 0x80
} tmbslHdmiRxHdmiMute_t;

typedef enum
{
    BSLHDMIRX_HDCPMODE_AUTO = 0x00,
    BSLHDMIRX_HDCPMODE_OESS = 0x10,
    BSLHDMIRX_HDCPMODE_EESS = 0x20
} tmbslHdmiRxHdcpMode_t;

typedef enum
{
    BSLHDMIRX_HDMIPROTOCOL_AUTO = 0x00,
    BSLHDMIRX_HDMIPROTOCOL_DVI  = 0x04,
    BSLHDMIRX_HDMIPROTOCOL_HDMI = 0x08
} tmbslHdmiRxHdmiProtocol_t;

typedef enum
{
    BSLHDMIRX_HDMIVSYNCPOLARITY_AUTO   = 0x00,
    BSLHDMIRX_HDMIVSYNCPOLARITY_MANUAL = 0x02
} tmbslHdmiRxHdmiVsyncPolarity_t;

typedef enum
{
    BSLHDMIRX_HDMIVSYNCTOGGLE_POSITIVE = 0x00,
    BSLHDMIRX_HDMIVSYNCTOGGLE_NEGATIVE = 0x01
} tmbslHdmiRxHdmiVsyncToggle_t;

typedef enum
{
    BSLHDMIRX_AUDIOPATH_SP_FLAG_IGNORED_BY_FIFO = 0x00,
    BSLHDMIRX_AUDIOPATH_SP_FLAG_USED_BY_FIFO    = 0x80
} tmbslHdmiRxAudioPathSpFlagMode_t;

typedef enum
{
    BSLHDMIRX_AUDIOPATH_LAYOUT_PACKET_HEADER = 0x00,
    BSLHDMIRX_AUDIOPATH_LAYOUT_FORCED        = 0x02
} tmbslHdmiRxAudioPathLayoutMode_t;

typedef enum
{
    BSLHDMIRX_AUDIO_LAYOUT_FORCED_0 = 0x00,
    BSLHDMIRX_AUDIO_LAYOUT_FORCED_1 = 0x01
} tmbslHdmiRxAudioPathForcedLayout_t;

typedef enum
{
    BSLHDMIRX_TESTTONE_OFF = 0x00,
    BSLHDMIRX_TESTTONE_ON  = 0x80
} tmbslHdmiRxAudioTestTone_t;

typedef enum
{
    BSLHDMIRX_AUDIOOUTPUT_I2S   = 0x00,
    BSLHDMIRX_AUDIOOUTPUT_SPDIF = 0x40
} tmbslHdmiRxAudioOutputFormat_t;

typedef enum
{
    BSLHDMIRX_AUDIOOUTPUT_NORMAL       = 0x00,
    BSLHDMIRX_AUDIOOUTPUT_FORCE_TO_LOW = 0x01
} tmbslHdmiRxAudioOutputForce_t;

typedef enum
{
    BSLHDMIRX_CLOCK_NORMAL_OPERATION = 0x00,
    BSLHDMIRX_CLOCK_POWERDOWN        = 0x80
} tmbslHdmiRxAudioClockPowerMode_t;

typedef enum
{
    BSLHDMIRX_CLOCKDIVIDER_NOUPDATE = 0x00,
    BSLHDMIRX_CLOCKDIVIDER_UPDATE   = 0x20
} tmbslHdmiRxAudioClockDividerUpdate_t;

typedef enum
{
    BSLHDMIRX_CLOCKDIVIDER_MODE_AUTO   = 0x00,
    BSLHDMIRX_CLOCKDIVIDER_MODE_MANUAL = 0x10
} tmbslHdmiRxAudioClockDividerMode_t;

typedef enum
{
    BSLHDMIRX_AUDIOFREQ_300_600HZ    = 0x00,
    BSLHDMIRX_AUDIOFREQ_600_900HZ    = 0x04,
    BSLHDMIRX_AUDIOFREQ_900_1200HZ   = 0x08,
    BSLHDMIRX_AUDIOFREQ_1200_1500HZ  = 0x0C
} tmbslHdmiRxAudioClockFrequency_t;

typedef enum
{
    BSLHDMIRX_SAMPLEFREQ_96_192KHZ = 0x00,
    BSLHDMIRX_SAMPLEFREQ_48_96KHZ  = 0x01,
    BSLHDMIRX_SAMPLEFREQ_24_48KHZ  = 0x02
} tmbslHdmiRxAudioSampleFrequency_t;

typedef enum
{
    BSLHDMIRX_AUTO_SYNCPOLARITY    = 0x80,
    BSLHDMIRX_MANUAL_SYNCPOLARITY  = 0x00
} tmbslHdmiRxAutomaticSyncPolarity_t;

typedef enum
{
    BSLHDMIRX_AUDIO_SAMPLE_PACKET_MODE  = 0x00,
    BSLHDMIRX_ONE_BIT_AUDIO_PACKET_MODE = 0x10,
    BSLHDMIRX_HBR_AUDIO_PACKET_MODE     = 0x01,
    BSLHDMIRX_DST_AUDIO_PACKET_MODE     = 0x03
} tmbslHdmiRxAudioPacketMode_t;

typedef enum
{
    BSLHDMIRX_POSITIVE_ONLY_AUDIOPLL_REF         = 0x00,
    BSLHDMIRX_POSITIVE_AND_NEGATIVE_AUDIOPLL_REF = 0x08
} tmbslHdmiRxAudioPLLInputRef_t;

typedef enum
{
    BSLHDMIRX_I2S_RESOLUTION_16_BITS = 0x00,
    BSLHDMIRX_I2S_RESOLUTION_32_BITS = 0x04
} tmbslHdmiRxAudioI2SResolution_t;

typedef enum
{
    BSLHDMIRX_AUDIOPLL_SELECTION_NORMAL = 0x00,
    BSLHDMIRX_AUDIOPLL_SELECTION_MCLK   = 0x01
} tmbslHdmiRxAudioPllSelection_t;

typedef enum
{
    BSLHDMIRX_HBR_STRAIGHT_OUTPUT_MODE    = 0x00,
    BSLHDMIRX_HBR_DEMUX_OUTPUT_MODE       = 0x04
} tmbslHdmiRxHBROutputMode_t;

typedef enum
{
    BSLHDMIRX_PACKET_TYPE_SCAN_VENDOR_SPEC = 0x81,
    BSLHDMIRX_PACKET_TYPE_SCAN_AVI         = 0x82,
    BSLHDMIRX_PACKET_TYPE_SCAN_SPD         = 0x83,
    BSLHDMIRX_PACKET_TYPE_SCAN_AUDIO       = 0x84,
    BSLHDMIRX_PACKET_TYPE_SCAN_MPEG        = 0x85
} tmbslHdmiRxPacketTypeScan_t;

typedef enum
{
    BSLHDMIRX_AUDIO_AUTOMUTE_OFF = 0x00,
    BSLHDMIRX_AUDIO_AUTOMUTE_ON  = 0x01
} tmbslHdmiRxAudioAutoMute_t;

typedef enum
{
    BSLHDMIRX_HDMICLOCK_NORMAL = 0x00,
    BSLHDMIRX_HDMICLOCK_MCLK   = 0x08
} tmbslHdmiRxHdmiClock_t;

typedef enum
{
    BSLHDMIRX_ASP_LAYOUT_0 = 0x00,
    BSLHDMIRX_ASP_LAYOUT_1 = 0x10
} tmbslHdmiRxAspLayout_t;

typedef enum
{
    BSLHDMIRX_ASP_SAMPLE_NOT_PRESENT = 0x00,
    BSLHDMIRX_ASP_SAMPLE_PRESENT     = 0x01
} tmbslHdmiRxAspSamplePresent_t;

typedef enum
{
    BSLHDMIRX_ASP_NOT_FIRST_FRAME_IN_BLOCK = 0x00,
    BSLHDMIRX_ASP_FIRST_FRAME_IN_BLOCK     = 0x01
} tmbslHdmiRxAspFirstFrame_t;

typedef enum
{
    BSLHDMIRX_ASP_SAMPLE_IS_NOT_FLATLINE = 0x00,
    BSLHDMIRX_ASP_SAMPLE_IS_FLATLINE     = 0x01
} tmbslHdmiRxAspFlatLine_t;

typedef struct
{
    UInt8                         PacketType;
    tmbslHdmiRxAspLayout_t        Layout;
    tmbslHdmiRxAspSamplePresent_t Sample3Present;
    tmbslHdmiRxAspSamplePresent_t Sample2Present;
    tmbslHdmiRxAspSamplePresent_t Sample1Present;
    tmbslHdmiRxAspSamplePresent_t Sample0Present;
    tmbslHdmiRxAspFirstFrame_t    FirstFrame3;
    tmbslHdmiRxAspFirstFrame_t    FirstFrame2;
    tmbslHdmiRxAspFirstFrame_t    FirstFrame1;
    tmbslHdmiRxAspFirstFrame_t    FirstFrame0;
    tmbslHdmiRxAspFlatLine_t      Flatline3;
    tmbslHdmiRxAspFlatLine_t      Flatline2;
    tmbslHdmiRxAspFlatLine_t      Flatline1;
    tmbslHdmiRxAspFlatLine_t      Flatline0;
} tmbslHdmiRxAudioSampleInfo_t;

typedef enum
{
    BSLHDMIRX_OTP_NO_COMMAND     = 0x00,
    BSLHDMIRX_OTP_START_DOWNLOAD = 0x04,
    BSLHDMIRX_OTP_START_READ     = 0x08
} tmbslHdmiRxOtpCommand_t;

typedef enum
{
    BSLHDMIRX_OTP_BCH_DISABLED = 0x00,
    BSLHDMIRX_OTP_BCH_ENABLED  = 0x02
} tmbslHdmiRxBchEnable_t;

typedef enum
{
    BSLHDMIRX_HDCP_DECRYPTKEY_OFF = 0x00,
    BSLHDMIRX_HDCP_DECRYPTKEY_ON  = 0x02
} tmbslHdmiRxHdcpDecryptKey_t;

typedef enum
{
    BSLHDMIRX_HDCP_DISABLE = 0x00,
    BSLHDMIRX_HDCP_ENABLE  = 0x01
} tmbslHdmiRxHdcpEnable_t;

typedef enum
{
    BSLHDMIRX_RXCAPS_REPEATER_OFF = 0x00,
    BSLHDMIRX_RXCAPS_REPEATER_ON  = 0x40
} tmbslHdmiRxCapsRepeater_t;

typedef enum
{
    BSLHDMIRX_RXCAPS_FAST_I2C_OFF = 0x00,
    BSLHDMIRX_RXCAPS_FAST_I2C_ON  = 0x10
} tmbslHdmiRxCapsFastI2c_t;

typedef enum
{
    BSLHDMIRX_RXCAPS_HDMI_1_1_OFF = 0x00,
    BSLHDMIRX_RXCAPS_HDMI_1_1_ON  = 0x02
} tmbslHdmiRxCapsHdmi11_t;

typedef enum
{
    BSLHDMIRX_RXCAPS_FAST_REAUTH_OFF = 0x00,
    BSLHDMIRX_RXCAPS_FAST_REAUTH_ON  = 0x01
} tmbslHdmiRxCapsFastReauth_t;




typedef enum
{
    BSLHDMIRX_HDCPREPEATER_MAXDEV_OK       = 0x00,
    BSLHDMIRX_HDCPREPEATER_MAXDEV_EXCEEDED = 0x80
} tmbslHdmiRxHDCPRepStatusMaxDev_t;

typedef enum
{
    BSLHDMIRX_OK_HDCPREPEATER_MAXCASC       = 0x00,
    BSLHDMIRX_EXCEEDED_HDCPREPEATER_MAXCASC = 0x08
} tmbslHdmiRxHDCPRepStatusMaxCascade_t;

typedef enum
{
    BSLHDMIRX_HDCP_DEMEASUREMENT_1_VDP = 0x00,
    BSLHDMIRX_HDCP_DEMEASUREMENT_2_VDP = 0x40,
    BSLHDMIRX_HDCP_DEMEASUREMENT_4_VDP = 0x80,
    BSLHDMIRX_HDCP_DEMEASUREMENT_8_VDP = 0xc0
} tmbslHdmiRxDEMeasurementMode_t;

typedef enum
{
    BSLHDMIRX_HDCP_DEREGENERATION_DISABLE = 0x00,
    BSLHDMIRX_HDCP_DEREGENERATION_ENABLE  = 0x20
} tmbslHdmiRxDERegeneration_t;

typedef enum
{
    BSLHDMIRX_HDCP_SENSIVITY_0_DEFILTER = 0x00,
    BSLHDMIRX_HDCP_SENSIVITY_1_DEFILTER = 0x08,
    BSLHDMIRX_HDCP_SENSIVITY_2_DEFILTER = 0x10,
    BSLHDMIRX_HDCP_SENSIVITY_3_DEFILTER = 0x18
} tmbslHdmiRxDEFilterSensivity_t;

typedef enum
{
    BSLHDMIRX_CH0_HDCP_DECOMPOSITION   = 0x00,
    BSLHDMIRX_CH1_HDCP_DECOMPOSITION   = 0x01,
    BSLHDMIRX_CH2_HDCP_DECOMPOSITION   = 0x02,
    BSLHDMIRX_AND_HDCP_DECOMPOSITION   = 0x03,
    BSLHDMIRX_OR_HDCP_DECOMPOSITION    = 0x04,
    BSLHDMIRX_MIXED_HDCP_DECOMPOSITION = 0x05
} tmbslHdmiRxDECompositionMode_t;

typedef enum
{
    BSLHDMIRX_HDCP_0_CTLFILTERSENSIVITY = 0x00,
    BSLHDMIRX_HDCP_1_CTLFILTERSENSIVITY = 0x10,
    BSLHDMIRX_HDCP_2_CTLFILTERSENSIVITY = 0x20,
    BSLHDMIRX_HDCP_3_CTLFILTERSENSIVITY = 0x30
} tmbslHdmiRxCTLFilterSensivity_t;

typedef enum
{
    BSLHDMIRX_HDCP_0_VSFILTERSENSIVITY = 0x00,
    BSLHDMIRX_HDCP_1_VSFILTERSENSIVITY = 0x04,
    BSLHDMIRX_HDCP_2_VSFILTERSENSIVITY = 0x08,
    BSLHDMIRX_HDCP_3_VSFILTERSENSIVITY = 0x0c
} tmbslHdmiRxVSFilterSensivity_t;

typedef enum
{
    BSLHDMIRX_HDCP_0_HSFILTERSENSIVITY = 0x00,
    BSLHDMIRX_HDCP_1_HSFILTERSENSIVITY = 0x01,
    BSLHDMIRX_HDCP_2_HSFILTERSENSIVITY = 0x02,
    BSLHDMIRX_HDCP_3_HSFILTERSENSIVITY = 0x03
} tmbslHdmiRxHSFilterSensivity_t;

typedef enum
{
    BSLHDMIRX_INFOFRAME_AVI,
    BSLHDMIRX_INFOFRAME_SPD,
    BSLHDMIRX_INFOFRAME_AUD,
    BSLHDMIRX_INFOFRAME_MPS
#ifdef TMFL_TDA19972_FAMILY
    ,BSLHDMIRX_INFOFRAME_VS,
    BSLHDMIRX_INFOFRAME_VS_OTHER_BK1,
    BSLHDMIRX_INFOFRAME_VS_OTHER_BK2
#endif
} tmbslHdmiRxInfoFrame_t;

typedef enum
{
    BSLHDMIRX_DONTNACK_HDCP = 0x00,
    BSLHDMIRX_NACK_HDCP     = 0x80
} tmbslHdmiRxHdmiNackHdcp_t;

typedef enum
{
    BSLHDMIRX_DONTRESET_AI = 0x00,
    BSLHDMIRX_RESET_AI     = 0x04
} tmbslHdmiRxHdmiResetAi_t;

typedef enum
{
    BSLHDMIRX_DONTRESET_INFOFRAME = 0x00,
    BSLHDMIRX_RESET_INFOFRAME     = 0x02
} tmbslHdmiRxHdmiResetInfoframe_t;

#ifdef TMFL_TDA19972_FAMILY
typedef enum
{
    BSLHDMIRX_DONTRESET_AUDIO_FIFO = 0x00,
    BSLHDMIRX_RESET_AUDIO_FIFO     = 0x10
} tmbslHdmiRxHdmiResetAudioFifo_t;
#else
typedef enum
{
    BSLHDMIRX_DONTRESET_AUDIO_FIFO = 0x00,
    BSLHDMIRX_RESET_AUDIO_FIFO     = 0x01
} tmbslHdmiRxHdmiResetAudioFifo_t;
#endif

typedef enum
{
    BSLHDMIRX_DONTRESET_GAMUT = 0x00,
    BSLHDMIRX_RESET_GAMUT     = 0x08
} tmbslHdmiRxHdmiResetGamut_t;

typedef enum
{
    BSLHDMIRX_DONTRESET_FIFOCTRL = 0x00,
    BSLHDMIRX_RESET_FIFOCTRL     = 0x20
} tmbslHdmiRxHdmiResetFifoCtrl_t;

typedef enum
{
    BSLHDMIRX_DONTRESET_SUS = 0x00,
    BSLHDMIRX_RESET_SUS     = 0x01
#ifdef ALLOW_DDC_ACK
    , BSLHDMIRX_DONTRESET_SUS_DDC = 0x80,
    BSLHDMIRX_RESET_SUS_DDC     = 0x81
#endif
} tmbslHdmiRxHdmiResetSus_t;

typedef enum
{
    BSLHDMIRX_DONTRESET_DC = 0x00,
    BSLHDMIRX_RESET_DC     = 0x80
} tmbslHdmiRxHdmiResetDc_t;

typedef enum
{
    BSLHDMIRX_HDMI_VERSION_UNKNOWN,   /**< Unknown   */
    BSLHDMIRX_HDMI_VERSION_1_0,       /**< HDMI 1.0  */
    BSLHDMIRX_HDMI_VERSION_1_1,       /**< HDMI 1.1  */
    BSLHDMIRX_HDMI_VERSION_1_2,       /**< HDMI 1.2  */
    BSLHDMIRX_HDMI_VERSION_1_2a,      /**< HDMI 1.2a */
    BSLHDMIRX_HDMI_VERSION_1_3,       /**< HDMI 1.3 */
    BSLHDMIRX_HDMI_VERSION_1_3a,      /**< HDMI 1.3a  */
    BSLHDMIRX_HDMI_VERSION_1_4,       /**< HDMI 1.4 */
    BSLHDMIRX_HDMI_VERSION_1_4a       /**< HDMI 1.4a */
} tmbslHdmiRxHdmiVersion_t;


typedef struct
{
    Bool HBR;              /**< High Bitrate Audio packet            */
    Bool DST;              /**< Direct Stream Transport audio packet */
    Bool oneBitAudio;       /**< One Bit Audio sample packet          */
} tmbslHdmiRxAudioPacket_t;


typedef struct
{
    tmbslHdmiRxVersion_t       deviceVersion;  /**< HW device version */
    Bool                       repeater;       /**< HDCP repeater capability (True/False) */
    Bool                       fastI2C;        /**< Fast I2C (400kHz) capability True/False) */
    tmbslHdmiRxHdmiVersion_t   hdmiVersion;    /**< Supported HDMI standard version  */
    tmbslHdmiRxAudioPacket_t   audioPacket;    /**< Supported audio packets */
    Bool                       fastReauth;     /**< Fast re-auth capability (HDCP) */
    Bool                       analogInput;    /**< Analog input */
    Bool                       hdmi11Features; /**< Support of HDMI 1.1 features */
    Bool                       internalEdid;   /**< Internal EDID feature */
} tmbslHdmiRxCapabilities_t;

/**
* \brief Audio Clocks power-down mode
*/
typedef enum
{
    BSLHDMIRX_CLOCKS_MODE_NORMAL    = 0x00,
    BSLHDMIRX_CLOCKS_MODE_POWERDOWN = 0x80
} tmbslHdmiRxAudioClocksModePowerMode_t;

/**
* \brief Audio Clocks reference mode
*/
typedef enum
{
    BSLHDMIRX_CLOCKS_MODE_REFMODE_XTAL = 0x00,
    BSLHDMIRX_CLOCKS_MODE_REFMODE_TMDS = 0x40
} tmbslHdmiRxAudioClocksModeRefMode_t;

/**
* \brief Audio Clocks reference frequency
*/
typedef enum
{
    BSLHDMIRX_CLOCKS_MODE_REFFREQ_13_5 = 0x00,
    BSLHDMIRX_CLOCKS_MODE_REFFREQ_27   = 0x10,
    BSLHDMIRX_CLOCKS_MODE_REFFREQ_54   = 0x20
} tmbslHdmiRxAudioClocksModeRefFreq_t;


/**
* \brief Audio Clocks mode
*/
#ifdef TMFL_TDA19972_FAMILY
typedef enum
{
    BSLHDMIRX_CLOCKS_MODE_16FS_AUDIOMODE  = 0x00,
    BSLHDMIRX_CLOCKS_MODE_32FS_AUDIOMODE  = 0x01,
    BSLHDMIRX_CLOCKS_MODE_64FS_AUDIOMODE  = 0x02,
    BSLHDMIRX_CLOCKS_MODE_128FS_AUDIOMODE = 0x03,
    BSLHDMIRX_CLOCKS_MODE_256FS_AUDIOMODE = 0x04,
    BSLHDMIRX_CLOCKS_MODE_512FS_AUDIOMODE = 0x05
} tmbslHdmiRxAudioClocksAudioMode_t;
#else
typedef enum
{
    BSLHDMIRX_CLOCKS_MODE_32FS_AUDIOMODE  = 0x00,
    BSLHDMIRX_CLOCKS_MODE_64FS_AUDIOMODE  = 0x04,
    BSLHDMIRX_CLOCKS_MODE_128FS_AUDIOMODE = 0x08
} tmbslHdmiRxAudioClocksAudioMode_t;
#endif


/**
* \brief Audio Clocks SYSCLK mode
*/
typedef enum
{
    BSLHDMIRX_CLOCKS_MODE_128FS_SYSCLKMODE = 0x00,
    BSLHDMIRX_CLOCKS_MODE_256FS_SYSCLKMODE = 0x01,
    BSLHDMIRX_CLOCKS_MODE_512FS_SYSCLKMODE = 0x02
} tmbslHdmiRxAudioClocksSysMode_t;

/**
* \brief Enums for format measurements.
*/
typedef enum
{
    BSLHDMIRX_FMT_MEAS_PROGRESSIVE = 0x00,
    BSLHDMIRX_FMT_MEAS_INTERLACED  = 0x04
} tmbslHdmiRxFmtMeasInterlaced_t;

typedef enum
{
    BSLHDMIRX_FMT_MEAS_VS_NEGATIVE = 0x00,
    BSLHDMIRX_FMT_MEAS_VS_POSITIVE = 0x02
} tmbslHdmiRxFmtMeasVSPol_t;

typedef enum
{
    BSLHDMIRX_FMT_MEAS_HS_NEGATIVE = 0x00,
    BSLHDMIRX_FMT_MEAS_HS_POSITIVE = 0x01
} tmbslHdmiRxFmtMeasHSPol_t;

typedef enum
{
    BSLHDMIRX_FMT_MEAS_DE_NOT_PRESENT = 0x00,
    BSLHDMIRX_FMT_MEAS_DE_PRESENT     = 0x01
} tmbslHdmiRxFmtMeasDEPresent_t;

/* Structure for better format measurements */
typedef struct
{
    tmbslHdmiRxFmtMeasInterlaced_t videoFormat;                 /**< (0x1F) Enum for interlaced or progressive format */
    tmbslHdmiRxFmtMeasVSPol_t      vsPolarity;                  /**< (0x1F) Enum for +ve or -ve polarity of the VS */
    tmbslHdmiRxFmtMeasHSPol_t      hsPolarity;                  /**< (0x1F) Enum for +ve or -ve polarity of the HS */
    UInt16                         horizontalTotalPeriod;       /**< (0x20/0x21) Indicates the period of 1 line counted in pixel clock period */
    UInt16                         horizontalVideoActiveWidth;  /**< (0x22/0x23) Indicates the period of hor video active length counted in pixel clock period */
    UInt16                         horizontalFrontPorchWidth;   /**< (0x24/0x25) Indicates the width of the front porch area before the first edge of the HS */
    UInt16                         horizontalSyncWidthPixClk;   /**< (0x26/0x27) Indicates the width of the HS pulse: Measured in pixel clock period */
    UInt16                         horizontalBackPorchWidth;    /**< (0x28/0x29) Indicates the width of the back porch area after the last edge of the HS but before the start of the hor video active area */
    UInt16                         verticalTotalPeriod;         /**< (0x2A/0x2B) Indicates the period of a frame counted in line numbers */
    UInt16                         verticalVideoActiveWidth;    /**< (0x2C/0x2D) Indicates the width of the vertical video active area counted in line numbers */
    UInt8                          verticalFrontPorchWidthF1;   /**< (0x2E) Vertical front porch width of Field 1 */
    UInt8                          verticalFrontPorchWidthF2;   /**< (0x2F) Vertical front porch width of Field 2 */
    UInt8                          verticalSyncWidth;           /**< (0x30) Width of the VS counted in the line numbers */
    UInt8                          verticalBackPorchWidthF1;    /**< (0x31) Vertical back porch width of Field 1 */
    UInt8                          verticalBackPorchWidthF2;    /**< (0x32) Vertical back porch width of Field 2 */
    tmbslHdmiRxFmtMeasDEPresent_t  dataEnablePresent;           /**< (0x33) DE signal present or absent */
} tmbslHdmiRxFormatMeas_t;

/**
* \brief DST audio sample rate
*/
typedef enum
{
    BSLHDMIRX_AUDIO_DSTRATE_SINGLE = 0x00,
    BSLHDMIRX_AUDIO_DSTRATE_DOUBLE = 0x80
} tmbslHdmiRxAudioDstRate_t;

/**
* \brief Audio sample frequencies
*/
typedef enum
{
    BSLHDMIRX_AUDIO_SAMPLEFREQ_ERROR   = 0x00,
    BSLHDMIRX_AUDIO_SAMPLEFREQ_32_KHZ  = 0x01,
    BSLHDMIRX_AUDIO_SAMPLEFREQ_44_KHZ  = 0x02,
    BSLHDMIRX_AUDIO_SAMPLEFREQ_48_KHZ  = 0x03,
    BSLHDMIRX_AUDIO_SAMPLEFREQ_88_KHZ  = 0x04,
    BSLHDMIRX_AUDIO_SAMPLEFREQ_96_KHZ  = 0x05,
    BSLHDMIRX_AUDIO_SAMPLEFREQ_176_KHZ = 0x06,
    BSLHDMIRX_AUDIO_SAMPLEFREQ_192_KHZ = 0x07
} tmbslHdmiRxAudioSampleFreq_t;

/**
* \brief Enums for internal EDID
*/
typedef enum
{
    BSLHDMIRX_EDID_NACK_OFF = 0x00,
    BSLHDMIRX_EDID_NACK_ON  = 0x80
} tmbslHdmiRxEDIDNack_t;

typedef enum
{
    BSLHDMIRX_EDID_RESET_OFF = 0x00,
    BSLHDMIRX_EDID_RESET_ON  = 0x40
} tmbslHdmiRxEDIDReset_t;

typedef enum
{
    BSLHDMIRX_EDID_PD_OFF = 0x00,
    BSLHDMIRX_EDID_PD_ON  = 0x20
} tmbslHdmiRxEDIDPowerDown_t;

typedef enum
{
    BSLHDMIRX_EDID_PD_DDC_OFF = 0x00,
    BSLHDMIRX_EDID_PD_DDC_ON  = 0x10
} tmbslHdmiRxEDIDPowerDownDDC_t;

typedef enum
{
    BSLHDMIRX_DEEPCOLORMODE_NOTINDICATED = 0x00,
    BSLHDMIRX_DEEPCOLORMODE_24           = 0x04,
    BSLHDMIRX_DEEPCOLORMODE_30           = 0x05,
    BSLHDMIRX_DEEPCOLORMODE_36           = 0x06,
    BSLHDMIRX_DEEPCOLORMODE_48           = 0x07
} tmbslHdmiRxDeepColorMode_t;

typedef enum
{
    BSLHDMIRX_TESTPATTERN_OFF = 0x00,
#ifdef TMFL_TDA19972_FAMILY
    BSLHDMIRX_TESTPATTERN_ON = 0x01,
    BSLHDMIRX_TESTPATTERN_RAMP = 0x09
#else
    BSLHDMIRX_TESTPATTERN_ON  = 0x80
#endif
} tmbslHdmiRxTestPatternSel_t;

typedef enum
{
    BSLHDMIRX_TESTPATTERN_480P = 0x20,
    BSLHDMIRX_TESTPATTERN_576P = 0x60
#ifdef TMFL_TDA19972_FAMILY
    ,BSLHDMIRX_TESTPATTERN_480I = 0x00,
    BSLHDMIRX_TESTPATTERN_576I = 0x40
#endif
} tmbslHdmiRxTestPatternFormat_t;

typedef enum
{
    BSLHDMIRX_TMDS_B_CLK		= 0x00,
    BSLHDMIRX_TMDS_B_CLK_INV	= 0x01
} tmbslHdmiRxBClk_t;

typedef enum
{
    BSLHDMIRX_TMDS_B_SOC		= 0x00,
    BSLHDMIRX_TMDS_B_CONNECTOR	= 0x01
} tmbslHdmiRxBSoc_t;

typedef enum
{
    BSLHDMIRX_TMDS_VP_24		= 0x00,
    BSLHDMIRX_TMDS_VP_30		= 0x01,
    BSLHDMIRX_TMDS_VP_36		= 0x02
} tmbslHdmiVP_t;

typedef enum
{
    BSLHDMIRX_OUT_SUP_3_3		= 0x00,
    BSLHDMIRX_OUT_SUP_2_5		= 0x01
} tmbslHdmiOutSupply;

typedef enum
{
    BSLHDMIRX_CEC_DISABLED		= 0x00,
    BSLHDMIRX_CEC_ENABLED		= 0x01
} tmbslHdmiCECEnabled_t;

typedef struct
{
    tmbslHdmiRxBClk_t		B_Clk;
    tmbslHdmiRxBSoc_t 		B_Soc;
    UInt8					configuration;
    UInt8					revision;
    tmbslHdmiVP_t			videoPort;
    tmbslHdmiOutSupply		outputSupply;
    tmbslHdmiCECEnabled_t	CECEnabled;
} tmbslHdmiChipConfiguration_t;


#ifdef TMFL_TDA19972_FAMILY
typedef enum {
    BSLHDMIRX_HPD_PULSE,
    BSLHDMIRX_HPD_LOW_BP,
    BSLHDMIRX_HPD_HIGH_BP,
    BSLHDMIRX_HPD_LOW_OTHER,
    BSLHDMIRX_HPD_HIGH_OTHER
} tmbslHdmiHPDManual_t;
#endif



#ifdef TMFL_HDMI_OUT
/** Number of 1 byte Short Video Descriptors stored in pEdidVFmts */
#define HDMI_RX_SVD_MAX_CNT     113

/** number of detailed timing descriptor stored in BSL */
#define NUMBER_DTD_STORED       10

/** Number of 3 byte Short Audio Descriptors stored in pEdidAFmts */
#define HDMI_RX_SAD_MAX_CNT     30

/** size descriptor block of monitor descriptor */
#define EDID_MONITOR_DESCRIPTOR_SIZE   13

/** Sink device type */
typedef enum
{
    HDMIRX_SINK_DVI                = 0,    /**< DVI  */
    HDMIRX_SINK_HDMI               = 1,    /**< HDMI */
    HDMIRX_SINK_EDID               = 2,    /**< As currently defined in EDID */
    HDMIRX_SINK_INVALID            = 3     /**< Invalid   */
} tmbslHdmiRxSinkType_t;

/*============================================================================*/
/**
 * \brief The tmbslTDA9984EdidGetVideoPreferred() parameter type
 * Detailed timining description structure
 */
typedef struct _tmbslHdmiRxEdidDtd_t
{
    UInt16  uPixelClock;        /**< Pixel Clock/10,000         */
    UInt16  uHActivePixels;     /**< Horizontal Active Pixels   */
    UInt16  uHBlankPixels;      /**< Horizontal Blanking Pixels */
    UInt16  uVActiveLines;      /**< Vertical Active Lines      */
    UInt16  uVBlankLines;       /**< Vertical Blanking Lines    */
    UInt16  uHSyncOffset;       /**< Horizontal Sync Offset     */
    UInt16  uHSyncWidth;        /**< Horiz. Sync Pulse Width    */
    UInt16  uVSyncOffset;       /**< Vertical Sync Offset       */
    UInt16  uVSyncWidth;        /**< Vertical Sync Pulse Width  */
    UInt16  uHImageSize;        /**< Horizontal Image Size      */
    UInt16  uVImageSize;        /**< Vertical Image Size        */
    UInt16  uHBorderPixels;     /**< Horizontal Border          */
    UInt16  uVBorderPixels;     /**< Vertical Border            */
    UInt8   Flags;              /**< Interlace/sync info        */
} tmbslHdmiRxEdidDtd_t;

/**
 * \brief Enum listing all IA/CEA 861-D video formats
 */
typedef enum
{
    HDMIRX_VFMT_NULL               = 0,    /**< Not a valid format...        */
    HDMIRX_VFMT_NO_CHANGE          = 0,    /**< ...or no change required     */
    HDMIRX_VFMT_MIN                = 1,    /**< Lowest valid format          */
    HDMIRX_VFMT_TV_MIN             = 1,    /**< Lowest valid TV format       */
    HDMIRX_VFMT_01_640x480p_60Hz   = 1,    /**< Format 01 640  x 480p  60Hz  */
    HDMIRX_VFMT_02_720x480p_60Hz   = 2,    /**< Format 02 720  x 480p  60Hz  */
    HDMIRX_VFMT_03_720x480p_60Hz   = 3,    /**< Format 03 720  x 480p  60Hz  */
    HDMIRX_VFMT_04_1280x720p_60Hz  = 4,    /**< Format 04 1280 x 720p  60Hz  */
    HDMIRX_VFMT_05_1920x1080i_60Hz = 5,    /**< Format 05 1920 x 1080i 60Hz  */
    HDMIRX_VFMT_06_720x480i_60Hz   = 6,    /**< Format 06 720  x 480i  60Hz  */
    HDMIRX_VFMT_07_720x480i_60Hz   = 7,    /**< Format 07 720  x 480i  60Hz  */
    HDMIRX_VFMT_08_720x240p_60Hz   = 8,    /**< Format 08 720  x 240p  60Hz  */
    HDMIRX_VFMT_09_720x240p_60Hz   = 9,    /**< Format 09 720  x 240p  60Hz  */
    HDMIRX_VFMT_10_720x480i_60Hz   = 10,   /**< Format 10 720  x 480i  60Hz  */
    HDMIRX_VFMT_11_720x480i_60Hz   = 11,   /**< Format 11 720  x 480i  60Hz  */
    HDMIRX_VFMT_12_720x240p_60Hz   = 12,   /**< Format 12 720  x 240p  60Hz  */
    HDMIRX_VFMT_13_720x240p_60Hz   = 13,   /**< Format 13 720  x 240p  60Hz  */
    HDMIRX_VFMT_14_1440x480p_60Hz  = 14,   /**< Format 14 1440 x 480p  60Hz  */
    HDMIRX_VFMT_15_1440x480p_60Hz  = 15,   /**< Format 15 1440 x 480p  60Hz  */
    HDMIRX_VFMT_16_1920x1080p_60Hz = 16,   /**< Format 16 1920 x 1080p 60Hz  */
    HDMIRX_VFMT_17_720x576p_50Hz   = 17,   /**< Format 17 720  x 576p  50Hz  */
    HDMIRX_VFMT_18_720x576p_50Hz   = 18,   /**< Format 18 720  x 576p  50Hz  */
    HDMIRX_VFMT_19_1280x720p_50Hz  = 19,   /**< Format 19 1280 x 720p  50Hz  */
    HDMIRX_VFMT_20_1920x1080i_50Hz = 20,   /**< Format 20 1920 x 1080i 50Hz  */
    HDMIRX_VFMT_21_720x576i_50Hz   = 21,   /**< Format 21 720  x 576i  50Hz  */
    HDMIRX_VFMT_22_720x576i_50Hz   = 22,   /**< Format 22 720  x 576i  50Hz  */
    HDMIRX_VFMT_23_720x288p_50Hz   = 23,   /**< Format 23 720  x 288p  50Hz  */
    HDMIRX_VFMT_24_720x288p_50Hz   = 24,   /**< Format 24 720  x 288p  50Hz  */
    HDMIRX_VFMT_25_720x576i_50Hz   = 25,   /**< Format 25 720  x 576i  50Hz  */
    HDMIRX_VFMT_26_720x576i_50Hz   = 26,   /**< Format 26 720  x 576i  50Hz  */
    HDMIRX_VFMT_27_720x288p_50Hz   = 27,   /**< Format 27 720  x 288p  50Hz  */
    HDMIRX_VFMT_28_720x288p_50Hz   = 28,   /**< Format 28 720  x 288p  50Hz  */
    HDMIRX_VFMT_29_1440x576p_50Hz  = 29,   /**< Format 29 1440 x 576p  50Hz  */
    HDMIRX_VFMT_30_1440x576p_50Hz  = 30,   /**< Format 30 1440 x 576p  50Hz  */
    HDMIRX_VFMT_31_1920x1080p_50Hz = 31,   /**< Format 31 1920 x 1080p 50Hz  */
    HDMIRX_VFMT_32_1920x1080p_24Hz = 32,   /**< Format 32 1920 x 1080p 24Hz  */
    HDMIRX_VFMT_33_1920x1080p_25Hz = 33,   /**< Format 33 1920 x 1080p 25Hz  */
    HDMIRX_VFMT_34_1920x1080p_30Hz = 34,   /**< Format 34 1920 x 1080p 30Hz  */
    HDMIRX_VFMT_35_2880x480p_60Hz  = 35,   /**< Format 35 2880 x 480p  60Hz 4:3  */
    HDMIRX_VFMT_36_2880x480p_60Hz  = 36,   /**< Format 36 2880 x 480p  60Hz 16:9 */
    HDMIRX_VFMT_37_2880x576p_50Hz  = 37,   /**< Format 37 2880 x 576p  50Hz 4:3  */
    HDMIRX_VFMT_38_2880x576p_50Hz  = 38,   /**< Format 38 2880 x 576p  50Hz 16:9 */

    HDMIRX_VFMT_INDEX_60_1280x720p_24Hz = 39,/**< Index of HDMITX_VFMT_60_1280x720p_24Hz */
    HDMIRX_VFMT_60_1280x720p_24Hz  = 60,   /**< Format 60 1280 x 720p  23.97/24Hz 16:9 */
    HDMIRX_VFMT_61_1280x720p_25Hz  = 61,   /**< Format 61 1280 x 720p  25Hz 16:9 */
    HDMIRX_VFMT_62_1280x720p_30Hz  = 62,   /**< Format 60 1280 x 720p  29.97/30Hz 16:9 */

    HDMIRX_VFMT_TV_MAX             = 62,   /**< Highest valid TV format      */
    HDMIRX_VFMT_TV_NO_REG_MIN      = 32,   /**< Lowest TV format without prefetched table */
    HDMIRX_VFMT_TV_NUM             = 42,   /**< Number of TV formats & null  */

    HDMIRX_VFMT_PC_MIN             = 128,  /**< Lowest valid PC format       */
    HDMIRX_VFMT_PC_640x480p_60Hz   = 128,  /**< PC format 128                */
    HDMIRX_VFMT_PC_800x600p_60Hz   = 129,  /**< PC format 129                */
    HDMIRX_VFMT_PC_1152x960p_60Hz  = 130,  /**< PC format 130                */
    HDMIRX_VFMT_PC_1024x768p_60Hz  = 131,  /**< PC format 131                */
    HDMIRX_VFMT_PC_1280x768p_60Hz  = 132,  /**< PC format 132                */
    HDMIRX_VFMT_PC_1280x1024p_60Hz = 133,  /**< PC format 133                */
    HDMIRX_VFMT_PC_1360x768p_60Hz  = 134,  /**< PC format 134                */
    HDMIRX_VFMT_PC_1400x1050p_60Hz = 135,  /**< PC format 135                */
    HDMIRX_VFMT_PC_1600x1200p_60Hz = 136,  /**< PC format 136                */
    HDMIRX_VFMT_PC_1024x768p_70Hz  = 137,  /**< PC format 137                */
    HDMIRX_VFMT_PC_640x480p_72Hz   = 138,  /**< PC format 138                */
    HDMIRX_VFMT_PC_800x600p_72Hz   = 139,  /**< PC format 139                */
    HDMIRX_VFMT_PC_640x480p_75Hz   = 140,  /**< PC format 140                */
    HDMIRX_VFMT_PC_1024x768p_75Hz  = 141,  /**< PC format 141                */
    HDMIRX_VFMT_PC_800x600p_75Hz   = 142,  /**< PC format 142                */
    HDMIRX_VFMT_PC_1024x864p_75Hz  = 143,  /**< PC format 143                */
    HDMIRX_VFMT_PC_1280x1024p_75Hz = 144,  /**< PC format 144                */
    HDMIRX_VFMT_PC_640x350p_85Hz   = 145,  /**< PC format 145                */
    HDMIRX_VFMT_PC_640x400p_85Hz   = 146,  /**< PC format 146                */
    HDMIRX_VFMT_PC_720x400p_85Hz   = 147,  /**< PC format 147                */
    HDMIRX_VFMT_PC_640x480p_85Hz   = 148,  /**< PC format 148                */
    HDMIRX_VFMT_PC_800x600p_85Hz   = 149,  /**< PC format 149                */
    HDMIRX_VFMT_PC_1024x768p_85Hz  = 150,  /**< PC format 150                */
    HDMIRX_VFMT_PC_1152x864p_85Hz  = 151,  /**< PC format 151                */
    HDMIRX_VFMT_PC_1280x960p_85Hz  = 152,  /**< PC format 152                */
    HDMIRX_VFMT_PC_1280x1024p_85Hz = 153,  /**< PC format 153                */
    HDMIRX_VFMT_PC_1024x768i_87Hz  = 154,  /**< PC format 154                */
    HDMIRX_VFMT_PC_MAX             = 154,  /**< Highest valid PC format      */
    HDMIRX_VFMT_PC_NUM             = (HDMIRX_VFMT_PC_MAX-HDMIRX_VFMT_PC_MIN+1)   /**< Number of PC formats         */
} tmbslHdmiRxVidFmt_t;

/*============================================================================*/
/**
 * \brief The tmbslTDA9984EdidGetAudioCapabilities() parameter type
 */
typedef struct _tmbslHdmiRxEdidSad_t
{
    UInt8   ModeChans;  /* Bits[6:3]: EIA/CEA861 mode; Bits[2:0]: channels */
    UInt8   Freqs;      /* Bits for each supported frequency */
    UInt8   Byte3;      /* EIA/CEA861B p83: data depending on audio mode */
} tmbslHdmiRxEdidSad_t;


/*============================================================================*/
/**
 * First monitor descriptor structure
 */
typedef struct _tmbslHdmiRxEdidFirstMD_t
{
    Bool    bDescRecord;                                    /**< True when parameters of struct are available   */
    UInt8   uMonitorName[EDID_MONITOR_DESCRIPTOR_SIZE];     /**< Monitor Name                                   */
} tmbslHdmiRxEdidFirstMD_t;

/*============================================================================*/
/**
 * Second monitor descriptor structure
 */
typedef struct _tmbslHdmiRxEdidSecondMD_t
{
    Bool    bDescRecord;                                    /**< True when parameters of struct are available   */
    UInt8   uMinVerticalRate;                               /**< Min vertical rate in Hz                        */
    UInt8   uMaxVerticalRate;                               /**< Max vertical rate in Hz                        */
    UInt8   uMinHorizontalRate;                             /**< Min horizontal rate in Hz                      */
    UInt8   uMaxHorizontalRate;                             /**< Max horizontal rate in Hz                      */
    UInt8   uMaxSupportedPixelClk;                          /**< Max suuported pixel clock rate in MHz          */
} tmbslHdmiRxEdidSecondMD_t;

/*============================================================================*/
/**
 * Other monitor descriptor structure
 */
typedef struct _tmbslHdmiRxEdidOtherMD_t
{
    Bool    bDescRecord;                                    /**< True when parameters of struct are available   */
    UInt8   uOtherDescriptor[EDID_MONITOR_DESCRIPTOR_SIZE]; /**< Other monitor Descriptor                       */
} tmbslHdmiRxEdidOtherMD_t;

/*============================================================================*/
/**
 * basic display parameters structure
 */
typedef struct _tmbslHdmiRxEdidBDParam_t
{
    UInt8   uVideoInputDef;      /**< Video Input Definition                       */
    UInt8   uMaxHorizontalSize;  /**< Max. Horizontal Image Size in cm             */
    UInt8   uMaxVerticalSize;    /**< Max. Vertical Image Size in cm               */
    UInt8   uGamma;              /**< Gamma                                        */
    UInt8   uFeatureSupport;     /**< Feature support                              */
} tmbslHdmiRxEdidBDParam_t;

/**
 * tmbslTDA9984EdidGetSinkType() parameter types
 */
typedef struct
{
    Bool   latency_available;
    Bool   Ilatency_available;
    UInt8  Edidvideo_latency;
    UInt8  Edidaudio_latency;
    UInt8  EdidIvideo_latency;
    UInt8  EdidIaudio_latency;

} tmbslHdmiRxEdidLatency_t;

/**
 * \brief Structure defining additional VSDB data
 */
typedef struct
{
    UInt8 maxTmdsClock;      /* maximum supported TMDS clock */
    UInt8 cnc0;              /* content type Graphics (text) */
    UInt8 cnc1;              /* content type Photo */
    UInt8 cnc2;              /* content type Cinema */
    UInt8 cnc3;              /* content type Game */
    UInt8 hdmiVideoPresent;  /* additional video format */
    UInt8 h3DPresent;        /* 3D support by the HDMI Sink */
    UInt8 h3DMultiPresent;   /* 3D multi strctures present */
    UInt8 imageSize;         /* additional info for the values in the image size area */
    UInt8 hdmi3DLen;         /* total length of 3D video formats */
    UInt8 hdmiVicLen;        /* total length of extended video formats */
    UInt8 ext3DData[21];     /* max_len-10, ie: 31-10=21 */
} tmbslHdmiRxEdidExtraVsdbData_t;

/**
 * tmbslTDA9984HotPlugGetStatus() parameter type
 */
/** Current hotplug status */
typedef enum
{
    HDMIRX_HOTPLUG_INACTIVE    = 0,    /**< Hotplug inactive */
    HDMIRX_HOTPLUG_ACTIVE      = 1,    /**< Hotplug active   */
    HDMIRX_HOTPLUG_INVALID     = 2     /**< Invalid Hotplug  */
} tmbslHdmiRxHotPlug_t;

/**
 * tmbslTDA9984RxSenseGetStatus() parameter type
 */
/** Current RX Sense status */
typedef enum
{
    HDMIRX_RX_SENSE_INACTIVE    = 0,    /**< RxSense inactive */
    HDMIRX_RX_SENSE_ACTIVE      = 1,    /**< RxSense active   */
    HDMIRX_RX_SENSE_INVALID     = 2     /**< Invalid RxSense  */
} tmbslHdmiRxRxSense_t;


#endif


#ifdef __cplusplus
}
#endif

#endif /* TMDLHDMIRX_BSLAPITYPES_H */

/*============================================================================*/
/*                            END OF FILE                                     */
/*============================================================================*/



