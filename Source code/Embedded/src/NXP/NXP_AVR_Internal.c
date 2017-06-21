/*
 * NXP_AVR_Internal.c
 *
 * Created: 7/19/2016 1:33:16 PM
 *  Author: Sensics
 */

#include "NXP_AVR_Internal.h"

#include "GlobalOptions.h"
#include "Console.h"
#ifdef SVR_HAVE_NXP

#include "tmbslHdmiRx_types.h"

/*============================================================================*/
/**
    \brief   Function that prints the returned error codes

    \param   Error code and line number

    \return  None

 ******************************************************************************/
void NXP_Private_PRINTIF(tmErrorCode_t errCode, uint16_t lineNumber)
{

//	return;
#ifdef HDMI_DEBUG
    char OutString[20];
    if(errCode != TM_OK)
    {
        sprintf(OutString,"E 0x%" TM_ERROR_CODE_FORMAT ", l %d", errCode, lineNumber);
        if  (errCode==999)
        {
            sprintf(OutString,"l %d",lineNumber);
            //return;
        }
        else
        {
            sprintf(OutString," " /*Error caught"*/);
        }

        if  (lineNumber==999)
        {
            sprintf(OutString,"l %" TM_ERROR_CODE_FORMAT, errCode);
            //return;
        }

        //MACRO_HDMI_DEBUG(OutString);
        WriteLn(OutString);
    }
#endif
}

#endif // SVR_HAVE_NXP
