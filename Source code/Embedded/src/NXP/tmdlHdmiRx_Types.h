#include "GlobalOptions.h"



/**
* Copyright (C) 2007 NXP N.V., All Rights Reserved.
* Exposed under Apache 2.0 Open Source License with permission of NXP N.V.
*
* \file          tmdlHdmiRx_Types.h
*
* \version       $Revision: 1 $
*
* \date          $Date: 12/10/06 10:32 $
*
* \brief         devlib driver component API for the TDA997x HDMI Receivers
*
* \section refs  Reference Documents
* TDA997X Driver - FRS.doc,
* TDA997X Driver - tmdlHdmiRx - SCS.doc
*
* \section info  Change Information
*
* \verbatim

  $History: tmdlHdmiRx_Types.h $
*
* *****************  Version 1  *****************
* User: Demoment     Date: 06/02/07   Time: 8:32
* Updated in $/Source/tmdlHdmiRx/inc
* initial version
*

  \endverbatim
*
*/

#ifndef TMDLHDMIRX_TYPES_H
#define TMDLHDMIRX_TYPES_H

/*============================================================================*/
/*                       INCLUDE FILES                                        */
/*============================================================================*/

#include "tmNxTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*/
/*                       MACRO DEFINITIONS                                    */
/*============================================================================*/


/*============================================================================*/
/*                                DEFINES                                     */
/*============================================================================*/

/**
 * \brief Define listing all the error codes that can be signalled to application
 */
#define TMDL_ERR_DLHDMIRX_BASE                      CID_DL_HDMIRX
#define TMDL_ERR_DLHDMIRX_COMPATIBILITY             (TMDL_ERR_DLHDMIRX_BASE + TM_ERR_COMPATIBILITY)             /**< SW Interface compatibility   */
#define TMDL_ERR_DLHDMIRX_MAJOR_VERSION             (TMDL_ERR_DLHDMIRX_BASE + TM_ERR_MAJOR_VERSION)             /**< SW Major Version error       */
#define TMDL_ERR_DLHDMIRX_COMP_VERSION              (TMDL_ERR_DLHDMIRX_BASE + TM_ERR_COMP_VERSION)              /**< SW component version error   */
#define TMDL_ERR_DLHDMIRX_BAD_UNIT_NUMBER           (TMDL_ERR_DLHDMIRX_BASE + TM_ERR_BAD_UNIT_NUMBER)           /**< Invalid device unit number   */
#define TMDL_ERR_DLHDMIRX_BAD_INSTANCE              (TMDL_ERR_DLHDMIRX_BASE + TM_ERR_BAD_INSTANCE)              /**< Bad input instance value     */
#define TMDL_ERR_DLHDMIRX_BAD_HANDLE                (TMDL_ERR_DLHDMIRX_BASE + TM_ERR_BAD_HANDLE)                /**< Bad input handle             */
#define TMDL_ERR_DLHDMIRX_BAD_PARAMETER             (TMDL_ERR_DLHDMIRX_BASE + TM_ERR_BAD_PARAMETER)             /**< Invalid input parameter      */
#define TMDL_ERR_DLHDMIRX_NO_RESOURCES              (TMDL_ERR_DLHDMIRX_BASE + TM_ERR_NO_RESOURCES)              /**< Resource is not available    */
#define TMDL_ERR_DLHDMIRX_RESOURCE_OWNED            (TMDL_ERR_DLHDMIRX_BASE + TM_ERR_RESOURCE_OWNED)            /**< Resource is already in use   */
#define TMDL_ERR_DLHDMIRX_RESOURCE_NOT_OWNED        (TMDL_ERR_DLHDMIRX_BASE + TM_ERR_RESOURCE_NOT_OWNED)        /**< Caller does not own resource */
#define TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS       (TMDL_ERR_DLHDMIRX_BASE + TM_ERR_INCONSISTENT_PARAMS)       /**< Inconsistent input params    */
#define TMDL_ERR_DLHDMIRX_NOT_INITIALIZED           (TMDL_ERR_DLHDMIRX_BASE + TM_ERR_NOT_INITIALIZED)           /**< Component is not initialized */
#define TMDL_ERR_DLHDMIRX_NOT_SUPPORTED             (TMDL_ERR_DLHDMIRX_BASE + TM_ERR_NOT_SUPPORTED)             /**< Function is not supported    */
#define TMDL_ERR_DLHDMIRX_INIT_FAILED               (TMDL_ERR_DLHDMIRX_BASE + TM_ERR_INIT_FAILED)               /**< Initialization failed        */
#define TMDL_ERR_DLHDMIRX_BUSY                      (TMDL_ERR_DLHDMIRX_BASE + TM_ERR_BUSY)                      /**< Component is busy            */
#define TMDL_ERR_DLHDMIRX_I2C_READ                  (TMDL_ERR_DLHDMIRX_BASE + TM_ERR_READ)                      /**< Read error                   */
#define TMDL_ERR_DLHDMIRX_I2C_WRITE                 (TMDL_ERR_DLHDMIRX_BASE + TM_ERR_WRITE)                     /**< Write error                  */
#define TMDL_ERR_DLHDMIRX_FULL                      (TMDL_ERR_DLHDMIRX_BASE + TM_ERR_FULL)                      /**< Queue is full                */
#define TMDL_ERR_DLHDMIRX_NOT_STARTED               (TMDL_ERR_DLHDMIRX_BASE + TM_ERR_NOT_STARTED)               /**< Function is not started      */
#define TMDL_ERR_DLHDMIRX_ALREADY_STARTED           (TMDL_ERR_DLHDMIRX_BASE + TM_ERR_ALREADY_STARTED)           /**< Function is already started  */
#define TMDL_ERR_DLHDMIRX_ASSERTION                 (TMDL_ERR_DLHDMIRX_BASE + TM_ERR_ASSERTION)                 /**< Assertion failure            */
#define TMDL_ERR_DLHDMIRX_INVALID_STATE             (TMDL_ERR_DLHDMIRX_BASE + TM_ERR_INVALID_STATE)             /**< Invalid state for function   */
#define TMDL_ERR_DLHDMIRX_OPERATION_NOT_PERMITTED   (TMDL_ERR_DLHDMIRX_BASE + TM_ERR_OPERATION_NOT_PERMITTED)   /**< corresponds to posix EPERM   */

/* Component specific error codes */
#define TM_ERR_RESOLUTION_NOT_DETECTED    0x803
#define TMDL_ERR_DLHDMIRX_RESOLUTION_NOT_DETECTED   (TMDL_ERR_DLHDMIRX_BASE + TM_ERR_RESOLUTION_NOT_DETECTED)   /**< The resolution has not been detected */

#define TM_ERR_RESOLUTION_UNKNOWN         0x804
#define TMDL_ERR_DLHDMIRX_RESOLUTION_UNKNOWN        (TMDL_ERR_DLHDMIRX_BASE + TM_ERR_RESOLUTION_UNKNOWN)        /**< The resolution is unknown */

#define TM_ERR_RESOLUTION_NOT_SUPPORTED   0x805
#define TMDL_ERR_DLHDMIRX_RESOLUTION_NOT_SUPPORTED  (TMDL_ERR_DLHDMIRX_BASE + TM_ERR_RESOLUTION_NOT_SUPPORTED)  /**< The resolution is not supported */

#define TM_ERR_RESOLUTION_NOT_CONSISTENT  0x806
#define TMDL_ERR_DLHDMIRX_RESOLUTION_NOT_CONSISTENT  (TMDL_ERR_DLHDMIRX_BASE + TM_ERR_RESOLUTION_NOT_CONSISTENT)  /**< The resolution is not supported */

/*============================================================================*/
/*                       ENUM OR TYPE DEFINITIONS                             */
/*============================================================================*/

/**
 * \brief Enum listing all events that can be signalled to application
 */
