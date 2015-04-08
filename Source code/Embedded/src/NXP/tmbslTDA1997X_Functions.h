#include "GlobalOptions.h"



/**
 * Copyright (C) 2006 NXP N.V., All Rights Reserved.
 * This source code and any compilation or derivative thereof is the proprietary
 * information of NXP N.V. and is confidential in nature. Under no circumstances
 * is this software to be  exposed to or placed under an Open Source License of
 * any type without the expressed written permission of NXP N.V.
 *
 * \file          tmbslTDA1997X_functions.h
 *
 * \version       $Revision: 1 $
 *
 * \date          $Date: 04/06/07 10:30 $
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

   $History: tmbslTDA1997X_functions.h $
 *

   \endverbatim
 *
*/

#ifndef tmbslTDA1997X_FUNCTIONS_H
#define tmbslTDA1997X_FUNCTIONS_H

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

/**
    \brief Create an instance of an HDMI Receiver: initialize the
           driver, identify and reset the receiver device. With this function,
           the client also register a number of callback functions. They
           will allow the driver to access the I2C bus.

    \param Unit            Receiver unit number
    \param uHwAddress      Device I2C slave address
    \param sysFuncWrite    System function to write I2C
    \param sysFuncRead     System function to read I2C
    \param callbackFunc    Pointer to interrupt callback function
    \param sysFuncTimer    Pointer to timer function

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is already initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_INIT_FAILED: the unit instance is already
              initialised
            - TMBSL_ERR_BSLHDMIRX_COMPATIBILITY: the driver is not compatiable
              with the internal device version code
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
                inconsistent

******************************************************************************/
tmErrorCode_t tmbslTDA1997XInit
(
    tmUnitSelect_t              unit,
    UInt8                       uHwAddress,
#ifdef TMFL_TDA19972_FAMILY
    UInt8						CECHwAddress,
#endif
    ptmbslHdmiRxSysFunc_t       sysFuncWrite,
    ptmbslHdmiRxSysFunc_t       sysFuncRead,
    ptmbslHdmiRxCallbackFunc_t  callbackFunc,
    ptmbslHdmiRxSysFuncTimer_t  sysFuncTimer
);

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
tmErrorCode_t tmbslTDA1997XDeInit
(
    tmUnitSelect_t Unit
);

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
);
/*============================================================================*/
/**
    \brief Returns the version of the HDMI RX chip identified during
           initialization of the driver

    \param Unit    Receiver unit number
    \param Version Version of the device

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XGetChipInfo
(
    tmUnitSelect_t        unit,
    tmbslHdmiRxVersion_t *pVersion
);

/*============================================================================*/
/**
    \brief Make driver handle an incoming interrupt. This function is used by
           application to tell the driver that the TDA1997X sent an interrupt.
           The driver will then check TDA1997X status and callback the
           application with the various events, if necessary.

           Necessity to protect function from interrupt.

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
);
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
);

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
);

/*============================================================================*/
/**
    \brief Set an interrupt mask. this function allows the application to
           define which interrupt source will actually trigger an interrupt.
           A value of 1 means that the interrupt source is enabled, 0 is
           disabled.

    \param Unit      Receiver unit number
    \param IrqSource Interrupt to be enabled/disabled
    \param MaskValue Interrupt mask value (0 = disabled, 1 = enabled)

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
    tmbslHdmiRxIRQSource_t irqSource,
    UInt8                   maskValue
);



/*============================================================================*/
/**
    \brief Configures the input of the HDMI RX chip. With this function, you can
           (de)activate the color bar generator, choose its format (NTSC/PAL),
           select the source of the sync timing measurement module for mode
           identification and select the physical digital or analog input to be
           processed.

    \param Unit               Receiver unit number
    \param SyncType           Analog or digital sync input       (ignored with TDA1997X)
    \param DigitalSyncMode    Digital sync mode (auto or manual)
    \param DigitalInputSource Digital input source
    \param AnDigSelect        Analog or digital video input      (ignored with TDA1997X)
    \param AnInputSelect      Analog video input source          (ignored with TDA1997X)
    \param vdpResetMode       VDP reset mode (auto/manual)
    \param vdpReset           VDP manual reset

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
tmbslTDA1997XConfigureInput
(
    tmUnitSelect_t                          unit,
    tmbslHdmiRxInputSelSyncType_t           syncSignal,
    tmbslHdmiRxInputSelDigitalMode_t        digitalSyncMode,
    tmbslHdmiRxInputSelDigitalSource_t      digitalInput,
    tmbslHdmiRxInputSelVideoSource_t        anDigSelect,
    tmbslHdmiRxInputSelAnalogVideoSource_t  anInputSelect,
    tmbslHdmiRxInputSelVDPResetMode_t       vdpResetMode,
    tmbslHdmiRxInputSelVDPReset_t           vdpReset
);

/*============================================================================*/
/**
    \brief Configures the sync-On-Green/Y input. This function allows to enable
           or disable the burst filter present on sync slicer input 3.
           This function is only available for TDA19976 IC.

    \param Unit   Receiver unit number
    \param Burst3 Enable or bypass the burst filter

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

*******************************************************************************/
tmErrorCode_t
tmbslTDA1997XConfigureSyncOnGreenInput
(
    tmUnitSelect_t          unit,
    tmbslHdmiRxSogBurst_t  burst3

);

