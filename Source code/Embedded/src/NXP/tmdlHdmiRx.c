
/**
 * Copyright (C) 2006 NXP N.V., All Rights Reserved.
 * Exposed under Apache 2.0 Open Source License with permission of NXP N.V.
 *
 * \file          tmdlHdmiRx_standard.c
 *
 * \version       Revision: 1
 *
 * \date          Date: 05/03/07 10:32
 *
 * \brief         devlib driver component API for the TDA997x HDMI Receiver
 *
 * \section refs  Reference Documents
 * HDMI Rx Driver - FRS.doc,
 *
 * \section info  Change Information
 *
 * \verbatim

   History:       tmdlHdmiRx_standard.c
 *

   \endverbatim
 *
*/

/*============================================================================*/
/*                             INCLUDE FILES                                  */
/*============================================================================*/

#include "GlobalOptions.h"


#include  "Console.h" // for WriteLn
#include "inttypes.h"
#include <stdio.h>
#include "avrhdmi.h"




/*============================================================================*/
/*                             INCLUDE FILES                                  */
/*============================================================================*/

#ifdef __LINUX_ARM_ARCH__
    #include <linux/kernel.h>
#endif

#ifdef TMFL_OS_LINUX_USER
    #include "tmNxTypes.h"
    #include <stdio.h>
#endif

#include "tmdlHdmiRx_IW.h"
#include "tmdlHdmiRx.h"
#include "tmdlHdmiRx_local.h"

#include "tmbslHdmiRx_functions.h"
#include "tmbslTDA1997X_Functions.h"
#include "tmdlHdmiRx_cfg.h"

/*============================================================================*/
/*                          TYPES DECLARATIONS                                */
/*============================================================================*/
#ifdef TMFL_OS_WINDOWS
    #define DUMMY_ACCESS(x) (x)
#else
    #define DUMMY_ACCESS(x)
#endif
/*============================================================================*/
/*                       CONSTANTS DECLARATIONS                               */
/*============================================================================*/
#ifdef SUPPORT_3D_FP
    #define VBLANK_1920x1080p_2530HZ  45
    #define VBLANK_MIN_1920x1080p_2530HZ  VBLANK_1920x1080p_2530HZ - 3
    #define VBLANK_MAX_1920x1080p_2530HZ  VBLANK_1920x1080p_2530HZ + 3
    #define VBLANK_F1_1920x1080i_5060HZ  22
    #define VBLANK_F1_MIN_1920x1080i_5060HZ  VBLANK_F1_1920x1080i_5060HZ - 3
    #define VBLANK_F1_MAX_1920x1080i_5060HZ  VBLANK_F1_1920x1080i_5060HZ + 3
#endif

/*============================================================================*/
/*                         FUNCTION PROTOTYPES                                */
/*============================================================================*/

/* prototypes of internal functions */
#ifndef TMFL_NO_RTOS
    static void CommandTaskUnit0(void);
#endif

static void dlHdmiRxManageCallbackInstance(tmbslHdmiRxIRQSource_t irqSource, tmInstance_t instance, UInt8 status);
static void dlHdmiRxManageCallback0(tmbslHdmiRxIRQSource_t irqSource, UInt8 status);
static void dlHdmiRxRecoverChannelAssignment(tmInstance_t instance, UInt8 audInfoByte1);
static void dlHdmiRxSetState(tmInstance_t instance, tmdlHdmiRxDriverState_t state);
static tmdlHdmiRxDriverState_t dlHdmiRxGetState(tmInstance_t instance);
static tmdlHdmiRxEventStatus_t dlHdmiRxGetEventStatus(tmInstance_t instance, tmdlHdmiRxEvent_t event);
static tmdlHdmiRxInput_t dlHdmiRxGetVideoInput(tmInstance_t instance);
static tmErrorCode_t dlHdmiRxCheckResolutionSyncTimings(tmInstance_t instance, tmdlHdmiRxResolutionID_t *pResolutionID);
static tmErrorCode_t dlHdmiRxGetInterruptStatus(tmInstance_t instance, tmdlHdmiRxEvent_t statusEvent);
static tmErrorCode_t dlHdmiRxFindHdcpSeed(tmInstance_t instance);
static tmErrorCode_t dlHdmiRxConfigVideoPort(tmInstance_t  instance, tmdlVPBitsConfig *VideoPortConfiguration, UInt8 *num_entries);

#ifdef HDMI_DEBUG
    //#define x
    //#define x	; Write("dl-"); PRINTIF(999,__LINE__);
#else
    //#define x	;
#endif

/*============================================================================*/
/*                       VARIABLES DECLARATIONS                               */
/*============================================================================*/

static unitConfig_t unitTable[MAX_UNITS] = {
    {   False,							/* Bool opened */
        False,							/* Bool hdcpEnable */
        False, 							/* Bool internalEdid */
        TMDL_HDMIRX_HDCPMODE_AUTO, 		/* tmdlHdmiRxHdcpMode_t hdcpMode */
        False, 							/* Bool repeaterEnable */
        TMDL_HDMIRX_DEVICE_UNKNOWN,		/* tmdlHdmiRxDeviceVersion_t deviceVersion */
        (tmdlHdmiRxIWTaskHandle_t)0, 	/* tmdlHdmiRxIWTaskHandle_t taskHandle */
        (tmdlHdmiRxIWQueueHandle_t)0, 	/* tmdlHdmiRxIWQueueHandle_t queueHandle */
        STATE_NOT_INITIALIZED, 			/* tmdlHdmiRxDriverState_t state */
        tmPowerOn, 						/* tmPowerState_t powerState */
        Null, 							/* ptmdlHdmiRxActivityCallback_t pAnalogActivityCallback */
        Null, 							/* ptmdlHdmiRxActivityCallback_t pDigitalActivityCallback */
        Null, 							/* ptmdlHdmiRxDataCallback_t pDataCallback */
        Null,							/* ptmdlHdmiRxInfoCallback_t pInfoCallback */
#ifdef TMFL_HDMI_OUT
        Null,							/* ptmdlHdmiRxHdmioutCallback_t  pHdmioutCallback */
        STATE_UNPLUGGED,				/* tmdlHdmiRxEdidDriverState_t   EdidState */
        Null,							/* UInt8 *pEdidBuffer */
        0								/* UInt32 edidBufferSize */
#endif
    }
};

#ifndef TMFL_NO_RTOS
static tmdlHdmiRxIWFuncPtr_t commandTaskTable[MAX_UNITS] = {
    CommandTaskUnit0
};
#endif

static ptmbslHdmiRxCallbackFunc_t callbackFuncTable[MAX_UNITS] = {
    dlHdmiRxManageCallback0
};

static tmdlHdmiRxDriverConfigTable_t gtmdlHdmiRxDriverConfigTable[MAX_UNITS];

/* One semaphore per instance to protect the I2C access */
static tmdlHdmiRxIWSemHandle_t gtmdlHdmiRxItSemaphore[MAX_UNITS];


/* Instance status */

/* Channel assignment */
static tmdlHdmiRxChannelAssignmentInfo_t channelAssignmentList = { 0,
                                                                   TMDL_HDMIRX_REFER_TO_AUD_INFOFRAME
                                                                 };

/* Video info */
static tmdlHdmiRxVideoInfo_t videoInfoList = { TMDL_HDMIRX_INPUT_HDMI_A, TMDL_HDMIRX_OUTPUTFORMAT_444 ,
                                               TMDL_HDMIRX_VIDEORES_UNKNOWN, TMDL_HDMIRX_OUTCLKMODE_SINGLE_EDGE,
                                               False, False
                                             };
/* Event state */
static tmdlHdmiRxEventState_t eventStateList[EVENT_NB] = {  {TMDL_HDMIRX_ACTIVITY_DETECTED,            TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_ACTIVITY_LOST,                TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_ISRC1_PACKET_RECEIVED,        TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_ISRC2_PACKET_RECEIVED,        TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_ACP_PACKET_RECEIVED,          TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_AVI_INFOFRAME_RECEIVED,       TMDL_HDMIRX_EVENT_DISABLED},
#ifdef TMFL_TDA19972_FAMILY
    {TMDL_HDMIRX_VS_INFOFRAME_RECEIVED,        TMDL_HDMIRX_EVENT_DISABLED},
#endif
    {TMDL_HDMIRX_SPD_INFOFRAME_RECEIVED,       TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_AUD_INFOFRAME_RECEIVED,       TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_MPS_INFOFRAME_RECEIVED,       TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_INPUT_LOCKED,                 TMDL_HDMIRX_EVENT_ENABLED},
    {TMDL_HDMIRX_ENCRYPTION_DETECTED,          TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_NO_ENCRYPTION_DETECTED,       TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_EESS_HDCP_DETECTED,           TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_OESS_HDCP_DETECTED,           TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_REPEATER,                     TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_GBD_PACKET_RECEIVED,          TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_DEEP_COLOR_MODE_24BITS,       TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_DEEP_COLOR_MODE_30BITS,       TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_DEEP_COLOR_MODE_36BITS,       TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_DEEP_COLOR_MODE_48BITS,       TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_AUDIO_SAMPLE_PACKET_DETECTED, TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_AUDIO_HBR_PACKET_DETECTED,    TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_AUDIO_OBA_PACKET_DETECTED,    TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_AUDIO_DST_PACKET_DETECTED,    TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_HDMI_DETECTED,                TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_DVI_DETECTED,                 TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_AVI_AVMUTE_ACTIVE,            TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_AVI_AVMUTE_INACTIVE,          TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_AUDIO_SAMPLE_FREQ_CHANGED,    TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_AUDIO_UNMUTED,                TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_AUDIO_AUDIO_LAYOUT_0,         TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_AUDIO_AUDIO_LAYOUT_1,         TMDL_HDMIRX_EVENT_DISABLED},
#ifdef TMFL_TDA19972_FAMILY
    {TMDL_HDMIRX_AUDIO_CHANNEL_STATUS,         TMDL_HDMIRX_EVENT_DISABLED},
#endif
#ifdef TMFL_HDMI_OUT
    {TMDL_HDMIRX_HDMIOUT_HPD_ACTIVE,         	TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_HDMIOUT_HPD_INACTIVE,         TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_HDMIOUT_RX_DEVICE_ACTIVE,		TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_HDMIOUT_RX_DEVICE_INACTIVE,	TMDL_HDMIRX_EVENT_DISABLED},
    {TMDL_HDMIRX_HDMIOUT_EDID_RECEIVED,		TMDL_HDMIRX_EVENT_DISABLED},
#endif
};

/* Mute and video port */
static tmdlHdmiRxMuteAndVideoPort_t muteAndVideoPortList = { False,
#ifdef TMFL_TDA19972_FAMILY
                                                             False,
#endif
                                                             TMDL_HDMIRX_AUDIOFORMAT_I2S32,
                                                             False,
                                                             TMDL_HDMIRX_VPRESOLUTION_10_BITS,
                                                             TMDL_HDMIRX_OUTPUTSIGNAL_U,
                                                             TMDL_HDMIRX_OUTPUTSIGNAL_Y,
                                                             TMDL_HDMIRX_OUTPUTSIGNAL_V,
#ifdef TMFL_TDA19972_FAMILY
{   {TMDL_HDMI_RX_VP24_G4_3_0,TMDL_HDMI_RX_B_CB_7_4},
    {TMDL_HDMI_RX_VP24_G4_7_4,TMDL_HDMI_RX_B_CB_11_8},
    {TMDL_HDMI_RX_VP24_G4_11_8,TMDL_HDMI_RX_G_Y_7_4},
    {TMDL_HDMI_RX_VP24_G4_15_12,TMDL_HDMI_RX_G_Y_11_8},
    {TMDL_HDMI_RX_VP24_G4_19_16,TMDL_HDMI_RX_R_CR_CBCR_YCBCR_7_4},
    {TMDL_HDMI_RX_VP24_G4_23_20,TMDL_HDMI_RX_R_CR_CBCR_YCBCR_11_8},
    {0,0},
    {0,0},
    {0,0}
},
#endif
TMDL_HDMIRX_AUDIOSYSCLK_256FS,
(UInt8) AVMUTE_COUNT_DEF_VAL
                                                           };


static instanceStatus_t instanceStatusInfo[MAX_UNITS] = { {
        &channelAssignmentList,
        &videoInfoList,
        eventStateList,
        &muteAndVideoPortList,
        0,
        0,
        BSLHDMIRX_NACK_HDCP,
        False
    }
};

/*============================================================================*/
/*                              FUNCTIONS                                     */
/*============================================================================*/



/**
    \brief Get the software version of the driver.

    \param pSWVersion Pointer to the version structure

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

******************************************************************************/
tmErrorCode_t tmdlHdmiRxGetSWVersion
(
    tmSWVersion_t  *pSWVersion
)
{
    /* check that input pointer is not NULL */
    RETIF(pSWVersion == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)

    /* copy SW version */
    pSWVersion->compatibilityNr = VERSION_COMPATIBILITY;
    pSWVersion->majorVersionNr = VERSION_MAJOR;
    pSWVersion->minorVersionNr = VERSION_MINOR;

    return(TM_OK);
}

/******************************************************************************
    \brief Get the number of available HDMI receivers devices in the system.
           A unit directly represents a physical device.

    \param pUnitCount Pointer to the number of available units.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

******************************************************************************/
tmErrorCode_t tmdlHdmiRxGetNumberOfUnits
(
    UInt32  *pUnitCount
)
{
    /* check that input pointer is not NULL */
    RETIF(pUnitCount == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)

    /* copy the maximum number of units */
    *pUnitCount = MAX_UNITS;

    return(TM_OK);
}

/******************************************************************************
    \brief Get the capabilities of unit 0. Capabilities are stored into a
           dedicated structure and are directly read from the HW device.

    \param pCapabilities Pointer to the capabilities structure.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range

******************************************************************************/
tmErrorCode_t tmdlHdmiRxGetCapabilities
(
    tmdlHdmiRxCapabilities_t *pCapabilities
)
{
    /* directly call GetCapabilitiesM function for unit 0 and return the result */
    return(tmdlHdmiRxGetCapabilitiesM((tmUnitSelect_t)0, pCapabilities));

}

/******************************************************************************
    \brief Get the capabilities of a specific unit. Capabilities are stored
           into a dedicated structure and are directly read from the HW
           device.

    \param pCapabilities Pointer to the capabilities structure.
    \param unit          Unit to be probed.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range

******************************************************************************/
tmErrorCode_t tmdlHdmiRxGetCapabilitiesM
(
    tmUnitSelect_t            unit,
    tmdlHdmiRxCapabilities_t *pCapabilities
)
{
    tmErrorCode_t             errCode;
    tmbslHdmiRxCapabilities_t bslCapabilities;


    /* check if unit number is in range */
    RETIF((unit < 0) || (unit >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_UNIT_NUMBER)

    /* check if pointer is Null */
    RETIF(pCapabilities == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)

    /* Retrieve the capabilities from the BSL layer */
    errCode = gtmdlHdmiRxDriverConfigTable[unit].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetCapabilities(unit,&bslCapabilities);
    RETIF(errCode != TM_OK, errCode)

    /* Store the capabilities in the global variable */
    switch (bslCapabilities.deviceVersion)
    {
    case BSLHDMIRX_TDA19978:
        pCapabilities->deviceVersion= TMDL_HDMIRX_DEVICE_TDA19978;
        break;

    case BSLHDMIRX_TDA19974:
        pCapabilities->deviceVersion= TMDL_HDMIRX_DEVICE_TDA19974;
        break;

    case BSLHDMIRX_TDA19977:
        pCapabilities->deviceVersion= TMDL_HDMIRX_DEVICE_TDA19977;
        break;

    case BSLHDMIRX_TDA19979:
        pCapabilities->deviceVersion= TMDL_HDMIRX_DEVICE_TDA19979;
        break;

    case BSLHDMIRX_TDA19971:
        pCapabilities->deviceVersion= TMDL_HDMIRX_DEVICE_TDA19971;
        break;

    case BSLHDMIRX_TDA19972:
        pCapabilities->deviceVersion= TMDL_HDMIRX_DEVICE_TDA19972;
        break;

    case BSLHDMIRX_TDA19973_SOC_IN:
        pCapabilities->deviceVersion= TMDL_HDMIRX_DEVICE_TDA19973_SOC_IN;
        break;

    case BSLHDMIRX_TDA19973_CONNECTOR_IN:
        pCapabilities->deviceVersion= TMDL_HDMIRX_DEVICE_TDA19973_CONNECTOR_IN;
        break;

    case BSLHDMIRX_TDA19971N2:
        pCapabilities->deviceVersion= TMDL_HDMIRX_DEVICE_TDA19971N2;
        break;

    case BSLHDMIRX_TDA19973N2_SOC_IN:
        pCapabilities->deviceVersion= TMDL_HDMIRX_DEVICE_TDA19973N2_SOC_IN;
        break;

    case BSLHDMIRX_TDA19973N2_CONNECTOR_IN:
        pCapabilities->deviceVersion= TMDL_HDMIRX_DEVICE_TDA19973N2_CONNECTOR_IN;
        break;

    case BSLHDMIRX_TDA_UNKNOWN:
        pCapabilities->deviceVersion= TMDL_HDMIRX_DEVICE_UNKNOWN;
        break;

    default:
        pCapabilities->deviceVersion= TMDL_HDMIRX_DEVICE_UNKNOWN;
        return TMDL_ERR_DLHDMIRX_BAD_PARAMETER;
    }

    pCapabilities->repeater = bslCapabilities.repeater;
    pCapabilities->fastI2C = bslCapabilities.fastI2C;

    switch (bslCapabilities.hdmiVersion)
    {
    case BSLHDMIRX_HDMI_VERSION_UNKNOWN:   /**< Unknown   */
        pCapabilities->hdmiVersion = TMDL_HDMIRX_HDMI_VERSION_UNKNOWN;
        break;

    case BSLHDMIRX_HDMI_VERSION_1_0:       /**< HDMI 1.0  */
        pCapabilities->hdmiVersion = TMDL_HDMIRX_HDMI_VERSION_1_0;
        break;

    case BSLHDMIRX_HDMI_VERSION_1_1:       /**< HDMI 1.1  */
        pCapabilities->hdmiVersion = TMDL_HDMIRX_HDMI_VERSION_1_1;
        break;

    case BSLHDMIRX_HDMI_VERSION_1_2:       /**< HDMI 1.2  */
        pCapabilities->hdmiVersion = TMDL_HDMIRX_HDMI_VERSION_1_2;
        break;

    case BSLHDMIRX_HDMI_VERSION_1_2a:      /**< HDMI 1.2a */
        pCapabilities->hdmiVersion = TMDL_HDMIRX_HDMI_VERSION_1_2a;
        break;

    case BSLHDMIRX_HDMI_VERSION_1_3:        /**< HDMI 1.3  */
        pCapabilities->hdmiVersion = TMDL_HDMIRX_HDMI_VERSION_1_3;
        break;

    case BSLHDMIRX_HDMI_VERSION_1_3a:        /**< HDMI 1.3a  */
        pCapabilities->hdmiVersion = TMDL_HDMIRX_HDMI_VERSION_1_3a;
        break;

    case BSLHDMIRX_HDMI_VERSION_1_4:        /**< HDMI 1.4  */
        pCapabilities->hdmiVersion = TMDL_HDMIRX_HDMI_VERSION_1_4;
        break;

    case BSLHDMIRX_HDMI_VERSION_1_4a:        /**< HDMI 1.4a  */
        pCapabilities->hdmiVersion = TMDL_HDMIRX_HDMI_VERSION_1_4a;
        break;

    default:
        pCapabilities->hdmiVersion = TMDL_HDMIRX_HDMI_VERSION_UNKNOWN;
        return TMDL_ERR_DLHDMIRX_BAD_PARAMETER;
    }

    pCapabilities->audioPacket.HBR = bslCapabilities.audioPacket.HBR;
    pCapabilities->audioPacket.DST = bslCapabilities.audioPacket.DST;
    pCapabilities->audioPacket.oneBitAudio = bslCapabilities.audioPacket.oneBitAudio;

    pCapabilities->fastReauth = bslCapabilities.fastReauth;
    pCapabilities->analogInput = bslCapabilities.analogInput;

    pCapabilities->hdmi11Features = bslCapabilities.hdmi11Features;
    pCapabilities->internalEdid   = bslCapabilities.internalEdid;

    return(TM_OK);
}

/******************************************************************************
    \brief Open unit 0 of HdmiRx driver and provides the instance number to
           the caller. Note that one unit of HdmiRx represents one physical
           HDMI receiver and that only one instance per unit can be opened.

    \param pInstance Pointer to the variable that will receive the instance
                     identifier.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMDL_ERR_DLHDMIRX_RESOURCE_OWNED: the resource is already in use
            - TMDL_ERR_DLHDMIRX_INIT_FAILED: the unit instance is already
              initialised or something wrong happened at lower level.
            - TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent
            - TMDL_ERR_DLHDMIRX_NO_RESOURCES: the resource is not available
            - TMDL_ERR_DLHDMIRX_INVALID_STATE: the state is invalid for
              the function

******************************************************************************/
tmErrorCode_t tmdlHdmiRxOpen
(
    tmInstance_t   *pInstance
)
{
    /* directly call OpenM function for unit 0 and return the result */
    return(tmdlHdmiRxOpenM(pInstance, (tmUnitSelect_t)0));
}

/******************************************************************************
    \brief Open a specific unit of HdmiRx driver and provides the instance
           number to the caller. Note that one unit of HdmiRx represents one
           physical HDMI receiver and that only one instance per unit can be
           opened. This function switches driver's state machine to
           "initialized" state.

    \param pInstance Pointer to the structure that will receive the instance
                     identifier.
    \param unit      Unit number to be opened.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMDL_ERR_DLHDMIRX_RESOURCE_OWNED: the resource is already in use
            - TMDL_ERR_DLHDMIRX_INIT_FAILED: the unit instance is already
              initialised or something wrong happened at lower level.
            - TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent
            - TMDL_ERR_DLHDMIRX_NO_RESOURCES: the resource is not available
            - TMDL_ERR_DLHDMIRX_INVALID_STATE: the state is invalid for
              the function

******************************************************************************/
tmErrorCode_t tmdlHdmiRxOpenM
(
    tmInstance_t   *pInstance,
    tmUnitSelect_t  unit
)
{
    tmErrorCode_t                   errCode;
    tmbslHdmiRxVersion_t            deviceVersion;

    /* check if unit number is in range */
    RETIF((unit < 0) || (unit >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_UNIT_NUMBER)

    /* check if Instance pointer is Null */
    RETIF(pInstance == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)

    /* Create the semaphore to protect variables modified under interruption */
    errCode = tmdlHdmiRxIWSemaphoreCreate(&gtmdlHdmiRxItSemaphore[unit]);
    RETIF(errCode != TM_OK, errCode)

    /* Take the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[unit]);
    RETIF(errCode != TM_OK, errCode)

    /* check if unit is already instantiated */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[unit], unitTable[unit].opened == True, TMDL_ERR_DLHDMIRX_RESOURCE_OWNED)

    /* Check the state */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[unit], dlHdmiRxGetState(unit) != STATE_NOT_INITIALIZED, TMDL_ERR_DLHDMIRX_INVALID_STATE)
    /* instantiate unit and return corresponding instance number */
    /* Since HW unit are only instanciable once, instance = unit */
    unitTable[unit].opened                   = True;
    unitTable[unit].hdcpEnable               = False;
    unitTable[unit].hdcpMode                 = TMDL_HDMIRX_HDCPMODE_AUTO;
    unitTable[unit].repeaterEnable           = False;
    unitTable[unit].deviceVersion            = TMDL_HDMIRX_DEVICE_UNKNOWN;
    unitTable[unit].pAnalogActivityCallback  = Null;
    unitTable[unit].pDigitalActivityCallback = Null;
    unitTable[unit].pDataCallback            = Null;
    unitTable[unit].pInfoCallback            = Null;
#ifdef TMFL_HDMI_OUT
    unitTable[unit].pHdmioutCallback         = Null;
    unitTable[unit].EdidState            	 = STATE_UNPLUGGED;
    unitTable[unit].pEdidBuffer            	 = Null;
    unitTable[unit].edidBufferSize           = 0;
#endif
    
#ifdef TMFL_TDA19972_FAMILY
    /* flag that video port has not been configured */
    instanceStatusInfo[unit].pMuteAndVideoPort->videoPortConfigured = False;
#endif
    
    /* Recover the configuration of the device library */
    errCode = tmdlHdmiRxCfgGetConfig(unit, &gtmdlHdmiRxDriverConfigTable[unit]);
    
    RETIF_SEM(gtmdlHdmiRxItSemaphore[unit], errCode != TM_OK, TMDL_ERR_DLHDMIRX_INIT_FAILED)
    
#ifndef TMFL_NO_RTOS
    /* create message queue associated to this instance/unit */
    errCode = tmdlHdmiRxIWQueueCreate(gtmdlHdmiRxDriverConfigTable[unit].commandTaskQueueSize, &(unitTable[unit].queueHandle));
    RETIF_SEM(gtmdlHdmiRxItSemaphore[unit], errCode != TM_OK, TMDL_ERR_DLHDMIRX_NO_RESOURCES)

    /* create the task associated to this instance/unit */
    errCode = tmdlHdmiRxIWTaskCreate(commandTaskTable[unit],
                                     gtmdlHdmiRxDriverConfigTable[unit].commandTaskPriority,
                                     gtmdlHdmiRxDriverConfigTable[unit].commandTaskStackSize,
                                     &(unitTable[unit].taskHandle));
    RETIF_SEM(gtmdlHdmiRxItSemaphore[unit], errCode != TM_OK, TMDL_ERR_DLHDMIRX_NO_RESOURCES)

    errCode = tmdlHdmiRxIWTaskStart(unitTable[unit].taskHandle);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[unit], errCode != TM_OK, TMDL_ERR_DLHDMIRX_NO_RESOURCES)
#endif

    *pInstance = (tmInstance_t)unit;

    





    //sprintf(Msg, "unit=%d",gtmdlHdmiRxDriverConfigTable[unit].i2cAddress);
    //sprintf(Msg, "unit=%d",unit);
    //WriteLn(Msg);

    /* Init the BSL */
    errCode = gtmdlHdmiRxDriverConfigTable[unit].ptmbslHdmiRxFunctionsList->tmbslHdmiRxInit(
                  unit,
                  gtmdlHdmiRxDriverConfigTable[unit].i2cAddress,
#ifdef TMFL_TDA19972_FAMILY
                  gtmdlHdmiRxDriverConfigTable[unit].CECi2cAddress,
#endif
                  gtmdlHdmiRxDriverConfigTable[unit].i2cWriteFunction,
                  gtmdlHdmiRxDriverConfigTable[unit].i2cReadFunction,
                  callbackFuncTable[unit],
                  (ptmbslHdmiRxSysFuncTimer_t) tmdlHdmiRxIWWait);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[unit], errCode != TM_OK, errCode)
    



    errCode = gtmdlHdmiRxDriverConfigTable[unit].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetChipInfo(unit,
              &deviceVersion);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[unit], errCode !=TM_OK, errCode)
    

    /* Store the capabilities in the global variable */
    switch (deviceVersion)
    {
    case BSLHDMIRX_TDA19978:
       
        gtmdlHdmiRxDriverConfigTable[unit].pCapabilitiesList->deviceVersion = TMDL_HDMIRX_DEVICE_TDA19978;
        break;

    case BSLHDMIRX_TDA19974:
        
        gtmdlHdmiRxDriverConfigTable[unit].pCapabilitiesList->deviceVersion = TMDL_HDMIRX_DEVICE_TDA19974;
        break;

    case BSLHDMIRX_TDA19977:
        
        gtmdlHdmiRxDriverConfigTable[unit].pCapabilitiesList->deviceVersion = TMDL_HDMIRX_DEVICE_TDA19977;
        break;

    case BSLHDMIRX_TDA19979:
        
        gtmdlHdmiRxDriverConfigTable[unit].pCapabilitiesList->deviceVersion = TMDL_HDMIRX_DEVICE_TDA19979;
        break;

    case BSLHDMIRX_TDA19971:
        
        gtmdlHdmiRxDriverConfigTable[unit].pCapabilitiesList->deviceVersion = TMDL_HDMIRX_DEVICE_TDA19971;
        break;

    case BSLHDMIRX_TDA19972:
        
        gtmdlHdmiRxDriverConfigTable[unit].pCapabilitiesList->deviceVersion = TMDL_HDMIRX_DEVICE_TDA19972;
        break;

    case BSLHDMIRX_TDA19973_SOC_IN:
        
        gtmdlHdmiRxDriverConfigTable[unit].pCapabilitiesList->deviceVersion = TMDL_HDMIRX_DEVICE_TDA19973_SOC_IN;
        break;

    case BSLHDMIRX_TDA19973_CONNECTOR_IN:
        
        gtmdlHdmiRxDriverConfigTable[unit].pCapabilitiesList->deviceVersion = TMDL_HDMIRX_DEVICE_TDA19973_CONNECTOR_IN;
        break;

    case BSLHDMIRX_TDA19971N2:
        
        gtmdlHdmiRxDriverConfigTable[unit].pCapabilitiesList->deviceVersion = TMDL_HDMIRX_DEVICE_TDA19971N2;
        break;

    case BSLHDMIRX_TDA19973N2_SOC_IN:
        
        gtmdlHdmiRxDriverConfigTable[unit].pCapabilitiesList->deviceVersion = TMDL_HDMIRX_DEVICE_TDA19973N2_SOC_IN;
        break;

    case BSLHDMIRX_TDA19973N2_CONNECTOR_IN:
        
        gtmdlHdmiRxDriverConfigTable[unit].pCapabilitiesList->deviceVersion = TMDL_HDMIRX_DEVICE_TDA19973N2_CONNECTOR_IN;
        break;

    case BSLHDMIRX_TDA_UNKNOWN:
        
        gtmdlHdmiRxDriverConfigTable[unit].pCapabilitiesList->deviceVersion = TMDL_HDMIRX_DEVICE_UNKNOWN;
        break;

    default:
        
        gtmdlHdmiRxDriverConfigTable[unit].pCapabilitiesList->deviceVersion = TMDL_HDMIRX_DEVICE_UNKNOWN;
        return TMDL_ERR_DLHDMIRX_BAD_PARAMETER;
    }
    
    // Disable HDCP
    /* The MCLK signal must be available  */
    errCode = gtmdlHdmiRxDriverConfigTable[unit].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureHDMIClock(unit,
              BSLHDMIRX_HDMICLOCK_MCLK);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[unit], errCode != TM_OK, errCode)
    
    instanceStatusInfo[unit].nackHdcp = BSLHDMIRX_NACK_HDCP;


    
    /* No HDCP acknowledge when HDCP is disabled */
    /* and reset SUS to force format detection */
    errCode = gtmdlHdmiRxDriverConfigTable[unit].ptmbslHdmiRxFunctionsList->tmbslHdmiRxHDMISoftReset(unit,
              instanceStatusInfo[unit].nackHdcp,
              BSLHDMIRX_DONTRESET_AI,
              BSLHDMIRX_DONTRESET_INFOFRAME,
              BSLHDMIRX_DONTRESET_AUDIO_FIFO,
              BSLHDMIRX_DONTRESET_GAMUT,
              BSLHDMIRX_DONTRESET_FIFOCTRL,
              BSLHDMIRX_RESET_SUS,
              BSLHDMIRX_DONTRESET_DC);

    RETIF_SEM(gtmdlHdmiRxItSemaphore[unit], errCode != TM_OK, errCode)
    



//    printf("SR3\n");

    errCode = gtmdlHdmiRxDriverConfigTable[unit].ptmbslHdmiRxFunctionsList->tmbslHdmiRxHDMISoftReset(unit,
              instanceStatusInfo[unit].nackHdcp,
              BSLHDMIRX_DONTRESET_AI,
              BSLHDMIRX_DONTRESET_INFOFRAME,
              BSLHDMIRX_DONTRESET_AUDIO_FIFO,
              BSLHDMIRX_DONTRESET_GAMUT,
              BSLHDMIRX_DONTRESET_FIFOCTRL,
              BSLHDMIRX_DONTRESET_SUS,
              BSLHDMIRX_DONTRESET_DC);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[unit], errCode != TM_OK, errCode)
    

    
    /* Get key description seed in function of seed table if provide in cfg*/
    errCode = dlHdmiRxFindHdcpSeed(*pInstance);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[unit], errCode != TM_OK, errCode)

    //* Encrypted, disable HDCP */
    errCode = gtmdlHdmiRxDriverConfigTable[unit].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureHDCP(unit,
              BSLHDMIRX_HDCP_DECRYPTKEY_ON,
              BSLHDMIRX_HDCP_DISABLE,
              TMDLHDMIRX_DDC_ADDR,
              gtmdlHdmiRxDriverConfigTable[unit].keyDescriptionSeed);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[unit], errCode != TM_OK, errCode)
    

    RETIF_SEM(gtmdlHdmiRxItSemaphore[unit], errCode != TM_OK, errCode)
    
    /* Set the state machine */
    dlHdmiRxSetState(unit, STATE_INITIALIZED);
    


    /* Release the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[unit]);
    RETIF(errCode != TM_OK, errCode)
    
    return(TM_OK);
}

/******************************************************************************
    \brief Close an instance of HdmiRx driver.

    \param instance Instance identifier.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_RESOURCE_NOT_OWNED: the caller does not own
              the resource

******************************************************************************/
tmErrorCode_t tmdlHdmiRxClose
(
    tmInstance_t   instance
)
{
    tmErrorCode_t errCode;

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* Take the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    /* check if unit corresponding to instance is opened */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], unitTable[instance].opened == False, TMDL_ERR_DLHDMIRX_RESOURCE_NOT_OWNED)

    /* close instance */
    unitTable[instance].opened = False;

    /* Reset an instance of an HDMI receiver */
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxDeInit(instance);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)

    /* Set the state machine */
    dlHdmiRxSetState(instance, STATE_NOT_INITIALIZED);

#ifndef TMFL_NO_RTOS
    /* destroy resources allocated for this instance/unit */
    tmdlHdmiRxIWTaskDestroy(unitTable[instance].taskHandle);
    tmdlHdmiRxIWQueueDestroy(unitTable[instance].queueHandle);
#endif

    /* *** TODO : Mute audio & video outputs */

    /* Release the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    /* Destroy the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreDestroy(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    return(TM_OK);
}

/******************************************************************************
    \brief Set the power state of an instance of the HDMI receiver. ON state
           correspond to a fully supplied, up and running device. Other modes
           correspond to the powerdown state of the device.

    \param instance   Instance identifier.
    \param powerState Power state to set.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range

******************************************************************************/
tmErrorCode_t tmdlHdmiRxSetPowerState
(
    tmInstance_t   instance,
    tmPowerState_t powerState
)
{
    tmErrorCode_t errCode;

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* Take the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    /* check if unit corresponding to instance is opened */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], unitTable[instance].opened == False, TMDL_ERR_DLHDMIRX_RESOURCE_NOT_OWNED)

    /* Set the power state */
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetPowerState(
                  instance,
                  powerState);

    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance],
              ((errCode != TM_OK) && (errCode != TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED)),
              errCode)

    unitTable[instance].powerState = powerState;

    /* Release the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    return(TM_OK);
}

/******************************************************************************
    \brief Get the power state of an instance of the HDMI receiver. ON state
           correspond to a fully supplied, up and running device. Other modes
           correspond to the powerdown state of the device.

    \param instance    Instance identifier.
    \param pPowerState Pointer to the power state.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

******************************************************************************/
tmErrorCode_t tmdlHdmiRxGetPowerState
(
    tmInstance_t    instance,
    tmPowerState_t *pPowerState
)
{
    tmErrorCode_t errCode;

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* check that input pointer is not NULL */
    RETIF(pPowerState == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)

    /* Take the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    /* check if unit corresponding to instance is opened */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], unitTable[instance].opened == False, TMDL_ERR_DLHDMIRX_RESOURCE_NOT_OWNED)

    *pPowerState = unitTable[instance].powerState;

    /* Release the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    return(TM_OK);
}

/******************************************************************************
    \brief Set the configuration of instance attributes when the instance is
           already opened.

    \param instance    Instance identifier.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range

******************************************************************************/
tmErrorCode_t tmdlHdmiRxInstanceConfig
(
    tmInstance_t    instance,
    tmdlHdmiRxInstanceCfgInfo_t *pCfgInfo
)
{

    tmErrorCode_t                   errCode;
    tmbslHdmiRxCapsRepeater_t       repeater;
    tmbslHdmiRxCapsFastI2c_t        fastI2c;
    tmbslHdmiRxCapsHdmi11_t         hdmi11;
    tmbslHdmiRxCapsFastReauth_t     fastReauth;

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* check that input pointer is not NULL */
    RETIF(pCfgInfo == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)

    /* Take the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    /* check if unit corresponding to instance is opened */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], unitTable[instance].opened == False, TMDL_ERR_DLHDMIRX_RESOURCE_NOT_OWNED)


    unitTable[instance].repeaterEnable = pCfgInfo->hdcpRepeaterEnable;

    if(pCfgInfo->hdcpRepeaterEnable)
    {
        repeater = BSLHDMIRX_RXCAPS_REPEATER_ON;
    }
    else
    {
        repeater = BSLHDMIRX_RXCAPS_REPEATER_OFF;
    }

    if(gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->fastI2C)
    {
        fastI2c = BSLHDMIRX_RXCAPS_FAST_I2C_ON;
    }
    else
    {
        fastI2c = BSLHDMIRX_RXCAPS_FAST_I2C_OFF;
    }

    if(gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->hdmi11Features)
    {
        hdmi11 = BSLHDMIRX_RXCAPS_HDMI_1_1_ON;
    }
    else
    {
        hdmi11 = BSLHDMIRX_RXCAPS_HDMI_1_1_OFF;
    }

    if(gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->fastReauth)
    {
        fastReauth = BSLHDMIRX_RXCAPS_FAST_REAUTH_ON;
    }
    else
    {
        fastReauth = BSLHDMIRX_RXCAPS_FAST_REAUTH_OFF;
    }

    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureReceiverCaps(
                  instance,
                  repeater,
                  fastI2c,
                  hdmi11,
                  fastReauth);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)

    /* Release the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)


    return(TM_OK);
}

/******************************************************************************
    \brief Setup the instance with its configuration parameters. This function
           allows basic instance configuration like enabling HDCP, choosing
           HDCP encryption mode or enabling HDCP repeater mode.
           This function is synchronous.
           This function is not ISR friendly.
           This function also calls the "tmdlHdmiRxSelectInput" API and configures
           the HDMI receiver in digital automatic mode for input selection.

    \param instance   Instance identifier.
    \param pSetupInfo Pointer to the structure containing all setup parameters

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_RESOURCE_NOT_OWNED: the caller does not own
              the resource

******************************************************************************/
tmErrorCode_t tmdlHdmiRxInstanceSetup
(
    tmInstance_t                   instance,
    tmdlHdmiRxInstanceSetupInfo_t *pSetupInfo
)
{
    tmErrorCode_t                   errCode;
    UInt8                           keyIndex, delockDelay;
    tmbslHdmiRxHdcpMode_t           hdcpMode;
    tmbslHdmiRxCapsRepeater_t       repeater;
    tmbslHdmiRxCapsFastI2c_t        fastI2c;
    tmbslHdmiRxCapsHdmi11_t         hdmi11;
    tmbslHdmiRxCapsFastReauth_t     fastReauth;

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* check that input pointer is not NULL */
    RETIF(pSetupInfo == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)

    /* Take the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    /* check if unit corresponding to instance is opened */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], unitTable[instance].opened == False, TMDL_ERR_DLHDMIRX_RESOURCE_NOT_OWNED)

    /* Check the state */
    //RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], dlHdmiRxGetState(instance) != STATE_NOT_INITIALIZED, TMDL_ERR_DLHDMIRX_INVALID_STATE)

    unitTable[instance].repeaterEnable = pSetupInfo->hdcpRepeaterEnable;
#ifdef TMFL_HDMI_OUT
    unitTable[instance].pEdidBuffer = pSetupInfo->pEdidBuffer;
    unitTable[instance].edidBufferSize = pSetupInfo->edidBufferSize;
#endif

    if(pSetupInfo->hdcpRepeaterEnable)
    {
        repeater = BSLHDMIRX_RXCAPS_REPEATER_ON;
    }
    else
    {
        repeater = BSLHDMIRX_RXCAPS_REPEATER_OFF;
    }

    if(gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->fastI2C)
    {
        fastI2c = BSLHDMIRX_RXCAPS_FAST_I2C_ON;
    }
    else
    {
        fastI2c = BSLHDMIRX_RXCAPS_FAST_I2C_OFF;
    }

    if(gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->hdmi11Features)
    {
        hdmi11 = BSLHDMIRX_RXCAPS_HDMI_1_1_ON;
    }
    else
    {
        hdmi11 = BSLHDMIRX_RXCAPS_HDMI_1_1_OFF;
    }

    if(gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->fastReauth)
    {
        fastReauth = BSLHDMIRX_RXCAPS_FAST_REAUTH_ON;
    }
    else
    {
        fastReauth = BSLHDMIRX_RXCAPS_FAST_REAUTH_OFF;
    }

    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureReceiverCaps(
                  instance,
                  repeater,
                  fastI2c,
                  hdmi11,
                  fastReauth);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)

    unitTable[instance].hdcpMode = pSetupInfo->hdcpMode;

    switch(pSetupInfo->hdcpMode)
    {
    case TMDL_HDMIRX_HDCPMODE_AUTO:
        hdcpMode = BSLHDMIRX_HDCPMODE_AUTO;
        break;
    case TMDL_HDMIRX_HDCPMODE_OESS:
        hdcpMode = BSLHDMIRX_HDCPMODE_OESS;
        break;
    case TMDL_HDMIRX_HDCPMODE_EESS:
        hdcpMode = BSLHDMIRX_HDCPMODE_EESS;
        break;
    default:
        hdcpMode = BSLHDMIRX_HDCPMODE_AUTO;
        break;
    }

    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureHDMI(instance,
              BSLHDMIRX_HDMIMUTE_CTRL_PACKET,
              hdcpMode,
              BSLHDMIRX_HDMIPROTOCOL_AUTO,
              BSLHDMIRX_HDMIVSYNCPOLARITY_AUTO,
              BSLHDMIRX_HDMIVSYNCTOGGLE_NEGATIVE);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)

    /* *** Choosing internal or external EDID *** */
    if (pSetupInfo->internalEdid == True)
    {
        unitTable[instance].internalEdid = True;
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureEDID(instance,
                  BSLHDMIRX_EDID_NACK_OFF,
                  BSLHDMIRX_EDID_RESET_OFF,
                  BSLHDMIRX_EDID_PD_OFF,
                  BSLHDMIRX_EDID_PD_DDC_OFF);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance],
                  ((errCode != TM_OK) && (errCode != TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED)),
                  errCode)
    }
    else
    {
        unitTable[instance].internalEdid = False;
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureEDID(instance,
                  BSLHDMIRX_EDID_NACK_ON,
                  BSLHDMIRX_EDID_RESET_OFF,
                  BSLHDMIRX_EDID_PD_ON,
                  BSLHDMIRX_EDID_PD_DDC_ON);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance],
                  ((errCode != TM_OK) && (errCode != TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED)),
                  errCode)
    }

    /* ***  HDCP activation *** */
    unitTable[instance].hdcpEnable = pSetupInfo->hdcpEnable;
    if (unitTable[instance].hdcpEnable==True)
    {
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)

        instanceStatusInfo[instance].nackHdcp = BSLHDMIRX_NACK_HDCP;
        /* No HDCP acknowledge when HDCP is disabled */
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxHDMISoftReset(instance,
                  instanceStatusInfo[instance].nackHdcp,
                  BSLHDMIRX_DONTRESET_AI,
                  BSLHDMIRX_DONTRESET_INFOFRAME,
                  BSLHDMIRX_DONTRESET_AUDIO_FIFO,
                  BSLHDMIRX_DONTRESET_GAMUT,
                  BSLHDMIRX_DONTRESET_FIFOCTRL,
                  BSLHDMIRX_DONTRESET_SUS,
                  BSLHDMIRX_DONTRESET_DC);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)

        /* Encrypted, disable HDCP */
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureHDCP(instance,
                  BSLHDMIRX_HDCP_DECRYPTKEY_ON,
                  BSLHDMIRX_HDCP_DISABLE,
                  TMDLHDMIRX_DDC_ADDR,
                  gtmdlHdmiRxDriverConfigTable[instance].keyDescriptionSeed);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)

        /* Index first secret key */
        keyIndex = (UInt8) 0;
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetPrivateKeyIndex(instance,
                  keyIndex);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)

        /* TBC for TDA19972 */
        /* Start download process */
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureOTP(instance,
                  BSLHDMIRX_OTP_START_DOWNLOAD,
                  BSLHDMIRX_OTP_BCH_ENABLED);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)

        errCode = tmdlHdmiRxIWWait(20);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)
        /* TBC for TDA19972 */

        /* Encrypted, enable HDCP */
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureHDCP(instance,
                  BSLHDMIRX_HDCP_DECRYPTKEY_OFF,
                  BSLHDMIRX_HDCP_ENABLE,
                  TMDLHDMIRX_DDC_ADDR,
                  gtmdlHdmiRxDriverConfigTable[instance].keyDescriptionSeed);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)

#ifndef  TMFL_HDMI_REPEATER
        instanceStatusInfo[instance].nackHdcp = BSLHDMIRX_DONTNACK_HDCP;

#endif  /* TMFL_HDMI_REPEATER */
#ifdef ALLOW_DDC_ACK
        instanceStatusInfo[instance].nackHdcp = BSLHDMIRX_DONTNACK_HDCP;
#endif
        /* Enable HDCP acknowledge when HDCP is enabled */
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxHDMISoftReset(instance,
                  instanceStatusInfo[instance].nackHdcp,
                  BSLHDMIRX_DONTRESET_AI,
                  BSLHDMIRX_DONTRESET_INFOFRAME,
                  BSLHDMIRX_DONTRESET_AUDIO_FIFO,
                  BSLHDMIRX_DONTRESET_GAMUT,
                  BSLHDMIRX_DONTRESET_FIFOCTRL,
#ifdef ALLOW_DDC_ACK
                  BSLHDMIRX_DONTRESET_SUS_DDC,
#else
                  BSLHDMIRX_DONTRESET_SUS,
#endif
                  BSLHDMIRX_DONTRESET_DC);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)

        /* Configure HDCP error protection */
        delockDelay = (UInt8) 0x07;
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureHDCPErrorProtection(instance,
                  delockDelay,
                  BSLHDMIRX_HDCP_DEMEASUREMENT_1_VDP,
                  BSLHDMIRX_HDCP_DEREGENERATION_DISABLE,
                  BSLHDMIRX_HDCP_SENSIVITY_3_DEFILTER,
                  BSLHDMIRX_CH0_HDCP_DECOMPOSITION,
                  BSLHDMIRX_HDCP_3_CTLFILTERSENSIVITY,
                  BSLHDMIRX_HDCP_3_VSFILTERSENSIVITY,
                  BSLHDMIRX_HDCP_3_HSFILTERSENSIVITY);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)


    }


#ifdef TMFL_TDA19972_FAMILY
    /* reset SUS to force format detection */
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxHDMISoftReset(instance,
              instanceStatusInfo[instance].nackHdcp,
              BSLHDMIRX_DONTRESET_AI,
              BSLHDMIRX_DONTRESET_INFOFRAME,
              BSLHDMIRX_DONTRESET_AUDIO_FIFO,
              BSLHDMIRX_DONTRESET_GAMUT,
              BSLHDMIRX_DONTRESET_FIFOCTRL,
#ifdef ALLOW_DDC_ACK
              BSLHDMIRX_DONTRESET_SUS_DDC,
#else
              BSLHDMIRX_RESET_SUS,
#endif
              BSLHDMIRX_DONTRESET_DC);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)

//    printf("SR4\n");

    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxHDMISoftReset(instance,
              instanceStatusInfo[instance].nackHdcp,
              BSLHDMIRX_DONTRESET_AI,
              BSLHDMIRX_DONTRESET_INFOFRAME,
              BSLHDMIRX_DONTRESET_AUDIO_FIFO,
              BSLHDMIRX_DONTRESET_GAMUT,
              BSLHDMIRX_DONTRESET_FIFOCTRL,
#ifdef ALLOW_DDC_ACK
              BSLHDMIRX_DONTRESET_SUS_DDC,
#else
              BSLHDMIRX_DONTRESET_SUS,
#endif
              BSLHDMIRX_DONTRESET_DC);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)


#endif

    errCode= tmbslTDA1997XManualHPD(instance, BSLHDMIRX_HPD_HIGH_OTHER);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)

    /* Release the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    return(TM_OK);
}

/******************************************************************************
    \brief enable or disable HDCP.

    \param instance   Instance identifier.
    \param enable     HDCP state

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_RESOURCE_NOT_OWNED: the caller does not own
              the resource
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range

******************************************************************************/
tmErrorCode_t tmdlHdmiRxDriveHdcp /* TBC for TDA19972 */
(
    tmInstance_t                   instance,
    Bool                           enable
)
{
    tmErrorCode_t errCode;

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* Take the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    /* check if unit corresponding to instance is opened */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], unitTable[instance].opened == False, TMDL_ERR_DLHDMIRX_RESOURCE_NOT_OWNED)

    if (enable)
    {
        instanceStatusInfo[instance].nackHdcp = BSLHDMIRX_DONTNACK_HDCP;
    }
    else
    {
        instanceStatusInfo[instance].nackHdcp = BSLHDMIRX_NACK_HDCP;
    }

    /* Enable HDCP acknowledge when HDCP is enabled */
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxHDMISoftReset(instance,
              instanceStatusInfo[instance].nackHdcp,
              BSLHDMIRX_DONTRESET_AI,
              BSLHDMIRX_DONTRESET_INFOFRAME,
              BSLHDMIRX_DONTRESET_AUDIO_FIFO,
              BSLHDMIRX_DONTRESET_GAMUT,
              BSLHDMIRX_DONTRESET_FIFOCTRL,
#ifdef ALLOW_DDC_ACK
              BSLHDMIRX_DONTRESET_SUS_DDC,
#else
              BSLHDMIRX_DONTRESET_SUS,
#endif
              BSLHDMIRX_DONTRESET_DC);

    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)

    /* Release the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    return(TM_OK);
}

/******************************************************************************
    \brief Get instance setup parameters.

    \param instance   Instance identifier.
    \param pSetupInfo Pointer to the structure that will receive setup
                      parameters

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_RESOURCE_NOT_OWNED: the caller does not own
              the resource
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

******************************************************************************/
tmErrorCode_t tmdlHdmiRxGetInstanceSetup
(
    tmInstance_t                   instance,
    tmdlHdmiRxInstanceSetupInfo_t *pSetupInfo
)
{
    tmErrorCode_t errCode;

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* check that input pointer is not NULL */
    RETIF(pSetupInfo == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)

    /* Take the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    /* check if unit corresponding to instance is opened */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], unitTable[instance].opened == False, TMDL_ERR_DLHDMIRX_RESOURCE_NOT_OWNED)

    pSetupInfo->hdcpEnable         = unitTable[instance].hdcpEnable;
    pSetupInfo->hdcpMode           = unitTable[instance].hdcpMode;
    pSetupInfo->hdcpRepeaterEnable = unitTable[instance].repeaterEnable;
    pSetupInfo->internalEdid       = unitTable[instance].internalEdid;

    /* Release the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    return(TM_OK);
}

/*============================================================================*/
/**
    \brief Make device library handle an incoming interrupt. This function is
           used by application to tell the device library that the hardware
           sent an interrupt.

    \param instance   Instance identifier.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_BAD_HANDLE: the handle number is wrong
            - TMDL_ERR_DLHDMIRX_RESOURCE_NOT_OWNED: the caller does not own
              the resource
            - TMDL_ERR_DLHDMIRX_FULL: the queue is full

 ******************************************************************************/
tmErrorCode_t tmdlHdmiRxHandleInterrupt
(
    tmInstance_t    instance
)
{
    tmErrorCode_t   errCode;
#ifndef TMFL_NO_RTOS
    UInt8           message = 0;
#endif

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

#ifndef TMFL_NO_RTOS
    errCode = tmdlHdmiRxIWQueueSend(unitTable[instance].queueHandle, message);
    RETIF(errCode != TM_OK, errCode)

    /* Disable interrupts for Rx until the callbacks have been done by the command task */
    switch(instance)
    {
    case INSTANCE_0:
        tmdlHdmiRxIWDisableInterrupts(TMDL_HDMI_IW_RX_1);
        break;
    default:
        return TMDL_ERR_DLHDMIRX_BAD_INSTANCE;
    }

#else
    if( (instanceStatusInfo[instance].pVideoInfo->videoInput != TMDL_HDMIRX_INPUT_TEST_480P) &&
#ifdef TMFL_TDA19972_FAMILY
            (instanceStatusInfo[instance].pVideoInfo->videoInput != TMDL_HDMIRX_INPUT_TEST_480I) &&
            (instanceStatusInfo[instance].pVideoInfo->videoInput != TMDL_HDMIRX_INPUT_TEST_576I) &&
            (instanceStatusInfo[instance].pVideoInfo->videoInput != TMDL_HDMIRX_INPUT_RAMPTEST_480P) &&
            (instanceStatusInfo[instance].pVideoInfo->videoInput != TMDL_HDMIRX_INPUT_RAMPTEST_576P) &&
            (instanceStatusInfo[instance].pVideoInfo->videoInput != TMDL_HDMIRX_INPUT_RAMPTEST_480I) &&
            (instanceStatusInfo[instance].pVideoInfo->videoInput != TMDL_HDMIRX_INPUT_RAMPTEST_576I) &&
#endif
            (instanceStatusInfo[instance].pVideoInfo->videoInput != TMDL_HDMIRX_INPUT_TEST_576P) )
    {
        
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxHandleInterrupt(instance);

        if (errCode != TM_OK)
            WriteLn("Hand Int err");
        RETIF(errCode != TM_OK, errCode)
    }
    else
    {
        
    }
#endif
    return TM_OK;
}

/*============================================================================*/
/**
    \brief This function must be called every 50ms. It is used
           by the BSL driver to handle all time repetitive tasks required by
           the receiver. This function makes the BSL driver independent of the
           OS infrastructure (timer allocation).

    \param instance   Instance identifier.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range

 ******************************************************************************/
tmErrorCode_t
tmdlHdmiRxHeartBeat /*TBC for TDA19972*/
(
    tmInstance_t    instance
)
{
    tmErrorCode_t            errCode;
    tmdlHdmiRxResolutionID_t resolutionID;
    static UInt8             heartBeatCounter = 0;

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* Take the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    if( (instanceStatusInfo[instance].pVideoInfo->videoInput != TMDL_HDMIRX_INPUT_TEST_480P) &&
#ifdef TMFL_TDA19972_FAMILY
            (instanceStatusInfo[instance].pVideoInfo->videoInput != TMDL_HDMIRX_INPUT_TEST_480I) &&
            (instanceStatusInfo[instance].pVideoInfo->videoInput != TMDL_HDMIRX_INPUT_TEST_576I) &&
            (instanceStatusInfo[instance].pVideoInfo->videoInput != TMDL_HDMIRX_INPUT_RAMPTEST_480P) &&
            (instanceStatusInfo[instance].pVideoInfo->videoInput != TMDL_HDMIRX_INPUT_RAMPTEST_576P) &&
            (instanceStatusInfo[instance].pVideoInfo->videoInput != TMDL_HDMIRX_INPUT_RAMPTEST_480I) &&
            (instanceStatusInfo[instance].pVideoInfo->videoInput != TMDL_HDMIRX_INPUT_RAMPTEST_576I) &&
#endif
            (instanceStatusInfo[instance].pVideoInfo->videoInput != TMDL_HDMIRX_INPUT_TEST_576P) )
    {
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxHeartBeat(instance);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)



        /* Check periodically the video resolution */

        if ( /* (heartBeatCounter % 4 == 0) && */ (dlHdmiRxGetState(instance) == STATE_CONFIGURED))
        {
            errCode = dlHdmiRxCheckResolutionSyncTimings(instance, &resolutionID);
            /* If an error code is detected, do not notify the application */
            RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, TM_OK)

            if( (instanceStatusInfo[instance].pVideoInfo->detectedVideoResolution != resolutionID) &&
                    (instanceStatusInfo[instance].pVideoInfo->detectedVideoResolution != TMDL_HDMIRX_VIDEORES_UNKNOWN) )
            {
                /* Memorize the detected resolution */
                printf("NEW RESOLUTION DETECTED: %d instead of %d\n", resolutionID, instanceStatusInfo[instance].pVideoInfo->detectedVideoResolution);
                instanceStatusInfo[instance].pVideoInfo->detectedVideoResolution = resolutionID;

                /* Reset Start-Up Sequencer to force an unlock of the TMDS */
                errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxHDMISoftReset(instance,
                          instanceStatusInfo[instance].nackHdcp,
                          BSLHDMIRX_DONTRESET_AI,
                          BSLHDMIRX_DONTRESET_INFOFRAME,
                          BSLHDMIRX_DONTRESET_AUDIO_FIFO,
                          BSLHDMIRX_DONTRESET_GAMUT,
                          BSLHDMIRX_DONTRESET_FIFOCTRL,
#ifdef ALLOW_DDC_ACK
                          BSLHDMIRX_RESET_SUS_DDC,
#else
                          BSLHDMIRX_RESET_SUS,
#endif
                          BSLHDMIRX_DONTRESET_DC);
                RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)
//	                printf("SR5\n");

                errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxHDMISoftReset(instance,
                          instanceStatusInfo[instance].nackHdcp,
                          BSLHDMIRX_DONTRESET_AI,
                          BSLHDMIRX_DONTRESET_INFOFRAME,
                          BSLHDMIRX_DONTRESET_AUDIO_FIFO,
                          BSLHDMIRX_DONTRESET_GAMUT,
                          BSLHDMIRX_DONTRESET_FIFOCTRL,
#ifdef ALLOW_DDC_ACK
                          BSLHDMIRX_DONTRESET_SUS_DDC,
#else
                          BSLHDMIRX_DONTRESET_SUS,
#endif
                          BSLHDMIRX_DONTRESET_DC);
                RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)
            }
        }


        heartBeatCounter++;
    }
    /* Release the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    return TM_OK;
}

/******************************************************************************
    \brief Register event callbacks. Three types of callbacks can be
           registered : input activity related callback, data related
           callback (infoframes, packets, etc.) and general information
           callback. A null pointer means that no callback are registered.

    \param instance                 Instance identifier.
    \param pAnalogActivityCallback  Pointer to the callback function that will
                                    handle activity related events on analog
                                    inputs.
    \param pDigitalActivityCallback Pointer to the callback function that will
                                    handle activity related events on digital
                                    inputs.
    \param pDataCallback            Pointer to the callback function that will
                                    handle data related events (typically
                                    infoframes).
    \param pInfoCallback            Pointer to the callback function that will
                                    handle data related events (typically
                                    infoframes).

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_RESOURCE_NOT_OWNED: the caller does not own
              the resource
            - TMDL_ERR_DLHDMIRX_INVALID_STATE: the state is invalid for
              the function

******************************************************************************/
tmErrorCode_t tmdlHdmiRxRegisterCallbacks
(
    tmInstance_t                  instance,
    ptmdlHdmiRxActivityCallback_t pAnalogActivityCallback,
    ptmdlHdmiRxActivityCallback_t pDigitalActivityCallback,
    ptmdlHdmiRxDataCallback_t     pDataCallback,
    ptmdlHdmiRxInfoCallback_t     pInfoCallback
#ifdef TMFL_HDMI_OUT
    ,ptmdlHdmiRxHdmioutCallback_t pHdmioutCallback
#endif
)
{
    tmErrorCode_t   errCode;

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* Take the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    /* check if unit corresponding to instance is opened */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], unitTable[instance].opened == False, TMDL_ERR_DLHDMIRX_RESOURCE_NOT_OWNED)

    /* check if instance state is correct */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], dlHdmiRxGetState(instance) != STATE_INITIALIZED, TMDL_ERR_DLHDMIRX_INVALID_STATE)

    /* store callback pointers */
    unitTable[instance].pAnalogActivityCallback  = pAnalogActivityCallback;
    unitTable[instance].pDigitalActivityCallback = pDigitalActivityCallback;
    unitTable[instance].pDataCallback            = pDataCallback;
    unitTable[instance].pInfoCallback            = pInfoCallback;
#ifdef TMFL_HDMI_OUT
    unitTable[instance].pHdmioutCallback 		 = pHdmioutCallback;
#endif

    /* Release the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    return(TM_OK);
}

/******************************************************************************
    \brief This function allows enabling a specific event. By default, all
           events are disabled, except input lock.

    \param instance Instance identifier.
    \param event    Event to enable

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range

******************************************************************************/
tmErrorCode_t tmdlHdmiRxEnableEvent
(
    tmInstance_t      instance,
    tmdlHdmiRxEvent_t event
)
{
    tmErrorCode_t   errCode = TM_OK;
    Bool            eventInfoframes = False;
    Bool            eventGamut = False;
    Bool            eventAdvancedInfoframes = False;

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* check if the event exists */
    RETIF(event >= EVENT_NB, TMDL_ERR_DLHDMIRX_BAD_PARAMETER)
    
    /* Take the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    instanceStatusInfo[instance].pEventState[event].status = TMDL_HDMIRX_EVENT_ENABLED;
    
    //sprintf(Msg,"Ev %x",event);
    //WriteLn(Msg);
    switch (event)
    {
    case TMDL_HDMIRX_ACTIVITY_DETECTED :
    case TMDL_HDMIRX_INPUT_LOCKED :
        errCode = dlHdmiRxGetInterruptStatus(instance,event);
        break;
    case TMDL_HDMIRX_HDMI_DETECTED:
    case TMDL_HDMIRX_DVI_DETECTED:
    case TMDL_HDMIRX_ENCRYPTION_DETECTED:
    case TMDL_HDMIRX_NO_ENCRYPTION_DETECTED:
    case TMDL_HDMIRX_EESS_HDCP_DETECTED:
    case TMDL_HDMIRX_OESS_HDCP_DETECTED:
    case TMDL_HDMIRX_AVI_AVMUTE_ACTIVE:
    case TMDL_HDMIRX_AVI_AVMUTE_INACTIVE:
        errCode = dlHdmiRxGetInterruptStatus(instance,event);
        break;
    case TMDL_HDMIRX_DEEP_COLOR_MODE_24BITS:
    case TMDL_HDMIRX_DEEP_COLOR_MODE_30BITS:
    case TMDL_HDMIRX_DEEP_COLOR_MODE_36BITS:
    case TMDL_HDMIRX_DEEP_COLOR_MODE_48BITS:
        errCode = dlHdmiRxGetInterruptStatus(instance,event);
        break;
    case TMDL_HDMIRX_AVI_INFOFRAME_RECEIVED :
        eventInfoframes = True;
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetInterruptMask(instance, BSLHDMIRX_IRQSOURCE_AVI_IF, True);
        break;
#ifdef TMFL_TDA19972_FAMILY
    case TMDL_HDMIRX_VS_INFOFRAME_RECEIVED :
        eventInfoframes = True;
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetInterruptMask(instance, BSLHDMIRX_IRQSOURCE_VS_IF, True);
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetInterruptMask(instance, BSLHDMIRX_IRQSOURCE_VS_OTHER_BK1_IF, True);
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetInterruptMask(instance, BSLHDMIRX_IRQSOURCE_VS_OTHER_BK2_IF, True);
        break;
#endif
    case TMDL_HDMIRX_SPD_INFOFRAME_RECEIVED :
        eventInfoframes = True;
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetInterruptMask(instance, BSLHDMIRX_IRQSOURCE_SPD_IF, True);
        break;
    case TMDL_HDMIRX_AUD_INFOFRAME_RECEIVED :
        eventInfoframes = True;
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetInterruptMask(instance, BSLHDMIRX_IRQSOURCE_AUD_IF, True);
        break;
    case TMDL_HDMIRX_MPS_INFOFRAME_RECEIVED :
        eventInfoframes = True;
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetInterruptMask(instance, BSLHDMIRX_IRQSOURCE_MPS_IF, True);
        break;
    case TMDL_HDMIRX_GBD_PACKET_RECEIVED :
        eventGamut = True;
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetInterruptMask(instance, BSLHDMIRX_IRQSOURCE_GAMUT, True);
        break;
    case TMDL_HDMIRX_ISRC1_PACKET_RECEIVED :
        eventAdvancedInfoframes = True;
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetInterruptMask(instance, BSLHDMIRX_IRQSOURCE_ISRC1, True);
        break;
    case TMDL_HDMIRX_ISRC2_PACKET_RECEIVED :
        eventAdvancedInfoframes = True;
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetInterruptMask(instance, BSLHDMIRX_IRQSOURCE_ISRC2, True);
        break;
    case TMDL_HDMIRX_ACP_PACKET_RECEIVED :
        eventAdvancedInfoframes = True;
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetInterruptMask(instance, BSLHDMIRX_IRQSOURCE_ACP, True);
        break;

    default :
        break;
    }
    
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)

    if(eventInfoframes) /* TBC for TDA19972 */
    {
        /* Reset infoframe */
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxHDMISoftReset(instance,
                  instanceStatusInfo[instance].nackHdcp,
                  BSLHDMIRX_DONTRESET_AI,
                  BSLHDMIRX_RESET_INFOFRAME,
                  BSLHDMIRX_DONTRESET_AUDIO_FIFO,
                  BSLHDMIRX_DONTRESET_GAMUT,
                  BSLHDMIRX_DONTRESET_FIFOCTRL,
#ifdef ALLOW_DDC_ACK
                  BSLHDMIRX_DONTRESET_SUS_DDC,
#else
                  BSLHDMIRX_DONTRESET_SUS,
#endif
                  BSLHDMIRX_DONTRESET_DC);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)

//        printf("SR6\n");
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxHDMISoftReset(instance,
                  instanceStatusInfo[instance].nackHdcp,
                  BSLHDMIRX_DONTRESET_AI,
                  BSLHDMIRX_DONTRESET_INFOFRAME,
                  BSLHDMIRX_DONTRESET_AUDIO_FIFO,
                  BSLHDMIRX_DONTRESET_GAMUT,
                  BSLHDMIRX_DONTRESET_FIFOCTRL,
#ifdef ALLOW_DDC_ACK
                  BSLHDMIRX_DONTRESET_SUS_DDC,
#else
                  BSLHDMIRX_DONTRESET_SUS,
#endif
                  BSLHDMIRX_DONTRESET_DC);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)
    }

    if(eventGamut) /* TBC for TDA19972 */
    {
        /* Reset gamut */
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxHDMISoftReset(instance,
                  instanceStatusInfo[instance].nackHdcp,
                  BSLHDMIRX_DONTRESET_AI,
                  BSLHDMIRX_DONTRESET_INFOFRAME,
                  BSLHDMIRX_DONTRESET_AUDIO_FIFO,
                  BSLHDMIRX_RESET_GAMUT,
                  BSLHDMIRX_DONTRESET_FIFOCTRL,
#ifdef ALLOW_DDC_ACK
                  BSLHDMIRX_DONTRESET_SUS_DDC,
#else
                  BSLHDMIRX_DONTRESET_SUS,
#endif
                  BSLHDMIRX_DONTRESET_DC);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)
//        printf("SR7\n");

        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxHDMISoftReset(instance,
                  instanceStatusInfo[instance].nackHdcp,
                  BSLHDMIRX_DONTRESET_AI,
                  BSLHDMIRX_DONTRESET_INFOFRAME,
                  BSLHDMIRX_DONTRESET_AUDIO_FIFO,
                  BSLHDMIRX_DONTRESET_GAMUT,
                  BSLHDMIRX_DONTRESET_FIFOCTRL,
#ifdef ALLOW_DDC_ACK
                  BSLHDMIRX_DONTRESET_SUS_DDC,
#else
                  BSLHDMIRX_DONTRESET_SUS,
#endif
                  BSLHDMIRX_DONTRESET_DC);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)
    }

    if(eventAdvancedInfoframes) /* TBC for TDA19972 */
    {
        /* Reset infoframe */
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxHDMISoftReset(instance,
                  instanceStatusInfo[instance].nackHdcp,
                  BSLHDMIRX_RESET_AI,
                  BSLHDMIRX_DONTRESET_INFOFRAME,
                  BSLHDMIRX_DONTRESET_AUDIO_FIFO,
                  BSLHDMIRX_DONTRESET_GAMUT,
                  BSLHDMIRX_DONTRESET_FIFOCTRL,
#ifdef ALLOW_DDC_ACK
                  BSLHDMIRX_DONTRESET_SUS_DDC,
#else
                  BSLHDMIRX_DONTRESET_SUS,
#endif
                  BSLHDMIRX_DONTRESET_DC);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)

//        printf("SR8\n");
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxHDMISoftReset(instance,
                  instanceStatusInfo[instance].nackHdcp,
                  BSLHDMIRX_DONTRESET_AI,
                  BSLHDMIRX_DONTRESET_INFOFRAME,
                  BSLHDMIRX_DONTRESET_AUDIO_FIFO,
                  BSLHDMIRX_DONTRESET_GAMUT,
                  BSLHDMIRX_DONTRESET_FIFOCTRL,
#ifdef ALLOW_DDC_ACK
                  BSLHDMIRX_DONTRESET_SUS_DDC,
#else
                  BSLHDMIRX_DONTRESET_SUS,
#endif
                  BSLHDMIRX_DONTRESET_DC);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)
    }

    /* Release the semaphore */
    
    errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    return(TM_OK);
}

/******************************************************************************
    \brief This function allows disabling a specific event. By default, all
           events are disabled, except input lock.

    \param instance Instance identifier.
    \param event    Event to disable

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range

******************************************************************************/
tmErrorCode_t tmdlHdmiRxDisableEvent
(
    tmInstance_t      instance,
    tmdlHdmiRxEvent_t event
)
{
    tmErrorCode_t   errCode;

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* check if the event exists */
    RETIF(event >= EVENT_NB, TMDL_ERR_DLHDMIRX_BAD_PARAMETER)

    /* Take the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    instanceStatusInfo[instance].pEventState[event].status = TMDL_HDMIRX_EVENT_DISABLED;

    switch (event)
    {
    case TMDL_HDMIRX_AVI_INFOFRAME_RECEIVED :
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetInterruptMask(instance, BSLHDMIRX_IRQSOURCE_AVI_IF, False);
        break;
#ifdef TMFL_TDA19972_FAMILY
    case TMDL_HDMIRX_VS_INFOFRAME_RECEIVED :
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetInterruptMask(instance, BSLHDMIRX_IRQSOURCE_VS_IF, False);
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetInterruptMask(instance, BSLHDMIRX_IRQSOURCE_VS_OTHER_BK1_IF, False);
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetInterruptMask(instance, BSLHDMIRX_IRQSOURCE_VS_OTHER_BK2_IF, False);
        break;
#endif
    case TMDL_HDMIRX_SPD_INFOFRAME_RECEIVED :
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetInterruptMask(instance, BSLHDMIRX_IRQSOURCE_SPD_IF, False);
        break;
    case TMDL_HDMIRX_AUD_INFOFRAME_RECEIVED :
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetInterruptMask(instance, BSLHDMIRX_IRQSOURCE_AUD_IF, False);
        break;
    case TMDL_HDMIRX_MPS_INFOFRAME_RECEIVED :
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetInterruptMask(instance, BSLHDMIRX_IRQSOURCE_MPS_IF, False);
        break;
    case TMDL_HDMIRX_GBD_PACKET_RECEIVED :
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetInterruptMask(instance, BSLHDMIRX_IRQSOURCE_GAMUT, False);
        break;
    case TMDL_HDMIRX_ISRC1_PACKET_RECEIVED :
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetInterruptMask(instance, BSLHDMIRX_IRQSOURCE_ISRC1, False);
        break;
    case TMDL_HDMIRX_ISRC2_PACKET_RECEIVED :
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetInterruptMask(instance, BSLHDMIRX_IRQSOURCE_ISRC2, False);
        break;
    case TMDL_HDMIRX_ACP_PACKET_RECEIVED :
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetInterruptMask(instance, BSLHDMIRX_IRQSOURCE_ACP, False);
        break;

    default :
        break;
    }
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)

    /* Release the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    return(TM_OK);
}

/******************************************************************************
    \brief Select an input. This function is typically used after having
           detected activity on one specific input. After having selected an
           input, application will be notified of the input lock through the
           appropriate event.

    \param instance Instance identifier.
    \param input    Input to switch to.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range

******************************************************************************/
tmErrorCode_t tmdlHdmiRxSelectInput
(
    tmInstance_t      instance,
    tmdlHdmiRxInput_t input
)
{
    tmErrorCode_t                           errCode            = TM_OK;
    tmbslHdmiRxInputSelSyncType_t           syncSignal         = BSLHDMIRX_SYNCSIGNAL_DIGITAL;
    tmbslHdmiRxInputSelDigitalMode_t        digitalMode        = BSLHDMIRX_INPUTSELECTION_AUTO;
    tmbslHdmiRxInputSelDigitalSource_t      digitalInputSelect = BSLHDMIRX_DIGITALINPUT_A;
    tmbslHdmiRxInputSelVideoSource_t        videoSource        = BSLHDMIRX_VIDEO_SOURCE_DIGITAL;
    tmbslHdmiRxInputSelAnalogVideoSource_t  anInputSelect      = BSLHDMIRX_VIDEOINPUT_AUTO;
    tmbslHdmiRxPixClkOutSelection_t         clkOutSelect       = BSLHDMIRX_PIXCLK_OUT_HDMICLOCK;
    tmbslHdmiRxPixClkForSelection_t         clkForSelect       = BSLHDMIRX_PIXCLK_FOR_ZERO;
    tmbslHdmiRxPixClkPixSelection_t         clkPixSelect       = BSLHDMIRX_PIXCLK_PIX_HDMICLOCK;
    tmbslHdmiRxInternalVSync_t              internalVSync;
    tmbslHdmiRxInternalHSync_t              internalHSync;
    tmbslHdmiRxSyncOutputVSSelection_t      outputVSSelection  = BSLHDMIRX_SYNCOUTPUT_VSYNC_HDMI;
    tmbslHdmiRxSyncOutputHSSelection_t      outputHSSelection  = BSLHDMIRX_SYNCOUTPUT_HSYNC_HDMI;
    UInt8                                   delockDelay;
    Bool									currentInputIsTestPattern = False;
    Bool									selectedInputIsTestPattern = False;
    tmbslHdmiRxTestPatternFormat_t          testPatternFormat  = BSLHDMIRX_TESTPATTERN_480P;
    tmdlHdmiRxTestPatternAction_t           testPatternAction;
    tmbslHdmiRxAudioOutputFormat_t          outputFormat       = BSLHDMIRX_AUDIOOUTPUT_I2S;
    tmbslHdmiRxAudioTestTone_t              testTone           = BSLHDMIRX_TESTTONE_OFF;
    tmbslHdmiRxAudioPathSpFlagMode_t        spFlagMode         = BSLHDMIRX_AUDIOPATH_SP_FLAG_USED_BY_FIFO;
    tmbslHdmiRxTestPatternSel_t				PatternSel         = BSLHDMIRX_TESTPATTERN_OFF;
    tmbslHdmiRxVhrefStandardDetection_t     vhrefStandardDetection = BSLHDMIRX_VHREF_STANDARD_OFF;

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* Take the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    /* evaluate Test Pattern bools */
    currentInputIsTestPattern = (
                                    (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_480P)
                                    || (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_576P)
#ifdef TMFL_TDA19972_FAMILY
                                    || (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_480I)
                                    || (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_576I)
                                    || (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_480P)
                                    || (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_576P)
                                    || (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_480I)
                                    || (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_576I)
#endif
                                );
    selectedInputIsTestPattern = (
                                     (input == TMDL_HDMIRX_INPUT_TEST_480P)
                                     || (input == TMDL_HDMIRX_INPUT_TEST_576P)
#ifdef TMFL_TDA19972_FAMILY
                                     || (input == TMDL_HDMIRX_INPUT_TEST_480I)
                                     || (input == TMDL_HDMIRX_INPUT_TEST_576I)
                                     || (input == TMDL_HDMIRX_INPUT_RAMPTEST_480P)
                                     || (input == TMDL_HDMIRX_INPUT_RAMPTEST_576P)
                                     || (input == TMDL_HDMIRX_INPUT_RAMPTEST_480I)
                                     || (input == TMDL_HDMIRX_INPUT_RAMPTEST_576I)
#endif
                                 );

    /* Check the previous selected input */
    if (selectedInputIsTestPattern)
    {
        testPatternAction = TEST_PATTERN_TO_BE_ENABLED;
    }
    else if( currentInputIsTestPattern && !selectedInputIsTestPattern )
    {
        testPatternAction = TEST_PATTERN_TO_BE_DISABLED;
    }
    else
    {
        testPatternAction = TEST_PATTERN_NOT_USED;
    }

    /* Memorize the new video input */
    instanceStatusInfo[instance].pVideoInfo->videoInput = input;

    /* Input selection */
    switch(input)
    {
    case TMDL_HDMIRX_INPUT_HDMI_A:
        /* Digital input A */
        syncSignal          = BSLHDMIRX_SYNCSIGNAL_DIGITAL;
        digitalMode         = BSLHDMIRX_INPUTSELECTION_MANUAL;
        digitalInputSelect  = BSLHDMIRX_DIGITALINPUT_A;
        videoSource         = BSLHDMIRX_VIDEO_SOURCE_DIGITAL;
        anInputSelect       = BSLHDMIRX_VIDEOINPUT_AUTO;
        internalVSync       = BSLHDMIRX_INTERNALVSYNC_HDMI;
        internalHSync       = BSLHDMIRX_INTERNALHSYNC_HDMI;
        outputVSSelection   = BSLHDMIRX_SYNCOUTPUT_VSYNC_HDMI;
        outputHSSelection   = BSLHDMIRX_SYNCOUTPUT_HSYNC_HDMI;

        /* Check the memorized video format */
        if( (instanceStatusInfo[instance].pVideoInfo->videoFormat == TMDL_HDMIRX_OUTPUTFORMAT_444) ||
                (instanceStatusInfo[instance].pVideoInfo->videoFormat == TMDL_HDMIRX_OUTPUTFORMAT_422_SMP) )
        {
            clkOutSelect = BSLHDMIRX_PIXCLK_OUT_HDMICLOCK;
            clkForSelect = BSLHDMIRX_PIXCLK_FOR_ZERO;
            clkPixSelect = BSLHDMIRX_PIXCLK_PIX_HDMICLOCK;
        }

        if(instanceStatusInfo[instance].pVideoInfo->videoFormat == TMDL_HDMIRX_OUTPUTFORMAT_422_CCIR)
        {
            if(instanceStatusInfo[instance].pVideoInfo->outputClockMode == TMDL_HDMIRX_OUTCLKMODE_DOUBLE_EDGE)
            {
                clkOutSelect = BSLHDMIRX_PIXCLK_OUT_HDMICLOCK;
            }
            else
            {
                clkOutSelect = BSLHDMIRX_PIXCLK_OUT_HDMICLOCK_X2;
            }
            clkForSelect = BSLHDMIRX_PIXCLK_FOR_CLOCKHDMI_X2;
            clkPixSelect = BSLHDMIRX_PIXCLK_PIX_HDMICLOCK;
        }
        break;

    case TMDL_HDMIRX_INPUT_HDMI_B:
        /* Digital input B */
        syncSignal          = BSLHDMIRX_SYNCSIGNAL_DIGITAL;
        digitalMode         = BSLHDMIRX_INPUTSELECTION_MANUAL;
        digitalInputSelect  = BSLHDMIRX_DIGITALINPUT_B;
        videoSource         = BSLHDMIRX_VIDEO_SOURCE_DIGITAL;
        anInputSelect       = BSLHDMIRX_VIDEOINPUT_AUTO;
        internalVSync       = BSLHDMIRX_INTERNALVSYNC_HDMI;
        internalHSync       = BSLHDMIRX_INTERNALHSYNC_HDMI;
        outputVSSelection   = BSLHDMIRX_SYNCOUTPUT_VSYNC_HDMI;
        outputHSSelection   = BSLHDMIRX_SYNCOUTPUT_HSYNC_HDMI;

        /* Check the memorized video format */
        if( (instanceStatusInfo[instance].pVideoInfo->videoFormat == TMDL_HDMIRX_OUTPUTFORMAT_444) ||
                (instanceStatusInfo[instance].pVideoInfo->videoFormat == TMDL_HDMIRX_OUTPUTFORMAT_422_SMP) )
        {
            clkOutSelect = BSLHDMIRX_PIXCLK_OUT_HDMICLOCK;
            clkForSelect = BSLHDMIRX_PIXCLK_FOR_ZERO;
            clkPixSelect = BSLHDMIRX_PIXCLK_PIX_HDMICLOCK;
        }

        if(instanceStatusInfo[instance].pVideoInfo->videoFormat == TMDL_HDMIRX_OUTPUTFORMAT_422_CCIR)
        {
            if(instanceStatusInfo[instance].pVideoInfo->outputClockMode == TMDL_HDMIRX_OUTCLKMODE_DOUBLE_EDGE)
            {
                clkOutSelect = BSLHDMIRX_PIXCLK_OUT_HDMICLOCK;
            }
            else
            {
                clkOutSelect = BSLHDMIRX_PIXCLK_OUT_HDMICLOCK_X2;
            }
            clkForSelect = BSLHDMIRX_PIXCLK_FOR_CLOCKHDMI_X2;
            clkPixSelect = BSLHDMIRX_PIXCLK_PIX_HDMICLOCK;
        }
        break;


    case TMDL_HDMIRX_INPUT_TEST_480P:
#ifdef TMFL_TDA19972_FAMILY
    case TMDL_HDMIRX_INPUT_RAMPTEST_480P:
#endif
        internalVSync       = BSLHDMIRX_INTERNALVSYNC_HDMI;
        internalHSync       = BSLHDMIRX_INTERNALHSYNC_HDMI;
        testPatternFormat   = BSLHDMIRX_TESTPATTERN_480P;
        PatternSel = BSLHDMIRX_TESTPATTERN_ON;
#ifdef TMFL_TDA19972_FAMILY
        outputVSSelection   = BSLHDMIRX_SYNCOUTPUT_VSYNC_VHREF;
        outputHSSelection   = BSLHDMIRX_SYNCOUTPUT_HSYNC_VHREF;
        vhrefStandardDetection = BSLHDMIRX_VHREF_STANDARD_NTSC;
        if (input == TMDL_HDMIRX_INPUT_RAMPTEST_480P)
        {
            PatternSel = BSLHDMIRX_TESTPATTERN_RAMP;
        }
#endif
        /* Check the memorized video format */
        if( (instanceStatusInfo[instance].pVideoInfo->videoFormat == TMDL_HDMIRX_OUTPUTFORMAT_444) ||
                (instanceStatusInfo[instance].pVideoInfo->videoFormat == TMDL_HDMIRX_OUTPUTFORMAT_422_SMP) )
        {
            clkOutSelect = BSLHDMIRX_PIXCLK_OUT_HDMICLOCK;
            clkForSelect = BSLHDMIRX_PIXCLK_FOR_ZERO;
            clkPixSelect = BSLHDMIRX_PIXCLK_PIX_HDMICLOCK;
        }

        if(instanceStatusInfo[instance].pVideoInfo->videoFormat == TMDL_HDMIRX_OUTPUTFORMAT_422_CCIR)
        {
            if(instanceStatusInfo[instance].pVideoInfo->outputClockMode == TMDL_HDMIRX_OUTCLKMODE_DOUBLE_EDGE)
            {
                clkOutSelect = BSLHDMIRX_PIXCLK_OUT_HDMICLOCK;
            }
            else
            {
                clkOutSelect = BSLHDMIRX_PIXCLK_OUT_HDMICLOCK_X2;
            }
            clkForSelect = BSLHDMIRX_PIXCLK_FOR_CLOCKHDMI_X2;
            clkPixSelect = BSLHDMIRX_PIXCLK_PIX_HDMICLOCK;
        }
        break;


    case TMDL_HDMIRX_INPUT_TEST_576P:
#ifdef TMFL_TDA19972_FAMILY
    case TMDL_HDMIRX_INPUT_RAMPTEST_576P:
#endif
        internalVSync       = BSLHDMIRX_INTERNALVSYNC_HDMI;
        internalHSync       = BSLHDMIRX_INTERNALHSYNC_HDMI;
        testPatternFormat   = BSLHDMIRX_TESTPATTERN_576P;
        PatternSel = BSLHDMIRX_TESTPATTERN_ON;
#ifdef TMFL_TDA19972_FAMILY
        outputVSSelection   = BSLHDMIRX_SYNCOUTPUT_VSYNC_VHREF;
        outputHSSelection   = BSLHDMIRX_SYNCOUTPUT_HSYNC_VHREF;
        vhrefStandardDetection = BSLHDMIRX_VHREF_STANDARD_PAL;
        if (input == TMDL_HDMIRX_INPUT_RAMPTEST_576P)
        {
            PatternSel = BSLHDMIRX_TESTPATTERN_RAMP;
        }
#endif

        /* Check the memorized video format */
        if( (instanceStatusInfo[instance].pVideoInfo->videoFormat == TMDL_HDMIRX_OUTPUTFORMAT_444) ||
                (instanceStatusInfo[instance].pVideoInfo->videoFormat == TMDL_HDMIRX_OUTPUTFORMAT_422_SMP) )
        {
            clkOutSelect = BSLHDMIRX_PIXCLK_OUT_HDMICLOCK;
            clkForSelect = BSLHDMIRX_PIXCLK_FOR_ZERO;
            clkPixSelect = BSLHDMIRX_PIXCLK_PIX_HDMICLOCK;
        }

        if(instanceStatusInfo[instance].pVideoInfo->videoFormat == TMDL_HDMIRX_OUTPUTFORMAT_422_CCIR)
        {
            if(instanceStatusInfo[instance].pVideoInfo->outputClockMode == TMDL_HDMIRX_OUTCLKMODE_DOUBLE_EDGE)
            {
                clkOutSelect = BSLHDMIRX_PIXCLK_OUT_HDMICLOCK;
            }
            else
            {
                clkOutSelect = BSLHDMIRX_PIXCLK_OUT_HDMICLOCK_X2;
            }
            clkForSelect = BSLHDMIRX_PIXCLK_FOR_CLOCKHDMI_X2;
            clkPixSelect = BSLHDMIRX_PIXCLK_PIX_HDMICLOCK;
        }
        break;


#ifdef TMFL_TDA19972_FAMILY
    case TMDL_HDMIRX_INPUT_TEST_480I:
    case TMDL_HDMIRX_INPUT_RAMPTEST_480I:
        internalVSync       = BSLHDMIRX_INTERNALVSYNC_HDMI;
        internalHSync       = BSLHDMIRX_INTERNALHSYNC_HDMI;
        testPatternFormat   = BSLHDMIRX_TESTPATTERN_480I;
        outputVSSelection   = BSLHDMIRX_SYNCOUTPUT_VSYNC_VHREF;
        outputHSSelection   = BSLHDMIRX_SYNCOUTPUT_HSYNC_VHREF;
        vhrefStandardDetection = BSLHDMIRX_VHREF_STANDARD_OFF;
        PatternSel = BSLHDMIRX_TESTPATTERN_ON;
        if (input == TMDL_HDMIRX_INPUT_RAMPTEST_480I)
        {
            PatternSel = BSLHDMIRX_TESTPATTERN_RAMP;
        }

        /* Check the memorized video format */
        if( (instanceStatusInfo[instance].pVideoInfo->videoFormat == TMDL_HDMIRX_OUTPUTFORMAT_444) ||
                (instanceStatusInfo[instance].pVideoInfo->videoFormat == TMDL_HDMIRX_OUTPUTFORMAT_422_SMP) )
        {
            clkOutSelect = BSLHDMIRX_PIXCLK_OUT_HDMICLOCK;
            clkForSelect = BSLHDMIRX_PIXCLK_FOR_ZERO;
            clkPixSelect = BSLHDMIRX_PIXCLK_PIX_HDMICLOCK;
        }

        if(instanceStatusInfo[instance].pVideoInfo->videoFormat == TMDL_HDMIRX_OUTPUTFORMAT_422_CCIR)
        {
            if(instanceStatusInfo[instance].pVideoInfo->outputClockMode == TMDL_HDMIRX_OUTCLKMODE_DOUBLE_EDGE)
            {
                clkOutSelect = BSLHDMIRX_PIXCLK_OUT_HDMICLOCK;
            }
            else
            {
                clkOutSelect = BSLHDMIRX_PIXCLK_OUT_HDMICLOCK_X2;
            }
            clkForSelect = BSLHDMIRX_PIXCLK_FOR_CLOCKHDMI_X2;
            clkPixSelect = BSLHDMIRX_PIXCLK_PIX_HDMICLOCK;
        }
        break;


    case TMDL_HDMIRX_INPUT_TEST_576I:
    case TMDL_HDMIRX_INPUT_RAMPTEST_576I:
        internalVSync       = BSLHDMIRX_INTERNALVSYNC_HDMI;
        internalHSync       = BSLHDMIRX_INTERNALHSYNC_HDMI;
        testPatternFormat   = BSLHDMIRX_TESTPATTERN_576I;
        outputVSSelection   = BSLHDMIRX_SYNCOUTPUT_VSYNC_VHREF;
        outputHSSelection   = BSLHDMIRX_SYNCOUTPUT_HSYNC_VHREF;
        vhrefStandardDetection = BSLHDMIRX_VHREF_STANDARD_PAL;
        PatternSel = BSLHDMIRX_TESTPATTERN_ON;
        if (input == TMDL_HDMIRX_INPUT_RAMPTEST_576I)
        {
            PatternSel = BSLHDMIRX_TESTPATTERN_RAMP;
        }

        /* Check the memorized video format */
        if( (instanceStatusInfo[instance].pVideoInfo->videoFormat == TMDL_HDMIRX_OUTPUTFORMAT_444) ||
                (instanceStatusInfo[instance].pVideoInfo->videoFormat == TMDL_HDMIRX_OUTPUTFORMAT_422_SMP) )
        {
            clkOutSelect = BSLHDMIRX_PIXCLK_OUT_HDMICLOCK;
            clkForSelect = BSLHDMIRX_PIXCLK_FOR_ZERO;
            clkPixSelect = BSLHDMIRX_PIXCLK_PIX_HDMICLOCK;
        }

        if(instanceStatusInfo[instance].pVideoInfo->videoFormat == TMDL_HDMIRX_OUTPUTFORMAT_422_CCIR)
        {
            if(instanceStatusInfo[instance].pVideoInfo->outputClockMode == TMDL_HDMIRX_OUTCLKMODE_DOUBLE_EDGE)
            {
                clkOutSelect = BSLHDMIRX_PIXCLK_OUT_HDMICLOCK;
            }
            else
            {
                clkOutSelect = BSLHDMIRX_PIXCLK_OUT_HDMICLOCK_X2;
            }
            clkForSelect = BSLHDMIRX_PIXCLK_FOR_CLOCKHDMI_X2;
            clkPixSelect = BSLHDMIRX_PIXCLK_PIX_HDMICLOCK;
        }
        break;



#endif

    default:
        /* Release the semaphore */
        errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
        RETIF(errCode != TM_OK, errCode)
        return TMDL_ERR_DLHDMIRX_BAD_PARAMETER;
    }

    if(testPatternAction == TEST_PATTERN_TO_BE_ENABLED)
    {
        /* Enable test pattern */
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxActivateTestPattern(instance,
                  PatternSel,
                  testPatternFormat);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, errCode)
    }
    else if(testPatternAction == TEST_PATTERN_TO_BE_DISABLED)
    {
        /* Disable test pattern */
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxActivateTestPattern(instance,
                  BSLHDMIRX_TESTPATTERN_OFF,
                  testPatternFormat);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, errCode)

        /* ConfigureInput */
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureInput(instance,
                  syncSignal, digitalMode,
                  digitalInputSelect, videoSource,
                  anInputSelect,
                  BSLHDMIRX_VDP_RESET_MODE_MANUAL,
                  BSLHDMIRX_VDP_RESET_ON);
//#if 0
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, errCode)

        /* Reset Start-Up Sequencer to force an unlock of the TMDS */
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxHDMISoftReset(instance,
                  instanceStatusInfo[instance].nackHdcp,
                  BSLHDMIRX_DONTRESET_AI,
                  BSLHDMIRX_DONTRESET_INFOFRAME,
                  BSLHDMIRX_DONTRESET_AUDIO_FIFO,
                  BSLHDMIRX_DONTRESET_GAMUT,
                  BSLHDMIRX_DONTRESET_FIFOCTRL,
#ifdef ALLOW_DDC_ACK
                  BSLHDMIRX_RESET_SUS_DDC,
#else
                  BSLHDMIRX_RESET_SUS,
#endif
                  BSLHDMIRX_DONTRESET_DC);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)

//        printf("SR9\n");
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxHDMISoftReset(instance,
                  instanceStatusInfo[instance].nackHdcp,
                  BSLHDMIRX_DONTRESET_AI,
                  BSLHDMIRX_DONTRESET_INFOFRAME,
                  BSLHDMIRX_DONTRESET_AUDIO_FIFO,
                  BSLHDMIRX_DONTRESET_GAMUT,
                  BSLHDMIRX_DONTRESET_FIFOCTRL,
#ifdef ALLOW_DDC_ACK
                  BSLHDMIRX_DONTRESET_SUS_DDC,
#else
                  BSLHDMIRX_DONTRESET_SUS,
#endif
                  BSLHDMIRX_DONTRESET_DC);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)
//#endif
        /* Interrupts are disabled when test pattern is on, so they must be re-enabled when test pattern is disabled */
        switch(instance)
        {
        case INSTANCE_0:
            tmdlHdmiRxIWEnableInterrupts(TMDL_HDMI_IW_RX_1);
            break;
        default:
            return TMDL_ERR_DLHDMIRX_BAD_INSTANCE;
        }
    }
    else
    {
        /* ConfigureInput */
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureInput(instance,
                  syncSignal, digitalMode,
                  digitalInputSelect, videoSource,
                  anInputSelect,
                  BSLHDMIRX_VDP_RESET_MODE_MANUAL,
                  BSLHDMIRX_VDP_RESET_ON);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, errCode)

        /* ConfigureSyncOutput */
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureSyncOutput(instance,
                  BSLHDMIRX_SYNCOUTPUT_CS_HS_VS,
                  outputVSSelection,
                  outputHSSelection);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, errCode)
    }

    /* ConfigurePixelClockGenerator */
#ifdef TMFL_TDA19972_FAMILY
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigurePixelClockGenerator(instance,
              BSLHDMIRX_PIXCLK_OUT_TOGGLE_OFF,
              instanceStatusInfo[instance].pVideoInfo->videoFormat,
              instanceStatusInfo[instance].pVideoInfo->outputClockMode);
#else
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigurePixelClockGenerator(instance,
              BSLHDMIRX_PIXCLK_OUT_TOGGLE_ON,
              clkOutSelect,
              clkForSelect,
              clkPixSelect);
#endif
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, errCode)


#ifdef TMFL_TDA19972_FAMILY
    /* Configure VHREF control */
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureVHRef(instance,
              BSLHDMIRX_VHREF_INTERLACED_FRAMEFIELD,
              BSLHDMIRX_VHREF_FRAMEVSYNC_AUTO,
              vhrefStandardDetection,
              BSLHDMIRX_VHREF_VREF_MANUAL,
              BSLHDMIRX_VHREF_HREF_MANUAL,
              BSLHDMIRX_VHREF_CSYNC_ACTIVE_LOW);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, errCode)
#endif



    /* ConfigureInternalVHSync */
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureInternalVHSync(instance,
              internalVSync,
              internalHSync);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, errCode)

    /* ConfigureHDCPErrorProtection */
    delockDelay = (UInt8) 0x07;
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureHDCPErrorProtection(instance,
              delockDelay,
              BSLHDMIRX_HDCP_DEMEASUREMENT_1_VDP,
              BSLHDMIRX_HDCP_DEREGENERATION_DISABLE,
              BSLHDMIRX_HDCP_SENSIVITY_3_DEFILTER,
              BSLHDMIRX_CH0_HDCP_DECOMPOSITION,
              BSLHDMIRX_HDCP_3_CTLFILTERSENSIVITY,
              BSLHDMIRX_HDCP_3_VSFILTERSENSIVITY,
              BSLHDMIRX_HDCP_3_HSFILTERSENSIVITY);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, errCode)

    /* Test the audio mute state */
    if(instanceStatusInfo[instance].pMuteAndVideoPort->audioMuteState == False)
    {
        /* Test tone must be enabled if video input is test color bar */
        if (selectedInputIsTestPattern)
        {
            testTone = BSLHDMIRX_TESTTONE_ON;
        }

        /* Test the audio format */
        switch(instanceStatusInfo[instance].pMuteAndVideoPort->audioFormat)
        {
        case TMDL_HDMIRX_AUDIOFORMAT_I2S16:
        /* I2S 16 bits */
        case TMDL_HDMIRX_AUDIOFORMAT_I2S32:
        /* I2S 32 bits */
        case TMDL_HDMIRX_AUDIOFORMAT_OBA:
            /* One Bit Audio */
            outputFormat  = BSLHDMIRX_AUDIOOUTPUT_I2S;
            spFlagMode    = BSLHDMIRX_AUDIOPATH_SP_FLAG_USED_BY_FIFO;
            break;

        case TMDL_HDMIRX_AUDIOFORMAT_SPDIF:
            /* SPDIF */
            outputFormat  = BSLHDMIRX_AUDIOOUTPUT_SPDIF;
            spFlagMode    = BSLHDMIRX_AUDIOPATH_SP_FLAG_USED_BY_FIFO;
            break;

        case TMDL_HDMIRX_AUDIOFORMAT_I2S16_HBR_DEMUX:
        /* I2S 16 bits High Bit Rate demux */
        case TMDL_HDMIRX_AUDIOFORMAT_I2S32_HBR_DEMUX:
        /* I2S 32 bits High Bit Rate demux */
        case TMDL_HDMIRX_AUDIOFORMAT_DST:
            /* Direct Stream Transfer */
            outputFormat  = BSLHDMIRX_AUDIOOUTPUT_I2S;
            spFlagMode    = BSLHDMIRX_AUDIOPATH_SP_FLAG_IGNORED_BY_FIFO;
            break;

        case TMDL_HDMIRX_AUDIOFORMAT_SPDIF_HBR_DEMUX:
            /* SPDIF High Bit Rate demux */
            outputFormat  = BSLHDMIRX_AUDIOOUTPUT_SPDIF;
            spFlagMode    = BSLHDMIRX_AUDIOPATH_SP_FLAG_IGNORED_BY_FIFO;
            break;

        default:
            return TMDL_ERR_DLHDMIRX_BAD_PARAMETER;
        }

        /* ConfigureAudioFormatter */
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureAudioFormatter(instance,
                  spFlagMode,
                  instanceStatusInfo[instance].pChannelAssignmentInfo->currentChannelAssignment,
                  BSLHDMIRX_AUDIOPATH_LAYOUT_PACKET_HEADER,
                  BSLHDMIRX_AUDIO_LAYOUT_FORCED_0,
                  FIFO_LATENCY,
                  testTone,
                  outputFormat,
                  BSLHDMIRX_AUDIOOUTPUT_NORMAL,
                  BSLHDMIRX_AUDIOOUTPUT_NORMAL,
                  BSLHDMIRX_AUDIOOUTPUT_NORMAL,
                  BSLHDMIRX_AUDIOOUTPUT_NORMAL,
                  BSLHDMIRX_AUDIOOUTPUT_NORMAL,
                  BSLHDMIRX_AUDIOOUTPUT_NORMAL,
                  BSLHDMIRX_AUDIOOUTPUT_NORMAL,
                  BSLHDMIRX_AUDIOOUTPUT_NORMAL,
                  BSLHDMIRX_AUDIOOUTPUT_NORMAL,
                  BSLHDMIRX_AUDIOOUTPUT_NORMAL);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, errCode)
    }

    /* Release the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    return(TM_OK);
}

/******************************************************************************
    \brief detect video resolution at selected input.

    \param instance      Instance identifier.
    \param pResolutionID Pointer to the ID of the detected resolution

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMDL_ERR_DLHDMIRX_RESOLUTION_NOT_DETECTED: the resolution has
              not been detected
            - TMDL_ERR_DLHDMIRX_NO_RESOURCES: the resource is not available
            - TMDL_ERR_DLHDMIRX_INVALID_STATE: the state is invalid for
              the function

******************************************************************************/
tmErrorCode_t tmdlHdmiRxDetectResolution
(
    tmInstance_t              instance,
    tmdlHdmiRxResolutionID_t *pResolutionID
)
{
    tmErrorCode_t               errCode = TM_OK;
    tmdlHdmiRxDriverState_t		State;

    /* Test pResolutionID <> NULL */
    RETIF(pResolutionID == Null,TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)


    RETIF(errCode != TM_OK, errCode)

    /* Take the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    /* Check the current state */
    State = dlHdmiRxGetState(instance);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], (State != STATE_LOCKED) && (State != STATE_CONFIGURED), TMDL_ERR_DLHDMIRX_INVALID_STATE)

    /* Check the sync timings to detect the resolution */
    errCode = dlHdmiRxCheckResolutionSyncTimings(instance, pResolutionID);

    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], ((errCode !=TM_OK) &&(errCode !=TMDL_ERR_DLHDMIRX_RESOLUTION_NOT_DETECTED)) , errCode)

    /* Memorize the detected resolution */
    instanceStatusInfo[instance].pVideoInfo->detectedVideoResolution = *pResolutionID;

    /* Release the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    return(TM_OK);
}

/******************************************************************************
    \brief Get specifications of a given resolution.

    \param instance         Instance identifier.
    \param resolutionID     ID of the resolution to retrieve specs from.
    \param presolutionSpecs Pointer to the structure receiving specs.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMDL_ERR_DLHDMIRX_RESOLUTION_UNKNOWN: the resolution is unknown

******************************************************************************/
tmErrorCode_t tmdlHdmiRxGetResolutionSpecs
(
    tmInstance_t                 instance,
    tmdlHdmiRxResolutionID_t     resolutionID,
    tmdlHdmiRxResolutionSpecs_t *pResolutionSpecs
)
{
    tmErrorCode_t   errCode;
    UInt8           i, find = 0;


    /* Test pResolutionSpecs <> NULL */
    RETIF(pResolutionSpecs == Null,TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* Take the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    for(i = 0; i < gtmdlHdmiRxDriverConfigTable[instance].resolutionSupportedNb; i++)
    {
        if(resolutionID == gtmdlHdmiRxDriverConfigTable[instance].pResolutionInfo[i].resolutionID)
        {
            find = 1;
            pResolutionSpecs->height      = gtmdlHdmiRxDriverConfigTable[instance].pResolutionInfo[i].resolutionSpecs.height;
            pResolutionSpecs->width       = gtmdlHdmiRxDriverConfigTable[instance].pResolutionInfo[i].resolutionSpecs.width;
            pResolutionSpecs->interlaced  = gtmdlHdmiRxDriverConfigTable[instance].pResolutionInfo[i].resolutionSpecs.interlaced;
            pResolutionSpecs->vfrequency  = gtmdlHdmiRxDriverConfigTable[instance].pResolutionInfo[i].resolutionSpecs.vfrequency;
            break;
        }
    }

    if (find == 0)
    {
        tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
        return TMDL_ERR_DLHDMIRX_RESOLUTION_UNKNOWN;
    }

    /* Release the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    return(TM_OK);
}

/******************************************************************************
    \brief Configure the active input to the given resolution.

    \param instance   Instance identifier.
    \param resolution Resolution to set.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMDL_ERR_DLHDMIRX_RESOLUTION_NOT_SUPPORTED: the resolution is
              not supported
            - TMDL_ERR_DLHDMIRX_INVALID_STATE: the state is invalid for
              the function

******************************************************************************/
tmErrorCode_t tmdlHdmiRxConfigureInput /* TBC for TDA19972 */
(
    tmInstance_t             instance,
    tmdlHdmiRxResolutionID_t resolution
)
{
    tmErrorCode_t                       errCode = TM_OK;
    UInt8                               i, find = 0;
    UInt16                              fdwStart;
    UInt16                              fdwEnd;
    tmbslHdmiRxPolarityHSyncOut_t       hSyncPolarity = BSLHDMIRX_POLARITY_HSYNCOUT_POSITIVE;
    tmbslHdmiRxPolarityVSyncOut_t       vSyncPolarity = BSLHDMIRX_POLARITY_VSYNCOUT_POSITIVE;

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* Take the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    /* Check the current state */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], (dlHdmiRxGetState(instance) != STATE_LOCKED) && (dlHdmiRxGetState(instance) != STATE_CONFIGURED), TMDL_ERR_DLHDMIRX_INVALID_STATE)


    for(i = 0; i < gtmdlHdmiRxDriverConfigTable[instance].resolutionSupportedNb; i++)
    {
        if(resolution == gtmdlHdmiRxDriverConfigTable[instance].ptmdlHdmiRxCfgResolutionTimings[i].resolutionID)
        {
            find = 1;
            break;
        }
    }

    if (find == 0)
    {
        tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
        return TMDL_ERR_DLHDMIRX_RESOLUTION_NOT_SUPPORTED;
    }

    switch(resolution)
    {
    case TMDL_HDMIRX_VIDEORES_720_480p_60HZ :       /* 720x480p 60Hz */
    case TMDL_HDMIRX_VIDEORES_720_480i_60HZ :       /* 720x480i 60Hz */
#ifdef SUPPORT_3D_FP
    case TMDL_HDMIRX_VIDEORES_720_480p_60HZ_FP :
#endif
    case TMDL_HDMIRX_VIDEORES_640_480p_60HZ :       /* VGA  640*480p    60HZ*/
    case TMDL_HDMIRX_VIDEORES_1024_768p_60HZ :      /* XGA  1024*768p   60HZ*/
    case TMDL_HDMIRX_VIDEORES_720_240p_60HZ_M1:     /* 720(1440, 2880)x240p 60Hz mode 1 */
    case TMDL_HDMIRX_VIDEORES_720_240p_60HZ_M2:     /* 720(1440, 2880)x240p 60Hz mode 2 */
        hSyncPolarity = BSLHDMIRX_POLARITY_HSYNCOUT_NEGATIVE;
        vSyncPolarity = BSLHDMIRX_POLARITY_VSYNCOUT_NEGATIVE;
        break;

    case TMDL_HDMIRX_VIDEORES_1280_720p_60HZ :      /* 1280x720p 60Hz */
    case TMDL_HDMIRX_VIDEORES_1920_1080i_60HZ :     /* 1920x1080i 60Hz */
    case TMDL_HDMIRX_VIDEORES_1920_1080p_60HZ :     /* 1920x1080p 60Hz */
    case TMDL_HDMIRX_VIDEORES_1080_1920p_60HZ :     /* 1080x1920p 60Hz */ /// added by Sensics
    case TMDL_HDMIRX_VIDEORES_800_600p_60HZ :       /* SVGA 800*600p    60HZ*/
    case TMDL_HDMIRX_VIDEORES_1280_960p_60HZ :      /* ???  1280*960p   60HZ*/
    case TMDL_HDMIRX_VIDEORES_1280_1024p_60HZ :     /* SXGA 1280*1024p  60HZ*/
    case TMDL_HDMIRX_VIDEORES_1360_768p_60HZ:       /* 1360x768p 60Hz (PC resolution) */
    case TMDL_HDMIRX_VIDEORES_1600_1200p_60HZ :     /* UGA 1600*1200p   60HZ*/
    case TMDL_HDMIRX_VIDEORES_1920_1200p_60HZ_RB :     /* WUXGA 1920*1200  60HZ*/
#ifdef SUPPORT_3D_FP
    case TMDL_HDMIRX_VIDEORES_1280_720p_60HZ_FP :
// Following video formats have lineCountNb and VREF_F1_Start values greater than 11-bit registers
// Don't use Blanking codes and Timing code with these video formats
#endif
        hSyncPolarity = BSLHDMIRX_POLARITY_HSYNCOUT_POSITIVE;
        vSyncPolarity = BSLHDMIRX_POLARITY_VSYNCOUT_POSITIVE;
        break;

    case TMDL_HDMIRX_VIDEORES_1400_1050p_60HZ:      /* 1400x1050p 60Hz (PC resolution) */
    case TMDL_HDMIRX_VIDEORES_1400_1050p_60HZ_RB:   /* 1400x1050p 60Hz Reduced Blanking (PC resolution) */
    case TMDL_HDMIRX_VIDEORES_1680_1050p_60HZ_RB :     /* WSXGA 1680*1050p 60HZ reduced blanking (Quantum CVR1660D)*/
        hSyncPolarity = BSLHDMIRX_POLARITY_HSYNCOUT_POSITIVE;
        vSyncPolarity = BSLHDMIRX_POLARITY_VSYNCOUT_NEGATIVE;
        break;
    case TMDL_HDMIRX_VIDEORES_1280_768p_60HZ :      /* WXGA 1024*768p   60HZ*/
    case TMDL_HDMIRX_VIDEORES_1440_900p_60HZ :      /* 1440*900p 60HZ PC resolution) */
        hSyncPolarity = BSLHDMIRX_POLARITY_HSYNCOUT_NEGATIVE;
        vSyncPolarity = BSLHDMIRX_POLARITY_VSYNCOUT_POSITIVE;
        break;

    default :
        /* Unknown resolution */
        hSyncPolarity = BSLHDMIRX_POLARITY_HSYNCOUT_POSITIVE;
        vSyncPolarity = BSLHDMIRX_POLARITY_VSYNCOUT_POSITIVE;
    }

    /* ConfigureFrameDetectionWindow */
    fdwStart = (UInt16) 0x2EF;
    fdwEnd = (UInt16) 0x141;

    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureFrameDetectionWindow(instance,
              fdwStart,
              fdwEnd);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, errCode)

    /* SetPixelAndLineCounters */
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetPixelAndLineCounters(instance,
              gtmdlHdmiRxDriverConfigTable[instance].ptmdlHdmiRxCfgResolutionTimings[i].pixCountPreset,
              gtmdlHdmiRxDriverConfigTable[instance].ptmdlHdmiRxCfgResolutionTimings[i].pixCountNb,
              gtmdlHdmiRxDriverConfigTable[instance].ptmdlHdmiRxCfgResolutionTimings[i].lineCountPreset,
              gtmdlHdmiRxDriverConfigTable[instance].ptmdlHdmiRxCfgResolutionTimings[i].lineCountNb);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, errCode)

    /* ConfigurePixelRepeater */
    /* WA: pixel derepeater is set according to the AVI infoframe */
    //errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigurePixelRepeater(instance,
    //                                        gtmdlHdmiRxDriverConfigTable[instance].ptmdlHdmiRxCfgResolutionTimings[i].pixelRepetition);
    //RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, errCode)

    /* ConfigureVHRef */
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureVHRef(instance,
              BSLHDMIRX_VHREF_INTERLACED_FRAMEFIELD,
              BSLHDMIRX_VHREF_FRAMEVSYNC_AUTO,
              BSLHDMIRX_VHREF_STANDARD_OFF,
              BSLHDMIRX_VHREF_VREF_MANUAL,
              BSLHDMIRX_VHREF_HREF_MANUAL,
              BSLHDMIRX_VHREF_CSYNC_ACTIVE_LOW);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, errCode)

    /* SetVHRefValues */
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetVHRefValues(instance,
              &(gtmdlHdmiRxDriverConfigTable[instance].ptmdlHdmiRxCfgResolutionTimings[i].VHRefValues) );
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, errCode)

    /* ConfigureOutputPolarity */
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureOutputPolarity(instance,
              BSLHDMIRX_POLARITY_DATAENOUT_POSITIVE,
              BSLHDMIRX_POLARITY_CSYNCOUT_NEGATIVE,
              hSyncPolarity,
              vSyncPolarity,
              BSLHDMIRX_POSITIVE_POLARITY_FIELDREFOUT,
              BSLHDMIRX_POLARITY_HREFOUT_POSITIVE,
              BSLHDMIRX_POLARITY_VREFOUT_POSITIVE,
              BSLHDMIRX_AUTO_SYNCPOLARITY);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, errCode)

    /* Set the state machine */
    
    dlHdmiRxSetState(instance, STATE_CONFIGURED);

    /* Release the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    return(TM_OK);
}

/******************************************************************************
    \brief (un)Mute audio or video outputs. This function is typically used
           during input selection/configuration or resolution detection to
           avoid outputting artefacts to the screen/speakers.

    \param instance  Instance identifier.
    \param videoMute Video mute active (True/False).
    \param audioMute Audio mute active (True/False).

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range

******************************************************************************/
tmErrorCode_t tmdlHdmiRxMuteOutput /* TBC for TDA19972 */
(
    tmInstance_t instance,
    Bool         videoMute,
    Bool         audioMute
)
{
    tmErrorCode_t                       errCode = TM_OK;
    tmbslHdmiRxVideoPortFormat_t        videoPortFormat     = BSLHDMIRX_VIDEOPORT_10BITS;
    tmbslHdmiRxVideoPortSelection_t     videoPortASelection = BSLHDMIRX_VIDEOPORT_HIGHIMPEDANCE;
    tmbslHdmiRxVideoPortSelection_t     videoPortBSelection = BSLHDMIRX_VIDEOPORT_HIGHIMPEDANCE;
    tmbslHdmiRxVideoPortSelection_t     videoPortCSelection = BSLHDMIRX_VIDEOPORT_HIGHIMPEDANCE;
    tmbslHdmiRxAudioOutputFormat_t      audioOutputFormat   = BSLHDMIRX_AUDIOOUTPUT_I2S;
    tmbslHdmiRxAudioI2SResolution_t     i2sResolution       = BSLHDMIRX_I2S_RESOLUTION_32_BITS;
    tmbslHdmiRxAudioPacketMode_t        packetMode          = BSLHDMIRX_AUDIO_SAMPLE_PACKET_MODE;
    tmbslHdmiRxAudioClocksSysMode_t     systemClockMode     = BSLHDMIRX_CLOCKS_MODE_256FS_SYSCLKMODE;
    tmbslHdmiRxAudioTestTone_t          testTone            = BSLHDMIRX_TESTTONE_OFF;
    tmbslHdmiRxAudioPathSpFlagMode_t    spFlagMode          = BSLHDMIRX_AUDIOPATH_SP_FLAG_USED_BY_FIFO;
    tmbslHdmiRxHBROutputMode_t          outputModeHBR       = BSLHDMIRX_HBR_STRAIGHT_OUTPUT_MODE;
    tmbslHdmiRxAudioClocksModeRefFreq_t clocksRefFreq       = BSLHDMIRX_CLOCKS_MODE_REFFREQ_27;
    tmbslHdmiRxAudioOutputForce_t       forceAudioPort      = BSLHDMIRX_AUDIOOUTPUT_FORCE_TO_LOW;
    tmbslHdmiRxOutputControl_t          outputControl       = BSLHDMIRX_OUTPUT_ACTIVE;
    tmbslHdmiRxOutputBlankingCodes_t    blkCodes;
    tmbslHdmiRxOutputTimingRefs_t       timingRefs;
    tmbslHdmiRxOutputFormat_t           videoOutputFormat   = BSLHDMIRX_OUTPUT_FORMAT_444;
#ifdef TMFL_TDA19972_FAMILY
    Bool								VideoMuteStateChanged;
    tmdlVPBitsConfig 					VideoPortConfiguration[9];
    UInt8								i;
#endif

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* Take the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

#ifdef TMFL_TDA19972_FAMILY
    /* if videoport not configured and unmute video, return error */
    RETIF((!instanceStatusInfo[instance].pMuteAndVideoPort->videoPortConfigured) && (!videoMute), TMDL_ERR_DLHDMIRX_OPERATION_NOT_PERMITTED);

    /* Check if video mute state changed */
    VideoMuteStateChanged = (instanceStatusInfo[instance].pMuteAndVideoPort->videoMuteState != videoMute);
#endif

    /* Memorize the video mute state */
    instanceStatusInfo[instance].pMuteAndVideoPort->videoMuteState = videoMute;

    /* Memorize the audio mute state */
    instanceStatusInfo[instance].pMuteAndVideoPort->audioMuteState = audioMute;

    switch (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion)
    {
    case TMDL_HDMIRX_DEVICE_TDA19971:
    case TMDL_HDMIRX_DEVICE_TDA19971N2:
        videoPortFormat     = BSLHDMIRX_VIDEOPORT_8BITS;
#ifdef TMFL_TDA19972_FAMILY
        /* init VideoPortConfiguration with all ports muted*/
        VideoPortConfiguration[0].pinGroup = TMDL_HDMI_RX_VP24_G4_3_0;
        VideoPortConfiguration[0].colorQuartet = TMDL_HDMI_RX_LOZ;
        VideoPortConfiguration[1].pinGroup = TMDL_HDMI_RX_VP24_G4_7_4;
        VideoPortConfiguration[1].colorQuartet = TMDL_HDMI_RX_LOZ;
        VideoPortConfiguration[2].pinGroup = TMDL_HDMI_RX_VP24_G4_11_8;
        VideoPortConfiguration[2].colorQuartet = TMDL_HDMI_RX_LOZ;
        VideoPortConfiguration[3].pinGroup = TMDL_HDMI_RX_VP24_G4_15_12;
        VideoPortConfiguration[3].colorQuartet = TMDL_HDMI_RX_LOZ;
        VideoPortConfiguration[4].pinGroup = TMDL_HDMI_RX_VP24_G4_19_16;
        VideoPortConfiguration[4].colorQuartet = TMDL_HDMI_RX_LOZ;
        VideoPortConfiguration[5].pinGroup = TMDL_HDMI_RX_VP24_G4_23_20;
        VideoPortConfiguration[5].colorQuartet = TMDL_HDMI_RX_LOZ;
#endif
        break;

    case TMDL_HDMIRX_DEVICE_TDA19973_SOC_IN:
    case TMDL_HDMIRX_DEVICE_TDA19973_CONNECTOR_IN:
    case TMDL_HDMIRX_DEVICE_TDA19973N2_SOC_IN:
    case TMDL_HDMIRX_DEVICE_TDA19973N2_CONNECTOR_IN:
        videoPortFormat     = BSLHDMIRX_VIDEOPORT_12BITS;
#ifdef TMFL_TDA19972_FAMILY
        /* init VideoPortConfiguration with all ports muted*/
        VideoPortConfiguration[0].pinGroup = TMDL_HDMI_RX_VP36_G4_3_0;
        VideoPortConfiguration[0].colorQuartet = TMDL_HDMI_RX_LOZ;
        VideoPortConfiguration[1].pinGroup = TMDL_HDMI_RX_VP36_G4_7_4;
        VideoPortConfiguration[1].colorQuartet = TMDL_HDMI_RX_LOZ;
        VideoPortConfiguration[2].pinGroup = TMDL_HDMI_RX_VP36_G4_11_8;
        VideoPortConfiguration[2].colorQuartet = TMDL_HDMI_RX_LOZ;
        VideoPortConfiguration[3].pinGroup = TMDL_HDMI_RX_VP36_G4_15_12;
        VideoPortConfiguration[3].colorQuartet = TMDL_HDMI_RX_LOZ;
        VideoPortConfiguration[4].pinGroup = TMDL_HDMI_RX_VP36_G4_19_16;
        VideoPortConfiguration[4].colorQuartet = TMDL_HDMI_RX_LOZ;
        VideoPortConfiguration[5].pinGroup = TMDL_HDMI_RX_VP36_G4_23_20;
        VideoPortConfiguration[5].colorQuartet = TMDL_HDMI_RX_LOZ;
        VideoPortConfiguration[6].pinGroup = TMDL_HDMI_RX_VP36_G4_27_24;
        VideoPortConfiguration[6].colorQuartet = TMDL_HDMI_RX_LOZ;
        VideoPortConfiguration[7].pinGroup = TMDL_HDMI_RX_VP36_G4_31_28;
        VideoPortConfiguration[7].colorQuartet = TMDL_HDMI_RX_LOZ;
        VideoPortConfiguration[8].pinGroup = TMDL_HDMI_RX_VP36_G4_35_32;
        VideoPortConfiguration[8].colorQuartet = TMDL_HDMI_RX_LOZ;
#endif
        break;

    case TMDL_HDMIRX_DEVICE_TDA19978:
        videoPortFormat     = BSLHDMIRX_VIDEOPORT_10BITS;
        break;

    default:
        videoPortFormat     = BSLHDMIRX_VIDEOPORT_10BITS;
        break;
    }

    /* Test on video mute parameter */
    if(!videoMute)
    {
        /* Video mute off */

        /* Test the video format */
        switch(instanceStatusInfo[instance].pMuteAndVideoPort->videoportResolution)
        {
        case TMDL_HDMIRX_VPRESOLUTION_8_BITS:
            /* 8 bits */
            videoPortFormat = BSLHDMIRX_VIDEOPORT_8BITS;
            break;

        case TMDL_HDMIRX_VPRESOLUTION_10_BITS:
            /* 10 bits */
            videoPortFormat = BSLHDMIRX_VIDEOPORT_10BITS;
            break;

        case TMDL_HDMIRX_VPRESOLUTION_12_BITS:
            /* 12 bits */
            videoPortFormat = BSLHDMIRX_VIDEOPORT_12BITS;
            break;

        default:
            /* Release the semaphore */
            errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
            RETIF(errCode != TM_OK, errCode)
            return TMDL_ERR_DLHDMIRX_BAD_PARAMETER;
        }

        for (i=0; i<9; i++)
        {
            VideoPortConfiguration[i].pinGroup= instanceStatusInfo[instance].pMuteAndVideoPort->VPConfig[i].pinGroup;
            VideoPortConfiguration[i].colorQuartet= instanceStatusInfo[instance].pMuteAndVideoPort->VPConfig[i].colorQuartet;
        }
    }

    if(!audioMute)
    {
        /* Audio mute off */

        /* Test tone must be enabled if video input is test color bar */
#ifdef TMFL_TDA19972_FAMILY
        if ((instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_480P) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_480I) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_576I) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_480P) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_576P) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_480I) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_576I) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_576P))
#endif /* TMFL_TDA19972_FAMILY */
        {
            testTone = BSLHDMIRX_TESTTONE_ON;
        }

        forceAudioPort = BSLHDMIRX_AUDIOOUTPUT_NORMAL;

        /* Test the audio format */
        switch(instanceStatusInfo[instance].pMuteAndVideoPort->audioFormat)
        {
        case TMDL_HDMIRX_AUDIOFORMAT_I2S16:
            /* I2S 16 bits */
            audioOutputFormat  = BSLHDMIRX_AUDIOOUTPUT_I2S;
            i2sResolution      = BSLHDMIRX_I2S_RESOLUTION_16_BITS;
            packetMode         = BSLHDMIRX_AUDIO_SAMPLE_PACKET_MODE;
            outputModeHBR      = BSLHDMIRX_HBR_STRAIGHT_OUTPUT_MODE;
            spFlagMode         = BSLHDMIRX_AUDIOPATH_SP_FLAG_USED_BY_FIFO;
            break;

        case TMDL_HDMIRX_AUDIOFORMAT_I2S32:
            /* I2S 32 bits */
            audioOutputFormat  = BSLHDMIRX_AUDIOOUTPUT_I2S;
            i2sResolution      = BSLHDMIRX_I2S_RESOLUTION_32_BITS;
            packetMode         = BSLHDMIRX_AUDIO_SAMPLE_PACKET_MODE;
            outputModeHBR      = BSLHDMIRX_HBR_STRAIGHT_OUTPUT_MODE;
            spFlagMode         = BSLHDMIRX_AUDIOPATH_SP_FLAG_USED_BY_FIFO;
            break;

        case TMDL_HDMIRX_AUDIOFORMAT_SPDIF:
            /* SPDIF */
            audioOutputFormat  = BSLHDMIRX_AUDIOOUTPUT_SPDIF;
            i2sResolution      = BSLHDMIRX_I2S_RESOLUTION_32_BITS;
            packetMode         = BSLHDMIRX_AUDIO_SAMPLE_PACKET_MODE;
            outputModeHBR      = BSLHDMIRX_HBR_STRAIGHT_OUTPUT_MODE;
            spFlagMode         = BSLHDMIRX_AUDIOPATH_SP_FLAG_USED_BY_FIFO;
            break;

        case TMDL_HDMIRX_AUDIOFORMAT_OBA:
            /* One Bit Audio */
            audioOutputFormat  = BSLHDMIRX_AUDIOOUTPUT_I2S;
            i2sResolution      = BSLHDMIRX_I2S_RESOLUTION_32_BITS;
            packetMode         = BSLHDMIRX_ONE_BIT_AUDIO_PACKET_MODE;
            outputModeHBR      = BSLHDMIRX_HBR_STRAIGHT_OUTPUT_MODE;
            spFlagMode         = BSLHDMIRX_AUDIOPATH_SP_FLAG_USED_BY_FIFO;
            break;

        case TMDL_HDMIRX_AUDIOFORMAT_I2S16_HBR_DEMUX:
            /* I2S 16 bits High Bit Rate demux */
            audioOutputFormat  = BSLHDMIRX_AUDIOOUTPUT_I2S;
            i2sResolution      = BSLHDMIRX_I2S_RESOLUTION_16_BITS;
            packetMode         = BSLHDMIRX_HBR_AUDIO_PACKET_MODE;
            outputModeHBR      = BSLHDMIRX_HBR_DEMUX_OUTPUT_MODE;
            spFlagMode         = BSLHDMIRX_AUDIOPATH_SP_FLAG_IGNORED_BY_FIFO;
            break;

        case TMDL_HDMIRX_AUDIOFORMAT_I2S32_HBR_DEMUX:
            /* I2S 32 bits High Bit Rate demux */
            audioOutputFormat  = BSLHDMIRX_AUDIOOUTPUT_I2S;
            i2sResolution      = BSLHDMIRX_I2S_RESOLUTION_32_BITS;
            packetMode         = BSLHDMIRX_HBR_AUDIO_PACKET_MODE;
            outputModeHBR      = BSLHDMIRX_HBR_DEMUX_OUTPUT_MODE;
            spFlagMode         = BSLHDMIRX_AUDIOPATH_SP_FLAG_IGNORED_BY_FIFO;
            break;

        case TMDL_HDMIRX_AUDIOFORMAT_SPDIF_HBR_DEMUX:
            /* SPDIF High Bit Rate demux */
            audioOutputFormat  = BSLHDMIRX_AUDIOOUTPUT_SPDIF;
            i2sResolution      = BSLHDMIRX_I2S_RESOLUTION_32_BITS;
            packetMode         = BSLHDMIRX_HBR_AUDIO_PACKET_MODE;
            outputModeHBR      = BSLHDMIRX_HBR_DEMUX_OUTPUT_MODE;
            spFlagMode         = BSLHDMIRX_AUDIOPATH_SP_FLAG_IGNORED_BY_FIFO;
            break;

        case TMDL_HDMIRX_AUDIOFORMAT_DST:
            /* Direct Stream Transfer */
            audioOutputFormat  = BSLHDMIRX_AUDIOOUTPUT_I2S;
            i2sResolution      = BSLHDMIRX_I2S_RESOLUTION_32_BITS;
            packetMode         = BSLHDMIRX_DST_AUDIO_PACKET_MODE;
            outputModeHBR      = BSLHDMIRX_HBR_STRAIGHT_OUTPUT_MODE;
            spFlagMode         = BSLHDMIRX_AUDIOPATH_SP_FLAG_IGNORED_BY_FIFO;
            break;

        default:
            /* Release the semaphore */
            errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
            RETIF(errCode != TM_OK, errCode)
            return TMDL_ERR_DLHDMIRX_BAD_PARAMETER;
        }
    }

    /* Test the audio system clock parameter */
    switch(instanceStatusInfo[instance].pMuteAndVideoPort->audioSysClk)
    {
    case TMDL_HDMIRX_AUDIOSYSCLK_128FS:
        /* Multiplication factor Fs * 128 */
        systemClockMode = BSLHDMIRX_CLOCKS_MODE_128FS_SYSCLKMODE;
        break;

    case TMDL_HDMIRX_AUDIOSYSCLK_256FS:
        /* Multiplication factor Fs * 256 */
        systemClockMode = BSLHDMIRX_CLOCKS_MODE_256FS_SYSCLKMODE;
        break;

    case TMDL_HDMIRX_AUDIOSYSCLK_512FS:
        /* Multiplication factor Fs * 512 */
        systemClockMode = BSLHDMIRX_CLOCKS_MODE_512FS_SYSCLKMODE;
        break;

    default:
        /* Release the semaphore */
        errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
        RETIF(errCode != TM_OK, errCode)
        return TMDL_ERR_DLHDMIRX_BAD_PARAMETER;
    }

    /* Configure the reference frequency */
    clocksRefFreq = BSLHDMIRX_CLOCKS_MODE_REFFREQ_27;


    /* Test on the video and audio mute state */
    if(videoMute && audioMute)
    {
        outputControl = BSLHDMIRX_OUTPUT_HIGHIMPEDANCE;
    }

    /* Test the blanking codes status */
    if(instanceStatusInfo[instance].pVideoInfo->blankingCodeInsert)
    {
        /* Insert the blanking codes */
        blkCodes = BSLHDMIRX_OUTPUT_BLANKING_INSERT;
    }
    else
    {
        /* Remove the blanking codes */
        blkCodes = BSLHDMIRX_OUTPUT_BLANKING_REMOVE;
    }

    /* Test the timing codes status */
    if(instanceStatusInfo[instance].pVideoInfo->timingCodeInsert)
    {
        /* Insert the timing reference codes */
        timingRefs = BSLHDMIRX_OUTPUT_TIMINGREF_INSERT;
    }
    else
    {
        /* Remove the timing reference codes */
        timingRefs = BSLHDMIRX_OUTPUT_TIMINGREF_REMOVE;
    }

    /* Test the video format */
    switch(instanceStatusInfo[instance].pVideoInfo->videoFormat)
    {
    case TMDL_HDMIRX_OUTPUTFORMAT_444:
        /* 4:4:4 format */
        videoOutputFormat = BSLHDMIRX_OUTPUT_FORMAT_444;
        break;

    case TMDL_HDMIRX_OUTPUTFORMAT_422_SMP:
        /* 4:2:2 semi-planar format */
        videoOutputFormat = BSLHDMIRX_OUTPUT_FORMAT_422_SMPT;
        break;

    case TMDL_HDMIRX_OUTPUTFORMAT_422_CCIR:
        /* 4:2:2 CCIR 656 format */
        videoOutputFormat = BSLHDMIRX_OUTPUT_FORMAT_422_CCIR;
        break;

    default:
        /* Release the semaphore */
        errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
        RETIF(errCode != TM_OK, errCode)
        return TMDL_ERR_DLHDMIRX_BAD_PARAMETER;
    }

    /* ConfigureVideoOutput */
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureVideoOutput(instance,
              outputControl,
              BSLHDMIRX_OUTPUT_VIDEO_LOW,
              BSLHDMIRX_OUTPUT_VHREF_ENABLE,
              blkCodes,
              timingRefs,
              videoOutputFormat);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, errCode)

    /* ConfigureVideoPort */
    if (VideoMuteStateChanged)
    {
        errCode = dlHdmiRxConfigVideoPort(instance, VideoPortConfiguration, &i);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, errCode)
    }

    /* ConfigureAudioFormatter */
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureAudioFormatter(instance,
              spFlagMode,
              instanceStatusInfo[instance].pChannelAssignmentInfo->currentChannelAssignment,
              BSLHDMIRX_AUDIOPATH_LAYOUT_PACKET_HEADER,
              BSLHDMIRX_AUDIO_LAYOUT_FORCED_0,
              FIFO_LATENCY, testTone,
              audioOutputFormat,
              forceAudioPort,
              forceAudioPort,
              forceAudioPort,
              forceAudioPort,
              forceAudioPort,
              forceAudioPort,
              forceAudioPort,
              forceAudioPort,
              forceAudioPort,
              forceAudioPort);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, errCode)

    /* ConfigureAudioSelection */
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureAudioSelection(instance,
              packetMode,
              BSLHDMIRX_POSITIVE_ONLY_AUDIOPLL_REF,
              i2sResolution,
              BSLHDMIRX_AUDIOPLL_SELECTION_NORMAL,
              outputModeHBR);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, errCode)

    /* AudioAutoMute TBD put mute on*/
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxAudioAutoMute(instance,
              BSLHDMIRX_AUDIO_AUTOMUTE_OFF);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, errCode)

    /* ConfigureAudioClkMode */
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureAudioClkMode(instance,
              BSLHDMIRX_CLOCKS_MODE_NORMAL,
              BSLHDMIRX_CLOCKS_MODE_REFMODE_XTAL,
              clocksRefFreq,
              BSLHDMIRX_CLOCKS_MODE_128FS_AUDIOMODE,
              systemClockMode);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, errCode)

    /* Release the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    return(TM_OK);
}

/******************************************************************************
    \brief Set color input. This function tells the driver what type of color
           space is currently at input. This allows setting internally the
           correct clamping, gain and limit values.

    \param instance   Instance identifier.
    \param colorSpace Color space currently received at input.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range

******************************************************************************/
tmErrorCode_t tmdlHdmiRxSetColorSpaceInput
(
    tmInstance_t           instance,
    tmdlHdmiRxColorSpace_t colorSpace
)
{
    DUMMY_ACCESS(instance);
    DUMMY_ACCESS(colorSpace);

    return(TM_OK);
}

/******************************************************************************
    \brief Set color conversion matrix. This function allows setting the color
           conversion matrix parameters, including bypass.

    \param instance        Instance identifier.
    \param colorMatrixMode Color conversion matrix mode : bypass, predefined
                           or custom.
    \param pMatrix         Pointer to the structure containing matrix
                           coefficients. Only relevant in "custom" mode
                           (the pointer must be different from Null in
                           custom mode).
    \param pBlankingCodes  Pointer to the structure containing the blanking
                           codes (if the pointer is Null, the blanking codes
                           are not set).

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

******************************************************************************/
tmErrorCode_t tmdlHdmiRxSetColorConversionMatrix
(
    tmInstance_t                  instance,
    tmdlHdmiRxColorMatrixMode     colorMatrixMode,
    tmdlHdmiRxColorMatrixCoefs_t  *pMatrix,
    tmdlHdmiRxBlankingCodes_t     *pBlankingCodes
)
{
    tmErrorCode_t                        errCode = TM_OK;
    tmbslHdmiRxColorSpaceBypass_t        bypass = BSLHDMIRX_COLORSP_MXBYPASS_OFF;
    tmbslHdmiRxColorSpaceCoefficients_t  matrix;


    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* Take the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    /* Test the color matrix mode */
    switch(colorMatrixMode)
    {
    case TMDL_HDMIRX_CONVERSIONMATRIX_BYPASS:
        /* Conversion bypassed */
        bypass  = BSLHDMIRX_COLORSP_MXBYPASS_ON;
        break;

    case TMDL_HDMIRX_CONVERSIONMATRIX_CUSTOM:
        /* Test pMatrix <> NULL */
        RETIF(pMatrix == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)

        /* Custom conversion */
        bypass         = BSLHDMIRX_COLORSP_MXBYPASS_OFF;
        matrix.offInt1 = pMatrix->offInt1;
        matrix.offInt2 = pMatrix->offInt2;
        matrix.offInt3 = pMatrix->offInt3;
        matrix.P11Coef = pMatrix->P11Coef;
        matrix.P12Coef = pMatrix->P12Coef;
        matrix.P13Coef = pMatrix->P13Coef;
        matrix.P21Coef = pMatrix->P21Coef;
        matrix.P22Coef = pMatrix->P22Coef;
        matrix.P23Coef = pMatrix->P23Coef;
        matrix.P31Coef = pMatrix->P31Coef;
        matrix.P32Coef = pMatrix->P32Coef;
        matrix.P33Coef = pMatrix->P33Coef;
        matrix.offOut1 = pMatrix->offOut1;
        matrix.offOut2 = pMatrix->offOut2;
        matrix.offOut3 = pMatrix->offOut3;
        break;

    case TMDL_HDMIRX_CONVERSIONMATRIX_PREDEFINED_1:
        /* Test the number of predefined matrix */
        RETIF(gtmdlHdmiRxDriverConfigTable[instance].predefinedColorMatrixNb < 1, TMDL_ERR_DLHDMIRX_BAD_PARAMETER)

        /* Predefined conversion n?1 */
        bypass         = BSLHDMIRX_COLORSP_MXBYPASS_OFF;
        matrix.offInt1 = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[0].offInt1;
        matrix.offInt2 = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[0].offInt2;
        matrix.offInt3 = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[0].offInt3;
        matrix.P11Coef = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[0].P11Coef;
        matrix.P12Coef = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[0].P12Coef;
        matrix.P13Coef = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[0].P13Coef;
        matrix.P21Coef = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[0].P21Coef;
        matrix.P22Coef = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[0].P22Coef;
        matrix.P23Coef = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[0].P23Coef;
        matrix.P31Coef = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[0].P31Coef;
        matrix.P32Coef = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[0].P32Coef;
        matrix.P33Coef = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[0].P33Coef;
        matrix.offOut1 = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[0].offOut1;
        matrix.offOut2 = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[0].offOut2;
        matrix.offOut3 = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[0].offOut3;
        break;

    case TMDL_HDMIRX_CONVERSIONMATRIX_PREDEFINED_2:
        /* Test the number of predefined matrix */
        RETIF(gtmdlHdmiRxDriverConfigTable[instance].predefinedColorMatrixNb < 2, TMDL_ERR_DLHDMIRX_BAD_PARAMETER)

        /* Predefined conversion n?2 */
        bypass         = BSLHDMIRX_COLORSP_MXBYPASS_OFF;
        matrix.offInt1 = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[1].offInt1;
        matrix.offInt2 = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[1].offInt2;
        matrix.offInt3 = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[1].offInt3;
        matrix.P11Coef = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[1].P11Coef;
        matrix.P12Coef = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[1].P12Coef;
        matrix.P13Coef = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[1].P13Coef;
        matrix.P21Coef = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[1].P21Coef;
        matrix.P22Coef = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[1].P22Coef;
        matrix.P23Coef = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[1].P23Coef;
        matrix.P31Coef = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[1].P31Coef;
        matrix.P32Coef = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[1].P32Coef;
        matrix.P33Coef = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[1].P33Coef;
        matrix.offOut1 = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[1].offOut1;
        matrix.offOut2 = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[1].offOut2;
        matrix.offOut3 = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[1].offOut3;
        break;

    case TMDL_HDMIRX_CONVERSIONMATRIX_PREDEFINED_3:
        /* Test the number of predefined matrix */
        RETIF(gtmdlHdmiRxDriverConfigTable[instance].predefinedColorMatrixNb < 3, TMDL_ERR_DLHDMIRX_BAD_PARAMETER)

        /* Predefined conversion n?3 */
        bypass         = BSLHDMIRX_COLORSP_MXBYPASS_OFF;
        matrix.offInt1 = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[2].offInt1;
        matrix.offInt2 = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[2].offInt2;
        matrix.offInt3 = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[2].offInt3;
        matrix.P11Coef = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[2].P11Coef;
        matrix.P12Coef = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[2].P12Coef;
        matrix.P13Coef = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[2].P13Coef;
        matrix.P21Coef = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[2].P21Coef;
        matrix.P22Coef = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[2].P22Coef;
        matrix.P23Coef = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[2].P23Coef;
        matrix.P31Coef = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[2].P31Coef;
        matrix.P32Coef = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[2].P32Coef;
        matrix.P33Coef = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[2].P33Coef;
        matrix.offOut1 = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[2].offOut1;
        matrix.offOut2 = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[2].offOut2;
        matrix.offOut3 = gtmdlHdmiRxDriverConfigTable[instance].pPredefinedColorMatrixList[2].offOut3;
        break;

    default:
        /* Release the semaphore */
        errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
        RETIF(errCode != TM_OK, errCode)
        return TMDL_ERR_DLHDMIRX_BAD_PARAMETER;
    }

    /* ConfigureColorSpaceConversion */
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureColorSpaceConversion(instance,
              bypass, &matrix);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)

    /* SetBlankingCodes */
    if(pBlankingCodes != Null)
    {
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetBlankingCodes(instance,
                  pBlankingCodes->blankingCodeGy,
                  pBlankingCodes->blankingCodeBu,
                  pBlankingCodes->blankingCodeRv);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)
    }

    /* Release the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    return(TM_OK);
}

/******************************************************************************
    \brief Set video output format. This function allows setting the general
           parameters of the output signal.

    \param instance       Instance identifier.
    \param videoFormat    Desired output format.
    \param timingCodes    Insert timing codes (True/False).
    \param blankingCodes  Insert blanking codes (True/False).
    \param clockMode      Choose single or double edge sampling.


    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range

******************************************************************************/
tmErrorCode_t tmdlHdmiRxSetVideoOutputFormat
(
    tmInstance_t             instance,
    tmdlHdmiRxVideoFormat_t  videoFormat,
    Bool                     timingCodes,
    Bool                     blankingCodes,
    tmdlHdmiRxOutClkMode_t   clockMode

)
{
    tmErrorCode_t                        errCode = TM_OK;
    tmbslHdmiRxOutputBlankingCodes_t     blkCodes;
    tmbslHdmiRxOutputTimingRefs_t        timingRefs;
    tmbslHdmiRxOutputFormat_t            outputFormat = BSLHDMIRX_OUTPUT_FORMAT_444;
    tmbslHdmiRxPixClkOutSelection_t      clkOutSelect = BSLHDMIRX_PIXCLK_OUT_HDMICLOCK;
    tmbslHdmiRxPixClkForSelection_t      clkForSelect = BSLHDMIRX_PIXCLK_FOR_ZERO;
    tmbslHdmiRxPixClkPixSelection_t      clkPixSelect = BSLHDMIRX_PIXCLK_PIX_HDMICLOCK;
    tmbslHdmiRxPreFilterConfig_t         filterConfig = BSLHDMIRX_PREFILTER_OFF;
    tmbslHdmiRxInputSelClockPulseDelay_t clockPulseDelay;
#ifdef TMFL_TDA19972_FAMILY
    tmbslHdmiRxOutClockEdgeMode_t     	 clockOutEdgeMode;
#endif
    UInt8                                dataEnDelay;
    tmbslHdmiRxOutputControl_t           outputControl;

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* Take the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    /* Memorize the video format and the clock mode */
    instanceStatusInfo[instance].pVideoInfo->videoFormat        = videoFormat;
    instanceStatusInfo[instance].pVideoInfo->outputClockMode    = clockMode;
    instanceStatusInfo[instance].pVideoInfo->timingCodeInsert   = timingCodes;
    instanceStatusInfo[instance].pVideoInfo->blankingCodeInsert = blankingCodes;

#ifdef TMFL_TDA19972_FAMILY
    /* type conversion */
    if (clockMode == TMDL_HDMIRX_OUTCLKMODE_SINGLE_EDGE)
    {
        clockOutEdgeMode = TMDL_HDMIRX_OUTCLKMODE_SINGLE_EDGE;
    }
    else
    {
        clockOutEdgeMode = TMDL_HDMIRX_OUTCLKMODE_DOUBLE_EDGE;

    }
#endif

    /* Test the video format */
    switch(videoFormat)
    {
    case TMDL_HDMIRX_OUTPUTFORMAT_444:
        /* 4:4:4 format */
        outputFormat = BSLHDMIRX_OUTPUT_FORMAT_444;
        filterConfig = BSLHDMIRX_PREFILTER_OFF;

        if( (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_HDMI_A) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_HDMI_B) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_480I) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_480P) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_480P) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_576P) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_480I) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_576I) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_576I) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_576P)
          )
        {
            clkOutSelect = BSLHDMIRX_PIXCLK_OUT_HDMICLOCK;
            clkForSelect = BSLHDMIRX_PIXCLK_FOR_ZERO;
            clkPixSelect = BSLHDMIRX_PIXCLK_PIX_HDMICLOCK;
        }
        break;

    case TMDL_HDMIRX_OUTPUTFORMAT_422_SMP:
        /* 4:2:2 semi-planar format */
        outputFormat = BSLHDMIRX_OUTPUT_FORMAT_422_SMPT;
        filterConfig = BSLHDMIRX_PREFILTER_27TAPS;

        if( (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_HDMI_A) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_HDMI_B) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_480I) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_480P) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_480P) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_576P) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_480I) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_576I) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_576I) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_576P)
          )
        {
            clkOutSelect = BSLHDMIRX_PIXCLK_OUT_HDMICLOCK;
            clkForSelect = BSLHDMIRX_PIXCLK_FOR_ZERO;
            clkPixSelect = BSLHDMIRX_PIXCLK_PIX_HDMICLOCK;
        }
        break;

    case TMDL_HDMIRX_OUTPUTFORMAT_422_CCIR:
        /* 4:2:2 CCIR 656 format */
        outputFormat = BSLHDMIRX_OUTPUT_FORMAT_422_CCIR;
        filterConfig = BSLHDMIRX_PREFILTER_27TAPS;

        /* Check the memorized video input */
        if( (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_HDMI_A) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_HDMI_B) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_480I) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_480P) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_480P) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_576P) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_480I) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_576I) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_576I) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_576P)
          )
        {
            switch (clockMode)
            {
            case TMDL_HDMIRX_OUTCLKMODE_SINGLE_EDGE:
                clkOutSelect = BSLHDMIRX_PIXCLK_OUT_HDMICLOCK_X2;
                break;

            case TMDL_HDMIRX_OUTCLKMODE_DOUBLE_EDGE:
                clkOutSelect = BSLHDMIRX_PIXCLK_OUT_HDMICLOCK;
                break;

            default:
                return TMDL_ERR_DLHDMIRX_BAD_PARAMETER;
            }
            clkForSelect = BSLHDMIRX_PIXCLK_FOR_CLOCKHDMI_X2;
            clkPixSelect = BSLHDMIRX_PIXCLK_PIX_HDMICLOCK;
        }
        break;

    default:
        /* Release the semaphore */
        errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
        RETIF(errCode != TM_OK, errCode)
        return TMDL_ERR_DLHDMIRX_BAD_PARAMETER;
    }

    /* Test on the video and audio mute state */
    if( (instanceStatusInfo[instance].pMuteAndVideoPort->videoMuteState) &&
            (instanceStatusInfo[instance].pMuteAndVideoPort->audioMuteState) )
    {
        //outputControl = BSLHDMIRX_OUTPUT_HIGHIMPEDANCE;
        outputControl = BSLHDMIRX_OUTPUT_ACTIVE;
    }
    else
    {
        outputControl = BSLHDMIRX_OUTPUT_ACTIVE;
    }


    /* Test the blankingCodes parameter */
    if(blankingCodes)
    {
#ifdef TMFL_TDA19972_FAMILY
        /* check that blanking codes are inserted WA for TDA19972*/
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance],(timingCodes == False), TMDL_ERR_DLHDMIRX_BAD_PARAMETER)
#endif

        /* Insert the blanking codes */
        blkCodes = BSLHDMIRX_OUTPUT_BLANKING_INSERT;
    }
    else
    {
        /* Remove the blanking codes */
        blkCodes = BSLHDMIRX_OUTPUT_BLANKING_REMOVE;
    }

    /* Test the timingCodes parameter */
    if(timingCodes)
    {
#ifdef TMFL_TDA19972_FAMILY
        /* check that blanking codes are inserted (not a WA)*/
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance],(blkCodes == BSLHDMIRX_OUTPUT_BLANKING_REMOVE), TMDL_ERR_DLHDMIRX_BAD_PARAMETER)
#endif

        /* Insert the timing reference codes */
        timingRefs = BSLHDMIRX_OUTPUT_TIMINGREF_INSERT;
    }
    else
    {
        /* Remove the timing reference codes */
        timingRefs = BSLHDMIRX_OUTPUT_TIMINGREF_REMOVE;
    }

    /* ConfigurePreFilter */
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigurePreFilter(instance,
              filterConfig,
              filterConfig);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, errCode)

    /* ConfigureVideoOutput */
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureVideoOutput(instance,
              outputControl,
              BSLHDMIRX_OUTPUT_VIDEO_LOW,
              BSLHDMIRX_OUTPUT_VHREF_ENABLE,
              blkCodes,
              timingRefs,
              outputFormat);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, errCode)

    /* ConfigurePixelClockGenerator */
#ifdef TMFL_TDA19972_FAMILY
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigurePixelClockGenerator(instance,
              BSLHDMIRX_PIXCLK_OUT_TOGGLE_OFF,
              outputFormat,
              clockOutEdgeMode);
#endif
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, errCode)

    if(gtmdlHdmiRxDriverConfigTable[instance].clockPulseDelay)
    {
        clockPulseDelay = BSLHDMIRX_CLOCK_PULSE_DELAY_ON;
    }
    else
    {
        clockPulseDelay = BSLHDMIRX_CLOCK_PULSE_DELAY_OFF;
    }

    /* SetClockPulseDelay */
#ifdef TMFL_TDA19972_FAMILY
    /* WA for tda19973: in case of dual_edge, force delay = 5 and toggle clock */
    if (clockMode == TMDL_HDMIRX_OUTCLKMODE_DOUBLE_EDGE)
    {
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigurePixelClockGenerator(instance,
                  BSLHDMIRX_PIXCLK_OUT_TOGGLE_ON,
                  outputFormat,
                  clockOutEdgeMode);
        if ((gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion == TMDL_HDMIRX_DEVICE_TDA19971) ||
                (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion == TMDL_HDMIRX_DEVICE_TDA19973_SOC_IN) ||
                (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion == TMDL_HDMIRX_DEVICE_TDA19973_CONNECTOR_IN))
        {
            /* set delay to 5 for N1 chips */
            errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetClockPulseDelay(instance,
                       clockPulseDelay,
                       5);
        }
        else
        {
            /* set delay to 0 for N2 */
            errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetClockPulseDelay(instance,
                       clockPulseDelay,
                       0);
        }
    }
    else
    {
        /* use value defined in cfg file except for N1 (forced to 0)*/
        if ((gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion == TMDL_HDMIRX_DEVICE_TDA19971) ||
                (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion == TMDL_HDMIRX_DEVICE_TDA19973_SOC_IN) ||
                (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion == TMDL_HDMIRX_DEVICE_TDA19973_CONNECTOR_IN))
        {
            errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetClockPulseDelay(instance,
                      clockPulseDelay,
                      0);
        }
        else
        {
            errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetClockPulseDelay(instance,
                      clockPulseDelay,
                      (UInt8) gtmdlHdmiRxDriverConfigTable[instance].clockPulseDelayValue);
        }
    }
#else
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetClockPulseDelay(instance,
              clockPulseDelay,
              (UInt8) gtmdlHdmiRxDriverConfigTable[instance].clockPulseDelayValue);
#endif
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], (errCode !=TM_OK) && (errCode != TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED), errCode)
    /* SetVHSyncDelay */
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetVHSyncDelay(instance,
              (UInt8) gtmdlHdmiRxDriverConfigTable[instance].vsSyncDelay,
              (UInt8) gtmdlHdmiRxDriverConfigTable[instance].hsSyncDelay);

#ifdef TMFL_TDA19972_FAMILY
    /* for TDA19972, if timing/blanking codes are inserted, force embedded sync*/

    if (timingRefs == BSLHDMIRX_OUTPUT_TIMINGREF_INSERT)
    {

        /* ConfigureSyncOutput */
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureSyncOutput(instance,
                  BSLHDMIRX_SYNCOUTPUT_CS_HS_VS,
                  BSLHDMIRX_SYNCOUTPUT_VREF_VHREF,
                  BSLHDMIRX_SYNCOUTPUT_HREF_VHREF);
        /* ConfigureOutputPolarity */
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureOutputPolarity(instance,
                  BSLHDMIRX_POLARITY_DATAENOUT_POSITIVE,
                  BSLHDMIRX_POLARITY_CSYNCOUT_NEGATIVE,
                  BSLHDMIRX_POLARITY_HSYNCOUT_POSITIVE,
                  BSLHDMIRX_POLARITY_VSYNCOUT_POSITIVE,
                  BSLHDMIRX_POSITIVE_POLARITY_FIELDREFOUT,
                  BSLHDMIRX_POLARITY_HREFOUT_POSITIVE,
                  BSLHDMIRX_POLARITY_VREFOUT_POSITIVE,
                  BSLHDMIRX_AUTO_SYNCPOLARITY);
        dataEnDelay = (UInt8) DE_DELAY;
        errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureDataEnable(instance,
                   BSLHDMIRX_HREF_EXPAND_OFF,
                   BSLHDMIRX_DATAEN_HREF_DONTFORCE_LOW,
                   BSLHDMIRX_DATAEN_EXPAND_OFF,
                   BSLHDMIRX_DATAEN_FREF_VHREF,
                   dataEnDelay);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, errCode)
    }
    else
    {
        /* ConfigureSyncOutput */
        if ((instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_480P) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_480I) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_576I) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_480P) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_576P) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_480I) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_576I) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_576P))

        {
            errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureSyncOutput(instance,
                      BSLHDMIRX_SYNCOUTPUT_CS_HS_VS,
                      BSLHDMIRX_SYNCOUTPUT_VSYNC_VHREF,
                      BSLHDMIRX_SYNCOUTPUT_HSYNC_VHREF);
        }
        else
        {
            errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureSyncOutput(instance,
                      BSLHDMIRX_SYNCOUTPUT_CS_HS_VS,
                      BSLHDMIRX_SYNCOUTPUT_VSYNC_HDMI,
                      BSLHDMIRX_SYNCOUTPUT_HSYNC_HDMI);
        }
        dataEnDelay = (UInt8) DE_DELAY;
        errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureDataEnable(instance,
                   BSLHDMIRX_HREF_EXPAND_OFF,
                   BSLHDMIRX_DATAEN_HREF_DONTFORCE_LOW,
                   BSLHDMIRX_DATAEN_EXPAND_OFF,
                   BSLHDMIRX_DATAEN_SELECTION_VHREF,
                   dataEnDelay);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, errCode)

    }
#endif

    /* Release the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    return(TM_OK);
}

/******************************************************************************
    \brief Set video output routing configuration. this function allows to map
           the output signal to the physical output ports. You can also select
           the output digital resolution.

    \param instance       Instance identifier.
    \param portResolution Video port resolution.
    \param videoPortA     Signal to be output on video port A.
    \param videoPortB     Signal to be output on video port B.
    \param videoPortC     Signal to be output on video port C.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range

******************************************************************************/
tmErrorCode_t dlHdmiRxConfigureVideoPort
(
    tmInstance_t             			instance,
    tmbslHdmiRxVideoPortFormat_t        videoPortFormat,
    tmbslHdmiRxVideoPortSelection_t     videoPortASelection,
    tmbslHdmiRxVideoPortSelection_t     videoPortBSelection,
    tmbslHdmiRxVideoPortSelection_t     videoPortCSelection
)
{
    tmErrorCode_t                       errCode = TM_OK;
#ifdef TMFL_TDA19972_FAMILY
    const UInt8			VP_Comp_8[4][2] = {	{BSLHDMIRX_R_CR_CBCR_YCBCR_7_4, BSLHDMIRX_R_CR_CBCR_YCBCR_11_8},
        {BSLHDMIRX_B_CB_7_4, BSLHDMIRX_B_CB_11_8},
        {BSLHDMIRX_G_Y_7_4, BSLHDMIRX_G_Y_11_8},
        {BSLHDMIRX_LOZ, BSLHDMIRX_LOZ}
    };
    //										 todo: check if below index is indeed 3
    //const UInt8			VP_Comp_12[4][3]= { {BSLHDMIRX_R_CR_CBCR_YCBCR_3_0, BSLHDMIRX_R_CR_CBCR_YCBCR_7_4, BSLHDMIRX_R_CR_CBCR_YCBCR_11_8},
    //{BSLHDMIRX_B_CB_3_0, BSLHDMIRX_B_CB_7_4, BSLHDMIRX_B_CB_11_8},
    //{BSLHDMIRX_G_Y_3_0, BSLHDMIRX_G_Y_7_4, BSLHDMIRX_G_Y_11_8},
    //{BSLHDMIRX_LOZ, BSLHDMIRX_LOZ, BSLHDMIRX_LOZ}};
#endif

    /* ConfigureVideoPort */

    if (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion != TMDL_HDMIRX_DEVICE_TDA19971)
    {
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureVideoPort(instance,
                  videoPortFormat,
                  videoPortASelection,
                  videoPortBSelection,
                  videoPortCSelection);
        RETIF(errCode !=TM_OK, errCode)
    }
    else /* TDA19971 */
    {

#ifdef TMFL_TDA19972_FAMILY
        if (videoPortFormat == BSLHDMIRX_VIDEOPORT_8BITS)
        {
            /*RGB444 or YUV 444*/
            /* on TDA19978, VPA is VP[6..13], VPB is VP[14..21] and VPC is VP[22..29]
             * keep the same mapping on a TDA19971 that is video ports on top of the video bus*/
            /* for TDA19971, out video bus is only 24 bits*/
            /* so VPA will be mapped on outVP[0..7], VPB vill be outVP[8..15] and VPC will be outVP[16..23] */
            /* based on the core 36 bit videoport / package 24 bit videoport mapping for the TDA19971 package*/
            /* VPA will be VP[4..11], VPB VP[16..23] and VPC VP[28..35] */
            /* Write VP_CTRL registers */

            errCode  = tmbslTDA1997XDefineVideoPort(instance, BSLHDMIRX_VP24_G4_3_0, VP_Comp_8[videoPortASelection][0]);
            errCode |= tmbslTDA1997XDefineVideoPort(instance, BSLHDMIRX_VP24_G4_7_4, VP_Comp_8[videoPortASelection][1]);
            errCode |= tmbslTDA1997XDefineVideoPort(instance, BSLHDMIRX_VP24_G4_11_8, VP_Comp_8[videoPortBSelection][0]);
            errCode |= tmbslTDA1997XDefineVideoPort(instance, BSLHDMIRX_VP24_G4_15_12, VP_Comp_8[videoPortBSelection][1]);
            errCode |= tmbslTDA1997XDefineVideoPort(instance, BSLHDMIRX_VP24_G4_19_16, VP_Comp_8[videoPortCSelection][0]);
            errCode |= tmbslTDA1997XDefineVideoPort(instance, BSLHDMIRX_VP24_G4_23_20, VP_Comp_8[videoPortCSelection][1]);

        }
        else /* BSLHDMIRX_VIDEOPORT_12BITS as BSLHDMIRX_VIDEOPORT_10BITS has been rejected before */
        {
            /* YUV 422 SMP or CCIR 12 bits. In that case VPA must be BSLHDMIRX_VIDEOPORT_HIGHIMPEDANCE, VPB and VPC are to be programmed,
             * bits 0..3 of the VP are HiZ
             */

            if (videoPortASelection != BSLHDMIRX_VIDEOPORT_HIGHIMPEDANCE)
            {
                /* Release the semaphore */
                errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
                RETIF(errCode != TM_OK, errCode)
                return TMDL_ERR_DLHDMIRX_BAD_PARAMETER;

            }
            
            // todo:  check i below can support index [2]
            errCode  = tmbslTDA1997XDefineVideoPort(instance, BSLHDMIRX_VP24_G4_3_0, VP_Comp_8[videoPortBSelection][0]);
            errCode |= tmbslTDA1997XDefineVideoPort(instance, BSLHDMIRX_VP24_G4_7_4, VP_Comp_8[videoPortBSelection][1]);
            
            errCode |= tmbslTDA1997XDefineVideoPort(instance, BSLHDMIRX_VP24_G4_11_8, VP_Comp_8[videoPortBSelection][2]); // added 
            errCode |= tmbslTDA1997XDefineVideoPort(instance, BSLHDMIRX_VP24_G4_15_12, VP_Comp_8[videoPortCSelection][0]);
            errCode |= tmbslTDA1997XDefineVideoPort(instance, BSLHDMIRX_VP24_G4_19_16, VP_Comp_8[videoPortCSelection][1]);
            errCode |= tmbslTDA1997XDefineVideoPort(instance, BSLHDMIRX_VP24_G4_23_20, VP_Comp_8[videoPortCSelection][2]); // added 
        }
#endif
    }
    return (errCode);
}


/******************************************************************************
    \brief Set video output format. This function allows setting the general
           parameters of the output signal.

    \param instance    Instance identifier.
    \param audioFormat Audio output format (I2S, SPDIF, OBA)
    \param audioSysclk Audio sysclk frequency (128,256,512FS)

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range

******************************************************************************/
tmErrorCode_t tmdlHdmiRxSetAudioOutputFormat
(
    tmInstance_t             instance,
    tmdlHdmiRxAudioFormat_t  audioFormat,
    tmdlHdmiRxAudioSysClk_t  audioSysClk
)
{
    tmErrorCode_t                       errCode = TM_OK;
    tmbslHdmiRxAudioOutputFormat_t      outputFormat;
    tmbslHdmiRxAudioI2SResolution_t     i2sResolution;
    tmbslHdmiRxAudioPacketMode_t        packetMode;
    tmbslHdmiRxAudioClocksSysMode_t     systemClockMode;
    tmbslHdmiRxAudioTestTone_t          testTone = BSLHDMIRX_TESTTONE_OFF;
    tmbslHdmiRxHBROutputMode_t          outputModeHBR;
    tmbslHdmiRxAudioClocksModeRefFreq_t clocksRefFreq;
    tmbslHdmiRxAudioPathSpFlagMode_t    spFlagMode;

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* Take the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    /* Memorize the audio format and system clock */
    instanceStatusInfo[instance].pMuteAndVideoPort->audioFormat = audioFormat;
    instanceStatusInfo[instance].pMuteAndVideoPort->audioSysClk = audioSysClk;

    /* Test the audio mute state */
    if(instanceStatusInfo[instance].pMuteAndVideoPort->audioMuteState == True)
    {
        /* Release the semaphore */
        errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
        RETIF(errCode != TM_OK, errCode)
        return(TM_OK);
    }

    /* Test tone must be enabled if video input is test color bar */
    if ((instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_480P) ||
#ifdef TMFL_TDA19972_FAMILY
            (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_480I) ||
            (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_576I) ||
            (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_480P) ||
            (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_576P) ||
            (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_480I) ||
            (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_576I) ||
#endif
            (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_576P))
    {
        testTone = BSLHDMIRX_TESTTONE_ON;
    }

    /* Test the audio format */
    switch(audioFormat)
    {
    case TMDL_HDMIRX_AUDIOFORMAT_I2S16:
        /* I2S 16 bits */
        outputFormat  = BSLHDMIRX_AUDIOOUTPUT_I2S;
        i2sResolution = BSLHDMIRX_I2S_RESOLUTION_16_BITS;
        packetMode    = BSLHDMIRX_AUDIO_SAMPLE_PACKET_MODE;
        outputModeHBR = BSLHDMIRX_HBR_STRAIGHT_OUTPUT_MODE;
        spFlagMode    = BSLHDMIRX_AUDIOPATH_SP_FLAG_USED_BY_FIFO;
        break;

    case TMDL_HDMIRX_AUDIOFORMAT_I2S32:
        /* I2S 32 bits */
        outputFormat  = BSLHDMIRX_AUDIOOUTPUT_I2S;
        i2sResolution = BSLHDMIRX_I2S_RESOLUTION_32_BITS;
        packetMode    = BSLHDMIRX_AUDIO_SAMPLE_PACKET_MODE;
        outputModeHBR = BSLHDMIRX_HBR_STRAIGHT_OUTPUT_MODE;
        spFlagMode    = BSLHDMIRX_AUDIOPATH_SP_FLAG_USED_BY_FIFO;
        break;

    case TMDL_HDMIRX_AUDIOFORMAT_SPDIF:
        /* SPDIF */
        outputFormat  = BSLHDMIRX_AUDIOOUTPUT_SPDIF;
        i2sResolution = BSLHDMIRX_I2S_RESOLUTION_32_BITS;
        packetMode    = BSLHDMIRX_AUDIO_SAMPLE_PACKET_MODE;
        outputModeHBR = BSLHDMIRX_HBR_STRAIGHT_OUTPUT_MODE;
        spFlagMode    = BSLHDMIRX_AUDIOPATH_SP_FLAG_USED_BY_FIFO;
        break;

    case TMDL_HDMIRX_AUDIOFORMAT_OBA:
        /* One Bit Audio */
        outputFormat  = BSLHDMIRX_AUDIOOUTPUT_I2S;
        i2sResolution = BSLHDMIRX_I2S_RESOLUTION_32_BITS;
        packetMode    = BSLHDMIRX_ONE_BIT_AUDIO_PACKET_MODE;
        outputModeHBR = BSLHDMIRX_HBR_STRAIGHT_OUTPUT_MODE;
        spFlagMode    = BSLHDMIRX_AUDIOPATH_SP_FLAG_USED_BY_FIFO;
        break;

    case TMDL_HDMIRX_AUDIOFORMAT_I2S16_HBR_STRAIGHT:
        /* I2S 16 bits High Bit Rate straight */
        outputFormat  = BSLHDMIRX_AUDIOOUTPUT_I2S;
        i2sResolution = BSLHDMIRX_I2S_RESOLUTION_16_BITS;
        packetMode    = BSLHDMIRX_HBR_AUDIO_PACKET_MODE;
        outputModeHBR = BSLHDMIRX_HBR_STRAIGHT_OUTPUT_MODE;
        spFlagMode    = BSLHDMIRX_AUDIOPATH_SP_FLAG_IGNORED_BY_FIFO;
        break;

    case TMDL_HDMIRX_AUDIOFORMAT_I2S16_HBR_DEMUX:
        /* I2S 16 bits High Bit Rate demux */
        outputFormat  = BSLHDMIRX_AUDIOOUTPUT_I2S;
        i2sResolution = BSLHDMIRX_I2S_RESOLUTION_16_BITS;
        packetMode    = BSLHDMIRX_HBR_AUDIO_PACKET_MODE;
        outputModeHBR = BSLHDMIRX_HBR_DEMUX_OUTPUT_MODE;
        spFlagMode    = BSLHDMIRX_AUDIOPATH_SP_FLAG_IGNORED_BY_FIFO;
        break;

    case TMDL_HDMIRX_AUDIOFORMAT_I2S32_HBR_DEMUX:
        /* I2S 32 bits High Bit Rate demux */
        outputFormat  = BSLHDMIRX_AUDIOOUTPUT_I2S;
        i2sResolution = BSLHDMIRX_I2S_RESOLUTION_32_BITS;
        packetMode    = BSLHDMIRX_HBR_AUDIO_PACKET_MODE;
        outputModeHBR = BSLHDMIRX_HBR_DEMUX_OUTPUT_MODE;
        spFlagMode    = BSLHDMIRX_AUDIOPATH_SP_FLAG_IGNORED_BY_FIFO;
        break;

    case TMDL_HDMIRX_AUDIOFORMAT_SPDIF_HBR_DEMUX:
        /* SPDIF High Bit Rate demux */
        outputFormat  = BSLHDMIRX_AUDIOOUTPUT_SPDIF;
        i2sResolution = BSLHDMIRX_I2S_RESOLUTION_32_BITS;
        packetMode    = BSLHDMIRX_HBR_AUDIO_PACKET_MODE;
        outputModeHBR = BSLHDMIRX_HBR_DEMUX_OUTPUT_MODE;
        spFlagMode    = BSLHDMIRX_AUDIOPATH_SP_FLAG_IGNORED_BY_FIFO;
        break;

    case TMDL_HDMIRX_AUDIOFORMAT_DST:
        /* Direct Stream Transfer */
        outputFormat  = BSLHDMIRX_AUDIOOUTPUT_I2S;
        i2sResolution = BSLHDMIRX_I2S_RESOLUTION_32_BITS;
        packetMode    = BSLHDMIRX_DST_AUDIO_PACKET_MODE;
        outputModeHBR = BSLHDMIRX_HBR_STRAIGHT_OUTPUT_MODE;
        spFlagMode    = BSLHDMIRX_AUDIOPATH_SP_FLAG_IGNORED_BY_FIFO;
        break;

    default:
        /* Release the semaphore */
        errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
        RETIF(errCode != TM_OK, errCode)
        return TMDL_ERR_DLHDMIRX_BAD_PARAMETER;
    }

    /* Test the audio system clock parameter */
    switch(audioSysClk)
    {
    case TMDL_HDMIRX_AUDIOSYSCLK_128FS:
        /* Multiplication factor Fs * 128 */
        systemClockMode = BSLHDMIRX_CLOCKS_MODE_128FS_SYSCLKMODE;
        break;

    case TMDL_HDMIRX_AUDIOSYSCLK_256FS:
        /* Multiplication factor Fs * 256 */
        systemClockMode = BSLHDMIRX_CLOCKS_MODE_256FS_SYSCLKMODE;
        break;

    case TMDL_HDMIRX_AUDIOSYSCLK_512FS:
        /* Multiplication factor Fs * 512 */
        systemClockMode = BSLHDMIRX_CLOCKS_MODE_512FS_SYSCLKMODE;
        break;

    default:
        /* Release the semaphore */
        errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
        RETIF(errCode != TM_OK, errCode)
        return TMDL_ERR_DLHDMIRX_BAD_PARAMETER;
    }

    /* Configure the reference frequency */
    clocksRefFreq = BSLHDMIRX_CLOCKS_MODE_REFFREQ_27;

    /* ConfigureAudioFormatter */
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureAudioFormatter(instance,
              spFlagMode,
              instanceStatusInfo[instance].pChannelAssignmentInfo->currentChannelAssignment,
              BSLHDMIRX_AUDIOPATH_LAYOUT_PACKET_HEADER,
              BSLHDMIRX_AUDIO_LAYOUT_FORCED_0,
              FIFO_LATENCY, testTone,
              outputFormat,
              BSLHDMIRX_AUDIOOUTPUT_NORMAL,
              BSLHDMIRX_AUDIOOUTPUT_NORMAL,
              BSLHDMIRX_AUDIOOUTPUT_NORMAL,
              BSLHDMIRX_AUDIOOUTPUT_NORMAL,
              BSLHDMIRX_AUDIOOUTPUT_NORMAL,
              BSLHDMIRX_AUDIOOUTPUT_NORMAL,
              BSLHDMIRX_AUDIOOUTPUT_NORMAL,
              BSLHDMIRX_AUDIOOUTPUT_NORMAL,
              BSLHDMIRX_AUDIOOUTPUT_NORMAL,
              BSLHDMIRX_AUDIOOUTPUT_NORMAL);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, errCode)

    /* ConfigureAudioSelection */
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureAudioSelection(instance,
              packetMode,
              BSLHDMIRX_POSITIVE_ONLY_AUDIOPLL_REF,
              i2sResolution,
              BSLHDMIRX_AUDIOPLL_SELECTION_NORMAL,
              outputModeHBR);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, errCode)

    /* AudioAutoMute TBD put mute on*/
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxAudioAutoMute(instance,
              BSLHDMIRX_AUDIO_AUTOMUTE_OFF);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, errCode)

    /* ConfigureAudioClkMode */
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureAudioClkMode(instance,
              BSLHDMIRX_CLOCKS_MODE_NORMAL,
              BSLHDMIRX_CLOCKS_MODE_REFMODE_XTAL,
              clocksRefFreq,
              BSLHDMIRX_CLOCKS_MODE_128FS_AUDIOMODE,
              systemClockMode);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, errCode)

    /* Release the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    return(TM_OK);
}

/******************************************************************************
    \brief Set the channel assignment. This function allows to manually
           configure the channel assignment instead of using the data of
           the audio infoframe.

    \param instance                 Instance identifier.
    \param channelAssignment        Channel assignment value (channel allocation, see CEA-861-D Table 20 for details)
    \param channelAssignmentMode    Channel assignment mode (refer to AUD infoframe or forced)

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range

******************************************************************************/
tmErrorCode_t tmdlHdmiRxSetChannelAssignment
(
    tmInstance_t                        instance,
    UInt8                               channelAssignment,
    tmdlHdmiRxChannelAssignmentMode_t   channelAssignmentMode
)
{
    tmErrorCode_t                    errCode = TM_OK;
    tmbslHdmiRxAudioOutputFormat_t   outputFormat;
    tmbslHdmiRxAudioTestTone_t       testTone     = BSLHDMIRX_TESTTONE_OFF;
    tmbslHdmiRxAudioPathSpFlagMode_t spFlagMode   = BSLHDMIRX_AUDIOPATH_SP_FLAG_USED_BY_FIFO;


    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* Take the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    /* Memorize the channel assignment mode */
    instanceStatusInfo[instance].pChannelAssignmentInfo->channelAssignmentMode = channelAssignmentMode;

    if(channelAssignmentMode == TMDL_HDMIRX_CHANNEL_ASSIGNMENT_FORCED)
    {
        instanceStatusInfo[instance].pChannelAssignmentInfo->currentChannelAssignment = channelAssignment;

        /* Test the audio mute state */
        if(instanceStatusInfo[instance].pMuteAndVideoPort->audioMuteState == True)
        {
            /* Release the semaphore */
            errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
            RETIF(errCode != TM_OK, errCode)
            return(TM_OK);
        }

        /* Test tone must be enabled if video input is test color bar */
        if ((instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_480P) ||
#ifdef TMFL_TDA19972_FAMILY
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_480I) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_576I) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_480P) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_576P) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_480I) ||
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_576I) ||
#endif
                (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_576P))
        {
            testTone = BSLHDMIRX_TESTTONE_ON;
        }

        /* Test the audio format */
        switch(instanceStatusInfo[instance].pMuteAndVideoPort->audioFormat)
        {
        case TMDL_HDMIRX_AUDIOFORMAT_I2S16:
        /* I2S 16 bits */
        case TMDL_HDMIRX_AUDIOFORMAT_I2S32:
        /* I2S 32 bits */
        case TMDL_HDMIRX_AUDIOFORMAT_OBA:
            /* One Bit Audio */
            outputFormat  = BSLHDMIRX_AUDIOOUTPUT_I2S;
            spFlagMode    = BSLHDMIRX_AUDIOPATH_SP_FLAG_USED_BY_FIFO;
            break;

        case TMDL_HDMIRX_AUDIOFORMAT_SPDIF:
            /* SPDIF */
            outputFormat  = BSLHDMIRX_AUDIOOUTPUT_SPDIF;
            spFlagMode    = BSLHDMIRX_AUDIOPATH_SP_FLAG_USED_BY_FIFO;
            break;

        case TMDL_HDMIRX_AUDIOFORMAT_I2S16_HBR_DEMUX:
        /* I2S 16 bits High Bit Rate demux */
        case TMDL_HDMIRX_AUDIOFORMAT_I2S32_HBR_DEMUX:
        /* I2S 32 bits High Bit Rate demux */
        case TMDL_HDMIRX_AUDIOFORMAT_DST:
            /* Direct Stream Transfer */
            outputFormat  = BSLHDMIRX_AUDIOOUTPUT_I2S;
            spFlagMode    = BSLHDMIRX_AUDIOPATH_SP_FLAG_IGNORED_BY_FIFO;
            break;

        case TMDL_HDMIRX_AUDIOFORMAT_SPDIF_HBR_DEMUX:
            /* SPDIF High Bit Rate demux */
            outputFormat  = BSLHDMIRX_AUDIOOUTPUT_SPDIF;
            spFlagMode    = BSLHDMIRX_AUDIOPATH_SP_FLAG_IGNORED_BY_FIFO;
            break;

        default:
            /* Release the semaphore */
            errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
            RETIF(errCode != TM_OK, errCode)
            return TMDL_ERR_DLHDMIRX_BAD_PARAMETER;
        }

        /* ConfigureAudioFormatter */
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureAudioFormatter(instance,
                  spFlagMode,
                  instanceStatusInfo[instance].pChannelAssignmentInfo->currentChannelAssignment,
                  BSLHDMIRX_AUDIOPATH_LAYOUT_PACKET_HEADER,
                  BSLHDMIRX_AUDIO_LAYOUT_FORCED_0,
                  FIFO_LATENCY, testTone,
                  outputFormat,
                  BSLHDMIRX_AUDIOOUTPUT_NORMAL,
                  BSLHDMIRX_AUDIOOUTPUT_NORMAL,
                  BSLHDMIRX_AUDIOOUTPUT_NORMAL,
                  BSLHDMIRX_AUDIOOUTPUT_NORMAL,
                  BSLHDMIRX_AUDIOOUTPUT_NORMAL,
                  BSLHDMIRX_AUDIOOUTPUT_NORMAL,
                  BSLHDMIRX_AUDIOOUTPUT_NORMAL,
                  BSLHDMIRX_AUDIOOUTPUT_NORMAL,
                  BSLHDMIRX_AUDIOOUTPUT_NORMAL,
                  BSLHDMIRX_AUDIOOUTPUT_NORMAL);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode !=TM_OK, errCode)
    }

    /* Release the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    return TM_OK;
}

/******************************************************************************
    \brief Configure repeater functionality of the IC. This function is used to
           configure and activate repeater functionality. With this function,
           client can load the list of KSV collected from attached devices, set
           the topology of the device cascade and also set the repeater status
           to be returned to HDMI Tx requesting KSV list.

    \param instance         Instance identifier.
    \param ksvNum           number of KSV to be relayed by repeater
    \param ksvList          pointer to the KSV list (from key 0 to key N
                            written in little endian order)
    \param depth            depth of the cascade of devices
    \param maxDevStatus     is the maximum number of attached devices
                            exceeded ? (True/False)
    \param maxCascadeStatus is the maximum cascade depth exceeded ? (True/False)
    \param dpUnauth         Attached devices failed to authenticate
    \param wdEnd            Time-out for KSV collection expired
    \param ksvready         KSV list is ready

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_NOT_SUPPORTED: parameters are not consistent with
              HW capabilities.

******************************************************************************/
tmErrorCode_t tmdlHdmiRxConfigureRepeater
(
    tmInstance_t        instance,
    UInt8               ksvNum,
    UInt8              *ksvList,
    UInt8               depth,
    Bool                maxDevStatus,
    Bool                maxCascadeStatus,
    Bool                dpUnauth,
    Bool                wdEnd,
    Bool                ksvReady
)

{
    tmErrorCode_t                           errCode = TM_OK;
    UInt8                                   ksvIndex;
    UInt8                                   ksvValue[KSV_SIZE];
    tmdlHdmiRxCapabilities_t                capabilities;
    tmbslHdmiRxHDCPRepStatusMaxDev_t        statusMaxDev;
    tmbslHdmiRxHDCPRepStatusMaxCascade_t    statusMaxCascade;
//    UInt8				TimeC5;

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* Make sure the repeater capability is supported by the chip */
    errCode = tmdlHdmiRxGetCapabilitiesM(instance, &capabilities);
    RETIF(errCode != TM_OK, errCode);
    RETIF(capabilities.repeater == False, TMDL_ERR_DLHDMIRX_NOT_SUPPORTED);

    /* Take the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    if(maxDevStatus == False)
    {
        statusMaxDev = BSLHDMIRX_HDCPREPEATER_MAXDEV_OK;
    }
    else
    {
        statusMaxDev = BSLHDMIRX_HDCPREPEATER_MAXDEV_EXCEEDED;
    }

    if(maxCascadeStatus == False)
    {
        statusMaxCascade = BSLHDMIRX_OK_HDCPREPEATER_MAXCASC;
    }
    else
    {
        statusMaxCascade = BSLHDMIRX_EXCEEDED_HDCPREPEATER_MAXCASC;
    }
    /* Set the repeater status */
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetHDCPRepeaterStatus(
                  instance,
                  statusMaxDev,
                  ksvNum,
                  statusMaxCascade,
                  depth);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode);


    /* Write the KSVs */
    for (ksvIndex = 0; ksvIndex < ksvNum; ksvIndex ++)
    {

        /* Set the KSV index */
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetKSVIndex(instance, ksvIndex);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode);

        /* Each KSV is made of five bytes */
        /* The BSL expects the KSV in big endian whereas DevLib takes KSV in little endian */
        ksvValue[4] = ksvList[(ksvIndex * KSV_SIZE) + 0];
        ksvValue[3] = ksvList[(ksvIndex * KSV_SIZE) + 1];
        ksvValue[2] = ksvList[(ksvIndex * KSV_SIZE) + 2];
        ksvValue[1] = ksvList[(ksvIndex * KSV_SIZE) + 3];
        ksvValue[0] = ksvList[(ksvIndex * KSV_SIZE) + 4];
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetKSVValue(instance, ksvValue);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode);
    }


    /* Set the repeater control */
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetHDCPRepeaterControl(instance,
              (UInt8)dpUnauth,
              (UInt8)wdEnd,
              (UInt8)ksvReady);
//    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxReadI2C(instance,((1<<8) | 0x40),1,&TimeC5);
//    printf("___Ready bit written____ @%d\n", TimeC5);

#ifdef TMFL_TDA19972_FAMILY
    /* WA Wait 600ms before to continue. This is to prevent any I2C write access in page 0 before the source has read the KSV list */
    errCode = tmdlHdmiRxIWWait(600);
#endif


    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode);

    /* Release the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    return TM_OK;
}








/*============================================================================*/
/**
    \brief Get TMDS frequency of a given input. The value is given in kHz.

    \param instance Instance identifier.
    \param inputSel Input to retreive the frequency of
    \param pFreq    Pointer on the integer that will receive the frequency of the
                    selected input

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range

******************************************************************************/
tmErrorCode_t tmdlHdmiRxGetTMDSFrequency
(
    tmInstance_t       instance,
    tmdlHdmiRxInput_t  inputSel,
    UInt32             *pFreq
)
{
    tmErrorCode_t                      errCode = TM_OK;
    tmbslHdmiRxInputSelDigitalSource_t bslDigitalInput = BSLHDMIRX_DIGITALINPUT_A;


    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE);

    /* Check the pointer */
    RETIF (pFreq == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS);



    switch (inputSel)  /* This function only applies to digital inputs */
    {
    case TMDL_HDMIRX_INPUT_HDMI_A:       /* Digital input A */
        bslDigitalInput = BSLHDMIRX_DIGITALINPUT_A;
        break;

    case TMDL_HDMIRX_INPUT_HDMI_B:       /* Digital input B */
        bslDigitalInput = BSLHDMIRX_DIGITALINPUT_B;
        break;

    default:
        errCode = TMDL_ERR_DLHDMIRX_BAD_PARAMETER;
        break;

    }



    /* Take the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode);


    /* Call the BSL */
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetTMDSFrequency(instance, bslDigitalInput, pFreq);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode);


    /* Release the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode);


    return (errCode);
}

/*============================================================================*/
/**
    \brief Get current audio sample frequency.

    \param instance  Instance identifier.
    \param dstRate   DST single/double rate
    \param audioFreq Audio sample frequency

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

******************************************************************************/
tmErrorCode_t tmdlHdmiRxGetAudioFrequency
(
    tmInstance_t                 instance,
    tmdlHdmiRxAudioDstRate_t     *pDstRate,
    tmdlHdmiRxAudioSampleFreq_t  *pAudioFreq
)
{
    tmErrorCode_t                 errCode = TM_OK;
    tmbslHdmiRxAudioDstRate_t     dstRateBsl;
    tmbslHdmiRxAudioSampleFreq_t  audioFreqBsl;


    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE);

    /* Check the pointer */
    RETIF (pDstRate == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS);
    RETIF (pAudioFreq == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS);

    /* Take the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode);

    /* Call the BSL */
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetAudioFrequency(instance,
              &dstRateBsl,
              &audioFreqBsl);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode);

    *pDstRate   = (tmdlHdmiRxAudioDstRate_t) dstRateBsl;
    *pAudioFreq = (tmdlHdmiRxAudioSampleFreq_t) audioFreqBsl;

    /* Release the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode);

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Loads EDID data into embedded EDID memory of receiver device.

    \param instance   Instance identifier.
    \param commonEDID Pointer to the two block EDID (array of 256 bytes)
                      that will be common to all HDMI inputs.
    \param sPA        Pointer to the array of 4 source physical addresses (a
                      total of 8 bytes written in big endian order).
                      The first element of this array corresponds to HDMI input A.
    \param sPAOffset  Offset of the first SPA byte inside EDID block 1
                      (offset is the same for all HDMI inputs: A, B, C and D).

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
tmErrorCode_t tmdlHdmiRxLoadEdidData
(
    tmInstance_t  instance,
    UInt8         *commonEDID,
    UInt16        *sPA,
    UInt8         sPAOffset
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           checksum[5];
    UInt8           intermediate_cksum=0;
    UInt16          i = 0;

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE);

    if (unitTable[instance].internalEdid == False)
    {
        errCode = TMDL_ERR_DLHDMIRX_OPERATION_NOT_PERMITTED;
    }
    else
    {
        /* Take the semaphore */
        errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
        RETIF(errCode != TM_OK, errCode);

        for (i=0; i < 5; i++)
        {
            checksum[i] = 0;
        }

        /* Calculate the checksum for block 0 */
        for(i=0; i < 127; i++)
        {
            /* Add the first 127 bytes */
            checksum[0] = checksum[0] + commonEDID[i];
        }

        /* Make the 2's complement of the addition and replace the last bytes */
        checksum[0] = (UInt8)((0xFF - checksum[0]) + 0x01);

        /* Calculate the checksum according to the SPA */
        for(i=128; i < 255; i++)
        {
            /* Calculate the intermediate checksum without including
            the bytes corresponding to the SPA */

            if ((i != (128 + sPAOffset)) && (i != (128 + sPAOffset + 1)) )
            {
                intermediate_cksum = intermediate_cksum + commonEDID[i];
            }
        }

        /* Calculate the checksum using the corresponding SPA */
        for (i=0; i < 2; i++)
        {
            checksum[i+1] = intermediate_cksum + (UInt8)((sPA[i] & 0xFF00)>>8) +
                            (UInt8)(sPA[i] & 0x00FF);
            checksum[i+1] = (UInt8)((0xFF - checksum[i+1]) + 0x01);
        }

        /* Call the BSL */
        errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxLoadEDIDData(instance,
                  commonEDID,
                  sPA,
                  checksum,
                  sPAOffset);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode);

        /* Release the semaphore */
        errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
        RETIF(errCode != TM_OK, errCode);
    }

    return errCode;
}

#ifdef TMFL_TDA19972_FAMILY
/*============================================================================*/
/**
    \brief Loads EDID data into embedded EDID memory of receiver device.

    \param instance   Instance identifier.
    \param commonEDID Pointer to the two block EDID (array of 256 bytes)
                      that will be common to all HDMI inputs.
    \param sPA        Pointer to the array of 4 source physical addresses (a
                      total of 8 bytes written in big endian order).
                      The first element of this array corresponds to HDMI input A.
    \param sPAOffset  Offset of the first SPA byte inside EDID block 1
                      (offset is the same for all HDMI inputs: A, B, C and D).

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
tmErrorCode_t tmdlHdmiRxLoadSecondaryEdidData
(
    tmInstance_t  instance,
    UInt8         *commonEDID,
    UInt16        *sPA,
    UInt8         sPAOffset
)
{
    tmErrorCode_t   errCode = TM_OK;
    UInt8           checksum[5];
    UInt8           intermediate_cksum=0;
    UInt16          i = 0;

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE);

    if (unitTable[instance].internalEdid == False)
    {
        errCode = TMDL_ERR_DLHDMIRX_OPERATION_NOT_PERMITTED;
    }
    else
    {
        /* Take the semaphore */
        errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
        RETIF(errCode != TM_OK, errCode);

        for (i=0; i < 5; i++)
        {
            checksum[i] = 0;
        }

        /* Calculate the checksum for block 0 */
        for(i=0; i < 127; i++)
        {
            /* Add the first 127 bytes */
            checksum[0] = checksum[0] + commonEDID[i];
        }

        /* Make the 2's complement of the addition and replace the last bytes */
        checksum[0] = (UInt8)((0xFF - checksum[0]) + 0x01);

        /* Calculate the checksum according to the SPA */
        for(i=128; i < 255; i++)
        {
            /* Calculate the intermediate checksum without including
            the bytes corresponding to the SPA */

            if ((i != (128 + sPAOffset)) && (i != (128 + sPAOffset + 1)) )
            {
                intermediate_cksum = intermediate_cksum + commonEDID[i];
            }
        }

        /* Calculate the checksum using the corresponding SPA */
        for (i=0; i < 2; i++)
        {
            checksum[i+1] = intermediate_cksum + (UInt8)((sPA[i] & 0xFF00)>>8) +
                            (UInt8)(sPA[i] & 0x00FF);
            checksum[i+1] = (UInt8)((0xFF - checksum[i+1]) + 0x01);
        }

        /* Call the BSL */
        errCode = tmbslTDA1997XLoadSecondaryEDIDData((tmUnitSelect_t)instance,
                  commonEDID,
                  sPA,
                  checksum,
                  sPAOffset);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode);

        /* Release the semaphore */
        errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
        RETIF(errCode != TM_OK, errCode);
    }

    return errCode;
}

/*============================================================================*/
/**
    \brief Reads EDID data from embedded EDID memory of receiver device.

    \param instance   Instance identifier.
    \param commonEDID Pointer to the two block EDID (array of 256 bytes)
                      that will be common to all HDMI inputs.
    \param sPA        Pointer to a 2 16bit words table
    \param checksum   Pointer to table of 3 byte that will be filled with:
            - byte 127 of the EDID
            - checksum for input A
            - checksum for input B
    \param  sPAOffset to a byte
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
tmErrorCode_t tmdlHdmiRxReadEdidData
(
    tmInstance_t  instance,
    UInt8         *commonEDID,
    UInt16        *sPA,
    UInt8         *checksum,
    UInt8         *sPAOffset
)
{
    tmErrorCode_t   errCode = TM_OK;

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE);

    if (unitTable[instance].internalEdid == False)
    {
        errCode = TMDL_ERR_DLHDMIRX_OPERATION_NOT_PERMITTED;
    }
    else
    {
        /* Take the semaphore */
        errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
        RETIF(errCode != TM_OK, errCode);

        /* Read the EDID data from embedded memory */
        errCode = tmbslTDA1997XReadEdidData((tmUnitSelect_t)instance,
                                            commonEDID,
                                            sPA,
                                            checksum,
                                            sPAOffset);

        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode);

        /* Release the semaphore */
        errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
        RETIF(errCode != TM_OK, errCode);
    }

    return errCode;
}

/*============================================================================*/
/**
    \brief Reads EDID data from embedded secondary EDID memory of receiver device.

    \param instance   Instance identifier.
    \param commonEDID Pointer to the two block EDID (array of 256 bytes)
                      that will be common to all HDMI inputs.
    \param sPA        Pointer to a 2 16bit words table
    \param checksum   Pointer to table of 3 byte that will be filled with:
            - byte 127 of the EDID
            - checksum for input A
            - checksum for input B
    \param  sPAOffset to a byte
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
tmErrorCode_t tmdlHdmiRxReadSecondaryEdidData
(
    tmInstance_t  instance,
    UInt8         *commonEDID,
    UInt16        *sPA,
    UInt8         *checksum,
    UInt8         *sPAOffset
)
{
    tmErrorCode_t   errCode = TM_OK;

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE);

    if (unitTable[instance].internalEdid == False)
    {
        errCode = TMDL_ERR_DLHDMIRX_OPERATION_NOT_PERMITTED;
    }
    else
    {
        /* Take the semaphore */
        errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
        RETIF(errCode != TM_OK, errCode);

        /* Read the EDID data from embedded memory */
        errCode = tmbslTDA1997XReadSecondaryEdidData((tmUnitSelect_t)instance,
                  commonEDID,
                  sPA,
                  checksum,
                  sPAOffset);

        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode);

        /* Release the semaphore */
        errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
        RETIF(errCode != TM_OK, errCode);
    }

    return errCode;
}

/*============================================================================*/
/**
    \brief Attach EDID to the specified input.

    \param instance   Instance identifier.
    \param tmdlHdmiRxInput Selected Input.
    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range

******************************************************************************/
tmErrorCode_t tmdlHdmiRxAttachEdidToInput
(
    tmInstance_t  instance,
    tmdlHdmiRxInput_t input
)
{
    tmErrorCode_t   errCode = TM_OK;

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE);

    switch (input)
    {
    case TMDL_HDMIRX_INPUT_HDMI_A:
        tmbslTDA1997XAttachEdidToInput((tmUnitSelect_t)instance, BSLHDMIRX_DIGITALINPUT_A);
        break;

    case TMDL_HDMIRX_INPUT_HDMI_B:
        tmbslTDA1997XAttachEdidToInput((tmUnitSelect_t)instance, BSLHDMIRX_DIGITALINPUT_B);
        break;
    default:
        errCode = TMDL_ERR_DLHDMIRX_BAD_PARAMETER;
    }

    return errCode;

}

/*============================================================================*/
/**
    \brief Attach EDID to the specified input.

    \param instance   Instance identifier.
    \param tmdlHdmiRxInput Selected Input.
    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range

******************************************************************************/
tmErrorCode_t tmdlHdmiRxAttachSecondaryEdidToInput
(
    tmInstance_t  instance,
    tmdlHdmiRxInput_t input
)
{
    tmErrorCode_t   errCode = TM_OK;

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE);

    switch (input)
    {
    case TMDL_HDMIRX_INPUT_HDMI_A:
        tmbslTDA1997XAttachSecondaryEdidToInput((tmUnitSelect_t)instance, BSLHDMIRX_DIGITALINPUT_A);
        break;

    case TMDL_HDMIRX_INPUT_HDMI_B:
        tmbslTDA1997XAttachSecondaryEdidToInput((tmUnitSelect_t)instance, BSLHDMIRX_DIGITALINPUT_B);
        break;
    default:
        errCode = TMDL_ERR_DLHDMIRX_BAD_PARAMETER;
    }

    return errCode;

}

/*============================================================================*/
/**
    \brief Compares EDID data with embedded EDID memory of receiver device.

    \param instance   Instance identifier.
    \param commonEDID Pointer to the two block EDID (array of 256 bytes)
                      that will be common to all HDMI inputs.
    \param sPA        Pointer to the array of 4 source physical addresses (a
                      total of 8 bytes written in big endian order).
                      The first element of this array corresponds to HDMI input A.
    \param sPAOffset  Offset of the first SPA byte inside EDID block 1
                      (offset is the same for all HDMI inputs: A, B, C and D).
    \param sMatch     Result of data comparison (True if content is the same)

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
tmErrorCode_t tmdlHdmiRxCompareEdidData
(
    tmInstance_t  instance,
    UInt8         *commonEDID,
    UInt16        *sPA,
    UInt8         sPAOffset,
    Bool          *sMatch
)
{
    tmErrorCode_t   errCode = TM_OK;

    UInt8           commonEDID_read[256];
    UInt16          sPA_read[2];
    UInt8           sPAOffset_read[1];
    UInt8           checksum_read[5];
    UInt8           subaddr;

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE);

    if (unitTable[instance].internalEdid == False)
    {
        errCode = TMDL_ERR_DLHDMIRX_OPERATION_NOT_PERMITTED;
    }
    else
    {
        /* Take the semaphore */
        errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
        RETIF(errCode != TM_OK, errCode);

        /* Read the EDID data from embedded memory */
        errCode = tmbslTDA1997XReadEdidData((tmUnitSelect_t)instance,
                                            commonEDID_read,
                                            sPA_read,
                                            checksum_read,
                                            sPAOffset_read);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode);

        /* Update comparison flag */
        *sMatch = True;

        /* Check EDID block 0 data */
        for (subaddr = 0; subaddr < 126; subaddr++)
        {
            if (commonEDID[subaddr] != commonEDID_read[subaddr])
            {
                PRNT_ARGS("/!\\Compare EDID difference at subaddress 0x%.2x\n", subaddr);
                *sMatch = False;
            }
        };

        /* Check EDID block 1 data */
        for (subaddr = 128; subaddr < 254; subaddr++)
        {
            if (commonEDID[subaddr] != commonEDID_read[subaddr])
            {
                PRNT_ARGS("/!\\Compare EDID difference at subaddress 0x%.2x\n", subaddr);
                *sMatch = False;
            }
        };

        /* Check sPA_read data */
        for (subaddr = 0; subaddr < 2; subaddr++)
        {
            if (sPA_read[subaddr] != sPA[subaddr])
            {
                PRNT_ARGS("/!\\Compare SPA difference at subaddress 0x%.2x\n", subaddr);
                *sMatch = False;
            }
        };

        /* Check sPAOffset data */
        if (sPAOffset_read[0] != sPAOffset)
        {
            PRNT_ARGS("/!\\Compare sPAOffset difference at subaddress 0x%.2x\n", 0);
            *sMatch = False;
        }

        ///* Check checksum_read data */
        //for (subaddr = 0; subaddr < 5; subaddr++)
        //{
        //    if (checksum_read[subaddr] != checksum[subaddr])
        //    {
        //        PRNT_ARGS("/!\\Compare checksum difference at subaddress 0x%.2x\n", subaddr);
        //        *sMatch = False;
        //    }
        //};
        //
        /* Release the semaphore */
        errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
        RETIF(errCode != TM_OK, errCode);
    }

    return errCode;
}

/*============================================================================*/
/**
    \brief Read EDID version.

    \param instance   Instance identifier.
    \param version    Pointer to 1 byte array.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range

******************************************************************************/
tmErrorCode_t tmdlHdmiRxReadEdidVersion
(
    tmInstance_t  instance,
    UInt8         *version
)
{
    tmErrorCode_t   errCode = TM_OK;

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE);

    /* Take the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode);

    /* Read the EDID data from embedded memory */
    errCode = tmbslTDA1997XReadI2C((tmUnitSelect_t)instance, (UInt16)(((0x20)<<8)|(0x80)), 1, version);
    RETIF(errCode != TM_OK, errCode);

    /* Release the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode);

    return errCode;
}

/*============================================================================*/
/**
    \brief Save EDID data into EDID MTP of receiver device.

    \param instance   Instance identifier.
    \param commonEDID Pointer to the two block EDID (array of 256 bytes)
                      that will be common to all HDMI inputs.
    \param sPA        Pointer to the array of 4 source physical addresses (a
                      total of 8 bytes written in big endian order).
                      The first element of this array corresponds to HDMI input A.
    \param sPAOffset  Offset of the first SPA byte inside EDID block 1
                      (offset is the same for all HDMI inputs: A, B, C and D).

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
tmErrorCode_t tmdlHdmiRxSaveEdidDataToMTP
(
    tmInstance_t  instance
)
{
    tmErrorCode_t   errCode = TM_OK;

    UInt8           intermediate_cksum=0;
    UInt16          i = 0;

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE);

    if (unitTable[instance].internalEdid == False)
    {
        errCode = TMDL_ERR_DLHDMIRX_OPERATION_NOT_PERMITTED;
    }
    else
    {
        /* Take the semaphore */
        errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
        RETIF(errCode != TM_OK, errCode);

        /* Save EDID data from internal memory to MTP  */
        errCode = tmbslTDA1997XSaveEDIDDataToMTP(instance);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode);

        /* Reload EDID data from MTP to internal memory */
        errCode = tmbslTDA1997XReLoadEdidDataFromMTP(instance);
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode);

        /* Release the semaphore */
        errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
        RETIF(errCode != TM_OK, errCode);
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
tmErrorCode_t tmdlHdmiRxLoadConfigData
(
    tmInstance_t  instance,
    UInt8         *DDC_Config,
    UInt8         *RT_Config
)
{
    tmErrorCode_t   errCode = TM_OK;

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* Take the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    /* Call the BSL */
    errCode = tmbslTDA1997XLoadConfigData((tmUnitSelect_t)instance,
                                          DDC_Config,
                                          RT_Config);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode);

    /* Release the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode);

    return errCode;
}

/*============================================================================*/
/**
    \brief Save DDC and RT configuration data into MTP of receiver device.

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
tmErrorCode_t tmdlHdmiRxSaveConfigDataToMTP
(
    tmInstance_t  instance
)
{
    tmErrorCode_t   errCode = TM_OK;

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE);

    /* Take the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode);

    /* Save config data from internal memory to MTP  */
    errCode = tmbslTDA1997XSaveConfigDataToMTP((tmUnitSelect_t)instance);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode);

    /* Reload config data from MTP to internal memory */
    errCode = tmbslTDA1997XReLoadConfigDataFromMTP((tmUnitSelect_t)instance);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode);

    /* Release the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode);

    return errCode;
}
#endif

/*============================================================================*/
/**
    \brief Writing on I2C bus (for debug purpose only)

    \param instance Instance identifier.
    \param page     Page number of the register to write
    \param reg      Address of the first register to write
    \param length   Data length (number of bytes)
    \param * pData  Pointer of data to be written

    \return The call result:
            - TM_OK  : the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_RESOURCE_NOT_OWNED: the caller does not own
              the resource

 ******************************************************************************/
tmErrorCode_t tmdlHdmiRxDebugWriteI2CRegister
(
    tmInstance_t instance,
    UInt8        page,
    UInt8        reg,
    UInt8        length,
    UInt8        *pData
)
{
    tmErrorCode_t errCode;

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* Take the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    /* check if unit corresponding to instance is opened */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], unitTable[instance].opened == False, TMDL_ERR_DLHDMIRX_RESOURCE_NOT_OWNED)

    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxWriteI2C(
                  (tmUnitSelect_t) instance,
                  ( ((UInt16) page) << 8 ) | ((UInt16) reg),
                  length,
                  pData);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)

    /* Release the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    return(TM_OK);
}

/*============================================================================*/
/**
    \brief        Reading on I2C bus (for debug purpose only)

    \param instance Instance identifier.
    \param page     Page number of the register to read
    \param reg      Address of the first register to read
    \param length   Data length (number of bytes)
    \param * pData  Pointer of data to be read

    \return The call result:
            - TM_OK  : the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_RESOURCE_NOT_OWNED: the caller does not own
              the resource

 ******************************************************************************/
tmErrorCode_t tmdlHdmiRxDebugReadI2CRegister
(
    tmInstance_t instance,
    UInt8        page,
    UInt8        reg,
    UInt8        length,
    UInt8        *pData
)
{
    tmErrorCode_t errCode;

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* Take the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    /* check if unit corresponding to instance is opened */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], unitTable[instance].opened == False, TMDL_ERR_DLHDMIRX_RESOURCE_NOT_OWNED)

    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxReadI2C(
                  (tmUnitSelect_t) instance,
                  ( ((UInt16) page) << 8 ) | ((UInt16) reg),
                  length,
                  pData);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode)

    /* Release the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode)

    return(TM_OK);
}

/*============================================================================*/
/*                           INTERNAL FUNCTION                                */
/*============================================================================*/
/******************************************************************************
    \brief Manage the callback rised by the BSL for an instance

    \param IrqSource    Source of the callback

    \return NA

******************************************************************************/
static void dlHdmiRxManageCallbackInstance
(
    tmbslHdmiRxIRQSource_t irqSource,
    tmInstance_t           instance,
    UInt8                   status
)
{
    UInt32                      hdmiStatus;
    UInt8                       packetAndInfoframeTable[MAX_IF_DATA];
    UInt8                       i;
    UInt8                       checksumCalc = 0;
#ifdef TMFL_TDA19972_FAMILY
    UInt8						susState;
#endif
    UInt32                      audioStatus;

    tmdlHdmiRxACPPacket_t       ACPPacket;
    tmdlHdmiRxGBDPacket_t       GBDPacket;
    tmdlHdmiRxAVIInfoframe_t    AVIInfoframe;
    tmdlHdmiRxAudioInfoframe_t  audioInfoframe;
    tmdlHdmiRxISRC1Packet_t     ISRC1Packet;
    tmdlHdmiRxISRC2Packet_t     ISRC2Packet;
    tmdlHdmiRxVSInfoframe_t     VSInfoframe;
#ifdef TMFL_TDA19972_FAMILY
    tmdlHdmiRxSPDInfoframe_t    SPDInfoframe;
#endif
    tmdlHdmiRxMPSInfoframe_t    MPSInfoframe;
    tmbslHdmiRxUpSampler_t      upSample;
    UInt8                       pixelPackingPhase;
    tmbslHdmiRxDeepColorMode_t  deepColorMode;

    tmdlHdmiRxEvent_t eventToReport;

    tmdlHdmiRxResolutionID_t resolutionID;

#ifdef TMFL_HDMI_OUT
    tmErrorCode_t           errCode;
    tmbslHdmiRxHotPlug_t    hpdStatus;  /* HPD status */
    tmbslHdmiRxRxSense_t    rxSenseStatus;  /* Rx Sense status */
    UInt8                   edidStatus = TMDL_HDMIRX_EDID_NOT_READ;
#endif

    /* Init the packetAndInfoframeTable */
    for(i = 0; i < MAX_IF_DATA; i++)
    {
        packetAndInfoframeTable[i] = 0;
    }

    
//sprintf(Msg,"IRQ %x inst %d",irqSource,instance);
//WriteLn(Msg);
    switch(irqSource)
    {
    case BSLHDMIRX_IRQSOURCE_FLAGS:
        /* GetHDMIStatus */
        gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetHDMIStatus(instance, &hdmiStatus);

        //sprintf(Msg,"hStat %x",hdmiStatus);
        //WriteLn(Msg);

        /* Do not take into account the "encrypted" info if HDCP repeater state C5 has not been reached */
        if( (!instanceStatusInfo[instance].stateC5Reached) && unitTable[instance].repeaterEnable)
        {
            hdmiStatus = (UInt32) (hdmiStatus & ~BSLHDMIRX_HDMISTATUS_ENCRYPTED);
        }


        /* Check if there is a change on the value or if a reset has been asked */

        if ( ( ( (hdmiStatus & BSLHDMIRX_HDMISTATUS_HDMI) &&
                 !(instanceStatusInfo[instance].hdmiFlagsStatus & BSLHDMIRX_HDMISTATUS_HDMI) ) ||
                ( (instanceStatusInfo[instance].hdmiFlagsReset & MASK_HDMISTATUS_HDMI_RESET) &&
                  (hdmiStatus & BSLHDMIRX_HDMISTATUS_HDMI) ) ) &&
                ( (dlHdmiRxGetState(instance) == STATE_LOCKED) || (dlHdmiRxGetState(instance) == STATE_CONFIGURED) ) )
        {
            /* Update with the new status */
            instanceStatusInfo[instance].hdmiFlagsStatus |= BSLHDMIRX_HDMISTATUS_HDMI;

            if(dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_HDMI_DETECTED) == TMDL_HDMIRX_EVENT_ENABLED)
            {

                /* Release the semaphore */
                tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

                unitTable[instance].pInfoCallback(TMDL_HDMIRX_HDMI_DETECTED);

                /* Take the semaphore */
                tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);

                /* Callback is done, the reset flag can be reset to 0 */
                instanceStatusInfo[instance].hdmiFlagsReset = (UInt16) (instanceStatusInfo[instance].hdmiFlagsReset & ~MASK_HDMISTATUS_HDMI_RESET);
            }
        }

        /* CBE: handle audio sample packet layout */
        if ( dlHdmiRxGetState(instance) == STATE_CONFIGURED )
        {
            /* Look at which event to report */
            if (hdmiStatus & BSLHDMIRX_HDMISTATUS_LAY_ASP) {

                if (instanceStatusInfo[instance].hdmiFlagsReset & MASK_HDMISTATUS_LAYOUT_1_RESET) {
                    eventToReport = TMDL_HDMIRX_AUDIO_AUDIO_LAYOUT_1;
                }
                else {
                    eventToReport = (tmdlHdmiRxEvent_t)0; /* Already reported once, we put 0 to avoid reporting the event */
                }
                /* Update devlib status flag */
                instanceStatusInfo[instance].hdmiFlagsStatus |= BSLHDMIRX_HDMISTATUS_LAY_ASP;
            }
            else {
                if (instanceStatusInfo[instance].hdmiFlagsReset & MASK_HDMISTATUS_LAYOUT_0_RESET) {
                    eventToReport = TMDL_HDMIRX_AUDIO_AUDIO_LAYOUT_0;
                }
                else {
                    eventToReport = (tmdlHdmiRxEvent_t)0; /* Already reported once, we put 0 to avoid reporting the event */
                }
                /* Update devlib status flag */
                instanceStatusInfo[instance].hdmiFlagsStatus &= (~BSLHDMIRX_HDMISTATUS_LAY_ASP);
            }


            if (eventToReport) {

                /* if event enabled */
                if(dlHdmiRxGetEventStatus(instance, eventToReport) == TMDL_HDMIRX_EVENT_ENABLED) {

                    /* Release the semaphore */
                    tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

                    unitTable[instance].pInfoCallback(eventToReport);

                    /* Take the semaphore */
                    tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);

                    /* Callback is done, the reset flag can be reset to 0 */
                    if (eventToReport == TMDL_HDMIRX_AUDIO_AUDIO_LAYOUT_1) {
                        instanceStatusInfo[instance].hdmiFlagsReset = (UInt16) (instanceStatusInfo[instance].hdmiFlagsReset & ~MASK_HDMISTATUS_LAYOUT_1_RESET);
                        instanceStatusInfo[instance].hdmiFlagsReset = (UInt16) (instanceStatusInfo[instance].hdmiFlagsReset | MASK_HDMISTATUS_LAYOUT_0_RESET);
                    }
                    else {
                        instanceStatusInfo[instance].hdmiFlagsReset = (UInt16) (instanceStatusInfo[instance].hdmiFlagsReset & ~MASK_HDMISTATUS_LAYOUT_0_RESET);
                        instanceStatusInfo[instance].hdmiFlagsReset = (UInt16) (instanceStatusInfo[instance].hdmiFlagsReset | MASK_HDMISTATUS_LAYOUT_1_RESET);
                    }

                }


            } /* if an event to report */

        } /* handle audio sample packet layout */


        if ( ( ( !(hdmiStatus & BSLHDMIRX_HDMISTATUS_HDMI) &&
                 (instanceStatusInfo[instance].hdmiFlagsStatus & BSLHDMIRX_HDMISTATUS_HDMI) ) ||
                ( (instanceStatusInfo[instance].hdmiFlagsReset & MASK_HDMISTATUS_DVI_RESET) &&
                  !(hdmiStatus & BSLHDMIRX_HDMISTATUS_HDMI) ) ) &&
                ( (dlHdmiRxGetState(instance) == STATE_LOCKED) || (dlHdmiRxGetState(instance) == STATE_CONFIGURED) ) )
        {
            /* Update with the new status */
            instanceStatusInfo[instance].hdmiFlagsStatus = (UInt32) (instanceStatusInfo[instance].hdmiFlagsStatus & ~BSLHDMIRX_HDMISTATUS_HDMI);

            /* DVI is always RGB 4:4:4, so up/down sampler must be bypassed  (up/down sampler is used for 4:2:2 input) */
            /* The color space management is made in the application, so DevLib does nothing to force RGB, it only forces the 4:4:4: sample format */
            gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureUpDownSampler(instance,
                    BSLHDMIRX_DOWNSAMPLE_BYPASS,
                    BSLHDMIRX_UPSAMPLE_BYPASS);

            /* Derepeater, no pixel repetition in DVI ?? */
            gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigurePixelRepeater(instance, 0x00);

            if(dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_DVI_DETECTED) == TMDL_HDMIRX_EVENT_ENABLED)
            {
                /* Release the semaphore */
                tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

                unitTable[instance].pInfoCallback(TMDL_HDMIRX_DVI_DETECTED);

                /* Take the semaphore */
                tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);

                /* Callback is done, the reset flag can be reset to 0 */
                instanceStatusInfo[instance].hdmiFlagsReset = (UInt16) (instanceStatusInfo[instance].hdmiFlagsReset & ~MASK_HDMISTATUS_DVI_RESET);
            }
        }

        if (hdmiStatus & BSLHDMIRX_HDMISTATUS_ENCRYPTED)
        {

            if ( ( ( (hdmiStatus & BSLHDMIRX_HDMISTATUS_ENCRYPTED) &&
                     !(instanceStatusInfo[instance].hdmiFlagsStatus & BSLHDMIRX_HDMISTATUS_ENCRYPTED) ) ||
                    ( (instanceStatusInfo[instance].hdmiFlagsReset & MASK_HDMISTATUS_ENCRYPTION_RESET) &&
                      (hdmiStatus & BSLHDMIRX_HDMISTATUS_ENCRYPTED) ) ) &&
                    ( (dlHdmiRxGetState(instance) == STATE_LOCKED) || (dlHdmiRxGetState(instance) == STATE_CONFIGURED) ) )
            {
                /* Update with the new status */
                instanceStatusInfo[instance].hdmiFlagsStatus |= BSLHDMIRX_HDMISTATUS_ENCRYPTED;

                if(dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_ENCRYPTION_DETECTED) == TMDL_HDMIRX_EVENT_ENABLED)
                {
                    /* Release the semaphore */
                    tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

                    unitTable[instance].pInfoCallback(TMDL_HDMIRX_ENCRYPTION_DETECTED);

                    /* Take the semaphore */
                    tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);

                    /* Callback is done, the reset flag can be reset to 0 */
                    instanceStatusInfo[instance].hdmiFlagsReset = (UInt16) (instanceStatusInfo[instance].hdmiFlagsReset & ~MASK_HDMISTATUS_ENCRYPTION_RESET);
                }
            }
        }

        if ( ( ( !(hdmiStatus & BSLHDMIRX_HDMISTATUS_ENCRYPTED) &&
                 (instanceStatusInfo[instance].hdmiFlagsStatus & BSLHDMIRX_HDMISTATUS_ENCRYPTED) ) ||
                ( (instanceStatusInfo[instance].hdmiFlagsReset & MASK_HDMISTATUS_NO_ENCRYPTION_RESET) &&
                  !(hdmiStatus & BSLHDMIRX_HDMISTATUS_ENCRYPTED) ) ) &&
                ( (dlHdmiRxGetState(instance) == STATE_LOCKED) || (dlHdmiRxGetState(instance) == STATE_CONFIGURED) ) )
        {
            /* Update with the new status */
            instanceStatusInfo[instance].hdmiFlagsStatus = (UInt32) (instanceStatusInfo[instance].hdmiFlagsStatus & ~BSLHDMIRX_HDMISTATUS_ENCRYPTED);

            if(dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_NO_ENCRYPTION_DETECTED) == TMDL_HDMIRX_EVENT_ENABLED)
            {
                /* Release the semaphore */
                tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

                unitTable[instance].pInfoCallback(TMDL_HDMIRX_NO_ENCRYPTION_DETECTED);

                /* Take the semaphore */
                tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);

                /* Callback is done, the reset flag can be reset to 0 */
                instanceStatusInfo[instance].hdmiFlagsReset = (UInt16) (instanceStatusInfo[instance].hdmiFlagsReset & ~MASK_HDMISTATUS_NO_ENCRYPTION_RESET);
            }
        }

        if ( ( ( (hdmiStatus & BSLHDMIRX_HDMISTATUS_EESS) &&
                 !(instanceStatusInfo[instance].hdmiFlagsStatus & BSLHDMIRX_HDMISTATUS_EESS) ) ||
                ( (instanceStatusInfo[instance].hdmiFlagsReset & MASK_HDMISTATUS_EESS_RESET) &&
                  (hdmiStatus & BSLHDMIRX_HDMISTATUS_EESS) ) ) &&
                ( (dlHdmiRxGetState(instance) == STATE_LOCKED) || (dlHdmiRxGetState(instance) == STATE_CONFIGURED) ) )
        {
            /* Update with the new status */
            instanceStatusInfo[instance].hdmiFlagsStatus |= BSLHDMIRX_HDMISTATUS_EESS;

            if(dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_EESS_HDCP_DETECTED) == TMDL_HDMIRX_EVENT_ENABLED)
            {
                /* Release the semaphore */
                tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

                unitTable[instance].pInfoCallback(TMDL_HDMIRX_EESS_HDCP_DETECTED);

                /* Take the semaphore */
                tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);

                /* Callback is done, the reset flag can be reset to 0 */
                instanceStatusInfo[instance].hdmiFlagsReset = (UInt16) (instanceStatusInfo[instance].hdmiFlagsReset & ~MASK_HDMISTATUS_EESS_RESET);
            }
        }

        if ( ( ( !(hdmiStatus & BSLHDMIRX_HDMISTATUS_EESS) &&
                 (instanceStatusInfo[instance].hdmiFlagsStatus & BSLHDMIRX_HDMISTATUS_EESS) ) ||
                ( (instanceStatusInfo[instance].hdmiFlagsReset & MASK_HDMISTATUS_OESS_RESET) &&
                  !(hdmiStatus & BSLHDMIRX_HDMISTATUS_EESS) ) ) &&
                ( (dlHdmiRxGetState(instance) == STATE_LOCKED) || (dlHdmiRxGetState(instance) == STATE_CONFIGURED) ) )
        {
            /* Update with the new status */
            instanceStatusInfo[instance].hdmiFlagsStatus = (UInt32) (instanceStatusInfo[instance].hdmiFlagsStatus & ~BSLHDMIRX_HDMISTATUS_EESS);

            if(dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_OESS_HDCP_DETECTED) == TMDL_HDMIRX_EVENT_ENABLED)
            {
                /* Release the semaphore */
                tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

                unitTable[instance].pInfoCallback(TMDL_HDMIRX_OESS_HDCP_DETECTED);

                /* Take the semaphore */
                tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);

                /* Callback is done, the reset flag can be reset to 0 */
                instanceStatusInfo[instance].hdmiFlagsReset = (UInt16) (instanceStatusInfo[instance].hdmiFlagsReset & ~MASK_HDMISTATUS_OESS_RESET);
            }
        }

        if ( ( ( (hdmiStatus & BSLHDMIRX_HDMISTATUS_AVMUTE) &&
                 !(instanceStatusInfo[instance].hdmiFlagsStatus & BSLHDMIRX_HDMISTATUS_AVMUTE) ) ||
                ( (instanceStatusInfo[instance].hdmiFlagsReset & MASK_HDMISTATUS_AVMUTE_ACTIVE_RESET) &&
                  (hdmiStatus & BSLHDMIRX_HDMISTATUS_AVMUTE) ) ) &&
                ( (dlHdmiRxGetState(instance) == STATE_LOCKED) || (dlHdmiRxGetState(instance) == STATE_CONFIGURED) ) )
        {
            /* Update with the new status */
            instanceStatusInfo[instance].hdmiFlagsStatus |= BSLHDMIRX_HDMISTATUS_AVMUTE;

            if(dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_AVI_AVMUTE_ACTIVE) == TMDL_HDMIRX_EVENT_ENABLED)
            {
                /* Release the semaphore */
                tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

                unitTable[instance].pInfoCallback(TMDL_HDMIRX_AVI_AVMUTE_ACTIVE);

                /* Take the semaphore */
                tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);

                /* Callback is done, the reset flag can be reset to 0 */
                instanceStatusInfo[instance].hdmiFlagsReset = (UInt16) (instanceStatusInfo[instance].hdmiFlagsReset & ~MASK_HDMISTATUS_AVMUTE_ACTIVE_RESET);

                /* Initialization of the AVMute counter: init value is 100 and the counter will be decremented */
                /* on every HeartBeat call (= every 50 ms) => counter will approximately expire after 5 s */
                instanceStatusInfo[instance].pMuteAndVideoPort->avMuteCount = (UInt8) AVMUTE_COUNT_INIT_VAL;
            }
        }

        if ( ( ( !(hdmiStatus & BSLHDMIRX_HDMISTATUS_AVMUTE) &&
                 (instanceStatusInfo[instance].hdmiFlagsStatus & BSLHDMIRX_HDMISTATUS_AVMUTE) ) ||
                ( (instanceStatusInfo[instance].hdmiFlagsReset & MASK_HDMISTATUS_AVMUTE_INACTIVE_RESET) &&
                  !(hdmiStatus & BSLHDMIRX_HDMISTATUS_AVMUTE) ) ) &&
                ( (dlHdmiRxGetState(instance) == STATE_LOCKED) || (dlHdmiRxGetState(instance) == STATE_CONFIGURED) ) )
        {
            /* Update with the new status */
            instanceStatusInfo[instance].hdmiFlagsStatus = (UInt32) (instanceStatusInfo[instance].hdmiFlagsStatus & ~BSLHDMIRX_HDMISTATUS_AVMUTE);

            if(dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_AVI_AVMUTE_INACTIVE) == TMDL_HDMIRX_EVENT_ENABLED)
            {
                /* Release the semaphore */
                tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

                unitTable[instance].pInfoCallback(TMDL_HDMIRX_AVI_AVMUTE_INACTIVE);

                /* Take the semaphore */
                tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);

                /* Callback is done, the reset flag can be reset to 0 */
                instanceStatusInfo[instance].hdmiFlagsReset = (UInt16) (instanceStatusInfo[instance].hdmiFlagsReset & ~MASK_HDMISTATUS_AVMUTE_INACTIVE_RESET);

                /* AVMute is inactive, the counter can be reset to the default value */
                instanceStatusInfo[instance].pMuteAndVideoPort->avMuteCount = (UInt8) AVMUTE_COUNT_DEF_VAL;
            }
        }
        break;







    case BSLHDMIRX_IRQSOURCE_GAMUT:
        /* Read Gamut packet */
        gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxReadGBDPacket(instance,
                &packetAndInfoframeTable[0],  /* Packet type - 1 byte */
                &packetAndInfoframeTable[1],  /* Packet header - 2 bytes */
                &packetAndInfoframeTable[3]); /* Packet data - up to 28 bytes */

        /* Re-organize the event data */
        GBDPacket.packetType = packetAndInfoframeTable[0];
        GBDPacket.nextField = (packetAndInfoframeTable[1] & 0x80U) >> 7;
        GBDPacket.GBDProfile = (packetAndInfoframeTable[1] & 0x70U) >> 4;
        GBDPacket.affectedGamutSeqNum = packetAndInfoframeTable[1] & 0x0FU;
        GBDPacket.noCrntGBD = (packetAndInfoframeTable[2] & 0x80U) >> 7;
        GBDPacket.packetSeq = (packetAndInfoframeTable[2] & 0x30U) >> 4;
        GBDPacket.currentGamutSeqNum = packetAndInfoframeTable[2] & 0x0FU;
        for (i  = 0; i < 28; i++) GBDPacket.GBDData[i] = packetAndInfoframeTable[3 + i];


        if (dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_GBD_PACKET_RECEIVED) == TMDL_HDMIRX_EVENT_ENABLED)
        {
            /* Release the semaphore */
            tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

            unitTable[instance].pDataCallback(TMDL_HDMIRX_GBD_PACKET_RECEIVED,
                                              (void *)&GBDPacket,
                                              sizeof(tmdlHdmiRxGBDPacket_t));
            /* Take the semaphore */
            tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
        }

        break;

    case BSLHDMIRX_IRQSOURCE_ISRC2:
        /* ReadISRC2Packet */
        gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxReadISRC2Packet(instance,
                &packetAndInfoframeTable[0],  /* Packet type */
                &packetAndInfoframeTable[3]); /* Packet data array */

        for(i=0; i<ISRC2_PKT_NB; i++)
        {
            checksumCalc = checksumCalc + packetAndInfoframeTable[i];
        }
        if (!checksumCalc)
        {

            /* Re-organaize the data */
            ISRC2Packet.packetType = packetAndInfoframeTable[0];
            for (i = 0; i < 16; i++) ISRC2Packet.UPC_EAN_ISRC[i] = packetAndInfoframeTable[3 + i];

            /* Call the callback */
            if (dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_ISRC2_PACKET_RECEIVED) == TMDL_HDMIRX_EVENT_ENABLED)
            {
                /* Release the semaphore */
                tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

                unitTable[instance].pDataCallback(TMDL_HDMIRX_ISRC2_PACKET_RECEIVED,
                                                  (void *)(&ISRC2Packet),
                                                  sizeof(tmdlHdmiRxISRC2Packet_t));
                /* Take the semaphore */
                tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
            }
        }
        break;


    case BSLHDMIRX_IRQSOURCE_ISRC1:
        /* ReadISRC1Packet */
        gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxReadISRC1Packet(instance,
                &packetAndInfoframeTable[0],  /* Packet type */
                &packetAndInfoframeTable[1],  /* Control value */
                &packetAndInfoframeTable[3]); /* Packet data array */
        for(i=0; i<ISRC1_PKT_NB; i++)
        {
            checksumCalc = checksumCalc + packetAndInfoframeTable[i];
        }
        if (!checksumCalc)
        {

            /* Re-organize the data */
            ISRC1Packet.packetType = packetAndInfoframeTable[0];
            ISRC1Packet.ISRCCont = (packetAndInfoframeTable[1] & 0x80U) >> 7;
            ISRC1Packet.ISRCValid = (packetAndInfoframeTable[1] & 0x40U) >> 6;
            ISRC1Packet.ISRCStatus = packetAndInfoframeTable[1] & 0x07U;
            for (i = 0; i < 16; i++) ISRC1Packet.UPC_EAN_ISRC[i] = packetAndInfoframeTable[3 + i];


            /* Call the callback */
            if (dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_ISRC1_PACKET_RECEIVED) == TMDL_HDMIRX_EVENT_ENABLED)
            {
                /* Release the semaphore */
                tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

                unitTable[instance].pDataCallback(TMDL_HDMIRX_ISRC1_PACKET_RECEIVED,
                                                  (void *)(&ISRC1Packet),
                                                  sizeof(tmdlHdmiRxISRC1Packet_t));
                /* Take the semaphore */
                tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
            }
        }
        break;

    case BSLHDMIRX_IRQSOURCE_ACP:
        /* ReadACPPacket */
        gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetACPPacket(instance,
                &ACPPacket.packetType, /* Packet type - 1 byte */
                &ACPPacket.ACPType,    /* ACP type - 1 byte */
                ACPPacket.ACPByte);    /* ACP bytes - 28 bytes */
        for(i=0; i<ACP_PKT_NB; i++)
        {
            checksumCalc = checksumCalc + packetAndInfoframeTable[i];
        }
        if (!checksumCalc)
        {

            /* No bit field - no need to reorganize the data */

            /* Call the callback */
            if (dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_ACP_PACKET_RECEIVED) == TMDL_HDMIRX_EVENT_ENABLED)
            {
                /* Release the semaphore */
                tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

                unitTable[instance].pDataCallback(TMDL_HDMIRX_ACP_PACKET_RECEIVED,
                                                  (void *)(&ACPPacket),
                                                  sizeof(tmdlHdmiRxACPPacket_t));
                /* Take the semaphore */
                tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
            }
        }
        break;

    case BSLHDMIRX_IRQSOURCE_AVI_IF:
        /* GetInfoframe */
        
        gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInfoframe(instance,
                BSLHDMIRX_INFOFRAME_AVI,
                &packetAndInfoframeTable[0],  /* Packet Type */
                &packetAndInfoframeTable[1],  /* Version */
                &packetAndInfoframeTable[2],  /* Length */
                &packetAndInfoframeTable[3],  /* Checksum */
                &packetAndInfoframeTable[4]); /* Data */
        for(i=0; i<AVI_IF_NB; i++)
        {
            checksumCalc = checksumCalc + packetAndInfoframeTable[i];
        }
        if (!checksumCalc /*&& (packetAndInfoframeTable[0] == 0x82) */)
        {

            /* Re-organize the data */
            AVIInfoframe.packetType = packetAndInfoframeTable[0];
            AVIInfoframe.version = packetAndInfoframeTable[1];
            AVIInfoframe.length = packetAndInfoframeTable[2];
            AVIInfoframe.checksum = packetAndInfoframeTable[3];
            AVIInfoframe.colorIndicator = (packetAndInfoframeTable[4] & 0x60U) >> 5;           /* Y1, Y0 */
            AVIInfoframe.activeInfoPresent = (packetAndInfoframeTable[4]  & 0x10U) >> 4;       /* A0 */
            AVIInfoframe.barInformationDataValid = (packetAndInfoframeTable[4] & 0x0CU) >> 2;  /* B1, B0 */
            AVIInfoframe.scanInformation = packetAndInfoframeTable[4] & 0x03U;                 /* S1, S0 */
            AVIInfoframe.colorimetry = (packetAndInfoframeTable[5] & 0xC0U) >> 6;              /* C1, C0 */
            AVIInfoframe.pictureAspectRatio = (packetAndInfoframeTable[5] & 0x30U) >> 4;       /* M1, M0 */
            AVIInfoframe.activeFormatAspectRatio = packetAndInfoframeTable[5] & 0x0FU;         /* R3, R2, R1, R0 */
            AVIInfoframe.nonUniformPictureScaling = packetAndInfoframeTable[6] & 0x03U;        /* SC1, SC0 */
            AVIInfoframe.videoFormatIdentificationCode = packetAndInfoframeTable[7] & 0x7FU;   /* VIC6, VIC5, VIC4, VIC3, VIC2, VIC1, VIC0 */
            AVIInfoframe.pixelRepetitionFactor = packetAndInfoframeTable[8] & 0x0FU;           /* PR3, PR2, PR1, PR0 */
            AVIInfoframe.lineNumberEndTopBarLow = packetAndInfoframeTable[9];
            AVIInfoframe.lineNumberEndTopBarHigh = packetAndInfoframeTable[10];
            AVIInfoframe.lineNumberStartBottomBarLow = packetAndInfoframeTable[11];
            AVIInfoframe.lineNumberStartBottomBarHigh = packetAndInfoframeTable[12];
            AVIInfoframe.lineNumberEndLeftBarLow = packetAndInfoframeTable[13];
            AVIInfoframe.lineNumberEndLeftBarHigh = packetAndInfoframeTable[14];
            AVIInfoframe.lineNumberStartRightBarLow = packetAndInfoframeTable[15];
            AVIInfoframe.lineNumberStartRightBarHigh = packetAndInfoframeTable[16];

            /* Test on the color indicator */
            if(AVIInfoframe.colorIndicator == COLOR_INDIC_YUV422)
            {
                upSample = BSLHDMIRX_UPSAMPLE_REPEATCHROMA;
            }
            else
            {
                upSample = BSLHDMIRX_UPSAMPLE_BYPASS;
            }

            /* The up/down sampler must be configured depending on the sample format (4:4:4 or 4:2:2) */
            /* The color space management is made in the application, so DevLib does nothing to force color space, it only forces the sample format */
            gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureUpDownSampler(instance,
                    BSLHDMIRX_DOWNSAMPLE_BYPASS,
                    upSample);



            /* Derepeater (WA since there is no way to distinguish 1440 or 2880 from 720 )*/
            gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigurePixelRepeater(instance, AVIInfoframe.pixelRepetitionFactor);




            /* Call the callback */
            if (dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_AVI_INFOFRAME_RECEIVED) == TMDL_HDMIRX_EVENT_ENABLED)
            {
                /* Release the semaphore */
                tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

                unitTable[instance].pDataCallback(TMDL_HDMIRX_AVI_INFOFRAME_RECEIVED,
                                                  (void *)(&AVIInfoframe),
                                                  sizeof (tmdlHdmiRxAVIInfoframe_t));
                /* Take the semaphore */
                tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
            }
        }
        break;

#ifdef TMFL_TDA19972_FAMILY
    case BSLHDMIRX_IRQSOURCE_VS_IF:
#else
#ifdef SUPPORT_3D_FP
    case BSLHDMIRX_IRQSOURCE_SPD_IF:
#endif
#endif
#if defined TMFL_TDA19972_FAMILY || (!defined TMFL_TDA19972_FAMILY && defined SUPPORT_3D_FP)
        /* GetInfoframe from VS */
        gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInfoframe(instance,
#ifdef TMFL_TDA19972_FAMILY
                BSLHDMIRX_INFOFRAME_VS,
#else
                BSLHDMIRX_IRQSOURCE_SPD_IF,
#endif
                &packetAndInfoframeTable[0],  /* Packet type */
                &packetAndInfoframeTable[1],  /* Version */
                &packetAndInfoframeTable[2],  /* Length */
                &packetAndInfoframeTable[3],  /* Checksum */
                &packetAndInfoframeTable[4]); /* Data */

#ifdef TMFL_TDA19972_FAMILY
        if (status == 0)
        {
            VSInfoframe.packetType     = packetAndInfoframeTable[0];   /* HB0 */
            VSInfoframe.version        = packetAndInfoframeTable[1];   /* HB1 */
            VSInfoframe.length         = packetAndInfoframeTable[2];   /* HB2 */
            VSInfoframe.checksum       = packetAndInfoframeTable[3];   /* PB0 */
            VSInfoframe.ieee_id[0]     = packetAndInfoframeTable[4];   /* PB1 */
            VSInfoframe.ieee_id[1]     = packetAndInfoframeTable[5];   /* PB2 */
            VSInfoframe.ieee_id[2]     = packetAndInfoframeTable[6];   /* PB3 */
            VSInfoframe.hdmi_video_fmt = 0; /* force No3D */
            VSInfoframe.h3d_structure  = packetAndInfoframeTable[8] >> TMDL_HDMIRX_3D_STRUCTURE_SHIFT; /* PB5 */
            VSInfoframe.h3d_ext_data   = packetAndInfoframeTable[9] >> TMDL_HDMIRX_3D_EXT_DATA_SHIFT;  /* PB6 */
            for (i = 0; i < TMDL_HDMIRX_VS_PKT_DATA_LEN-7; i++)
            {
                VSInfoframe.vsData[i] = packetAndInfoframeTable[10+i];
            }

        }
        else if (packetAndInfoframeTable[VS_IF_NB] > 3) /* check update flag */
        {
            break;
        }
#endif

        for(i=0; i<VS_IF_NB; i++)
        {
            checksumCalc = checksumCalc + packetAndInfoframeTable[i];
        }
        if (!checksumCalc)
        {
            /* Re-organize the data */
            VSInfoframe.packetType     = packetAndInfoframeTable[0];   /* HB0 */
            VSInfoframe.version        = packetAndInfoframeTable[1];   /* HB1 */
            VSInfoframe.length         = packetAndInfoframeTable[2];   /* HB2 */
            VSInfoframe.checksum       = packetAndInfoframeTable[3];   /* PB0 */
            VSInfoframe.ieee_id[0]     = packetAndInfoframeTable[4];   /* PB1 */
            VSInfoframe.ieee_id[1]     = packetAndInfoframeTable[5];   /* PB2 */
            VSInfoframe.ieee_id[2]     = packetAndInfoframeTable[6];   /* PB3 */
            VSInfoframe.hdmi_video_fmt = packetAndInfoframeTable[7] >> TMDL_HDMIRX_VIDEO_FORMAT_SHIFT; /* PB4 */
            VSInfoframe.h3d_structure  = packetAndInfoframeTable[8] >> TMDL_HDMIRX_3D_STRUCTURE_SHIFT; /* PB5 */
            VSInfoframe.h3d_ext_data   = packetAndInfoframeTable[9] >> TMDL_HDMIRX_3D_EXT_DATA_SHIFT;  /* PB6 */
            for (i = 0; i < TMDL_HDMIRX_VS_PKT_DATA_LEN-7; i++)
            {
                VSInfoframe.vsData[i] = packetAndInfoframeTable[10+i];
            }

            /* Call the callback */
#ifdef TMFL_TDA19972_FAMILY
            if (dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_VS_INFOFRAME_RECEIVED) == TMDL_HDMIRX_EVENT_ENABLED)
#else
            if (dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_SPD_INFOFRAME_RECEIVED) == TMDL_HDMIRX_EVENT_ENABLED)
#endif
            {
                /* Release the semaphore */
                tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

#ifdef TMFL_TDA19972_FAMILY
                unitTable[instance].pDataCallback(TMDL_HDMIRX_VS_INFOFRAME_RECEIVED,
#else
                unitTable[instance].pDataCallback(TMDL_HDMIRX_SPD_INFOFRAME_RECEIVED,
#endif
                                                  (void *)(&VSInfoframe),
                                                  sizeof(tmdlHdmiRxVSInfoframe_t));
                /* Take the semaphore */
                tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
            }
        }
        break;
#endif
#ifdef TMFL_TDA19972_FAMILY
    case BSLHDMIRX_IRQSOURCE_SPD_IF:
#else
#ifndef SUPPORT_3D_FP
    case BSLHDMIRX_IRQSOURCE_SPD_IF:
#endif
#endif
#if defined TMFL_TDA19972_FAMILY || (!defined TMFL_TDA19972_FAMILY && !defined SUPPORT_3D_FP)
        /* GetInfoframe */
        gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInfoframe(instance,
#ifdef TMFL_TDA19972_FAMILY
                BSLHDMIRX_INFOFRAME_SPD,
#else
                BSLHDMIRX_IRQSOURCE_SPD_IF,
#endif
                &packetAndInfoframeTable[0],  /* Packet type */
                &packetAndInfoframeTable[1],  /* Version */
                &packetAndInfoframeTable[2],  /* Length */
                &packetAndInfoframeTable[3],  /* Checksum */
                &packetAndInfoframeTable[4]); /* Data */

        for(i=0; i<SPD_IF_NB; i++)
        {
            checksumCalc = checksumCalc + packetAndInfoframeTable[i];
        }
        if (!checksumCalc)
        {

            /* Re-organize the data */
            SPDInfoframe.packetType = packetAndInfoframeTable[0];
            SPDInfoframe.version = packetAndInfoframeTable[1];
            SPDInfoframe.length = packetAndInfoframeTable[2];
            SPDInfoframe.checksum = packetAndInfoframeTable[3];
            for (i = 0; i < 8; i++) SPDInfoframe.vendorName[i] = packetAndInfoframeTable[4 + i];
            for (i = 0; i < 16; i++) SPDInfoframe.productDescription[i] = packetAndInfoframeTable[4 + 8 + i];
            SPDInfoframe.sourceDeviceDescription = packetAndInfoframeTable[4 + 8 + 16];


            /* Call the callback */
#ifdef TMFL_TDA19972_FAMILY
            if (dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_SPD_INFOFRAME_RECEIVED) == TMDL_HDMIRX_EVENT_ENABLED)
#else
            if (dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_SPD_INFOFRAME_RECEIVED) == TMDL_HDMIRX_EVENT_ENABLED)
#endif
            {
                /* Release the semaphore */
                tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

#ifdef TMFL_TDA19972_FAMILY
                unitTable[instance].pDataCallback(TMDL_HDMIRX_SPD_INFOFRAME_RECEIVED,
#else
                unitTable[instance].pDataCallback(TMDL_HDMIRX_SPD_INFOFRAME_RECEIVED,
#endif
                                                  (void *)(&SPDInfoframe),
                                                  sizeof(tmdlHdmiRxSPDInfoframe_t));

                /* Take the semaphore */
                tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
            }
        }
        break;
#endif

#ifdef TMFL_TDA19972_FAMILY
    case BSLHDMIRX_IRQSOURCE_VS_OTHER_BK1_IF:
    case BSLHDMIRX_IRQSOURCE_VS_OTHER_BK2_IF:
        /* GetInfoframe from VS_OTHER_BK1 */
        
        gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInfoframe(instance,
                BSLHDMIRX_INFOFRAME_VS_OTHER_BK1,
                &packetAndInfoframeTable[0],  /* Packet type */
                &packetAndInfoframeTable[1],  /* Version */
                &packetAndInfoframeTable[2],  /* Length */
                &packetAndInfoframeTable[3],  /* Checksum */
                &packetAndInfoframeTable[4]); /* Data */


        /* check update flag */
        
        if (packetAndInfoframeTable[VS_IF_NB] > HDMI_INFO_EXCEED)
        {
            /* GetInfoframe from VS_OTHER_BK2*/
            
            gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInfoframe(instance,
                    BSLHDMIRX_INFOFRAME_VS_OTHER_BK2,
                    &packetAndInfoframeTable[0],  /* Packet type */
                    &packetAndInfoframeTable[1],  /* Version */
                    &packetAndInfoframeTable[2],  /* Length */
                    &packetAndInfoframeTable[3],  /* Checksum */
                    &packetAndInfoframeTable[4]); /* Data */
        }


        for(i=0; i<VS_IF_NB; i++)
        {
            checksumCalc = checksumCalc + packetAndInfoframeTable[i];
        }
        if (!checksumCalc)
        {
            /* Re-organize the data */
            
            VSInfoframe.packetType     = packetAndInfoframeTable[0];   /* HB0 */
            VSInfoframe.version        = packetAndInfoframeTable[1];   /* HB1 */
            VSInfoframe.length         = packetAndInfoframeTable[2];   /* HB2 */
            VSInfoframe.checksum       = packetAndInfoframeTable[3];   /* PB0 */
            VSInfoframe.ieee_id[0]     = packetAndInfoframeTable[4];   /* PB1 */
            VSInfoframe.ieee_id[1]     = packetAndInfoframeTable[5];   /* PB2 */
            VSInfoframe.ieee_id[2]     = packetAndInfoframeTable[6];   /* PB3 */
            VSInfoframe.hdmi_video_fmt = packetAndInfoframeTable[7] >> TMDL_HDMIRX_VIDEO_FORMAT_SHIFT; /* PB4 */
            VSInfoframe.h3d_structure  = packetAndInfoframeTable[8] >> TMDL_HDMIRX_3D_STRUCTURE_SHIFT; /* PB5 */
            VSInfoframe.h3d_ext_data   = packetAndInfoframeTable[9] >> TMDL_HDMIRX_3D_EXT_DATA_SHIFT;  /* PB6 */
            for (i = 0; i < TMDL_HDMIRX_VS_PKT_DATA_LEN-4; i++)
            {
                VSInfoframe.vsData[i] = packetAndInfoframeTable[7+i];
            }
            
            /* Call the callback */
            if (dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_VS_INFOFRAME_RECEIVED) == TMDL_HDMIRX_EVENT_ENABLED)
            {
                /* Release the semaphore */
                
                tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

                unitTable[instance].pDataCallback(TMDL_HDMIRX_VS_INFOFRAME_RECEIVED,
                                                  (void *)(&VSInfoframe),
                                                  sizeof(tmdlHdmiRxVSInfoframe_t));
                /* Take the semaphore */
                tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
            }
        }
        
        break;
#endif

    case BSLHDMIRX_IRQSOURCE_AUD_IF:
        /* GetInfoframe */
        gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInfoframe(instance,
                BSLHDMIRX_INFOFRAME_AUD,
                &packetAndInfoframeTable[0],  /* Packet type */
                &packetAndInfoframeTable[1],  /* Version */
                &packetAndInfoframeTable[2],  /* Length */
                &packetAndInfoframeTable[3],  /* Checksum */
                &packetAndInfoframeTable[4]); /* Data */

        for(i=0; i<AUD_IF_NB; i++)
        {
            checksumCalc = checksumCalc + packetAndInfoframeTable[i];
        }
        if (!checksumCalc)
        {

            /* Re-organise the data */
            audioInfoframe.packetType = packetAndInfoframeTable[0];
            audioInfoframe.version = packetAndInfoframeTable[1];
            audioInfoframe.length = packetAndInfoframeTable[2];
            audioInfoframe.checksum = packetAndInfoframeTable[3];
            audioInfoframe.codingType = (packetAndInfoframeTable[4] & 0xF0U) >> 4;        /* CT3, CT2, CT1, CT0 */
            audioInfoframe.channelCount = packetAndInfoframeTable[4] & 0x07U;             /* CC2, CC1, CC0 */
            audioInfoframe.samplefrequency = (packetAndInfoframeTable[5] & 0x1CU) >> 2;   /* SF2, SF1, SF0 */
            audioInfoframe.sampleSize = packetAndInfoframeTable[5] & 0x03U;               /* SS1, SS0 */
            audioInfoframe.dataByte3 = packetAndInfoframeTable[6];
            audioInfoframe.channelAllocation = packetAndInfoframeTable[7];               /* CA7 .. CA0 */
            audioInfoframe.downmixInhibit = (packetAndInfoframeTable[8] & 0x80U) >> 7;    /* DM_INH */
            audioInfoframe.levelShiftValue = (packetAndInfoframeTable[8] & 0x78U) >> 3;   /* LSV3, LSV2, LSV1, LSV0 */


            if(instanceStatusInfo[instance].pChannelAssignmentInfo->channelAssignmentMode == TMDL_HDMIRX_REFER_TO_AUD_INFOFRAME)
            {
                /* Recover the channel assignment from the audio infoframe */
                if (audioInfoframe.channelAllocation <= MAX_CHANNEL_ALLOC)
                {
                    dlHdmiRxRecoverChannelAssignment(instance, audioInfoframe.channelAllocation);
                }
            }


            /* Call the callback */
            if (dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_AUD_INFOFRAME_RECEIVED) == TMDL_HDMIRX_EVENT_ENABLED)
            {
                /* Release the semaphore */
                tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

                unitTable[instance].pDataCallback(TMDL_HDMIRX_AUD_INFOFRAME_RECEIVED,
                                                  (void *)(&audioInfoframe),
                                                  sizeof(tmdlHdmiRxAudioInfoframe_t));
                /* Take the semaphore */
                tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
            }
        }
        break;

    case BSLHDMIRX_IRQSOURCE_MPS_IF:
        /* GetInfoframe */
        gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInfoframe(instance,
                BSLHDMIRX_INFOFRAME_MPS,
                &packetAndInfoframeTable[0],  /* Packet type */
                &packetAndInfoframeTable[1],  /* Version */
                &packetAndInfoframeTable[2],  /* Length */
                &packetAndInfoframeTable[3],  /* Checksum */
                &packetAndInfoframeTable[4]); /* Data */
        for(i=0; i<MPS_IF_NB; i++)
        {
            checksumCalc = checksumCalc + packetAndInfoframeTable[i];
        }
        if (!checksumCalc)
        {

            /* Re-organise the data */
            MPSInfoframe.packetType = packetAndInfoframeTable[0];
            MPSInfoframe.version = packetAndInfoframeTable[1];
            MPSInfoframe.length = packetAndInfoframeTable[2];
            MPSInfoframe.checksum = packetAndInfoframeTable[3];
            for (i = 0; i < 4; i++) MPSInfoframe.MPEG_bitRate[i] = packetAndInfoframeTable[4 + i];
            MPSInfoframe.fieldRepeat = (packetAndInfoframeTable[8] & 0x10U) >> 4;
            MPSInfoframe.MPEG_Frame = packetAndInfoframeTable[8] & 0x03U;


            /* Call the callback */
            if (dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_MPS_INFOFRAME_RECEIVED) == TMDL_HDMIRX_EVENT_ENABLED)
            {
                /* Release the semaphore */
                tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

                unitTable[instance].pDataCallback(TMDL_HDMIRX_MPS_INFOFRAME_RECEIVED,
                                                  (void *)(&MPSInfoframe),
                                                  sizeof(tmdlHdmiRxMPSInfoframe_t));
                /* Take the semaphore */
                tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
            }
        }
        break;

    case BSLHDMIRX_IRQSOURCE_RXDB:
        if ((dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_ACTIVITY_DETECTED) == TMDL_HDMIRX_EVENT_ENABLED) && (status == 1))
        {
            /* Release the semaphore */
            tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

            unitTable[instance].pDigitalActivityCallback(TMDL_HDMIRX_ACTIVITY_DETECTED,
                    TMDL_HDMIRX_INPUT_HDMI_B,
                    TMDL_HDMIRX_SYNC_TYPE_SEPARATED);
            /* Take the semaphore */
            tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
        }

        if ((dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_ACTIVITY_LOST) == TMDL_HDMIRX_EVENT_ENABLED) && (status == 0))
        {
#ifdef TMFL_TDA19972_FAMILY
            /* Set the state machine */
            
            dlHdmiRxSetState(instance, STATE_UNLOCKED);
            /* Activity has been lost, so switch back to DVI RGB 4:4:4, no pixelrepeat*/
            /* Reset Hdmi Flags status */
            instanceStatusInfo[instance].hdmiFlagsStatus = (UInt32) 0;
            /* The up/down sampler must be bypass*/
            gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureUpDownSampler(instance,
                    BSLHDMIRX_DOWNSAMPLE_BYPASS,
                    BSLHDMIRX_UPSAMPLE_BYPASS);
            /* Derepeater no pixel repeat*/
            gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigurePixelRepeater(instance, 0);
#endif

            /* Release the semaphore */
            tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

            unitTable[instance].pDigitalActivityCallback(TMDL_HDMIRX_ACTIVITY_LOST,
                    TMDL_HDMIRX_INPUT_HDMI_B,
                    TMDL_HDMIRX_SYNC_TYPE_SEPARATED);
            /* Take the semaphore */
            tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
        }
        break;

    case BSLHDMIRX_IRQSOURCE_RXDA:

        if ((dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_ACTIVITY_DETECTED) == TMDL_HDMIRX_EVENT_ENABLED) && (status == 1))
        {
            /* Release the semaphore */
            tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
            
            unitTable[instance].pDigitalActivityCallback(TMDL_HDMIRX_ACTIVITY_DETECTED,
                    TMDL_HDMIRX_INPUT_HDMI_A,
                    //TMDL_HDMIRX_SYNC_TYPE_SEPARATED);
                    TMDL_HDMIRX_SYNC_TYPE_NONE);
            /* Take the semaphore */
            tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
        }
        
        if ((dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_ACTIVITY_LOST) == TMDL_HDMIRX_EVENT_ENABLED) && (status == 0))
        {
#ifdef TMFL_TDA19972_FAMILY
            /* Set the state machine */
            
            dlHdmiRxSetState(instance, STATE_UNLOCKED);
            /* Activity has been lost, so switch back to DVI RGB 4:4:4, no pixelrepeat*/
            /* Reset Hdmi Flags status */
            instanceStatusInfo[instance].hdmiFlagsStatus = (UInt32) 0;
            /* The up/down sampler must be bypass*/
            gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureUpDownSampler(instance,
                    BSLHDMIRX_DOWNSAMPLE_BYPASS,
                    BSLHDMIRX_UPSAMPLE_BYPASS);
            /* Derepeater no pixel repeat*/
            gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigurePixelRepeater(instance, 0);
#endif

            /* Release the semaphore */
            tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

            unitTable[instance].pDigitalActivityCallback(TMDL_HDMIRX_ACTIVITY_LOST,
                    TMDL_HDMIRX_INPUT_HDMI_A,
                    TMDL_HDMIRX_SYNC_TYPE_NONE);
            //TMDL_HDMIRX_SYNC_TYPE_SEPARATED);
            /* Take the semaphore */
            tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
        }
        break;

    case BSLHDMIRX_IRQSOURCE_HDMI_LOCK:
        
        if (((dlHdmiRxGetVideoInput(instance) == TMDL_HDMIRX_INPUT_HDMI_A) ||
                (dlHdmiRxGetVideoInput(instance) == TMDL_HDMIRX_INPUT_HDMI_B)
            ) && (status == 1))
        {

#ifdef TMFL_TDA19972_FAMILY
            /* check sus_state */
            
            gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxReadI2C(
                (tmUnitSelect_t) instance,
                ( ((UInt16) 0) << 8 ) | ((UInt16) 0x21),
                1,
                &susState);
            if ((susState & 0x1f)!= SUS_LAST_STATE)
            {
                //printf ("------------ BAD SUS STATUS 0x%2x\n", i);
                
                break;
            }
#endif

            /* Set the state machine */
            
            dlHdmiRxSetState(instance, STATE_LOCKED);

            /* There is a new activity, the status for HDCP repeater state C5 must be reset */
            instanceStatusInfo[instance].stateC5Reached = False;

            

            /* detect the new resolution*/
            dlHdmiRxCheckResolutionSyncTimings(instance, &resolutionID);
            instanceStatusInfo[instance].pVideoInfo->detectedVideoResolution = resolutionID;

            

            if (dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_INPUT_LOCKED) == TMDL_HDMIRX_EVENT_ENABLED)
            {
                /* Release the semaphore */
                tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

                /* call the callback */
                unitTable[instance].pInfoCallback(TMDL_HDMIRX_INPUT_LOCKED);

                /* Take the semaphore */
                tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
            }

            /* Set the hdmi flags reset variable to force a callback of the status (for all HDMI_FLAGS events)*/
            instanceStatusInfo[instance].hdmiFlagsReset |= MASK_HDMISTATUS_RESET_ALL;

            /* Check the status of the HDMI_FLAGS register */
            gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInterruptStatus(instance,BSLHDMIRX_IRQSOURCE_FLAGS);

        }

        if (((dlHdmiRxGetVideoInput(instance) == TMDL_HDMIRX_INPUT_HDMI_A) ||
                (dlHdmiRxGetVideoInput(instance) == TMDL_HDMIRX_INPUT_HDMI_B)
            ) && (status == 0))
        {
            /* Set the state machine */
            
            dlHdmiRxSetState(instance, STATE_UNLOCKED);
            /* There is a no longer activity, we will consider that detected resolution is unknown */
            instanceStatusInfo[instance].pVideoInfo->detectedVideoResolution = TMDL_HDMIRX_VIDEORES_UNKNOWN;
        }
        break;


    case BSLHDMIRX_IRQSOURCE_STATE_C5:
        /*configure the repeater if required */
        if ((dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_REPEATER) == TMDL_HDMIRX_EVENT_ENABLED) && (status == 1))
        {
            /* HDCP repeater has reached state C5 of HDCP state machine */
            instanceStatusInfo[instance].stateC5Reached = True;

            /* Release the semaphore */
            tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

            /* call the callback */
            unitTable[instance].pInfoCallback(TMDL_HDMIRX_REPEATER);

            /* Take the semaphore */
            tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
        }
        break;

    case BSLHDMIRX_IRQSOURCE_DEEP_COLOR_MODE_CHANGED:
        /* Read the deep color mode */

        gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetDeepColorMode(instance,
                &pixelPackingPhase,
                &deepColorMode);

        switch(deepColorMode)
        {
        case BSLHDMIRX_DEEPCOLORMODE_24:
            if (dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_DEEP_COLOR_MODE_24BITS) == TMDL_HDMIRX_EVENT_ENABLED)
            {
                /* Release the semaphore */
                tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

                unitTable[instance].pInfoCallback(TMDL_HDMIRX_DEEP_COLOR_MODE_24BITS);

                /* Take the semaphore */
                tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
            }
            break;

        case BSLHDMIRX_DEEPCOLORMODE_30:
            if (dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_DEEP_COLOR_MODE_30BITS) == TMDL_HDMIRX_EVENT_ENABLED)
            {
                /* Release the semaphore */
                tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

                unitTable[instance].pInfoCallback(TMDL_HDMIRX_DEEP_COLOR_MODE_30BITS);

                /* Take the semaphore */
                tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
            }
            break;

        case BSLHDMIRX_DEEPCOLORMODE_36:
            if (dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_DEEP_COLOR_MODE_36BITS) == TMDL_HDMIRX_EVENT_ENABLED)
            {
                /* Release the semaphore */
                tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

                unitTable[instance].pInfoCallback(TMDL_HDMIRX_DEEP_COLOR_MODE_36BITS);

                /* Take the semaphore */
                tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
            }
            break;

        case BSLHDMIRX_DEEPCOLORMODE_48:
            if (dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_DEEP_COLOR_MODE_48BITS) == TMDL_HDMIRX_EVENT_ENABLED)
            {
                /* Release the semaphore */
                tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

                unitTable[instance].pInfoCallback(TMDL_HDMIRX_DEEP_COLOR_MODE_48BITS);

                /* Take the semaphore */
                tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
            }
            break;

        default:
            break;
        }
        break;

    case BSLHDMIRX_IRQSOURCE_AUDIO_PACKET:
        /* GetAudioStatus */
        gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetAudioStatus(instance, &audioStatus);

        if (( (audioStatus & MASK_AUDIO_FLAGS_ALL) == BSLHDMIRX_AUDIO_STATUS_DST_PACKET) &&
                (dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_AUDIO_DST_PACKET_DETECTED) == TMDL_HDMIRX_EVENT_ENABLED))
        {
            /* Release the semaphore */
            tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

            unitTable[instance].pInfoCallback(TMDL_HDMIRX_AUDIO_DST_PACKET_DETECTED);

            /* Take the semaphore */
            tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
        }

        if (( (audioStatus & MASK_AUDIO_FLAGS_ALL) == BSLHDMIRX_AUDIO_STATUS_OBA_PACKET) &&
                (dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_AUDIO_OBA_PACKET_DETECTED) == TMDL_HDMIRX_EVENT_ENABLED))
        {
            /* Release the semaphore */
            tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

            unitTable[instance].pInfoCallback(TMDL_HDMIRX_AUDIO_OBA_PACKET_DETECTED);

            /* Take the semaphore */
            tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
        }

        if (( (audioStatus & MASK_AUDIO_FLAGS_ALL) == BSLHDMIRX_AUDIO_STATUS_HBR_PACKET) &&
                (dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_AUDIO_HBR_PACKET_DETECTED) == TMDL_HDMIRX_EVENT_ENABLED))
        {
            /* Release the semaphore */
            tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

            unitTable[instance].pInfoCallback(TMDL_HDMIRX_AUDIO_HBR_PACKET_DETECTED);

            /* Take the semaphore */
            tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
        }

        if (( (audioStatus & MASK_AUDIO_FLAGS_ALL) == BSLHDMIRX_AUDIO_STATUS_AUDIO_SAMPLE_PACKET) &&
                (dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_AUDIO_SAMPLE_PACKET_DETECTED) == TMDL_HDMIRX_EVENT_ENABLED))
        {
            /* Release the semaphore */
            tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

            unitTable[instance].pInfoCallback(TMDL_HDMIRX_AUDIO_SAMPLE_PACKET_DETECTED);

            /* Take the semaphore */
            tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
        }
        break;

    case BSLHDMIRX_IRQSOURCE_AUDIO_SAMPLE_FREQ_CHANGED:
        if (dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_AUDIO_SAMPLE_FREQ_CHANGED) == TMDL_HDMIRX_EVENT_ENABLED)
        {
            /* Release the semaphore */
            tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

            /* call the callback */
            unitTable[instance].pInfoCallback(TMDL_HDMIRX_AUDIO_SAMPLE_FREQ_CHANGED);

            /* Take the semaphore */
            tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
        }
        break;

    case BSLHDMIRX_IRQSOURCE_AUDIO_FIFO_UNMUTED:
        if (dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_AUDIO_UNMUTED) == TMDL_HDMIRX_EVENT_ENABLED)
        {
            /* Release the semaphore */
            tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

            /* call the callback */
            unitTable[instance].pInfoCallback(TMDL_HDMIRX_AUDIO_UNMUTED);

            /* Take the semaphore */
            tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
        }

        /* Check the status of the AUDIO_FLAGS register */
        gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInterruptStatus(instance,BSLHDMIRX_IRQSOURCE_AUDIO_PACKET);
        break;

#ifdef TMFL_TDA19972_FAMILY
    case BSLHDMIRX_IRQSOURCE_AUDIO_CHANNEL_STATUS:
        if (dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_AUDIO_CHANNEL_STATUS) == TMDL_HDMIRX_EVENT_ENABLED)
        {
            /* call the callback */
            /* Release the semaphore */
            tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

            /* call the callback */
            unitTable[instance].pInfoCallback(TMDL_HDMIRX_AUDIO_CHANNEL_STATUS);

            /* Take the semaphore */
            tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
        }
        break;
#endif

#ifdef TMFL_HDMI_OUT
    case BSLHDMIRX_IRQSOURCE_HPD:

        /* call the callback with the good event*/

        hpdStatus = HDMIRX_HOTPLUG_INVALID;

        /* Get Hot Plug status */
        errCode = tmbslHdmiRxHdmioutHotPlugGetStatus(instance,
                  &hpdStatus,True);

        if (errCode != TM_OK) return;

        /* Has hot plug changed to Active? */
        if (hpdStatus == HDMIRX_HOTPLUG_ACTIVE)
        {
            /* Set state machine to Plugged */
            unitTable[instance].EdidState = STATE_PLUGGED;

            if (dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_HDMIOUT_HPD_ACTIVE) == TMDL_HDMIRX_EVENT_ENABLED)
            {
                /* Release the sempahore */
                tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
                unitTable[instance].pHdmioutCallback(TMDL_HDMIRX_HDMIOUT_HPD_ACTIVE);
                /* Take the sempahore */
                tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
            }

            if (unitTable[instance].powerState == tmPowerOn)
            {
                /* Yes: Wait for DDC line to settle before reading EDID */
                errCode = tmdlHdmiRxIWWait(500); /* ms */

                /* Request EDID read */
                errCode &= tmbslHdmiRxHdmioutEdidRequestBlockData(instance,
                           unitTable[instance].pEdidBuffer, (Int)((unitTable[instance].edidBufferSize) >> 7),
                           (Int)(unitTable[instance].edidBufferSize));

                if (errCode != TM_OK) return;
            }
        }
        else
        {
            /* Set state machine to Unplugged */
            unitTable[instance].EdidState = STATE_UNPLUGGED;

            if (dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_HDMIOUT_HPD_INACTIVE) == TMDL_HDMIRX_EVENT_ENABLED)
            {
                /* Release the sempahore */
                tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
                unitTable[instance].pHdmioutCallback(TMDL_HDMIRX_HDMIOUT_HPD_INACTIVE);
                /* Take the sempahore */
                tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
            }
        }
        break;

    case BSLHDMIRX_IRQSOURCE_RXS:

        /* call the callback  with the good event*/
        errCode = tmbslHdmiRxHdmioutRxSenseGetStatus(instance,
                  &rxSenseStatus,True);

        if (errCode != TM_OK) return;

        errCode = tmbslHdmiRxHdmioutHotPlugGetStatus(instance,
                  &hpdStatus,False);

        if (errCode != TM_OK) return;

        if (rxSenseStatus == HDMIRX_RX_SENSE_ACTIVE)
        {
            if (dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_HDMIOUT_RX_DEVICE_ACTIVE) == TMDL_HDMIRX_EVENT_ENABLED)
            {
                /* Release the sempahore */
                tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
                unitTable[instance].pHdmioutCallback(TMDL_HDMIRX_HDMIOUT_RX_DEVICE_ACTIVE);
                /* Take the sempahore */
                tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
            }
        }
        else if (rxSenseStatus == HDMIRX_RX_SENSE_INACTIVE)
        {

            if (dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_HDMIOUT_RX_DEVICE_INACTIVE) == TMDL_HDMIRX_EVENT_ENABLED)
            {
                /* Release the sempahore */
                tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
                unitTable[instance].pHdmioutCallback(TMDL_HDMIRX_HDMIOUT_RX_DEVICE_INACTIVE);
                /* Take the sempahore */
                tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
            }
        }
        break;

    case BSLHDMIRX_IRQSOURCE_EDID_READ:

        /* Get the edid status and read the connected device's EDID */

        /* Get Edid status */
        errCode = tmbslHdmiRxHdmioutEdidGetStatus(instance,
                  &edidStatus);

        if (errCode != TM_OK)
        {
            /* Set state machine to Plugged */
            unitTable[instance].EdidState = STATE_PLUGGED;
            return;
        }

        /* Has hot plug changed to Active? */
        if ((edidStatus == TMDL_HDMIRX_EDID_READ) ||
                (edidStatus == TMDL_HDMIRX_EDID_ERROR_CHK))
        {
            /* Set state machine to EDID available */
            unitTable[instance].EdidState = STATE_EDID_AVAILABLE;
        }
        else
        {
            /* Set state machine to Plugged */
            unitTable[instance].EdidState = STATE_PLUGGED;
        }


        if (dlHdmiRxGetEventStatus(instance, TMDL_HDMIRX_HDMIOUT_EDID_RECEIVED) == TMDL_HDMIRX_EVENT_ENABLED)
        {
            /* Release the sempahore */
            tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
            unitTable[instance].pHdmioutCallback(TMDL_HDMIRX_HDMIOUT_EDID_RECEIVED);
            /* Take the sempahore */
            tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
        }
        break;
#endif


    default:
        break;
        /* !! TBD, For the moment, there are no corresponding callbacks for:
                    BSLHDMIRX_IRQSOURCE_ASD
                    BSLHDMIRX_IRQSOURCE_OTP
                    BSLHDMIRX_IRQSOURCE_AUDIO_MUTE
        */

    }
}

/******************************************************************************
    \brief Manage the callback rised by the BSL for unit/instance 0

    \param IrqSource    Source of the callback

    \return NA

******************************************************************************/
static void dlHdmiRxManageCallback0(tmbslHdmiRxIRQSource_t irqSource, UInt8 status)
{
    dlHdmiRxManageCallbackInstance(irqSource, 0, status);
}

static void dlHdmiRxManageCallback1(tmbslHdmiRxIRQSource_t irqSource, UInt8 status)
{
    dlHdmiRxManageCallbackInstance(irqSource, 1, status);
}

#ifndef TMFL_NO_RTOS
/**
    \brief Command processing task, dedicated to unit/instance 0.
******************************************************************************/
static void CommandTaskUnit0()
{
    UInt8   command;
    Bool    loop = True;    /* Just to avoid compiler warning */

    while(loop)
    {
        tmdlHdmiRxIWQueueReceive(unitTable[0].queueHandle, &command);

        /* Take the semaphore */
        tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[0]);

        if( (instanceStatusInfo[0].pVideoInfo->videoInput != TMDL_HDMIRX_INPUT_TEST_480P) &&
#ifdef TMFL_TDA19972_FAMILY
                (instanceStatusInfo[0].pVideoInfo->videoInput != TMDL_HDMIRX_INPUT_TEST_480I) &&
                (instanceStatusInfo[0].pVideoInfo->videoInput != TMDL_HDMIRX_INPUT_TEST_576I) &&
                (instanceStatusInfo[0].pVideoInfo->videoInput != TMDL_HDMIRX_INPUT_RAMPTEST_480P) &&
                (instanceStatusInfo[0].pVideoInfo->videoInput != TMDL_HDMIRX_INPUT_RAMPTEST_576P) &&
                (instanceStatusInfo[0].pVideoInfo->videoInput != TMDL_HDMIRX_INPUT_RAMPTEST_480I) &&
                (instanceStatusInfo[0].pVideoInfo->videoInput != TMDL_HDMIRX_INPUT_RAMPTEST_576I) &&
#endif
                (instanceStatusInfo[0].pVideoInfo->videoInput != TMDL_HDMIRX_INPUT_TEST_576P) )
        {
            gtmdlHdmiRxDriverConfigTable[0].ptmbslHdmiRxFunctionsList->tmbslHdmiRxHandleInterrupt(0);

            /* Enable interrupts for Rx (interrupts are disabled in the HandleInterrupt function) */
            /* Interrupts must only be re-enabled if the interrupts have been cleared on the Rx */
            /* =>  (done by BSL HandleInterrupt) */
            tmdlHdmiRxIWEnableInterrupts(TMDL_HDMI_IW_RX_1);
        }


        /* Release the semaphore */
        tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[0]);
    };
}
#endif

/**
    \brief Recover the channel assignment from the audio infoframe.
******************************************************************************/
static void dlHdmiRxRecoverChannelAssignment
(
    tmInstance_t           instance,
    UInt8                  readChannelAssignment
)
{
    tmbslHdmiRxAudioOutputFormat_t   outputFormat = BSLHDMIRX_AUDIOOUTPUT_I2S;
    tmbslHdmiRxAudioTestTone_t       testTone     = BSLHDMIRX_TESTTONE_OFF;
    tmbslHdmiRxAudioPathSpFlagMode_t spFlagMode   = BSLHDMIRX_AUDIOPATH_SP_FLAG_USED_BY_FIFO;


    if(readChannelAssignment != instanceStatusInfo[instance].pChannelAssignmentInfo->currentChannelAssignment)
    {
        instanceStatusInfo[instance].pChannelAssignmentInfo->currentChannelAssignment = readChannelAssignment;

        /* Test the audio mute state */
        if(instanceStatusInfo[instance].pMuteAndVideoPort->audioMuteState == False)
        {

            /* Test tone must be enabled if video input is test color bar */
            if ((instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_480P) ||
#ifdef TMFL_TDA19972_FAMILY
                    (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_480I) ||
                    (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_576I) ||
                    (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_480P) ||
                    (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_576P) ||
                    (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_480I) ||
                    (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_RAMPTEST_576I) ||
#endif
                    (instanceStatusInfo[instance].pVideoInfo->videoInput == TMDL_HDMIRX_INPUT_TEST_576P))
            {
                testTone = BSLHDMIRX_TESTTONE_ON;
            }

            /* Test the audio format */
            switch(instanceStatusInfo[instance].pMuteAndVideoPort->audioFormat)
            {
            case TMDL_HDMIRX_AUDIOFORMAT_I2S16:
            /* I2S 16 bits */
            case TMDL_HDMIRX_AUDIOFORMAT_I2S32:
            /* I2S 32 bits */
            case TMDL_HDMIRX_AUDIOFORMAT_OBA:
                /* One Bit Audio */
                outputFormat  = BSLHDMIRX_AUDIOOUTPUT_I2S;
                spFlagMode    = BSLHDMIRX_AUDIOPATH_SP_FLAG_USED_BY_FIFO;
                break;

            case TMDL_HDMIRX_AUDIOFORMAT_SPDIF:
                /* SPDIF */
                outputFormat  = BSLHDMIRX_AUDIOOUTPUT_SPDIF;
                spFlagMode    = BSLHDMIRX_AUDIOPATH_SP_FLAG_USED_BY_FIFO;
                break;

            case TMDL_HDMIRX_AUDIOFORMAT_I2S16_HBR_DEMUX:
            /* I2S 16 bits High Bit Rate demux */
            case TMDL_HDMIRX_AUDIOFORMAT_I2S32_HBR_DEMUX:
            /* I2S 32 bits High Bit Rate demux */
            case TMDL_HDMIRX_AUDIOFORMAT_DST:
                /* Direct Stream Transfer */
                outputFormat  = BSLHDMIRX_AUDIOOUTPUT_I2S;
                spFlagMode    = BSLHDMIRX_AUDIOPATH_SP_FLAG_IGNORED_BY_FIFO;
                break;

            case TMDL_HDMIRX_AUDIOFORMAT_SPDIF_HBR_DEMUX:
                /* SPDIF High Bit Rate demux */
                outputFormat  = BSLHDMIRX_AUDIOOUTPUT_SPDIF;
                spFlagMode    = BSLHDMIRX_AUDIOPATH_SP_FLAG_IGNORED_BY_FIFO;
                break;
            default:
                break;
            }

            /* ConfigureAudioFormatter */
            gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureAudioFormatter(instance,
                    spFlagMode,
                    instanceStatusInfo[instance].pChannelAssignmentInfo->currentChannelAssignment,
                    BSLHDMIRX_AUDIOPATH_LAYOUT_PACKET_HEADER,
                    BSLHDMIRX_AUDIO_LAYOUT_FORCED_0,
                    FIFO_LATENCY,
                    testTone,
                    outputFormat,
                    BSLHDMIRX_AUDIOOUTPUT_NORMAL,
                    BSLHDMIRX_AUDIOOUTPUT_NORMAL,
                    BSLHDMIRX_AUDIOOUTPUT_NORMAL,
                    BSLHDMIRX_AUDIOOUTPUT_NORMAL,
                    BSLHDMIRX_AUDIOOUTPUT_NORMAL,
                    BSLHDMIRX_AUDIOOUTPUT_NORMAL,
                    BSLHDMIRX_AUDIOOUTPUT_NORMAL,
                    BSLHDMIRX_AUDIOOUTPUT_NORMAL,
                    BSLHDMIRX_AUDIOOUTPUT_NORMAL,
                    BSLHDMIRX_AUDIOOUTPUT_NORMAL);
        }

        /* Reset the audio FIFO */
        gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxHDMISoftReset(instance,
                instanceStatusInfo[instance].nackHdcp,
                BSLHDMIRX_DONTRESET_AI,
                BSLHDMIRX_DONTRESET_INFOFRAME,
                BSLHDMIRX_RESET_AUDIO_FIFO,
                BSLHDMIRX_DONTRESET_GAMUT,
                BSLHDMIRX_RESET_FIFOCTRL,
#ifdef ALLOW_DDC_ACK
                BSLHDMIRX_DONTRESET_SUS_DDC,
#else
                BSLHDMIRX_DONTRESET_SUS,
#endif
                BSLHDMIRX_DONTRESET_DC);

//        printf("SR2\n");

        gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxHDMISoftReset(instance,
                instanceStatusInfo[instance].nackHdcp,
                BSLHDMIRX_DONTRESET_AI,
                BSLHDMIRX_DONTRESET_INFOFRAME,
                BSLHDMIRX_DONTRESET_AUDIO_FIFO,
                BSLHDMIRX_DONTRESET_GAMUT,
                BSLHDMIRX_DONTRESET_FIFOCTRL,
#ifdef ALLOW_DDC_ACK
                BSLHDMIRX_DONTRESET_SUS_DDC,
#else
                BSLHDMIRX_DONTRESET_SUS,
#endif
                BSLHDMIRX_DONTRESET_DC);
    }

}


#include "tmbslTDA1997X_local.h"

void reportLinesPixels()
// read and report how many lines and pixels the NXP sees
{

    tmbslHdmiRxAsdMeasureInterlaced_t   pInterlaced;
    tmbslHdmiRxVhrefAsdLineStandard_t   pLineMatch;
    tmbslHdmiRxVhrefAsdMeaslin525_t     pFrameFormat;
    UInt16                              pLines;
    UInt16                              pPixels;

    tmErrorCode_t               errCode;
    tmInstance_t             instance=0;

    errCode = tmbslTDA1997XGetFrameMeasurements(instance,&pInterlaced,&pLineMatch,&pFrameFormat,&pLines,&pPixels);

    sprintf(Msg,"L %d P %d I %d",pLines,pPixels,pInterlaced);
    WriteLn(Msg);

    return;

    UInt8           pTabRegValue[7];
    UInt8           pData[20];

    //errCode =  tmbslTDA1997XReadI2C(instance, (UInt16)V_PER_MSB, 7, pTabRegValue);
    //RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

    errCode =  tmbslTDA1997XReadI2C(instance, (UInt16)0x18, 20, pData); 
    //RETIF(errCode !=TM_OK,TMBSL_ERR_BSLHDMIRX_I2C_READ)

    for (int i=0; i<20; i=i+4)
    {
        sprintf(Msg,"%x %x %x %x",pData[i],pData[i+1],pData[i+2],pData[i+3]);
        WriteLn(Msg);
    }

}

/**
    \brief Check the resolution sync timings.
******************************************************************************/
static tmErrorCode_t dlHdmiRxCheckResolutionSyncTimings
(
    tmInstance_t             instance,
    tmdlHdmiRxResolutionID_t *pResolutionID
)
{
    tmErrorCode_t               errCode;
    UInt32                      verticalPeriod = 0;
    UInt16                      horizontalPeriod = 0;
    UInt16                      hsWidth = 0;
    UInt8                       vPerCmp = 0, hPerCmp = 0, hsWidthCmp = 0;
    UInt8                       i, find = 0;
#ifdef SUPPORT_3D_FP
    UInt8                       vBlk = 0;
#endif
    UInt16                       h_act = 0;

    /* Structure to be used to get the format parameters for determining the
     * resolution with accuracy.
     */
    tmbslHdmiRxFormatMeas_t     extendedFormatParam;

    /* by default, no resolution detected yet*/
    *pResolutionID = TMDL_HDMIRX_VIDEORES_UNKNOWN;

    /* GetSyncTimings */
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetSyncTimings(instance,
              &verticalPeriod,
              &horizontalPeriod,
              &hsWidth,
              &extendedFormatParam);
    RETIF(errCode !=TM_OK, errCode);

    //#ifdef HDMI_DEBUG
    //sprintf(Msg,"V %lx",verticalPeriod);
    //WriteLn(Msg);
////
    //sprintf(Msg,"H %x",horizontalPeriod);
    //WriteLn(Msg);
////
    //sprintf(Msg,"HP %x",hsWidth);
    //WriteLn(Msg);
    //#endif
    //
    //sprintf(Msg,"VPER %d, HPER %d\n",verticalPeriod, horizontalPeriod);
    //WriteLn(Msg);
//
    //sprintf(Msg,"HTOT %d, HACT %d, HFRONT %d, HSYNC %d HBACK %d\n", extendedFormatParam.horizontalTotalPeriod, extendedFormatParam.horizontalVideoActiveWidth,
    //extendedFormatParam.horizontalFrontPorchWidth, extendedFormatParam.horizontalSyncWidthPixClk,
    //extendedFormatParam.horizontalBackPorchWidth);
    //
    //WriteLn(Msg);
    //
    //sprintf(Msg,"VTOT %d, VACT %d, VFRONT1 %d, VSYNC %d VBACK1 %d\n", extendedFormatParam.verticalTotalPeriod, extendedFormatParam.verticalVideoActiveWidth,
    //extendedFormatParam.verticalFrontPorchWidthF1, extendedFormatParam.verticalSyncWidth,
    //extendedFormatParam.verticalBackPorchWidthF1);
    //
    //WriteLn(Msg);
    //
    if ((extendedFormatParam.verticalFrontPorchWidthF2!=0)||(extendedFormatParam.verticalBackPorchWidthF2!=0))
    {
        sprintf(Msg,"WINDOWS is actve!!\n");
        WriteLn(Msg);
    }


    //reportLinesPixels();

#ifdef __LINUX_ARM_ARCH__
    /*     printk("DBG resolutionToBeDetectedNb:%d\n",gtmdlHdmiRxDriverConfigTable[instance].resolutionToBeDetectedNb); */
    /*     printk("DBG rx v:%ld h:%d hs:%d vfp:%d fsw:%d vbp:%d\n",verticalPeriod,horizontalPeriod,hsWidth, \ */
    /*            extendedFormatParam.verticalFrontPorchWidthF1, \ */
    /*            extendedFormatParam.verticalSyncWidth, \ */
    /*            extendedFormatParam.verticalBackPorchWidthF1); */
#endif

    for(i = 0; i < gtmdlHdmiRxDriverConfigTable[instance].resolutionToBeDetectedNb; i++)
    {
        vPerCmp = (UInt8) ( (verticalPeriod >= gtmdlHdmiRxDriverConfigTable[instance].pResolutionToBeDetectedList[i].verticalPeriodMin) &&
                            (verticalPeriod <= gtmdlHdmiRxDriverConfigTable[instance].pResolutionToBeDetectedList[i].verticalPeriodMax) );

        hPerCmp = (UInt8) ( (horizontalPeriod >= gtmdlHdmiRxDriverConfigTable[instance].pResolutionToBeDetectedList[i].horizontalPeriodMin) &&
                            (horizontalPeriod <= gtmdlHdmiRxDriverConfigTable[instance].pResolutionToBeDetectedList[i].horizontalPeriodMax) );

        hsWidthCmp = (UInt8) ( (hsWidth >= gtmdlHdmiRxDriverConfigTable[instance].pResolutionToBeDetectedList[i].hsWidthMin) &&
                               (hsWidth <= gtmdlHdmiRxDriverConfigTable[instance].pResolutionToBeDetectedList[i].hsWidthMax) );

#ifdef __LINUX_ARM_ARCH__
        /*         printk("DBG v:%dl-h:%d-w:%d - table v:(%ld,%ld)-h:(%d,%d)-w:(%d,%d)\n",vPerCmp,hPerCmp,hsWidthCmp, \ */
        /*                gtmdlHdmiRxDriverConfigTable[instance].pResolutionToBeDetectedList[i].verticalPeriodMin, \ */
        /*                gtmdlHdmiRxDriverConfigTable[instance].pResolutionToBeDetectedList[i].verticalPeriodMax, \ */
        /*                gtmdlHdmiRxDriverConfigTable[instance].pResolutionToBeDetectedList[i].horizontalPeriodMin, \ */
        /*                gtmdlHdmiRxDriverConfigTable[instance].pResolutionToBeDetectedList[i].horizontalPeriodMax, \ */
        /*                gtmdlHdmiRxDriverConfigTable[instance].pResolutionToBeDetectedList[i].hsWidthMin, \ */
        /*                gtmdlHdmiRxDriverConfigTable[instance].pResolutionToBeDetectedList[i].hsWidthMax); */
#endif
        if(vPerCmp && hPerCmp && hsWidthCmp)
        {
            find = 1;
            //WriteLn("res found");
#ifdef SUPPORT_3D_FP
            vBlk = extendedFormatParam.verticalFrontPorchWidthF1 + extendedFormatParam.verticalSyncWidth + extendedFormatParam.verticalBackPorchWidthF1;
#endif
            h_act = extendedFormatParam.horizontalVideoActiveWidth;
#ifdef __LINUX_ARM_ARCH__
            /*             printk("DBG vBlk:%d\n",vBlk); */
#endif
            break;
        }
    }

    if(find == 0)
    {
        //printf("V= %d, H= %d, W= %d, VTOT= %d\n",verticalPeriod ,horizontalPeriod ,hsWidth, extendedFormatParam.verticalTotalPeriod);
        return TMDL_ERR_DLHDMIRX_RESOLUTION_NOT_DETECTED;
    }

    *pResolutionID = gtmdlHdmiRxDriverConfigTable[instance].pResolutionToBeDetectedList[i].resolutionID;

#ifdef SUPPORT_3D_FP
    /* Check and rectify resolution detection for some conflicting 3D FP formats */
#endif
    /* other resolution detection conflict to be resolved */
    if(*pResolutionID == TMDL_HDMIRX_VIDEORES_1680_1050p_60HZ_RB)
    {
        if(h_act == 1400)
            *pResolutionID = TMDL_HDMIRX_VIDEORES_1400_1050p_60HZ_RB;
    }
    else if(*pResolutionID == TMDL_HDMIRX_VIDEORES_1400_1050p_60HZ_RB)
    {
        if(h_act == 1680)
            *pResolutionID = TMDL_HDMIRX_VIDEORES_1680_1050p_60HZ_RB;
    }


    return TM_OK;
}

/**
    \brief Set the state of the state machine.
******************************************************************************/
static void dlHdmiRxSetState
(
    tmInstance_t             instance,
    tmdlHdmiRxDriverState_t  state
)
{
    /* Set the state */
    unitTable[instance].state = state;
}


/**
    \brief Get the state of the state machine.
******************************************************************************/
static tmdlHdmiRxDriverState_t dlHdmiRxGetState
(
    tmInstance_t             instance
)
{
    tmdlHdmiRxDriverState_t state;


    /* Get the state */
    state = unitTable[instance].state;

    return (state);
}



/**
    \brief Get the status of the event (enabled or disabled).
******************************************************************************/
static tmdlHdmiRxEventStatus_t dlHdmiRxGetEventStatus
(
    tmInstance_t             instance,
    tmdlHdmiRxEvent_t        event
)
{
    tmdlHdmiRxEventStatus_t   eventStatus;


    /* Get the event status */
    eventStatus = instanceStatusInfo[instance].pEventState[event].status;

    return (eventStatus);
}



/**
    \brief Get the selected video input.
******************************************************************************/
static tmdlHdmiRxInput_t dlHdmiRxGetVideoInput
(
    tmInstance_t             instance
)
{
    tmdlHdmiRxInput_t videoInput;


    /* Get the selected video input */
    videoInput = instanceStatusInfo[instance].pVideoInfo->videoInput;

    return (videoInput);
}

/**
    \brief Recover the interrupt status from the Bsl
 ******************************************************************************/
static tmErrorCode_t dlHdmiRxGetInterruptStatus
(
    tmInstance_t            instance,
    tmdlHdmiRxEvent_t       statusEvent
)
{
    tmErrorCode_t           errCode = TM_OK;
    tmbslHdmiRxIRQSource_t  irqSource;

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)


    if (dlHdmiRxGetEventStatus(instance, statusEvent) == TMDL_HDMIRX_EVENT_ENABLED)
    {
        switch(statusEvent)
        {
        case TMDL_HDMIRX_ACTIVITY_DETECTED :
            if ((gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion != TMDL_HDMIRX_DEVICE_TDA19971) &&
                    (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion != TMDL_HDMIRX_DEVICE_TDA19971N2) &&
                    (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion != TMDL_HDMIRX_DEVICE_TDA19972) &&
                    (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion != TMDL_HDMIRX_DEVICE_TDA19973_SOC_IN) &&
                    (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion != TMDL_HDMIRX_DEVICE_TDA19973_CONNECTOR_IN) &&
                    (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion != TMDL_HDMIRX_DEVICE_TDA19973N2_SOC_IN) &&
                    (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion != TMDL_HDMIRX_DEVICE_TDA19973N2_CONNECTOR_IN) &&
                    (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion != TMDL_HDMIRX_DEVICE_TDA19974) &&
                    (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion != TMDL_HDMIRX_DEVICE_TDA19978) &&
                    (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion != TMDL_HDMIRX_DEVICE_TDA19977) &&
                    (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion != TMDL_HDMIRX_DEVICE_TDA19979))
            {
                irqSource = BSLHDMIRX_IRQSOURCE_SOGD2;
                
                errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInterruptStatus(
                              instance,
                              irqSource);
                RETIF(errCode != TM_OK, errCode)

                irqSource = BSLHDMIRX_IRQSOURCE_DCSD2;
                
                errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInterruptStatus(
                              instance,
                              irqSource);
                RETIF(errCode != TM_OK, errCode)

                irqSource = BSLHDMIRX_IRQSOURCE_DSSD2;
                
                errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInterruptStatus(
                              instance,
                              irqSource);
                RETIF(errCode != TM_OK, errCode)

                irqSource = BSLHDMIRX_IRQSOURCE_SOGD1;
                
                errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInterruptStatus(
                              instance,
                              irqSource);
                RETIF(errCode != TM_OK, errCode)

                irqSource = BSLHDMIRX_IRQSOURCE_DCSD1;
                
                errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInterruptStatus(
                              instance,
                              irqSource);
                RETIF(errCode != TM_OK, errCode)

                irqSource = BSLHDMIRX_IRQSOURCE_DSSD1;
                
                errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInterruptStatus(
                              instance,
                              irqSource);
                RETIF(errCode != TM_OK, errCode)

                irqSource = BSLHDMIRX_IRQSOURCE_SOGD3;
                
                errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInterruptStatus(
                              instance,
                              irqSource);
                RETIF(errCode != TM_OK, errCode)

                irqSource = BSLHDMIRX_IRQSOURCE_DCSD3;
                
                errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInterruptStatus(
                              instance,
                              irqSource);
                RETIF(errCode != TM_OK, errCode)

                irqSource = BSLHDMIRX_IRQSOURCE_DSSD3;
                
                errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInterruptStatus(
                              instance,
                              irqSource);
                
                RETIF(errCode != TM_OK, errCode)
            }

            if ((gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion != TMDL_HDMIRX_DEVICE_TDA19971) &&
                    (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion != TMDL_HDMIRX_DEVICE_TDA19971N2))
            {
                irqSource = BSLHDMIRX_IRQSOURCE_RXDB;
                errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInterruptStatus(
                              instance,
                              irqSource);
                RETIF(errCode != TM_OK, errCode)
            }

            irqSource = BSLHDMIRX_IRQSOURCE_RXDA;
            errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInterruptStatus(
                          instance,
                          irqSource);
            
            RETIF(errCode != TM_OK, errCode)

            if ((gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion != TMDL_HDMIRX_DEVICE_TDA19971) &&
                    (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion != TMDL_HDMIRX_DEVICE_TDA19971N2) &&
                    (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion != TMDL_HDMIRX_DEVICE_TDA19972) &&
                    (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion != TMDL_HDMIRX_DEVICE_TDA19973_SOC_IN) &&
                    (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion != TMDL_HDMIRX_DEVICE_TDA19973N2_SOC_IN) &&
                    (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion != TMDL_HDMIRX_DEVICE_TDA19973_CONNECTOR_IN) &&
                    (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion != TMDL_HDMIRX_DEVICE_TDA19973N2_CONNECTOR_IN))
            {
                irqSource = BSLHDMIRX_IRQSOURCE_RXDC;
                errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInterruptStatus(
                              instance,
                              irqSource);
                RETIF(errCode != TM_OK, errCode)

                irqSource = BSLHDMIRX_IRQSOURCE_RXDD;
                errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInterruptStatus(
                              instance,
                              irqSource);
                RETIF(errCode != TM_OK, errCode)
            }
            break;

        case TMDL_HDMIRX_INPUT_LOCKED  :
            if ((gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion != TMDL_HDMIRX_DEVICE_TDA19971) &&
                    (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion != TMDL_HDMIRX_DEVICE_TDA19971N2) &&
                    (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion != TMDL_HDMIRX_DEVICE_TDA19972) &&
                    (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion != TMDL_HDMIRX_DEVICE_TDA19973_SOC_IN) &&
                    (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion != TMDL_HDMIRX_DEVICE_TDA19973N2_SOC_IN) &&
                    (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion != TMDL_HDMIRX_DEVICE_TDA19973_CONNECTOR_IN) &&
                    (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion != TMDL_HDMIRX_DEVICE_TDA19973N2_CONNECTOR_IN) &&
                    (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion != TMDL_HDMIRX_DEVICE_TDA19974) &&
                    (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion != TMDL_HDMIRX_DEVICE_TDA19978) &&
                    (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion != TMDL_HDMIRX_DEVICE_TDA19977) &&
                    (gtmdlHdmiRxDriverConfigTable[instance].pCapabilitiesList->deviceVersion != TMDL_HDMIRX_DEVICE_TDA19979))
            {
                irqSource = BSLHDMIRX_IRQSOURCE_PLL_LOCKED;
                
                errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInterruptStatus(
                              instance,
                              irqSource);
                RETIF(errCode != TM_OK, errCode)
            }
            irqSource = BSLHDMIRX_IRQSOURCE_HDMI_LOCK;
            
            errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInterruptStatus(
                          instance,
                          irqSource);
            RETIF(errCode != TM_OK, errCode)
            break;

        case TMDL_HDMIRX_HDMI_DETECTED:
            /* Set the corresponding reset flag to 1 to force a callback of the status */
            instanceStatusInfo[instance].hdmiFlagsReset |= MASK_HDMISTATUS_HDMI_RESET;

            irqSource = BSLHDMIRX_IRQSOURCE_FLAGS;
            
            errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInterruptStatus(
                          instance,
                          irqSource);
            RETIF(errCode != TM_OK, errCode)
            break;

        case TMDL_HDMIRX_DVI_DETECTED:
            /* Set the corresponding reset flag to 1 to force a callback of the status */
            instanceStatusInfo[instance].hdmiFlagsReset |= MASK_HDMISTATUS_DVI_RESET;

            irqSource = BSLHDMIRX_IRQSOURCE_FLAGS;
            
            errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInterruptStatus(
                          instance,
                          irqSource);
            RETIF(errCode != TM_OK, errCode)
            break;

        case TMDL_HDMIRX_ENCRYPTION_DETECTED:
            /* Set the corresponding reset flag to 1 to force a callback of the status */
            instanceStatusInfo[instance].hdmiFlagsReset |= MASK_HDMISTATUS_ENCRYPTION_RESET;

            irqSource = BSLHDMIRX_IRQSOURCE_FLAGS;
            errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInterruptStatus(
                          instance,
                          irqSource);
            RETIF(errCode != TM_OK, errCode)
            break;

        case TMDL_HDMIRX_NO_ENCRYPTION_DETECTED:
            /* Set the corresponding reset flag to 1 to force a callback of the status */
            instanceStatusInfo[instance].hdmiFlagsReset |= MASK_HDMISTATUS_NO_ENCRYPTION_RESET;

            irqSource = BSLHDMIRX_IRQSOURCE_FLAGS;
            errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInterruptStatus(
                          instance,
                          irqSource);
            RETIF(errCode != TM_OK, errCode)
            break;

        case TMDL_HDMIRX_EESS_HDCP_DETECTED:
            /* Set the corresponding reset flag to 1 to force a callback of the status */
            instanceStatusInfo[instance].hdmiFlagsReset |= MASK_HDMISTATUS_EESS_RESET;

            irqSource = BSLHDMIRX_IRQSOURCE_FLAGS;
            errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInterruptStatus(
                          instance,
                          irqSource);
            RETIF(errCode != TM_OK, errCode)
            break;

        case TMDL_HDMIRX_OESS_HDCP_DETECTED:
            /* Set the corresponding reset flag to 1 to force a callback of the status */
            instanceStatusInfo[instance].hdmiFlagsReset |= MASK_HDMISTATUS_OESS_RESET;

            irqSource = BSLHDMIRX_IRQSOURCE_FLAGS;
            errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInterruptStatus(
                          instance,
                          irqSource);
            RETIF(errCode != TM_OK, errCode)
            break;

        case TMDL_HDMIRX_AVI_AVMUTE_ACTIVE:
            /* Set the corresponding reset flag to 1 to force a callback of the status */
            instanceStatusInfo[instance].hdmiFlagsReset |= MASK_HDMISTATUS_AVMUTE_ACTIVE_RESET;

            irqSource = BSLHDMIRX_IRQSOURCE_FLAGS;
            errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInterruptStatus(
                          instance,
                          irqSource);
            RETIF(errCode != TM_OK, errCode)
            break;

        case TMDL_HDMIRX_AVI_AVMUTE_INACTIVE:
            /* Set the corresponding reset flag to 1 to force a callback of the status */
            instanceStatusInfo[instance].hdmiFlagsReset |= MASK_HDMISTATUS_AVMUTE_INACTIVE_RESET;

            irqSource = BSLHDMIRX_IRQSOURCE_FLAGS;
            errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInterruptStatus(
                          instance,
                          irqSource);
            RETIF(errCode != TM_OK, errCode)
            break;

        case TMDL_HDMIRX_DEEP_COLOR_MODE_24BITS:
        case TMDL_HDMIRX_DEEP_COLOR_MODE_30BITS:
        case TMDL_HDMIRX_DEEP_COLOR_MODE_36BITS:
        case TMDL_HDMIRX_DEEP_COLOR_MODE_48BITS:
            irqSource = BSLHDMIRX_IRQSOURCE_DEEP_COLOR_MODE_CHANGED;
            errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetInterruptStatus(
                          instance,
                          irqSource);
            RETIF(errCode != TM_OK, errCode)
            break;

        default :
            errCode = TMDL_ERR_DLHDMIRX_BAD_PARAMETER;

        }
    }
    return errCode;
}

/******************************************************************************
    \brief Get hdcp seed.

    \param instance Instance identifier.

    \return NA.

******************************************************************************/
static tmErrorCode_t dlHdmiRxFindHdcpSeed
(
    tmInstance_t instance
)
{

    UInt32 otp;
    UInt8  otpByteTst;
    int i;

    tmErrorCode_t           errCode = TM_OK;


    /* Read OTP LSB at address 0x00 and try to match in seed table */
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->
              tmbslHdmiRxSetOTPAddress(instance, 0);
    RETIF(errCode != TM_OK, errCode)
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->
              tmbslHdmiRxConfigureOTP(instance,
                                      BSLHDMIRX_OTP_START_READ,BSLHDMIRX_OTP_BCH_DISABLED);
    RETIF(errCode != TM_OK, errCode)
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->
              tmbslHdmiRxReadOTP(instance, &otp);
    RETIF(errCode != TM_OK, errCode)

    otpByteTst =  (UInt8) (otp & 0xFF);

    if ( errCode == TM_OK)
    {
        for (i = 0; i < RX_SEED_TABLE_LEN; i++)
        {
            if ((kRxSeedTable[i].seedVal == 0) && (kRxSeedTable[i].lookUpVal == 0))
            {
                /* list of RX Seed not more filled */
                break;
            }
            else if (kRxSeedTable[i].lookUpVal == otpByteTst) /* OTP_DATA_LSB */
            {
                /* Found seed replace seed in keyDescriptionSeed */
                gtmdlHdmiRxDriverConfigTable[instance].keyDescriptionSeed = kRxSeedTable[i].seedVal;
                break;
            }
        }
    }

    return errCode;

}

/*============================================================================*/
/**
    \brief Get current audio channel status bytes.

    \param instance    Instance identifier.
    \param pChanStatus channel status bytes

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

******************************************************************************/
tmErrorCode_t tmdlHdmiRxGetChannelStatus
(
    tmInstance_t  instance,
    tmdlHdmiRxChannelStatus_t  *pChanStatus
)
{
    tmErrorCode_t errCode = TM_OK;

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE);

    /* Check the pointer */
    RETIF (pChanStatus == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS);

    /* Take the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode);

    /* Call the BSL */
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetChannelStatus(instance,
              (UInt8 *)pChanStatus);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode);

    /* Release the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode);

    return TM_OK;
}

/*============================================================================*/
/**
    \brief Get current HDMI flags.

    \param instance    Instance identifier.
    \param pHdmiStatus HDMI flags

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

******************************************************************************/
tmErrorCode_t tmdlHdmiRxGetHdmiFlags
(
    tmInstance_t  instance,
    UInt32  *pHdmiFlags
)
{
    tmErrorCode_t errCode = TM_OK;

    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE);

    /* Check the pointer */
    RETIF (pHdmiFlags == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS);

    /* Take the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode);

    /* Call the BSL */
    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetHDMIStatus(instance,
              pHdmiFlags);
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance], errCode != TM_OK, errCode);

    /* Release the semaphore */
    errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);
    RETIF(errCode != TM_OK, errCode);

    return TM_OK;
}



/*============================================================================*/
/*                              specific FUNCTIONS                            */
/*============================================================================*/


#ifdef TMFL_TDA19972_FAMILY

tmbslHdmiRxVideoPortPinGroup_t dl2bslVP (tmdlHdmiRxVideoPortPinGroup_t portPinGroup)
{
    switch (portPinGroup)
    {
    case TMDL_HDMI_RX_VP24_G4_3_0:
        return(BSLHDMIRX_VP24_G4_3_0);
    case TMDL_HDMI_RX_VP24_G4_7_4:
        return(BSLHDMIRX_VP24_G4_7_4);
    case TMDL_HDMI_RX_VP24_G4_11_8:
        return(BSLHDMIRX_VP24_G4_11_8);
    case TMDL_HDMI_RX_VP24_G4_15_12:
        return(BSLHDMIRX_VP24_G4_15_12);
    case TMDL_HDMI_RX_VP24_G4_19_16:
        return(BSLHDMIRX_VP24_G4_19_16);
    case TMDL_HDMI_RX_VP24_G4_23_20:
        return(BSLHDMIRX_VP24_G4_23_20);
    case TMDL_HDMI_RX_VP30_G2_1_0:
        return(BSLHDMIRX_VP30_G2_1_0);
    case TMDL_HDMI_RX_VP30_G4_5_2:
        return(BSLHDMIRX_VP30_G4_5_2);
    case TMDL_HDMI_RX_VP30_G4_9_6:
        return(BSLHDMIRX_VP30_G4_9_6);
    case TMDL_HDMI_RX_VP30_G2_11_10:
        return(BSLHDMIRX_VP30_G2_11_10);
    case TMDL_HDMI_RX_VP30_G4_15_12:
        return(BSLHDMIRX_VP30_G4_15_12);
    case TMDL_HDMI_RX_VP30_G4_19_16:
        return(BSLHDMIRX_VP30_G4_19_16);
    case TMDL_HDMI_RX_VP30_G2_21_20:
        return(BSLHDMIRX_VP30_G2_21_20);
    case TMDL_HDMI_RX_VP30_G4_25_22:
        return(BSLHDMIRX_VP30_G4_25_22);
    case TMDL_HDMI_RX_VP30_G4_29_26:
        return(BSLHDMIRX_VP30_G4_29_26);
    case TMDL_HDMI_RX_VP36_G4_3_0:
        return(BSLHDMIRX_VP36_G4_3_0);
    case TMDL_HDMI_RX_VP36_G4_7_4:
        return(BSLHDMIRX_VP36_G4_7_4);
    case TMDL_HDMI_RX_VP36_G4_11_8:
        return(BSLHDMIRX_VP36_G4_11_8);
    case TMDL_HDMI_RX_VP36_G4_15_12:
        return(BSLHDMIRX_VP36_G4_15_12);
    case TMDL_HDMI_RX_VP36_G4_19_16:
        return(BSLHDMIRX_VP36_G4_19_16);
    case TMDL_HDMI_RX_VP36_G4_23_20:
        return(BSLHDMIRX_VP36_G4_23_20);
    case TMDL_HDMI_RX_VP36_G4_27_24:
        return(BSLHDMIRX_VP36_G4_27_24);
    case TMDL_HDMI_RX_VP36_G4_31_28:
        return(BSLHDMIRX_VP36_G4_31_28);
    case TMDL_HDMI_RX_VP36_G4_35_32:
        return(BSLHDMIRX_VP36_G4_35_32);
    default:
        return(BSLHDMIRX_UNDEFINED);
    }
}

tmbslHdmiRxVideoColorQuartet_t dl2bslColQ (tmdlHdmiRxVideoColorQuartet_t ColorQuartet)
{
    switch (ColorQuartet)
    {
    case TMDL_HDMI_RX_LOZ:
        return(BSLHDMIRX_LOZ);
    case TMDL_HDMI_RX_HIZ:
        return(BSLHDMIRX_HIZ);
    case TMDL_HDMI_RX_R_CR_CBCR_YCBCR_3_0:
        return(BSLHDMIRX_R_CR_CBCR_YCBCR_3_0);
    case TMDL_HDMI_RX_R_CR_CBCR_YCBCR_7_4:
        return(BSLHDMIRX_R_CR_CBCR_YCBCR_7_4);
    case TMDL_HDMI_RX_R_CR_CBCR_YCBCR_11_8:
        return(BSLHDMIRX_R_CR_CBCR_YCBCR_11_8);
    case TMDL_HDMI_RX_B_CB_3_0:
        return(BSLHDMIRX_B_CB_3_0);
    case TMDL_HDMI_RX_B_CB_7_4:
        return(BSLHDMIRX_B_CB_7_4);
    case TMDL_HDMI_RX_B_CB_11_8:
        return(BSLHDMIRX_B_CB_11_8);
    case TMDL_HDMI_RX_G_Y_3_0:
        return(BSLHDMIRX_G_Y_3_0);
    case TMDL_HDMI_RX_G_Y_7_4:
        return(BSLHDMIRX_G_Y_7_4);
    case TMDL_HDMI_RX_G_Y_11_8:
        return(BSLHDMIRX_G_Y_11_8);
    case TMDL_HDMI_RX_R_CR_CBCR_YCBCR_3_0_S:
        return(BSLHDMIRX_R_CR_CBCR_YCBCR_3_0_S);
    case TMDL_HDMI_RX_R_CR_CBCR_YCBCR_7_4_S:
        return(BSLHDMIRX_R_CR_CBCR_YCBCR_7_4_S);
    case TMDL_HDMI_RX_R_CR_CBCR_YCBCR_11_8_S:
        return(BSLHDMIRX_R_CR_CBCR_YCBCR_11_8_S);
    case TMDL_HDMI_RX_B_CB_3_0_S:
        return(BSLHDMIRX_B_CB_3_0_S);
    case TMDL_HDMI_RX_B_CB_7_4_S:
        return(BSLHDMIRX_B_CB_7_4_S);
    case TMDL_HDMI_RX_B_CB_11_8_S:
        return(BSLHDMIRX_B_CB_11_8_S);
    case TMDL_HDMI_RX_G_Y_3_0_S:
        return(BSLHDMIRX_G_Y_3_0_S);
    case TMDL_HDMI_RX_G_Y_7_4_S:
        return(BSLHDMIRX_G_Y_7_4_S);
    case TMDL_HDMI_RX_G_Y_11_8_S:
        return(BSLHDMIRX_G_Y_11_8_S);
    default:
        return(BSLHDMIRX_HIZ);
    }
}

tmErrorCode_t dlHdmiRxConfigVideoPort
(
    tmInstance_t  instance,
    tmdlVPBitsConfig *VideoPortConfiguration,
    UInt8 *num_entries
)
{
    UInt8 i;
    tmErrorCode_t           errCode = TM_OK;
    tmdlHdmiRxVPResolution_t portResolution;

    /* read first VideoPortPinGroup */
    if (VideoPortConfiguration->pinGroup <= TMDL_HDMI_RX_VP24_G4_23_20)
    {
        /* 24 bit configuration */
        *num_entries = 6;
        portResolution = TMDL_HDMIRX_VPRESOLUTION_8_BITS;
    }
    else
    {
        /* 30 or 36 bit configuration */
        *num_entries = 9;
        portResolution = TMDL_HDMIRX_VPRESOLUTION_12_BITS;
    }

    instanceStatusInfo[instance].pMuteAndVideoPort->videoportResolution = portResolution;


    for (i=0; i<*num_entries; i++)
    {
        errCode |= tmbslTDA1997XDefineVideoPort(instance,
                                                dl2bslVP(VideoPortConfiguration[i].pinGroup),
                                                dl2bslColQ(VideoPortConfiguration[i].colorQuartet));
    }

    return (errCode);
}

// todo: *** test why this turns off channel B
tmErrorCode_t tmdlHdmiRxConfigureOutputVideoPort
(
    tmInstance_t  instance,
    tmdlVPBitsConfig *VideoPortConfiguration
)
{
    tmErrorCode_t           errCode = TM_OK;
    UInt8 num_entries, i;


    /* check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* Tag that this API has been used for VideoPort configuration and that video port has been configured*/
    instanceStatusInfo[instance].pMuteAndVideoPort->videoPortConfigured = True;

    /* --------------------- */
    /* Take the semaphore    */
    /* --------------------- */
    tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);

    // UInt8 SaveInstance=instance; added for debug - unclear why Instance becomes 0
    errCode = dlHdmiRxConfigVideoPort(instance, VideoPortConfiguration, &num_entries);

    /* ----------------------------- */
    /* Save video port configuration */
    /* ----------------------------- */

    //instance=SaveInstance;
    for (i=0; i<num_entries; i++)
    {
        if (i<4) // $$$
        {
            instanceStatusInfo[instance].pMuteAndVideoPort->VPConfig[i].pinGroup = VideoPortConfiguration[i].pinGroup;
            instanceStatusInfo[instance].pMuteAndVideoPort->VPConfig[i].colorQuartet = VideoPortConfiguration[i].colorQuartet;
        }
    }

    //instanceStatusInfo[instance].pMuteAndVideoPort->VPConfig[0].pinGroup = VideoPortConfiguration[0].pinGroup;
    //instanceStatusInfo[instance].pMuteAndVideoPort->VPConfig[0].colorQuartet = VideoPortConfiguration[0].colorQuartet;
//
    //instanceStatusInfo[instance].pMuteAndVideoPort->VPConfig[1].pinGroup = VideoPortConfiguration[1].pinGroup;
    //instanceStatusInfo[instance].pMuteAndVideoPort->VPConfig[1].colorQuartet = VideoPortConfiguration[1].colorQuartet;
//
    //instanceStatusInfo[instance].pMuteAndVideoPort->VPConfig[2].pinGroup = VideoPortConfiguration[2].pinGroup;
    //instanceStatusInfo[instance].pMuteAndVideoPort->VPConfig[2].colorQuartet = VideoPortConfiguration[2].colorQuartet;
//
    //instanceStatusInfo[instance].pMuteAndVideoPort->VPConfig[3].pinGroup = VideoPortConfiguration[3].pinGroup;
    //instanceStatusInfo[instance].pMuteAndVideoPort->VPConfig[3].colorQuartet = VideoPortConfiguration[3].colorQuartet;
//
    //instanceStatusInfo[instance].pMuteAndVideoPort->VPConfig[4].pinGroup = VideoPortConfiguration[4].pinGroup;
    //instanceStatusInfo[instance].pMuteAndVideoPort->VPConfig[4].colorQuartet = VideoPortConfiguration[4].colorQuartet;
//
    //instanceStatusInfo[instance].pMuteAndVideoPort->VPConfig[5].pinGroup = VideoPortConfiguration[5].pinGroup;
    //instanceStatusInfo[instance].pMuteAndVideoPort->VPConfig[5].colorQuartet = VideoPortConfiguration[5].colorQuartet;
//

    /* --------------------------- */
    /* Release the sempahore */
    /* --------------------------- */
    tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

    return (errCode);
}

tmErrorCode_t tmdlHdmiRxManualHPD
(
    tmInstance_t  instance,
    tmdlHdmiRxHPDManual_t  ManualHPD
)
{
    tmErrorCode_t           errCode = TM_OK;

    /* --------------------- */
    /* Take the semaphore    */
    /* --------------------- */
    tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);
    
    switch (ManualHPD)
    {
    case TMDL_HDMIRX_HPD_LOW:
        errCode= tmbslTDA1997XManualHPD(instance, BSLHDMIRX_HPD_LOW_BP);
        break;
    case TMDL_HDMIRX_HPD_HIGH:
        errCode= tmbslTDA1997XManualHPD(instance, BSLHDMIRX_HPD_HIGH_BP);
        break;
    case TMDL_HDMIRX_HPD_PULSE:
    default:
        errCode= tmbslTDA1997XManualHPD(instance, BSLHDMIRX_HPD_PULSE);
        break;
    }
    
    /* --------------------------- */
    /* Release the sempahore */
    /* --------------------------- */
    tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

    return (errCode);
}

#endif


#ifdef TMFL_HDMI_OUT

/******************************************************************************
    \brief Get the state of the Edid state machine.

    \param instance Instance identifier.

    \return tmdlHdmiRxEdidDriverState_t Current State of the state machine.

******************************************************************************/
tmdlHdmiRxEdidDriverState_t dlHdmiRxGetHdmioutState
(
    tmInstance_t instance
)
{
    tmdlHdmiRxEdidDriverState_t state;

    /* Get the state */
    state = unitTable[instance].EdidState;

    return (state);
}


/******************************************************************************
    \brief EdidGetDTD .

    \param .

    \return NA.

******************************************************************************/
tmErrorCode_t dlHdmiRxEdidGetDTD
(
    tmInstance_t                    instance,
    tmdlHdmiRxEdidVideoTimings_t    *pDTDescriptors,
    UInt8                           maxDTDesc,
    UInt8                           *pWrittenDTDesc
)
{
    tmErrorCode_t   errCode;

    /* Check the current state */
    RETIF( dlHdmiRxGetHdmioutState(instance) != STATE_EDID_AVAILABLE, TMDL_ERR_DLHDMIRX_INVALID_STATE)

    /* Get detailled descriptors from EDID, return TMDL_ERR_DLHDMITX_NO_RESOURCES if EDID are not read */
    RETIF((errCode = tmbslHdmiRxHdmioutEdidGetDetailedTimingDescriptors(
                         instance, (tmbslHdmiRxEdidDtd_t *)pDTDescriptors, maxDTDesc, pWrittenDTDesc) ) != TM_OK, errCode);

    return TM_OK;
}

/******************************************************************************
    \brief Caculation of aspect ratio.

    \param HImageSize Horizontal image size.
    \param VImageSize Vertical image size.

    \return NA.

******************************************************************************/
static tmdlHdmiRxPictAspectRatio_t dlHdmiRxCalcAspectRatio (
    UInt16    HImageSize,
    UInt16    VImageSize
)
{
    tmdlHdmiRxPictAspectRatio_t    pictureAspectRatio;
    UInt16                         calcPictureAspectRatio;

    /* Define picture Aspect Ratio                                          */
    /* 16/9 = 1.77777 so the result approach is 2                           */
    /* 4/3 = 1.33333 so the result approach is 1                            */
    /*  operation :                                                         */
    /* ImageSize + (vImageSize/2)                                           */
    /* -------------------------- > vImageSize     ->True 16/9 False 4/3    */
    /*           2                                                          */

    calcPictureAspectRatio = ((UInt16)(HImageSize + ((VImageSize)>>1)))>>1;

    if(calcPictureAspectRatio > VImageSize)
    {
        pictureAspectRatio = TMDL_HDMIRX_P_ASPECT_RATIO_16_9;
    }
    else
    {
        pictureAspectRatio = TMDL_HDMIRX_P_ASPECT_RATIO_4_3;
    }

    return pictureAspectRatio;

}



/******************************************************************************
    \brief dlHdmiRxConvertDTDtoCEA_640HAP .

    \param  pDTDescriptors      DTD to convert.
            pictureAspectRatio  aspect ratio of DTD
            formatInterlaced    DTD Interlaced or progressif

    \return NA.

******************************************************************************/
static tmdlHdmiRxVidFmt_t dlHdmiRxConvertDTDtoCEA_640HAP
(
    tmdlHdmiRxEdidVideoTimings_t    *pDTDescriptors
)
{
    tmdlHdmiRxVidFmt_t          codeCEA;

    switch (pDTDescriptors->vActiveLines)
    {
    case 480:
        codeCEA = TMDL_HDMIRX_VFMT_01_640x480p_60Hz;
        break;

    default:
        /* Not a valid format */
        codeCEA = TMDL_HDMIRX_VFMT_NULL;
        break;
    }

    return codeCEA;

}

/******************************************************************************
    \brief dlHdmiRxConvertDTDtoCEA_720HAP .

    \param  pDTDescriptors      DTD to convert.
            pictureAspectRatio  aspect ratio of DTD
            formatInterlaced    DTD Interlaced or progressif

    \return NA.

******************************************************************************/
static tmdlHdmiRxVidFmt_t dlHdmiRxConvertDTDtoCEA_720HAP
(
    tmdlHdmiRxEdidVideoTimings_t    *pDTDescriptors,
    tmdlHdmiRxPictAspectRatio_t     pictureAspectRatio
)
{
    tmdlHdmiRxVidFmt_t          codeCEA;

    switch (pDTDescriptors->vActiveLines)
    {
    case 480:
        if (pictureAspectRatio == TMDL_HDMIRX_P_ASPECT_RATIO_4_3)
        {
            codeCEA = TMDL_HDMIRX_VFMT_02_720x480p_60Hz;
        }
        else
        {
            codeCEA = TMDL_HDMIRX_VFMT_03_720x480p_60Hz;
        }
        break;

    case 576:
        if (pictureAspectRatio == TMDL_HDMIRX_P_ASPECT_RATIO_4_3)
        {
            codeCEA = TMDL_HDMIRX_VFMT_17_720x576p_50Hz;
        }
        else
        {
            codeCEA = TMDL_HDMIRX_VFMT_18_720x576p_50Hz;
        }
        break;

    default:
        /* Not a valid format */
        codeCEA = TMDL_HDMIRX_VFMT_NULL;
        break;
    }

    return codeCEA;

}
/******************************************************************************
    \brief dlHdmiRxConvertDTDtoCEA_1280HAP .

    \param  pDTDescriptors      DTD to convert.
            pictureAspectRatio  aspect ratio of DTD
            formatInterlaced    DTD Interlaced or progressif

    \return NA.

******************************************************************************/
static tmdlHdmiRxVidFmt_t dlHdmiRxConvertDTDtoCEA_1280HAP
(
    tmdlHdmiRxEdidVideoTimings_t    *pDTDescriptors
)
{
    tmdlHdmiRxVidFmt_t          codeCEA;

    switch (pDTDescriptors->vActiveLines)
    {
    case 720:
        switch (pDTDescriptors->hBlankPixels)
        {
        case 370:
            codeCEA = TMDL_HDMIRX_VFMT_04_1280x720p_60Hz;
            break;

        case 700:
            codeCEA = TMDL_HDMIRX_VFMT_19_1280x720p_50Hz;
            break;

        default:
            /* Not a valid format */
            codeCEA = TMDL_HDMIRX_VFMT_NULL;
            break;
        }
        break;

    default:
        /* Not a valid format */
        codeCEA = TMDL_HDMIRX_VFMT_NULL;
        break;
    }


    return codeCEA;
}

/******************************************************************************
    \brief dlHdmiRxConvertDTDtoCEA_1920HAP .

    \param  pDTDescriptors      DTD to convert.
            pictureAspectRatio  aspect ratio of DTD
            formatInterlaced    DTD Interlaced or progressif

    \return NA.

******************************************************************************/
static tmdlHdmiRxVidFmt_t dlHdmiRxConvertDTDtoCEA_1920HAP
(
    tmdlHdmiRxEdidVideoTimings_t    *pDTDescriptors,
    Bool                            formatInterlaced

)
{
    tmdlHdmiRxVidFmt_t          codeCEA;

    switch (pDTDescriptors->hBlankPixels)
    {
    case 280:
        if (formatInterlaced)
        {
            codeCEA = TMDL_HDMIRX_VFMT_05_1920x1080i_60Hz;
        }
        else
        {
            if ( pDTDescriptors->pixelClock == 14850 )
            {
                codeCEA = TMDL_HDMIRX_VFMT_16_1920x1080p_60Hz;
            }
            else
            {
                codeCEA = TMDL_HDMIRX_VFMT_34_1920x1080p_30Hz;
            }
        }
        break;

    case 720:
        if (formatInterlaced)
        {
            codeCEA = TMDL_HDMIRX_VFMT_20_1920x1080i_50Hz;
        }
        else
        {
            switch (pDTDescriptors->pixelClock)
            {
            case 14850:
                codeCEA = TMDL_HDMIRX_VFMT_31_1920x1080p_50Hz;
                break;

            case 7425:
                codeCEA = TMDL_HDMIRX_VFMT_33_1920x1080p_25Hz;
                break;
            default:
                /* Not a valid format */
                codeCEA = TMDL_HDMIRX_VFMT_NULL;
                break;
            }
        }
        break;

    case 830:
        codeCEA = TMDL_HDMIRX_VFMT_32_1920x1080p_24Hz;
        break;

    default:
        /* Not a valid format */
        codeCEA = TMDL_HDMIRX_VFMT_NULL;
        break;
    }


    return codeCEA;
}

/******************************************************************************
    \brief dlHdmiRxConvertDTDtoCEA_1440HAP .

    \param  pDTDescriptors      DTD to convert.
            pictureAspectRatio  aspect ratio of DTD
            formatInterlaced    DTD Interlaced or progressif

    \return NA.

******************************************************************************/
static tmdlHdmiRxVidFmt_t dlHdmiRxConvertDTDtoCEA_1440HAP
(
    tmdlHdmiRxEdidVideoTimings_t    *pDTDescriptors,
    tmdlHdmiRxPictAspectRatio_t     pictureAspectRatio,
    Bool                            formatInterlaced

)
{
    tmdlHdmiRxVidFmt_t          codeCEA;

    switch (pDTDescriptors->vActiveLines)
    {
    case 240:
        if (formatInterlaced)
        {
            if (pictureAspectRatio == TMDL_HDMIRX_P_ASPECT_RATIO_4_3)
            {
                codeCEA = TMDL_HDMIRX_VFMT_06_720x480i_60Hz;
            }
            else
            {
                codeCEA = TMDL_HDMIRX_VFMT_07_720x480i_60Hz;
            }
        }
        else
        {
            if (pictureAspectRatio == TMDL_HDMIRX_P_ASPECT_RATIO_4_3)
            {
                codeCEA = TMDL_HDMIRX_VFMT_08_720x240p_60Hz;
            }
            else
            {
                codeCEA = TMDL_HDMIRX_VFMT_09_720x240p_60Hz;
            }
        }
        break;

    case 288:
        if (formatInterlaced)
        {
            if (pictureAspectRatio == TMDL_HDMIRX_P_ASPECT_RATIO_4_3)
            {
                codeCEA = TMDL_HDMIRX_VFMT_21_720x576i_50Hz;
            }
            else
            {
                codeCEA = TMDL_HDMIRX_VFMT_22_720x576i_50Hz;
            }
        }
        else
        {
            if (pictureAspectRatio == TMDL_HDMIRX_P_ASPECT_RATIO_4_3)
            {
                codeCEA = TMDL_HDMIRX_VFMT_23_720x288p_50Hz;
            }
            else
            {
                codeCEA = TMDL_HDMIRX_VFMT_24_720x288p_50Hz;
            }
        }
        break;

    case 480:
        if (pictureAspectRatio == TMDL_HDMIRX_P_ASPECT_RATIO_4_3)
        {
            codeCEA = TMDL_HDMIRX_VFMT_14_1440x480p_60Hz;
        }
        else
        {
            codeCEA = TMDL_HDMIRX_VFMT_15_1440x480p_60Hz;
        }
        break;

    case 576:
        if (pictureAspectRatio == TMDL_HDMIRX_P_ASPECT_RATIO_4_3)
        {
            codeCEA = TMDL_HDMIRX_VFMT_29_1440x576p_50Hz;
        }
        else
        {
            codeCEA = TMDL_HDMIRX_VFMT_30_1440x576p_50Hz;
        }
        break;

    default:
        /* Not a valid format */
        codeCEA = TMDL_HDMIRX_VFMT_NULL;
        break;
    }

    return codeCEA;
}

/******************************************************************************
    \brief dlHdmiRxConvertDTDtoCEA_2880HAP .

    \param  pDTDescriptors      DTD to convert.
            pictureAspectRatio  aspect ratio of DTD
            formatInterlaced    DTD Interlaced or progressif

    \return NA.

******************************************************************************/
static tmdlHdmiRxVidFmt_t dlHdmiRxConvertDTDtoCEA_2880HAP
(
    tmdlHdmiRxEdidVideoTimings_t    *pDTDescriptors,
    tmdlHdmiRxPictAspectRatio_t     pictureAspectRatio,
    Bool                            formatInterlaced
)
{
    tmdlHdmiRxVidFmt_t          codeCEA;

    switch (pDTDescriptors->vActiveLines)
    {
    case 240:
        if (formatInterlaced)
        {
            if (pictureAspectRatio == TMDL_HDMIRX_P_ASPECT_RATIO_4_3)
            {
                codeCEA = TMDL_HDMIRX_VFMT_10_720x480i_60Hz;
            }
            else
            {
                codeCEA = TMDL_HDMIRX_VFMT_11_720x480i_60Hz;
            }
        }
        else
        {
            if (pictureAspectRatio == TMDL_HDMIRX_P_ASPECT_RATIO_4_3)
            {
                codeCEA = TMDL_HDMIRX_VFMT_12_720x240p_60Hz;
            }
            else
            {
                codeCEA = TMDL_HDMIRX_VFMT_13_720x240p_60Hz;
            }
        }
        break;

    case 288:
        if (formatInterlaced)
        {
            if (pictureAspectRatio == TMDL_HDMIRX_P_ASPECT_RATIO_4_3)
            {
                codeCEA = TMDL_HDMIRX_VFMT_25_720x576i_50Hz;
            }
            else
            {
                codeCEA = TMDL_HDMIRX_VFMT_26_720x576i_50Hz;
            }
        }
        else
        {
            if (pictureAspectRatio == TMDL_HDMIRX_P_ASPECT_RATIO_4_3)
            {
                codeCEA = TMDL_HDMIRX_VFMT_27_720x288p_50Hz;
            }
            else
            {
                codeCEA = TMDL_HDMIRX_VFMT_28_720x288p_50Hz;
            }
        }
        break;

    default:
        /* Not a valid format */
        codeCEA = TMDL_HDMIRX_VFMT_NULL;
        break;
    }

    return codeCEA;
}




/******************************************************************************
    \brief tmdlHdmiRxConvertDTDtoCEA .

    \param DTDescriptors     DTD to convert.

    \return NA.

******************************************************************************/
tmdlHdmiRxVidFmt_t dlHdmiRxConvertDTDtoCEA
(
    tmInstance_t    instance,
    tmdlHdmiRxEdidVideoTimings_t    *pDTDescriptors
)
{

    tmdlHdmiRxVidFmt_t          codeCEA;
    tmdlHdmiRxPictAspectRatio_t pictureAspectRatio;
    Bool                        formatInterlaced;

    /* --------------------- */
    /* Take the semaphore    */
    /* --------------------- */
    tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);

    formatInterlaced = False;

    if ((pDTDescriptors->flags) & 0x80)
    {
        formatInterlaced = True;
    }

    pictureAspectRatio = dlHdmiRxCalcAspectRatio (pDTDescriptors->hImageSize, pDTDescriptors->vImageSize);

    switch (pDTDescriptors->hActivePixels)
    {
    case 640:
        codeCEA = dlHdmiRxConvertDTDtoCEA_640HAP(pDTDescriptors);
        break;

    case 720:
        codeCEA = dlHdmiRxConvertDTDtoCEA_720HAP(pDTDescriptors, pictureAspectRatio);
        break;

    case 1280:
        codeCEA = dlHdmiRxConvertDTDtoCEA_1280HAP(pDTDescriptors);
        break;

    case 1920:
        codeCEA = dlHdmiRxConvertDTDtoCEA_1920HAP(pDTDescriptors, formatInterlaced);
        break;

    case 1440:
        codeCEA = dlHdmiRxConvertDTDtoCEA_1440HAP(pDTDescriptors, pictureAspectRatio, formatInterlaced);
        break;

    case 2880:
        codeCEA = dlHdmiRxConvertDTDtoCEA_2880HAP(pDTDescriptors, pictureAspectRatio, formatInterlaced);
        break;

    default:
        /* Not a valid format */
        codeCEA = TMDL_HDMIRX_VFMT_NULL;
        break;
    }

    /* --------------------------- */
    /* Release the sempahore */
    /* --------------------------- */
    tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

    return codeCEA;

}







/*============================================================================*/
/**
    \brief Select an Output content for TDA19973. Only some combinations are allowed.

    \param instance Instance identifier.
    \param outputs  Output combination to use.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range

******************************************************************************/
tmErrorCode_t tmdlHdmiRxSelectOutputs
(
    tmInstance_t      instance,
    tmdlHdmiRxOutputs_t outputs
)
{
    tmErrorCode_t   errCode = TM_OK;
    tmbslHdmiRxVersion_t            deviceVersion;
    tmdlHdmiRxOutputs_t 	testPatternOutput;

    /* --------------------- */
    /* Take the semaphore    */
    /* --------------------- */
    tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]);

    errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxGetChipInfo(instance,
              &deviceVersion);

    switch (outputs)
    {
    case TMDL_HDMIRX_AVOUT_A_HDMIOUT_B:
        if ((deviceVersion == BSLHDMIRX_TDA19973_CONNECTOR_IN) || (deviceVersion == BSLHDMIRX_TDA19973_SOC_IN))
        {
            errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureInput(instance,
                      BSLHDMIRX_SYNCSIGNAL_DIGITAL,
                      BSLHDMIRX_INPUTSELECTION_MANUAL,
                      BSLHDMIRX_DIGITALINPUT_A,
                      BSLHDMIRX_VIDEO_SOURCE_DIGITAL,
                      BSLHDMIRX_VIDEOINPUT_1,
                      BSLHDMIRX_VDP_RESET_MODE_MANUAL,
                      BSLHDMIRX_VDP_RESET_OFF);
            errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetHdmiOutMode(instance,
                       BSLHDMIRX_HDMI_OUT_LOOP_PON);
            errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxActivateTestPattern(instance,
                       BSLHDMIRX_TESTPATTERN_OFF,
                       BSLHDMIRX_TESTPATTERN_480P);
        }
        else
        {
            errCode = TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS;
        }
        break;

    case TMDL_HDMIRX_AVOUT_NONE_HDMIOUT_B:
        if (deviceVersion == BSLHDMIRX_TDA19973_CONNECTOR_IN)
        {
            errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureInput(instance,
                      BSLHDMIRX_SYNCSIGNAL_DIGITAL,
                      BSLHDMIRX_INPUTSELECTION_MANUAL,
                      BSLHDMIRX_DIGITALINPUT_B,
                      BSLHDMIRX_VIDEO_SOURCE_DIGITAL,
                      BSLHDMIRX_VIDEOINPUT_1,
                      BSLHDMIRX_VDP_RESET_MODE_MANUAL,
                      BSLHDMIRX_VDP_RESET_OFF);
            errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetHdmiOutMode(instance,
                       BSLHDMIRX_HDMI_OUT_BYPASS);
            errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxActivateTestPattern(instance,
                       BSLHDMIRX_TESTPATTERN_OFF,
                       BSLHDMIRX_TESTPATTERN_480P);
        }
        else
        {
            errCode = TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS;
        }
        break;

    case TMDL_HDMIRX_AVOUT_A_HDMIOUT_NONE:
        if ((deviceVersion == BSLHDMIRX_TDA19973_CONNECTOR_IN) || (deviceVersion == BSLHDMIRX_TDA19973_SOC_IN))
        {
            errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureInput(instance,
                      BSLHDMIRX_SYNCSIGNAL_DIGITAL,
                      BSLHDMIRX_INPUTSELECTION_MANUAL,
                      BSLHDMIRX_DIGITALINPUT_A,
                      BSLHDMIRX_VIDEO_SOURCE_DIGITAL,
                      BSLHDMIRX_VIDEOINPUT_1,
                      BSLHDMIRX_VDP_RESET_MODE_MANUAL,
                      BSLHDMIRX_VDP_RESET_OFF);
            errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetHdmiOutMode(instance,
                       BSLHDMIRX_HDMI_OUT_LOOP);
            errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxActivateTestPattern(instance,
                       BSLHDMIRX_TESTPATTERN_OFF,
                       BSLHDMIRX_TESTPATTERN_480P);
        }
        else
        {
            errCode = TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS;
        }
        break;

    case TMDL_HDMIRX_AVOUT_B_HDMIOUT_NONE:
        if ((deviceVersion == BSLHDMIRX_TDA19973_CONNECTOR_IN) || (deviceVersion == BSLHDMIRX_TDA19973_SOC_IN))
        {
            errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureInput(instance,
                      BSLHDMIRX_SYNCSIGNAL_DIGITAL,
                      BSLHDMIRX_INPUTSELECTION_MANUAL,
                      BSLHDMIRX_DIGITALINPUT_B,
                      BSLHDMIRX_VIDEO_SOURCE_DIGITAL,
                      BSLHDMIRX_VIDEOINPUT_1,
                      BSLHDMIRX_VDP_RESET_MODE_MANUAL,
                      BSLHDMIRX_VDP_RESET_OFF);
            errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetHdmiOutMode(instance,
                       BSLHDMIRX_HDMI_OUT_LOOP);
            errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxActivateTestPattern(instance,
                       BSLHDMIRX_TESTPATTERN_OFF,
                       BSLHDMIRX_TESTPATTERN_480P);
        }
        else
        {
            errCode = TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS;
        }
        break;

    case TMDL_HDMIRX_AVOUT_NONE_HDMIOUT_A:
        if ((deviceVersion == BSLHDMIRX_TDA19973_CONNECTOR_IN) || (deviceVersion == BSLHDMIRX_TDA19973_SOC_IN))
        {
            errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureInput(instance,
                      BSLHDMIRX_SYNCSIGNAL_DIGITAL,
                      BSLHDMIRX_INPUTSELECTION_MANUAL,
                      BSLHDMIRX_DIGITALINPUT_A,
                      BSLHDMIRX_VIDEO_SOURCE_DIGITAL,
                      BSLHDMIRX_VIDEOINPUT_1,
                      BSLHDMIRX_VDP_RESET_MODE_MANUAL,
                      BSLHDMIRX_VDP_RESET_OFF);
            errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetHdmiOutMode(instance,
                       BSLHDMIRX_HDMI_OUT_BYPASS);
            errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxActivateTestPattern(instance,
                       BSLHDMIRX_TESTPATTERN_OFF,
                       BSLHDMIRX_TESTPATTERN_480P);
        }
        else
        {
            errCode = TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS;
        }
        break;

    case TMDL_HDMIRX_AVOUT_COLORBAR480i_HDMIOUT_B:
    case TMDL_HDMIRX_AVOUT_COLORBAR480p_HDMIOUT_B:
    case TMDL_HDMIRX_AVOUT_COLORBAR576i_HDMIOUT_B:
    case TMDL_HDMIRX_AVOUT_COLORBAR576p_HDMIOUT_B:
    case TMDL_HDMIRX_AVOUT_RAMP480i_HDMIOUT_B:
    case TMDL_HDMIRX_AVOUT_RAMP480p_HDMIOUT_B:
    case TMDL_HDMIRX_AVOUT_RAMP576i_HDMIOUT_B:
    case TMDL_HDMIRX_AVOUT_RAMP576p_HDMIOUT_B:
        testPatternOutput = outputs;
        switch (testPatternOutput)
        {
        case TMDL_HDMIRX_AVOUT_COLORBAR480i_HDMIOUT_B:
            errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxActivateTestPattern(instance,
                       BSLHDMIRX_TESTPATTERN_ON,
                       BSLHDMIRX_TESTPATTERN_480I);
            break;
        case TMDL_HDMIRX_AVOUT_COLORBAR480p_HDMIOUT_B:
            errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxActivateTestPattern(instance,
                       BSLHDMIRX_TESTPATTERN_ON,
                       BSLHDMIRX_TESTPATTERN_480P);
            break;
        case TMDL_HDMIRX_AVOUT_COLORBAR576i_HDMIOUT_B:
            errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxActivateTestPattern(instance,
                       BSLHDMIRX_TESTPATTERN_ON,
                       BSLHDMIRX_TESTPATTERN_576I);
            break;
        case TMDL_HDMIRX_AVOUT_COLORBAR576p_HDMIOUT_B:
            errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxActivateTestPattern(instance,
                       BSLHDMIRX_TESTPATTERN_ON,
                       BSLHDMIRX_TESTPATTERN_576P);
            break;
        case TMDL_HDMIRX_AVOUT_RAMP480i_HDMIOUT_B:
            errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxActivateTestPattern(instance,
                       BSLHDMIRX_TESTPATTERN_RAMP,
                       BSLHDMIRX_TESTPATTERN_480I);
            break;
        case TMDL_HDMIRX_AVOUT_RAMP480p_HDMIOUT_B:
            errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxActivateTestPattern(instance,
                       BSLHDMIRX_TESTPATTERN_RAMP,
                       BSLHDMIRX_TESTPATTERN_480P);
            break;
        case TMDL_HDMIRX_AVOUT_RAMP576i_HDMIOUT_B:
            errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxActivateTestPattern(instance,
                       BSLHDMIRX_TESTPATTERN_RAMP,
                       BSLHDMIRX_TESTPATTERN_576I);
            break;
        case TMDL_HDMIRX_AVOUT_RAMP576p_HDMIOUT_B:
            errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxActivateTestPattern(instance,
                       BSLHDMIRX_TESTPATTERN_RAMP,
                       BSLHDMIRX_TESTPATTERN_576P);
            break;

        default:
            break;
        }
        if (deviceVersion == BSLHDMIRX_TDA19973_SOC_IN)
        {
            errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureInput(instance,
                      BSLHDMIRX_SYNCSIGNAL_DIGITAL,
                      BSLHDMIRX_INPUTSELECTION_MANUAL,
                      BSLHDMIRX_DIGITALINPUT_A,
                      BSLHDMIRX_VIDEO_SOURCE_DIGITAL,
                      BSLHDMIRX_VIDEOINPUT_1,
                      BSLHDMIRX_VDP_RESET_MODE_MANUAL,
                      BSLHDMIRX_VDP_RESET_OFF);
            errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetHdmiOutMode(instance,
                       BSLHDMIRX_HDMI_OUT_LOOP);
        }
        else if (deviceVersion == BSLHDMIRX_TDA19973_CONNECTOR_IN)
        {
            errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureInput(instance,
                      BSLHDMIRX_SYNCSIGNAL_DIGITAL,
                      BSLHDMIRX_INPUTSELECTION_MANUAL,
                      BSLHDMIRX_DIGITALINPUT_B,
                      BSLHDMIRX_VIDEO_SOURCE_DIGITAL,
                      BSLHDMIRX_VIDEOINPUT_1,
                      BSLHDMIRX_VDP_RESET_MODE_MANUAL,
                      BSLHDMIRX_VDP_RESET_OFF);
            errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetHdmiOutMode(instance,
                       BSLHDMIRX_HDMI_OUT_BYPASS);
        }
        else
        {
            errCode = TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS;
        }
        break;

    case TMDL_HDMIRX_AVOUT_COLORBAR480i_HDMIOUT_A:
    case TMDL_HDMIRX_AVOUT_COLORBAR480p_HDMIOUT_A:
    case TMDL_HDMIRX_AVOUT_COLORBAR576i_HDMIOUT_A:
    case TMDL_HDMIRX_AVOUT_COLORBAR576p_HDMIOUT_A:
    case TMDL_HDMIRX_AVOUT_RAMP480i_HDMIOUT_A:
    case TMDL_HDMIRX_AVOUT_RAMP480p_HDMIOUT_A:
    case TMDL_HDMIRX_AVOUT_RAMP576i_HDMIOUT_A:
    case TMDL_HDMIRX_AVOUT_RAMP576p_HDMIOUT_A:
        testPatternOutput = outputs;
        switch (testPatternOutput)
        {
        case TMDL_HDMIRX_AVOUT_COLORBAR480i_HDMIOUT_A:
            errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxActivateTestPattern(instance,
                       BSLHDMIRX_TESTPATTERN_ON,
                       BSLHDMIRX_TESTPATTERN_480I);
            break;
        case TMDL_HDMIRX_AVOUT_COLORBAR480p_HDMIOUT_A:
            errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxActivateTestPattern(instance,
                       BSLHDMIRX_TESTPATTERN_ON,
                       BSLHDMIRX_TESTPATTERN_480P);
            break;
        case TMDL_HDMIRX_AVOUT_COLORBAR576i_HDMIOUT_A:
            errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxActivateTestPattern(instance,
                       BSLHDMIRX_TESTPATTERN_ON,
                       BSLHDMIRX_TESTPATTERN_576I);
            break;
        case TMDL_HDMIRX_AVOUT_COLORBAR576p_HDMIOUT_A:
            errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxActivateTestPattern(instance,
                       BSLHDMIRX_TESTPATTERN_ON,
                       BSLHDMIRX_TESTPATTERN_576P);
            break;
        case TMDL_HDMIRX_AVOUT_RAMP480i_HDMIOUT_A:
            errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxActivateTestPattern(instance,
                       BSLHDMIRX_TESTPATTERN_RAMP,
                       BSLHDMIRX_TESTPATTERN_480I);
            break;
        case TMDL_HDMIRX_AVOUT_RAMP480p_HDMIOUT_A:
            errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxActivateTestPattern(instance,
                       BSLHDMIRX_TESTPATTERN_RAMP,
                       BSLHDMIRX_TESTPATTERN_480P);
            break;
        case TMDL_HDMIRX_AVOUT_RAMP576i_HDMIOUT_A:
            errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxActivateTestPattern(instance,
                       BSLHDMIRX_TESTPATTERN_RAMP,
                       BSLHDMIRX_TESTPATTERN_576I);
            break;
        case TMDL_HDMIRX_AVOUT_RAMP576p_HDMIOUT_A:
            errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxActivateTestPattern(instance,
                       BSLHDMIRX_TESTPATTERN_RAMP,
                       BSLHDMIRX_TESTPATTERN_576P);
            break;

        default:
            break;

        }
        if ((deviceVersion == BSLHDMIRX_TDA19973_CONNECTOR_IN) || (deviceVersion == BSLHDMIRX_TDA19973_SOC_IN))
        {
            errCode = gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxConfigureInput(instance,
                      BSLHDMIRX_SYNCSIGNAL_DIGITAL,
                      BSLHDMIRX_INPUTSELECTION_MANUAL,
                      BSLHDMIRX_DIGITALINPUT_A,
                      BSLHDMIRX_VIDEO_SOURCE_DIGITAL,
                      BSLHDMIRX_VIDEOINPUT_1,
                      BSLHDMIRX_VDP_RESET_MODE_MANUAL,
                      BSLHDMIRX_VDP_RESET_OFF);
            errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxSetHdmiOutMode(instance,
                       BSLHDMIRX_HDMI_OUT_BYPASS);
            errCode |= gtmdlHdmiRxDriverConfigTable[instance].ptmbslHdmiRxFunctionsList->tmbslHdmiRxActivateTestPattern(instance,
                       BSLHDMIRX_TESTPATTERN_ON,
                       BSLHDMIRX_TESTPATTERN_480I);
        }
        else
        {
            errCode = TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS;
        }
        break;

    default:
        errCode = TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS;
        break;
    }

    /* --------------------------- */
    /* Release the sempahore */
    /* --------------------------- */
    tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

    return (TM_OK);
}


/*****************************************************************************/
/**
    \brief Retrieve Utility status from driver.
           This function is synchronous.
           This function is ISR friendly.

    \param instance         Instance identifier.
    \param pUtilityStatus       Pointer to the variable that will receive the Utility Status.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMITX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

******************************************************************************/
tmErrorCode_t tmdlHdmiRxGetHdmioutUtilityStatus
(
    tmInstance_t            instance,
    tmdlHdmiRxHdmioutUtility_t *   pUtilityStatus
)
{
    return (TM_OK);
}


/*****************************************************************************/
/**
    \brief Retrieve EDID Status from driver.
           This function is synchronous.
           This function is ISR friendly.

    \param instance         Instance identifier.
    \param pEdidStatus      Pointer to the array that will receive the EDID Status.
    \param pEdidBlkCount    Pointer to the integer that will receive the number of
                            read EDID block.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMITX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMITX_BAD_HANDLE: the handle number is wrong
            - TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent
            - TMBSL_ERR_HDMI_BAD_PARAMETER: a parameter was out of range
            - TMBSL_ERR_HDMI_BAD_UNIT_NUMBER: bad transmitter unit number
            - TMBSL_ERR_HDMI_NOT_INITIALIZED: transmitter not initialized

******************************************************************************/
tmErrorCode_t tmdlHdmiRxGetHdmioutEdidStatus
(
    tmInstance_t            instance,
    tmdlHdmiRxEdidStatus_t  *pEdidStatus,
    UInt8                   *pEdidBlkCount
)
{
    tmErrorCode_t   errCode = TM_OK;
    tmdlHdmiRxEdidStatus_t           edidStatus = TMDL_HDMIRX_EDID_STATUS_INVALID;

    /* Check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* Check if EdidStatus and pReadBytesNumber pointers are Null */
    RETIF(pEdidStatus == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pEdidBlkCount == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)

    /* Take the sempahore */
    RETIF( (errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]) ) != TM_OK, errCode)

    /* Get the EDID status from BSL driver */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance],
              (errCode = tmbslHdmiRxHdmioutEdidGetStatus(instance,
                         (UInt8 *)&edidStatus) ) != TM_OK, errCode)

    if (edidStatus >= TMDL_HDMIRX_EDID_STATUS_INVALID)
    {
        *pEdidStatus = TMDL_HDMIRX_EDID_STATUS_INVALID;
    }
    else
    {
        *pEdidStatus = (tmdlHdmiRxEdidStatus_t)edidStatus;
    }

    if ((*pEdidStatus == TMDL_HDMIRX_EDID_READ) ||
            (*pEdidStatus == TMDL_HDMIRX_EDID_ERROR_CHK))
    {
        /* Get the read EDID block number from BSL driver */
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance],
                  (errCode = tmbslHdmiRxHdmioutEdidGetBlockCount(instance,
                             pEdidBlkCount) ) != TM_OK, errCode)
    }

    if (errCode != TM_OK)
    {
        /* Error during read EDID, number of read block is 0 */
        *pEdidBlkCount = 0;

        /* Release the sempahore */
        (void)tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]);

        return errCode;
    }

    /* Release the sempahore */
    RETIF( (errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]) ) != TM_OK, errCode)

    return TM_OK;
}


/*****************************************************************************/
/**
    \brief Retrieves audio descriptors from receiver's EDID. This function
           parses the EDID of Tx device to get the relevant data.
           This function is synchronous.
           This function is not ISR friendly.


    \param instance             Instance identifier.
    \param pAudioDescs          Pointer to the array that will receive audio
                                descriptors.
    \param maxAudioDescs        Size of the array.
    \param pWrittenAudioDescs   Pointer to the integer that will receive the actual
                                number of written descriptors.
    \param pAudioFlags          Pointer to the byte to receive Audio Capabilities Flags.
                                This byte is filled as such:
                                b7    is the Basic audio bit from CEA Extension Version 3.
                                If this bit is set to 1 this means that the sink handles "Basic audio" i.e.
                                two channel L-PCM audio at sample rates of 32kHz, 44.1kHz, and 48kHz.
                                b6    is the Supports_AI bit from the VSDB
                                This bit set to 1 if the sink supports at least one function that uses
                                information carried by the ACP, ISRC1, or ISRC2 packets.
                                b5-b0    0

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMITX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMITX_BAD_HANDLE: the handle number is wrong
            - TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent
            - TMDL_ERR_DLHDMITX_INVALID_STATE: the state is invalid for
              the function
            - TMBSL_ERR_HDMI_BAD_PARAMETER: a parameter was out of range
            - TMBSL_ERR_HDMI_BAD_UNIT_NUMBER: bad transmitter unit number
            - TMBSL_ERR_HDMI_RESOURCE_NOT_AVAILABLE : EDID not read
            - TMBSL_ERR_HDMI_NOT_INITIALIZED: transmitter not initialized

******************************************************************************/
tmErrorCode_t tmdlHdmiRxGetHdmioutEdidAudioCaps
(
    tmInstance_t                instance,
    tmdlHdmiRxEdidAudioDesc_t   *pAudioDescs,
    UInt                        maxAudioDescs,
    UInt                        *pWrittenAudioDescs,
    UInt8                       *pAudioFlags
)
{
    tmErrorCode_t           errCode;
    tmbslHdmiRxEdidSad_t    edidSad[HDMI_RX_SAD_MAX_CNT];
    UInt                    i;

    /* Check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* Check if AudioDescs, WrittenAudioDescs and AudioFlags pointers are Null */
    RETIF(pAudioDescs == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pWrittenAudioDescs == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pAudioFlags == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)

    /* Take the sempahore */
    RETIF( (errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]) ) != TM_OK, errCode)

    /* Check the current state */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance],
              dlHdmiRxGetState(instance) != STATE_EDID_AVAILABLE, TMDL_ERR_DLHDMIRX_INVALID_STATE)

    /* Get video capabilities from EDID, return TMDL_ERR_DLHDMITX_NO_RESOURCES if EDID are not read */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance],
              (errCode = tmbslHdmiRxHdmioutEdidGetAudioCapabilities(instance,
                         edidSad, maxAudioDescs, pWrittenAudioDescs, pAudioFlags) ) != TM_OK, errCode)

    for (i=0; i<*pWrittenAudioDescs; i++)
    {
        pAudioDescs[i].format            = (edidSad[i].ModeChans & 0x78) >> 3; /* Bits[6:3]: EIA/CEA861 mode */
        pAudioDescs[i].channels          = edidSad[i].ModeChans & 0x07;        /* Bits[2:0]: channels */
        pAudioDescs[i].supportedFreqs    = edidSad[i].Freqs;                   /* Supported frequencies */

        if (pAudioDescs[i].format == 1)             /* LPCM format */
        {
            pAudioDescs[i].supportedRes = edidSad[i].Byte3 & 0x07;
            pAudioDescs[i].maxBitrate   = 0x00;
        }
        else if ( (pAudioDescs[i].format >= 2) &&   /* Compressed format */
                  (pAudioDescs[i].format <= 8) )
        {
            pAudioDescs[i].supportedRes = 0x00;
            pAudioDescs[i].maxBitrate   = edidSad[i].Byte3;
        }
        else
        {
            pAudioDescs[i].supportedRes = 0x00;
            pAudioDescs[i].maxBitrate   = 0x00;
        }
    }

    /* Release the sempahore */
    RETIF( (errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]) ) != TM_OK, errCode)

    return TM_OK;
}



/*****************************************************************************/
/**
    \brief Retrieves supported video formats (short descriptors) from
           receiver's EDID. This function parses the EDID of Rx device to get
           the relevant data.
           This function is synchronous.
           This function is not ISR friendly.

    \param instance             Instance identifier.
    \param pVideoDesc           Pointer to the structure that will receive short
                                video descriptors.
    \param maxVideoFormats      Size of the array.
    \param pWrittenVideoFormats Pointer to the integer that will receive the actual
                                number of written descriptors.
    \param pVideoFlags          Pointer to the byte to receive Video Capability Flags.
                                b7: underscan supported
                                b6: YCbCr 4:4:4 supported
                                b5: YCbCr 4:2:2 supported

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMITX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMITX_BAD_HANDLE: the handle number is wrong
            - TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent
            - TMDL_ERR_DLHDMITX_INVALID_STATE: the state is invalid for
              the function
            - TMBSL_ERR_HDMI_BAD_PARAMETER: a parameter was out of range
            - TMBSL_ERR_HDMI_BAD_UNIT_NUMBER: bad transmitter unit number
            - TMBSL_ERR_HDMI_RESOURCE_NOT_AVAILABLE : EDID not read
            - TMBSL_ERR_HDMI_NOT_INITIALIZED: transmitter not initialized

******************************************************************************/
tmErrorCode_t tmdlHdmiRxGetHdmioutEdidVideoCaps
(
    tmInstance_t                instance,
    tmdlHdmiRxShortVidDesc_t    *pVideoDesc,
    UInt                        maxVideoFormats,
    UInt                        *pWrittenVideoFormats,
    UInt8                       *pVideoFlags
)
{
    tmErrorCode_t                   errCode;
    UInt8                           edidVFmtsBuffer[HDMI_RX_SVD_MAX_CNT];
    tmdlHdmiRxEdidVideoTimings_t    edidDTDBuffer[NUMBER_DTD_STORED];
    UInt8                           i;
    UInt8                           writtenDTD = 0;
    UInt8                           dtdCounter = 0;

    /* Check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* Check if Videoformats, WrittenVideoFormats and VideoFlags pointers are Null */
    RETIF(pVideoDesc == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pWrittenVideoFormats == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pVideoFlags == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(maxVideoFormats == 0, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)

    /* Take the sempahore */
    RETIF( (errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]) ) != TM_OK, errCode)

    /* Check the current state */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance],
              dlHdmiRxGetHdmioutState(instance) != STATE_EDID_AVAILABLE, TMDL_ERR_DLHDMIRX_INVALID_STATE)

    /* Get video capabilities from EDID, return TMDL_ERR_DLHDMITX_NO_RESOURCES if EDID are not read */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance],
              (errCode = tmbslHdmiRxHdmioutEdidGetVideoCapabilities(instance,
                         edidVFmtsBuffer, HDMI_RX_SVD_MAX_CNT, pWrittenVideoFormats, pVideoFlags) ) != TM_OK, errCode)

    /* Get detailled descriptors from EDID, return TMDL_ERR_DLHDMITX_NO_RESOURCES if EDID are not read */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance],
              (errCode = dlHdmiRxEdidGetDTD(instance, edidDTDBuffer, NUMBER_DTD_STORED, &writtenDTD) ) != TM_OK, errCode)

    dtdCounter = 0;
    if (writtenDTD > 0)
    {
        /* Release the sempahore */
        RETIF( (errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]) ) != TM_OK, errCode)

        /* Write first DTD in first position of table video desc  */
        pVideoDesc[0].videoFormat = dlHdmiRxConvertDTDtoCEA(instance, &(edidDTDBuffer[dtdCounter]));

        /* Take the sempahore */
        RETIF( (errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]) ) != TM_OK, errCode)

        dtdCounter++;

        pVideoDesc[0].nativeVideoFormat = False;
    }

    /* Start with i = 1 keep the first position for the first DTD */
    for (i = dtdCounter; i< maxVideoFormats ; i++)
    {
        if ((i < (HDMI_RX_SVD_MAX_CNT + dtdCounter)) && (i < ((*pWrittenVideoFormats) + dtdCounter)))
        {
            /* Store SVD */
            pVideoDesc[i].videoFormat = (tmdlHdmiRxVidFmt_t)((Int)edidVFmtsBuffer[i - dtdCounter] & 0x7F);
            /* if bit 7 is true, it means that is a preferred video format */
            if ((edidVFmtsBuffer[i - dtdCounter] & 0x80) == 0x80)
            {
                pVideoDesc[i].nativeVideoFormat = True;
            }
            else
            {
                pVideoDesc[i].nativeVideoFormat = False;
            }
        }
        else
        {
            if ((dtdCounter < NUMBER_DTD_STORED) && (dtdCounter < writtenDTD))
            {
                /* Release the sempahore */
                RETIF( (errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]) ) != TM_OK, errCode)
                /* Store DTD except first DTD */
                pVideoDesc[i].videoFormat = dlHdmiRxConvertDTDtoCEA(instance, &(edidDTDBuffer[dtdCounter]));
                /* Take the sempahore */
                RETIF( (errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]) ) != TM_OK, errCode)

                dtdCounter++;

                pVideoDesc[i].nativeVideoFormat = False;
            }
            else
            {
                /* VGA is always supported */
                pVideoDesc[i].videoFormat = TMDL_HDMIRX_VFMT_01_640x480p_60Hz;
                pVideoDesc[i].nativeVideoFormat = False;
                /* Last format supported exit from loop for */
                break;
            }
        }
    }

    *pWrittenVideoFormats = *pWrittenVideoFormats + dtdCounter + 1;     /* + 1 for VGA format */

    /* Release the sempahore */
    RETIF( (errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]) ) != TM_OK, errCode)

    return TM_OK;
}


/*****************************************************************************/
/**
    \brief Retrieves supported video formats (short descriptors) from
           receiver's EDID. This function parses the EDID of Rx device to get
           the relevant data.
           This function is synchronous.
           This function is not ISR friendly.

    \param instance                 Instance identifier.
    \param pNativeVideoFormat    Pointer to the array that will receive video
                                    timing descriptor.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMITX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMITX_BAD_HANDLE: the handle number is wrong
            - TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent
            - TMDL_ERR_DLHDMITX_INVALID_STATE: the state is invalid for
              the function
            - TMBSL_ERR_HDMI_BAD_PARAMETER: a parameter was out of range
            - TMBSL_ERR_HDMI_BAD_UNIT_NUMBER: bad transmitter unit number
            - TMBSL_ERR_HDMI_RESOURCE_NOT_AVAILABLE : EDID not read
            - TMBSL_ERR_HDMI_NOT_INITIALIZED: transmitter not initialized

******************************************************************************/
tmErrorCode_t tmdlHdmiRxGetHdmioutEdidVideoPreferred
(
    tmInstance_t                    instance,
    tmdlHdmiRxEdidVideoTimings_t    *pNativeVideoFormat
)
{
    tmErrorCode_t   errCode;

    /* Check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* Check if NativeVideoFormat pointer is Null */
    RETIF(pNativeVideoFormat == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)

    /* Take the sempahore */
    RETIF( (errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]) ) != TM_OK, errCode)

    /* Check the current state */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance],
              dlHdmiRxGetState(instance) != STATE_EDID_AVAILABLE, TMDL_ERR_DLHDMIRX_INVALID_STATE)

    /* Get preferred video format from EDID, return TMDL_ERR_DLHDMITX_NO_RESOURCES if EDID are not read */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance],
              (errCode = tmbslHdmiRxHdmioutEdidGetVideoPreferred(instance,
                         (tmbslHdmiRxEdidDtd_t *)pNativeVideoFormat) ) != TM_OK, errCode)

    /* Release the sempahore */
    RETIF( (errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]) ) != TM_OK, errCode)

    return TM_OK;
}


/*****************************************************************************/
/**
    \brief Retrieves the sink type from receiver's EDID (HDMI or DVI). This
           function parses the EDID of Rx device to get the relevant data.
           This function is synchronous.
           This function is not ISR friendly.

    \param instance  Instance identifier.
    \param pSinkType Pointer to the array that will receive sink type.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMITX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMITX_BAD_HANDLE: the handle number is wrong
            - TMDL_ERR_DLHDMITX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMDL_ERR_DLHDMITX_NO_RESOURCES: the resource is not available
            - TMDL_ERR_DLHDMITX_BAD_UNIT_NUMBER: bad transmitter unit number
            - TMDL_ERR_DLHDMITX_NOT_INITIALIZED: the transmitter is not initialized
            - TMDL_ERR_DLHDMITX_INVALID_STATE: the state is invalid for
              the function

******************************************************************************/
tmErrorCode_t tmdlHdmiRxGetHdmioutEdidSinkType
(
    tmInstance_t            instance,
    tmdlHdmiRxSinkType_t    *pSinkType
)
{
    tmErrorCode_t   errCode;

    /* Check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* Check if SinkType pointer is Null */
    RETIF(pSinkType == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)

    /* Take the sempahore */
    RETIF( (errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]) ) != TM_OK, errCode)

    /* Check the current state */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance],
              dlHdmiRxGetState(instance) != STATE_EDID_AVAILABLE, TMDL_ERR_DLHDMIRX_INVALID_STATE)

    /* Read the source address from EDID, return TMDL_ERR_DLHDMIRX_NO_RESOURCES if EDID are not read */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance],
              (errCode = tmbslHdmiRxHdmioutEdidGetSinkType(instance,
                         (tmbslHdmiRxSinkType_t *)pSinkType) ) != TM_OK, errCode)

    /* Release the sempahore */
    RETIF( (errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]) ) != TM_OK, errCode)

    return TM_OK;
}


/*****************************************************************************/
/**
    \brief Retrieves source address from receivers's EDID. This
           function parses the EDID of Rx device to get the relevant data.
           This function is synchronous.
           This function is not ISR friendly.

    \param instance         Instance identifier.
    \param pSourceAddress   Pointer to the integer that will receive the EDID source
                            address.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMITX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMITX_BAD_HANDLE: the handle number is wrong
            - TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent
            - TMDL_ERR_DLHDMITX_INVALID_STATE: the state is invalid for
              the function
            - TMBSL_ERR_HDMI_BAD_PARAMETER: a parameter was out of range
            - TMBSL_ERR_HDMI_BAD_UNIT_NUMBER: bad transmitter unit number
            - TMBSL_ERR_HDMI_RESOURCE_NOT_AVAILABLE : EDID not read
            - TMBSL_ERR_HDMI_NOT_INITIALIZED: transmitter not initialized

******************************************************************************/
tmErrorCode_t tmdlHdmiRxGetHdmioutEdidSourceAddress
(
    tmInstance_t    instance,
    UInt16          *pSourceAddress
)
{
    tmErrorCode_t   errCode;

    /* Check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* Check if SourceAddress pointer is Null */
    RETIF(pSourceAddress == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)

    /* Take the sempahore */
    RETIF( (errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]) ) != TM_OK, errCode)

    /* Check the current state */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance],
              dlHdmiRxGetState(instance) != STATE_EDID_AVAILABLE, TMDL_ERR_DLHDMIRX_INVALID_STATE)

    /* Read the source address from EDID, return TMDL_ERR_DLHDMITX_NO_RESOURCES if EDID are not read */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance],
              (errCode = tmbslHdmiRxHdmioutEdidGetSourceAddress(instance,
                         pSourceAddress) ) != TM_OK, errCode)

    /* Release the sempahore */
    RETIF( (errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]) ) != TM_OK, errCode)

    return TM_OK;
}


/*****************************************************************************/
/**
    \brief Retrieves supported detailled video descriptors from
           receiver's EDID. This function parses the EDID of Rx device to get
           the relevant data.
           This function is synchronous.
           This function is not ISR friendly.

    \param instance         Instance identifier.
    \param pDTDescriptors   Pointer to the array that will receive detailled
                            timing descriptors.
    \param maxDTDesc        Size of the array.
    \param pWrittenDesc     Pointer to the integer that will receive the actual
                            number of written descriptors.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMITX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMITX_BAD_HANDLE: the handle number is wrong
            - TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent
            - TMDL_ERR_DLHDMITX_INVALID_STATE: the state is invalid for
              the function
            - TMBSL_ERR_HDMI_BAD_PARAMETER: a parameter was out of range
            - TMBSL_ERR_HDMI_BAD_UNIT_NUMBER: bad transmitter unit number
            - TMBSL_ERR_HDMI_RESOURCE_NOT_AVAILABLE : EDID not read
            - TMBSL_ERR_HDMI_NOT_INITIALIZED: transmitter not initialized

******************************************************************************/
tmErrorCode_t tmdlHdmiRxGetHdmioutEdidDetailledTimingDescriptors
(
    tmInstance_t                    instance,
    tmdlHdmiRxEdidVideoTimings_t    *pDTDescriptors,
    UInt8                           maxDTDesc,
    UInt8                           *pWrittenDTDesc
)
{
    tmErrorCode_t   errCode;

    /* Check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* Check if DTDescriptors, WrittenDTDesc pointers are Null */
    RETIF(pDTDescriptors == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pWrittenDTDesc == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)

    /* Take the sempahore */
    RETIF( (errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]) ) != TM_OK, errCode)

    /* Get detailled descriptors from EDID, return TMDL_ERR_DLHDMITX_NO_RESOURCES if EDID are not read */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance],
              (errCode = dlHdmiRxEdidGetDTD(instance, pDTDescriptors, maxDTDesc, pWrittenDTDesc) ) != TM_OK, errCode)

    /* Release the sempahore */
    RETIF( (errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]) ) != TM_OK, errCode)

    return TM_OK;
}


/*****************************************************************************/
/**
    \brief Retrieves supported monitor descriptor from receiver's EDID.
           This function parses the EDID of Rx device to get
           the relevant data.
           This function is synchronous.
           This function is not ISR friendly.

    \param instance         Instance identifier.
    \param pEdidFirstMD     Pointer to the array that will receive the first monitor
                            descriptors.
    \param pEdidSecondMD    Pointer to the array that will receive the second monitor
                            descriptors.
    \param pEdidOtherMD     Pointer to the array that will receive the other monitor
                            descriptors.
    \param maxOtherMD       Size of the array.
    \param pWrittenOtherMD  Pointer to the integer that will receive the actual
                            number of written descriptors.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMITX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMITX_BAD_HANDLE: the handle number is wrong
            - TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent
            - TMDL_ERR_DLHDMITX_INVALID_STATE: the state is invalid for
              the function
            - TMBSL_ERR_HDMI_BAD_PARAMETER: a parameter was out of range
            - TMBSL_ERR_HDMI_BAD_UNIT_NUMBER: bad transmitter unit number
            - TMBSL_ERR_HDMI_RESOURCE_NOT_AVAILABLE : EDID not read
            - TMBSL_ERR_HDMI_NOT_INITIALIZED: transmitter not initialized

******************************************************************************/
tmErrorCode_t tmdlHdmiRxGetHdmioutEdidMonitorDescriptors
(
    tmInstance_t                instance,
    tmdlHdmiRxEdidFirstMD_t     *pEdidFirstMD,
    tmdlHdmiRxEdidSecondMD_t    *pEdidSecondMD,
    tmdlHdmiRxEdidOtherMD_t     *pEdidOtherMD,
    UInt8                       maxOtherMD,
    UInt8                       *pWrittenOtherMD
)
{
    tmErrorCode_t   errCode;

    /* Check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* Check if DTDescriptors, WrittenDTDesc pointers are Null */
    RETIF(pEdidFirstMD == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pEdidSecondMD == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)
    RETIF(pEdidOtherMD == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)

    /* Take the sempahore */
    RETIF( (errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]) ) != TM_OK, errCode)

    /* Check the current state */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance],
              dlHdmiRxGetState(instance) != STATE_EDID_AVAILABLE, TMDL_ERR_DLHDMIRX_INVALID_STATE)

    /* Get monitor descriptors from EDID, return TMDL_ERR_DLHDMITX_NO_RESOURCES if EDID are not read */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance],
              (errCode = tmbslHdmiRxHdmioutEdidGetMonitorDescriptors(instance,
                         (tmbslHdmiRxEdidFirstMD_t *)pEdidFirstMD, (tmbslHdmiRxEdidSecondMD_t *)pEdidSecondMD,
                         (tmbslHdmiRxEdidOtherMD_t *)pEdidOtherMD, maxOtherMD, pWrittenOtherMD) ) != TM_OK, errCode)

    /* Release the sempahore */
    RETIF( (errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]) ) != TM_OK, errCode)

    return TM_OK;
}


/*****************************************************************************/
/**
    \brief Retrieves TV picture ratio from receiver's EDID.
           This function parses the EDID of Rx device to get
           the relevant data.
           This function is synchronous.
           This function is not ISR friendly.

    \param instance             Instance identifier.
    \param pEdidTvPictureRatio  Pointer to the variable that will receive TV picture ratio
                                value.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMITX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMITX_BAD_HANDLE: the handle number is wrong
            - TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent
            - TMDL_ERR_DLHDMITX_INVALID_STATE: the state is invalid for
              the function
            - TMBSL_ERR_HDMI_BAD_PARAMETER: a parameter was out of range
            - TMBSL_ERR_HDMI_BAD_UNIT_NUMBER: bad transmitter unit number
            - TMBSL_ERR_HDMI_RESOURCE_NOT_AVAILABLE : EDID not read
            - TMBSL_ERR_HDMI_NOT_INITIALIZED: transmitter not initialized

******************************************************************************/
tmErrorCode_t tmdlHdmiRxGetHdmioutEdidTVPictureRatio
(
    tmInstance_t                    instance,
    tmdlHdmiRxPictAspectRatio_t     *pEdidTvPictureRatio
)
{
    tmErrorCode_t               errCode;
    tmbslHdmiRxEdidBDParam_t    edidBDParam;
    UInt16                      horizontalSize;
    UInt16                      verticalSize;
    tmbslHdmiRxEdidDtd_t        edidDTDBuffer;
    UInt8                       writtenDTD = 0;
    Bool                        bDataAvailable = False; /* Data available in EDID for calcul TV picture ratio */

    /* Check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* Check if DTDescriptors, WrittenDTDesc pointers are Null */
    RETIF(pEdidTvPictureRatio == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)

    /* Take the sempahore */
    RETIF( (errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]) ) != TM_OK, errCode)

    /* Check the current state */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance],
              dlHdmiRxGetState(instance) != STATE_EDID_AVAILABLE, TMDL_ERR_DLHDMIRX_INVALID_STATE)

    /* Get Basic Display Parameter from EDID, return TMDL_ERR_DLHDMITX_NO_RESOURCES if EDID are not read */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance],
              (errCode = tmbslHdmiRxHdmioutEdidGetBasicDisplayParam(instance,
                         &edidBDParam) ) != TM_OK, errCode)

    horizontalSize  = (UInt16)edidBDParam.uMaxHorizontalSize;
    verticalSize    = (UInt16)edidBDParam.uMaxVerticalSize;

    if ((horizontalSize == 0) && (verticalSize == 0))
    {
        /* Get Basic Display Parameter from EDID, return TMDL_ERR_DLHDMITX_NO_RESOURCES if EDID are not read */
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance],
                  (errCode = tmbslHdmiRxHdmioutEdidGetDetailedTimingDescriptors
                             (instance, &edidDTDBuffer, 1, &writtenDTD) ) != TM_OK, errCode);

        if (writtenDTD == 1)
        {
            horizontalSize  = edidDTDBuffer.uHImageSize;
            verticalSize    = edidDTDBuffer.uVImageSize;
            bDataAvailable = True;
        }
        else
        {
            *pEdidTvPictureRatio = TMDL_HDMIRX_P_ASPECT_RATIO_UNDEFINED;
        }
    }
    else
    {
        bDataAvailable = True;
    }

    if (bDataAvailable == True)
    {
        *pEdidTvPictureRatio = dlHdmiRxCalcAspectRatio(horizontalSize, verticalSize);
    }

    /* Release the sempahore */
    RETIF( (errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]) ) != TM_OK, errCode)

    return TM_OK;


}


/*****************************************************************************/
/**
    \brief Retrieves latency information from receiver's EDID.
           This function is synchronous.
           This function is not ISR friendly.


    \param instance             Instance identifier.
    \param pLatency             Pointer to the data structure that receive latency
                                information.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMITX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent
            - TMDL_ERR_DLHDMITX_INVALID_STATE: the state is invalid for
              the function
            - TMBSL_ERR_HDMI_RESOURCE_NOT_AVAILABLE : EDID not read
            - TMBSL_ERR_HDMI_NOT_INITIALIZED: transmitter not initialized

******************************************************************************/
tmErrorCode_t tmdlHdmiRxGetHdmioutEdidLatencyInfo
(
    tmInstance_t                instance,
    tmdlHdmiRxEdidLatency_t    *pLatency
)
{
    tmErrorCode_t   errCode;

    /* Check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* Check pointer is Null */
    RETIF( pLatency == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)

    /* Take the sempahore */
    RETIF( (errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]) ) != TM_OK, errCode)

    /* Check the current state */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance],
              dlHdmiRxGetState(instance) != STATE_EDID_AVAILABLE, TMDL_ERR_DLHDMIRX_INVALID_STATE)

    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance],
              (errCode = tmbslHdmiRxHdmioutEdidGetLatencyInfo(instance, (tmbslHdmiRxEdidLatency_t *) pLatency) ) != TM_OK, errCode)

    /* Release the sempahore */
    RETIF( (errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]) ) != TM_OK, errCode)

    return TM_OK;
}


/******************************************************************************
    \brief Retrieves additional data from receiver's EDID VSDB. This function
           parses the EDID of Rx device to get the relevant data.
           This function is synchronous.
           This function is not ISR friendly.

    \param instance             Instance identifier.
    \param pExtraVsdbData       Pointer to the structure of additional VSDB data

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMITX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent
            - TMDL_ERR_DLHDMITX_INVALID_STATE: the state is invalid for
              the function
            - TMBSL_ERR_HDMI_BAD_PARAMETER: a parameter was out of range
            - TMBSL_ERR_HDMI_BAD_UNIT_NUMBER: bad transmitter unit number
            - TMBSL_ERR_HDMI_RESOURCE_NOT_AVAILABLE : EDID not read
            - TMBSL_ERR_HDMI_NOT_INITIALIZED: transmitter not initialized

******************************************************************************/
tmErrorCode_t tmdlHdmiRxGetHdmioutEdidExtraVsdbData
(
    tmInstance_t                     instance,
    tmdlHdmiRxEdidExtraVsdbData_t    **pExtraVsdbData
)
{
    tmErrorCode_t   errCode;

    /* Check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* Check pointer is Null */
    RETIF(pExtraVsdbData == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)

    /* Take the sempahore */
    RETIF( (errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]) ) != TM_OK, errCode)

    /* Check the current state */
    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance],
              dlHdmiRxGetState(instance) != STATE_EDID_AVAILABLE, TMDL_ERR_DLHDMIRX_INVALID_STATE)

    RETIF_SEM(gtmdlHdmiRxItSemaphore[instance],
              (errCode = tmbslHdmiRxHdmioutEdidGetExtraVsdbData(instance, (tmbslHdmiRxEdidExtraVsdbData_t **)pExtraVsdbData) ) != TM_OK, errCode)

    /* Release the sempahore */
    RETIF( (errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]) ) != TM_OK, errCode)

    return TM_OK;
}



/*****************************************************************************/
/**
    \brief Retrieve HPD status from driver.
           This function is synchronous.
           This function is ISR friendly.

    \param instance         Instance identifier.
    \param pHPDStatus       Pointer to the variable that will receive the HPD Status.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMITX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

******************************************************************************/
tmErrorCode_t tmdlHdmiRxGetHdmioutHPDStatus
(
    tmInstance_t            instance,
    tmdlHdmiRxHdmioutHotPlug_t *   pHPDStatus
)
{
    tmErrorCode_t   errCode;

    /* Check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    RETIF( pHPDStatus== Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)


    /* Take the sempahore */
    RETIF( (errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]) ) != TM_OK, errCode)

    /* Get the HPD status from BSL driver */
    errCode = tmbslHdmiRxHdmioutHotPlugGetStatus(instance,(tmbslHdmiRxHotPlug_t *)pHPDStatus,False);

    if (errCode == TM_OK) {
        /* do nothing */
    }
    else {
        *pHPDStatus = TMDL_HDMIRX_HDMIOUT_HOTPLUG_INACTIVE;
    }

    /* Release the sempahore */
    RETIF( (errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]) ) != TM_OK, errCode)

    return TM_OK;

}



/*****************************************************************************/
/**
    \brief Retrieve RXSense status from driver.
           This function is synchronous.
           This function is ISR friendly.

    \param instance         Instance identifier.
    \param pRXSenseStatus   Pointer to the variable that will receive the RXSense Status.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMITX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

******************************************************************************/
tmErrorCode_t tmdlHdmiRxGetHdmioutRXSenseStatus
(
    tmInstance_t            instance,
    tmdlHdmiRxHdmioutRxSense_t *   pRXSenseStatus
)
{
    tmErrorCode_t   errCode;

    /* Check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    RETIF( pRXSenseStatus== Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)


    /* Take the sempahore */
    RETIF( (errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]) ) != TM_OK, errCode)

    /* Get the RXS sense status from BSL driver */
    errCode = tmbslHdmiRxHdmioutRxSenseGetStatus(instance,( tmbslHdmiRxRxSense_t*)pRXSenseStatus,True);

    if (errCode == TM_OK) {
        /* do nothing */
    }
    else {
        *pRXSenseStatus = TMDL_HDMIRX_HDMIOUT_RX_SENSE_INACTIVE;
    }

    /* Release the sempahore */
    RETIF( (errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]) ) != TM_OK, errCode)

    return TM_OK;

}




/*****************************************************************************/
/**
    \brief Request EDID read at Hdmiout.
           This function is synchronous.
           This function is ISR friendly.

    \param instance         Instance identifier.
    \param pRXSenseStatus   Pointer to the variable that will receive the RXSense Status.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMITX_BAD_INSTANCE: the instance number is wrong or
              out of range
            - TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

******************************************************************************/
tmErrorCode_t tmdlHdmiRxRequestHdmioutEDID
(
    tmInstance_t            instance
)
{
    tmErrorCode_t   errCode;
    tmdlHdmiRxHdmioutHotPlug_t HPDStatus;

    /* Check if instance number is in range */
    RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMIRX_BAD_INSTANCE)

    /* Take the sempahore */
    RETIF( (errCode = tmdlHdmiRxIWSemaphoreP(gtmdlHdmiRxItSemaphore[instance]) ) != TM_OK, errCode)

    /* Get Hot Plug status */
    errCode = tmbslHdmiRxHdmioutHotPlugGetStatus(instance, &HPDStatus, True);

    if ((HPDStatus == TMDL_HDMIRX_HDMIOUT_HOTPLUG_ACTIVE) && ( dlHdmiRxGetHdmioutState(instance) != STATE_EDID_AVAILABLE))
    {
        /* Request EDID read */
        RETIF_SEM(gtmdlHdmiRxItSemaphore[instance],
                  (errCode = tmbslHdmiRxHdmioutEdidRequestBlockData(instance,
                             unitTable[instance].pEdidBuffer, (Int)((unitTable[instance].edidBufferSize) >> 7),
                             (Int)(unitTable[instance].edidBufferSize)) ) != TM_OK, errCode)
    }

    /* Release the sempahore */
    RETIF( (errCode = tmdlHdmiRxIWSemaphoreV(gtmdlHdmiRxItSemaphore[instance]) ) != TM_OK, errCode)

    return TM_OK;

}



#endif


/*============================================================================*/
/*                            END OF FILE                                     */
/*============================================================================*/