typedef enum
{
    TMDL_HDMIRX_ACTIVITY_DETECTED,            /**< New activity has been detected */ // 0
    TMDL_HDMIRX_ACTIVITY_LOST,                /**< Activity has been lost */
    TMDL_HDMIRX_ISRC1_PACKET_RECEIVED,        /**< ISRC1 packet has been received */ // 2
    TMDL_HDMIRX_ISRC2_PACKET_RECEIVED,        /**< ISRC2 packet has been received */
    TMDL_HDMIRX_ACP_PACKET_RECEIVED,          /**< Audio Content Protection packet has been received */ //4
    TMDL_HDMIRX_AVI_INFOFRAME_RECEIVED,       /**< Auxiliary Video Information infoframe has been received */
#ifdef TMFL_TDA19972_FAMILY
    TMDL_HDMIRX_VS_INFOFRAME_RECEIVED,        /**< Vendor specific infoframe has been received */ // 6
#endif
    TMDL_HDMIRX_SPD_INFOFRAME_RECEIVED,       /**< Source product Description infoframe has been received */
    TMDL_HDMIRX_AUD_INFOFRAME_RECEIVED,       /**< Audio infoframe has been received */ //8
    TMDL_HDMIRX_MPS_INFOFRAME_RECEIVED,       /**< MPEG Source infoframe has been received */
    TMDL_HDMIRX_INPUT_LOCKED,                 /**< Indicates that clocks are locked on the new active input */ // a
    TMDL_HDMIRX_ENCRYPTION_DETECTED,          /**< Indicates the active digital input is receiving HDCP encrypted data */
    TMDL_HDMIRX_NO_ENCRYPTION_DETECTED,       /**< Indicates the active digital input is receiving not HDCP encrypted data */ // c
    TMDL_HDMIRX_EESS_HDCP_DETECTED,           /**< Indicates the active digital input is receiving EESS HDCP encrypted data */
    TMDL_HDMIRX_OESS_HDCP_DETECTED,           /**< Indicates the active digital input is receiving OESS HDCP encrypted data */ // e
    TMDL_HDMIRX_REPEATER,                     /**< Indicates that the repater is ready to load the KSVs */
    TMDL_HDMIRX_GBD_PACKET_RECEIVED,          /**< Indicates a GAMUT packet has been received */ // 10
    TMDL_HDMIRX_DEEP_COLOR_MODE_24BITS,       /**< Indicates that the deep color mode is 24 bits per pixel */
    TMDL_HDMIRX_DEEP_COLOR_MODE_30BITS,       /**< Indicates that the deep color mode is 30 bits per pixel */ // 12
    TMDL_HDMIRX_DEEP_COLOR_MODE_36BITS,       /**< Indicates that the deep color mode is 36 bits per pixel */
    TMDL_HDMIRX_DEEP_COLOR_MODE_48BITS,       /**< Indicates that the deep color mode is 48 bits per pixel */  // 14
    TMDL_HDMIRX_AUDIO_SAMPLE_PACKET_DETECTED, /**< Indicates that audio samples packets are detected */
    TMDL_HDMIRX_AUDIO_HBR_PACKET_DETECTED,    /**< Indicates that HBR packets are detected */ // 16
    TMDL_HDMIRX_AUDIO_OBA_PACKET_DETECTED,    /**< Indicates that OBA packets are detected */
    TMDL_HDMIRX_AUDIO_DST_PACKET_DETECTED,    /**< Indicates that DST packets are detected */ // 18
    TMDL_HDMIRX_HDMI_DETECTED,                /**< Indicates that the active digital input i HDMI data*/
    TMDL_HDMIRX_DVI_DETECTED,                 /**< Indicates that the active digital input is receiving DVI data*/ // 1a
    TMDL_HDMIRX_AVI_AVMUTE_ACTIVE,            /**< Indicates that AVMUTE is active*/
    TMDL_HDMIRX_AVI_AVMUTE_INACTIVE,          /**< Indicates that AVMUTE is inactive*/ // 1c
    TMDL_HDMIRX_AUDIO_SAMPLE_FREQ_CHANGED,    /**< Indicates that audio sampling frequency has changed */
    TMDL_HDMIRX_AUDIO_UNMUTED,                /**< Indicates that audio is now unmuted */ // 1e
    TMDL_HDMIRX_AUDIO_AUDIO_LAYOUT_0,         /**< Audio Sample Packet header reports layout 0 */
    TMDL_HDMIRX_AUDIO_AUDIO_LAYOUT_1,         /**< Audio Sample Packet header reports layout 1 */ // 20
    TMDL_HDMIRX_AUDIO_CHANNEL_STATUS,
#ifdef TMFL_HDMI_OUT
    TMDL_HDMIRX_HDMIOUT_HPD_ACTIVE,			 /**< HPD active at Hdmi output side */ //
    TMDL_HDMIRX_HDMIOUT_HPD_INACTIVE,		 /**< HPD inactive at Hdmi output side */
    TMDL_HDMIRX_HDMIOUT_RX_DEVICE_ACTIVE,	 /**< Rx Sense inactive at Hdmi output side */ /
    TMDL_HDMIRX_HDMIOUT_RX_DEVICE_INACTIVE,	 /**< Rx Sense inactive at Hdmi output side */
    TMDL_HDMIRX_HDMIOUT_EDID_RECEIVED,		 /**< Edid received at Hdmi output side */
#endif
    EVENT_NB // 22
} tmdlHdmiRxEvent_t;

/**
 * \brief Enum listing all available event status
 */
typedef enum
{
    TMDL_HDMIRX_EVENT_ENABLED,      /**< Event is enabled */
    TMDL_HDMIRX_EVENT_DISABLED      /**< Event is disabled */
} tmdlHdmiRxEventStatus_t;

/**
 * \brief Enum listing all available inputs
 */
#ifdef TMFL_TDA19972_FAMILY
typedef enum
{
    TMDL_HDMIRX_INPUT_HDMI_A,       /**< Digital input A */
    TMDL_HDMIRX_INPUT_HDMI_B,       /**< Digital input B */
    TMDL_HDMIRX_INPUT_TEST_480P,    /**< 480p Color bar test pattern with audio tone */
    TMDL_HDMIRX_INPUT_TEST_576P,    /**< 576p Color bar test pattern with audio tone */
    TMDL_HDMIRX_INPUT_TEST_480I,    /**< 480p Color bar test pattern with audio tone */
    TMDL_HDMIRX_INPUT_TEST_576I,     /**< 576p Color bar test pattern with audio tone */
    TMDL_HDMIRX_INPUT_RAMPTEST_480P,    /**< 480p Color bar test pattern with audio tone */
    TMDL_HDMIRX_INPUT_RAMPTEST_576P,    /**< 576p Color bar test pattern with audio tone */
    TMDL_HDMIRX_INPUT_RAMPTEST_480I,    /**< 480p Color bar test pattern with audio tone */
    TMDL_HDMIRX_INPUT_RAMPTEST_576I     /**< 576p Color bar test pattern with audio tone */
} tmdlHdmiRxInput_t;
#else
typedef enum
{
    TMDL_HDMIRX_INPUT_AUTO_ANALOG,  /**< Input is not specified and will be automatically selected among analog inputs */
    TMDL_HDMIRX_INPUT_AUTO_DIGITAL, /**< Input is not specified and will be automatically selected among digital inputs */
    TMDL_HDMIRX_INPUT_HDMI_A,       /**< Digital input A */
    TMDL_HDMIRX_INPUT_HDMI_B,       /**< Digital input B */
    TMDL_HDMIRX_INPUT_HDMI_C,       /**< Digital input C */
    TMDL_HDMIRX_INPUT_HDMI_D,       /**< Digital input D */
    TMDL_HDMIRX_INPUT_ANALOG_1,     /**< Analog input 1 */
    TMDL_HDMIRX_INPUT_ANALOG_2,     /**< Analog input 2 */
    TMDL_HDMIRX_INPUT_ANALOG_3,     /**< Analog input 3 */
    TMDL_HDMIRX_INPUT_TEST_480P,    /**< 480p Color bar test pattern with audio tone */
    TMDL_HDMIRX_INPUT_TEST_576P     /**< 576p Color bar test pattern with audio tone */
} tmdlHdmiRxInput_t;
#endif

/**
 * \brief Enum listing all detectable sync types
 */
typedef enum
{
    TMDL_HDMIRX_SYNC_TYPE_SOG,       /**< Sync on green has been detected */
    TMDL_HDMIRX_SYNC_TYPE_COMPOSITE, /**< Digital composite sync has been detected on H/CSYNC */
    TMDL_HDMIRX_SYNC_TYPE_SEPARATED, /**< Digital seprated sync has been detected on VSYNC and H/CSYNC */
    TMDL_HDMIRX_SYNC_TYPE_NONE       /**< No sync detected (used for HDMI inputs) */
} tmdlHdmiRxSyncType_t;

/**
 * \brief Callback function pointer type, used to allow driver to callback
          application when activity status is changing at input.
 * \param Event Identifier of the source event.
 */
typedef void (*ptmdlHdmiRxActivityCallback_t) (tmdlHdmiRxEvent_t event,
        tmdlHdmiRxInput_t input,
        tmdlHdmiRxSyncType_t syncType);

/**
 * \brief Callback function pointer type, used to allow driver to callback
          application when new data (infoframes) has been received.
 * \param Event Identifier of the source event.
 */
typedef void (*ptmdlHdmiRxDataCallback_t) (tmdlHdmiRxEvent_t  event,
        void              *data,
        UInt8              size);

/**
 * \brief Callback function pointer type, used to allow driver to callback
          application when new informative events have been received.
 * \param Event Identifier of the source event.
 */
typedef void (*ptmdlHdmiRxInfoCallback_t) (tmdlHdmiRxEvent_t  event);

/**
 * \brief Enum listing all supported device versions
 */