/*============================================================================*/
/**
    \brief Configure the sync detection, recognition and separation of the
           analog input. A call to this functions automaticaly resets the sync
           detection algorithm. You can change a number of parameters of the
           sync and especially disable the automatic mode and force some
           specific parameters (SOG, H/CSYNC, polarity, etc.). Depending on
           the input configuration the sync detection module will process
           digital or analog signal.
           This function is only available for TDA19976 IC.

    \param Unit            Receiver unit number
    \param AdvancedTV      Adanced TV mode
    \param CsSam           Composite sync sampling mode
    \param AutoSyncDetect  Automatic sync detection (on/off)
    \param SyncGreenforced Sync on green forced (on/off)
    \param DigitCompSyncFo Force the use of H/CSYNCx digital composite
           sync
    \param VSyncAutoPol    VSYNC automatic polarity mode (on/off)
    \param VSyncToggle     VSYNC toggle in manual mode
    \param HSyncAutoPol    HSYNC automatic polarity mode (on/off)
    \param HSyncToggle     HSYNC toggle in manual mode

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
tmbslTDA1997XConfigureSyncDetection
(
    tmUnitSelect_t                    unit,
    tmbslHdmiRxSyncDetectAtv_t       advancedTV,
    tmbslHdmiRxSyncDetectComposite_t csSam,
    tmbslHdmiRxSyncDetect_t          autoSyncDetect,
    tmbslHdmiRxForceSog_t            syncGreenForced,
    tmbslHdmiRxSForceDcs_t           digitCompSyncFo,
    tmbslHdmiRxSVsyncPolarityMode_t  vSyncAutoPol,
    tmbslHdmiRxSVsyncToggle_t        vSyncToggle,
    tmbslHdmiRxSHsyncPolarityMode_t  hSyncAutoPol,
    tmbslHdmiRxHsyncToggle_t         hSyncToggle
);

/*============================================================================*/
/**
    \brief Configure the PLL. The PLL is the reference clock when the input
           is anolog. It is used to derivate all subsequent clocks (CLOKOUT,
           CLKFOR, CLKPIX). When the input is digital, this PLL is not used.
           The coast mode is used to protect the automatic phase detector
           against incorrect HSYNC signals. You can select the coast source with
           this function. The coast signal is used to disable the automatic
           phase detector when horizontal sync is absent.
           This function is only available for TDA19976 IC.

    \param Unit           Receiver unit number
    \param MasterDivider  Value of the master divider
    \param PixelDivider   Value of the pixel divider
    \param PLLSync        Rising or falling edge PLL sync
    \param CoastMode      Coast mode (on/off)
    \param CoastSelection Source signal that coasts the PLL
    \param CoastValues    Structure containing values of coast signal to be
                          generated by VHREF.

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
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XConfigurePLL
(
    tmUnitSelect_t                   unit,
    tmbslHdmiRxPllMdiv_t            masterDivider,
    UInt16                           pixelDivider,
    tmbslHdmiRxPllEdgeSync_t        pllSync,
    tmbslHdmiRxPllCoastMode_t       coastMode,
    tmbslHdmiRxPllCoastSelection_t  coastSelection,
    tmbslHdmiRxCoastSignalValues_t  *pCoastValues
);

/*============================================================================*/
/**
    \brief Configure the ADC. The ADC is responsible for analog to digital
           conversion of video signal. You can configure an output clock delay
           and change the ADC output current. Note that it is strongly
           recommanded to only use 100% current output.
           This function is only available for TDA19976 IC.

    \param Unit        Receiver unit number
    \param ADCPolarity Polarity of the ADC
    \param ClockDelay  Value of the clock output delay
    \param BPBCurrent  Value of the B/Pb current
    \param GYCurrent   Value of the GY current
    \param PRPCurrent  Value of the P/Rp current

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
tmbslTDA1997XConfigureADC
(
    tmUnitSelect_t              unit,
    tmbslHdmiRxADCPolarity_t   adcPolarity,
    tmbslHdmiRxADCClkDelay_t   clockDelay,
    tmbslHdmiRxADCCurrentPBp_t bpbCurrent,
    tmbslHdmiRxADCCurrentGy_t  gyCurrent,
    tmbslHdmiRxADCCurrentPRp_t prpCurrent
);

/*============================================================================*/
/**
    \brief Configure the DLL. The DLL is used to shift the clock. This is
           typically used to ensure that sampling edge will be correctly
           phased against data change edges.
           This function is only available for TDA19976 IC.

    \param Unit           Receiver unit number
    \param Phase          Phase shift for the clocks CLKPIX, CLKFOR, CLKOUT
    \param ClockOutPreset Phase shift for CLKOUT
    \param ClockPixPreset Phase shift for CLKPIX
    \param ClockOutDiv    Division factor for CLKOUT
    \param ClockPixDiv    Division factor for CLKPIX
    \param PresetDelay    Preset delay ON/OFF
    \param PhaseEdge      Phase edge configuration (raising/falling)

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
tmbslTDA1997XConfigureDLL
(
    tmUnitSelect_t              unit,
    UInt8                       phase,
    UInt8                       clockOutPreset,
    UInt8                       clockPixPreset,
    tmbslHdmiRxDllDivClkOut_t  clockOutDiv,
    tmbslHdmiRxDllDivClkPix_t  clockPixDiv,
    tmbslHdmiRxDllDivPrDelay_t presetDelay,
    tmbslHdmiRxDllDivPhEdge_t  phaseEdge
);

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
);

#ifdef TMFL_TDA19972_FAMILY

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
);
#endif

/*============================================================================*/
/**
    \brief Configure the clamping level. The clamping level is used by the ADCs
           to define the black level of the image. The ADCs have two main
           operating modes : manual clamping, when the value provided by this
           function are used as is, and the automatic adjustment, when ADCs
           evaluate the clamping level during the horizontal sync. In case of
           automatic adjustment, the zone where the ADCs are evaluating the
           clamping level is given by the clamp signal (coming either from VHREF
           or from an external source). The ClampValues parameter defines the
           clamp signal that VHREF module will output to ADCs.
           This function is only available for TDA19976 IC.

    \param Unit             Receiver unit number
    \param GYLevel          Clamp level of the G/Y channel
    \param BULevel          Clamp level of the B/Pb channel
    \param RVLevel          Clamp level of the R/Pr channel
    \param ClampUpdate      Update rate of the digital clamp adjustement
    \param DigitalAdjust    Digital adjustment mode (ON/OFF)
    \param PulseSelect      Clamp pulse source selection
    \param ClampValues      Structure containing values of  clamp signal to be
                            generated by VHREF.

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
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XConfigureClamping
(
    tmUnitSelect_t                       unit,
    UInt16                               gyLevel,
    UInt16                               buLevel,
    UInt16                               rvLevel,
    tmbslHdmiRxClampUpdateRate_t        clampUpdate,
    tmbslHdmiRxClampDigitalAdjustment_t digitalAdjust,
    tmbslHdmiRxClampPulseSelection_t    pulseSelect,
    tmbslHdmiRxClampSignalValues_t      *pClampValues
);

/*============================================================================*/
/**
    \brief Configure Video gain R/Pr or B/Pb or G/Y. You can, with this
           function, control all the parameters of the gain applied by ADCs. You
           can choose, for each channel, the gain, the minimum and maximum
           allowed value. If theses min/max value are crossed by pixels, the
           out-of-range signal is activated for the time of the concerned
           pixels. You can also select the source of the gain signal (VHREF or
           extern) in case of automatic adjustment.
           This function is only available for TDA19976 IC.

    \param Unit               Receiver unit number
    \param GainRv             Gain definition for channel R/Pr
    \param GainBu             Gain definition for channel B/Pb
    \param GainGy             Gain definition for channel G/Y
    \param GainPulseSelection Gain calibration signal selection
    \param GainStart          Start position of the gain signal to be generated
                              by VHREF
    \param GainEnd            End position of the gain signal to be generated
                              by VHREF

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
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XConfigureVideoGain
(
    tmUnitSelect_t                    unit,
    tmbslHdmiRxGainValue_t           *pGainRv,
    tmbslHdmiRxGainValue_t           *pGainBu,
    tmbslHdmiRxGainValue_t           *pGainGy,
    tmbslHdmiRxGainPulseSelection_t  gainPulseSelection,
    UInt16                            gainStart,
    UInt16                            gainEnd
);

/*============================================================================*/
/**
    \brief Get the number of pixels outside the range defined by the low and
           high parameters of the gain. The Value is given in log2 base. This
           function is typically used together with "ConfigureVideoGain" to
           optimize the gain value for maximum contrast.
           This function is only available for TDA19976 IC.

    \param Unit         Receiver unit number
    \param OutOfRangeRv Number of out of range pixel in R/Pr
    \param OutOfRangeBu Number of out of range pixel in B/Pb
    \param OutOfRangeGy Number of out of range pixel in GY

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading from the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED: this function is not supported on the
              current configured IC.
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XGetOutOfRangePixels
(
    tmUnitSelect_t  unit,
    UInt8           *pOutOfRangeRv,
    UInt8           *pOutOfRangeBu,
    UInt8           *pOutOfRangeGy
);

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
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XGetSyncTimings
(
    tmUnitSelect_t  unit,
    UInt32         *pVerticalPeriod,
    UInt16         *pHorizontalPeriod,
    UInt16         *pHorizontalSyncWidth,
    tmbslHdmiRxFormatMeas_t   *pFormatMeasurements
);

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
    tmUnitSelect_t                        unit,
    tmbslHdmiRxColorSpaceBypass_t        bypass,
    tmbslHdmiRxColorSpaceCoefficients_t  *pCoefficients
);

/*============================================================================*/
/**
    \brief Set the pixel and line counters

    \param Unit             Receiver unit number
    \param PixelCountPreset Preset number of the pixel counter
    \param PixelCountNumber Maximum value of line counter before roll-over
    \param LineCountPreset  Preset number of the pixel counter
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
);

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
    tmbslHdmiRxVhrefCSyncPol_t           cSyncPol
);

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
    tmUnitSelect_t              unit,
    tmbslHdmiRxVHRefValues_t    *pVHRefValues
);

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
    tmUnitSelect_t                unit,
    tmbslHdmiRxVHSyncValues_t  *pVHSyncValues
);

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
);

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
);

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
);

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
);

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
    tmUnitSelect_t                unit,
    tmbslHdmiRxPreFilterConfig_t filterConfigBu,
    tmbslHdmiRxPreFilterConfig_t filterConfigRv
);

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
);

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
    tmbslHdmiRxOutputRef_t           refOutput,
    tmbslHdmiRxOutputBlankingCodes_t blankingCode,
    tmbslHdmiRxOutputTimingRefs_t    timingRefs,
    tmbslHdmiRxOutputFormat_t        outputFormat
);

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
    tmUnitSelect_t            unit,
    tmbslHdmiRxDownSampler_t downConfig,
    tmbslHdmiRxUpSampler_t   upConfig
);

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
    tmUnitSelect_t              unit,
    tmbslHdmiRxInternalVSync_t vSync,
    tmbslHdmiRxInternalHSync_t hSync
);

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
    tmUnitSelect_t                      unit,
    tmbslHdmiRxSyncOutputCSSelection_t csSelection,
    tmbslHdmiRxSyncOutputVSSelection_t vsSelection,
    tmbslHdmiRxSyncOutputHSSelection_t hsSelection
);

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
    tmbslHdmiRxPolarityCSyncOut_t       cSyncPolarity,
    tmbslHdmiRxPolarityHSyncOut_t       hSyncPolarity,
    tmbslHdmiRxPolarityVSyncOut_t       vSyncPolarity,
    tmbslHdmiRxPolarityFieldrefOut_t    fieldRefPolarity,
    tmbslHdmiRxPolarityHRefOut_t        hRefPolarity,
    tmbslHdmiRxPolarityVRefOut_t        vRefPolarity,
    tmbslHdmiRxAutomaticSyncPolarity_t  hsVsSelection
);

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
    tmUnitSelect_t                   unit,
    tmbslHdmiRxVideoPortFormat_t    videoPortFormat,
    tmbslHdmiRxVideoPortSelection_t videoPortASelection,
    tmbslHdmiRxVideoPortSelection_t videoPortBSelection,
    tmbslHdmiRxVideoPortSelection_t videoPortCSelection
);

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
    tmUnitSelect_t                   unit,
    tmbslHdmiRxDataEnHRefExpand_t   dataEnHrefExpand,
    tmbslHdmiRxDataEnHRefForceLow_t dataEnHRefForce,
    tmbslHdmiRxDataEnExpand_t       dataEnExpand,
    tmbslHdmiRxDataEnSelection_t    dataEnSelection,
    UInt8                            dataEnDelay
);

/*============================================================================*/
/**
    \brief Get the result of the termination resistance calibration.
           This function is used together with "ConfigureTerminationResistance"
           to get the result of the calibration in automatic mode.
           If this function is called on the TDA1997X, it does nothing and
           returns the following error code : TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED.

    \param Unit           Receiver unit number
    \param TRACalibration Gives the code corresponding to 50 ohms calculated
                          with resitance A.
    \param TRBCalibration Gives the code corresponding to 50 ohms calculated
                          with resitance B.

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus
            - TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED: function not supported on the
              active chip.
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XGetTerminationResistanceCalibration
(
    tmUnitSelect_t  unit,
    UInt8          *pTRACalibration,
    UInt8          *pTRBCalibration
);

/*============================================================================*/
/**
    \brief Configure the termination resistances. You can choose to manually
           set the calibration values of the termination restistances or to
           activate the automatic mode. If you are using the automatic mode,
           the function "GetTerminationResistanceCalibration" will give you
           the result of the calibration.
           If this function is called on a TDA1997X, it does nothing and
           returns the following error code : TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED.

    \param Unit               Receiver unit number
    \param TRAControl         Calibration mode of resistance A
    \param TRAResistanceValue Calibration value of resistance A in manual mode
    \param TRBControl         Calibration mode of resistance B
    \param TRBResistanceValue Calibration value of resistance B in manual mode

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
tmbslTDA1997XConfigureTerminationResistance
(
    tmUnitSelect_t                  unit,
    tmbslHdmiRxTRCalibrationMode_t trAControlMode,
    UInt8                           trAResistanceValue,
    tmbslHdmiRxTRCalibrationMode_t trBControlMode,
    UInt8                           trBResistanceValue
);

/*============================================================================*/
/**
    \brief AVI software reset. Allows you to reset the analog video input
           modules. An automatic mode is also available to automatically reset
           the module when no activity is detected.
           This function is not available for TDA1997X ICs.

    \param Unit                Receiver unit number
    \param ResetMode Automatic reset mode (ON/OFF)
    \param ResetAVI  Reset AVI now (YES/NO)

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
tmbslTDA1997XAVISoftReset
(
    tmUnitSelect_t          unit,
    tmbslHdmiRxResetMode_t resetMode,
    tmbslHdmiRxResetAVI_t  resetAVI
);

/*============================================================================*/
/**
    \brief Configure PLL output. With this function you can choose which signal
           will be routed to the PL output pin. This is mostly for debugging
           purpose.
           This function is not available for TDA1997X ICs.

    \param Unit          Receiver unit number
    \param LockThreshold Sensivity of the lock detector
    \param PLPolarity    Polarity of the PLL lock signal
    \param PLSelection   PLL output signal selection

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
tmbslTDA1997XConfigurePLLOutput
(
    tmUnitSelect_t                unit,
    UInt8                         lockThreshold,
    tmbslHdmiRxPllOutputLock_t   plPolarity,
    tmbslHdmiRxPllOutputSignal_t plSelection
);

/*============================================================================*/
/**
    \brief Configure out-of-range output signal. With this function, you can
           choose which signal will actually be output by the out-of-range
           pin. This is mostly for debugging purpose.
           This function is only available for TDA19976 ICs.

    \param Unit          Receiver unit number
    \param OutOfrangeRCR
    \param OutOfrangeGY
    \param OutOfrangeBCB

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
tmbslTDA1997XConfigureOutOfRangeOutput
(
    tmUnitSelect_t              unit,
    tmbslHdmiRxOutOfRangeRCR_t outOfrangeRCR,
    tmbslHdmiRxOutOfRangeGY_t  outOfrangeGY,
    tmbslHdmiRxOutOfRangeBCB_t outOfrangeBCB
);

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
    tmUnitSelect_t                  unit,
    tmbslHdmiRxHdmiMute_t          hdmiMute,
    tmbslHdmiRxHdcpMode_t          hdcpMode,
    tmbslHdmiRxHdmiProtocol_t      hdmiProtocol,
    tmbslHdmiRxHdmiVsyncPolarity_t vsyncPolarityMode,
    tmbslHdmiRxHdmiVsyncToggle_t   vsyncToggle
);

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
tmbslTDA1997XConfigureHDMIClock
(
    tmUnitSelect_t          unit,
    tmbslHdmiRxHdmiClock_t hdmiClock
);

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
    tmbslHdmiRxAudioPathForcedLayout_t forcedlayout,
    UInt8                              fifoLatency,
    tmbslHdmiRxAudioTestTone_t         testTone,
    tmbslHdmiRxAudioOutputFormat_t     outputFormat,
    tmbslHdmiRxAudioOutputForce_t      forceWordSelect,
    tmbslHdmiRxAudioOutputForce_t      forceAudioPort7,
    tmbslHdmiRxAudioOutputForce_t      forceAudioPort6,
    tmbslHdmiRxAudioOutputForce_t      forceAudioPort5,
    tmbslHdmiRxAudioOutputForce_t      forceAudioPort4,
    tmbslHdmiRxAudioOutputForce_t      forceAudioPort3,
    tmbslHdmiRxAudioOutputForce_t      forceAudioPort2,
    tmbslHdmiRxAudioOutputForce_t      forceAudioPort1,
    tmbslHdmiRxAudioOutputForce_t      forceAudioPort0,
    tmbslHdmiRxAudioOutputForce_t      forceAudioClock
);

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
);

/*============================================================================*/
/**
    \brief Configure audio clock. All audio clock parameters (divider, sync,
           mode) are modified through this function.
           If this function is called on the TDA1997X, it does nothing and
           returns the following error code : TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED.

    \param Unit                 Receiver unit number
    \param ManualCycleTimeStamp Cycle time stamp in manual mode
    \param ManualNDivider       N Divider in manual mode
    \param ClockPowerMode       Clock power mode
    \param ClockDividerUpdate   Clock divider synchronisation type
    \param ClockDividerMode     Clock divider mode
    \param ClockFrequency       Audio clock reference frequency
    \param SampleFrequency      Sample clock frequency

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
tmbslTDA1997XConfigureAudioClock
(
    tmUnitSelect_t                        unit,
    UInt32                                manualCycleTimeStamp,
    UInt32                                manualNDivider,
    tmbslHdmiRxAudioClockPowerMode_t     clockPowerMode,
    tmbslHdmiRxAudioClockDividerUpdate_t clockDividerUpdate,
    tmbslHdmiRxAudioClockDividerMode_t   clockDividerMode,
    tmbslHdmiRxAudioClockFrequency_t     clockFrequency,
    tmbslHdmiRxAudioSampleFrequency_t    sampleFrequency
);

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
    tmUnitSelect_t                           unit,
    tmbslHdmiRxAudioPacketMode_t            packetMode,
    tmbslHdmiRxAudioPLLInputRef_t           pllInputRef,
    tmbslHdmiRxAudioI2SResolution_t         i2SResolution,
    tmbslHdmiRxAudioPllSelection_t          pllInputSelection,
    tmbslHdmiRxHBROutputMode_t              outputModeHBR
);

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
);

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
            - TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED: this function is not supported
              on the current configured IC.

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XAudioAutoMute
(
    tmUnitSelect_t              unit,
    tmbslHdmiRxAudioAutoMute_t  autoMute
);

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
);

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
    UInt32           *pStatus
);

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
    UInt32            *pAcrpCts,
    UInt32            *pAcrpN
);

/*============================================================================*/
/**
    \brief Get audio sample info. This function provides a set of information
           about the audio subpackets received (layout, presence, first frame,
           flatline).
           If this function is called on the TDA1997X, it does nothing and
           returns the following error code : TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED.

    \param Unit                          Receiver unit number
    \param tmbslHdmiRxAudioSampleInfo_t  Pointer to the structure that will
                                         receive the audio sample packet info

    \return The call result:
            - TM_OK: the call was successful
            - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMBSL_ERR_BSLHDMIRX_BAD_PARAMETER: a parameter is invalid or out
              of range
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus
            - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent
            - TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED: function not supported on the
              active chip.

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XGetAudioSampleInfo
(
    tmUnitSelect_t                    unit,
    tmbslHdmiRxAudioSampleInfo_t      *pAudioSamplePacketInfo
);

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
    UInt16            *pErrorPerframe,
    UInt16            *pFramesBetweenErrors
);

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
    UInt8            *pDelta
);

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
    UInt8            *pAcpUpdate
);

/*============================================================================*/
/**
    \brief Configure OTP memory. This function allows configuring and sending
           commands to OTP memory.

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
tmbslTDA1997XConfigureOTP
(
    tmUnitSelect_t           unit,
    tmbslHdmiRxOtpCommand_t otpCommand,
    tmbslHdmiRxBchEnable_t  bchEnable
);

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
tmbslTDA1997XSetOTPAddress
(
    tmUnitSelect_t  unit,
    UInt8           address
);

/*============================================================================*/
/**
    \brief Read OTP memory. This function allows to read the OTP memory. It is
           used together with "ConfigureOTP" function. The end of the read
           process is indicated by the OTP status byte accessible through
           the interrupt callback function after a call to
           tmbslTDA1997XHandleInterrupt function.

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
tmbslTDA1997XReadOTP
(
    tmUnitSelect_t             unit,
    UInt32                     *pOtpData
);

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
    tmbslHdmiRxHdcpDecryptKey_t decryptKeys,
    tmbslHdmiRxHdcpEnable_t     hdcpEnable,
    UInt8                        ddcI2cAddress,
    UInt16                       keyDescriptionSeed
);

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
);

/*============================================================================*/
/**
    \brief Set HDCP Repeater status. This function allows the caller to define
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
    tmUnitSelect_t                        unit,
    tmbslHdmiRxHDCPRepStatusMaxDev_t      maxDevStatus,
    UInt8                                 attachedDevices,
    tmbslHdmiRxHDCPRepStatusMaxCascade_t  maxCascadeStatus,
    UInt8                                 depth
);

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
);

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
);

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
);

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
);

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
);

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
);

/*============================================================================*/
/**
    \brief Configure HDCP error protection. This function is used to set the
           parameters that affects HDCP error protection mechanisms :
           Error detection and error filtering.

    \param Unit                 Receiver unit number
    \param delockDelay            Delay before delocking the word locker
    \param DEMeasureMode        DE measurement mode
    \param DERegenerationMode    Enable/disable DE regeneration
    \param DEFilterSensivity    DE filter sensivity
    \param DECompMode            DE composition mode
    \param CTLFilterSensivity    CTL filter sensivity
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
    tmUnitSelect_t                   unit,
    UInt8                            delockDelay,
    tmbslHdmiRxDEMeasurementMode_t  DEMeasureMode,
    tmbslHdmiRxDERegeneration_t     DERegenerationMode,
    tmbslHdmiRxDEFilterSensivity_t  DEFilterSensivity,
    tmbslHdmiRxDECompositionMode_t  DECompMode,
    tmbslHdmiRxCTLFilterSensivity_t CTLFilterSensivity,
    tmbslHdmiRxVSFilterSensivity_t  VSFilterSensivity,
    tmbslHdmiRxHSFilterSensivity_t  HSFilterSensivity
);

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
    UInt8           *pEnhancedLinkVerification,
    UInt8           *pCipherMode
);

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
    tmbslHdmiRxInfoFrame_t  infoFrame,
    UInt8                    *pReadType,
    UInt8                    *pVersion,
    UInt8                    *pLength,
    UInt8                    *pCheckSum,
    UInt8                    *pData
);

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
);

/*============================================================================*/
/**
    \brief Get ISRC1 packet. This function is used to read an ISRC1 packet
           after beeing notified that an ISRC1 packet as been received by HW.

    \param Unit       Receiver unit number
    \param PacketType ISRC packet type
    \param Control    ISRC control calue
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
);

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
);

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
);

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

);

/******************************************************************************
    \brief Get the capabilities of unit 0. Capabilities are stored into a
           dedicated structure and are directly read from the HW device.


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
);

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
tmbslTDA1997XConfigureAudioClkMode
(
    tmUnitSelect_t                          unit,
    tmbslHdmiRxAudioClocksModePowerMode_t   clocksPowerMode,
    tmbslHdmiRxAudioClocksModeRefMode_t     clocksRefMode,
    tmbslHdmiRxAudioClocksModeRefFreq_t     clocksRefFreq,
    tmbslHdmiRxAudioClocksAudioMode_t       clocksAudioMode,
    tmbslHdmiRxAudioClocksSysMode_t         clocksSysMode
);

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
);

/*============================================================================*/
/**
    \brief        Writing on I2C bus

    \param        unit          Unit number
    \param        firstRegister First register to write
    \param        lenghtData    Length
    \param        * pBuffer     Pointer of data to be written

    \return     - TM_OK  : the call was successful
                - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
                  the receiver instance is already initialised
                - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing the I2C bus
                - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
                  inconsistent

 ******************************************************************************/
