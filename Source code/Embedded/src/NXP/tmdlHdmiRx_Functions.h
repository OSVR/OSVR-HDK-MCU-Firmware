#include "GlobalOptions.h"




/**
 * Copyright (C) 2007 NXP N.V., All Rights Reserved.
 * Exposed under Apache 2.0 Open Source License with permission of NXP N.V.
 *
 * \file          tmdlHdmiRx_Functions.h
 *
 * \version       $Revision: 1 $
 *
 * \date          $Date: 06/02/07 8:32 $
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

   $History: tmdlHdmiRx_Functions.h $
 *
 * *****************  Version 1  *****************
 * User: Demoment     Date: 06/02/07   Time: 8:32
 * Updated in $/Source/tmdlHdmiRx/inc
 * initial version
 *

   \endverbatim
 *
*/

#ifndef TMDLHDMIRX_FUNCTIONS_H
#define TMDLHDMIRX_FUNCTIONS_H

/*============================================================================*/
/*                       INCLUDE FILES                                        */
/*============================================================================*/

#include "tmNxTypes.h"
#include "tmdlHdmiRx_Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*/
/*                       EXTERN FUNCTION PROTOTYPES                           */
/*============================================================================*/

/*============================================================================*/
/**
    \brief Get the software version of the driver.
           This function is synchronous.
           This function is ISR friendly.

    \param pSWVersion Pointer to the version structure

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

******************************************************************************/
tmErrorCode_t tmdlHdmiRxGetSWVersion
(
    tmSWVersion_t  *pSWVersion
);

/*============================================================================*/
/**
    \brief Get the number of available HDMI receivers devices in the system.
           A unit directly represents a physical device.
           This function is synchronous.
           This function is ISR friendly.

    \param pUnitCount Pointer to the number of available units.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

******************************************************************************/
tmErrorCode_t tmdlHdmiRxGetNumberOfUnits
(
    UInt32  *pUnitCount
);

/*============================================================================*/
/**
    \brief Get the capabilities of unit 0. Capabilities are stored into a
           dedicated structure and are directly read from the HW device.
           This function is synchronous.
           This function is not ISR friendly.

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
);

/*============================================================================*/
/**
    \brief Get the capabilities of a specific unit. Capabilities are stored
           into a dedicated structure and are directly read from the HW
           device.
           This function is synchronous.
           This function is not ISR friendly.

    \param unit          Unit to be probed.
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
tmErrorCode_t tmdlHdmiRxGetCapabilitiesM
(
    tmUnitSelect_t            unit,
    tmdlHdmiRxCapabilities_t *pCapabilities
);

/*============================================================================*/
/**
    \brief Open unit 0 of HdmiRx driver and provides the instance number to
           the caller. Note that one unit of HdmiRx represents one physical
           HDMI receiver and that only one instance per unit can be opened.
           This function is synchronous.
           This function is not ISR friendly.

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

******************************************************************************/
tmErrorCode_t tmdlHdmiRxOpen
(
    tmInstance_t   *pInstance
);

/*============================================================================*/
/**
    \brief Open a specific unit of HdmiRx driver and provides the instance
           number to the caller. Note that one unit of HdmiRx represents one
           physical HDMI receiver and that only one instance per unit can be
           opened.
           This function is synchronous.
           This function is not ISR friendly.

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

******************************************************************************/
tmErrorCode_t tmdlHdmiRxOpenM
(
    tmInstance_t   *pInstance,
    tmUnitSelect_t  unit
);

/*============================================================================*/
/**
    \brief Close an instance of HdmiRx driver.
           This function is synchronous.
           This function is not ISR friendly.

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
);

/*============================================================================*/
/**
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
);

/*============================================================================*/
/**
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
);

/*============================================================================*/
/**
    \brief Set the configuration of instance attributes when the instance is
           already opened.

           This function is synchronous.
           This function is ISR friendly.

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
);

/*============================================================================*/
/**
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
);

/*============================================================================*/
/**
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
);

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
);

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
tmdlHdmiRxHeartBeat
(
    tmInstance_t    instance
);

/*============================================================================*/
/**
    \brief Register event callbacks. Three types of callbacks can be
           registered : input activity related callback, data related
           callback (infoframes, packets, etc.) and general information
           callback. A null pointer means that no callback are registered.
           This function is synchronous.
           This function is ISR friendly.

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
);

/*============================================================================*/
/**
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
);

/*============================================================================*/
/**
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
);

/*============================================================================*/
/**
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
);

/*============================================================================*/
/**
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

******************************************************************************/
tmErrorCode_t tmdlHdmiRxDetectResolution
(
    tmInstance_t              instance,
    tmdlHdmiRxResolutionID_t *pResolutionID
);

/*============================================================================*/
/**
    \brief Get specifications of a given resolution.

    \param instance         Instance identifier.
    \param resolutionID     ID of the resolution to retrieve specs from.
    \param pResolutionSpecs Pointer to the structure receiving specs.

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
);

/*============================================================================*/
/**
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

******************************************************************************/
tmErrorCode_t tmdlHdmiRxConfigureInput
(
    tmInstance_t             instance,
    tmdlHdmiRxResolutionID_t resolution
);

