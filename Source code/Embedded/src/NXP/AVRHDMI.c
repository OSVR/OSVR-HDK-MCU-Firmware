// AVRHDMI.c : Defines the entry point for the console application.
//


//#include "stdafx.h"
#include "GlobalOptions.h"
#include "Console.h"
#include "NXP_AVR_Internal.h"


#include <asf.h>

#include "tmdlHdmiRx_cfg.h"
#include "tmbslTDA1997X_functions.h"

#include "my_bit.h"
#include "my_hardware.h"

#include "fpga.h"

#include "DeviceDrivers/Solomon.h"

#ifdef BNO070
#include "DeviceDrivers/BNO070.h"
#endif

#include "avrhdmi.h"
#include <util/delay.h>
#include <stdio.h>
#include "I2C.h"

#include "main.h"

#include "debugStrings.h"


void WriteLn_progmem(const char* Msg);

void WriteLn_progmem(const char* Msg)


{
//#ifdef HDMI_DEBUG
    uint8_t i;
    char Str[40];
    char c;

    i=0;
    do
    {
        c=pgm_read_byte(&(Msg[i]));
        Str[i]=c;
        i++;
    }
    while ((c!=0) && (i<40)); // prevent overflow
    Str[39]=0; // make sure the string is terminated
    WriteLn(Str);
//#endif
};


#ifdef HDMI_DEBUG
    //#define	MACRO_HDMI_DEBUG(Message)	WriteLn(Message);
    #define HDMI_debug_progmem(Message)	WriteLn_progmem(Message);
#else
    //#define	MACRO_HDMI_DEBUG(Message)	;
    #define HDMI_debug_progmem(Message)	;
    //#define HDMI_debug_progmem(Message)	WriteLn_progmem(Message);
#endif						\


bool InstanceOpened0=false; // indicates whether HDMI instance already created
bool KnownResolution0=false; // true once a known resolution is detected on port A
bool ActivityDetected0=false; // true if activity is detected on port A
tmdlHdmiRxResolutionID_t ResolutionID0=-1; // identifies detected resolution of port A
bool PortraitMode=false; // true if incoming video is in portrait mode
uint8_t HDMIStatus=0; // shows whether we have video and also video mode

/* int _tmain(int argc, _TCHAR* argv[])
{
	return 0;
}*/

/**
 * Copyright (C) 2006 Koninklijke Philips Electronics N.V., All Rights Reserved.
 * This source code and any compilation or derivative thereof is the proprietary
 * information of Koninklijke Philips Electronics N.V. and is confidential in
 * nature. Under no circumstances is this software to be  exposed to or placed
 * under an Open Source License of any type without the expressed written
 * permission of Koninklijke Philips Electronics N.V.
 *
 * \file          tmdlHdmiRxExampleAppli_No_OS.c
 *
 * \version       $Revision: 1 $
 *
 * \date          $Date: 16/04/08 14:00 $
 *
 * \brief         Example application for No OS
 *
 * \section refs  Reference Documents
 *
 * \section info  Change Information
 *
 * \verbatim
   $History: TMDLHDMIRXEXAMPLEAPPLI_NO_OS.C $
 *
  \endverbatim
 *
*/

/*============================================================================*/
/*                   FILE CONFIGURATION                                       */
/*============================================================================*/

/*============================================================================*/
/*                   STANDARD INCLUDE FILES                                   */
/*============================================================================*/
/* library system*/
#include <stdio.h>
//#include "LPC214x.H"
//#include "RTL.H"

/*============================================================================*/
/*                   PROJECT INCLUDE FILES                                    */
/*============================================================================*/

/* drivers includes */
#include "tmdlHdmiRx.h"
#include "tmdlHdmiRx_cfg.h"
#include "tmdlHdmiRx_IW.h"
#ifdef TMFL_TDA19972_FAMILY
    #include "tmbslTDA1997X_local.h"
#endif

/* drivers infrastructure */
#include "I2C.h"

/*============================================================================*/
/*                   INTERNAL TYPES                                           */
/*============================================================================*/

/* Enum listing the color spaces indicated in AVI infoframe (byte 1, field Y1:Y0) */
typedef enum
{
    TMEX_APPLI_COLORSPACE_RGB,        /* Color space is RGB */
    TMEX_APPLI_COLORSPACE_YCBCR_422,  /* Color space is YCbCr 4:2:2 */
    TMEX_APPLI_COLORSPACE_YCBCR_444   /* Color space is YCbCr 4:4:4 */
} tmExAppliColorSpace_t;

/* Enum listing the colorimetry indicated in AVI infoframe (byte 2, field C1:C0) */
typedef enum
{
    TMEX_APPLI_COLORIMETRY_NO_DATA,
    TMEX_APPLI_COLORIMETRY_SMPTE170_ITU601,
    TMEX_APPLI_COLORIMETRY_ITU709,
    TMEX_APPLI_COLORIMETRY_XVYCC
} tmExAppliColorimetry_t;

/* Enum listing the resolution types, it can be useful when colorimetry is not indicated in the AVI infoframe */
typedef enum
{
    TMEX_APPLI_SDTV_RESOLUTION,
    TMEX_APPLI_HDTV_RESOLUTION,
    TMEX_APPLI_PC_RESOLUTION
} tmExAppliResolutionType_t;

typedef struct
{
    tmExAppliColorSpace_t       colorSpace;
    tmExAppliColorimetry_t      colorimetry;
    tmExAppliResolutionType_t   resolutionType;
} tmExAppliColorSpaceManagement_t;

/*============================================================================*/
/*                   INTERNAL DEFINITIONS                                     */
/*============================================================================*/
/* Version of the SW driver */
#define VERSION_COMPATIBILITY 0
#define VERSION_MAJOR         7
#define VERSION_MINOR         0

/* Conversion matrices */
#define TMEX_APPLI_CONVERSIONMATRIX_RGBLimited_YCbCr601 TMDL_HDMIRX_CONVERSIONMATRIX_PREDEFINED_1
#define TMEX_APPLI_CONVERSIONMATRIX_YCbCr601_RGBLimited TMDL_HDMIRX_CONVERSIONMATRIX_PREDEFINED_2

tmdlHdmiRxColorMatrixCoefs_t TMEX_APPLI_CONVERSIONMATRIX_YCbCr709_RGBLimited = { -256, -2048, -2048,
                                                                                 4096, -1875,  -750,
                                                                                 4096,  6307,     0,
                                                                                 4096,     0,  7431,
                                                                                 256,   256,   256
                                                                               };

/* EDID */
/* 1080p60&50 xvYCC */
/*const UInt8 Oldedid1080p6050xvYCC[256] =
{ // Block 0
  0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x3B, 0x10, 0x0A, 0x4E, 0x01, 0x00, 0x00, 0x00,
  0x26, 0x11, 0x01, 0x03, 0x80, 0x00, 0x00, 0x00, 0x1A, 0xEE, 0x91, 0xA3, 0x54, 0x4C, 0x99, 0x26,
  0x0F, 0x50, 0x54, 0x20, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x1D, 0x00, 0x72, 0x51, 0xD0, 0x1E, 0x20, 0x6E, 0x28,
  0x55, 0x00, 0xC4, 0x8E, 0x21, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x44, 0x45, 0x4D,
  0x4F, 0x20, 0x54, 0x44, 0x41, 0x31, 0x39, 0x39, 0x37, 0x38, 0x00, 0x00, 0x00, 0xFD, 0x00, 0x32,
  0x3C, 0x0F, 0x44, 0x0F, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFE,
  0x00, 0x31, 0x30, 0x38, 0x30, 0x70, 0x36, 0x30, 0x26, 0x35, 0x30, 0x78, 0x76, 0x43, 0x01, 0xA1,

                                        // source physical address (2 bytes)
                                        // These bytes are replaced by sPAEdid[0] for input A, sPAEdid[1] for input B,
                                        // sPAEdid[2] for input C, sPAEdid[3] for input D
                                        // The position depends on the sPAOffsetEdid value
  // Block 1                                        ^     ^
  //
  0x02, 0x03, 0x2F, 0xF0, 0x6C, 0x03, 0x0C, 0x00, 0x10, 0x00, 0xB8, 0x2D, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x4B, 0x04, 0x13, 0x10, 0x1F, 0x03, 0x12, 0x05, 0x14, 0x07, 0x16, 0x01, 0x29, 0x0F, 0x17,
  0x07, 0x5F, 0x7F, 0x01, 0x67, 0x7F, 0x00, 0x83, 0x2F, 0x00, 0x00, 0xE3, 0x05, 0x03, 0x01, 0x01,
  0x1D, 0x00, 0xBC, 0x52, 0xD0, 0x1E, 0x20, 0xB8, 0x28, 0x55, 0x40, 0xC4, 0x8E, 0x21, 0x00, 0x00,
  0x1E, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C, 0x45, 0x00, 0xC4, 0x8E, 0x21,
  0x00, 0x00, 0x1E, 0x02, 0x3A, 0x80, 0xD0, 0x72, 0x38, 0x2D, 0x40, 0x10, 0x2C, 0x45, 0x80, 0xC4,
  0x8E, 0x21, 0x00, 0x00, 0x1E, 0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10, 0x10, 0x3E, 0x96,
  0x00, 0xC4, 0x8E, 0x21, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81
};*/