typedef enum
{
    TMDL_HDMIRX_DEVICE_UNKNOWN = 0x00,     	/**< HW device is unknown */
    TMDL_HDMIRX_DEVICE_TDA19978,           	/**< HW device is a TDA19978 */
    TMDL_HDMIRX_DEVICE_TDA19974,           	/**< HW device is a TDA19974 */
    TMDL_HDMIRX_DEVICE_TDA19977,           	/**< HW device is a TDA19977 */
    TMDL_HDMIRX_DEVICE_TDA19979,           	/**< HW device is a TDA19979 */
    TMDL_HDMIRX_DEVICE_TDA19971,           	/**< HW device is a TDA19971 */
    TMDL_HDMIRX_DEVICE_TDA19972,           	/**< HW device is a TDA19972 */
    TMDL_HDMIRX_DEVICE_TDA19973_SOC_IN,    	/**< HW device is a TDA19973 with B configured as SOC input */
    TMDL_HDMIRX_DEVICE_TDA19973_CONNECTOR_IN,   /**< HW device is a TDA19973 with b configured as connector input*/
    TMDL_HDMIRX_DEVICE_TDA19971N2,           	/**< HW device is a TDA19971N2 */
    TMDL_HDMIRX_DEVICE_TDA19973N2_SOC_IN,    	/**< HW device is a TDA19973N2 with B configured as SOC input */
    TMDL_HDMIRX_DEVICE_TDA19973N2_CONNECTOR_IN   /**< HW device is a TDA19973N2 with b configured as connector input*/
} tmdlHdmiRxDeviceVersion_t;


/**
 * \brief Enum defining the supported HDMI standard version
 */
typedef enum
{
    TMDL_HDMIRX_HDMI_VERSION_UNKNOWN, /**< Unknown   */
    TMDL_HDMIRX_HDMI_VERSION_1_0,     /**< HDMI 1.0  */
    TMDL_HDMIRX_HDMI_VERSION_1_1,     /**< HDMI 1.1  */
    TMDL_HDMIRX_HDMI_VERSION_1_2,     /**< HDMI 1.2  */
    TMDL_HDMIRX_HDMI_VERSION_1_2a,    /**< HDMI 1.2a */
    TMDL_HDMIRX_HDMI_VERSION_1_3,     /**< HDMI 1.3  */
    TMDL_HDMIRX_HDMI_VERSION_1_3a,    /**< HDMI 1.3a  */
    TMDL_HDMIRX_HDMI_VERSION_1_4,     /**< HDMI 1.4  */
    TMDL_HDMIRX_HDMI_VERSION_1_4a     /**< HDMI 1.4a  */
} tmdlHdmiRxHdmiVersion_t;

/**
 * \brief Structure defining the supported audio packets
 */
typedef struct
{
    Bool HBR;              /**< High Bitrate Audio packet */
    Bool DST;              /**< Direct Stream Transport audio packet */
    Bool oneBitAudio;       /**< One Bit Audio sample packet */
} tmdlHdmiRxAudioPacket_t;

/**
 * \brief Structure describing unit capabilities
 */
typedef struct
{
    tmdlHdmiRxDeviceVersion_t deviceVersion;  /**< HW device version */
    Bool                      repeater;       /**< HDCP repeater capability (True/False) */
    Bool                      fastI2C;        /**< Fast I2C (400kHz) capability True/False) */
    tmdlHdmiRxHdmiVersion_t   hdmiVersion;    /**< Supported HDMI standard version  */
    tmdlHdmiRxAudioPacket_t   audioPacket;    /**< Supported audio packets */
    Bool                      fastReauth;     /**< Fast re-auth capability (HDCP) */
    Bool                      analogInput;    /**< Analog Input */
    Bool                      hdmi11Features; /**< Support of HDMI 1.1 features */
    Bool                      internalEdid;   /**< Internal EDID feature */
} tmdlHdmiRxCapabilities_t;

/**
 * \brief Enum listing all HDCP encryption modes
 */
typedef enum
{
    TMDL_HDMIRX_HDCPMODE_AUTO, /**< HDCP encryption mode is automatically detected */
    TMDL_HDMIRX_HDCPMODE_OESS, /**< HDCP encryption mode is forced to OESS */
    TMDL_HDMIRX_HDCPMODE_EESS  /**< HDCP encryption mode is forced to EESS */
} tmdlHdmiRxHdcpMode_t;

/**
 * \brief Structure gathering all instance setup parameters
 */
typedef struct
{
    Bool                 hdcpEnable;         /**< Enable HDCP (True/False) */
    tmdlHdmiRxHdcpMode_t hdcpMode;           /**< HDCP encryption mode */
    Bool                 hdcpRepeaterEnable; /**< Enable HDCP repeater (True/False) */
    Bool                 internalEdid;       /**< Enable internal EDID memory */
#ifdef TMFL_HDMI_OUT
    UInt8   *pEdidBuffer;       /**< Pointer to raw EDID data */
    UInt32  edidBufferSize;     /**< Size of buffer for raw EDID data */
#endif
} tmdlHdmiRxInstanceSetupInfo_t;

/**
 * \brief Structure gathering all instance config parameters
 */
typedef struct
{
    Bool                 hdcpRepeaterEnable; /**< Enable HDCP repeater (True/False) */
} tmdlHdmiRxInstanceCfgInfo_t;

/**
 * \brief Enum listing all possible video resolution
 */
typedef enum
{
    TMDL_HDMIRX_VIDEORES_720_480p_60HZ,   /**< 720x480p 60Hz */
    TMDL_HDMIRX_VIDEORES_1280_720p_60HZ,  /**< 1280x720p 60Hz */
    TMDL_HDMIRX_VIDEORES_1920_1080i_60HZ, /**< 1920x1080i 60Hz */
    TMDL_HDMIRX_VIDEORES_720_480i_60HZ,   /**< 720x480i 60Hz */
    TMDL_HDMIRX_VIDEORES_1920_1080p_60HZ, /**< 1920x1080p 60Hz */


#ifdef SUPPORT_3D_FP
    TMDL_HDMIRX_VIDEORES_720_480p_60HZ_FP,   /**< 720x480p 60Hz Frame Packing */
    TMDL_HDMIRX_VIDEORES_1280_720p_60HZ_FP,  /**< 1280x720p 60Hz Frame Packing */
#endif

    TMDL_HDMIRX_VIDEORES_640_480p_60HZ,   /**< VGA  640*480p    60HZ*/
    TMDL_HDMIRX_VIDEORES_800_600p_60HZ,   /**< SVGA 800*600p    60HZ*/
    TMDL_HDMIRX_VIDEORES_1024_768p_60HZ,  /**< XGA  1024*768p   60HZ*/
    TMDL_HDMIRX_VIDEORES_1280_768p_60HZ,  /**< WXGA 1280*768p   60HZ*/
    TMDL_HDMIRX_VIDEORES_1280_960p_60HZ,  /**< ???? 1280*960p   60HZ*/
    TMDL_HDMIRX_VIDEORES_1280_1024p_60HZ, /**< SXGA 1280*1024p  60HZ*/
    TMDL_HDMIRX_VIDEORES_1440_900p_60HZ,  /**< 1440*900p 60HZ */
    TMDL_HDMIRX_VIDEORES_1600_1200p_60HZ, /**< UGA 1600*1200p   60HZ*/
    TMDL_HDMIRX_VIDEORES_1680_1050p_60HZ_RB, /**< WSXGA 1680*1050p 60HZ (Quantum CVR1660D)*/
    TMDL_HDMIRX_VIDEORES_1920_1200p_60HZ_RB, /**< WUXGA 1920*1200  60HZ*/

    TMDL_HDMIRX_VIDEORES_720_240p_60HZ_M1,/**< 720(1440, 2880)x240p 60Hz mode 1 */
    TMDL_HDMIRX_VIDEORES_720_240p_60HZ_M2,/**< 720(1440, 2880)x240p 60Hz mode 2 */


    TMDL_HDMIRX_VIDEORES_1360_768p_60HZ,  /**< 1360x768p 60Hz (PC resolution) */
    TMDL_HDMIRX_VIDEORES_1400_1050p_60HZ, /**< 1400x1050p 60Hz (PC resolution) */
    TMDL_HDMIRX_VIDEORES_1400_1050p_60HZ_RB, /**< 1400x1050p 60Hz Reduced Blanking (PC resolution) */

    TMDL_HDMIRX_VIDEORES_1080_1920p_60HZ, // added by Sensics
    TMDL_HDMIRX_VIDEORES_UNKNOWN          /**< Should always be the last one */
} tmdlHdmiRxResolutionID_t;

/**
 * \brief Enum listing color conversion matrix modes
 */
typedef enum
{
    TMDL_HDMIRX_CONVERSIONMATRIX_BYPASS,       /**< Color conversion matrix is bypassed */
    TMDL_HDMIRX_CONVERSIONMATRIX_CUSTOM,       /**< Color conversion matrix is defined by application */
    TMDL_HDMIRX_CONVERSIONMATRIX_PREDEFINED_1, /**< 1st predefined color conversion matrix is loaded*/
    TMDL_HDMIRX_CONVERSIONMATRIX_PREDEFINED_2, /**< 2nd predefined color conversion matrix is loaded*/
    TMDL_HDMIRX_CONVERSIONMATRIX_PREDEFINED_3 /**< 3rd predefined color conversion matrix is loaded*/
} tmdlHdmiRxColorMatrixMode;

/**
 * \brief Structure storing specifications of a video resolution
 */
typedef struct
{
    UInt16 width;         /**< Width of the frame in pixels */
    UInt16 height;        /**< Height of the frame in pixels */
    Bool   interlaced;    /**< Interlaced mode (True/False) */
    UInt8  vfrequency;    /**< Vertical frequency in Hz */
} tmdlHdmiRxResolutionSpecs_t;

/**
 * \brief Colorspace conversion matrix coefficients and offsets
 */
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
} tmdlHdmiRxColorMatrixCoefs_t;

