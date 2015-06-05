#include "GlobalOptions.h"



/**
 * Copyright (C) 2006 NXP N.V., All Rights Reserved.
 * Exposed under Apache 2.0 Open Source License with permission of NXP N.V.
 *
 * \file          tmdlHdmiRx_bslAPIfunctions.h
 *
 * \version       $Revision: 1 $
 *
 * \date          $Date: 12/10/06 10:32 $
 *
 * \brief         BSL driver component API for the TDA997x HDMI Receiver
 *
 * \section refs  Reference Documents
 * HDMI Rx Driver - FRS.doc,
 *
 * \section info  Change Information
 *
 * \verbatim

   $History: tmdlHdmiRx_bslAPIfunctions.h $
 *
   \endverbatim
 *
*/

#ifndef TMDLHDMIRX_BSLAPIFUNCTIONS_H
#define TMDLHDMIRX_BSLAPIFUNCTIONS_H

/*============================================================================*/
/*                       INCLUDE FILES                                        */
/*============================================================================*/

#include "tmNxTypes.h"
#include "tmbslHdmiRx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*/
/*                       EXTERN FUNCTION PROTOTYPES                           */
/*============================================================================*/

typedef tmErrorCode_t (*ptmbslHdmiRxInit_t) (tmUnitSelect_t,
        UInt8,
#ifdef TMFL_TDA19972_FAMILY
        UInt8,
#endif

        ptmbslHdmiRxSysFunc_t,
        ptmbslHdmiRxSysFunc_t,
        ptmbslHdmiRxCallbackFunc_t,
        ptmbslHdmiRxSysFuncTimer_t);

typedef tmErrorCode_t (*ptmbslHdmiRxDeInit_t) (tmUnitSelect_t);

typedef tmErrorCode_t (*ptmbslHdmiRxGetChipInfo_t) (tmUnitSelect_t,
        tmbslHdmiRxVersion_t*);

typedef tmErrorCode_t (*ptmbslHdmiRxHandleInterrupt_t) (tmUnitSelect_t);

typedef tmErrorCode_t (*ptmbslHdmiRxGetInterruptStatus_t) (tmUnitSelect_t,
        tmbslHdmiRxIRQSource_t);
typedef tmErrorCode_t (*ptmbslHdmiRxHeartBeat_t) (tmUnitSelect_t);

typedef tmErrorCode_t (*ptmbslHdmiRxSetInterruptMask_t) (tmUnitSelect_t,
        tmbslHdmiRxIRQSource_t,
        UInt8);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigureInput_t) (tmUnitSelect_t,
        tmbslHdmiRxInputSelSyncType_t,
        tmbslHdmiRxInputSelDigitalMode_t,
        tmbslHdmiRxInputSelDigitalSource_t,
        tmbslHdmiRxInputSelVideoSource_t,
        tmbslHdmiRxInputSelAnalogVideoSource_t,
        tmbslHdmiRxInputSelVDPResetMode_t,
        tmbslHdmiRxInputSelVDPReset_t);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigureSyncOnGreenInput_t) (tmUnitSelect_t,
        tmbslHdmiRxSogBurst_t);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigureSyncDetection_t) (tmUnitSelect_t,
        tmbslHdmiRxSyncDetectAtv_t,
        tmbslHdmiRxSyncDetectComposite_t,
        tmbslHdmiRxSyncDetect_t,
        tmbslHdmiRxForceSog_t,
        tmbslHdmiRxSForceDcs_t,
        tmbslHdmiRxSVsyncPolarityMode_t,
        tmbslHdmiRxSVsyncToggle_t,
        tmbslHdmiRxSHsyncPolarityMode_t,
        tmbslHdmiRxHsyncToggle_t);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigurePLL_t) (tmUnitSelect_t,
        tmbslHdmiRxPllMdiv_t,
        UInt16,
        tmbslHdmiRxPllEdgeSync_t,
        tmbslHdmiRxPllCoastMode_t,
        tmbslHdmiRxPllCoastSelection_t,
        tmbslHdmiRxCoastSignalValues_t*);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigureADC_t) (tmUnitSelect_t,
        tmbslHdmiRxADCPolarity_t,
        tmbslHdmiRxADCClkDelay_t,
        tmbslHdmiRxADCCurrentPBp_t,
        tmbslHdmiRxADCCurrentGy_t,
        tmbslHdmiRxADCCurrentPRp_t);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigureDLL_t) (tmUnitSelect_t,
        UInt8,
        UInt8,
        UInt8,
        tmbslHdmiRxDllDivClkOut_t,
        tmbslHdmiRxDllDivClkPix_t,
        tmbslHdmiRxDllDivPrDelay_t,
        tmbslHdmiRxDllDivPhEdge_t);