#ifdef OSVRHDK
const UInt8 edid1080p6050xvYCC[256] =
{
    //// EDID Block 0
    //0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x3B, 0x10, 0x03, 0x4E, 0x01, 0x00, 0x00, 0x00,
    //0x30, 0x15, 0x01, 0x03, 0x80, 0x00, 0x00, 0x00, 0x1A, 0xEE, 0x95, 0xA3, 0x54, 0x4C, 0x99, 0x26,
    //0x0F, 0x50, 0x54, 0x20, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    //0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
    //0x45, 0x00, 0xC4, 0x8E, 0x21, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x44, 0x45, 0x4D,
    //0x4F, 0x20, 0x54, 0x44, 0x41, 0x31, 0x39, 0x39, 0x37, 0x31, 0x00, 0x00, 0x00, 0xFD, 0x00, 0x32,
    //0x3C, 0x0F, 0x44, 0x0F, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFE,
    //0x00, 0x31, 0x30, 0x38, 0x30, 0x70, 0x44, 0x43, 0x78, 0x76, 0x43, 0x33, 0x44, 0x0A, 0x01, 0xAD,
//
    //// EDID Block 1
    //0x02, 0x03, 0x30, 0xF0, 0x70, 0x03, 0x0C, 0x00, 0x00, 0x00, 0xB8, 0x2D, 0x2F, 0xA0, 0x06, 0x01,
    //0x40, 0x30, 0x40, 0x60, 0x70, 0x83, 0x01, 0x00, 0x00, 0xE3, 0x05, 0x1F, 0x01, 0x4E, 0x10, 0x1F,
    //0x20, 0x22, 0x21, 0x04, 0x13, 0x03, 0x12, 0x05, 0x14, 0x07, 0x16, 0x01, 0x23, 0x09, 0x07, 0x07,
    //0x02, 0x3A, 0x80, 0xD0, 0x72, 0x38, 0x2D, 0x40, 0x10, 0x2C, 0x45, 0x80, 0xC4, 0x8E, 0x21, 0x00,
    //0x00, 0x1E, 0x01, 0x1D, 0x80, 0x3E, 0x73, 0x38, 0x2D, 0x40, 0x7E, 0x2C, 0x45, 0x80, 0xC4, 0x8E,
    //0x21, 0x00, 0x00, 0x1E, 0x01, 0x1D, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C, 0x45, 0x00,
    //0xC4, 0x8E, 0x21, 0x00, 0x00, 0x1E, 0x01, 0x1D, 0x80, 0xD0, 0x72, 0x38, 0x2D, 0x40, 0x10, 0x2C,
    //0x45, 0x80, 0xC4, 0x8E, 0x21, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB2};


// block 0
    //0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x4c, 0xae, 0x99, 0x66, 0x1a, 0x46, 0xda, 0x01,
    //0x23, 0x14, 0x01, 0x03, 0x80, 0x33, 0x1d, 0x78, 0xee, 0xee, 0x95, 0xa3, 0x54, 0x4c, 0x99, 0x26,
    //0x0f, 0x50, 0x54, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01,
    //0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x30, 0x2a, 0x00, 0x98, 0x51, 0x00, 0x2a, 0x40, 0x30, 0x70,
    //0x13, 0x00, 0xfe, 0x1f, 0x11, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0xff, 0x00, 0x53, 0x45, 0x4e,
    //0x53, 0x49, 0x43, 0x53, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x53,
    //0x45, 0x4e, 0x20, 0x7a, 0x53, 0x69, 0x67, 0x68, 0x74, 0x0a, 0x20, 0x20, 0x00, 0x00, 0x00, 0xfd,
    //0x00, 0x38, 0x4c, 0x1e, 0x53, 0x11, 0x00, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x1c,
//
    //// block 1
//
    //0x02, 0x03, 0x12, 0x71, 0x23, 0x09, 0x07, 0x07, 0x83, 0x01, 0x00, 0x00, 0x65, 0x03, 0x0c, 0x00,
    //0x10, 0x00, 0x30, 0x2a, 0x00, 0x98, 0x51, 0x00, 0x2a, 0x40, 0x30, 0x70, 0x13, 0x00, 0xfe, 0x1f,
    //0x11, 0x00, 0x00, 0x1e, 0x01, 0x1d, 0x00, 0x72, 0x51, 0xd0, 0x1e, 0x20, 0x6e, 0x28, 0x55, 0x00,
    //0xfe, 0x1f, 0x11, 0x00, 0x00, 0x1e, 0x93, 0x2e, 0x90, 0xa0, 0x60, 0x1a, 0x1e, 0x40, 0x30, 0x20,
    //0x36, 0x00, 0xfe, 0x1f, 0x11, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    //0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    //0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    //0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcb

// reference OSVRHDK OSVR below
    /*
    	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x4C,0xAE,0x19,0x10,0x1A,0x46,0xDA,0x01,
    	0x23,0x18,0x01,0x03,0x80,0x33,0x1D,0x78,0xEE,0xEE,0x95,0xA3,0x54,0x4C,0x99,0x26,
    	0x0F,0x50,0x54,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x01,0x01,0x01,
    	0x01,0x01,0x01,0x01,0x01,0x01,0x37,0x35,0x38,0x52,0x40,0x80,0x22,0x70,0x12,0x10,
    	0x36,0x00,0xA8,0x3E,0x11,0x00,0x00,0x1C,0x00,0x00,0x00,0xFF,0x00,0x53,0x45,0x4E,
    	0x53,0x49,0x43,0x53,0x0A,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0xFC,0x00,0x4F,
    	0x53,0x56,0x52,0x20,0x45,0x69,0x6C,0x65,0x65,0x6E,0x0A,0x20,0x00,0x00,0x00,0xFD,
    	0x00,0x38,0x4C,0x1E,0x53,0x11,0x00,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x01,0xCA,
    	0x02,0x03,0x12,0x71,0x23,0x09,0x07,0x07,0x83,0x01,0x00,0x00,0x65,0x03,0x0C,0x00,
    	0x10,0x00,0x30,0x2A,0x00,0x98,0x51,0x00,0x2A,0x40,0x30,0x70,0x13,0x00,0xFE,0x1F,
    	0x11,0x00,0x00,0x1E,0x01,0x1D,0x00,0x72,0x51,0xD0,0x1E,0x20,0x6E,0x28,0x55,0x00,
    	0xFE,0x1F,0x11,0x00,0x00,0x1E,0x93,0x2E,0x90,0xA0,0x60,0x1A,0x1E,0x40,0x30,0x20,
    	0x36,0x00,0xFE,0x1F,0x11,0x00,0x00,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCB

    */

// work area
    //0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x4C,0xAE,0x19,0x10,0x1A,0x46,0xDA,0x01,
    //0x23,0x18,0x01,0x03,0x80,0x33,0x1D,0x78,0xEE,0xEE,0x95,0xA3,0x54,0x4C,0x99,0x26,
    //0x0F,0x50,0x54,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x01,0x01,0x01,
    //0x01,0x01,0x01,0x01,0x01,0x01,0x02,0x3A,0x38,0xA5,0x40,0x80,0x44,0x70,0x82,0x04,
//
    //0xC6,0x0C,0xA8,0x3E,0x11,0x00,0x00,0x1C,0x00,0x00,0x00,0xFF,0x00,0x53,0x45,0x4E,
    //0x53,0x49,0x43,0x53,0x0A,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0xFC,0x00,0x4F,
    //0x53,0x56,0x52,0x20,0x48,0x44,0x4B,0x0A,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0xFD,
    //0x00,0x3B,0x3C,0x64,0x78,0x11,0x00,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x01,0x42,
//
    //0x02,0x03,0x12,0x71,0x23,0x09,0x07,0x07,0x83,0x01,0x00,0x00,0x65,0x03,0x0C,0x00,
    //0x10,0x00,0x30,0x2A,0x00,0x98,0x51,0x00,0x2A,0x40,0x30,0x70,0x13,0x00,0xFE,0x1F,
    //0x11,0x00,0x00,0x1E,0x01,0x1D,0x00,0x72,0x51,0xD0,0x1E,0x20,0x6E,0x28,0x55,0x00,
    //0xFE,0x1F,0x11,0x00,0x00,0x1E,0x93,0x2E,0x90,0xA0,0x60,0x1A,0x1E,0x40,0x30,0x20,
    //0x36,0x00,0xFE,0x1F,0x11,0x00,0x00,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCB

#ifdef H546DLT01
	// 0x4E,0xD2, 0x19, for SVR ID, checksum 0xBA
	// 0x4C 0xAE  0x19, for SEN ID - soon deprecated - checksum 0xE0)
    0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x4E,0xD2,
    0x19,0x10,0x1A,0x46,0xDA,0x01,0x0C,0x19,0x01,0x03,
    0x80,0x0C,0x07,0x78,0xEE,0xD8,0x7D,0xAC,0x53,0x3C,
    0xB6,0x21,0x10,0x4C,0x4F,0x00,0x00,0x00,0xD1,0xC0,
    0x01,0x00,0x01,0x00,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x65,0x33,0x38,0x30,0x40,0x80,
    0x17,0x70,0x20,0x05,0xC5,0x00,0x46,0x78,0x00,0x00,
    0x00,0x1C,0x00,0x00,0x00,0xFF,0x00,0x53,0x45,0x4E, 
    0x53,0x49,0x43,0x53,0x0A,0x20,0x20,0x20,0x20,0x20,
    0x00,0x00,0x00,0xFC,0x00,0x4f,0x53,0x56,0x52,0x20,
    0x48,0x44,0x4B,0x0A,0x20,0x20,0x20,0x20,0x00,0x00,
    0x00,0xFD,0x00,0x3B,0x3C,0x64,0x78,0x11,0x00,0x0A,
    0x20,0x20,0x20,0x20,0x20,0x20,0x00,0xF3, 

	// optional timing extension, not used
    0x02,0x03,0x0E,0x00,0x41,0x90,0x23,0x09,0x07,0x07,
    0x83,0x21,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3E
#else
    0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x4C,0xAE,
    0x19,0x10,0x1A,0x46,0xDA,0x01,0x23,0x18,0x01,0x03,
    0x80,0x06,0x0B,0x78,0xEE,0xEE,0x95,0xA3,0x54,0x4C,
    0x99,0x26,0x0F,0x50,0x54,0x00,0x00,0x00,0x01,0x00,
    0x01,0x00,0x01,0x00,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0xD5,0x34,0x38,0x52,0x40,0x80,
    0x14,0x70,0x0A,0x04,0x42,0x00,0x3E,0x6E,0x00,0x00,
    0x00,0x1C,0x00,0x00,0x00,0xFF,0x00,0x53,0x45,0x4E,
    0x53,0x49,0x43,0x53,0x0A,0x20,0x20,0x20,0x20,0x20,
    0x00,0x00,0x00,0xFC,0x00,0x4f,0x53,0x56,0x52,0x20,
    0x48,0x44,0x4B,0x0A,0x20,0x20,0x20,0x20,0x00,0x00,
    0x00,0xFD,0x00,0x3B,0x3C,0x64,0x78,0x0F,0x00,0x0A,
    0x20,0x20,0x20,0x20,0x20,0x20,0x01,0x8C,

    0x02,0x03,0x0E,0x00,0x41,0x90,0x23,0x09,0x07,0x07,
    0x83,0x21,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3E
#endif
    //0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x4C,0xAE,0x19,0x10,0x1A,0x46,0xDA,0x01,
    //0x23,0x14,0x01,0x03,0x80,0x33,0x1D,0x78,0xEE,0xEE,0x95,0xA3,0x54,0x4C,0x99,0x26,
    //0x0F,0x50,0x54,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x01,0x01,0x01,
    //0x01,0x01,0x01,0x01,0x01,0x01,0x21,0x35,0x38,0x42,0x40,0x80,0x22,0x70,0x12,0x0A,
    //0x36,0x00,0xA8,0x3E,0x11,0x00,0x00,0x1E,0x00,0x00,0x00,0xFF,0x00,0x53,0x45,0x4E,
    //0x53,0x49,0x43,0x53,0x0A,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0xFC,0x00,0x53,
    //0x45,0x4E,0x20,0x64,0x53,0x69,0x67,0x68,0x74,0x20,0x31,0x0A,0x00,0x00,0x00,0xFD,
    //0x00,0x38,0x4C,0x1E,0x53,0x11,0x00,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x01,0x04,
    //0x02,0x03,0x12,0x71,0x23,0x09,0x07,0x07,0x83,0x01,0x00,0x00,0x65,0x03,0x0C,0x00,
    //0x10,0x00,0x30,0x2A,0x00,0x98,0x51,0x00,0x2A,0x40,0x30,0x70,0x13,0x00,0xFE,0x1F,
    //0x11,0x00,0x00,0x1E,0x01,0x1D,0x00,0x72,0x51,0xD0,0x1E,0x20,0x6E,0x28,0x55,0x00,
    //0xFE,0x1F,0x11,0x00,0x00,0x1E,0x93,0x2E,0x90,0xA0,0x60,0x1A,0x1E,0x40,0x30,0x20,
    //0x36,0x00,0xFE,0x1F,0x11,0x00,0x00,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCB
};