/**
 * \brief Blanking codes structure
 */
typedef struct
{
    UInt16 blankingCodeGy;
    UInt16 blankingCodeBu;
    UInt16 blankingCodeRv;
} tmdlHdmiRxBlankingCodes_t;

/**
 * \brief Enum listing all supported video output formats
 */
typedef enum
{
    TMDL_HDMIRX_OUTPUTFORMAT_444,     /**< Output format is full 4:4:4 */
    TMDL_HDMIRX_OUTPUTFORMAT_422_SMP, /**< Output format is 4:2:2 Semi Planar */
    TMDL_HDMIRX_OUTPUTFORMAT_422_CCIR /**< Output format is 4:2:2 CCIR 656 */
} tmdlHdmiRxVideoFormat_t;

/**
 * \brief Enum listing all video port digital resolution supported
 */
typedef enum
{
    TMDL_HDMIRX_VPRESOLUTION_8_BITS,  /**< Video port resolution is 8 bits */
    TMDL_HDMIRX_VPRESOLUTION_10_BITS, /**< Video port resolution is 10 bits */
    TMDL_HDMIRX_VPRESOLUTION_12_BITS  /**< Video port resolution is 12 bits */
} tmdlHdmiRxVPResolution_t;

/**
 * \brief Enum listing all dignal available for output
 */
typedef enum
{
    TMDL_HDMIRX_OUTPUTSIGNAL_R,              /**< R video signal */
    TMDL_HDMIRX_OUTPUTSIGNAL_G,              /**< G video signal */
    TMDL_HDMIRX_OUTPUTSIGNAL_B,              /**< B video signal */
    TMDL_HDMIRX_OUTPUTSIGNAL_Y,              /**< Y video signal */
    TMDL_HDMIRX_OUTPUTSIGNAL_U,              /**< U video signal */
    TMDL_HDMIRX_OUTPUTSIGNAL_V,              /**< V video signal */
    TMDL_HDMIRX_OUTPUTSIGNAL_CBCR,           /**< Cb-Cr video signal */
    TMDL_HDMIRX_OUTPUTSIGNAL_CBYCRY,         /**< Cb-Y-Cr-Y video signal */
    TMDL_HDMIRX_OUTPUTSIGNAL_HIGH_IMPEDANCE  /**< Output signal put in high impedance */
} tmdlHdmiRxOutputSignal_t;

#ifdef TMFL_TDA19972_FAMILY
typedef enum
{
    TMDL_HDMI_RX_VP24_G4_3_0,
    TMDL_HDMI_RX_VP24_G4_7_4,
    TMDL_HDMI_RX_VP24_G4_11_8,
    TMDL_HDMI_RX_VP24_G4_15_12,
    TMDL_HDMI_RX_VP24_G4_19_16,
    TMDL_HDMI_RX_VP24_G4_23_20,
    TMDL_HDMI_RX_VP30_G2_1_0,
    TMDL_HDMI_RX_VP30_G4_5_2,
    TMDL_HDMI_RX_VP30_G4_9_6,
    TMDL_HDMI_RX_VP30_G2_11_10,
    TMDL_HDMI_RX_VP30_G4_15_12,
    TMDL_HDMI_RX_VP30_G4_19_16,
    TMDL_HDMI_RX_VP30_G2_21_20,
    TMDL_HDMI_RX_VP30_G4_25_22,
    TMDL_HDMI_RX_VP30_G4_29_26,
    TMDL_HDMI_RX_VP36_G4_3_0,
    TMDL_HDMI_RX_VP36_G4_7_4,
    TMDL_HDMI_RX_VP36_G4_11_8,
    TMDL_HDMI_RX_VP36_G4_15_12,
    TMDL_HDMI_RX_VP36_G4_19_16,
    TMDL_HDMI_RX_VP36_G4_23_20,
    TMDL_HDMI_RX_VP36_G4_27_24,
    TMDL_HDMI_RX_VP36_G4_31_28,
    TMDL_HDMI_RX_VP36_G4_35_32
} tmdlHdmiRxVideoPortPinGroup_t;

typedef enum
{
    TMDL_HDMI_RX_LOZ,
    TMDL_HDMI_RX_HIZ,
    TMDL_HDMI_RX_R_CR_CBCR_YCBCR_3_0,
    TMDL_HDMI_RX_R_CR_CBCR_YCBCR_7_4,
    TMDL_HDMI_RX_R_CR_CBCR_YCBCR_11_8,
    TMDL_HDMI_RX_B_CB_3_0,
    TMDL_HDMI_RX_B_CB_7_4,
    TMDL_HDMI_RX_B_CB_11_8,
    TMDL_HDMI_RX_G_Y_3_0,
    TMDL_HDMI_RX_G_Y_7_4,
    TMDL_HDMI_RX_G_Y_11_8,
    TMDL_HDMI_RX_R_CR_CBCR_YCBCR_3_0_S,
    TMDL_HDMI_RX_R_CR_CBCR_YCBCR_7_4_S,
    TMDL_HDMI_RX_R_CR_CBCR_YCBCR_11_8_S,
    TMDL_HDMI_RX_B_CB_3_0_S,
    TMDL_HDMI_RX_B_CB_7_4_S,
    TMDL_HDMI_RX_B_CB_11_8_S,
    TMDL_HDMI_RX_G_Y_3_0_S,
    TMDL_HDMI_RX_G_Y_7_4_S,
    TMDL_HDMI_RX_G_Y_11_8_S
} tmdlHdmiRxVideoColorQuartet_t;

typedef struct
{
    tmdlHdmiRxVideoPortPinGroup_t pinGroup;
    tmdlHdmiRxVideoColorQuartet_t colorQuartet;
} tmdlVPBitsConfig;
#endif



/**
 * \brief Enum listing all audio output formats available
 */
typedef enum
{
    TMDL_HDMIRX_AUDIOFORMAT_I2S16,           /**< Audio output format is I2S 16 bits */
    TMDL_HDMIRX_AUDIOFORMAT_I2S32,           /**< Audio output format is I2S 32 bits */
    TMDL_HDMIRX_AUDIOFORMAT_SPDIF,           /**< Audio output format is SPDIF */
    TMDL_HDMIRX_AUDIOFORMAT_OBA,             /**< Audio output format is One Bit Audio */
    TMDL_HDMIRX_AUDIOFORMAT_I2S16_HBR_STRAIGHT, /**< Audio output format HBR straight in I2S 16-bit mode */
    TMDL_HDMIRX_AUDIOFORMAT_I2S16_HBR_DEMUX, /**< Audio output format HBR demux in I2S 16-bit mode */
    TMDL_HDMIRX_AUDIOFORMAT_I2S32_HBR_DEMUX, /**< Audio output format HBR demux in I2S 32-bit mode */
    TMDL_HDMIRX_AUDIOFORMAT_SPDIF_HBR_DEMUX, /**< Audio output format HBR demux in SPDIF mode */
    TMDL_HDMIRX_AUDIOFORMAT_DST              /**< Audio output format is Direct Stream Transfer */
} tmdlHdmiRxAudioFormat_t;

/**
 * \brief Enum listing possible frequencies for SYSCLK signal
 */
typedef enum
{
    TMDL_HDMIRX_AUDIOSYSCLK_128FS, /**< SYSCLK is 128.fs */
    TMDL_HDMIRX_AUDIOSYSCLK_256FS, /**< SYSCLK is 256.fs */
    TMDL_HDMIRX_AUDIOSYSCLK_512FS  /**< SYSCLK is 512.fs */
} tmdlHdmiRxAudioSysClk_t;

/**
 * \brief Enum listing possible colorspaces at input
 */
typedef enum
{
    TMDL_HDMIRX_COLORSPACE_RGB,    /**< Color space is RGB */
    TMDL_HDMIRX_COLORSPACE_YUV701, /**< Color space is YUV701 */
    TMDL_HDMIRX_COLORSPACE_YUV709  /**< Color space is YUV709 */
} tmdlHdmiRxColorSpace_t;

/**
 * \brief Enum listing the possible channel assignment mode
 */
typedef enum
{
    TMDL_HDMIRX_REFER_TO_AUD_INFOFRAME,     /**< Refer to the audio infoframe */
    TMDL_HDMIRX_CHANNEL_ASSIGNMENT_FORCED   /**< Channel assignment forced by the user */
} tmdlHdmiRxChannelAssignmentMode_t;

/**
 * \brief Structure used to parse an AVI infoframe
 */