typedef tmErrorCode_t (*ptmbslHdmiRxSetClockPulseDelay_t) (tmUnitSelect_t,
        tmbslHdmiRxInputSelClockPulseDelay_t,
        UInt8);

#ifdef TMFL_TDA19972_FAMILY

typedef tmErrorCode_t (*ptmbslHdmiRxConfigurePixelClockGenerator_t) (tmUnitSelect_t,
        tmbslHdmiRxPixClkToggle_t         clockOutToggle,
        tmbslHdmiRxOutputFormat_t         outputFormat,
        tmbslHdmiRxOutClockEdgeMode_t     clockOutEdgeMode);
#endif

typedef tmErrorCode_t (*ptmbslHdmiRxConfigureClamping_t) (tmUnitSelect_t,
        UInt16,
        UInt16,
        UInt16,
        tmbslHdmiRxClampUpdateRate_t,
        tmbslHdmiRxClampDigitalAdjustment_t,
        tmbslHdmiRxClampPulseSelection_t,
        tmbslHdmiRxClampSignalValues_t*);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigureVideoGain_t) (tmUnitSelect_t,
        tmbslHdmiRxGainValue_t*,
        tmbslHdmiRxGainValue_t*,
        tmbslHdmiRxGainValue_t*,
        tmbslHdmiRxGainPulseSelection_t,
        UInt16,
        UInt16);

typedef tmErrorCode_t (*ptmbslHdmiRxGetOutOfRangePixels_t) (tmUnitSelect_t,
        UInt8*,
        UInt8*,
        UInt8*);

typedef tmErrorCode_t (*ptmbslHdmiRxGetSyncTimings_t) (tmUnitSelect_t,
        UInt32*,
        UInt16*,
        UInt16*,
        tmbslHdmiRxFormatMeas_t*);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigureColorSpaceConversion_t) (tmUnitSelect_t,
        tmbslHdmiRxColorSpaceBypass_t,
        tmbslHdmiRxColorSpaceCoefficients_t*);

typedef tmErrorCode_t (*ptmbslHdmiRxSetPixelAndLineCounters_t) (tmUnitSelect_t,
        UInt16,
        UInt16,
        UInt16,
        UInt16);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigureVHRef_t) (tmUnitSelect_t,
        tmbslHdmiRxVhrefInterlaceDetection_t,
        tmbslHdmiRxVhrefFrameVsync_t,
        tmbslHdmiRxVhrefStandardDetection_t,
        tmbslHdmiRxVhrefVrefProg_t,
        tmbslHdmiRxVhrefHrefProg_t,
        tmbslHdmiRxVhrefCSyncPol_t);

typedef tmErrorCode_t (*ptmbslHdmiRxSetVHRefValues_t) (tmUnitSelect_t,
        tmbslHdmiRxVHRefValues_t*);

typedef tmErrorCode_t (*ptmbslHdmiRxSetVHSyncValues_t) (tmUnitSelect_t,
        tmbslHdmiRxVHSyncValues_t*);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigureFrameDetectionWindow_t) (tmUnitSelect_t,
        UInt16,
        UInt16);

typedef tmErrorCode_t (*ptmbslHdmiRxGetFrameMeasurements_t) (tmUnitSelect_t,
        tmbslHdmiRxAsdMeasureInterlaced_t*,
        tmbslHdmiRxVhrefAsdLineStandard_t*,
        tmbslHdmiRxVhrefAsdMeaslin525_t*,
        UInt16*,
        UInt16*);

