#include "GlobalOptions.h"




/**
 * Copyright (C) 2006 NXP N.V., All Rights Reserved.
 * Exposed under Apache 2.0 Open Source License with permission of NXP N.V.
 *
 * \file          tmdlHdmiRx_local.h
 *
 * \version       $Revision: 1 $
 *
 * \date          $Date: 05/03/07 10:32 $
 *
 * \brief         devlib driver component API for the TDA997x HDMI Receivers
 *
 * \section refs  Reference Documents
 * HDMI Rx Driver - FRS.doc,
 *
 * \section info  Change Information
 *
 * \verbatim

   $History: tmdlHdmiRx_local.h $
 *
 * *****************  Version 1  *****************
 * User: Demoment     Date: 05/03/07   Time: 10:32
 * initial version
 *

   \endverbatim
 *
*/

#ifndef TMDLHDMIRX_LOCAL_H
#define TMDLHDMIRX_LOCAL_H

#include "tmdlHdmiRx_IW.h"
#include "tmNxTypes.h"
#include "tmdlHdmiRx_Types.h"
#include "tmbslHdmiRx_types.h"


#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*/
/*                            MACRO DEFINITIONS                               */
/*============================================================================*/

/* Version of the SW driver */
#define VERSION_COMPATIBILITY 0
#define VERSION_MAJOR         6
#define VERSION_MINOR         2


/* Debug macro for error checking */
//#define RETIF(cond, rslt) if ((cond)){return (rslt);}

/* A macro to check a condition and if true, release the semaphore describe by handle and return a result */
//#define RETIF_SEM(handle, cond, rslt)       if ((cond)){tmdlHdmiRxIWSemaphoreV(handle); return (rslt);}

#define RETIF(cond, rslt) if ((cond)) \
	{								\
	return (rslt);					\
	}

/* A macro to check a condition and if true, release the semaphore describe by handle and return a result */
#define RETIF_SEM(handle, cond, rslt)       if ((cond)){WriteLn("Release local"); tmdlHdmiRxIWSemaphoreV(handle); return (rslt);}

/* Maximum number of data contained in the infoframes */
#define MAX_IF_DATA 40

/* Number of bytes in the infoframes or packet */

/* Auxiliary Video Information infoframe */
#define AVI_IF_NB 17

/* Source Product Description infoframe */
#define SPD_IF_NB 31

/* Source Vendor Specific infoframe */
#define VS_IF_NB 31

/* Audio infoframe */
#define AUD_IF_NB 14

/* MPEG Source infoframe */
#define MPS_IF_NB 14

/* ACP packet */
#define ACP_PKT_NB 5

/* ISRC1 packet */
#define ISRC1_PKT_NB 19

/* ISRC2 packet */
#define ISRC2_PKT_NB 19

/* GBD packet */
#define GBD_PKT_NB 31


/*Address of the slave I2C on the DDC bus*/
/* TBD add this define i n the BSL instead of the DL*/
#define TMDLHDMIRX_DDC_ADDR 0x74

/* Instance number */
#define INSTANCE_0  0

/* Number of event
#define EVENT_NB    32*/

/* Size of a KSV is five bytes */
#define KSV_SIZE  5

/* FIFO latency */
#define FIFO_LATENCY 0x80

/* Channel allocation max value */
#define MAX_CHANNEL_ALLOC   0x1FU

/* AVI infoframe: color indicator */
#define COLOR_INDIC_YUV422  1

/* Delays */
#define DE_DELAY    0x08

/* Masks for HDMI_FLAGS reset */
#define MASK_HDMISTATUS_LAYOUT_0_RESET        0x0200U
#define MASK_HDMISTATUS_LAYOUT_1_RESET        0x0100U
#define MASK_HDMISTATUS_HDMI_RESET            0x0080U
#define MASK_HDMISTATUS_DVI_RESET             0x0040U
#define MASK_HDMISTATUS_EESS_RESET            0x0020U
#define MASK_HDMISTATUS_OESS_RESET            0x0010U
#define MASK_HDMISTATUS_ENCRYPTION_RESET      0x0008U
#define MASK_HDMISTATUS_NO_ENCRYPTION_RESET   0x0004U
#define MASK_HDMISTATUS_AVMUTE_ACTIVE_RESET   0x0002U
#define MASK_HDMISTATUS_AVMUTE_INACTIVE_RESET 0x0001U
#define MASK_HDMISTATUS_RESET_ALL             0xFFFFU