typedef struct
{
    UInt8 packetType;                    /**< Infoframe packet type (0x82 for AVI) */
    UInt8 version;                       /**< Infoframe packet version */
    UInt8 length;                        /**< Infoframe packet length */
    UInt8 checksum;                      /**< Packet checksum */
    UInt8 colorIndicator;                /**< RGB or YCbCr indicator. See CEA-861-B table 8 for details */
    UInt8 activeInfoPresent;             /**< Active information present. Indicates if activeFormatAspectRatio field is valid */
    UInt8 barInformationDataValid;       /**< Bar information data valid */
    UInt8 scanInformation;               /**< Scan information. See CEA-861-B table 8 for details */
    UInt8 colorimetry;                   /**< Colorimetry. See CEA-861-B table 9 for details */
    UInt8 pictureAspectRatio;            /**< Picture aspect ratio. See CEA-861-B table 9 for details */
    UInt8 activeFormatAspectRatio;       /**< Active Format aspect ratio. See CEA-861-B table 10 and Annex H for details */
    UInt8 nonUniformPictureScaling;      /**< Non-uniform picture scaling. See CEA-861-B table 11 for details */
    UInt8 videoFormatIdentificationCode; /**< Video format indentification code. See CEA-861-B section 6.3 for details */
    UInt8 pixelRepetitionFactor;         /**< Pixel repetition factor. See CEA-861-B table 11 for details */
    UInt8 lineNumberEndTopBarLow;
    UInt8 lineNumberEndTopBarHigh;
    UInt8 lineNumberStartBottomBarLow;
    UInt8 lineNumberStartBottomBarHigh;
    UInt8 lineNumberEndLeftBarLow;
    UInt8 lineNumberEndLeftBarHigh;
    UInt8 lineNumberStartRightBarLow;
    UInt8 lineNumberStartRightBarHigh;
} tmdlHdmiRxAVIInfoframe_t;

/**
 * \brief Structure used to parse an SPD infoframe
 */
typedef struct
{
    UInt8       packetType;               /**< Infoframe packet type (0x82 for AVI) */
    UInt8       version;                  /**< Infoframe packet version */
    UInt8       length;                   /**< Infoframe packet length */
    UInt8       checksum;                 /**< Packet checksum */
    UInt8       vendorName[8];            /**< Vendor name */
    UInt8       productDescription[16];   /**< Product description*/
    UInt8       sourceDeviceDescription;  /**< Source device description*/
} tmdlHdmiRxSPDInfoframe_t;


#ifdef SUPPORT_3D_FP
/**
 * \brief Structure defining the content of a VS infoframe packet according to HDMI 1.4a standard
 */

/* HDMI version */
#define TMDL_HDMIRX_VERSION            0x01

/* Minimum length for a valid 3D VS infoframe */
#define TMDL_HDMIRX_MINIMUM_VALID_VS_LEN  5

/* HDMI video format [3bits] */
#define TMDL_HDMIRX_VIDEO_FORMAT_SHIFT 5
#define TMDL_HDMIRX_FORMAT_EXTENDED    0x01
#define TMDL_HDMIRX_3D                 0x02

/* IEEE registration identifier (0x000C03) with least significant byte first */
#define TMDL_HDMIRX_HDMI_IEEE_BYTE0    0x03
#define TMDL_HDMIRX_HDMI_IEEE_BYTE1    0x0C
#define TMDL_HDMIRX_HDMI_IEEE_BYTE2    0x00

/* 3D structure [4bits] */
#define TMDL_HDMIRX_3D_STRUCTURE_SHIFT 4
#define TMDL_HDMIRX_FRAME_PACKING      0x00
#define TMDL_HDMIRX_FIELD_ALTERNATIVE  0x01
#define TMDL_HDMIRX_LINE_ALTERNATIVE   0x02
#define TMDL_HDMIRX_SIDE_BY_SIDE_FULL  0x03
#define TMDL_HDMIRX_L_DEPTH            0x04
#define TMDL_HDMIRX_L_DEPTH_GFX        0x05
#define TMDL_HDMIRX_TOP_AND_BOTTOM     0x06
#define TMDL_HDMIRX_SIDE_BY_SIDE_HALF  0x08

/* 3D EXT Data [4bits] */
#define TMDL_HDMIRX_3D_EXT_DATA_SHIFT  4
#define TMDL_HDMIRX_HORIZONTAL_SUB     0x00   /* Horizontal sub-sampling */
#define TMDL_HDMIRX_QUINCUNX_OLOR      0x04   /* Odd/Left picture, Odd/Right picture */
#define TMDL_HDMIRX_QUINCUNX_OLER      0x05   /* Odd/Left picture, Even/Right picture */
#define TMDL_HDMIRX_QUINCUNX_ELOR      0x06   /* Even/Left picture, Odd/Right picture */
#define TMDL_HDMIRX_QUINCUNX_ELER      0x07   /* Even/Left picture, Even/Right picture */

/* 3D Meta field */
#define TMDL_HDMIRX_3D_META_PRESENT_SHIFT 3
#define TMDL_HDMIRX_3D_META_TYPE_SHIFT    5
#define TMDL_HDMIRX_3D_META_PRESENT       0x01
#define TMDL_HDMIRX_3D_META_PARALLAX      0x00

#define TMDL_HDMIRX_VS_PKT_DATA_LEN    28
/**
 * \brief Structure used to parse an VS infoframe
 */
typedef struct
{
    /* HB0, HB1, HB2 */
    UInt8       packetType;               /**< Infoframe packet type (0x81 for VS infoframe) */
    UInt8       version;                  /**< Infoframe packet version */
    UInt8       length;                   /**< Infoframe packet length */

    /*
    Packet Byte #        7     6     5     4     3     2     1     0
      PB0          (checksum                                       )
      PB1          24bit IEEE Registration Identifier (0x000C03)
      PB2                 ( least significant byte first )
      PB3
      PB4          (HDMI_Video_Format  ) (0)   (0)   (0)   (0)   (0)
      PB5          (3D_Structure             ) +Meta (0)   (0)   (0)
      PB6          (3D_Ext_Data              ) (0)   (0)   (0)   (0)
      PB7          (3D_Metadata_type   )  (3D_Metadata_Length (= N))
      PB8          (3D_Metadata_1                                  )
       ...                                              ...
      PB [7+N]     (3D_Metadata_N                                  )
      PB[8+N]~[Nv] (Reserved (0)                                   )
    */
    UInt8       checksum;                /* PB0 */
    UInt8       ieee_id[3];              /* PB1, PB2, PB3 */
    UInt8       hdmi_video_fmt;          /* PB4 */
    UInt8       h3d_structure;           /* PB5 */
    UInt8       h3d_ext_data;            /* PB6 */
    UInt8       vsData[TMDL_HDMIRX_VS_PKT_DATA_LEN-4];
} tmdlHdmiRxVSInfoframe_t;
#endif

/**
 * \brief Structure used to parse a MPS infoframe
 */
typedef struct
{
    UInt8       packetType;                     /**< Infoframe packet type (0x82 for AVI) */
    UInt8       version;                        /**< Infoframe packet version */
    UInt8       length;                         /**< Infoframe packet length */
    UInt8       checksum;                       /**< Packet checksum */
    UInt8       MPEG_bitRate[4];                /**< MPEG bit rate (LSB first) */
    UInt8       fieldRepeat;                    /**< Field Repeat*/
    UInt8       MPEG_Frame;                     /**< MPEG Frame*/
} tmdlHdmiRxMPSInfoframe_t;

/**
 * \brief Structure used to parse an audio infoframe
 */
typedef struct
{
    UInt8    packetType;        /**< Infoframe packet type (0x84 for audio) */
    UInt8    version;           /**< Infoframe packet version */
    UInt8    length;            /**< Infoframe packet length */
    UInt8    checksum;          /**< Packet checksum */
    UInt8    codingType;        /**< Coding type (always set to zero) */
    UInt8    channelCount;      /**< Channel count. See CEA-861-B table 17 for details */
    UInt8    samplefrequency;   /**< Sample frequency. See CEA-861-B table 18 for details */
    UInt8    sampleSize;        /**< Sample frequency. See CEA-861-B table 18 for details */
    UInt8    dataByte3;         /**< Depends on coding type */
    UInt8    channelAllocation; /**< Channel allocation. See CEA-861-B section 6.3.2 for details */
    UInt8    downmixInhibit;    /**< Downmix inhibit. See CEA-861-B section 6.3.2 for details */
    UInt8    levelShiftValue;   /**< level shift value for downmixing. See CEA-861-B section 6.3.2 and table 23 for details */
} tmdlHdmiRxAudioInfoframe_t;

/**
 * \brief Structure used to parse an ISRC1 packet
 */
typedef struct
{
    UInt8     packetType;       /**< Packet type (0x05 for ISRC1) */
    UInt8     ISRCCont;         /**< ISRC packet continued in next packet */
    UInt8     ISRCValid;        /**< Set to one when ISRCStatus and UPC_EAN_ISRC_xx are valid */
    UInt8     ISRCStatus;       /**< ISRC status */
    UInt8     UPC_EAN_ISRC[16]; /**< ISRC packet data */
} tmdlHdmiRxISRC1Packet_t;

/**
 * \brief Structure used to parse an ISRC2 packet
 */
typedef struct
{
    UInt8     packetType;       /**< Packet type (0x06 for ISRC2) */
    UInt8     UPC_EAN_ISRC[16]; /**< ISRC packet data */
} tmdlHdmiRxISRC2Packet_t;

/**
 * \brief Structure used to parse an ACP packet
 */
typedef struct
{
    UInt8    packetType;   /**< Packet packet type (0x04 for ACP) */
    UInt8    ACPType;      /**< ACP packet type */
    UInt8    ACPByte[28];  /**< ACP data depending on ACP packet type */
} tmdlHdmiRxACPPacket_t;


/**
 * \brief Structure used to parse a GAMUT packet
 */