#endif


// DDC config 0
#define Reg_EDID_VERSION 0x19
#define Reg_EDID_ENABLE 0x41
#define Reg_EDID_BLOCK_SELECT 0x44
#define Reg_Empty_0x83 0x00
#define Reg_HPD_POWER 0x06
#define Reg_HPD_AUTO_CTRL 0x18
#define Reg_HPD_DURATION 0x00
#define Reg_HPD_HEAC 0x00
const UInt8 DDC_Config0[8] = {Reg_EDID_VERSION, Reg_EDID_ENABLE, Reg_EDID_BLOCK_SELECT, Reg_Empty_0x83, Reg_HPD_POWER, Reg_HPD_AUTO_CTRL, Reg_HPD_DURATION, Reg_HPD_HEAC};

// DDC config 1
#define Reg_EDID_SPA_SUB 0x08
#define Reg_EDID_SPA_AB_A 0x10
#define Reg_EDID_SPA_CD_A 0x00
#define Reg_CHECKSUM_A 0xA2
#define Reg_EDID_SPA_AB_B 0x00
#define Reg_EDID_SPA_CD_B 0x00
#define Reg_CHECKSUM_B 0xB2
const UInt8 DDC_Config1[7] = {Reg_EDID_SPA_SUB, Reg_EDID_SPA_AB_A, Reg_EDID_SPA_CD_A, Reg_CHECKSUM_A, Reg_EDID_SPA_AB_B, Reg_EDID_SPA_CD_B, Reg_CHECKSUM_B};
const UInt16 sPAEdid[2] = { (Reg_EDID_SPA_AB_A << 8) + Reg_EDID_SPA_CD_A, (Reg_EDID_SPA_AB_B << 8) + Reg_EDID_SPA_CD_B };

// RT config
#define Reg_RT_AUTO_CTRL 0x78
#define Reg_EQ_MAN_CTRL0 0x03
#define Reg_EQ_MAN_CTRL1 0x33
#define Reg_OUTPUT_CFG 0xF0
#define Reg_MUTE_CTRL 0x00
#define Reg_SLAVE_ADR 0x10
const UInt8 RT_Config[6] = {Reg_RT_AUTO_CTRL, Reg_EQ_MAN_CTRL0, Reg_EQ_MAN_CTRL1, Reg_OUTPUT_CFG, Reg_MUTE_CTRL, Reg_SLAVE_ADR};

const UInt8 sPAOffsetEdid = (UInt8) 0x08U;  /* Offset of the first SPA byte inside EDID block 1 */

/* Interrupt line */
#define IT_LINE_ACTIVE 0

/*============================================================================*/
/*                   EXTERNAL PROTOTYPE                                       */
/*============================================================================*/

/*============================================================================*/
/*                   INTERNAL PROTOTYPE                                       */
/*============================================================================*/

/* Callbacks for first HDMI components */

static void digitalActivityCallback0(tmdlHdmiRxEvent_t event,
                                     tmdlHdmiRxInput_t input,
                                     tmdlHdmiRxSyncType_t syncType);
static void infoFrameCallback0(tmdlHdmiRxEvent_t  event,
                               UInt8             *data,
                               UInt8              size);
static void eventCallback0(tmdlHdmiRxEvent_t  event);



/* Other */
tmErrorCode_t tmdlHdmiRxExampleAppliInit(void);

static void readPortInterrupt(int *GPIO_input);

static void storeResolution0(tmdlHdmiRxResolutionID_t resolutionID);
static tmErrorCode_t tmExAppliConfigureConversion0(void);








/*============================================================================*/
/*                   GLOBAL VARIABLES                                  */
/*============================================================================*/
/* Instance variable of the HdmiRx device library */
tmInstance_t   gDlHdmiRxInstance0=0;


/* Instance setup variable of the HdmiRx device library */
tmdlHdmiRxInstanceSetupInfo_t gDlHdmiRxSetupInfo0;



/* Blanking codes values, they depend on the output color space (RGB or YCbCr) */
tmdlHdmiRxBlankingCodes_t gRGBBlankingCode = {64,64,64};
tmdlHdmiRxBlankingCodes_t gYCbCrBlankingCode = {64,512,512};

/* Color space information from AVI infoframe must be memorized in order to choose the proper conversion */
tmExAppliColorSpaceManagement_t gColorSpaceManagement0;


#ifdef HDMI_DEBUG
    //#define x	;
    //#define x	Write("AV-"); NXP_Private_PRINTIF(999,__LINE__);
#else
    //#define x	NXP_Private_PRINTIF(999,__LINE__);
    //#define x	;
#endif


/*============================================================================*/
/**
    \brief        Initialize all the required components

    \param        None

    \return     - TM_OK  : the call was successful
                - Any others: failed

 ******************************************************************************/


