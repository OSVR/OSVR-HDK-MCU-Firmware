/*
 * Copyright (C) 2007 NXP N.V., All Rights Reserved.
 * This source code and any compilation or derivative thereof is the proprietary
 * information of NXP N.V. and is confidential in nature. Under no circumstances
 * is this software to be  exposed to or placed under an Open Source License of
 * any type without the expressed written permission of NXP N.V.
 *
 * \file          tmdlHdmiRxCfg.c
 *
 * \version       $Revision: 1 $
 *
 * \date          $Date: 06/02/07 8:32 $
 *
 * \brief         devlib driver component API for the TDA997x HDMI Receivers
 *                  Configuration module
 * \section refs  Reference Documents
 * TDA997X Driver - FRS.doc,
 * TDA997X Driver - tmdlHdmiRx - SCS.doc
 *
 * \section info  Change Information
 *
 * \verbatim

   $History: tmdlHdmiRxCfg.c $
 *
 * *****************  Version 1  *****************
 * User: Demoment     Date: 06/02/07   Time: 8:32
 * Updated in $/Source/tmdlHdmiRx/inc
 * initial version
 *

   \endverbatim
 *
*/


/*============================================================================*/
/*                       INCLUDE FILES                                        */
/*============================================================================*/

#include "tmNxTypes.h"
#include "tmdlHdmiRx_Types.h"
#include "tmdlHdmiRx_cfg.h"
#include "tmNxCompId.h"
#include "tmdlHdmiRx_IW.h"

#include "LPC214x.H"
#include "Console.h"

#ifndef TMFL_NO_RTOS
    #include <RTL.h>
#endif

#include <asf.h>
#include "conf_usb.h"
#include "ui.h"
#include "uart.h"

#include <util/delay.h>

//char Msg[30];

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*/
/*                       DEFINES                                              */
/*============================================================================*/

/* maximum number of tasks that can be handled by the wrapper */
#define MAX_TASKS  6
/* maximum number of message queues that can be handled by the wrapper */
#define MAX_QUEUES 6
/* maximum number of message queues that can be handled by the wrapper */
#define MAX_SEMA   8

/* macro for quick error handling */
//#define RETIF(cond, rslt) if ((cond)){WriteLn("rx-iw"); return (rslt);}
#define RETIF(cond, rslt)       if ((cond)) \
	{PRINTIF(1998,__LINE__); \
	return (rslt);\
	}
/* Timer tick value in ms (defined in the "RTX_Config.c" file) */
#define TIMER_TICK_VALUE 10

/* Defines for Program Status Register (mode => CPSR[4:0]) */
#define MASK_CPSR_MODE   0x0000001FU
#define CPSR_NORMAL_MODE 0x00000010U
#define CPSR_IRQ_MODE    0x00000012U

/*============================================================================*/
/*                       TYPE DEFINITIONS                                     */
/*============================================================================*/

#ifndef TMFL_NO_RTOS
/* structure describing each task handled by the wrapper */
typedef struct
{
    Bool                   created;
    Bool                   started;
    UInt8                  priority;
    UInt16                 stackSize;
    UInt                   threadHandle;
    tmdlHdmiRxIWFuncPtr_t  associatedTask;
} iwTcb_t;

/* structure describing each message queue handled by the wrapper */
#define Queue_default_Size 128

os_mbx_declare(mbox0, Queue_default_Size);
os_mbx_declare(mbox1, Queue_default_Size);
os_mbx_declare(mbox2, Queue_default_Size);
os_mbx_declare(mbox3, Queue_default_Size);
os_mbx_declare(mbox4, Queue_default_Size);
os_mbx_declare(mbox5, Queue_default_Size);

_declare_box (mpool0,sizeof(UInt8),Queue_default_Size);
_declare_box (mpool1,sizeof(UInt8),Queue_default_Size);
_declare_box (mpool2,sizeof(UInt8),Queue_default_Size);
_declare_box (mpool3,sizeof(UInt8),Queue_default_Size);
_declare_box (mpool4,sizeof(UInt8),Queue_default_Size);
_declare_box (mpool5,sizeof(UInt8),Queue_default_Size);