/* Masks for AUDIO_FLAGS */
#define MASK_AUDIO_FLAGS_ALL                  0x0F000000U

/* AVMute counter */
#define AVMUTE_COUNT_DEF_VAL  255
#define AVMUTE_COUNT_INIT_VAL 100

/* SUS last state */
#define SUS_LAST_STATE 	0x1BU

/*============================================================================*/
/*                         ENUM OR TYPE DEFINITIONS                           */
/*============================================================================*/

/* possible states of the state machine */
typedef enum
{
    STATE_NOT_INITIALIZED, /**< Driver is not initialized */
    STATE_INITIALIZED,     /**< Driver is initialized */
    STATE_UNLOCKED,        /**< Driver is not locked */
    STATE_LOCKED,          /**< Driver is locked */
    STATE_CONFIGURED       /**< Driver is configured */
} tmdlHdmiRxDriverState_t;

#ifdef TMFL_HDMI_OUT
/* Possible states of the EDID state machine */
typedef enum
{
    STATE_UNPLUGGED,        /**< Receiver device not connected */
    STATE_PLUGGED,          /**< Receiver device connected, clock lock */
    STATE_EDID_AVAILABLE    /**< Managed to read receiver's EDID */
} tmdlHdmiRxEdidDriverState_t;
#endif




/* possible actions to do for the test pattern */
typedef enum
{
    TEST_PATTERN_TO_BE_ENABLED,  /**< Test pattern must be enabled */
    TEST_PATTERN_TO_BE_DISABLED, /**< Test pattern must be disabled */
    TEST_PATTERN_NOT_USED        /**< Test pattern is not used, so nothing to change */
} tmdlHdmiRxTestPatternAction_t;

/* unit configuration structure */
typedef struct
{
    Bool                          opened;                   /**< is unit instanciated ? */
    Bool                          hdcpEnable;               /**< is HDCP enabled ? */
    Bool                          internalEdid;             /**< is int or ext EDID ? */
    tmdlHdmiRxHdcpMode_t          hdcpMode;                 /**< HDCP mode */
    Bool                          repeaterEnable;           /**< is repeater enabled ? */
    tmdlHdmiRxDeviceVersion_t     deviceVersion;            /**< Version of the HW device */
    tmdlHdmiRxIWTaskHandle_t      taskHandle;               /**< handle of the task associated to this unit */
    tmdlHdmiRxIWQueueHandle_t     queueHandle;              /**< handle of the message queue associated to this unit */
    tmdlHdmiRxDriverState_t       state;                    /**< Current state of the driver */
    tmPowerState_t                powerState;               /**< Current power state of the device */
    ptmdlHdmiRxActivityCallback_t pAnalogActivityCallback;  /**< Analogue activity callback */
    ptmdlHdmiRxActivityCallback_t pDigitalActivityCallback; /**< Digital activity callback */
    ptmdlHdmiRxDataCallback_t     pDataCallback;            /**< Data callback */
    ptmdlHdmiRxInfoCallback_t     pInfoCallback;            /**< Info callback */
#ifdef TMFL_HDMI_OUT
    ptmdlHdmiRxHdmioutCallback_t  pHdmioutCallback;			/**< Hdmiout callback */
    tmdlHdmiRxEdidDriverState_t   EdidState;
    UInt8                         *pEdidBuffer;			/**< Pointer to raw EDID data */
    UInt32                        edidBufferSize; 		/**< Size of buffer for raw EDID data */
#endif
} unitConfig_t;


/* Instance status */

/* Channel assignment structure */
typedef struct _tmdlHdmiRxChannelAssignmentInfo_t
{
    UInt8                               currentChannelAssignment; /* Current channel assignment */
    tmdlHdmiRxChannelAssignmentMode_t   channelAssignmentMode;    /* Channel assignment mode */
} tmdlHdmiRxChannelAssignmentInfo_t, *ptmdlHdmiRxChannelAssignmentInfo_t;