tmErrorCode_t tmdlHdmiRxExampleAppliInit(void)
{

    tmSWVersion_t   versionNr;
    tmdlVPBitsConfig VPConf[9];



    versionNr.compatibilityNr = VERSION_COMPATIBILITY;
    versionNr.majorVersionNr = VERSION_MAJOR;
    versionNr.minorVersionNr = VERSION_MINOR;


    /* Initialization of the variables */
    gColorSpaceManagement0.colorSpace     = TMEX_APPLI_COLORSPACE_RGB;
    gColorSpaceManagement0.colorimetry    = TMEX_APPLI_COLORIMETRY_NO_DATA;
    gColorSpaceManagement0.resolutionType = TMEX_APPLI_SDTV_RESOLUTION;


    /*--------------------------------------------------------*/
    /* Initialize the tmdlHdmiRx component                    */
    /*--------------------------------------------------------*/

    /* Open an instance of HdmiRx driver and provides the instance number to the caller */
    /* It recovers the configuration of the device librray and sets the I2C read/write functions */
    /* from the CFG file. */

#ifndef BYPASS_FIRST_NXP
    if (InstanceOpened0==false)
    {
        NXP_Private_PRINTIF(tmdlHdmiRxOpenM(&gDlHdmiRxInstance0, 0), __LINE__);

#ifdef TMFL_TDA19972_FAMILY
        /* Set HPD low */
        NXP_Private_PRINTIF(tmdlHdmiRxManualHPD(gDlHdmiRxInstance0, TMDL_HDMIRX_HPD_LOW), __LINE__);
#endif

        

//		sprintf(Msg,"Inst0=%d",gDlHdmiRxInstance0);
//		WriteLn(Msg);

        /* Register the callbacks */
        /* Three types of callbacks can be registered : */
        /*  - input activity related callback */
        /*  - data related callback (infoframes, packets, etc.) */
        /*  - general information callback */
        /* A null pointer means that no callback are registered */

        /* Parameters: */
        /* 1: Instance identifier */
        /* 2: pAnalogActivityCallback   Pointer to the callback function that will
                                        handle activity related events on analog
                                        inputs
                                        => Not used on the TDA19978 because there is no analog input */
        /* 3: pDigitalActivityCallback  Pointer to the callback function that will
                                        handle activity related events on digital
                                        inputs */
        /* 4: pDataCallback             Pointer to the callback function that will
                                        handle data related events (typically infoframes) */
        /* 5: pInfoCallback             Pointer to the callback function that will
                                        handle new informative events (input locked ...)*/
        NXP_Private_PRINTIF(tmdlHdmiRxRegisterCallbacks(gDlHdmiRxInstance0,
                                            Null,
                                            (ptmdlHdmiRxActivityCallback_t) digitalActivityCallback0,
                                            (ptmdlHdmiRxDataCallback_t)     infoFrameCallback0,
                                            (ptmdlHdmiRxInfoCallback_t)     eventCallback0
                                           ), __LINE__);

        
        InstanceOpened0=true;
    }
#endif




    /* This function allows basic instance configuration like enabling HDCP, */
    /* choosing HDCP encryption mode or enabling HDCP repeater mode and internal EDID */
    gDlHdmiRxSetupInfo0.hdcpEnable = true;
    gDlHdmiRxSetupInfo0.hdcpMode = TMDL_HDMIRX_HDCPMODE_AUTO;
    gDlHdmiRxSetupInfo0.hdcpRepeaterEnable = False;
    gDlHdmiRxSetupInfo0.internalEdid = true;

    
#ifndef BYPASS_FIRST_NXP
    NXP_Private_PRINTIF(tmdlHdmiRxInstanceSetup(gDlHdmiRxInstance0, &gDlHdmiRxSetupInfo0), __LINE__);
#endif
    



    /* The internal EDIDs are enabled, so load EDID data into embedded EDID memory */
#ifdef TMFL_TDA19972_FAMILY

#ifdef OSVRHDK
    /* The internal EDIDs are enabled, so we can now load EDID data into embedded EDID memory */
    NXP_Private_PRINTIF(tmdlHdmiRxLoadEdidData(gDlHdmiRxInstance0, (UInt8 *) edid1080p6050xvYCC, (UInt16 *) sPAEdid, Reg_EDID_SPA_SUB), __LINE__);
#endif

    /* Load DDC and RT data into embedded memory */ //
#ifndef BYPASS_FIRST_NXP
    NXP_Private_PRINTIF(tmdlHdmiRxLoadConfigData(gDlHdmiRxInstance0, (UInt8 *) DDC_Config0, (UInt8 *) RT_Config), __LINE__);


    /* Set HPD high */
    NXP_Private_PRINTIF(tmdlHdmiRxManualHPD(gDlHdmiRxInstance0, TMDL_HDMIRX_HPD_HIGH), __LINE__);
#endif
#else
    NXP_Private_PRINTIF(tmdlHdmiRxLoadEdidData(gDlHdmiRxInstance0, (UInt8 *) edid1080p6050xvYCC, (UInt16 *) sPAEdid, sPAOffsetEdid), __LINE__);
    
#endif


    /*-----------------------------------------------------------------------------*/
    /* Set default datapath                                                        */
    /*-----------------------------------------------------------------------------*/

    /* Select an input */
    /* Auto digital means that we use the automatic mode of chip, the input selection is based on a priority */
    /* input A > input B > input C > input D */

#ifndef BYPASS_FIRST_NXP
    NXP_Private_PRINTIF(tmdlHdmiRxSelectInput(gDlHdmiRxInstance0, TMDL_HDMIRX_INPUT_HDMI_A), __LINE__);
#endif
    /* Set bypass of the color space matrix => no conversion */
    /* Blanking codes values depend on the output color space (RGB or YCbCr), if Null they are not set */
#ifndef BYPASS_FIRST_NXP
    NXP_Private_PRINTIF(tmdlHdmiRxSetColorConversionMatrix(gDlHdmiRxInstance0, TMDL_HDMIRX_CONVERSIONMATRIX_BYPASS, Null, Null), __LINE__);
#endif



    
    /* Set output mode to 4:4:4, blanking and timing code are not inserted, clock mode is single edge */

#ifndef BYPASS_FIRST_NXP
    NXP_Private_PRINTIF(tmdlHdmiRxSetVideoOutputFormat(gDlHdmiRxInstance0, TMDL_HDMIRX_OUTPUTFORMAT_444, False, False, TMDL_HDMIRX_OUTCLKMODE_SINGLE_EDGE), __LINE__); 
#endif
    

    /* Route the video port according to board configuration */
    /* Video port resolution must always be 10 bits */
    /* Here, Blue is output on video port A, Green is output on video port B, Red is output on video port C */


    


    VPConf[0].pinGroup = TMDL_HDMI_RX_VP24_G4_3_0;
    VPConf[0].colorQuartet = TMDL_HDMI_RX_B_CB_7_4;

    VPConf[1].pinGroup = TMDL_HDMI_RX_VP24_G4_7_4;
    VPConf[1].colorQuartet = TMDL_HDMI_RX_B_CB_11_8;

    VPConf[2].pinGroup = TMDL_HDMI_RX_VP24_G4_11_8;
    VPConf[2].colorQuartet = TMDL_HDMI_RX_G_Y_7_4;

    VPConf[3].pinGroup = TMDL_HDMI_RX_VP24_G4_15_12;
    VPConf[3].colorQuartet = TMDL_HDMI_RX_G_Y_11_8;

    VPConf[4].pinGroup = TMDL_HDMI_RX_VP24_G4_19_16;
    VPConf[4].colorQuartet = TMDL_HDMI_RX_R_CR_CBCR_YCBCR_7_4;

    VPConf[5].pinGroup = TMDL_HDMI_RX_VP24_G4_23_20;
    VPConf[5].colorQuartet = TMDL_HDMI_RX_R_CR_CBCR_YCBCR_11_8;

#ifndef BYPASS_FIRST_NXP
    tmdlHdmiRxConfigureOutputVideoPort(gDlHdmiRxInstance0, VPConf);
#endif


    



    /* Set audio to I2S 32 bits format and audio sysclk frequency to 256FS */
#ifndef BYPASS_FIRST_NXP
    NXP_Private_PRINTIF(tmdlHdmiRxSetAudioOutputFormat(gDlHdmiRxInstance0,
                                           TMDL_HDMIRX_AUDIOFORMAT_I2S32,
                                           TMDL_HDMIRX_AUDIOSYSCLK_256FS), __LINE__);
#endif
    




    /* Enable detection of all the event */
#ifndef BYPASS_FIRST_NXP
    NXP_Private_PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance0, TMDL_HDMIRX_ACTIVITY_DETECTED), __LINE__);                 /* New activity has been detected */
    NXP_Private_PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance0, TMDL_HDMIRX_ACTIVITY_LOST), __LINE__);                     /* Activity has been lost */
#endif
    




    // ISRC was developed by the international recording industry through the International Organisation for Standardisation (ISO) as a response to a need to identify sound and music video recordings
#ifndef BYPASS_FIRST_NXP

    NXP_Private_PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance0, TMDL_HDMIRX_ISRC1_PACKET_RECEIVED), __LINE__);             /* ISRC1 packet has been received */
    NXP_Private_PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance0, TMDL_HDMIRX_ISRC2_PACKET_RECEIVED), __LINE__);             /* ISRC2 packet has been received */

    NXP_Private_PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance0, TMDL_HDMIRX_ACP_PACKET_RECEIVED), __LINE__);               /* Audio Content Protection packet has been received */
    NXP_Private_PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance0, TMDL_HDMIRX_AVI_INFOFRAME_RECEIVED), __LINE__);            /* Auxiliary Video Information infoframe has been received */
    NXP_Private_PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance0, TMDL_HDMIRX_SPD_INFOFRAME_RECEIVED), __LINE__);            /* Source product Description infoframe has been received */
    NXP_Private_PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance0, TMDL_HDMIRX_AUD_INFOFRAME_RECEIVED), __LINE__);            /* Audio infoframe has been received */
    NXP_Private_PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance0, TMDL_HDMIRX_MPS_INFOFRAME_RECEIVED), __LINE__);            /* MPEG Source infoframe has been received */
    
    NXP_Private_PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance0, TMDL_HDMIRX_ENCRYPTION_DETECTED), __LINE__);               /* Indicates that the active digital input is receiving HDCP encrypted data */
    NXP_Private_PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance0, TMDL_HDMIRX_EESS_HDCP_DETECTED), __LINE__);                /* Indicates that the active digital input is receiving data in EESS mode */
    NXP_Private_PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance0, TMDL_HDMIRX_GBD_PACKET_RECEIVED), __LINE__);               /* Indicates a GAMUT packet has been received */
    NXP_Private_PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance0, TMDL_HDMIRX_AUDIO_SAMPLE_PACKET_DETECTED), __LINE__);      /* Indicates that audio samples packets are detected */
    NXP_Private_PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance0, TMDL_HDMIRX_AUDIO_HBR_PACKET_DETECTED), __LINE__);         /* Indicates that audio HBR packets are detected */
    NXP_Private_PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance0, TMDL_HDMIRX_AUDIO_OBA_PACKET_DETECTED), __LINE__);         /* Indicates that audio OBA packets are detected */
    
    NXP_Private_PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance0, TMDL_HDMIRX_AUDIO_DST_PACKET_DETECTED), __LINE__);         /* Indicates that audio DST packets are detected */
    NXP_Private_PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance0, TMDL_HDMIRX_HDMI_DETECTED), __LINE__);                     /* Indicates that HDMI stream is detected */
    NXP_Private_PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance0, TMDL_HDMIRX_DVI_DETECTED), __LINE__);                      /* Indicates that DVI stream is detected */
    NXP_Private_PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance0, TMDL_HDMIRX_NO_ENCRYPTION_DETECTED), __LINE__);            /* Indicates the the active digital input is not receiving HDCP encrypted data */
    NXP_Private_PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance0, TMDL_HDMIRX_OESS_HDCP_DETECTED), __LINE__);                /* Indicates that the active digital input is receiving data in OESS mode */
    NXP_Private_PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance0, TMDL_HDMIRX_AVI_AVMUTE_ACTIVE), __LINE__);                 /* AV mute active received */
    
    NXP_Private_PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance0, TMDL_HDMIRX_AVI_AVMUTE_INACTIVE), __LINE__);               /* AV mute inactive received */
    
    NXP_Private_PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance0, TMDL_HDMIRX_AUDIO_SAMPLE_FREQ_CHANGED), __LINE__);         /* Indicates that audio sampling frequency has changed */
    
    NXP_Private_PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance0, TMDL_HDMIRX_INPUT_LOCKED), __LINE__);                      /* Indicates that clocks are locked on the new active input */
    
    NXP_Private_PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance0, TMDL_HDMIRX_REPEATER), __LINE__);                          /* Indicates that the HDCP repeater authentication can start */
    
    NXP_Private_PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance0, TMDL_HDMIRX_DEEP_COLOR_MODE_24BITS), __LINE__);            /* Indicates that deep color mode is 24 bits */
    
    NXP_Private_PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance0, TMDL_HDMIRX_DEEP_COLOR_MODE_30BITS), __LINE__);            /* Indicates that deep color mode is 30 bits */
    
    NXP_Private_PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance0, TMDL_HDMIRX_DEEP_COLOR_MODE_36BITS), __LINE__);            /* Indicates that deep color mode is 36 bits */
