#include "GlobalOptions.h"




/**
 * Copyright (C) 2006 NXP N.V., All Rights Reserved.
 * Exposed under Apache 2.0 Open Source License with permission of NXP N.V.
 *
 * \file          tmdlHdmiRx_cfg.h
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

   $History: tmbslTDA997X_cfg.h $
 *
 * *****************  Version 1  *****************
 * User: Demoment     Date: 05/03/07   Time: 10:32
 * initial version
 *

   \endverbatim
 *
*/
/*****************************************************************************/
/*****************************************************************************/
/*                THIS FILE MUST NOT BE MODIFIED BY CUSTOMER                 */
/*****************************************************************************/
/*****************************************************************************/

#ifndef TMDLHDMIRX_CFG_H
#define TMDLHDMIRX_CFG_H

#include "tmNxTypes.h"
#include "tmbslHdmiRx_types.h"
#include "tmdlHdmiRx_Types.h"
#include "tmbslHdmiRx_functions.h"
#ifdef __cplusplus
extern "C" {
#endif



/* Number of HW units supported by SW driver */
#define MAX_UNITS 1


/* Size of seed table  */
#define RX_SEED_TABLE_LEN 10

/*============================================================================*/
/*                          TYPES DECLARATIONS                                */
/*============================================================================*/
typedef struct _tmdlHdmiRxCfgSeed_t {
    UInt16     lookUpVal;
    UInt16     seedVal;
} tmdlHdmiRxCfgSeed_t, *ptmdlHdmiRxCfgSeed_t;


typedef struct _tmdlHdmiRxCfgResolution_t {
    tmdlHdmiRxResolutionID_t        resolutionID;
    tmdlHdmiRxResolutionSpecs_t     resolutionSpecs;
} tmdlHdmiRxCfgResolution_t, *ptmdlHdmiRxCfgResolution_t;

typedef struct _tmdlHdmiRxCfgResolutionDetect_t
{
    tmdlHdmiRxResolutionID_t    resolutionID;
    UInt32                      verticalPeriodMin; /* = MCLK(27MHz) / VFreq minus 0.7% */
    UInt32                      verticalPeriodMax; /* same + 0.7% */
    UInt16                      horizontalPeriodMin; /* = MCLK(27MHz) / HFreq minus 1% */
    UInt16                      horizontalPeriodMax;
    UInt16                      hsWidthMin; /* = MCLK(27MHz) / pixclk * hWidth minux ...% */
    UInt16                      hsWidthMax;
} tmdlHdmiRxCfgResolutionDetect_t,*ptmdlHdmiRxCfgResolutionDetect_t;

typedef struct _tmdlHdmiRxCfgResolutionTimings_t {
    tmdlHdmiRxResolutionID_t        resolutionID;
    UInt16                          pixCountPreset;
    UInt16                          pixCountNb;
    UInt16                          lineCountPreset;
    UInt16                          lineCountNb;
    tmbslHdmiRxVHRefValues_t        VHRefValues;
} tmdlHdmiRxCfgResolutionTimings_t, * ptmdlHdmiRxCfgResolutionTimings_t;


typedef struct
{
    UInt8                               commandTaskPriority;
    UInt8                               commandTaskStackSize;
    UInt8                               commandTaskQueueSize;
    UInt8                               i2cAddress;
#ifdef TMFL_TDA19972_FAMILY
    UInt8                               CECi2cAddress;
#endif
    ptmbslHdmiRxSysFunc_t               i2cReadFunction;
    ptmbslHdmiRxSysFunc_t               i2cWriteFunction;
    UInt8                               resolutionSupportedNb;
    ptmdlHdmiRxCfgResolution_t          pResolutionInfo;
    UInt8                               resolutionToBeDetectedNb;
    ptmdlHdmiRxCfgResolutionDetect_t    pResolutionToBeDetectedList;
    tmdlHdmiRxColorMatrixCoefs_t        *pPredefinedColorMatrixList;
    UInt8                               predefinedColorMatrixNb;
    ptmbslHdmiRxFunctionsList_t         ptmbslHdmiRxFunctionsList;
    ptmdlHdmiRxCfgResolutionTimings_t   ptmdlHdmiRxCfgResolutionTimings;
    tmdlHdmiRxCapabilities_t            *pCapabilitiesList;
    UInt16                              keyDescriptionSeed;
    UInt8                               vsSyncDelay;
    UInt8                               hsSyncDelay;
    Bool                                clockPulseDelay;
    UInt8                               clockPulseDelayValue;
} tmdlHdmiRxDriverConfigTable_t;

extern const tmdlHdmiRxCfgSeed_t kRxSeedTable[RX_SEED_TABLE_LEN];

/*============================================================================*/
/*                       FUNCTIONS DECLARATIONS                               */
/*============================================================================*/

/**
    \brief This function allows to the main driver to retrieve its
           configuration parameters.

    \param pConfig Pointer to the config structure

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMICEC_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMDL_ERR_DLHDMICEC_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

******************************************************************************/
tmErrorCode_t tmdlHdmiRxCfgGetConfig
(
    tmUnitSelect_t                 unit,
    tmdlHdmiRxDriverConfigTable_t *pConfig
);

/**
    \brief This function allows to the main driver to store its
           configuration parameters.

    \param pConfig Pointer to the config structure

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMICEC_BAD_UNIT_NUMBER: the unit number is wrong or
              the receiver instance is not initialised
            - TMDL_ERR_DLHDMICEC_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

******************************************************************************/
tmErrorCode_t tmdlHdmiRxCfgSetConfig
(
    tmUnitSelect_t                 unit,
    tmdlHdmiRxDriverConfigTable_t *pConfig
);

#ifdef __cplusplus
}
#endif

#endif /* TMDLHDMIRX_CFG_H */

/*============================================================================*/
/*                               END OF FILE                                  */
/*============================================================================*/