typedef struct
{
    Bool            created;
    UInt16          queueSize;
    void *          mbox;
    void *          mpool;
} iwQueue_t;

/* structure describing each task handled by the wrapper */
typedef struct
{
    Bool                    created;
    OS_SEM                  handle;
} iwSem_t;
#endif
/*============================================================================*/
/*                       FUNCTION PROTOTYPES                                  */
/*============================================================================*/

/*============================================================================*/
/*                       VARIABLES                                            */
/*============================================================================*/

#ifndef TMFL_NO_RTOS
/* table storing all tasks descriptions */
iwTcb_t taskTable[MAX_TASKS] =
{
    {False, False, 0, 0, 0, Null},
    {False, False, 0, 0, 0, Null},
    {False, False, 0, 0, 0, Null},
    {False, False, 0, 0, 0, Null},
    {False, False, 0, 0, 0, Null},
    {False, False, 0, 0, 0, Null}
};

/* table storing all message queues descriptions */
iwQueue_t queueTable[MAX_QUEUES] =
{
    {False, 0, mbox0, mpool0},
    {False, 0, mbox1, mpool1},
    {False, 0, mbox2, mpool2},
    {False, 0, mbox3, mpool3},
    {False, 0, mbox4, mpool4},
    {False, 0, mbox5, mpool5}
};


/* table storing all message queues descriptions */
iwSem_t semTable[MAX_SEMA] =
{
    {False, 0},
    {False, 0},
    {False, 0},
    {False, 0},
    {False, 0},
    {False, 0},
    {False, 0},
    {False, 0}
};
#endif
/*============================================================================*/
/*                       FUNCTION                                                */
/*============================================================================*/

/**
    \brief This function creates a task and allocates all the necessary resources. Note that creating a task do not start it automatically, an explicit call to IWTaskStart must be made.
Parameters:

    \param pSWVersion    Pointer to the version structure
    \param pFunc        Pointer to the function that will be executed in the task context.
    \param Priority        Priority of the task. The minimum priority is 0, the maximum is 255.
    \param StackSize    Size of the stack to allocate for this task.
    \param pHandle        Pointer to the handle buffer.

    \return The call result:
            - TM_OK        If call is successful.
            - TM_NOK    If call failed.

******************************************************************************/
tmErrorCode_t tmdlHdmiRxIWTaskCreate(tmdlHdmiRxIWFuncPtr_t     pFunc,
                                     UInt8                     priority,
                                     UInt16                    stackSize,
                                     tmdlHdmiRxIWTaskHandle_t  *pHandle)
{
#ifndef TMFL_NO_RTOS
    UInt32 i;

    /* check that input pointer is not NULL */
    RETIF(pFunc == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)

    /* check that input pointer is not NULL */
    RETIF(pHandle == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)

    /* search for available task slot */
    for(i = 0; i < MAX_TASKS; i++)
    {
        if (taskTable[i].created == False)
            break;

    }
    if(i >= MAX_TASKS) return(TM_ERR_NO_RESOURCES);

    /* store task parameters into the dedicated structure */
    taskTable[i].priority = priority;
    taskTable[i].stackSize = stackSize;
    taskTable[i].associatedTask = pFunc;
    taskTable[i].created = True;

    *pHandle = (tmdlHdmiRxIWTaskHandle_t)i;

#endif

    return(TM_OK);
}

/*============================================================================*/

/**
    \brief    This function destroys an existing task and frees resources used by it.

    \param Handle        Handle of the task to be destroyed, as returned by IWTaskCreate.

    \return The call result:
            - TM_OK    If call is successful.
            - ~TM_OK    If call failed.

******************************************************************************/
tmErrorCode_t tmdlHdmiRxIWTaskDestroy(tmdlHdmiRxIWTaskHandle_t handle)
{
#ifndef TMFL_NO_RTOS
    /* check if handle number is in range */
    RETIF(handle >= MAX_TASKS, TMDL_ERR_DLHDMIRX_BAD_HANDLE)

    if (taskTable[handle].started == True)
    {
        if (os_tsk_delete(taskTable[handle].threadHandle) == OS_R_OK)
        {
            taskTable[handle].started = False;
        }
        else
        {
            return ~TM_OK;
        }
    }
    taskTable[handle].created = False;

#endif

    return(TM_OK);
}

