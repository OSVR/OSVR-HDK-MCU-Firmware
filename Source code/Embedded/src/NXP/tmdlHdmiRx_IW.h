#include "GlobalOptions.h"




/**
 * Copyright (C) 2007 NXP N.V., All Rights Reserved.
 * Exposed under Apache 2.0 Open Source License with permission of NXP N.V.
 *
 * \file          tmdlHdmiRxCfg.h
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

   $History: tmdlHdmiRxCfg.h $
 *
 * *****************  Version 1  *****************
 * User: Demoment     Date: 06/02/07   Time: 8:32
 * Updated in $/Source/tmdlHdmiRx/inc
 * initial version
 *

   \endverbatim
 *
*/

#ifndef TMDLHDMIRX_IW_H
#define TMDLHDMIRX_IW_H

/*============================================================================*/
/*                       INCLUDE FILES                                        */
/*============================================================================*/
#ifdef TMFL_OS_WINDOWS
    #define _WIN32_WINNT 0x0500
    #include "windows.h"
#endif

#include "tmNxTypes.h"
#include "tmdlHdmiRx_Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*/
/*                       TYPE DEFINITIONS                                     */
/*============================================================================*/
typedef void (*tmdlHdmiRxIWFuncPtr_t) (void);
typedef UInt8 tmdlHdmiRxIWTaskHandle_t;
typedef UInt8 tmdlHdmiRxIWQueueHandle_t;
#ifdef __LINUX_ARM_ARCH__
typedef unsigned long tmdlHdmiRxIWSemHandle_t;
#else
typedef UInt8 tmdlHdmiRxIWSemHandle_t;
#endif

/**
 * \brief Enum listing all available devices for enable/disable interrupts
 */
typedef enum
{
    TMDL_HDMI_IW_RX_1,
    TMDL_HDMI_IW_RX_2,
    TMDL_HDMI_IW_TX_1,
    TMDL_HDMI_IW_TX_2,
    TMDL_HDMI_IW_CEC_1,
    TMDL_HDMI_IW_CEC_2
} tmdlHdmiIWDeviceInterrupt_t;

/*============================================================================*/
/*                       EXTERN FUNCTION PROTOTYPES                           */
/*============================================================================*/

/**
    \brief This function creates a task and allocates all the necessary
           resources. Note that creating a task do not start it automatically,
           an explicit call to IWTaskStart must be made.

    \param pFunc      Pointer to the function that will be executed in the task context.
    \param priority   Priority of the task. The minimum priority is 0, the maximum is 255.
    \param stackSize  Size of the stack to allocate for this task.
    \param pHandle    Pointer to the handle buffer.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_NO_RESOURCES: the resource is not available
            - TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

******************************************************************************/
tmErrorCode_t tmdlHdmiRxIWTaskCreate(tmdlHdmiRxIWFuncPtr_t pFunc,UInt8 priority, UInt16 stackSize, tmdlHdmiRxIWTaskHandle_t *pHandle);

/*============================================================================*/

/**
    \brief    This function destroys an existing task and frees resources used by it.

    \param handle        Handle of the task to be destroyed, as returned by IWTaskCreate.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_RESOURCE_NOT_OWNED: the caller does not own
              the resource
            - TMDL_ERR_DLHDMIRX_BAD_HANDLE: the handle number is wrong

******************************************************************************/
tmErrorCode_t tmdlHdmiRxIWTaskDestroy(tmdlHdmiRxIWTaskHandle_t handle);

/*============================================================================*/

/**
    \brief This function start an existing task.

    \param handle        Handle of the task to be started.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_ALREADY_STARTED: the function is already started
            - TMDL_ERR_DLHDMIRX_NOT_STARTED: the function is not started
            - TMDL_ERR_DLHDMIRX_BAD_HANDLE: the handle number is wrong

******************************************************************************/
tmErrorCode_t tmdlHdmiRxIWTaskStart(tmdlHdmiRxIWTaskHandle_t handle);

/*============================================================================*/

/**
    \brief This function blocks the current task for the specified amount time. This is a passive wait.

    \param duration    Duration of the task blocking in milliseconds.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_NO_RESOURCES: the resource is not available

******************************************************************************/
tmErrorCode_t tmdlHdmiRxIWWait(UInt16 duration);

/*============================================================================*/