typedef tmErrorCode_t (*ptmbslHdmiRxSetVHSyncDelay_t) (tmUnitSelect_t,
        UInt8,
        UInt8);

typedef tmErrorCode_t (*ptmbslHdmiRxSetBlankingCodes_t) (tmUnitSelect_t,
        UInt16,
        UInt16,
        UInt16);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigurePreFilter_t) (tmUnitSelect_t,
        tmbslHdmiRxPreFilterConfig_t,
        tmbslHdmiRxPreFilterConfig_t);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigureRangeControl_t) (tmUnitSelect_t,
        UInt16,
        UInt16,
        UInt16,
        UInt16);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigureVideoOutput_t) (tmUnitSelect_t,
        tmbslHdmiRxOutputControl_t,
        tmbslHdmiRxOutputVideoPort_t,
        tmbslHdmiRxOutputRef_t,
        tmbslHdmiRxOutputBlankingCodes_t,
        tmbslHdmiRxOutputTimingRefs_t,
        tmbslHdmiRxOutputFormat_t);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigureUpDownSampler_t) (tmUnitSelect_t,
        tmbslHdmiRxDownSampler_t,
        tmbslHdmiRxUpSampler_t);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigureInternalVHSync_t) (tmUnitSelect_t,
        tmbslHdmiRxInternalVSync_t,
        tmbslHdmiRxInternalHSync_t);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigureSyncOutput_t) (tmUnitSelect_t,
        tmbslHdmiRxSyncOutputCSSelection_t,
        tmbslHdmiRxSyncOutputVSSelection_t,
        tmbslHdmiRxSyncOutputHSSelection_t);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigureOutputPolarity_t) (tmUnitSelect_t,
        tmbslHdmiRxPolarityDataEnOut_t,
        tmbslHdmiRxPolarityCSyncOut_t,
        tmbslHdmiRxPolarityHSyncOut_t,
        tmbslHdmiRxPolarityVSyncOut_t,
        tmbslHdmiRxPolarityFieldrefOut_t,
        tmbslHdmiRxPolarityHRefOut_t,
        tmbslHdmiRxPolarityVRefOut_t,
        tmbslHdmiRxAutomaticSyncPolarity_t);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigureVideoPort_t) (tmUnitSelect_t,
        tmbslHdmiRxVideoPortFormat_t,
        tmbslHdmiRxVideoPortSelection_t,
        tmbslHdmiRxVideoPortSelection_t,
        tmbslHdmiRxVideoPortSelection_t);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigureDataEnable_t) (tmUnitSelect_t,
        tmbslHdmiRxDataEnHRefExpand_t,
        tmbslHdmiRxDataEnHRefForceLow_t,
        tmbslHdmiRxDataEnExpand_t,
        tmbslHdmiRxDataEnSelection_t,
        UInt8);

typedef tmErrorCode_t (*ptmbslHdmiRxGetTerminationResistanceCalibration_t) (tmUnitSelect_t,
        UInt8*,
        UInt8*);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigureTerminationResistance_t) (tmUnitSelect_t,
        tmbslHdmiRxTRCalibrationMode_t,
        UInt8,
        tmbslHdmiRxTRCalibrationMode_t,
        UInt8);