#endif
    

#ifdef HDMI_DEBUG
    WriteLn("End init");
#endif
    return TM_OK;
}

/*============================================================================*/
/**
    \brief          Callback on digital activity detection
                    Used to select the correct input and to display
                    information on the terminal.

    \param event    Activities Event raised by the interrupts
    \param input    Input that raises the event
    \param syncType Type of synchronization used by the input
                    (not relevant in our case because HDMI always uses separated sync)

    \return         None

 ******************************************************************************/
static void digitalActivityCallback0 (tmdlHdmiRxEvent_t event,
                                      tmdlHdmiRxInput_t input,
                                      tmdlHdmiRxSyncType_t syncType)
{
    WriteLn("Digital CB 0");

    HDMI_debug_progmem(cInput);
    switch (input)
    {
    case TMDL_HDMIRX_INPUT_HDMI_A: /* New activity has been detected */
        HDMI_debug_progmem(cA);
        break;
    case TMDL_HDMIRX_INPUT_HDMI_B: /* New activity has been detected */
        HDMI_debug_progmem(cB);
        break;


    default:
        HDMI_debug_progmem(cUnknown);
    }

    switch (event) {
    case TMDL_HDMIRX_ACTIVITY_DETECTED: /* New activity has been detected */

        //HDMI_debug_progmem(cDigitalActivityDetected);
		WriteLn_progmem(cDigitalActivityDetected);
        //WriteLn("AD0 1+");
        ActivityDetected0=true;
        KnownResolution0=false;
        ResolutionID0=-1;
		#ifdef Solomon1_SPI
			#ifdef H546DLT01
				Display_On(Solomon1);
				NXP_Update_Resolution_Detection();
				#ifdef BNO070
					Update_BNO_Report_Header();
				#endif
			#endif
		#endif
        break;
    case TMDL_HDMIRX_ACTIVITY_LOST: /* New activity has been detected */
        //HDMI_debug_progmem(cDigitalActivityLost);
		WriteLn_progmem(cDigitalActivityLost);
        ResolutionID0=-1;
		VideoLost=true;
        //WriteLn("AD0 1");
        ActivityDetected0=false;
        KnownResolution0=false;
        if (HDMIShadow)
        {
        }

        break;
    default:
        HDMI_debug_progmem(cUnknown);
    }

}



/*============================================================================*/
/**
    \brief          Callback on infoframe reception
                    Used to display information on the terminal.

    \param  event   Event raised corresponding to infoframe type received
    \param  data    content of the infoframe received
    \param  size    size of the infoframe received

    \return         None

 ******************************************************************************/
static void infoFrameCallback0(tmdlHdmiRxEvent_t  event,
                               UInt8             *intData,
                               UInt8              size)
{
    //WriteLn("Info CB 0");
    tmdlHdmiRxAVIInfoframe_t*   pAVIInfoFrame;

    
    HDMI_debug_progmem(cReceiverA);
    HDMI_debug_progmem(cInfoframe);
    switch (event)
    {
    case TMDL_HDMIRX_ISRC1_PACKET_RECEIVED:
        HDMI_debug_progmem(cISRC1Received);
        break;

    case TMDL_HDMIRX_ISRC2_PACKET_RECEIVED:
        HDMI_debug_progmem(cISRC2Received);
        break;

    case TMDL_HDMIRX_ACP_PACKET_RECEIVED:
        HDMI_debug_progmem(cACPReceived);
        break;

    case TMDL_HDMIRX_AVI_INFOFRAME_RECEIVED:
        HDMI_debug_progmem(cAVIReceived);
        pAVIInfoFrame = (tmdlHdmiRxAVIInfoframe_t*) intData;

        if (pAVIInfoFrame->packetType != 0x82)
        {
            HDMI_debug_progmem(cWrongPacketType);
        }

        /* Memorize the color indicator */
        HDMI_debug_progmem(cColorSpace);
        switch (pAVIInfoFrame->colorIndicator)
        {
        case 0:
            gColorSpaceManagement0.colorSpace = TMEX_APPLI_COLORSPACE_RGB;
            HDMI_debug_progmem(cRGB);
            break;
        case 1:
            gColorSpaceManagement0.colorSpace = TMEX_APPLI_COLORSPACE_YCBCR_422;
            HDMI_debug_progmem(cYCbCr422);
            break;
        case 2:
            gColorSpaceManagement0.colorSpace = TMEX_APPLI_COLORSPACE_YCBCR_444;
            HDMI_debug_progmem(cYCbCr444);
            break;
        case 3:
            gColorSpaceManagement0.colorSpace = TMEX_APPLI_COLORSPACE_YCBCR_444;
            HDMI_debug_progmem(cFuture);
            break;
        default:
            gColorSpaceManagement0.colorSpace=TMEX_APPLI_COLORSPACE_RGB;
            HDMI_debug_progmem(cWrongColorIndicator);
        }

#ifdef HDMI_DEBUG
        Write("colorimetry:");
#endif

        /* Memorize the colorimetry */
        switch (pAVIInfoFrame->colorimetry)
        {
        case 0:
            gColorSpaceManagement0.colorimetry = TMEX_APPLI_COLORIMETRY_NO_DATA;
            HDMI_debug_progmem(cNoData);
            break;
        case 1:
            gColorSpaceManagement0.colorimetry = TMEX_APPLI_COLORIMETRY_SMPTE170_ITU601;
            HDMI_debug_progmem(cSMPTE170M);
            break;
        case 2:
            gColorSpaceManagement0.colorimetry = TMEX_APPLI_COLORIMETRY_ITU709;
            HDMI_debug_progmem(cITU709);
            break;
        case 3:
            gColorSpaceManagement0.colorimetry = TMEX_APPLI_COLORIMETRY_XVYCC;
            HDMI_debug_progmem(cExtendedxvYCC);
            break;
        default:
            gColorSpaceManagement0.colorimetry = TMEX_APPLI_COLORIMETRY_NO_DATA;
            HDMI_debug_progmem(cWrong);
        }

        /* Configure the receiver with the new color space */
        
        NXP_Private_PRINTIF(tmExAppliConfigureConversion0(), __LINE__);
        
        break;

#ifndef SUPPORT_3D_FP
    case TMDL_HDMIRX_SPD_INFOFRAME_RECEIVED:
        HDMI_debug_progmem(cInfoframeSPDreceived);
        break;
#endif /* SUPPORT_3D_FP */

    case TMDL_HDMIRX_AUD_INFOFRAME_RECEIVED:
        HDMI_debug_progmem(cInfoframeAUDreceived);
        break;

    case TMDL_HDMIRX_MPS_INFOFRAME_RECEIVED:
        HDMI_debug_progmem(cInfoframeMPEGReceived);
        break;

    case TMDL_HDMIRX_GBD_PACKET_RECEIVED:
        HDMI_debug_progmem(cGamutMetadataReceived);
        break;

    default:
        HDMI_debug_progmem(cErroneousEventInfoframe);
        break;
    }
}




/*============================================================================*/
/**
    \brief          Callback on event reception
                    Used to handle informative events.

    \param          event: event raised by the device library

    \return         None

 ******************************************************************************/

// Sensics - to continue