tmErrorCode_t tmbslTDA1997XWriteI2C(
    tmUnitSelect_t          Unit,
    UInt16                  firstRegister,
    UInt8                   lengthData,
    UInt8                   * pBuffer
);

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
tmErrorCode_t tmbslTDA1997XReadI2C(
    tmUnitSelect_t          Unit,
    UInt16                  firstRegister,
    UInt8                   lengthData,
    UInt8                   * pBuffer
);

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
    UInt32                             *pFreq
);

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
);

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
    tmbslHdmiRxEDIDReset_t        resetEdid,
    tmbslHdmiRxEDIDPowerDown_t    powerDownEdid,
    tmbslHdmiRxEDIDPowerDownDDC_t powerDownEdidDDC
);

/*============================================================================*/
/**
    \brief Load EDID data into the internal EDID memory. Note that on TDA1997X
           EDID is common to all HDMI inputs except source physical address
           and checksum.

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
);

/*============================================================================*/
/**
    \brief Get current deep color mode

    \param unit               Receiver unit number
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
);

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
            - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing to the I2C
              bus
            - TMBSL_ERR_BSLHDMIRX_I2C_READ: failed when reading the I2C bus
            - TMBSL_ERR_BSLHDMIRX_NOT_SUPPORTED: this function is not supported on the
              current configured IC.

******************************************************************************/
tmErrorCode_t tmbslTDA1997XSetPowerState
(
    tmUnitSelect_t unit,
    tmPowerState_t powerState
);

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
);

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
tmErrorCode_t tmbslTDA1997XGetChannelStatus
(
    tmUnitSelect_t  unit,
    UInt8           *pChanStatus
);