typedef struct
{
    UInt8 packetType;          /**< Packet type (0x0A for Gamut) */
    UInt8 nextField;           /**< GBD carried in this packet will be effective on the next video field */
    UInt8 GBDProfile;          /**< Transmission profile number */
    UInt8 affectedGamutSeqNum; /**< Indicates which video fields are relevant for this metadata */
    UInt8 noCrntGBD;           /**< no gamut metadata available for the currently transmitted video */
    UInt8 packetSeq;           /**< Indicates whether this packet is the only, the first, an intermediate or the last packet */
    UInt8 currentGamutSeqNum;  /**< gamut number of the currently transmitted video stream */
    UInt8 GBDData[28];         /**< GDB data depend on the profile number. Refer to HDMI standard */
} tmdlHdmiRxGBDPacket_t;



/**
 * \brief Structure used to parse a GCP (General Control Packet) packet
 */
typedef struct
{
    UInt8 packetType;         /**< Packet type (0x03 for GCP) */
    UInt8 clearAVMute;        /**< To reduce the negative impact on the Sink of TMDS clock changes or interruptions */
    UInt8 setAVMute;          /**< To reduce the negative impact on the Sink of TMDS clock changes or interruptions */
    UInt8 pixelPackingPhase;  /**< Pixel Packing Phase PP3, PP2, PP1, PP0 */
    UInt8 colorDepth;         /**< Color Depth : CD3, CD2, CD1, CD0 */
    UInt8 defaultPhase;       /**< Used with the pixel packing phase */
} tmdlHdmiRxGCPPacket_t;



/**
 * \brief Enum listing possible video output clock mode
 */
typedef enum
{
    TMDL_HDMIRX_OUTCLKMODE_SINGLE_EDGE, /**< Output clock mode is single edge */
    TMDL_HDMIRX_OUTCLKMODE_DOUBLE_EDGE  /**< Output clock mode is double edge */
} tmdlHdmiRxOutClkMode_t;

/**
* \brief DST audio sample rate
*/
typedef enum
{
    TMDL_HDMIRX_AUDIO_DSTRATE_SINGLE = 0x00,
    TMDL_HDMIRX_AUDIO_DSTRATE_DOUBLE = 0x80
} tmdlHdmiRxAudioDstRate_t;

/**
* \brief Audio sample frequencies
*/
typedef enum
{
    TMDL_HDMIRX_AUDIO_SAMPLEFREQ_ERROR   = 0x00,
    TMDL_HDMIRX_AUDIO_SAMPLEFREQ_32_KHZ  = 0x01,
    TMDL_HDMIRX_AUDIO_SAMPLEFREQ_44_KHZ  = 0x02,
    TMDL_HDMIRX_AUDIO_SAMPLEFREQ_48_KHZ  = 0x03,
    TMDL_HDMIRX_AUDIO_SAMPLEFREQ_88_KHZ  = 0x04,
    TMDL_HDMIRX_AUDIO_SAMPLEFREQ_96_KHZ  = 0x05,
    TMDL_HDMIRX_AUDIO_SAMPLEFREQ_176_KHZ = 0x06,
    TMDL_HDMIRX_AUDIO_SAMPLEFREQ_192_KHZ = 0x07
} tmdlHdmiRxAudioSampleFreq_t;


/**
 * \brief Structure used for channel status
 */
typedef struct
{
    UInt8    csByte0;        /**< channel status byte 0 */
    UInt8    csByte1;        /**< channel status byte 1 */
    UInt8    csByte3;        /**< channel status byte 3 */
    UInt8    csByte4;        /**< channel status byte 4 */
    UInt8    csByte2AP0L;    /**< channel status byte 2 audio port0 left */
    UInt8    csByte2AP0R;    /**< channel status byte 2 audio port0 right */
    UInt8    csByte2AP1L;    /**< channel status byte 2 audio port1 left */
    UInt8    csByte2AP1R;    /**< channel status byte 2 audio port1 right */
    UInt8    csByte2AP2L;    /**< channel status byte 2 audio port2 left */
    UInt8    csByte2AP2R;    /**< channel status byte 2 audio port2 right */
    UInt8    csByte2AP3L;    /**< channel status byte 2 audio port3 left */
    UInt8    csByte2AP3R;    /**< channel status byte 2 audio port3 right */
} tmdlHdmiRxChannelStatus_t;

#ifdef TMFL_TDA19972_FAMILY
typedef enum {
    TMDL_HDMIRX_HPD_PULSE,
    TMDL_HDMIRX_HPD_LOW,
    TMDL_HDMIRX_HPD_HIGH
} tmdlHdmiRxHPDManual_t;
#endif

/*============================================================================*/
/*                               Specific DEFINES                             */
/*============================================================================*/

#ifdef TMFL_HDMI_OUT
/**
 * \brief Enum listing all available outputs combination
 */
typedef enum
{
    TMDL_HDMIRX_AVOUT_A_HDMIOUT_B,      		/**< Audio/Video output from input_A and HDMI output is analog bypass of input_B )*/
    TMDL_HDMIRX_AVOUT_NONE_HDMIOUT_B,      		/**< Audio/Video output muted and HDMI output is analog bypass of input_B */
    TMDL_HDMIRX_AVOUT_NONE_HDMIOUT_A,      		/**< Audio/Video output muted and HDMI output is analog bypass of input_A */
    TMDL_HDMIRX_AVOUT_A_HDMIOUT_NONE,      		/**< Audio/Video output from input_A  and HDMI output is disabled */
    TMDL_HDMIRX_AVOUT_B_HDMIOUT_NONE,      		/**< Audio/Video output from input_B  and HDMI output is disabled */
    TMDL_HDMIRX_AVOUT_COLORBAR480i_HDMIOUT_B,	/**< Audio/Video output is 480i colorbar and HDMI output is analog bypass of input_B */
    TMDL_HDMIRX_AVOUT_COLORBAR480i_HDMIOUT_A,	/**< Audio/Video output is 480i colorbar and HDMI output is analog bypass of input_A */
    TMDL_HDMIRX_AVOUT_COLORBAR576i_HDMIOUT_B,	/**< Audio/Video output is 576i colorbar and HDMI output is analog bypass of input_B */
    TMDL_HDMIRX_AVOUT_COLORBAR576i_HDMIOUT_A,	/**< Audio/Video output is 576i colorbar and HDMI output is analog bypass of input_A */
    TMDL_HDMIRX_AVOUT_COLORBAR480p_HDMIOUT_B,	/**< Audio/Video output is 480p colorbar and HDMI output is analog bypass of input_B */
    TMDL_HDMIRX_AVOUT_COLORBAR480p_HDMIOUT_A,	/**< Audio/Video output is 480p colorbar and HDMI output is analog bypass of input_A */
    TMDL_HDMIRX_AVOUT_COLORBAR576p_HDMIOUT_B,	/**< Audio/Video output is 576p colorbar and HDMI output is analog bypass of input_B */
    TMDL_HDMIRX_AVOUT_COLORBAR576p_HDMIOUT_A,	/**< Audio/Video output is 576p colorbar and HDMI output is analog bypass of input_A */
    TMDL_HDMIRX_AVOUT_RAMP480i_HDMIOUT_B,	/**< Audio/Video output is 480i colorbar and HDMI output is analog bypass of input_B */
    TMDL_HDMIRX_AVOUT_RAMP480i_HDMIOUT_A,	/**< Audio/Video output is 480i colorbar and HDMI output is analog bypass of input_A */
    TMDL_HDMIRX_AVOUT_RAMP576i_HDMIOUT_B,	/**< Audio/Video output is 576i colorbar and HDMI output is analog bypass of input_B */
    TMDL_HDMIRX_AVOUT_RAMP576i_HDMIOUT_A,	/**< Audio/Video output is 576i colorbar and HDMI output is analog bypass of input_A */
    TMDL_HDMIRX_AVOUT_RAMP480p_HDMIOUT_B,	/**< Audio/Video output is 480p colorbar and HDMI output is analog bypass of input_B */
    TMDL_HDMIRX_AVOUT_RAMP480p_HDMIOUT_A,	/**< Audio/Video output is 480p colorbar and HDMI output is analog bypass of input_A */
    TMDL_HDMIRX_AVOUT_RAMP576p_HDMIOUT_B,	/**< Audio/Video output is 576p colorbar and HDMI output is analog bypass of input_B */
    TMDL_HDMIRX_AVOUT_RAMP576p_HDMIOUT_A	/**< Audio/Video output is 576p colorbar and HDMI output is analog bypass of input_A */
} tmdlHdmiRxOutputs_t;


/**
 * \brief Enum defining possible Utility status
 */
typedef enum
{
    TMDL_HDMIRX_UTILITY_INACTIVE    = 0,    /**< Utility inactive */
    TMDL_HDMIRX_UTILITY_ACTIVE      = 1,    /**< Utility active   */
    TMDL_HDMIRX_UTILITY_INVALID     = 2     /**< Invalid Utility  */
} tmdlHdmiRxHdmioutUtility_t;


/**
 * \brief Enum defining the EDID Status
 */