typedef tmErrorCode_t (*ptmbslHdmiRxAVISoftReset_t) (tmUnitSelect_t,
        tmbslHdmiRxResetMode_t,
        tmbslHdmiRxResetAVI_t);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigurePLLOutput_t) (tmUnitSelect_t,
        UInt8,
        tmbslHdmiRxPllOutputLock_t,
        tmbslHdmiRxPllOutputSignal_t);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigureOutOfRangeOutput_t) (tmUnitSelect_t,
        tmbslHdmiRxOutOfRangeRCR_t,
        tmbslHdmiRxOutOfRangeGY_t,
        tmbslHdmiRxOutOfRangeBCB_t);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigureHDMI_t) (tmUnitSelect_t,
        tmbslHdmiRxHdmiMute_t,
        tmbslHdmiRxHdcpMode_t,
        tmbslHdmiRxHdmiProtocol_t,
        tmbslHdmiRxHdmiVsyncPolarity_t,
        tmbslHdmiRxHdmiVsyncToggle_t);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigureHDMIClock_t) (tmUnitSelect_t,
        tmbslHdmiRxHdmiClock_t);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigureAudioFormatter_t) (tmUnitSelect_t,
        tmbslHdmiRxAudioPathSpFlagMode_t,
        UInt8,
        tmbslHdmiRxAudioPathLayoutMode_t,
        tmbslHdmiRxAudioPathForcedLayout_t,
        UInt8,
        tmbslHdmiRxAudioTestTone_t,
        tmbslHdmiRxAudioOutputFormat_t,
        tmbslHdmiRxAudioOutputForce_t,
        tmbslHdmiRxAudioOutputForce_t,
        tmbslHdmiRxAudioOutputForce_t,
        tmbslHdmiRxAudioOutputForce_t,
        tmbslHdmiRxAudioOutputForce_t,
        tmbslHdmiRxAudioOutputForce_t,
        tmbslHdmiRxAudioOutputForce_t,
        tmbslHdmiRxAudioOutputForce_t,
        tmbslHdmiRxAudioOutputForce_t,
        tmbslHdmiRxAudioOutputForce_t);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigurePixelRepeater_t) (tmUnitSelect_t,
        UInt8);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigureAudioClock_t) (tmUnitSelect_t,
        UInt32,
        UInt32,
        tmbslHdmiRxAudioClockPowerMode_t,
        tmbslHdmiRxAudioClockDividerUpdate_t,
        tmbslHdmiRxAudioClockDividerMode_t,
        tmbslHdmiRxAudioClockFrequency_t,
        tmbslHdmiRxAudioSampleFrequency_t);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigureAudioSelection_t) (tmUnitSelect_t,
        tmbslHdmiRxAudioPacketMode_t,
        tmbslHdmiRxAudioPLLInputRef_t,
        tmbslHdmiRxAudioI2SResolution_t,
        tmbslHdmiRxAudioPllSelection_t,
        tmbslHdmiRxHBROutputMode_t);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigurePacketTypeScan_t) (tmUnitSelect_t,
        tmbslHdmiRxPacketTypeScan_t);

typedef tmErrorCode_t (*ptmbslHdmiRxAudioAutoMute_t) (tmUnitSelect_t,
        tmbslHdmiRxAudioAutoMute_t);

typedef tmErrorCode_t (*ptmbslHdmiRxGetHDMIStatus_t) (tmUnitSelect_t,
        UInt32*);

typedef tmErrorCode_t (*ptmbslHdmiRxGetAudioStatus_t) (tmUnitSelect_t,
        UInt32*);

typedef tmErrorCode_t (*ptmbslHdmiRxGetAudioClockRegPacket_t) (tmUnitSelect_t,
        UInt32*,
        UInt32*);

typedef tmErrorCode_t (*ptmbslHdmiRxGetAudioSampleInfo_t) (tmUnitSelect_t,
        tmbslHdmiRxAudioSampleInfo_t*);


typedef tmErrorCode_t (*ptmbslHdmiRxGetDataIslandErrorCorrection_t) (tmUnitSelect_t,
        UInt16*,
        UInt16*);

typedef tmErrorCode_t (*ptmbslHdmiRxGetAudioReadWriteDelta_t) (tmUnitSelect_t,
        UInt8*);

typedef tmErrorCode_t (*ptmbslHdmiRxGetAudioContentProtectionUpdate_t) (tmUnitSelect_t  unit,
        UInt8*);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigureOTP_t) (tmUnitSelect_t,
        tmbslHdmiRxOtpCommand_t,
        tmbslHdmiRxBchEnable_t);

typedef tmErrorCode_t (*ptmbslHdmiRxSetOTPAddress_t) (tmUnitSelect_t,
        UInt8);