#ifdef TMFL_TDA19972_FAMILY
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
);

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
tmErrorCode_t
tmbslTDA1997XLoadConfigData
(
    tmUnitSelect_t unit,
    UInt8          *DDC_Config,
    UInt8          *RT_Config
);

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
);

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
tmErrorCode_t tmbslTDA1997XReadEdidData
(
    tmUnitSelect_t unit,
    UInt8         *commonEDID,
    UInt16        *sPA,
    UInt8         *checksum,
    UInt8         *sPAOffset
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
tmErrorCode_t tmbslTDA1997XReadSecondaryEdidData
(
    tmUnitSelect_t unit,
    UInt8         *commonEDID,
    UInt16        *sPA,
    UInt8         *checksum,
    UInt8         *sPAOffset
);

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
tmErrorCode_t tmbslTDA1997XAttachEdidToInput
(
    tmUnitSelect_t unit,
    tmbslHdmiRxInputSelDigitalSource_t  inputSel
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
tmErrorCode_t tmbslTDA1997XAttachSecondaryEdidToInput
(
    tmUnitSelect_t unit,
    tmbslHdmiRxInputSelDigitalSource_t  inputSel
);

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
);

tmErrorCode_t tmbslTDA1997XManualHPD
(
    tmUnitSelect_t unit,
    tmbslHdmiHPDManual_t  ManualHPD
);
#endif

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
tmErrorCode_t tmbslTDA1997XSetHdmiOutMode
(
    tmUnitSelect_t unit,
    tmbslHdmiRxHdmiOutMode_t outmode
);


/**
    \brief      Get detailed timing descriptor from previously read EDID

    \param[in]  txUnit      Transmitter unit number
    \param[out] pEdidDTD    Pointer to the array to receive the Detailed timing descriptor

    \param[in]  nb_size     Number of DTD supported in buffer pEdidDTD

    \param[out] pDTDAvail Pointer to receive the number of DTD available

    \return     The call result:
                - TM_OK: the call was successful
                - Else a problem has been detected:
                  - TMBSL_ERR_HDMI_BAD_PARAMETER: a parameter was out of range
                  - TMBSL_ERR_HDMI_BAD_UNIT_NUMBER: bad transmitter unit number
                  - TMBSL_ERR_HDMI_I2C_READ: failed when reading the I2C bus
                  - TMBSL_ERR_HDMI_RESOURCE_NOT_AVAILABLE : EDID not read
                  - TMBSL_ERR_HDMI_NOT_INITIALIZED: transmitter not initialized
*/
tmErrorCode_t
tmbslHdmiRxHdmioutEdidGetDetailedTimingDescriptors
(
    tmUnitSelect_t          txUnit,
    tmbslHdmiRxEdidDtd_t    *pEdidDTD,
    UInt8                   nb_size,
    UInt8                   *pDTDAvail
);


/**
    \brief      Get supported video format(s) from previously read EDID

    \param[in]  txUnit      Transmitter unit number
    \param[out] pEdidVFmts  Pointer to the array to receive the supported Short
                            Video Descriptors
    \param[in]  vFmtLength  Number of SVDs supported in buffer pEdidVFmts,
                            up to HDMI_TX_SVD_MAX_CNT
    \param[out] pVFmtsAvail Pointer to receive the number of SVDs available
    \param[out] pVidFlags   Ptr to the byte to receive Video Capability Flags
                            b7: underscan supported
                            b6: YCbCr 4:4:4 supported
                            b5: YCbCr 4:2:2 supported

    \return     The call result:
                - TM_OK: the call was successful
                - Else a problem has been detected:
                  - TMBSL_ERR_HDMI_BAD_PARAMETER: a parameter was out of range
                  - TMBSL_ERR_HDMI_BAD_UNIT_NUMBER: bad transmitter unit number
                  - TMBSL_ERR_HDMI_I2C_READ: failed when reading the I2C bus
                  - TMBSL_ERR_HDMI_RESOURCE_NOT_AVAILABLE : EDID not read
                  - TMBSL_ERR_HDMI_NOT_INITIALIZED: transmitter not initialized
                  - TMBSL_ERR_HDMI_NULL_CONNECTION: HPD pin is inactive

    \note \verbatim
                Supported Short Video Descriptors array:
                    (htX_SVD_NATIVE_MASK bit set to indicate native format)
                EdidVFmts[0]   EIA/CEA Short Video Descriptor 1, or 0
                ...
                EdidVFmts[n-1]  EIA/CEA Short Video Descriptor 32, or 0
                (Where n is the smaller of vFmtLength and pVFmtAvail)
    \endverbatim
    \sa         tmbslTDA9989EdidGetBlockData
 */
tmErrorCode_t
tmbslHdmiRxHdmioutEdidGetVideoCapabilities
(
    tmUnitSelect_t  txUnit,
    UInt8           *pEdidVFmts,
    UInt            vFmtLength,
    UInt            *pVFmtsAvail,
    UInt8           *pVidFlags
);

/*============================================================================*/
/* tmbslHdmiRxHdmioutEdidGetStatus                                                  */
/*============================================================================*/
tmErrorCode_t
tmbslHdmiRxHdmioutEdidGetStatus
(
    tmUnitSelect_t  Unit,
    UInt8           *puEdidStatus
);

/*============================================================================*/
/* tmbslHdmiRxHdmioutEdidGetBlockCount                                               */
/*============================================================================*/
tmErrorCode_t
tmbslHdmiRxHdmioutEdidGetBlockCount
(
    tmUnitSelect_t      Unit,
    UInt8               *puEdidBlockCount
);

/*============================================================================*/
/* tmbslHdmiRxHdmioutEdidGetAudioCapabilities                                        */
/*============================================================================*/
tmErrorCode_t
tmbslHdmiRxHdmioutEdidGetAudioCapabilities
(
    tmUnitSelect_t          Unit,
    tmbslHdmiRxEdidSad_t    *pEdidAFmts,
    UInt                    aFmtLength,
    UInt                    *pAFmtsAvail,
    UInt8                   *pAudioFlags
);

/*============================================================================*/
/* tmbslHdmiRxHdmioutEdidGetVideoPreferred                                           */
/*============================================================================*/

tmErrorCode_t
tmbslHdmiRxHdmioutEdidGetVideoPreferred
(
    tmUnitSelect_t         Unit,
    tmbslHdmiRxEdidDtd_t *pEdidDTD
);

/*============================================================================*/
/* ClearEdidRequest                                                           */
/*============================================================================*/

tmErrorCode_t
tmbslHdmiRxHdmioutClearEdidRequest (tmUnitSelect_t Unit);

/*============================================================================*/
/* tmbslHdmiRxHdmioutEdidGetSinkType                                                 */
/*============================================================================*/
tmErrorCode_t
tmbslHdmiRxHdmioutEdidGetSinkType
(
    tmUnitSelect_t              Unit,
    tmbslHdmiRxSinkType_t      *pSinkType
);

/*============================================================================*/
/* tmbslHdmiRxHdmioutEdidGetSourceAddress                                            */
/*============================================================================*/

tmErrorCode_t
tmbslHdmiRxHdmioutEdidGetSourceAddress
(
    tmUnitSelect_t  Unit,
    UInt16          *pSourceAddress
);

/*============================================================================*/
/* tmbslHdmiRxHdmioutEdidGetMonitorDescriptors                                      */
/*============================================================================*/
tmErrorCode_t
tmbslHdmiRxHdmioutEdidGetMonitorDescriptors
(
    tmUnitSelect_t              Unit,
    tmbslHdmiRxEdidFirstMD_t    *pEdidFirstMD,
    tmbslHdmiRxEdidSecondMD_t   *pEdidSecondMD,
    tmbslHdmiRxEdidOtherMD_t    *pEdidOtherMD,
    UInt8                       sizeOtherMD,
    UInt8                       *pOtherMDAvail
);

/*============================================================================*/
/* tmbslHdmiRxHdmioutEdidGetBasicDisplayParam                                       */
/*============================================================================*/
tmErrorCode_t
tmbslHdmiRxHdmioutEdidGetBasicDisplayParam
(
    tmUnitSelect_t              Unit,
    tmbslHdmiRxEdidBDParam_t    *pEdidBDParam
);

/*============================================================================*/
/* tmbslHdmiRxHdmioutEdidGetLatencyInfo                                             */
/*============================================================================*/
tmErrorCode_t
tmbslHdmiRxHdmioutEdidGetLatencyInfo
(
    tmUnitSelect_t  Unit,
    tmbslHdmiRxEdidLatency_t * pEdidLatency
);

/*============================================================================*/
/* tmbslHdmiRxHdmioutEdidGetExtraVsdbData                                             */
/*============================================================================*/
tmErrorCode_t tmbslHdmiRxHdmioutEdidGetExtraVsdbData
(
    tmUnitSelect_t                  Unit,
    tmbslHdmiRxEdidExtraVsdbData_t  **pExtraVsdbData
);

/*============================================================================*/
/* tmbslHdmiRxHdmioutHotPlugGetStatus                                                */
/*============================================================================*/
tmErrorCode_t
tmbslHdmiRxHdmioutHotPlugGetStatus
(
    tmUnitSelect_t        Unit,
    tmbslHdmiRxHotPlug_t *pHotPlugStatus,
    Bool                  client /* Used to determine whether the request comes from the application */
);

/*============================================================================*/
/* tmbslHdmiRxHdmioutRxSenseGetStatus                                                */
/*============================================================================*/
tmErrorCode_t
tmbslHdmiRxHdmioutRxSenseGetStatus
(
    tmUnitSelect_t        Unit,
    tmbslHdmiRxRxSense_t *pRxSenseStatus,
    Bool                  client /* Used to determine whether the request comes from the application */
);

/*============================================================================*/
/* tmbslTDA9989RxSenseGetStatus                                                */
/*============================================================================*/
tmErrorCode_t
tmbslHdmiRxHdmioutHandleRXSHPD
(
    tmUnitSelect_t        Unit,
    UInt8				  interrupt_rxshpd,
    UInt8				  rxshpd_level,
    UInt8				  *interrupts_raised
);

/*============================================================================*/
/* tmbslHdmiRxHdmioutEdidRequestBlockData                                           */
/*============================================================================*/
tmErrorCode_t
tmbslHdmiRxHdmioutEdidRequestBlockData
(
    tmUnitSelect_t  Unit,
    UInt8           *pRawEdid,
    Int             numBlocks,  /* Only relevant if pRawEdid valid */
    Int             lenRawEdid  /* Only relevant if pRawEdid valid */
);

/*============================================================================*/
/* tmbslHdmiRxHdmioutEdidRequestBlockData                                           */
/*============================================================================*/
tmErrorCode_t
tmbslHdmiRxHdmioutEdidBlockAvailable
(
    tmUnitSelect_t Unit,
    Bool * pSendEDIDCallback
);

#endif


#ifdef __cplusplus
}
#endif

#endif /* tmbslTDA1997X_FUNCTIONS_H */

/*============================================================================*/
/*                            END OF FILE                                     */
/*============================================================================*/

