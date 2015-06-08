/* ============================================================================
 *
 * Copyright (c) 2010-2012 Hillcrest Laboratories, Inc.  All rights reserved.
 * HILLCREST LABORATORIES, INC. PROPRIETARY/CONFIDENTIAL
 *
 * ============================================================================*/

#ifndef SENSORHUB_PLATFORM_H
#define SENSORHUB_PLATFORM_H

/*
 * This file includes stdint.h, stdbool.h, and stdlib.h or
 * defines equivalent types and definitions for platforms
 * that don't include them.
 */

#if (defined __C51__)
/* Keil C51 compiler is missing stdint.h */
typedef unsigned char uint8_t;
typedef signed char int_fast8_t;
typedef signed char int8_t;
typedef unsigned char uint_fast8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed short int_fast16_t;
typedef unsigned short uint_fast16_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;

#define INT16_MAX (32767)
#define UINT16_MAX (65535)
#define INT16_MIN (-32768)
#define INT32_MIN (-2147483648)
#define INT32_MAX (2147483647)

#define lrintf(x) ((x) < 0 ? ((int) ((x) - 0.5f)) : ((int) ((x) + 0.5f)))
#define fminf(x, y) ((x) < (y) ? (x) : (y))
#define fmaxf(x, y) ((x) < (y) ? (y) : (x))

/* Keil doesn't have fsqrt, but doubles are 4-bytes just
   like floats with the C51. */
#define sqrtf(x) sqrt(x)
#define fabsf(x) fabs(x)

#define FAR_DATA

#elif defined (__ICC8051__)

/* If the IAR compiler is setup to use DLIB, then stdint.h is available */
/* #include <stdint.h> */

/* Assume CLIB and no stdint.h */
typedef __INT8_T_TYPE__ int8_t;
typedef __UINT8_T_TYPE__ uint8_t;
typedef __INT16_T_TYPE__ int16_t;
typedef __UINT16_T_TYPE__ uint16_t;
typedef __INT32_T_TYPE__ int32_t;
typedef __UINT32_T_TYPE__ uint32_t;
typedef __INT_FAST8_T_TYPE__ int_fast8_t;
typedef __UINT_FAST8_T_TYPE__ uint_fast8_t;
typedef __INT_FAST16_T_TYPE__ int_fast16_t;
typedef __UINT_FAST16_T_TYPE__ uint_fast16_t;

#define INT16_MAX   __INT16_T_MAX__
#define INT16_MIN   __INT16_T_MIN__
#define UINT16_MAX __UINT16_T_MAX__
#define INT32_MAX   __INT32_T_MAX__
#define INT32_MIN   __INT32_T_MIN__

/* Define several C functions not found in CLIB */
#define lrintf(x) ((x) < 0 ? ((int) ((x) - 0.5f)) : ((int) ((x) + 0.5f)))
#define fminf(x, y) ((x) < (y) ? (x) : (y))
#define fmaxf(x, y) ((x) < (y) ? (y) : (x))
#define sqrtf(x) sqrt(x)
#define fabsf(x) fabs(x)

/* Arrays and structures larger than 255 bytes need to be in __xdata */
#define FAR_DATA __xdata

#else
#include <stdint.h>
#define FAR_DATA
#endif

#include <stdlib.h>

#ifndef __cplusplus
#if defined(__bool_true_false_are_defined) && __bool_true_false_are_defined != 0
/* bool is already defined, no need to do anything! */

#elif defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L

#include <stdbool.h>

#elif defined (__C51__)

/* Keil has doesn't need stdbool.h and doesn't like*/
/*typedef uint8_t bool;*/
typedef uint8_t _Bool;
typedef _Bool bool;
#define true 1
#define false 0
#ifndef FALSE
#define FALSE false
#endif
#ifndef TRUE
#define TRUE true
#endif
#define __bool_true_false_are_defined 1

#else
/* Hack C99 boolean support for non-compliant platforms
 * Windows (http://en.wikipedia.org/wiki/Stdbool.h)
 * http://msdn.microsoft.com/en-us/library/tf4dy80a.aspx
 * <stdbool.h> is not included in all toolchains so using it is problematic.
 * For example, Rowley's v1.7b22 does not have it.
 */

#if defined(FME_PLATFORM_WIN32)
typedef uint8_t _Bool;
#endif
typedef _Bool bool;
#define true 1
#define false 0
#ifndef FALSE
#define FALSE false
#endif
#ifndef TRUE
#define TRUE true
#endif
#define __bool_true_false_are_defined 1

#endif
#endif

#endif                          // SENSORHUB_PLATFORM_H