typedef tmErrorCode_t (*ptmbslHdmiRxReadOTP_t) (tmUnitSelect_t, UInt32*);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigureHDCP_t) (tmUnitSelect_t,
        tmbslHdmiRxHdcpDecryptKey_t,
        tmbslHdmiRxHdcpEnable_t,
        UInt8,
        UInt16);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigureReceiverCaps_t) (tmUnitSelect_t,
        tmbslHdmiRxCapsRepeater_t,
        tmbslHdmiRxCapsFastI2c_t,
        tmbslHdmiRxCapsHdmi11_t,
        tmbslHdmiRxCapsFastReauth_t);

typedef tmErrorCode_t (*ptmbslHdmiRxSetHDCPRepeaterStatus_t) (tmUnitSelect_t,
        tmbslHdmiRxHDCPRepStatusMaxDev_t,
        UInt8,
        tmbslHdmiRxHDCPRepStatusMaxCascade_t,
        UInt8);

typedef tmErrorCode_t (*ptmbslHdmiRxSetHDCPRepeaterControl_t) (tmUnitSelect_t,
        UInt8,
        UInt8,
        UInt8);

typedef tmErrorCode_t (*ptmbslHdmiRxSetKeySelectionVector_t) (tmUnitSelect_t,
        UInt8[5]);

typedef tmErrorCode_t (*ptmbslHdmiRxSetPrivateKeyIndex_t) (tmUnitSelect_t,
        UInt8);

typedef tmErrorCode_t (*ptmbslHdmiRxSetPrivateKey_t) (tmUnitSelect_t,
        UInt8[7]);

typedef tmErrorCode_t (*ptmbslHdmiRxSetKSVIndex_t) (tmUnitSelect_t,
        UInt8);

typedef tmErrorCode_t (*ptmbslHdmiRxSetKSVValue_t) (tmUnitSelect_t,
        UInt8[5]);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigureHDCPErrorProtection_t) (tmUnitSelect_t,
        UInt8,
        tmbslHdmiRxDEMeasurementMode_t,
        tmbslHdmiRxDERegeneration_t,
        tmbslHdmiRxDEFilterSensivity_t,
        tmbslHdmiRxDECompositionMode_t,
        tmbslHdmiRxCTLFilterSensivity_t,
        tmbslHdmiRxVSFilterSensivity_t,
        tmbslHdmiRxHSFilterSensivity_t);

typedef tmErrorCode_t (*ptmbslHdmiRxGetDisplayData_t) (tmUnitSelect_t,
        UInt16*,
        UInt8[5],
        UInt8[8],
        UInt8*,
        UInt8*);

typedef tmErrorCode_t (*ptmbslHdmiRxGetInfoframe_t) (tmUnitSelect_t,
        tmbslHdmiRxInfoFrame_t,
        UInt8*,
        UInt8*,
        UInt8*,
        UInt8*,
        UInt8*);

typedef tmErrorCode_t (*ptmbslHdmiRxGetACPPacket_t) (tmUnitSelect_t,
        UInt8*,
        UInt8*,
        UInt8[28]);

typedef tmErrorCode_t (*ptmbslHdmiRxReadISRC1Packet_t) (tmUnitSelect_t,
        UInt8*,
        UInt8*,
        UInt8[16]);

typedef tmErrorCode_t (*ptmbslHdmiRxReadISRC2Packet_t) (tmUnitSelect_t,
        UInt8*,
        UInt8[16]);

typedef tmErrorCode_t (*ptmbslHdmiRxReadGBDPacket_t) (tmUnitSelect_t,
        UInt8*,
        UInt8[2],
        UInt8[28]);

typedef tmErrorCode_t (*ptmbslHdmiRxHDMISoftReset_t) (tmUnitSelect_t,
        tmbslHdmiRxHdmiNackHdcp_t,
        tmbslHdmiRxHdmiResetAi_t,
        tmbslHdmiRxHdmiResetInfoframe_t,
        tmbslHdmiRxHdmiResetAudioFifo_t,
        tmbslHdmiRxHdmiResetGamut_t,
        tmbslHdmiRxHdmiResetFifoCtrl_t,
        tmbslHdmiRxHdmiResetSus_t,
        tmbslHdmiRxHdmiResetDc_t);