/*============================================================================*/

/**
    \brief This function start an existing task.

    \param Handle        Handle of the task to be started.

    \return The call result:
            - TM_OK    If call is successful.
            - ~TM_OK    If call failed.

******************************************************************************/
tmErrorCode_t tmdlHdmiRxIWTaskStart(tmdlHdmiRxIWTaskHandle_t handle)
{
#ifndef TMFL_NO_RTOS
    UInt threadHandle;

    /* check if handle number is in range */
    RETIF(handle >= MAX_TASKS, TMDL_ERR_DLHDMIRX_BAD_HANDLE)

    /* check if task is already started */
    if (taskTable[handle].started == True)
        return(TM_ERR_ALREADY_STARTED);

    /* start thread associated to the task */
    threadHandle = os_tsk_create(taskTable[handle].associatedTask,
                                 taskTable[handle].priority);

    /* check return code for errors */
    if (!threadHandle )
    {
        return(TM_ERR_NOT_STARTED);
    }

    /* update task status */
    taskTable[handle].threadHandle = threadHandle;
    taskTable[handle].started = True;

#endif

    return(TM_OK);
}

/*============================================================================*/

/**
    \brief This function blocks the current task for the specified amount time. This is a passive wait.

    \param Duration    Duration of the task blocking in milliseconds.

    \return The call result:
            - TM_OK    If call is successful.
            - ~TM_OK    If call failed.

******************************************************************************/
tmErrorCode_t tmdlHdmiRxIWWait(UInt16 duration)
{
#ifndef TMFL_NO_RTOS
    UInt16 waitDuration = 0;

    if(duration < TIMER_TICK_VALUE)
    {
        waitDuration = TIMER_TICK_VALUE;
    }
    else
    {
        waitDuration = duration;
    }

    /* Perform conversion with Tick system in order to consider ms */
    os_dly_wait(waitDuration/TIMER_TICK_VALUE);
#else
    UInt16 count = 0;
    sprintf(Msg,"Delay %d",duration);
    dWriteLn(Msg,debugNXPLineMask);

    return(TM_OK); // *** - remove

    for (count=0; count<duration; count++)
        _delay_ms(1);

    return(TM_OK); // *** - remove

    /* The wait must be performed by using a timer */
    T0TCR = 0x00; /* Stop timer counter */
    T0TCR = 0x02; /* Reset timer counter */
    T0TCR = 0x01; /* Enable timer counter */

    while(count < duration)
    {
        while( !(T0IR & 0x01) );
        T0IR = 0x01; /* Clear the interrupt */
        count++;
        T0TCR = 0x01; /* Enable timer counter */
    }
#endif

    return(TM_OK);
}

/*============================================================================*/

/**
    \brief This function creates a message queue.

    \param QueueSize    Maximum number of messages in the message queue.
    \param pHandle        Pointer to the handle buffer.
    \return The call result:
            - TM_OK    If call is successful.
            - ~TM_OK    If call failed.

******************************************************************************/
tmErrorCode_t tmdlHdmiRxIWQueueCreate(UInt8 queueSize, tmdlHdmiRxIWQueueHandle_t *pHandle)
{
#ifndef TMFL_NO_RTOS
    UInt8 i;

    /* check that input pointer is not NULL */
    RETIF(pHandle == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)

    if (queueSize > Queue_default_Size) return TM_ERR_BAD_PARAMETER;


    /* search for available queue slot */
    for(i = 0; i < MAX_QUEUES; i++)
    {
        if (queueTable[i].created == False)
            break;

    }
    if(i >= MAX_QUEUES) return(TM_ERR_NO_RESOURCES);

    *pHandle = i;

    /* Initialisation of queue object */
    //TODO check if we can reuse deleted queue
    _init_box (queueTable[i].mpool, sizeof(mpool0),sizeof(UInt8));
    os_mbx_init(queueTable[i].mbox, sizeof(mbox0));

    /* update status of the queue table */
    queueTable[i].created = True;
    queueTable[i].queueSize = queueSize;

#endif

    return(TM_OK);
}