static void eventCallback0 (tmdlHdmiRxEvent_t  event)
{
    tmdlHdmiRxResolutionID_t     resolutionID = TMDL_HDMIRX_VIDEORES_UNKNOWN;
    tmdlHdmiRxAudioDstRate_t     dstRate;
    tmdlHdmiRxAudioSampleFreq_t  audioFreq;

    //WriteLn("Event CB 0");
    switch (event)
    {
    case TMDL_HDMIRX_INPUT_LOCKED:

        HDMI_debug_progmem(cAutomaticALocked);


        /* Detect the video resolution */
        NXP_Private_PRINTIF(tmdlHdmiRxDetectResolution(gDlHdmiRxInstance0, &resolutionID), __LINE__);

        /* Display on the terminal the name of the detected resolution */
        HDMI_debug_progmem(cAutomaticDetectionOfResolution);
		
		// the following lines force the resolution. Uncomment for test if you wish
        //resolutionID=TMDL_HDMIRX_VIDEORES_1920_1080p_60HZ; 
        //resolutionID=TMDL_HDMIRX_VIDEORES_1080_1920p_60HZ; 
		
		NewVideoDetected=true; // tell main that it might need to reconfigure Solomon
        storeResolution0(resolutionID);


        /* Configure the active input to the given resolution */

        NXP_Private_PRINTIF(tmdlHdmiRxConfigureInput(gDlHdmiRxInstance0, resolutionID), __LINE__);

        /* On "input locked" event, RGB color space is forced (the AVI infoframe is not received yet at this moment) */
        /* If an AVI infoframe is received later, the color space will be re-configured in the AVI infoframe callback */
        gColorSpaceManagement0.colorSpace  = TMEX_APPLI_COLORSPACE_RGB;
        gColorSpaceManagement0.colorimetry = TMEX_APPLI_COLORIMETRY_NO_DATA;

        /* Configure the receiver with the new color space */
        
        NXP_Private_PRINTIF(tmExAppliConfigureConversion0(), __LINE__);
        
        break;

    case TMDL_HDMIRX_HDMI_DETECTED:
        HDMI_debug_progmem(cHDMIMode);
        break;

    case TMDL_HDMIRX_DVI_DETECTED:
        HDMI_debug_progmem(cDVIMode);
        /* RGB mandatory in DVI mode and there are no infoframes */
        gColorSpaceManagement0.colorSpace  = TMEX_APPLI_COLORSPACE_RGB;
        gColorSpaceManagement0.colorimetry = TMEX_APPLI_COLORIMETRY_NO_DATA;

        /* Configure the receiver with the new color space */
        
        NXP_Private_PRINTIF(tmExAppliConfigureConversion0(), __LINE__);
        
        break;

    case TMDL_HDMIRX_ENCRYPTION_DETECTED:
        HDMI_debug_progmem(cHDCPEncrypted);
        break;

    case TMDL_HDMIRX_NO_ENCRYPTION_DETECTED:
        HDMI_debug_progmem(cNotHDCPEncrypted);
        break;

    case TMDL_HDMIRX_EESS_HDCP_DETECTED:
        HDMI_debug_progmem(cEESSMode);
        break;

    case TMDL_HDMIRX_OESS_HDCP_DETECTED:
        HDMI_debug_progmem(cDVIOESSMode);
        break;

    case TMDL_HDMIRX_AVI_AVMUTE_ACTIVE:
        HDMI_debug_progmem(cAVMuteActive);
        break;

    case TMDL_HDMIRX_AVI_AVMUTE_INACTIVE:
        HDMI_debug_progmem(cAVMuteInactive);
        break;

    case TMDL_HDMIRX_DEEP_COLOR_MODE_24BITS:
        HDMI_debug_progmem(cDeepColor24Bit);
        break;

    case TMDL_HDMIRX_DEEP_COLOR_MODE_30BITS:
        HDMI_debug_progmem(cDeepColor30Bit);
        break;

    case TMDL_HDMIRX_DEEP_COLOR_MODE_36BITS:
        HDMI_debug_progmem(cDeepColor36Bit);
        break;

    case TMDL_HDMIRX_AUDIO_SAMPLE_PACKET_DETECTED:
        HDMI_debug_progmem(cAudioSamplePackets);
        break;

    case TMDL_HDMIRX_AUDIO_HBR_PACKET_DETECTED:
        HDMI_debug_progmem(cAudioHBRPackets);
        break;

    case TMDL_HDMIRX_AUDIO_OBA_PACKET_DETECTED:
        HDMI_debug_progmem(cAudioOBAPackets);
        break;

    case TMDL_HDMIRX_AUDIO_DST_PACKET_DETECTED:
        HDMI_debug_progmem(cAudioDSTPackets);
        break;

    case TMDL_HDMIRX_AUDIO_SAMPLE_FREQ_CHANGED:
#ifdef HDMI_debug
        WriteLn("Audio changed");
#endif
        /* Get the new audio sampling frequency */
        NXP_Private_PRINTIF(tmdlHdmiRxGetAudioFrequency(gDlHdmiRxInstance0, &dstRate, &audioFreq), __LINE__);

        switch(audioFreq)
        {
        case TMDL_HDMIRX_AUDIO_SAMPLEFREQ_ERROR:
            HDMI_debug_progmem(cCalculationError);
            break;
        case TMDL_HDMIRX_AUDIO_SAMPLEFREQ_32_KHZ:
            HDMI_debug_progmem(cAudioSample32KHz);
            break;
        case TMDL_HDMIRX_AUDIO_SAMPLEFREQ_44_KHZ:
            HDMI_debug_progmem(cAudioSample44KHz);
            break;
        case TMDL_HDMIRX_AUDIO_SAMPLEFREQ_48_KHZ:
            HDMI_debug_progmem(cAudioSample48KHz);
            break;
        case TMDL_HDMIRX_AUDIO_SAMPLEFREQ_88_KHZ:
            HDMI_debug_progmem(cAudioSample88KHz);
            break;
        case TMDL_HDMIRX_AUDIO_SAMPLEFREQ_96_KHZ:
            HDMI_debug_progmem(cAudioSample96KHz);
            break;
        case TMDL_HDMIRX_AUDIO_SAMPLEFREQ_176_KHZ:
            HDMI_debug_progmem(cAudioSample176KHz);
            break;
        case TMDL_HDMIRX_AUDIO_SAMPLEFREQ_192_KHZ:
            HDMI_debug_progmem(cAudioSample192KHz);
            break;
        default:
            HDMI_debug_progmem(cWrongValue);
        }
        break;

    case TMDL_HDMIRX_REPEATER:
        HDMI_debug_progmem(cRepeaterState5Reached);
        break;

    default:
        HDMI_debug_progmem(cErroneousEvent);
        break;
    }
    
}




/*============================================================================*/
/**
    \brief        Read the Rx interrupt pin status on microcontroller

    \param        Integer to recover GPIO status

    \return       None

 ******************************************************************************/


/*============================================================================*/
/**
    \brief        print resolution on the terminal

    \param        resolutionID: Resolution to display

    \return     - TM_OK  : the call was successful
                - Any others: failed

 ******************************************************************************/
static void storeResolution0(tmdlHdmiRxResolutionID_t resolutionID)
{


    HDMI_debug_progmem(cReceiverA);
    switch (resolutionID)
    {
    /* 60 Hz TV*/
    case TMDL_HDMIRX_VIDEORES_720_480p_60HZ:
        gColorSpaceManagement0.resolutionType = TMEX_APPLI_SDTV_RESOLUTION;
        HDMI_debug_progmem(c720_480p_60HZ);
        break;
    case TMDL_HDMIRX_VIDEORES_1280_720p_60HZ:
        gColorSpaceManagement0.resolutionType = TMEX_APPLI_HDTV_RESOLUTION;
        HDMI_debug_progmem(c1280_720p_60HZ);
        break;
    case TMDL_HDMIRX_VIDEORES_1920_1080i_60HZ :
        gColorSpaceManagement0.resolutionType = TMEX_APPLI_HDTV_RESOLUTION;
        HDMI_debug_progmem(c1920_1080i_60HZ);
        break;
    case TMDL_HDMIRX_VIDEORES_720_480i_60HZ:
        gColorSpaceManagement0.resolutionType = TMEX_APPLI_SDTV_RESOLUTION;
        HDMI_debug_progmem(c720_480i_60HZ);
        break;
    case TMDL_HDMIRX_VIDEORES_1920_1080p_60HZ:
        gColorSpaceManagement0.resolutionType = TMEX_APPLI_HDTV_RESOLUTION;
        HDMI_debug_progmem(c1920_1080p_60HZ);
        break;
    case TMDL_HDMIRX_VIDEORES_1080_1920p_60HZ:
        gColorSpaceManagement0.resolutionType = TMEX_APPLI_PC_RESOLUTION;
        HDMI_debug_progmem(c1080_1920p_60HZ);
        break;
    /* 60 Hz PC*/
    case TMDL_HDMIRX_VIDEORES_640_480p_60HZ:
        gColorSpaceManagement0.resolutionType = TMEX_APPLI_PC_RESOLUTION;
        HDMI_debug_progmem(c640_480p_60HZ);
        break;
    case TMDL_HDMIRX_VIDEORES_800_600p_60HZ:
        gColorSpaceManagement0.resolutionType = TMEX_APPLI_PC_RESOLUTION;
        HDMI_debug_progmem(c800_600p_60HZ);
        break;
    case TMDL_HDMIRX_VIDEORES_1024_768p_60HZ :
        gColorSpaceManagement0.resolutionType = TMEX_APPLI_PC_RESOLUTION;
        HDMI_debug_progmem(c1024_768p_60HZ);
        break;
    case TMDL_HDMIRX_VIDEORES_1280_768p_60HZ :
        gColorSpaceManagement0.resolutionType = TMEX_APPLI_PC_RESOLUTION;
        HDMI_debug_progmem(c1280_768p_60HZ);
        break;
    case TMDL_HDMIRX_VIDEORES_1280_960p_60HZ :
        gColorSpaceManagement0.resolutionType = TMEX_APPLI_PC_RESOLUTION;
        HDMI_debug_progmem(c1280_960p_60HZ);
        break;
    case TMDL_HDMIRX_VIDEORES_1280_1024p_60HZ :
        gColorSpaceManagement0.resolutionType = TMEX_APPLI_PC_RESOLUTION;
        HDMI_debug_progmem(c1280_1024p_60HZ);
        break;

    //case TMDL_HDMIRX_VIDEORES_1600_1024p_60HZ:
    //gColorSpaceManagement0.resolutionType = TMEX_APPLI_PC_RESOLUTION;
    //HDMI_debug_progmem(c1600_1024p_60HZ);
    //break;

    case TMDL_HDMIRX_VIDEORES_1600_1200p_60HZ:
        gColorSpaceManagement0 .resolutionType = TMEX_APPLI_PC_RESOLUTION;
        HDMI_debug_progmem(c1600_1200p_60HZ);
        break;
    case TMDL_HDMIRX_VIDEORES_1680_1050p_60HZ_RB:
        gColorSpaceManagement0.resolutionType = TMEX_APPLI_PC_RESOLUTION;
        HDMI_debug_progmem(c1680_1050p_60HZ_RB);
        break;
    case TMDL_HDMIRX_VIDEORES_1920_1200p_60HZ_RB:
        gColorSpaceManagement0.resolutionType = TMEX_APPLI_PC_RESOLUTION;
        HDMI_debug_progmem(c1920_1200p_60HZ_RB);
        break;
    case TMDL_HDMIRX_VIDEORES_1360_768p_60HZ:
        gColorSpaceManagement0.resolutionType = TMEX_APPLI_PC_RESOLUTION;
        HDMI_debug_progmem(c1360_768p_60HZ);
        break;
    case TMDL_HDMIRX_VIDEORES_1400_1050p_60HZ:
        gColorSpaceManagement0.resolutionType = TMEX_APPLI_PC_RESOLUTION;
        HDMI_debug_progmem(c1400_1050p_60HZ);
        break;
    case TMDL_HDMIRX_VIDEORES_1400_1050p_60HZ_RB:
        gColorSpaceManagement0.resolutionType = TMEX_APPLI_PC_RESOLUTION;
        HDMI_debug_progmem(c1400_1050p_60HZ_RB);
        break;

    default:
        HDMI_debug_progmem(cUnknownResolution);
        KnownResolution0=false;
        ResolutionID0=-1;
        break;
    }


    if ((resolutionID==TMDL_HDMIRX_VIDEORES_1280_720p_60HZ) || (resolutionID==TMDL_HDMIRX_VIDEORES_1280_1024p_60HZ) ||
#ifdef mode1024x768
            (resolutionID==TMDL_HDMIRX_VIDEORES_1024_768p_60HZ) ||
#endif
            (resolutionID==TMDL_HDMIRX_VIDEORES_1920_1080p_60HZ) ||
            /* (resolutionID==TMDL_HDMIRX_VIDEORES_1600_1024p_60HZ) || */ (resolutionID==TMDL_HDMIRX_VIDEORES_1680_1050p_60HZ_RB)) // || (resolutionID==TMDL_HDMIRX_VIDEORES_1600_1200p_60HZ))
    {

        KnownResolution0=true;
        ResolutionID0=resolutionID;



    }
    else if (resolutionID==TMDL_HDMIRX_VIDEORES_640_480p_60HZ)
    {
        KnownResolution0=true;
        ResolutionID0=resolutionID;

    }
    else
    {
        ResolutionID0=-1;
        KnownResolution0=false;
    }

}