typedef tmErrorCode_t (*ptmbslHdmiRxGetCapabilities_t) (tmUnitSelect_t,
        tmbslHdmiRxCapabilities_t*);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigureAudioClkMode_t) (tmUnitSelect_t,
        tmbslHdmiRxAudioClocksModePowerMode_t,
        tmbslHdmiRxAudioClocksModeRefMode_t,
        tmbslHdmiRxAudioClocksModeRefFreq_t,
        tmbslHdmiRxAudioClocksAudioMode_t,
        tmbslHdmiRxAudioClocksSysMode_t);

typedef tmErrorCode_t (*ptmbslHdmiRxReadGCP_t) (tmUnitSelect_t,
        UInt8*,
        UInt8[2],
        UInt8[7]);

typedef tmErrorCode_t (*ptmbslHdmiRxWriteI2C_t) (tmUnitSelect_t,
        UInt16,
        UInt8,
        UInt8*);

typedef tmErrorCode_t (*ptmbslHdmiRxReadI2C_t) (tmUnitSelect_t,
        UInt16,
        UInt8,
        UInt8*);

typedef tmErrorCode_t (*ptmbslHdmiRxGetTMDSFrequency_t) (tmUnitSelect_t,
        tmbslHdmiRxInputSelDigitalSource_t,
        UInt32*);

typedef tmErrorCode_t (*ptmbslHdmiRxGetAudioFrequency_t) (tmUnitSelect_t,
        tmbslHdmiRxAudioDstRate_t*,
        tmbslHdmiRxAudioSampleFreq_t*);

typedef tmErrorCode_t (*ptmbslHdmiRxConfigureEDID_t)(tmUnitSelect_t,
        tmbslHdmiRxEDIDNack_t,
        tmbslHdmiRxEDIDReset_t,
        tmbslHdmiRxEDIDPowerDown_t,
        tmbslHdmiRxEDIDPowerDownDDC_t);

typedef tmErrorCode_t (*ptmbslHdmiRxLoadEDIDData_t)(tmUnitSelect_t,
        UInt8*,
        UInt16*,
        UInt8*,
        UInt8);

typedef tmErrorCode_t (*ptmbslHdmiRxGetDeepColorMode_t)(tmUnitSelect_t,
        UInt8*,
        tmbslHdmiRxDeepColorMode_t*);

typedef tmErrorCode_t (*ptmbslHdmiRxSetPowerState_t)(tmUnitSelect_t,
        tmPowerState_t);

typedef tmErrorCode_t (*ptmbslHdmiRxGetPowerState_t)(tmUnitSelect_t,
        tmPowerState_t*);

typedef tmErrorCode_t (*ptmbslHdmiRxActivateTestPattern_t)(tmUnitSelect_t,
        tmbslHdmiRxTestPatternSel_t,
        tmbslHdmiRxTestPatternFormat_t);

typedef tmErrorCode_t (*ptmbslHdmiRxGetChannelStatus_t) (tmUnitSelect_t,
        UInt8*);

#ifdef TMFL_HDMI_OUT
typedef tmErrorCode_t (*ptmbslHdmiRxSetHdmiOutMode_t) (tmUnitSelect_t,
        tmbslHdmiRxHdmiOutMode_t);
#endif