typedef enum
{
    TMDL_HDMIRX_EDID_READ                = 0,   /**< All blocks read OK */
    TMDL_HDMIRX_EDID_READ_INCOMPLETE     = 1,   /**< All blocks read OK but buffer too small to return all of them */
    TMDL_HDMIRX_EDID_ERROR_CHK_BLOCK_0   = 2,   /**< Block 0 checksum error */
    TMDL_HDMIRX_EDID_ERROR_CHK           = 3,   /**< Block 0 OK, checksum error in one or more other blocks */
    TMDL_HDMIRX_EDID_NOT_READ            = 4,   /**< EDID not read */
    TMDL_HDMIRX_EDID_STATUS_INVALID      = 5    /**< Invalid   */
} tmdlHdmiRxEdidStatus_t;

/**
 * \brief Structure defining the Edid audio descriptor
 */
typedef struct
{
    UInt8 format;         /* EIA/CEA861 mode */
    UInt8 channels;       /* number of channels */
    UInt8 supportedFreqs; /* bitmask of supported frequencies */
    UInt8 supportedRes;   /* bitmask of supported resolutions (LPCM only) */
    UInt8 maxBitrate;     /* Maximum bitrate divided by 8KHz (compressed formats only) */
} tmdlHdmiRxEdidAudioDesc_t;


/**
 * \brief Enum listing all IA/CEA 861-D video formats
 */
typedef enum
{
    TMDL_HDMIRX_VFMT_NULL               = 0,    /**< Not a valid format...        */
    TMDL_HDMIRX_VFMT_NO_CHANGE          = 0,    /**< ...or no change required     */
    TMDL_HDMIRX_VFMT_MIN                = 1,    /**< Lowest valid format          */
    TMDL_HDMIRX_VFMT_TV_MIN             = 1,    /**< Lowest valid TV format       */
    TMDL_HDMIRX_VFMT_01_640x480p_60Hz   = 1,    /**< Format 01 640  x 480p  60Hz  */
    TMDL_HDMIRX_VFMT_02_720x480p_60Hz   = 2,    /**< Format 02 720  x 480p  60Hz  */
    TMDL_HDMIRX_VFMT_03_720x480p_60Hz   = 3,    /**< Format 03 720  x 480p  60Hz  */
    TMDL_HDMIRX_VFMT_04_1280x720p_60Hz  = 4,    /**< Format 04 1280 x 720p  60Hz  */
    TMDL_HDMIRX_VFMT_05_1920x1080i_60Hz = 5,    /**< Format 05 1920 x 1080i 60Hz  */
    TMDL_HDMIRX_VFMT_06_720x480i_60Hz   = 6,    /**< Format 06 720  x 480i  60Hz  */
    TMDL_HDMIRX_VFMT_07_720x480i_60Hz   = 7,    /**< Format 07 720  x 480i  60Hz  */
    TMDL_HDMIRX_VFMT_08_720x240p_60Hz   = 8,    /**< Format 08 720  x 240p  60Hz  */
    TMDL_HDMIRX_VFMT_09_720x240p_60Hz   = 9,    /**< Format 09 720  x 240p  60Hz  */
    TMDL_HDMIRX_VFMT_10_720x480i_60Hz   = 10,   /**< Format 10 720  x 480i  60Hz  */
    TMDL_HDMIRX_VFMT_11_720x480i_60Hz   = 11,   /**< Format 11 720  x 480i  60Hz  */
    TMDL_HDMIRX_VFMT_12_720x240p_60Hz   = 12,   /**< Format 12 720  x 240p  60Hz  */
    TMDL_HDMIRX_VFMT_13_720x240p_60Hz   = 13,   /**< Format 13 720  x 240p  60Hz  */
    TMDL_HDMIRX_VFMT_14_1440x480p_60Hz  = 14,   /**< Format 14 1440 x 480p  60Hz  */
    TMDL_HDMIRX_VFMT_15_1440x480p_60Hz  = 15,   /**< Format 15 1440 x 480p  60Hz  */
    TMDL_HDMIRX_VFMT_16_1920x1080p_60Hz = 16,   /**< Format 16 1920 x 1080p 60Hz  */
    TMDL_HDMIRX_VFMT_17_720x576p_50Hz   = 17,   /**< Format 17 720  x 576p  50Hz  */
    TMDL_HDMIRX_VFMT_18_720x576p_50Hz   = 18,   /**< Format 18 720  x 576p  50Hz  */
    TMDL_HDMIRX_VFMT_19_1280x720p_50Hz  = 19,   /**< Format 19 1280 x 720p  50Hz  */
    TMDL_HDMIRX_VFMT_20_1920x1080i_50Hz = 20,   /**< Format 20 1920 x 1080i 50Hz  */
    TMDL_HDMIRX_VFMT_21_720x576i_50Hz   = 21,   /**< Format 21 720  x 576i  50Hz  */
    TMDL_HDMIRX_VFMT_22_720x576i_50Hz   = 22,   /**< Format 22 720  x 576i  50Hz  */
    TMDL_HDMIRX_VFMT_23_720x288p_50Hz   = 23,   /**< Format 23 720  x 288p  50Hz  */
    TMDL_HDMIRX_VFMT_24_720x288p_50Hz   = 24,   /**< Format 24 720  x 288p  50Hz  */
    TMDL_HDMIRX_VFMT_25_720x576i_50Hz   = 25,   /**< Format 25 720  x 576i  50Hz  */
    TMDL_HDMIRX_VFMT_26_720x576i_50Hz   = 26,   /**< Format 26 720  x 576i  50Hz  */
    TMDL_HDMIRX_VFMT_27_720x288p_50Hz   = 27,   /**< Format 27 720  x 288p  50Hz  */
    TMDL_HDMIRX_VFMT_28_720x288p_50Hz   = 28,   /**< Format 28 720  x 288p  50Hz  */
    TMDL_HDMIRX_VFMT_29_1440x576p_50Hz  = 29,   /**< Format 29 1440 x 576p  50Hz  */
    TMDL_HDMIRX_VFMT_30_1440x576p_50Hz  = 30,   /**< Format 30 1440 x 576p  50Hz  */
    TMDL_HDMIRX_VFMT_31_1920x1080p_50Hz = 31,   /**< Format 31 1920 x 1080p 50Hz  */
    TMDL_HDMIRX_VFMT_32_1920x1080p_24Hz = 32,   /**< Format 32 1920 x 1080p 24Hz  */
    TMDL_HDMIRX_VFMT_33_1920x1080p_25Hz = 33,   /**< Format 33 1920 x 1080p 25Hz  */
    TMDL_HDMIRX_VFMT_34_1920x1080p_30Hz = 34,   /**< Format 34 1920 x 1080p 30Hz  */
    TMDL_HDMIRX_VFMT_35_2880x480p_60Hz  = 35,   /**< Format 35 2880 x 480p  60Hz 4:3  */
    TMDL_HDMIRX_VFMT_36_2880x480p_60Hz  = 36,   /**< Format 36 2880 x 480p  60Hz 16:9 */
    TMDL_HDMIRX_VFMT_37_2880x576p_50Hz  = 37,   /**< Format 37 2880 x 576p  50Hz 4:3  */
    TMDL_HDMIRX_VFMT_38_2880x576p_50Hz  = 38,   /**< Format 38 2880 x 576p  50Hz 16:9 */

    TMDL_HDMIRX_VFMT_INDEX_60_1280x720p_24Hz = 39,/**< Index of HDMITX_VFMT_60_1280x720p_24Hz */
    TMDL_HDMIRX_VFMT_60_1280x720p_24Hz  = 60,   /**< Format 60 1280 x 720p  23.97/24Hz 16:9 */
    TMDL_HDMIRX_VFMT_61_1280x720p_25Hz  = 61,   /**< Format 61 1280 x 720p  25Hz 16:9 */
    TMDL_HDMIRX_VFMT_62_1280x720p_30Hz  = 62,   /**< Format 60 1280 x 720p  29.97/30Hz 16:9 */

    TMDL_HDMIRX_VFMT_TV_MAX             = 62,   /**< Highest valid TV format      */
    TMDL_HDMIRX_VFMT_TV_NO_REG_MIN      = 32,   /**< Lowest TV format without prefetched table */
    TMDL_HDMIRX_VFMT_TV_NUM             = 42,   /**< Number of TV formats & null  */

    TMDL_HDMIRX_VFMT_PC_MIN             = 128,  /**< Lowest valid PC format       */
    TMDL_HDMIRX_VFMT_PC_640x480p_60Hz   = 128,  /**< PC format 128                */
    TMDL_HDMIRX_VFMT_PC_800x600p_60Hz   = 129,  /**< PC format 129                */
    TMDL_HDMIRX_VFMT_PC_1152x960p_60Hz  = 130,  /**< PC format 130                */
    TMDL_HDMIRX_VFMT_PC_1024x768p_60Hz  = 131,  /**< PC format 131                */
    TMDL_HDMIRX_VFMT_PC_1280x768p_60Hz  = 132,  /**< PC format 132                */
    TMDL_HDMIRX_VFMT_PC_1280x1024p_60Hz = 133,  /**< PC format 133                */
    TMDL_HDMIRX_VFMT_PC_1360x768p_60Hz  = 134,  /**< PC format 134                */
    TMDL_HDMIRX_VFMT_PC_1400x1050p_60Hz = 135,  /**< PC format 135                */
    TMDL_HDMIRX_VFMT_PC_1600x1200p_60Hz = 136,  /**< PC format 136                */
    TMDL_HDMIRX_VFMT_PC_1024x768p_70Hz  = 137,  /**< PC format 137                */
    TMDL_HDMIRX_VFMT_PC_640x480p_72Hz   = 138,  /**< PC format 138                */
    TMDL_HDMIRX_VFMT_PC_800x600p_72Hz   = 139,  /**< PC format 139                */
    TMDL_HDMIRX_VFMT_PC_640x480p_75Hz   = 140,  /**< PC format 140                */
    TMDL_HDMIRX_VFMT_PC_1024x768p_75Hz  = 141,  /**< PC format 141                */
    TMDL_HDMIRX_VFMT_PC_800x600p_75Hz   = 142,  /**< PC format 142                */
    TMDL_HDMIRX_VFMT_PC_1024x864p_75Hz  = 143,  /**< PC format 143                */
    TMDL_HDMIRX_VFMT_PC_1280x1024p_75Hz = 144,  /**< PC format 144                */
    TMDL_HDMIRX_VFMT_PC_640x350p_85Hz   = 145,  /**< PC format 145                */
    TMDL_HDMIRX_VFMT_PC_640x400p_85Hz   = 146,  /**< PC format 146                */
    TMDL_HDMIRX_VFMT_PC_720x400p_85Hz   = 147,  /**< PC format 147                */
    TMDL_HDMIRX_VFMT_PC_640x480p_85Hz   = 148,  /**< PC format 148                */
    TMDL_HDMIRX_VFMT_PC_800x600p_85Hz   = 149,  /**< PC format 149                */
    TMDL_HDMIRX_VFMT_PC_1024x768p_85Hz  = 150,  /**< PC format 150                */
    TMDL_HDMIRX_VFMT_PC_1152x864p_85Hz  = 151,  /**< PC format 151                */
    TMDL_HDMIRX_VFMT_PC_1280x960p_85Hz  = 152,  /**< PC format 152                */
    TMDL_HDMIRX_VFMT_PC_1280x1024p_85Hz = 153,  /**< PC format 153                */
    TMDL_HDMIRX_VFMT_PC_1024x768i_87Hz  = 154,  /**< PC format 154                */
    TMDL_HDMIRX_VFMT_PC_MAX             = 154,  /**< Highest valid PC format      */
    TMDL_HDMIRX_VFMT_PC_NUM             = (TMDL_HDMIRX_VFMT_PC_MAX-TMDL_HDMIRX_VFMT_PC_MIN+1)   /**< Number of PC formats         */
} tmdlHdmiRxVidFmt_t;