void NXP_Video_On(void)

{
	
    // FPGA_reset();

    //if (HDMIShadow)
//			Lower_VideoB;
    _delay_ms(1);
}


void NXP_Video_Off(void)

{
}



/*============================================================================*/
/**
    \brief        Configure the conversion in the receiver

    \param        None

    \return     - TM_OK  : the call was successful
                - Any others: failed

 ******************************************************************************/

static tmErrorCode_t tmExAppliConfigureConversion0(void)
{
    
    HDMI_debug_progmem(cReceiverA);
    /* In this example application, the chosen output color space is RGB 4:4:4 */

    /* Choose the proper matrix depending on the input color space */
    switch (gColorSpaceManagement0.colorSpace)
    {
    case TMEX_APPLI_COLORSPACE_RGB:
        HDMI_debug_progmem(cRGBColorSpace);
        /* Input color space is RGB */
        /* As output color space is also RGB => no conversion to do */
        /* The blanking codes depend on the output color space => it will always be RGB in our case */
        NXP_Private_PRINTIF(tmdlHdmiRxSetColorConversionMatrix(gDlHdmiRxInstance0,
                TMDL_HDMIRX_CONVERSIONMATRIX_BYPASS,
                Null,
                &gRGBBlankingCode), __LINE__);

        if(gColorSpaceManagement0.resolutionType == TMEX_APPLI_PC_RESOLUTION)
        {
            HDMI_debug_progmem(cPCFormatRGBFullRange);
        }
        else
        {
            HDMI_debug_progmem(cTVFormatRGBLimitedRange);
        }
        break;

    case TMEX_APPLI_COLORSPACE_YCBCR_444:
    case TMEX_APPLI_COLORSPACE_YCBCR_422:
        /* Input color space is YCbCr */
        HDMI_debug_progmem(cYCBCRColorSpace);

        /* As output color space is RGB => a conversion must be made */

        /* The blanking codes depend on the output color space => it will always be RGB in our case */

        /* In this case, SW must check the colorimetry field of the AVI infoframe */
        switch(gColorSpaceManagement0.colorimetry)
        {
        case TMEX_APPLI_COLORIMETRY_NO_DATA:
            HDMI_debug_progmem(cNoColorimetryData);
        case TMEX_APPLI_COLORIMETRY_XVYCC:
            HDMI_debug_progmem(cXVYCCColorimetryData);
            /* The colorimetry is not specify, the conversion depends on the resolution: */
            /* YCbCr ITU709 for a HD resolution and YCbCr ITU601 for a SD resolution. */
            /* This case should not happen for a PC resolution because a PC format should */
            /* always be in RGB */
            if(gColorSpaceManagement0.resolutionType == TMEX_APPLI_HDTV_RESOLUTION)
            {
                HDMI_debug_progmem(cITU709); /* YCbCr ITU709 is used */
                NXP_Private_PRINTIF(tmdlHdmiRxSetColorConversionMatrix(gDlHdmiRxInstance0,
                        TMDL_HDMIRX_CONVERSIONMATRIX_CUSTOM,
                        &TMEX_APPLI_CONVERSIONMATRIX_YCbCr709_RGBLimited,
                        &gRGBBlankingCode), __LINE__);
            }
            else
            {
                HDMI_debug_progmem(cSMPTE170M);
                NXP_Private_PRINTIF(tmdlHdmiRxSetColorConversionMatrix(gDlHdmiRxInstance0,
                        TMEX_APPLI_CONVERSIONMATRIX_YCbCr601_RGBLimited,
                        Null,
                        &gRGBBlankingCode), __LINE__);
            }
            break;

        case TMEX_APPLI_COLORIMETRY_SMPTE170_ITU601:
            /* YCbCr ITU601 is used */
            HDMI_debug_progmem(cSMPTE170M);
            NXP_Private_PRINTIF(tmdlHdmiRxSetColorConversionMatrix(gDlHdmiRxInstance0,
                    TMEX_APPLI_CONVERSIONMATRIX_YCbCr601_RGBLimited,
                    Null,
                    &gRGBBlankingCode), __LINE__);
            break;

        case TMEX_APPLI_COLORIMETRY_ITU709:
            HDMI_debug_progmem(cITU709);
            NXP_Private_PRINTIF(tmdlHdmiRxSetColorConversionMatrix(gDlHdmiRxInstance0,
                    TMDL_HDMIRX_CONVERSIONMATRIX_CUSTOM,
                    &TMEX_APPLI_CONVERSIONMATRIX_YCbCr709_RGBLimited,
                    &gRGBBlankingCode), __LINE__);
            break;
        default:
            HDMI_debug_progmem(SwitchProblem);
        }
        break;
    default:
        HDMI_debug_progmem(SwitchProblem);
    }
    
    return TM_OK;
}






void NXP_Init_HDMI(void)

// performs initialization tasks for NXP

{


    //return;

#ifdef HDMI_DEBUG
    WriteLn("HDMI init");
#endif
    tmErrorCode_t errCode = TM_OK;

    Init_i2c();


    /* Initialize the HDMI component */
#ifndef No_NXP
    errCode = tmdlHdmiRxExampleAppliInit();
    HDMI_task=true;


    //HDMITaskDelay=10;

#endif
}



void NXP_HDMI_Task(void)
// periodically called to address HDMI task

{



    return; // this disables the HDMI task. Should be removed after debug

    tmErrorCode_t errCode = TM_OK;



    /* Check the status based on the task manager */

    /* Read the interruption status*/



    if (ioport_get_value(Int_HDMI_A)==false)
    {
        // determine source of interrupt
        /*

        the actual source of the INT pin is the INTERRUPT register in the asynchronous registers part of the TDA19971. You can read this register using the “CEC” I2C address of the chip that is 0x34 (instead of 0x48 for the “HDMI” I2C address), and the register is at address 0xFC.
        This is a 5bit register:
        Bit 4 is HPD int (HDMI output of TDA19973)
        Bit 3 is Util int   (HDMI output of TDA19973)
        Bit 2 is RSX int (HDMI output of TDA19973)
        Bit 1 is Hdmi    (corresponding to the TOP register)
        Bit 0 is CEC  int

        So if the INT pin signals an interrupt and TOP flag register is 0, that means that one of the 4 other interrupts sources is active. On a TDA19971, only CEC should be able to generate interrups.

        You can mask the unused interrupts sources using the associated 3bit ENABLE_INTERRUPT register at address 0xFD:
        Bit 2: enable HPD int
        Bit 1: enable Util int
        Bit 0: enable RSX int
        */

        // Select_LeftNXPComm;
        // todo: replicate this process for other NXP when using two receivers

        //uint8_t CECInterrupt=I2CReadRegister(0x68,0xFC);
        //sprintf(Msg,"CEC %x",CECInterrupt);
        //WriteLn(Msg);
        //uint8_t CECInterrupt;
        //errCode = tmdlHdmiRxDebugReadI2CRegister(0, 0x80, 0xFC, 1, &CECInterrupt);
        //NXP_Private_PRINTIF(errCode, __LINE__);
        //sprintf(Msg,"CEC %x",CECInterrupt);
        //WriteLn(Msg);
        //if ((CECInterrupt & 2)>0) // bit 1 is HDMI interrupt
        // *** end of previously-commented area
        {
#ifndef SkipNXP1
            errCode = tmdlHdmiRxHandleInterrupt(0);
            NXP_Private_PRINTIF(errCode, __LINE__);
#endif
        }
    }

    {
        {
            //errCode = tmdlHdmiRxHandleInterrupt(0); 
            //NXP_Private_PRINTIF(errCode, __LINE__);
        }
    }
    //else
    //{
    //WriteLn("No event");
    //	errCode = tmdlHdmiRxHandleInterrupt(0);
    //};




#ifndef SkipNXP1
    errCode = tmdlHdmiRxHeartBeat(0);
    NXP_Private_PRINTIF(errCode, __LINE__);
#endif




}