/*============================================================================*/

/**
    \brief This function destroys an existing message queue.

    \param    Handle        Handle of the queue to be destroyed.

    \return The call result:
            - TM_OK    If call is successful.
            - ~TM_OK    If call failed.

******************************************************************************/
tmErrorCode_t tmdlHdmiRxIWQueueDestroy(tmdlHdmiRxIWQueueHandle_t handle)
{
#ifndef TMFL_NO_RTOS
    void *msg;

    RETIF(handle > MAX_QUEUES, TM_ERR_BAD_HANDLE);
    RETIF(queueTable[handle].created == False, TM_ERR_BAD_HANDLE);

    queueTable[handle].created = False;

    while(os_mbx_check(queueTable[handle].mbox) != queueTable[handle].queueSize)
    {
        RETIF(os_mbx_wait(queueTable[handle].mbox, &msg, 10) == OS_R_TMO, TM_ERR_TIMEOUT);
    }

#endif

    return(TM_OK);
}

/*============================================================================*/

/**
    \brief This function sends a message into the specified message queue.

    \param Handle    Handle of the queue that will receive the message.
                    Message    Message to be sent.
    \return The call result:
            - TM_OK    If call is successful.
            - ~TM_OK    If call failed.

******************************************************************************/
tmErrorCode_t tmdlHdmiRxIWQueueSend(tmdlHdmiRxIWQueueHandle_t handle, UInt8 message)
{
#ifndef TMFL_NO_RTOS
    UInt8 * msg;
    UInt32 programStatus;

    /* check that handle is correct */
    RETIF(handle > MAX_QUEUES, TM_ERR_BAD_HANDLE);
    RETIF(queueTable[handle].created != True, TM_ERR_BAD_HANDLE);

    msg = _alloc_box (queueTable[handle].mpool);
    *msg = message;

    __asm{MRS programStatus, CPSR;}

    if(os_mbx_check(queueTable[handle].mbox) != 0)
    {
        if( (programStatus & MASK_CPSR_MODE) == CPSR_NORMAL_MODE)
        {
            RETIF(os_mbx_send(queueTable[handle].mbox, msg, 0xffff) == OS_R_TMO, TM_ERR_TIMEOUT);
        }
        else if( (programStatus & MASK_CPSR_MODE) == CPSR_IRQ_MODE)
        {
            isr_mbx_send(queueTable[handle].mbox, msg);
        }
        else
        {
            return TM_ERR_INVALID_STATE;
        }
    }
    else
    {
        return TM_ERR_FULL;
    }

#endif

    return(TM_OK);
}
/*============================================================================*/

/**
    \brief This function reads a message from the specified message queue.

    \param    Handle        Handle of the queue from which to read the message.
            pMessage    Pointer to the message buffer.

    \return The call result:
            - TM_OK    If call is successful.
            - ~TM_OK    If call failed.

******************************************************************************/
tmErrorCode_t tmdlHdmiRxIWQueueReceive(tmdlHdmiRxIWQueueHandle_t handle, UInt8 *pMessage)
{
#ifndef TMFL_NO_RTOS
    UInt8 * msg;
    /* check that handle is correct */
    RETIF(handle > MAX_QUEUES, TM_ERR_BAD_HANDLE);
    RETIF(queueTable[handle].created != True, TM_ERR_BAD_HANDLE);

    /* if we reach this point, this means that we got a message */
    /* ask for exclusive access to this queue */
    RETIF(os_mbx_wait(queueTable[handle].mbox , (void **) &msg, 0xffff) == OS_R_TMO, TM_ERR_TIMEOUT);
    *pMessage = *msg;
    _free_box (queueTable[handle].mpool, msg);

#endif

    return(TM_OK);
}

/*============================================================================*/