/**
 * \brief Structure defining the EDID short video descriptor
 */
typedef struct
{
    tmdlHdmiRxVidFmt_t  videoFormat;            /**< Video format as defined by EIA/CEA 861-D */
    Bool                nativeVideoFormat;   /**< True if format is the preferred video format */
} tmdlHdmiRxShortVidDesc_t;

/**
 * \brief Enum listing all the type of sunk
 */
typedef enum
{
    TMDL_HDMIRX_SINK_DVI  = 0, /**< DVI  */
    TMDL_HDMIRX_SINK_HDMI = 1, /**< HDMI */
    TMDL_HDMIRX_SINK_EDID = 2  /**< As currently defined in EDID */
} tmdlHdmiRxSinkType_t;

/**
 * \brief Structure defining detailed timings of a video format
 */
typedef struct
{
    UInt16  pixelClock;        /**< Pixel Clock/10 000         */
    UInt16  hActivePixels;     /**< Horizontal Active Pixels   */
    UInt16  hBlankPixels;      /**< Horizontal Blanking Pixels */
    UInt16  vActiveLines;      /**< Vertical Active Lines      */
    UInt16  vBlankLines;       /**< Vertical Blanking Lines    */
    UInt16  hSyncOffset;       /**< Horizontal Sync Offset     */
    UInt16  hSyncWidth;        /**< Horiz. Sync Pulse Width    */
    UInt16  vSyncOffset;       /**< Vertical Sync Offset       */
    UInt16  vSyncWidth;        /**< Vertical Sync Pulse Width  */
    UInt16  hImageSize;        /**< Horizontal Image Size      */
    UInt16  vImageSize;        /**< Vertical Image Size        */
    UInt16  hBorderPixels;     /**< Horizontal Border          */
    UInt16  vBorderPixels;     /**< Vertical Border            */
    UInt8   flags;             /**< Interlace/sync info        */
} tmdlHdmiRxEdidVideoTimings_t;

/** size descriptor block of monitor descriptor */
#define EDID_MONITOR_DESCRIPTOR_SIZE   13

/**
 * \brief Structure defining the first monitor descriptor
 */
typedef struct
{
    Bool    descRecord;                                 /**< True when parameters of struct are available   */
    UInt8   monitorName[EDID_MONITOR_DESCRIPTOR_SIZE];  /**< Monitor Name                                   */
} tmdlHdmiRxEdidFirstMD_t;

/**
 * \brief Structure defining the second monitor descriptor
 */
typedef struct
{
    Bool    descRecord;             /**< True when parameters of struct are available   */
    UInt8   minVerticalRate;        /**< Min vertical rate in Hz                        */
    UInt8   maxVerticalRate;        /**< Max vertical rate in Hz                        */
    UInt8   minHorizontalRate;      /**< Min horizontal rate in Hz                      */
    UInt8   maxHorizontalRate;      /**< Max horizontal rate in Hz                      */
    UInt8   maxSupportedPixelClk;   /**< Max suuported pixel clock rate in MHz          */
} tmdlHdmiRxEdidSecondMD_t;

/**
 * \brief Structure defining the other monitor descriptor
 */
typedef struct
{
    Bool    descRecord;                                     /**< True when parameters of struct are available   */
    UInt8   otherDescriptor[EDID_MONITOR_DESCRIPTOR_SIZE];  /**< Other monitor Descriptor                       */
} tmdlHdmiRxEdidOtherMD_t;

/**
 * \brief Enum listing all picture aspect ratio (H:V) (4:3, 16:9)
 */
typedef enum
{
    TMDL_HDMIRX_P_ASPECT_RATIO_UNDEFINED    = 0,    /**< Undefined picture aspect ratio */
    TMDL_HDMIRX_P_ASPECT_RATIO_6_5          = 1,    /**< 6:5 picture aspect ratio (PAR) */
    TMDL_HDMIRX_P_ASPECT_RATIO_5_4          = 2,    /**< 5:4 PAR */
    TMDL_HDMIRX_P_ASPECT_RATIO_4_3          = 3,    /**< 4:3 PAR */
    TMDL_HDMIRX_P_ASPECT_RATIO_16_10        = 4,    /**< 16:10 PAR */
    TMDL_HDMIRX_P_ASPECT_RATIO_5_3          = 5,    /**< 5:3 PAR */
    TMDL_HDMIRX_P_ASPECT_RATIO_16_9         = 6,    /**< 16:9 PAR */
    TMDL_HDMIRX_P_ASPECT_RATIO_9_5          = 7     /**< 9:5 PAR */
} tmdlHdmiRxPictAspectRatio_t;

/**
 * \brief EDID information about sink latency
 */
typedef struct
{
    Bool   latency_available;
    Bool   Ilatency_available;
    UInt8  Edidvideo_latency;
    UInt8  Edidaudio_latency;
    UInt8  EdidIvideo_latency;
    UInt8  EdidIaudio_latency;

} tmdlHdmiRxEdidLatency_t;

/**
 * \brief Structure defining the additional Edid VSDB data according to HDMI 1.4a standard
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
} tmdlHdmiRxEdidExtraVsdbData_t;


/**
 * \brief Enum defining possible HotPlug status
 */
typedef enum
{
    TMDL_HDMIRX_HDMIOUT_HOTPLUG_INACTIVE    = 0,    /**< Hotplug inactive */
    TMDL_HDMIRX_HDMIOUT_HOTPLUG_ACTIVE      = 1,    /**< Hotplug active   */
    TMDL_HDMIRX_HDMIOUT_HOTPLUG_INVALID     = 2     /**< Invalid Hotplug  */
} tmdlHdmiRxHdmioutHotPlug_t;

/**
 * \brief Enum defining possible RxSense status
 */
typedef enum
{
    TMDL_HDMIRX_HDMIOUT_RX_SENSE_INACTIVE    = 0,    /**< RxSense inactive */
    TMDL_HDMIRX_HDMIOUT_RX_SENSE_ACTIVE      = 1,    /**< RxSense active   */
    TMDL_HDMIRX_HDMIOUT_RX_SENSE_INVALID     = 2     /**< Invalid RxSense  */
} tmdlHdmiRxHdmioutRxSense_t;



/**
 * \brief Callback function pointer type, used to allow driver to callback
          application when activity status is changing at input.
 * \param Event Identifier of the source event.
 */
typedef void (*ptmdlHdmiRxHdmioutCallback_t) (tmdlHdmiRxEvent_t event);





#endif

#ifdef __cplusplus
}
#endif

#endif /* TMDLHDMIRX_TYPES_H */

/*============================================================================*/
/*                            END OF FILE                                     */
/*============================================================================*/