/* Video information structure */
typedef struct _tmdlHdmiRxVideoInfo_t
{
    tmdlHdmiRxInput_t               videoInput;                  /* Current video input */
    tmdlHdmiRxVideoFormat_t         videoFormat;                 /* Current video format */
    tmdlHdmiRxResolutionID_t        detectedVideoResolution;     /* Current detected video resolution */
    tmdlHdmiRxOutClkMode_t          outputClockMode;             /* Current output clock mode */
    Bool                            timingCodeInsert;            /* Current timing codes insertion status */
    Bool                            blankingCodeInsert;          /* Current blanking codes insertion status */
} tmdlHdmiRxVideoInfo_t, *ptmdlHdmiRxVideoInfo_t;

/* Event state structure */
typedef struct _tmdlHdmiRxEventState_t
{
    tmdlHdmiRxEvent_t       event;                              /* Event */
    tmdlHdmiRxEventStatus_t status;                             /* Event status: enabled or disabled */
} tmdlHdmiRxEventState_t, *ptmdlHdmiRxEventState_t;

/* Mute and video port status structure */
typedef struct _ptmdlHdmiRxMuteAndVideoPort_t
{
    Bool                            audioMuteState;             /* Audio mute state: on/off */
#ifdef TMFL_TDA19972_FAMILY
    Bool							videoPortConfigured;		/* Video port has been configured */
#endif
    tmdlHdmiRxAudioFormat_t         audioFormat;                /* Audio format: I2S, SPDIF or OBA */
    Bool                            videoMuteState;             /* Video mute state: on/off */
    tmdlHdmiRxVPResolution_t        videoportResolution;        /* Video port resolution: 8, 10 or 12 bits */
#ifdef TMFL_TDA19972_FAMILY
    tmbslHdmiRxVideoPortSelection_t videoPortASel;              /* Output signal on port A */
    tmbslHdmiRxVideoPortSelection_t videoPortBSel;              /* Output signal on port B */
    tmbslHdmiRxVideoPortSelection_t videoPortCSel;              /* Output signal on port C */
#else
    tmdlHdmiRxOutputSignal_t        videoPortASel;              /* Output signal on port A */
    tmdlHdmiRxOutputSignal_t        videoPortBSel;              /* Output signal on port B */
    tmdlHdmiRxOutputSignal_t        videoPortCSel;              /* Output signal on port C */
#endif
#ifdef TMFL_TDA19972_FAMILY
    tmdlVPBitsConfig				VPConfig[9];
#endif
    tmdlHdmiRxAudioSysClk_t         audioSysClk;                /* Frequencies for SYSCLK signal */
    UInt8                           avMuteCount;                /* Counter for the AVMute, audio and video */
    /* must automatically be unmuted after 5s even if */
    /* the source does not send an AVMute clear */
    /* (Simplay HD compliance). */
} tmdlHdmiRxMuteAndVideoPort_t, *ptmdlHdmiRxMuteAndVideoPort_t;

/* instance status structure */
typedef struct
{
    ptmdlHdmiRxChannelAssignmentInfo_t  pChannelAssignmentInfo;   /* Channel Assignment Info: current channel assignment and mode */
    ptmdlHdmiRxVideoInfo_t              pVideoInfo;               /* Video information: current input and format */
    ptmdlHdmiRxEventState_t             pEventState;              /* Event state: enabled or disabled */
    ptmdlHdmiRxMuteAndVideoPort_t       pMuteAndVideoPort;        /* Mute and video port status */
    UInt32                              hdmiFlagsStatus;          /* Memorize HDMI_FLAGS status coming from Bsl */
    UInt16                              hdmiFlagsReset;           /* Reset HDMI_FLAGS status */
    tmbslHdmiRxHdmiNackHdcp_t           nackHdcp;                 /* Memorize nackHdcp status */
    Bool                                stateC5Reached;           /* Memorize if HDCP repeater has reached state C5 of HDCP state machine */
} instanceStatus_t;

#ifdef __cplusplus
}
#endif

#endif /* TMDLHDMIRX_LOCAL_H */

/*============================================================================*/
/*                               END OF FILE                                  */
/*============================================================================*/