/**
    \brief This function creates a message queue.

    \param QueueSize    Maximum number of messages in the message queue.
    \param pHandle        Pointer to the handle buffer.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent
            - TMDL_ERR_DLHDMIRX_NO_RESOURCES: the resource is not available

******************************************************************************/
tmErrorCode_t tmdlHdmiRxIWQueueCreate(UInt8 queueSize, tmdlHdmiRxIWQueueHandle_t *pHandle);

/*============================================================================*/

/**
    \brief This function destroys an existing message queue.

    \param    handle        Handle of the queue to be destroyed.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_HANDLE: the handle number is wrong
            - TMDL_ERR_DLHDMIRX_RESOURCE_NOT_OWNED: the caller does not own
              the resource

******************************************************************************/
tmErrorCode_t tmdlHdmiRxIWQueueDestroy(tmdlHdmiRxIWQueueHandle_t handle);

/*============================================================================*/

/**
    \brief This function sends a message into the specified message queue.

    \param handle    Handle of the queue that will receive the message.
    \param message   Message to be sent.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_HANDLE: the handle number is wrong
            - TMDL_ERR_DLHDMIRX_RESOURCE_NOT_OWNED: the caller does not own
              the resource
            - TMDL_ERR_DLHDMIRX_FULL: the queue is full

******************************************************************************/
tmErrorCode_t tmdlHdmiRxIWQueueSend(tmdlHdmiRxIWQueueHandle_t handle, UInt8 message);

/*============================================================================*/

/**
    \brief This function reads a message from the specified message queue.

    \param    handle        Handle of the queue from which to read the message.
    \param    pMessage      Pointer to the message buffer.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_HANDLE: the handle number is wrong
            - TMDL_ERR_DLHDMIRX_RESOURCE_NOT_OWNED: the caller does not own
              the resource
            - TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

******************************************************************************/
tmErrorCode_t tmdlHdmiRxIWQueueReceive(tmdlHdmiRxIWQueueHandle_t handle, UInt8 *pMessage);

/*============================================================================*/

/**
    \brief This function creates a semaphore.

    \param     pHandle    Pointer to the handle buffer.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_NO_RESOURCES: the resource is not available
            - TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
              inconsistent

******************************************************************************/
tmErrorCode_t tmdlHdmiRxIWSemaphoreCreate(tmdlHdmiRxIWSemHandle_t *pHandle);

/*============================================================================*/

/**
    \brief This function destroys an existing semaphore.

    \param    handle        Handle of the semaphore to be destroyed.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_HANDLE: the handle number is wrong

******************************************************************************/
tmErrorCode_t tmdlHdmiRxIWSemaphoreDestroy(tmdlHdmiRxIWSemHandle_t handle);

/*============================================================================*/

/**
    \brief This function acquires the specified semaphore.

    \param handle        Handle of the semaphore to be acquired.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_HANDLE: the handle number is wrong

******************************************************************************/
tmErrorCode_t tmdlHdmiRxIWSemaphoreP(tmdlHdmiRxIWSemHandle_t handle);

/*============================================================================*/

/**
    \brief This function releases the specified semaphore.

    \param    handle        Handle of the semaphore to be released.

    \return The call result:
            - TM_OK: the call was successful
            - TMDL_ERR_DLHDMIRX_BAD_HANDLE: the handle number is wrong

******************************************************************************/
tmErrorCode_t tmdlHdmiRxIWSemaphoreV(tmdlHdmiRxIWSemHandle_t handle);

/*============================================================================*/
/**
    \brief  This function disables the interrupts for a specific device.

    \param  device  Device on which the interrupts are disabled.

    \return The call result:
            - TM_OK: the call was successful

******************************************************************************/
void tmdlHdmiRxIWDisableInterrupts(tmdlHdmiIWDeviceInterrupt_t device);

/*============================================================================*/
/**
    \brief  This function enables the interrupts for a specific device.

    \param  device  Device on which the interrupts are enabled.

    \return The call result:
            - TM_OK: the call was successful

******************************************************************************/
void tmdlHdmiRxIWEnableInterrupts(tmdlHdmiIWDeviceInterrupt_t device);


#ifdef __cplusplus
}
#endif

#endif /* TMDLHDMIRX_IW_H */

/*============================================================================*/
/*                            END OF FILE                                     */
/*============================================================================*/