/*============================================================================*/
/**
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
tmErrorCode_t tmdlHdmiRxMuteOutput
(
    tmInstance_t instance,
    Bool         videoMute,
    Bool         audioMute
);

/*============================================================================*/
/**
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
);

/*============================================================================*/
/**
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
);

/*============================================================================*/
/**
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
);

/*============================================================================*/
/**
    \brief Set video output format. This function allows setting the general
           parameters of the output signal.

    \param instance    Instance identifier.
    \param audioFormat Audio output format (I2S, SPDIF, OBA)
    \param audioSysClk Audio sysclk frequency (128,256,512FS)

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
);

/*============================================================================*/
/**
    \brief Set the channel assignment. This function allows to manually
           configure the channel assignment instead of using the data of
           the audio infoframe.

    \param instance                 Instance identifier.
    \param channelAssignment        Channel assignment value (number of channels)
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
);





/*============================================================================*/
/**
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
    \param ksvReady         KSV list is ready

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
);



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
);

/*============================================================================*/
/**
    \brief Get current audio sample frequency.

    \param instance  Instance identifier.
    \param pDstRate   DST single/double rate
    \param pAudioFreq Audio sample frequency

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
);

/*============================================================================*/
/**
    \brief Loads EDID data into embedded EDID memory of receiver device.

    \param instance   Instance identifier.
    \param commonEDID Pointer to the two block EDID (array of 256 bytes)
                      that will be common to all HDMI inputs. If null pointer is provided,
                      only sPA data are written.
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
);

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
tmErrorCode_t tmdlHdmiRxReadEdidData
(
    tmInstance_t  instance,
    UInt8         *commonEDID,
    UInt16        *sPA,
    UInt8         *checksum,
    UInt8         *sPAOffset
);

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
);

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
);

/*============================================================================*/
/**
    \brief Save EDID data from MEMORY to MTP.

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
tmErrorCode_t tmdlHdmiRxSaveEdidDataToMTP
(
    tmInstance_t  instance
);

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
);

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
);

/*============================================================================*/
/**
    \brief Writing on I2C bus (for debug purpose only)

    \param instance Instance identifier.
    \param page     Page number of the register to write
    \param reg      Address of the first register to write
    \param length   Data length (number of bytes)
    \param pData    Pointer of data to be written

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
    UInt8 page,
    UInt8 reg,
    UInt8 length,
    UInt8 *pData
);

/*============================================================================*/
/**
    \brief        Reading on I2C bus (for debug purpose only)

    \param instance Instance identifier.
    \param page     Page number of the register to read
    \param reg      Address of the first register to read
    \param length   Data length (number of bytes)
    \param pData    Pointer of data to be read

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
    UInt8 page,
    UInt8 reg,
    UInt8 length,
    UInt8 *pData
);

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
tmErrorCode_t tmdlHdmiRxDriveHdcp
(
    tmInstance_t                   instance,
    Bool                           enable
);

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
);

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
);




/*============================================================================*/
/* Following APIs are defined for TDA19972 and TDA19973 only */
/*============================================================================*/

/*============================================================================*/
/**
    \brief Reloads EDID data from MTP.

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
tmErrorCode_t tmdlHdmiRxReLoadMTPEdidData
(
    tmInstance_t  instance
);

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
tmErrorCode_t tmdlHdmiRxReadSecondaryEdidData
(
    tmInstance_t  instance,
    UInt8         *commonEDID,
    UInt16        *sPA,
    UInt8         *checksum,
    UInt8         *sPAOffset
);


/*============================================================================*/
/**
    \brief Loads EDID data into embedded secondary EDID memory of receiver device.

    \param instance   Instance identifier.
    \param commonEDID Pointer to the two block EDID (array of 256 bytes)
                      that will be common to all HDMI inputs. If null pointer is provided,
                      only sPA data are written.
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
);


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
tmErrorCode_t tmdlHdmiRxAttachEdidToInput
(
    tmInstance_t  instance,
    tmdlHdmiRxInput_t  inputSel
);


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
tmErrorCode_t tmdlHdmiRxAttachSecondaryEdidToInput
(
    tmInstance_t  instance,
    tmdlHdmiRxInput_t  inputSel
);



/*============================================================================*/
/**
    \brief Copy secondary EDID data to Edid data. sPAOffset is also copied,
           but sPA are not copied.

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
tmErrorCode_t tmdlHdmiRxCopySecondaryEdidToEdid
(
    tmInstance_t  instance
);


/*============================================================================*/
/**
    \brief Copy EDID data to secondary Edid data. sPAOffset is also copied,
           but sPA are not copied.

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
tmErrorCode_t tmdlHdmiRxCopyEdidToSecondaryEdid
(
    tmInstance_t  instance
);


/*============================================================================*/
/*                              specific FUNCTIONS                            */
/*============================================================================*/

#ifdef TMFL_TDA19972_FAMILY

tmErrorCode_t tmdlHdmiRxConfigureOutputVideoPort
(
    tmInstance_t  instance,
    tmdlVPBitsConfig *VideoPortConfiguration
);

tmErrorCode_t tmdlHdmiRxManualHPD
(
    tmInstance_t  instance,
    tmdlHdmiRxHPDManual_t  ManualHPD
);
#endif

#ifdef TMFL_HDMI_OUT

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
);


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
);


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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);


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
);


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
);

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
);

#endif





#ifdef __cplusplus
}
#endif

#endif /* TMDLHDMIRX_FUNCTIONS_H */

/*============================================================================*/
/*                            END OF FILE                                     */
/*============================================================================*/