/**
    \brief This function creates a semaphore.

    \param     pHandle    Pointer to the handle buffer.

    \return The call result:
            - TM_OK    If call is successful.
            - ~TM_OK    If call failed.
******************************************************************************/
tmErrorCode_t tmdlHdmiRxIWSemaphoreCreate(tmdlHdmiRxIWSemHandle_t *pHandle)
{
#ifndef TMFL_NO_RTOS
    UInt8 i;

    /* check that input pointer is not NULL */
    RETIF(pHandle == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)

    /* check that input pointer is not NULL */
    RETIF(pHandle == Null, TMDL_ERR_DLHDMIRX_INCONSISTENT_PARAMS)

    /* search for available queue slot */
    for(i = 0; i < MAX_SEMA; i++)
    {
        if (semTable[i].created == False)
            break;

    }
    if(i >= MAX_SEMA) return(TM_ERR_NO_RESOURCES);

    os_sem_init(semTable[i].handle, 1);

    *pHandle = (tmdlHdmiRxIWSemHandle_t) i;

    semTable[i].created = True;

#endif

    return(TM_OK);
}

/*============================================================================*/

/**
    \brief This function destroys an existing semaphore.

    \param    Handle        Handle of the semaphore to be destroyed.

    \return The call result:
            - TM_OK     If call is successful.
            - ~TM_OK If call failed.
******************************************************************************/
tmErrorCode_t tmdlHdmiRxIWSemaphoreDestroy(tmdlHdmiRxIWSemHandle_t handle)
{
#ifndef TMFL_NO_RTOS
    UInt8  i;

    for(i=0; i<MAX_SEMA; i++)
    {
        if(i == handle)
        {
            break;
        }
    }
    if(i >= MAX_SEMA) return(TM_ERR_NO_RESOURCES);

    semTable[i].created = False;

#endif

    return(TM_OK);
}

/*============================================================================*/

/**
    \brief This function acquires the specified semaphore.

    \param Handle        Handle of the semaphore to be acquired.

    \return The call result:
            - TM_OK    If call is successful.
            - ~TM_OK    If call failed.

******************************************************************************/
tmErrorCode_t tmdlHdmiRxIWSemaphoreP(tmdlHdmiRxIWSemHandle_t handle)
{
#ifndef TMFL_NO_RTOS
    if (semTable[handle].created==False) return TM_ERR_BAD_HANDLE;

    RETIF(os_sem_wait(semTable[handle].handle, 0xffff) == OS_R_TMO, TM_ERR_TIMEOUT);
#endif

    return(TM_OK);
}

/*============================================================================*/

/**
    \brief This function releases the specified semaphore.

    \param    Handle        Handle of the semaphore to be released.

    \return The call result:
            - TM_OK    If call is successful.
            - ~TM_OK    If call failed.

******************************************************************************/
tmErrorCode_t tmdlHdmiRxIWSemaphoreV(tmdlHdmiRxIWSemHandle_t handle)
{
#ifndef TMFL_NO_RTOS
    if (semTable[handle].created==False) return TM_ERR_BAD_HANDLE;

    RETIF(os_sem_send(semTable[handle].handle) == OS_R_NOK, TM_ERR_BAD_HANDLE);
#endif

    return(TM_OK);
}

/*============================================================================*/

/**
    \brief  This function disables the interrupts for a specific device.

    \param  device  Device on which the interrupts are disabled.

    \return The call result:
            - TM_OK: the call was successful

******************************************************************************/
void tmdlHdmiRxIWDisableInterrupts(tmdlHdmiIWDeviceInterrupt_t device)
{
}

/*============================================================================*/

/**
    \brief  This function enables the interrupts for a specific device.

    \param  device  Device on which the interrupts are enabled.

    \return The call result:
            - TM_OK: the call was successful

******************************************************************************/
void tmdlHdmiRxIWEnableInterrupts(tmdlHdmiIWDeviceInterrupt_t device)
{
}


#ifdef __cplusplus
}
#endif

/*============================================================================*/
/*                            END OF FILE                                     */
/*============================================================================*/