typedef struct _tmbslHdmiRxFunctionsList_t
{
    ptmbslHdmiRxInit_t                                tmbslHdmiRxInit;
    ptmbslHdmiRxDeInit_t                              tmbslHdmiRxDeInit;
    ptmbslHdmiRxGetChipInfo_t                         tmbslHdmiRxGetChipInfo;
    ptmbslHdmiRxHandleInterrupt_t                     tmbslHdmiRxHandleInterrupt;
    ptmbslHdmiRxGetInterruptStatus_t                  tmbslHdmiRxGetInterruptStatus;
    ptmbslHdmiRxHeartBeat_t                           tmbslHdmiRxHeartBeat;
    ptmbslHdmiRxSetInterruptMask_t                    tmbslHdmiRxSetInterruptMask;
    ptmbslHdmiRxConfigureInput_t                      tmbslHdmiRxConfigureInput;
    ptmbslHdmiRxSetClockPulseDelay_t                  tmbslHdmiRxSetClockPulseDelay;
    ptmbslHdmiRxConfigurePixelClockGenerator_t        tmbslHdmiRxConfigurePixelClockGenerator;
    ptmbslHdmiRxGetSyncTimings_t                      tmbslHdmiRxGetSyncTimings;
    ptmbslHdmiRxConfigureColorSpaceConversion_t       tmbslHdmiRxConfigureColorSpaceConversion;
    ptmbslHdmiRxSetPixelAndLineCounters_t             tmbslHdmiRxSetPixelAndLineCounters;
    ptmbslHdmiRxConfigureVHRef_t                      tmbslHdmiRxConfigureVHRef;
    ptmbslHdmiRxSetVHRefValues_t                      tmbslHdmiRxSetVHRefValues;
    ptmbslHdmiRxSetVHSyncValues_t                     tmbslHdmiRxSetVHSyncValues;
    ptmbslHdmiRxConfigureFrameDetectionWindow_t       tmbslHdmiRxConfigureFrameDetectionWindow;
    ptmbslHdmiRxGetFrameMeasurements_t                tmbslHdmiRxGetFrameMeasurements;
    ptmbslHdmiRxSetVHSyncDelay_t                      tmbslHdmiRxSetVHSyncDelay;
    ptmbslHdmiRxSetBlankingCodes_t                    tmbslHdmiRxSetBlankingCodes;
    ptmbslHdmiRxConfigurePreFilter_t                  tmbslHdmiRxConfigurePreFilter;
    ptmbslHdmiRxConfigureRangeControl_t               tmbslHdmiRxConfigureRangeControl;
    ptmbslHdmiRxConfigureVideoOutput_t                tmbslHdmiRxConfigureVideoOutput;
    ptmbslHdmiRxConfigureUpDownSampler_t              tmbslHdmiRxConfigureUpDownSampler;
    ptmbslHdmiRxConfigureInternalVHSync_t             tmbslHdmiRxConfigureInternalVHSync;
    ptmbslHdmiRxConfigureSyncOutput_t                 tmbslHdmiRxConfigureSyncOutput;
    ptmbslHdmiRxConfigureOutputPolarity_t             tmbslHdmiRxConfigureOutputPolarity;
    ptmbslHdmiRxConfigureVideoPort_t                  tmbslHdmiRxConfigureVideoPort;
    ptmbslHdmiRxConfigureDataEnable_t                 tmbslHdmiRxConfigureDataEnable;
    ptmbslHdmiRxConfigureHDMI_t                       tmbslHdmiRxConfigureHDMI;
    ptmbslHdmiRxConfigureHDMIClock_t                  tmbslHdmiRxConfigureHDMIClock;
    ptmbslHdmiRxConfigureAudioFormatter_t             tmbslHdmiRxConfigureAudioFormatter;
    ptmbslHdmiRxConfigurePixelRepeater_t              tmbslHdmiRxConfigurePixelRepeater;
    ptmbslHdmiRxConfigureAudioSelection_t             tmbslHdmiRxConfigureAudioSelection;
    ptmbslHdmiRxConfigurePacketTypeScan_t             tmbslHdmiRxConfigurePacketTypeScan;
    ptmbslHdmiRxAudioAutoMute_t                       tmbslHdmiRxAudioAutoMute;
    ptmbslHdmiRxGetHDMIStatus_t                       tmbslHdmiRxGetHDMIStatus;
    ptmbslHdmiRxGetHDMIStatus_t                       tmbslHdmiRxGetAudioStatus;
    ptmbslHdmiRxGetAudioClockRegPacket_t              tmbslHdmiRxGetAudioClockRegPacket;
    ptmbslHdmiRxGetDataIslandErrorCorrection_t        tmbslHdmiRxGetDataIslandErrorCorrection;
    ptmbslHdmiRxGetAudioReadWriteDelta_t              tmbslHdmiRxGetAudioReadWriteDelta;
    ptmbslHdmiRxGetAudioContentProtectionUpdate_t     tmbslHdmiRxGetAudioContentProtectionUpdate;
    ptmbslHdmiRxConfigureOTP_t                        tmbslHdmiRxConfigureOTP;
    ptmbslHdmiRxSetOTPAddress_t                       tmbslHdmiRxSetOTPAddress;
    ptmbslHdmiRxReadOTP_t                             tmbslHdmiRxReadOTP;
    ptmbslHdmiRxConfigureHDCP_t                       tmbslHdmiRxConfigureHDCP;
    ptmbslHdmiRxConfigureReceiverCaps_t               tmbslHdmiRxConfigureReceiverCaps;
    ptmbslHdmiRxSetHDCPRepeaterStatus_t               tmbslHdmiRxSetHDCPRepeaterStatus;
    ptmbslHdmiRxSetHDCPRepeaterControl_t              tmbslHdmiRxSetHDCPRepeaterControl;
    ptmbslHdmiRxSetKeySelectionVector_t               tmbslHdmiRxSetKeySelectionVector;
    ptmbslHdmiRxSetPrivateKeyIndex_t                  tmbslHdmiRxSetPrivateKeyIndex;
    ptmbslHdmiRxSetPrivateKey_t                       tmbslHdmiRxSetPrivateKey;
    ptmbslHdmiRxSetKSVIndex_t                         tmbslHdmiRxSetKSVIndex;
    ptmbslHdmiRxSetKSVValue_t                         tmbslHdmiRxSetKSVValue;
    ptmbslHdmiRxConfigureHDCPErrorProtection_t        tmbslHdmiRxConfigureHDCPErrorProtection;
    ptmbslHdmiRxGetDisplayData_t                      tmbslHdmiRxGetDisplayData;
    ptmbslHdmiRxGetInfoframe_t                        tmbslHdmiRxGetInfoframe;
    ptmbslHdmiRxGetACPPacket_t                        tmbslHdmiRxGetACPPacket;
    ptmbslHdmiRxReadISRC1Packet_t                     tmbslHdmiRxReadISRC1Packet;
    ptmbslHdmiRxReadISRC2Packet_t                     tmbslHdmiRxReadISRC2Packet;
    ptmbslHdmiRxReadGBDPacket_t                       tmbslHdmiRxReadGBDPacket;
    ptmbslHdmiRxHDMISoftReset_t                       tmbslHdmiRxHDMISoftReset;
    ptmbslHdmiRxGetCapabilities_t                     tmbslHdmiRxGetCapabilities;
    ptmbslHdmiRxConfigureAudioClkMode_t               tmbslHdmiRxConfigureAudioClkMode;
    ptmbslHdmiRxReadGCP_t                             tmbslHdmiRxReadGCP;
    ptmbslHdmiRxWriteI2C_t                            tmbslHdmiRxWriteI2C;
    ptmbslHdmiRxReadI2C_t                             tmbslHdmiRxReadI2C;
    ptmbslHdmiRxGetTMDSFrequency_t                    tmbslHdmiRxGetTMDSFrequency;
    ptmbslHdmiRxGetAudioFrequency_t                   tmbslHdmiRxGetAudioFrequency;
    ptmbslHdmiRxConfigureEDID_t                       tmbslHdmiRxConfigureEDID;
    ptmbslHdmiRxLoadEDIDData_t                        tmbslHdmiRxLoadEDIDData;
    ptmbslHdmiRxGetDeepColorMode_t                    tmbslHdmiRxGetDeepColorMode;
    ptmbslHdmiRxSetPowerState_t                       tmbslHdmiRxSetPowerState;
    ptmbslHdmiRxActivateTestPattern_t                 tmbslHdmiRxActivateTestPattern;
    ptmbslHdmiRxGetChannelStatus_t                    tmbslHdmiRxGetChannelStatus;
#ifdef TMFL_HDMI_OUT
    ptmbslHdmiRxSetHdmiOutMode_t					  tmbslHdmiRxSetHdmiOutMode;
#endif
} tmbslHdmiRxFunctionsList_t, *ptmbslHdmiRxFunctionsList_t;

void HZ5(void); // debug function

#ifdef __cplusplus
}
#endif

#endif /* TMDLHDMIRX_BSLAPIFUNCTIONS_H */

/*============================================================================*/
/*                            END OF FILE                                     */
/*============================================================================*/