void NXP_Report_HDMI_status()

{

    if (KnownResolution0)
        WriteLn("Known res 0");

    if (ActivityDetected0)
        WriteLn("Activity 0");

    sprintf(Msg,"Res0 %d",ResolutionID0);
    WriteLn(Msg);

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
		
	#ifdef OSVRHDK
		if (ioport_get_pin_level(FPGA_unlocked))
			WriteLn("No Video detected");
		else
		{
			if (PortraitMode)
				WriteLn("Portrait mode video");
			else
				WriteLn("Landscape mode video");
		}
	#endif
};

uint8_t Get_HDMI_Status()
// returns byte showing HDMI status. This is used for reporting video mode in USB reports
{
	uint8_t Result=0;
	#ifdef OSVRHDK
	if (!ioport_get_pin_level(FPGA_unlocked))
	{
		Result+=1;
		if (PortraitMode)
			Result+=2;
	}
	#else
	if (PortraitMode)
		Result+=2;
	#endif
	return Result;
}

void NXP_Suspend(void)

{
    tmbslTDA1997XSetPowerState(gDlHdmiRxInstance0,tmPowerSuspend);
    tmbslTDA1997XConfigureEDID(gDlHdmiRxInstance0,BSLHDMIRX_EDID_NACK_OFF,BSLHDMIRX_EDID_RESET_OFF,BSLHDMIRX_EDID_PD_OFF,BSLHDMIRX_EDID_PD_DDC_OFF);
};

void NXP_Resume(void)

{
    tmbslTDA1997XSetPowerState(gDlHdmiRxInstance0,tmPowerOn);
    tmbslTDA1997XConfigureEDID(gDlHdmiRxInstance0,BSLHDMIRX_EDID_NACK_ON,BSLHDMIRX_EDID_RESET_ON,BSLHDMIRX_EDID_PD_ON,BSLHDMIRX_EDID_PD_DDC_ON);

};



void NXP_Debug(bool bothSides)

{
}



void NXP_HDMI_Reset(uint8_t HDMINum)

{
    if (HDMINum==1)
    {
        WriteLn("reset HDMI1");
        ioport_set_pin_low(NXP1_Reset_Pin);
        delay_ms(1);
        ioport_set_pin_high(NXP1_Reset_Pin);
    }
#ifndef OSVRHDK
    else if (HDMINum==2)
    {
        WriteLn("reset HDMI2");
        ioport_set_pin_low(NXP2_Reset_Pin);
        delay_ms(1);
        ioport_set_pin_high(NXP2_Reset_Pin);
    }
#endif
    else
        WriteLn("Wrong HDMI num");
}


tmErrorCode_t
bslTDA1997XWriteI2C
(
    tmUnitSelect_t          unit,
    UInt16                  firstRegister,
    UInt8                   lengthData,
    UInt8                   *pBuffer
); // definition for I2C routine


void NXP_Program_MTP0(void)

// program the MTP for the NXP

{
    UInt8 NewData=0x02;

    // clear the interrupt flags
    NewData=0x80;
    tmErrorCode_t result = bslTDA1997XWriteI2C(0, INT_FLG_CLR_DDC, 1, &NewData);
    sprintf(Msg,"Write result: %x",result);
    WriteLn(Msg);

    NewData=0x02;
    result = bslTDA1997XWriteI2C(0,EMTP_CTRL,1,&NewData);
    sprintf(Msg,"Write result: %x",result);
    WriteLn(Msg);
    _delay_ms(10000);

    /* Wait interrupt flag */
    uint32_t timeout;
    timeout = 0;
    tmErrorCode_t   errCode = TM_OK;
    uint8_t regVal;

    do
    {
        errCode = tmbslTDA1997XReadI2C(0, INT_FLG_CLR_DDC, 1, &regVal);
        //if (errCode) return TM_ERR_BAD_PARAMETER;
        _delay_ms(200);
        //RxHdmiConfig[0].sysFuncTimer(200);
        timeout += 200;
    }
    while ((!(regVal & 0x80)) && (timeout < 5000));
    if (timeout == 5000)
    {
        WriteLn("/!\\Write E-MTP Error: Timeout of waiting E-MTP flag\n");
        //return TM_ERR_BAD_PARAMETER;
    }

    /* Clear interrupt flag */
    regVal = 0x80;
    errCode = tmbslTDA1997XWriteI2C(0, INT_FLG_CLR_SUS, 1, &regVal);
    //if (errCode) return TM_ERR_BAD_PARAMETER;


    result = bslTDA1997XWriteI2C(0,CMTP_CTRL,1,&NewData);
    sprintf(Msg,"Write result: %x",result);
    WriteLn(Msg);
    _delay_ms(10000);



    /* Wait interrupt flag */
    timeout = 0;
    do
    {
        errCode = tmbslTDA1997XReadI2C(0, INT_FLG_CLR_SUS, 1, &regVal);
        //if (errCode) return TM_ERR_BAD_PARAMETER;
        _delay_ms(200);
        //RxHdmiConfig[0].sysFuncTimer(200);
        timeout += 200;
    }
    while ((!(regVal & 0x80)) && (timeout < 5000));
    if (timeout == 5000)
    {
        WriteLn("/!\\Write C-MTP Error: Timeout of waiting C-MTP flag\n");
        //return TM_ERR_BAD_PARAMETER;
    }

    /* Clear interrupt flag */
    regVal = 0x80;
    errCode = tmbslTDA1997XWriteI2C(0,INT_FLG_CLR_SUS, 1, &regVal);
    //if (errCode) return TM_ERR_BAD_PARAMETER;

}

void NXP_Program_MTP1(void)

// program the MTP for the NXP

{
    UInt8 NewData=0x02;

    // clear the interrupt flags
    NewData=0x80;
    tmErrorCode_t result = bslTDA1997XWriteI2C(1, INT_FLG_CLR_DDC, 1, &NewData);
    sprintf(Msg,"Write result: %x",result);
    WriteLn(Msg);

    NewData=0x02;
    result = bslTDA1997XWriteI2C(1,EMTP_CTRL,1,&NewData);
    sprintf(Msg,"Write result: %x",result);
    WriteLn(Msg);
    _delay_ms(10000);

    uint32_t timeout;

    /* Wait interrupt flag */
    timeout = 0;
    tmErrorCode_t   errCode = TM_OK;
    uint8_t regVal;

    do
    {
        errCode = tmbslTDA1997XReadI2C(1, INT_FLG_CLR_DDC, 1, &regVal);
        //if (errCode) return TM_ERR_BAD_PARAMETER;
        _delay_ms(200);
        //RxHdmiConfig[0].sysFuncTimer(200);
        timeout += 200;
    }
    while ((!(regVal & 0x80)) && (timeout < 5000));
    if (timeout == 5000)
    {
        WriteLn("/!\\Write E-MTP Error: Timeout of waiting E-MTP flag\n");
        //return TM_ERR_BAD_PARAMETER;
    }

    /* Clear interrupt flag */
    regVal = 0x80;
    errCode = tmbslTDA1997XWriteI2C(1, INT_FLG_CLR_SUS, 1, &regVal);
    //if (errCode) return TM_ERR_BAD_PARAMETER;

    result = bslTDA1997XWriteI2C(1,CMTP_CTRL,1,&NewData);
    sprintf(Msg,"Write result: %x",result);
    WriteLn(Msg);
    _delay_ms(10000);


    /* Wait interrupt flag */
    timeout = 0;
    do
    {
        errCode = tmbslTDA1997XReadI2C(1, INT_FLG_CLR_SUS, 1, &regVal);
        //if (errCode) return TM_ERR_BAD_PARAMETER;
        _delay_ms(200);
        //RxHdmiConfig[0].sysFuncTimer(200);
        timeout += 200;
    }
    while ((!(regVal & 0x80)) && (timeout < 5000));
    if (timeout == 5000)
    {
        WriteLn("/!\\Write C-MTP Error: Timeout of waiting C-MTP flag\n");
        //return TM_ERR_BAD_PARAMETER;
    }

    /* Clear interrupt flag */
    regVal = 0x80;
    errCode = tmbslTDA1997XWriteI2C(1,INT_FLG_CLR_SUS, 1, &regVal);
    //if (errCode) return TM_ERR_BAD_PARAMETER;

}

// uses NXP to determine if we are in portrait or landscape mode and then updates USB report
void NXP_Update_Resolution_Detection()

{
	tmbslHdmiRxAsdMeasureInterlaced_t   pInterlaced;
	tmbslHdmiRxVhrefAsdLineStandard_t   pLineMatch;
	tmbslHdmiRxVhrefAsdMeaslin525_t     pFrameFormat;
	UInt16                              pLines;
	UInt16                              pPixels;

	tmErrorCode_t               errCode;
	tmInstance_t             instance=0;

	errCode = tmbslTDA1997XGetFrameMeasurements(instance,&pInterlaced,&pLineMatch,&pFrameFormat,&pLines,&pPixels);
	HDMIStatus=Get_HDMI_Status();
}

