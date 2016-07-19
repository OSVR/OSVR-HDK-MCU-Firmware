#include "GlobalOptions.h"
#include "NXP_AVR_Internal.h"


/**
 * Copyright (C) 2006 NXP N.V., All Rights Reserved.
 * Exposed under Apache 2.0 Open Source License with permission of NXP N.V.
 *
 * \file          tmbslTDA1997X_local.h
 *
 * \version       $Revision: 1 $
 *
 * \date          $Date: 22/11/07 10:30 $
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

   $History: tmbslTDA1997X_local.h $
 *

   \endverbatim
 *
*/

#ifndef TMBSLTDA1997X_LOCAL_H
#define TMBSLTDA1997X_LOCAL_H

#define ACC_REG(a,p)            (UInt16)(((p)<<8)|(a))
//#define RETIF(cond, rslt) {if ((cond)){return (rslt);} }
#define RETIF(cond, rslt)       if ((cond)) \
	{NXP_Private_PRINTIF(1998,__LINE__); \
	return (rslt);\
	}

#define MAX_UNIT                2

/*-------------------------------------------------------*/
/*               BSL Software Version                    */
/*-------------------------------------------------------*/
/* The values of the defines for software version must not
   have a leading zero. Otherwise, it is interpreted as an
   octal constant */

#define COMPATIBILITY_BSL 0
#define MAJOR_VERSION_BSL 1
#define MINOR_VERSION_BSL 4

/*-------------------------------------------------------*/
/*                    VAI registers                      */
/*-------------------------------------------------------*/
#define NBR_REG_INT_FLAGS       9
#define NBR_BIT_INT_FLAGS       8
#define NBR_REG_VAI_TO_READ     7
#define NBR_REG_VAI_123         3
#define NBR_REG_VAI_567         3
#define NBR_REG_VAI_56          2
#define INDEX_REG_VAI_7         5

/*-------------------------------------------------------*/
/*                    Supported ICs                      */
/*-------------------------------------------------------*/

#define TDA19972A_CORE_ID		0xC0U
#define TDA19972B_CORE_ID		0xD0U
#define TDA19971_CONFIG_ID     	0x71U
#define TDA19972_CONFIG_ID     	0x72U
#define TDA19973_CONFIG_ID     	0x73U

/*-------------------------------------------------------*/
/*             VAI_FLAGS_CTRL default value              */
/*-------------------------------------------------------*/
#define VAI_FLAGS_CTRL_DEF_VAL  0x0B

/*-------------------------------------------------------*/
/*               Masks for VAI registers                 */
/*-------------------------------------------------------*/
/* The masks depend on the available IRQ sources on the different chips */

/* TDA19972 (common masks)
#define INTERRUPT_MASK_DEF_VAI1_7X        0x88U	otp, sus5
#define INTERRUPT_MASK_DEF_VAI2_7X        0xFFU	(hdmi)flag, isrc2, isrc1, acp, avi_if, spd_if, aud_if, mps_if
#define INTERRUPT_MASK_DEF_VAI3_7X        0x9EU	r5(c5), dc_mode, gamut, audio_flg, mute_flg
#define INTERRUPT_MASK_DEF_VAI5_7X        0x44U	rate_b_st, rate_a_st
#define INTERRUPT_MASK_DEF_VAI7_7X        0x02U unmute_fifo
*/
/* TDA19978/74
#define INTERRUPT_MASK_DEF_VAI6_78_74     0x44U
*/

#define INTERRUPT_MASK_DEF_TOP			0x7FU	/* hdcp, audio, info, mode, rate, ddc, sus */
#define INTERRUPT_MASK_DEF_SUS			0x88U	/* config_mtp, sus_end */
#define INTERRUPT_MASK_DEF_DDC			0x00U
#define INTERRUPT_MASK_DEF_RATE			0x44U	/* rate_b_st, rate_a_st */
#define INTERRUPT_MASK_DEF_MODE			0xB9U	/* hdmi_flg, isrc2, isrc1, acp, dc_mode */
#define INTERRUPT_MASK_DEF_INFO			0x78U	/* mps_if, aud_if, spd_if, avi_if */
#define INTERRUPT_MASK_DEF_AUDIO        0x1FU	/* audio_flg, mute_flg, ch stat, unmute_fifo, fifo_err */
//#define INTERRUPT_MASK_DEF_AUDIO        0x1EU	/* audio_flg, mute_flg, ch stat, unmute_fifo */
#define INTERRUPT_MASK_DEF_HDCP			0x02U	/* state_c5 */
#define INTERRUPT_MASK_DEF_AFE			0x00U

#define INTERRUPT_MASK_DEF_UTILRXSHPD	0x1CU
#define INTERRUPT_MASK_DEF_UTILHPD	    0xC0U
#define INTERRUPT_ENA_UTILRXSHPD		0x07U


#define INTERRUPT_SUS_IDX				0x00U
#define INTERRUPT_DDC_IDX				0x01U
#define INTERRUPT_RATE_IDX				0x02U
#define INTERRUPT_MODE_IDX				0x03U
#define INTERRUPT_INFO_IDX				0x04U
#define INTERRUPT_AUDIO_IDX				0x05U
#define INTERRUPT_HDCP_IDX				0x06U
#define INTERRUPT_AFE_IDX				0x07U
#define INTERRUPT_CEC_IDX				0x08U

#define INTERRUPT_SUS					(0x01 << INTERRUPT_SUS_IDX)
#define INTERRUPT_DDC					(0x01 << INTERRUPT_DDC_IDX)
#define INTERRUPT_RATE					(0x01 << INTERRUPT_RATE_IDX)
#define INTERRUPT_MODE					(0x01 << INTERRUPT_MODE_IDX)
#define INTERRUPT_INFO					(0x01 << INTERRUPT_INFO_IDX)
#define INTERRUPT_AUDIO					(0x01 << INTERRUPT_AUDIO_IDX)
#define INTERRUPT_HDCP					(0x01 << INTERRUPT_HDCP_IDX)
#define INTERRUPT_AFE					(0x01 << INTERRUPT_AFE_IDX)


/* Value used to clear the interrupts */
#define INTERRUPT_CLEAR_VAL               0xFFU

/* Number of INT_FLG_CLR registers */
#ifdef TMFL_HDMI_OUT
    #define INTERRUPT_CLR_FLG_REG_NUM		0x09U
#else
    #define INTERRUPT_CLR_FLG_REG_NUM		0x08U
#endif


/*-------------------------------------------------------*/
/*        Masks for status registers for interrupt       */
/*-------------------------------------------------------*/
#define MASK_SUS_STATE_VALUE    0x1FU
#define LAST_STATE_REACHED      0x1BU
#define MASK_CLK_STABLE         0x04U
#define MASK_CLK_ACTIVE         0x02U

#define MASK_RXDA_BIT           0x01U
#define MASK_RXDB_BIT           0x02U
#define MASK_RXDC_BIT           0x04U
#define MASK_RXDD_BIT           0x08U
#define MASK_SUS_STATE_BIT      0x10U

#define MASK_MPT_BIT			0x80U
#define MASK_SUS_END_BIT      	0x08U
#define MASK_SR_FIFO_FIFO_CTRL  0x30U
#define MASK_FIFO_ERR			0x01U
#define MASK_MUTE_FLAG         0x08U
#define MASK_CHAN_STATUS        0x04U
#define MASK_AUDIO_FLAG         0x10U

/*-------------------------------------------------------*/
/*                      Page register                    */
/*-------------------------------------------------------*/
typedef enum
{
    E_PAGE_00               = 0x00,
    E_PAGE_01               = 0x01,
    E_PAGE_02               = 0x02,
    E_PAGE_12               = 0x12,
    E_PAGE_13               = 0x13,
    E_PAGE_14               = 0x14,
    E_PAGE_20               = 0x20,
    E_PAGE_21               = 0x21,
    E_PAGE_22               = 0x22,
    E_PAGE_23               = 0x23,
    E_PAGE_30               = 0x30,
    E_PAGE_40               = 0x40,
    E_PAGE_41               = 0x41,
    E_PAGE_42               = 0x42,
    E_PAGE_CEC				= 0x80,
    E_PAGE_INVALID          = 0xFF          /* Index value indicating invalid page */
} tmbslTDA1997XPage_t_old;

typedef uint8_t tmbslTDA1997XPage_t; //  added by Sensics

/*-------------------------------------------------------*/
/*                      Page register                    */
/*-------------------------------------------------------*/
typedef struct
{
    Bool                        	initUnit;
    tmbslTDA1997XPage_t         	currentPage;        /* The current page address */
    UInt8                       	uHwAddress;         /* The I2C slave address */
    UInt8                       	uCECAddress;        /* The CEC slave address */
    ptmbslHdmiRxSysFunc_t       	sysFuncWrite;
    ptmbslHdmiRxSysFunc_t       	sysFuncRead;
    ptmbslHdmiRxCallbackFunc_t  	callbackFunc;
    ptmbslHdmiRxSysFuncTimer_t  	sysFuncTimer;
    tmbslHdmiRxVersion_t        	version;            /* Chip version */
    tmbslHdmiChipConfiguration_t	chipConfiguration;
    UInt8                       	writePageErrorFlag;
    UInt8                       	currentAudioFreq;
    Bool							MPTRWInProgress;
    /* WORKAROUND START: PR1296 */
    UInt8                       	audioTestModeUsed;
    /* WORKAROUND END: PR1296 */
    UInt8                       	hdmiFlagsStatus;
    Bool							VSIReceived;
    /* WORKAROUND START: PR1633 */
#ifdef TMFL_CALIBRATION_OPT
    Bool                        susResetBySW;
#endif
    /* WORKAROUND END: PR1633 */
} tmbslTDA1997XConfigUnit_t;

/*-------------------------------------------------------*/
/*                     RT Control                        */
/*-------------------------------------------------------*/
#define CONTROL_RTA 					0x01U
#define CONTROL_RTB 					0x02U


/*-------------------------------------------------------*/
/*                     SUS                               */
/*-------------------------------------------------------*/
#define SUS_RESET_ON 					0x80U
#define SUS_RESET_OFF 					0x00U


/*-------------------------------------------------------*/
/*                     Audio Fifo                        */
/*-------------------------------------------------------*/
//#define RESET_AUDIO_FIFO                0x21U
// try not to reset audio (only FIFO) as it reset also infoframe
#define RESET_AUDIO_FIFO_CTRL           0x20U
#define RESET_AUDIO_FIFO                0x10U
#define MASK_FIFO_FAIL                  0x02U

/* WORKAROUND START: PR1296 */
#define AUDIO_FREQ_NB                   7
#define MASK_AUDIO_FREQ_P13             0x0EU
#define TEST_AUDIO_FREQ_CONST           0x10U
/* WORKAROUND END: PR1296 */

/* WORKAROUND START: PR1308 */
#define MASK_LAYOUT_FLAG                0x04U
#define FIFO_LAT_CTRL_DEF_VAL           0x06U
/* WORKAROUND END: PR1308 */

/* WORKAROUND START: PR1359 */
#define WL_CFG_HW_MODE                  0xBCU
#define WL_CFG_SW_MODE                  0x82U
/* WORKAROUND END: PR1359 */

/* WORKAROUND START: PR1633 */
#define GAIN_MODE_MANUAL                0x01U
#define GAIN_MODE_AUTO                  0x00U
/* WORKAROUND END: PR1633 */

/* WORKAROUND START: PR1255 */
#define HDCP_BCAPS_READY_BIT            0x20U
#define HDMI_FLAGS_HDMI_MODE            0x40
/* WORKAROUND END: PR1255 */

/*-------------------------------------------------------*/
/*              Input selection register                 */
/*-------------------------------------------------------*/
#define RESET_VDP					0x04
#define RESET_FTM					0x80

#define MASK_DIG_INPUT         		0x01
#define MASK_DIG_INPUT_VDPR_FMTR	0x85
#define MASK_HDMIOUTMODE			0x02

#define SELECT_INPUT_A				0x00
#define SELECT_INPUT_B				0x01

/*-------------------------------------------------------*/
/*            Pixel clock generation register            */
/*-------------------------------------------------------*/
#define MASK_CLK_DELAY          0x70
#define MASK_CLK_SEL			0x03
#define MAX_VAL_CLK_DEL         0x07U
#define PIX_CLOCK				0x00
#define PIX_CLOCK_X2			0x01
#define PIX_CLOCK_DIV2			0x02
#define PIX_CLOCK_DIV4 			0x03

/*-------------------------------------------------------*/
/*                  Sync timing register                 */
/*-------------------------------------------------------*/
#define MASK_VPER_MSB           0x3F
#define MASK_HPER_MSB           0x0F
#define MASK_HSWIDTH_MSB        0x03

/*-------------------------------------------------------*/
/*           Color Space conversion register             */
/*-------------------------------------------------------*/
#define MAT_OFFSET_NB           3
#define MAT_COEFF_NB            9
#define OFFSET_LOOP_NB          2
#define MIN_VAL_OFFSET          -4096
#define MAX_VAL_OFFSET          4095
#define MIN_VAL_COEFF           -16384
#define MAX_VAL_COEFF           16383
#define MASK_MAT_COEFF_LSB      0x00FF
#define MASK_MAT_BP				0x01

/*-------------------------------------------------------*/
/*              Pixel and line counters register         */
/*-------------------------------------------------------*/
#define MAX_COUNTER             0x3FFF
#define MASK_COUNT_LSB          0x00FF
#define MASK_COUNT_MSB          0x3F00

/*-------------------------------------------------------*/
/*               VHREF and VHSync registers              */
/*-------------------------------------------------------*/
#define MAX_REF_START_END       0x3FFF
#define MASK_REF_START_END_LSB  0x00FF
#define MASK_REF_START_END_MSB  0x3F00
#define MAX_REF_FI_START        0x3FFF
#define MASK_REF_FI_START_MSB   0x3F00
#define MASK_REF_FI_START_LSB   0x00FF
#define MASK_IHS_SEL			0x01

/*-------------------------------------------------------*/
/*                Frame detection registers              */
/*-------------------------------------------------------*/
#define MAX_VAL_FDW             0x3FFF
#define MASK_FDW_MSB            0x3F00
#define MASK_FDW_LSB            0x00FF

/*-------------------------------------------------------*/
/*              Frame measurement registers              */
/*-------------------------------------------------------*/
#define MASK_INTERLACE_TYPE     0x80
#define MASK_LINE_MATCH         0x40
#define MASK_FRAME_FORMAT       0x20
#define MASK_NB_LINE            0x3F
#define MASK_NB_PIX             0x3F

/*-------------------------------------------------------*/
/*                  Sync delay register                  */
/*-------------------------------------------------------*/
#define MAX_VS_DEL              0x0F
#define MAX_HS_DEL              0x0F
#define MASK_VS_DEL             0x0F
#define MASK_HS_DEL             0x0F

/*-------------------------------------------------------*/
/*                  Blanking code register               */
/*-------------------------------------------------------*/
#define MAX_BLK_CODE            0xFFF
#define MASK_BLK_CODE_LSB       0xFF

/*-------------------------------------------------------*/
/*           Up/downsample control registers             */
/*-------------------------------------------------------*/
#define MASK_UPDOWN_SEL         0x0F
#define MASK_UP_SEL         	0x30

/*-------------------------------------------------------*/
/*               Range control registers                 */
/*-------------------------------------------------------*/
#define MASK_CEIL_FLOOR_LSB     0xFF

/*-------------------------------------------------------*/
/*               Output formatter register               */
/*-------------------------------------------------------*/
#define MASK_BLK_CODE_OF_CTRL   0x10
#define MASK_TMG_REF_OF_CTRL    0x08

/*-------------------------------------------------------*/
/*            Internal sync selection register           */
/*-------------------------------------------------------*/
#define MASK_IHS_SEL            0x01
#define MASK_I_HSYNC            0x01

/*-------------------------------------------------------*/
/*             Sync output selection register            */
/*-------------------------------------------------------*/
#define MASK_CS_SEL             0x40
#define MASK_VS_SEL             0x03
#define MASK_HS_SEL             0x03
/*#define HREF_DELAY_FIXED        0x04
#define VREF_DELAY_FIXED        0x20*/

/*-------------------------------------------------------*/
/*           Output polarity control register            */
/*-------------------------------------------------------*/
#define MASK_CLK_TOG            0x04
#define MASK_POL            	0x04
#define MASK_PXQ				0x08

/*-------------------------------------------------------*/
/*          Data enable signal control register          */
/*-------------------------------------------------------*/
#define MAX_VAL_DE_DEL          0x0F

/*-------------------------------------------------------*/
/*                  MCK signal register                  */
/*-------------------------------------------------------*/
#define MCLK_27MHZ_DEFVAL        0x01

/*-------------------------------------------------------*/
/*           Audio selection/formatter registers         */
/*-------------------------------------------------------*/
#define MAX_VAL_CH_ASGT          0x1F
#define MASK_AUDIO_CLK_INV       0x80
#define MASK_TEST_TONE			 0x40
#define MASK_I2S_SPDIF			 0x20
#define MASK_I2S_16_32			 0x10
#define MASK_AUDIO_AUTO_MUTE     0x08
#define	MASK_HBR_DEMUX			 0x04
#define MASK_AUDIO_TYPE			 0x03

/*-------------------------------------------------------*/
/*                Pixel repetition registers             */
/*-------------------------------------------------------*/
#define MAX_VAL_PIX_REPEAT       0x09
#define PIX_REPEAT_DEF_VAL       0x90
#define MASK_PIX_REP			 0x0F

/*-------------------------------------------------------*/
/*                  OTP memory registers                 */
/*-------------------------------------------------------*/
#define MAX_VAL_OTP_ADDR         0x7F

/*-------------------------------------------------------*/
/*               HDCP control register                   */
/*-------------------------------------------------------*/
#define MASK_KEYS_DESC           0x00FF

/*-------------------------------------------------------*/
/*                HDCP Caps register                     */
/*-------------------------------------------------------*/
#define DEF_VAL_CAPS             0x80

/*-------------------------------------------------------*/
/*               HDCP BStatus registers                  */
/*-------------------------------------------------------*/
#define MAX_VAL_DEVICE_COUNT     0x7F
#define MAX_VAL_DEPTH            0x07

/*-------------------------------------------------------*/
/*           HDCP repeater control register              */
/*-------------------------------------------------------*/
#define K_DP_UNAUTH             0x04
#define K_WD_END                0x02
#define K_KSV_READY             0x01
#define MASK_AUTO_RESET         0x10

/*-------------------------------------------------------*/
/*            HDCP Error protection registers            */
/*-------------------------------------------------------*/
#define MAX_VAL_DELOCK_DEL      0x07

/*-------------------------------------------------------*/
/*                 Infoframe registers                   */
/*-------------------------------------------------------*/
#define MASK_IF_LENGTH          0x1F
#define MAX_VAL_IF_NB_DATA      32
#define AVI_IF_NB_DATA          17
#define SPD_IF_NB_DATA          31
#define VS_IF_NB_DATA           31
#define AUD_IF_NB_DATA          14
#define MPS_IF_NB_DATA          14
#define IF_HEAD_CHECK_LENGTH    4

/*-------------------------------------------------------*/
/*                 ACP Packet registers                  */
/*-------------------------------------------------------*/
#define ACP_PKT_HEADER_LENGTH   3
#define ACP_PKT_DATA_LENGTH     16

/*-------------------------------------------------------*/
/*                ISRC Packet registers                 */
/*-------------------------------------------------------*/
#define ISRC_PKT_HEADER_LENGTH   3
#define ISRC_PKT_DATA_LENGTH     16

/*-------------------------------------------------------*/
/*           Gamut Metadata packet registers             */
/*-------------------------------------------------------*/
#define GAMUT_PKT_HEADER_LENGTH   3
#define GAMUT_PKT_DATA_LENGTH     28

/*-------------------------------------------------------*/
/*          General Control Packet registers             */
/*-------------------------------------------------------*/
#define GCP_HEADER_LENGTH   3
#define GCP_DATA_LENGTH     7

/*-------------------------------------------------------*/
/*          Deep Color PLL control registers             */
/*-------------------------------------------------------*/
#define DEEP_PLL1_CCIR_MODE_ON    0x07
#define DEEP_PLL1_CCIR_MODE_OFF   0x06

/*-------------------------------------------------------*/
/*              Audio frequency registers                */
/*-------------------------------------------------------*/
#define MASK_AUDIO_DST_RATE       0x80U
#define MASK_AUDIO_FREQ           0x07U

/*-------------------------------------------------------*/
/*              Deep color mode registers                */
/*-------------------------------------------------------*/
#define MASK_DC_COLOR_DEPTH       0x0FU
#define MASK_DC_PIXEL_PHASE       0xF0U

/*-------------------------------------------------------*/
/*                 HDMI flags register                   */
/*-------------------------------------------------------*/
#define MASK_HDMI_FLAGS           0x7CU

/*-------------------------------------------------------*/
/*                Power mode management                  */
/*-------------------------------------------------------*/
#define MASK_OF_CTRL_OUT_HIZ      0x80U
#define MASK_AUDIO_PLL_PD         0x80U
#define DC_PLL_PD                 0x01U
#define DC_PLL_PON                0x00U
#define MASK_XTAL_OSC_PD          0x02U
#define MASK_TMDS_CLK_DIS         0x08U
#define CBIAS_PON                 0x01U
#define CBIAS_POFF                0x00U
#define TMDS_AUTO_PON             0x00U
#define TMDS_MAN_PON              0x01U
#define MASK_LOW_PW_EDID          0x01U

/*-------------------------------------------------------*/
/*                     Test pattern                      */
/*-------------------------------------------------------*/

#define CLKX2_XTAL				0x00U
#define CLKX2_XTAL_DIV2			0x40U
#define CLKX2_HDMIX2			0xC0U

#define CLK_XTAL				0x00U
#define CLK_XTAL_DIV2			0x10U
#define CLK_HDMI				0x30U

#define STD_NTSC				0x00U
#define STD_PAL					0x04U

#define INTPRO_INT				0x00U
#define INTPRO_PROG				0x02U

#define COLOR_BAR				0x00U
#define RAMP					0x08U

#define SM_OFF					0x00U
#define SM_ON					0x01U

/* to be checked */
#define HDMICLOCK_NORMAL          0x00U
#define HDMICLOCK_MCLK            0x08U
#define PIXCLOCK_HDMICLOCK        0x80U
#define TIMERS_CONTROL_CB_VAL     0x36U
#define HS_VS_FROM_VHREF          0x00U
#define HS_VS_FROM_HDMI           0x12U
#define SYNC_FROM_VHREF		 	  0x00U
#define REF_FROM_VHREF		 	  0x01U
#define SYNC_FROM_HDMI		 	  0x02U
#define VHREF_ASD_SEL_480P        0x08U
#define VHREF_ASD_SEL_576P        0x00U
#define VHREF_ASD_SEL_OFF         0x00U
#define SUS_SEL_MAN               0xFFU
#define SUS_SEL_AUTO              0x00U
#define INPUT_SEL_AUTO            0x10
#define POL_CTRL_CS_HS_VS_NEG     0x38U
#define SYNC_POL_NEG			  0x04U
#define SYNC_POL_POS			  0x00U

/* Hs and Vs values */
#define HS_S_LSB_480P_VAL         0x01U
#define HS_MSB_480P_VAL           0x00U
#define HS_E_LSB_480P_VAL         0x3FU
#define VS_F1_LINE_S_MSB_480P_VAL 0x00U
#define VS_F1_LINE_S_LSB_480P_VAL 0x07U
#define VS_F1_LINE_WIDTH_480P_VAL 0x06U

#define HS_S_LSB_576P_VAL         0x01U
#define HS_MSB_576P_VAL           0x00U
#define HS_E_LSB_576P_VAL         0x41U
#define VS_F1_LINE_S_MSB_576P_VAL 0x00U
#define VS_F1_LINE_S_LSB_576P_VAL 0x01U
#define VS_F1_LINE_WIDTH_576P_VAL 0x05U

#define VS_F2_LINE_S_MSB_VAL      0x00U
#define VS_F2_LINE_S_LSB_VAL      0x00U
#define VS_F2_LINE_WIDTH_VAL      0x00U
#define VS_F1_PIX_S_LSB_VAL       0x01U
#define VS_F1_PIX_MSB_VAL         0x00U
#define VS_F1_PIX_E_LSB_VAL       0x01U
#define VS_F2_PIX_S_LSB_VAL       0x00U
#define VS_F2_PIX_MSB_VAL         0x00U
#define VS_F2_PIX_E_LSB_VAL       0x00U

/* Pixel and line counter values */
#define PXCNT_PR_LSB_480P_VAL     0x01U
#define PXCNT_MSB_480P_VAL        0x03U
#define PXCNT_NPIX_LSB_480P_VAL   0x5AU
#define LCNT_PR_LSB_480P_VAL      0x01U
#define LCNT_MSB_480P_VAL         0x02U
#define LCNT_NLIN_LSB_480P_VAL    0x0DU

#define PXCNT_PR_LSB_576P_VAL     0x01U
#define PXCNT_MSB_576P_VAL        0x03U
#define PXCNT_NPIX_LSB_576P_VAL   0x60U
#define LCNT_PR_LSB_576P_VAL      0x01U
#define LCNT_MSB_576P_VAL         0x02U
#define LCNT_NLIN_LSB_576P_VAL    0x71U

/*-------------------------------------------------------*/
/*                      Init TMDS                        */
/*-------------------------------------------------------*/
#define CLK_MIN_RATE_MSB_VAL     0x00U
#define CLK_MIN_RATE_ISB_VAL     0x57U
#define CLK_MIN_RATE_LSB_VAL     0xE4U
#define CLK_MAX_RATE_MSB_VAL     0x03U
#define CLK_MAX_RATE_ISB_VAL     0x95U
#define CLK_MAX_RATE_LSB_VAL     0xF8U
#define TIMERS_CONTROL_VAL       0x37U
#define RATE_DELTA_VAL           0xFFU
/* WORKAROUND START: PR1296 */
#define TEST_MODE_VAL            0x00U
/* WORKAROUND END: PR1296 */
/* WORKAROUND START: PR1308 */
//#define FIFO_LATENCY_CTRL_VAL    0x07U
#define FIFO_LATENCY_CTRL_VAL    0x01U
/* WORKAROUND END: PR1308 */
#define PHS_EXTRACTPHASE_VAL     0x00U
#define GAIN_HIGHIFEQUAL_VAL     0x01U
#define GAIN_NUMEDGES_VAL        0x03U
#define GAIN_VIDEOPERIOD_VAL     0x03U
#define GAIN_ASSUME_ACTIVITY_VAL 0x01U
#define GAIN_NB_SYNC_VAL         0x0FU
#define WDL_CFG_VAL              0x82U
#define DC_FILTER_VAL            0x31U

/*-------------------------------------------------------*/
/*                   Rate measurement                    */
/*-------------------------------------------------------*/
#define RATE_REFTIM_ENABLE       0x01U


/*-------------------------------------------------------------------
 * Register list
 *
 * Each register symbol has these fields: E_REG_page_register_access
 *
 * The symbols have a 8-bit value as follows
 *
  -------------------------------------------------------------------*/
typedef enum _tmbslTDA1997X_Reg_CTRL
{
    /*************************************************************************/
    /** Rows formatted in "HDMI Driver - Register CTRL"  updated 19972       **/
    /*************************************************************************/
    VERSION             = ACC_REG( 0x00, 0x00),
    INPUT_SEL           = ACC_REG( 0x01, 0x00),
    SERVICE_MODE        = ACC_REG( 0x02, 0x00),
    HPD_MAN_CTRL        = ACC_REG( 0x03, 0x00),
    RT_MAN_CTRL         = ACC_REG( 0x04, 0x00),
    MANUAL_PD_DDC       = ACC_REG( 0x05, 0x00),
    MANUAL_PD_TMDS      = ACC_REG( 0x06, 0x00),
    MANUAL_PD_VDP       = ACC_REG( 0x07, 0x00),
    MANUAL_PD_ADP       = ACC_REG( 0x08, 0x00),
    DDC_SOFT_RST        = ACC_REG( 0x09, 0x00),
    STANDBY_SOFT_RST    = ACC_REG( 0x0A, 0x00),
    HDMI_SOFT_RST       = ACC_REG( 0x0B, 0x00),
    HDMI_INFO_RST       = ACC_REG( 0x0C, 0x00),
    INT_FLG_CLR_TOP     = ACC_REG( 0x0E, 0x00),
    INT_FLG_CLR_SUS     = ACC_REG( 0x0F, 0x00),
    INT_FLG_CLR_DDC     = ACC_REG( 0x10, 0x00),
    INT_FLG_CLR_RATE    = ACC_REG( 0x11, 0x00),
    INT_FLG_CLR_MODE	= ACC_REG( 0x12, 0x00),
    INT_FLG_CLR_INFO	= ACC_REG( 0x13, 0x00),
    INT_FLG_CLR_AUDIO	= ACC_REG( 0x14, 0x00),
    INT_FLG_CLR_HDCP	= ACC_REG( 0x15, 0x00),
    INT_FLG_CLR_AFE		= ACC_REG( 0x16, 0x00),
    INT_MASK_TOP		= ACC_REG( 0x17, 0x00),
    INT_MASK_SUS		= ACC_REG( 0x18, 0x00),
    INT_MASK_DDC		= ACC_REG( 0x19, 0x00),
    INT_MASK_RATE		= ACC_REG( 0x1A, 0x00),
    INT_MASK_MODE		= ACC_REG( 0x1B, 0x00),
    INT_MASK_INFO		= ACC_REG( 0x1C, 0x00),
    INT_MASK_AUDIO		= ACC_REG( 0x1D, 0x00),
    INT_MASK_HDCP		= ACC_REG( 0x1E, 0x00),
    INT_MASK_AFE        = ACC_REG( 0x1F, 0x00),
    DETECT_5V_HPD       = ACC_REG( 0x20, 0x00),
    SUS_STATUS			= ACC_REG( 0x21, 0x00),
    V_PER_MSB       	= ACC_REG( 0x22, 0x00),
    V_PER_ISB       	= ACC_REG( 0x23, 0x00),
    V_PER_LSB     		= ACC_REG( 0x24, 0x00),
    H_PER_MSB     		= ACC_REG( 0x25, 0x00),
    H_PER_LSB      		= ACC_REG( 0x26, 0x00),
    HS_WIDTH_MSB      	= ACC_REG( 0x27, 0x00),
    HS_WIDTH_LSB      	= ACC_REG( 0x28, 0x00),
    FMT_H_TOT_MSB      	= ACC_REG( 0x29, 0x00),
    FMT_H_TOT_LSB       = ACC_REG( 0x2A, 0x00),
    FMT_H_ACT_MSB       = ACC_REG( 0x2B, 0x00),
    FMT_H_ACT_LSB       = ACC_REG( 0x2C, 0x00),
    FMT_H_FRONT_MSB		= ACC_REG( 0x2D, 0x00),
    FMT_H_FRONT_LSB		= ACC_REG( 0x2E, 0x00),
    FMT_H_SYNC_MSB      = ACC_REG( 0x2F, 0x00),
    FMT_H_SYNC_LSB		= ACC_REG( 0x30, 0x00),
    FMT_H_BACK_MSB		= ACC_REG( 0x31, 0x00),
    FMT_H_BACK_LSB		= ACC_REG( 0x32, 0x00),
    FMT_V_TOT_MSB		= ACC_REG( 0x33, 0x00),
    FMT_V_TOT_LSB		= ACC_REG( 0x34, 0x00),
    FMT_V_ACT_MSB		= ACC_REG( 0x35, 0x00),
    FMT_V_ACT_LSB		= ACC_REG( 0x36, 0x00),
    FMT_V_FRONT_F1		= ACC_REG( 0x37, 0x00),
    FMT_V_FRONT_F2		= ACC_REG( 0x38, 0x00),
    FMT_V_SYNC          = ACC_REG( 0x39, 0x00),
    FMT_V_BACK_F1		= ACC_REG( 0x3A, 0x00),
    FMT_V_BACK_F2		= ACC_REG( 0x3B, 0x00),
    FMT_DE_ACT          = ACC_REG( 0x3C, 0x00),
    EQ_STATUS0          = ACC_REG( 0x3E, 0x00),
    EQ_STATUS1          = ACC_REG( 0x3F, 0x00),
    RATE_CTRL          	= ACC_REG( 0x40, 0x00),
    RATE_DELTA          = ACC_REG( 0x41, 0x00),
    RATE_STABLE_CNT		= ACC_REG( 0x42, 0x00),
    CLK_MIN_RATE_MSB	= ACC_REG( 0x43, 0x00),
    CLK_MIN_RATE_ISB	= ACC_REG( 0x44, 0x00),
    CLK_MIN_RATE_LSB	= ACC_REG( 0x45, 0x00),
    CLK_MAX_RATE_MSB	= ACC_REG( 0x46, 0x00),
    CLK_MAX_RATE_ISB	= ACC_REG( 0x47, 0x00),
    CLK_MAX_RATE_LSB	= ACC_REG( 0x48, 0x00),
    CLK_A_STATUS		= ACC_REG( 0x49, 0x00),
    CLK_A_RATE_MSB		= ACC_REG( 0x4A, 0x00),
    CLK_A_RATE_ISB		= ACC_REG( 0x4B, 0x00),
    CLK_A_RATE_LSB		= ACC_REG( 0x4C, 0x00),
    DRIFT_CLK_A_REG		= ACC_REG( 0x4D, 0x00),
    CLK_B_STATUS		= ACC_REG( 0x4E, 0x00),
    CLK_B_RATE_MSB		= ACC_REG( 0x4F, 0x00),
    CLK_B_RATE_ISB		= ACC_REG( 0x50, 0x00),
    CLK_B_RATE_LSB		= ACC_REG( 0x51, 0x00),
    DRIFT_CLK_B_REG		= ACC_REG( 0x52, 0x00),
    HDCP_CTRL			= ACC_REG( 0x60, 0x00),
    HDCP_KDS_MSB		= ACC_REG( 0x61, 0x00),
    HDCP_KDS_LSB		= ACC_REG( 0x62, 0x00),
    HDCP_BCAPS			= ACC_REG( 0x63, 0x00),
    HDCP_KEY_CTRL		= ACC_REG( 0x64, 0x00),
    HDCP_BSTATUS_MSB	= ACC_REG( 0x65, 0x00),
    HDCP_BSTATUS_LSB	= ACC_REG( 0x66, 0x00),
    FIFO_KSV_IDX		= ACC_REG( 0x67, 0x00),
    FIFO_KSV4         	= ACC_REG( 0x68, 0x00),
    FIFO_KSV3         	= ACC_REG( 0x69, 0x00),
    FIFO_KSV2         	= ACC_REG( 0x6A, 0x00),
    FIFO_KSV1         	= ACC_REG( 0x6B, 0x00),
    FIFO_KSV0         	= ACC_REG( 0x6C, 0x00),
    HDCP_RPT_CTRL		= ACC_REG( 0x6D, 0x00),
    INFO_CTRL			= ACC_REG( 0x76, 0x00),
    INFO_EXCEED			= ACC_REG( 0x77, 0x00),
    ACP_TIMEOUT         = ACC_REG( 0x78, 0x00),
    PACKET_TYPE_SCAN	= ACC_REG( 0x79, 0x00),
    PIX_REPEAT         	= ACC_REG( 0x7B, 0x00),
    AUDIO_PATH         	= ACC_REG( 0x7C, 0x00),
    AUDIO_SEL			= ACC_REG( 0x7D, 0x00),
    AUDIO_OUT_ENABLE	= ACC_REG( 0x7E, 0x00),
    AUDIO_OUT_LOW_HIZ	= ACC_REG( 0x7F, 0x00),
    VDP_CTRL            = ACC_REG( 0x80, 0x00),
    MAT_OI1_MSB         = ACC_REG( 0x81, 0x00),
    MAT_OI1_LSB         = ACC_REG( 0x82, 0x00),
    MAT_OI2_MSB         = ACC_REG( 0x83, 0x00),
    MAT_OI2_LSB         = ACC_REG( 0x84, 0x00),
    MAT_OI3_MSB         = ACC_REG( 0x85, 0x00),
    MAT_OI3_LSB         = ACC_REG( 0x86, 0x00),
    MAT_P11_MSB         = ACC_REG( 0x87, 0x00),
    MAT_P11_LSB         = ACC_REG( 0x88, 0x00),
    MAT_P12_MSB         = ACC_REG( 0x89, 0x00),
    MAT_P12_LSB         = ACC_REG( 0x8A, 0x00),
    MAT_P13_MSB         = ACC_REG( 0x8B, 0x00),
    MAT_P13_LSB         = ACC_REG( 0x8C, 0x00),
    MAT_P21_MSB         = ACC_REG( 0x8D, 0x00),
    MAT_P21_LSB         = ACC_REG( 0x8E, 0x00),
    MAT_P22_MSB         = ACC_REG( 0x8F, 0x00),
    MAT_P22_LSB         = ACC_REG( 0x90, 0x00),
    MAT_P23_MSB         = ACC_REG( 0x91, 0x00),
    MAT_P23_LSB         = ACC_REG( 0x92, 0x00),
    MAT_P31_MSB         = ACC_REG( 0x93, 0x00),
    MAT_P31_LSB         = ACC_REG( 0x94, 0x00),
    MAT_P32_MSB         = ACC_REG( 0x95, 0x00),
    MAT_P32_LSB         = ACC_REG( 0x96, 0x00),
    MAT_P33_MSB         = ACC_REG( 0x97, 0x00),
    MAT_P33_LSB         = ACC_REG( 0x98, 0x00),
    MAT_OO1_MSB         = ACC_REG( 0x99, 0x00),
    MAT_OO1_LSB         = ACC_REG( 0x9A, 0x00),
    MAT_OO2_MSB         = ACC_REG( 0x9B, 0x00),
    MAT_OO2_LSB         = ACC_REG( 0x9C, 0x00),
    MAT_OO3_MSB         = ACC_REG( 0x9D, 0x00),
    MAT_OO3_LSB         = ACC_REG( 0x9E, 0x00),
    VHREF_CTRL          = ACC_REG( 0xA0, 0x00),
    PXCNT_PR_MSB		= ACC_REG( 0xA2, 0x00),
    PXCNT_PR_LSB        = ACC_REG( 0xA3, 0x00),
    PXCNT_NPIX_MSB      = ACC_REG( 0xA4, 0x00),
    PXCNT_NPIX_LSB      = ACC_REG( 0xA5, 0x00),
    LCNT_PR_MSB			= ACC_REG( 0xA6, 0x00),
    LCNT_PR_LSB         = ACC_REG( 0xA7, 0x00),
    LCNT_NLIN_MSB       = ACC_REG( 0xA8, 0x00),
    LCNT_NLIN_LSB       = ACC_REG( 0xA9, 0x00),
    HREF_S_MSB			= ACC_REG( 0xAA, 0x00),
    HREF_S_LSB          = ACC_REG( 0xAB, 0x00),
    HREF_E_MSB          = ACC_REG( 0xAC, 0x00),
    HREF_E_LSB          = ACC_REG( 0xAD, 0x00),
    HS_S_MSB			= ACC_REG( 0xAE, 0x00),
    HS_S_LSB            = ACC_REG( 0xAF, 0x00),
    HS_E_MSB            = ACC_REG( 0xB0, 0x00),
    HS_E_LSB            = ACC_REG( 0xB1, 0x00),
    VREF_F1_S_MSB       = ACC_REG( 0xB2, 0x00),
    VREF_F1_S_LSB       = ACC_REG( 0xB3, 0x00),
    VREF_F1_WIDTH       = ACC_REG( 0xB4, 0x00),
    VREF_F2_S_MSB       = ACC_REG( 0xB5, 0x00),
    VREF_F2_S_LSB       = ACC_REG( 0xB6, 0x00),
    VREF_F2_WIDTH       = ACC_REG( 0xB7, 0x00),
    VS_F1_LINE_S_MSB    = ACC_REG( 0xB8, 0x00),
    VS_F1_LINE_S_LSB    = ACC_REG( 0xB9, 0x00),
    VS_F1_LINE_WIDTH    = ACC_REG( 0xBA, 0x00),
    VS_F2_LINE_S_MSB    = ACC_REG( 0xBB, 0x00),
    VS_F2_LINE_S_LSB    = ACC_REG( 0xBC, 0x00),
    VS_F2_LINE_WIDTH    = ACC_REG( 0xBD, 0x00),
    VS_F1_PIX_S_MSB		= ACC_REG( 0xBE, 0x00),
    VS_F1_PIX_S_LSB     = ACC_REG( 0xBF, 0x00),
    VS_F1_PIX_E_MSB     = ACC_REG( 0xC0, 0x00),
    VS_F1_PIX_E_LSB     = ACC_REG( 0xC1, 0x00),
    VS_F2_PIX_S_MSB		= ACC_REG( 0xC2, 0x00),
    VS_F2_PIX_S_LSB     = ACC_REG( 0xC3, 0x00),
    VS_F2_PIX_E_MSB     = ACC_REG( 0xC4, 0x00),
    VS_F2_PIX_E_LSB     = ACC_REG( 0xC5, 0x00),
    FREF_F1_S_MSB       = ACC_REG( 0xC6, 0x00),
    FREF_F1_S_LSB       = ACC_REG( 0xC7, 0x00),
    FREF_F2_S_MSB       = ACC_REG( 0xC8, 0x00),
    FREF_F2_S_LSB       = ACC_REG( 0xC9, 0x00),
    FDW_S_MSB			= ACC_REG( 0xCA, 0x00),
    FDW_S_LSB           = ACC_REG( 0xCB, 0x00),
    FDW_E_MSB           = ACC_REG( 0xCC, 0x00),
    FDW_E_LSB           = ACC_REG( 0xCD, 0x00),
    MEASLIN_MSB         = ACC_REG( 0xCE, 0x00),
    MEASLIN_LSB         = ACC_REG( 0xCF, 0x00),
    MEASPIX_MSB         = ACC_REG( 0xD0, 0x00),
    MEASPIX_LSB         = ACC_REG( 0xD1, 0x00),
    ASD_MEASLIN_MSB     = ACC_REG( 0xD2, 0x00),
    BLK_GY_MSB          = ACC_REG( 0xDA, 0x00),
    BLK_GY_LSB          = ACC_REG( 0xDB, 0x00),
    BLK_BU_MSB          = ACC_REG( 0xDC, 0x00),
    BLK_BU_LSB          = ACC_REG( 0xDD, 0x00),
    BLK_RV_MSB          = ACC_REG( 0xDE, 0x00),
    BLK_RV_LSB          = ACC_REG( 0xDF, 0x00),
    FILTERS_CTRL		= ACC_REG( 0xE0, 0x00),
    C_CEIL_MSB			= ACC_REG( 0xE1, 0x00),
    C_CEIL_LSB			= ACC_REG( 0xE2, 0x00),
    C_FLOOR_MSB			= ACC_REG( 0xE3, 0x00),
    C_FLOOR_LSB			= ACC_REG( 0xE4, 0x00),
    Y_CEIL_MSB			= ACC_REG( 0xE5, 0x00),
    Y_CEIL_LSB			= ACC_REG( 0xE6, 0x00),
    Y_FLOOR_MSB			= ACC_REG( 0xE7, 0x00),
    Y_FLOOR_LSB			= ACC_REG( 0xE8, 0x00),
    DITHERING_CTRL      = ACC_REG( 0xE9, 0x00),
    OF_CTRL             = ACC_REG( 0xEA, 0x00),
    CLKOUT_CTRL         = ACC_REG( 0xEB, 0x00),
    HS_HREF_SEL			= ACC_REG( 0xEC, 0x00),
    VS_VREF_SEL			= ACC_REG( 0xED, 0x00),
    DE_FREF_SEL			= ACC_REG( 0xEE, 0x00),
    VP35_32_CTRL		= ACC_REG( 0xEF, 0x00),
    VP31_28_CTRL		= ACC_REG( 0xF0, 0x00),
    VP27_24_CTRL		= ACC_REG( 0xF1, 0x00),
    VP23_20_CTRL		= ACC_REG( 0xF2, 0x00),
    VP19_16_CTRL		= ACC_REG( 0xF3, 0x00),
    VP15_12_CTRL		= ACC_REG( 0xF4, 0x00),
    VP11_8_CTRL			= ACC_REG( 0xF5, 0x00),
    VP7_4_CTRL			= ACC_REG( 0xF6, 0x00),
    VP3_0_CTRL			= ACC_REG( 0xF7, 0x00),
    EMTP_CTRL			= ACC_REG( 0xFA, 0x00),
    EMTP_ADDRESS		= ACC_REG( 0xFB, 0x00),
    CMTP_CTRL			= ACC_REG( 0xFC, 0x00),
    CMTP_ADDRESS		= ACC_REG( 0xFD, 0x00),
    ECC_CHECK			= ACC_REG( 0xFE, 0x00),
    CURPAGE_ADR_00H     = ACC_REG( 0xFF, 0x00)
} tmbslTDA1997X_Reg_CTRL;

/*-------------------------------------------------------------------
 * Register list
 *
 * Each register symbol has these fields: E_REG_page_register_access
 *
 * The symbols have a 8-bit value as follows
 *
  -------------------------------------------------------------------*/
typedef enum _tmbslTDA1997X_Reg_HDMI
{
    /*************************************************************************/
    /** Rows formatted in "HDMI Driver - Register HDMI"  updated 19972      **/
    /*************************************************************************/
    HDMI_FLAGS			= ACC_REG( 0x00, 0x01),
    DEEP_COLOR_MODE		= ACC_REG( 0x01, 0x01),
    ERR_FR_MSB        	= ACC_REG( 0x02, 0x01),
    ERR_FR_LSB        	= ACC_REG( 0x03, 0x01),
    FR_ERR_MSB         	= ACC_REG( 0x04, 0x01),
    FR_ERR_LSB       	= ACC_REG( 0x05, 0x01),
    AUDIO_FLAGS      	= ACC_REG( 0x08, 0x01),
    AUDIO_FREQ       	= ACC_REG( 0x09, 0x01),
    ACRP_CTS_MSB       	= ACC_REG( 0x0A, 0x01),
    ACRP_CTS_ISB        = ACC_REG( 0x0B, 0x01),
    ACRP_CTS_LSB       	= ACC_REG( 0x0C, 0x01),
    ACRP_N_MSB     		= ACC_REG( 0x0D, 0x01),
    ACRP_N_ISB     		= ACC_REG( 0x0E, 0x01),
    ACRP_N_LSB     		= ACC_REG( 0x0F, 0x01),
    CH_STAT_BYTE0     	= ACC_REG( 0x10, 0x01),
    CH_STAT_BYTE1    	= ACC_REG( 0x11, 0x01),
    CH_STAT_BYTE3		= ACC_REG( 0x12, 0x01),
    CH_STAT_BYTE4		= ACC_REG( 0x13, 0x01),
    CH_STAT_BYTE2_AP0_L	= ACC_REG( 0x14, 0x01),
    CH_STAT_BYTE2_AP0_R	= ACC_REG( 0x15, 0x01),
    CH_STAT_BYTE2_AP1_L	= ACC_REG( 0x16, 0x01),
    CH_STAT_BYTE2_AP1_R	= ACC_REG( 0x17, 0x01),
    CH_STAT_BYTE2_AP2_L	= ACC_REG( 0x18, 0x01),
    CH_STAT_BYTE2_AP2_R	= ACC_REG( 0x19, 0x01),
    CH_STAT_BYTE2_AP3_L	= ACC_REG( 0x1A, 0x01),
    CH_STAT_BYTE2_AP3_R	= ACC_REG( 0x1B, 0x01),
    GCP_PACKET_TYPE		= ACC_REG( 0x21, 0x01),
    GCP_HEADER1       	= ACC_REG( 0x22, 0x01),
    GCP_HEADER2       	= ACC_REG( 0x23, 0x01),
    GCP_BYTE0     		= ACC_REG( 0x24, 0x01),
    GCP_BYTE1     		= ACC_REG( 0x25, 0x01),
    GCP_BYTE2      		= ACC_REG( 0x26, 0x01),
    GCP_BYTE3			= ACC_REG( 0x27, 0x01),
    GCP_BYTE4			= ACC_REG( 0x28, 0x01),
    GCP_BYTE5			= ACC_REG( 0x29, 0x01),
    GCP_BYTE6			= ACC_REG( 0x2A, 0x01),
    ACP_UPDATE			= ACC_REG( 0x40, 0x01),
    ACP_PACKET_TYPE		= ACC_REG( 0x41, 0x01),
    ACP_TYPE			= ACC_REG( 0x42, 0x01),
    ACP_RSVD			= ACC_REG( 0x43, 0x01),
    ACP_BYTE0			= ACC_REG( 0x44, 0x01),
    ACP_BYTE1			= ACC_REG( 0x45, 0x01),
    ACP_BYTE2			= ACC_REG( 0x46, 0x01),
    ACP_BYTE3			= ACC_REG( 0x47, 0x01),
    ACP_BYTE4			= ACC_REG( 0x48, 0x01),
    ACP_BYTE5			= ACC_REG( 0x49, 0x01),
    ACP_BYTE6			= ACC_REG( 0x4A, 0x01),
    ACP_BYTE7			= ACC_REG( 0x4B, 0x01),
    ACP_BYTE8			= ACC_REG( 0x4C, 0x01),
    ACP_BYTE9			= ACC_REG( 0x4D, 0x01),
    ACP_BYTE10			= ACC_REG( 0x4E, 0x01),
    ACP_BYTE11			= ACC_REG( 0x4F, 0x01),
    ACP_BYTE12			= ACC_REG( 0x50, 0x01),
    ACP_BYTE13			= ACC_REG( 0x51, 0x01),
    ACP_BYTE14			= ACC_REG( 0x52, 0x01),
    ACP_BYTE15			= ACC_REG( 0x53, 0x01),
    ISRC1_PACKET_TYPE	= ACC_REG( 0x61, 0x01),
    ISRC1_CTRL			= ACC_REG( 0x62, 0x01),
    ISRC1_RSVD			= ACC_REG( 0x63, 0x01),
    UPC_EAN_ISRC0		= ACC_REG( 0x64, 0x01),
    UPC_EAN_ISRC1		= ACC_REG( 0x65, 0x01),
    UPC_EAN_ISRC2		= ACC_REG( 0x66, 0x01),
    UPC_EAN_ISRC3		= ACC_REG( 0x67, 0x01),
    UPC_EAN_ISRC4		= ACC_REG( 0x68, 0x01),
    UPC_EAN_ISRC5		= ACC_REG( 0x69, 0x01),
    UPC_EAN_ISRC6		= ACC_REG( 0x6A, 0x01),
    UPC_EAN_ISRC7		= ACC_REG( 0x6B, 0x01),
    UPC_EAN_ISRC8		= ACC_REG( 0x6C, 0x01),
    UPC_EAN_ISRC9		= ACC_REG( 0x6D, 0x01),
    UPC_EAN_ISRC10		= ACC_REG( 0x6E, 0x01),
    UPC_EAN_ISRC11		= ACC_REG( 0x6F, 0x01),
    UPC_EAN_ISRC12		= ACC_REG( 0x70, 0x01),
    UPC_EAN_ISRC13		= ACC_REG( 0x71, 0x01),
    UPC_EAN_ISRC14		= ACC_REG( 0x72, 0x01),
    UPC_EAN_ISRC15		= ACC_REG( 0x73, 0x01),
    ISRC2_PACKET_TYPE	= ACC_REG( 0x81, 0x01),
    ISRC2_RSVD1			= ACC_REG( 0x82, 0x01),
    ISRC2_RSVD2			= ACC_REG( 0x83, 0x01),
    UPC_EAN_ISRC16		= ACC_REG( 0x84, 0x01),
    UPC_EAN_ISRC17		= ACC_REG( 0x85, 0x01),
    UPC_EAN_ISRC18		= ACC_REG( 0x86, 0x01),
    UPC_EAN_ISRC19		= ACC_REG( 0x87, 0x01),
    UPC_EAN_ISRC20		= ACC_REG( 0x88, 0x01),
    UPC_EAN_ISRC21		= ACC_REG( 0x89, 0x01),
    UPC_EAN_ISRC22		= ACC_REG( 0x8A, 0x01),
    UPC_EAN_ISRC23		= ACC_REG( 0x8B, 0x01),
    UPC_EAN_ISRC24		= ACC_REG( 0x8C, 0x01),
    UPC_EAN_ISRC25		= ACC_REG( 0x8D, 0x01),
    UPC_EAN_ISRC26		= ACC_REG( 0x8E, 0x01),
    UPC_EAN_ISRC27		= ACC_REG( 0x8F, 0x01),
    UPC_EAN_ISRC28		= ACC_REG( 0x90, 0x01),
    UPC_EAN_ISRC29		= ACC_REG( 0x91, 0x01),
    UPC_EAN_ISRC30		= ACC_REG( 0x92, 0x01),
    UPC_EAN_ISRC31		= ACC_REG( 0x93, 0x01),
    GBD_UPDATE			= ACC_REG( 0xA0, 0x01),
    GBD_PACKET_TYPE		= ACC_REG( 0xA1, 0x01),
    GBD_HEADER1       	= ACC_REG( 0xA2, 0x01),
    GBD_HEADER2       	= ACC_REG( 0xA3, 0x01),
    GBD_BYTE0     		= ACC_REG( 0xA4, 0x01),
    GBD_BYTE1     		= ACC_REG( 0xA5, 0x01),
    GBD_BYTE2      		= ACC_REG( 0xA6, 0x01),
    GBD_BYTE3			= ACC_REG( 0xA7, 0x01),
    GBD_BYTE4			= ACC_REG( 0xA8, 0x01),
    GBD_BYTE5			= ACC_REG( 0xA9, 0x01),
    GBD_BYTE6			= ACC_REG( 0xAA, 0x01),
    GBD_BYTE7     		= ACC_REG( 0xAB, 0x01),
    GBD_BYTE8     		= ACC_REG( 0xAC, 0x01),
    GBD_BYTE9      		= ACC_REG( 0xAD, 0x01),
    GBD_BYTE10			= ACC_REG( 0xAE, 0x01),
    GBD_BYTE11			= ACC_REG( 0xAF, 0x01),
    GBD_BYTE12			= ACC_REG( 0xB0, 0x01),
    GBD_BYTE13			= ACC_REG( 0xB1, 0x01),
    GBD_BYTE14    		= ACC_REG( 0xB2, 0x01),
    GBD_BYTE15     		= ACC_REG( 0xB3, 0x01),
    GBD_BYTE16     		= ACC_REG( 0xB4, 0x01),
    GBD_BYTE17			= ACC_REG( 0xB5, 0x01),
    GBD_BYTE18			= ACC_REG( 0xB6, 0x01),
    GBD_BYTE19			= ACC_REG( 0xB7, 0x01),
    GBD_BYTE20			= ACC_REG( 0xB8, 0x01),
    GBD_BYTE21			= ACC_REG( 0xB9, 0x01),
    GBD_BYTE22			= ACC_REG( 0xBA, 0x01),
    GBD_BYTE23			= ACC_REG( 0xBB, 0x01),
    GBD_BYTE24    		= ACC_REG( 0xBC, 0x01),
    GBD_BYTE25     		= ACC_REG( 0xBD, 0x01),
    GBD_BYTE26     		= ACC_REG( 0xBE, 0x01),
    GBD_BYTE27			= ACC_REG( 0xBF, 0x01),
    HDCP_RI_MSB			= ACC_REG( 0xE0, 0x01),
    HDCP_RI_LSB			= ACC_REG( 0xE1, 0x01),
    HDCP_PJ				= ACC_REG( 0xE2, 0x01),
    HDCP_AKSV4			= ACC_REG( 0xE3, 0x01),
    HDCP_AKSV3			= ACC_REG( 0xE4, 0x01),
    HDCP_AKSV2			= ACC_REG( 0xE5, 0x01),
    HDCP_AKSV1			= ACC_REG( 0xE6, 0x01),
    HDCP_AKSV0			= ACC_REG( 0xE7, 0x01),
    HDCP_AN7			= ACC_REG( 0xE8, 0x01),
    HDCP_AN6      		= ACC_REG( 0xE9, 0x01),
    HDCP_AN5			= ACC_REG( 0xEA, 0x01),
    HDCP_AN4         	= ACC_REG( 0xEB, 0x01),
    HDCP_AN3			= ACC_REG( 0xEC, 0x01),
    HDCP_AN2			= ACC_REG( 0xED, 0x01),
    HDCP_AN1			= ACC_REG( 0xEE, 0x01),
    HDCP_AN0			= ACC_REG( 0xEF, 0x01),
    HDCP_AINFO			= ACC_REG( 0xF0, 0x01),
    HDCP_FSM_STATE		= ACC_REG( 0xF1, 0x01),
    CURPAGE_ADR_01H     = ACC_REG( 0xFF, 0x01)
} tmbslTDA1997X_Reg_HDMI;

typedef enum _tmbslTDA1997X_Reg_AVI
{
    /*************************************************************************/
    /** Rows formatted in "HDMI Driver - Register AVI"  updated 19972		**/
    /*************************************************************************/
    VS_IF_HDMI_UPDATE		= ACC_REG( 0x00, 0x02),
    VS_IF_HDMI_TYPE     	= ACC_REG( 0x01, 0x02),
    VS_IF_HDMI_VERSION		= ACC_REG( 0x02, 0x02),
    VS_IF_HDMI_LENGTH		= ACC_REG( 0x03, 0x02),
    VS_IF_HDMI_CHECKSUM 	= ACC_REG( 0x04, 0x02),
    VS_IF_HDMI_BYTE1		= ACC_REG( 0x05, 0x02),
    VS_IF_HDMI_BYTE2		= ACC_REG( 0x06, 0x02),
    VS_IF_HDMI_BYTE3		= ACC_REG( 0x07, 0x02),
    VS_IF_HDMI_BYTE4		= ACC_REG( 0x08, 0x02),
    VS_IF_HDMI_BYTE5		= ACC_REG( 0x09, 0x02),
    VS_IF_HDMI_BYTE6	    = ACC_REG( 0x0A, 0x02),
    VS_IF_HDMI_BYTE7		= ACC_REG( 0x0B, 0x02),
    VS_IF_HDMI_BYTE8	    = ACC_REG( 0x0C, 0x02),
    VS_IF_HDMI_BYTE9	    = ACC_REG( 0x0D, 0x02),
    VS_IF_HDMI_BYTE10		= ACC_REG( 0x0E, 0x02),
    VS_IF_HDMI_BYTE11		= ACC_REG( 0x0F, 0x02),
    VS_IF_HDMI_BYTE12		= ACC_REG( 0x10, 0x02),
    VS_IF_HDMI_BYTE13		= ACC_REG( 0x11, 0x02),
    VS_IF_HDMI_BYTE14		= ACC_REG( 0x12, 0x02),
    VS_IF_HDMI_BYTE15		= ACC_REG( 0x13, 0x02),
    VS_IF_HDMI_BYTE16		= ACC_REG( 0x14, 0x02),
    VS_IF_HDMI_BYTE17		= ACC_REG( 0x15, 0x02),
    VS_IF_HDMI_BYTE18		= ACC_REG( 0x16, 0x02),
    VS_IF_HDMI_BYTE19		= ACC_REG( 0x17, 0x02),
    VS_IF_HDMI_BYTE20		= ACC_REG( 0x18, 0x02),
    VS_IF_HDMI_BYTE21		= ACC_REG( 0x19, 0x02),
    VS_IF_HDMI_BYTE22		= ACC_REG( 0x1A, 0x02),
    VS_IF_HDMI_BYTE23		= ACC_REG( 0x1B, 0x02),
    VS_IF_HDMI_BYTE24		= ACC_REG( 0x1C, 0x02),
    VS_IF_HDMI_BYTE25		= ACC_REG( 0x1D, 0x02),
    VS_IF_HDMI_BYTE26		= ACC_REG( 0x1E, 0x02),
    VS_IF_HDMI_BYTE27		= ACC_REG( 0x1F, 0x02),
    VS_IF_OTHER_BK1_UPDATE	= ACC_REG( 0x20, 0x02),
    VS_IF_OTHER_BK1_TYPE	= ACC_REG( 0x21, 0x02),
    VS_IF_OTHER_BK1_VERSION	= ACC_REG( 0x22, 0x02),
    VS_IF_OTHER_BK1_LENGTH	= ACC_REG( 0x23, 0x02),
    VS_IF_OTHER_BK1_CHECKSUM= ACC_REG( 0x24, 0x02),
    VS_IF_OTHER_BK1_BYTE1	= ACC_REG( 0x25, 0x02),
    VS_IF_OTHER_BK1_BYTE2	= ACC_REG( 0x26, 0x02),
    VS_IF_OTHER_BK1_BYTE3	= ACC_REG( 0x27, 0x02),
    VS_IF_OTHER_BK1_BYTE4	= ACC_REG( 0x28, 0x02),
    VS_IF_OTHER_BK1_BYTE5	= ACC_REG( 0x29, 0x02),
    VS_IF_OTHER_BK1_BYTE6	= ACC_REG( 0x2A, 0x02),
    VS_IF_OTHER_BK1_BYTE7	= ACC_REG( 0x2B, 0x02),
    VS_IF_OTHER_BK1_BYTE8	= ACC_REG( 0x2C, 0x02),
    VS_IF_OTHER_BK1_BYTE9	= ACC_REG( 0x2D, 0x02),
    VS_IF_OTHER_BK1_BYTE10 	= ACC_REG( 0x2E, 0x02),
    VS_IF_OTHER_BK1_BYTE11 	= ACC_REG( 0x2F, 0x02),
    VS_IF_OTHER_BK1_BYTE12 	= ACC_REG( 0x30, 0x02),
    VS_IF_OTHER_BK1_BYTE13 	= ACC_REG( 0x31, 0x02),
    VS_IF_OTHER_BK1_BYTE14 	= ACC_REG( 0x32, 0x02),
    VS_IF_OTHER_BK1_BYTE15 	= ACC_REG( 0x33, 0x02),
    VS_IF_OTHER_BK1_BYTE16 	= ACC_REG( 0x34, 0x02),
    VS_IF_OTHER_BK1_BYTE17 	= ACC_REG( 0x35, 0x02),
    VS_IF_OTHER_BK1_BYTE18	= ACC_REG( 0x36, 0x02),
    VS_IF_OTHER_BK1_BYTE19	= ACC_REG( 0x37, 0x02),
    VS_IF_OTHER_BK1_BYTE20	= ACC_REG( 0x38, 0x02),
    VS_IF_OTHER_BK1_BYTE21	= ACC_REG( 0x39, 0x02),
    VS_IF_OTHER_BK1_BYTE22	= ACC_REG( 0x3A, 0x02),
    VS_IF_OTHER_BK1_BYTE23	= ACC_REG( 0x3B, 0x02),
    VS_IF_OTHER_BK1_BYTE24	= ACC_REG( 0x3C, 0x02),
    VS_IF_OTHER_BK1_BYTE25	= ACC_REG( 0x3D, 0x02),
    VS_IF_OTHER_BK1_BYTE26	= ACC_REG( 0x3E, 0x02),
    VS_IF_OTHER_BK1_BYTE27	= ACC_REG( 0x3F, 0x02),
    VS_IF_OTHER_BK2_UPDATE	= ACC_REG( 0x40, 0x02),
    VS_IF_OTHER_BK2_TYPE	= ACC_REG( 0x41, 0x02),
    VS_IF_OTHER_BK2_VERSION	= ACC_REG( 0x42, 0x02),
    VS_IF_OTHER_BK2_LENGTH	= ACC_REG( 0x43, 0x02),
    VS_IF_OTHER_BK2_CHECKSUM= ACC_REG( 0x44, 0x02),
    VS_IF_OTHER_BK2_BYTE1	= ACC_REG( 0x45, 0x02),
    VS_IF_OTHER_BK2_BYTE2	= ACC_REG( 0x46, 0x02),
    VS_IF_OTHER_BK2_BYTE3	= ACC_REG( 0x47, 0x02),
    VS_IF_OTHER_BK2_BYTE4	= ACC_REG( 0x48, 0x02),
    VS_IF_OTHER_BK2_BYTE5	= ACC_REG( 0x49, 0x02),
    VS_IF_OTHER_BK2_BYTE6	= ACC_REG( 0x4A, 0x02),
    VS_IF_OTHER_BK2_BYTE7	= ACC_REG( 0x4B, 0x02),
    VS_IF_OTHER_BK2_BYTE8	= ACC_REG( 0x4C, 0x02),
    VS_IF_OTHER_BK2_BYTE9	= ACC_REG( 0x4D, 0x02),
    VS_IF_OTHER_BK2_BYTE10 	= ACC_REG( 0x4E, 0x02),
    VS_IF_OTHER_BK2_BYTE11 	= ACC_REG( 0x4F, 0x02),
    VS_IF_OTHER_BK2_BYTE12 	= ACC_REG( 0x50, 0x02),
    VS_IF_OTHER_BK2_BYTE13 	= ACC_REG( 0x51, 0x02),
    VS_IF_OTHER_BK2_BYTE14 	= ACC_REG( 0x52, 0x02),
    VS_IF_OTHER_BK2_BYTE15 	= ACC_REG( 0x53, 0x02),
    VS_IF_OTHER_BK2_BYTE16 	= ACC_REG( 0x54, 0x02),
    VS_IF_OTHER_BK2_BYTE17 	= ACC_REG( 0x55, 0x02),
    VS_IF_OTHER_BK2_BYTE18	= ACC_REG( 0x56, 0x02),
    VS_IF_OTHER_BK2_BYTE19	= ACC_REG( 0x57, 0x02),
    VS_IF_OTHER_BK2_BYTE20	= ACC_REG( 0x58, 0x02),
    VS_IF_OTHER_BK2_BYTE21	= ACC_REG( 0x59, 0x02),
    VS_IF_OTHER_BK2_BYTE22	= ACC_REG( 0x5A, 0x02),
    VS_IF_OTHER_BK2_BYTE23	= ACC_REG( 0x5B, 0x02),
    VS_IF_OTHER_BK2_BYTE24	= ACC_REG( 0x5C, 0x02),
    VS_IF_OTHER_BK2_BYTE25	= ACC_REG( 0x5D, 0x02),
    VS_IF_OTHER_BK2_BYTE26	= ACC_REG( 0x5E, 0x02),
    VS_IF_OTHER_BK2_BYTE27	= ACC_REG( 0x5F, 0x02),
    AVI_IF_UPDATE			= ACC_REG( 0x60, 0x02),
    AVI_IF_TYPE				= ACC_REG( 0x61, 0x02),
    AVI_IF_VERSION			= ACC_REG( 0x62, 0x02),
    AVI_IF_LENGTH			= ACC_REG( 0x63, 0x02),
    AVI_IF_CHECKSUM			= ACC_REG( 0x64, 0x02),
    AVI_IF_BYTE1			= ACC_REG( 0x65, 0x02),
    AVI_IF_BYTE2			= ACC_REG( 0x66, 0x02),
    AVI_IF_BYTE3			= ACC_REG( 0x67, 0x02),
    AVI_IF_BYTE4			= ACC_REG( 0x68, 0x02),
    AVI_IF_BYTE5			= ACC_REG( 0x69, 0x02),
    AVI_IF_BYTE6			= ACC_REG( 0x6A, 0x02),
    AVI_IF_BYTE7			= ACC_REG( 0x6B, 0x02),
    AVI_IF_BYTE8			= ACC_REG( 0x6C, 0x02),
    AVI_IF_BYTE9			= ACC_REG( 0x6D, 0x02),
    AVI_IF_BYTE10			= ACC_REG( 0x6E, 0x02),
    AVI_IF_BYTE11			= ACC_REG( 0x6F, 0x02),
    AVI_IF_BYTE12			= ACC_REG( 0x70, 0x02),
    AVI_IF_BYTE13			= ACC_REG( 0x71, 0x02),
    SPD_IF_UPDATE			= ACC_REG( 0x80, 0x02),
    SPD_IF_TYPE				= ACC_REG( 0x81, 0x02),
    SPD_IF_VERSION			= ACC_REG( 0x82, 0x02),
    SPD_IF_LENGTH			= ACC_REG( 0x83, 0x02),
    SPD_IF_CHECKSUM			= ACC_REG( 0x84, 0x02),
    SPD_IF_BYTE1			= ACC_REG( 0x85, 0x02),
    SPD_IF_BYTE2			= ACC_REG( 0x86, 0x02),
    SPD_IF_BYTE3			= ACC_REG( 0x87, 0x02),
    SPD_IF_BYTE4			= ACC_REG( 0x88, 0x02),
    SPD_IF_BYTE5			= ACC_REG( 0x89, 0x02),
    SPD_IF_BYTE6			= ACC_REG( 0x8A, 0x02),
    SPD_IF_BYTE7			= ACC_REG( 0x8B, 0x02),
    SPD_IF_BYTE8			= ACC_REG( 0x8C, 0x02),
    SPD_IF_BYTE9			= ACC_REG( 0x8D, 0x02),
    SPD_IF_BYTE10			= ACC_REG( 0x8E, 0x02),
    SPD_IF_BYTE11			= ACC_REG( 0x8F, 0x02),
    SPD_IF_BYTE12			= ACC_REG( 0x90, 0x02),
    SPD_IF_BYTE13			= ACC_REG( 0x91, 0x02),
    SPD_IF_BYTE14			= ACC_REG( 0x92, 0x02),
    SPD_IF_BYTE15			= ACC_REG( 0x93, 0x02),
    SPD_IF_BYTE16			= ACC_REG( 0x94, 0x02),
    SPD_IF_BYTE17			= ACC_REG( 0x95, 0x02),
    SPD_IF_BYTE18			= ACC_REG( 0x96, 0x02),
    SPD_IF_BYTE19			= ACC_REG( 0x97, 0x02),
    SPD_IF_BYTE20			= ACC_REG( 0x98, 0x02),
    SPD_IF_BYTE21			= ACC_REG( 0x99, 0x02),
    SPD_IF_BYTE22			= ACC_REG( 0x9A, 0x02),
    SPD_IF_BYTE23			= ACC_REG( 0x9B, 0x02),
    SPD_IF_BYTE24			= ACC_REG( 0x9C, 0x02),
    SPD_IF_BYTE25			= ACC_REG( 0x9D, 0x02),
    SPD_IF_BYTE26			= ACC_REG( 0x9E, 0x02),
    SPD_IF_BYTE27			= ACC_REG( 0x9F, 0x02),
    AUD_IF_UPDATE 			= ACC_REG( 0xA0, 0x02),
    AUD_IF_TYPE 			= ACC_REG( 0xA1, 0x02),
    AUD_IF_VERSION     		= ACC_REG( 0xA2, 0x02),
    AUD_IF_LENGTH      		= ACC_REG( 0xA3, 0x02),
    AUD_IF_CHECKSUM		    = ACC_REG( 0xA4, 0x02),
    AUD_IF_BYTE1        	= ACC_REG( 0xA5, 0x02),
    AUD_IF_BYTE2        	= ACC_REG( 0xA6, 0x02),
    AUD_IF_BYTE3        	= ACC_REG( 0xA7, 0x02),
    AUD_IF_BYTE4        	= ACC_REG( 0xA8, 0x02),
    AUD_IF_BYTE5        	= ACC_REG( 0xA9, 0x02),
    AUD_IF_BYTE6        	= ACC_REG( 0xAA, 0x02),
    AUD_IF_BYTE7        	= ACC_REG( 0xAB, 0x02),
    AUD_IF_BYTE8        	= ACC_REG( 0xAC, 0x02),
    AUD_IF_BYTE9        	= ACC_REG( 0xAD, 0x02),
    AUD_IF_BYTE10       	= ACC_REG( 0xAE, 0x02),
    MPS_IF_UPDATE       	= ACC_REG( 0xC0, 0x02),
    MPS_IF_TYPE         	= ACC_REG( 0xC1, 0x02),
    MPS_IF_VERSION      	= ACC_REG( 0xC2, 0x02),
    MPS_IF_LENGTH       	= ACC_REG( 0xC3, 0x02),
    MPS_IF_CHECKSUM     	= ACC_REG( 0xC4, 0x02),
    MPS_IF_BYTE1        	= ACC_REG( 0xC5, 0x02),
    MPS_IF_BYTE2        	= ACC_REG( 0xC6, 0x02),
    MPS_IF_BYTE3        	= ACC_REG( 0xC7, 0x02),
    MPS_IF_BYTE4        	= ACC_REG( 0xC8, 0x02),
    MPS_IF_BYTE5        	= ACC_REG( 0xC9, 0x02),
    MPS_IF_BYTE6        	= ACC_REG( 0xCA, 0x02),
    MPS_IF_BYTE7        	= ACC_REG( 0xCB, 0x02),
    MPS_IF_BYTE8        	= ACC_REG( 0xCC, 0x02),
    MPS_IF_BYTE9        	= ACC_REG( 0xCD, 0x02),
    MPS_IF_BYTE10       	= ACC_REG( 0xCE, 0x02),
    CHECKSUM_VS_IF_HDMI		= ACC_REG( 0xE0, 0x02),
    CHECKSUM_VS_IF_OTHER_BK1= ACC_REG( 0xE1, 0x02),
    CHECKSUM_VS_IF_OTHER_BK2= ACC_REG( 0xE2, 0x02),
    CHECKSUM_AVI_IF			= ACC_REG( 0xE3, 0x02),
    CHECKSUM_SPD_IF			= ACC_REG( 0xE4, 0x02),
    CHECKSUM_AUD_IF			= ACC_REG( 0xE5, 0x02),
    CHECKSUM_MPS_IF			= ACC_REG( 0xE6, 0x02),
    CURPAGE_ADR_02H     	= ACC_REG( 0xFF, 0x02)
} tmbslTDA1997X_Reg_AVI;

typedef enum _tmbslTDA1997X_Reg_Page12
{
    /*************************************************************************/
    /** Rows formatted in "HDMI Driver - Register TMDS"    updated 19972    **/
    /*************************************************************************/
    CLK_CFG                   = ACC_REG( 0x00, 0x12),
    CLK_OUT_CFG               = ACC_REG( 0x01, 0x12),
    CFG1                      = ACC_REG( 0x02, 0x12),
    CFG2                      = ACC_REG( 0x03, 0x12),
    PHS_CONF              	  = ACC_REG( 0x05, 0x12),
    PHS_AVERAGINGK            = ACC_REG( 0x06, 0x12),
    PHS_CH0_OVRD              = ACC_REG( 0x07, 0x12),
    PHS_CH1_OVRD              = ACC_REG( 0x08, 0x12),
    PHS_CH2_OVRD              = ACC_REG( 0x09, 0x12),
    PHS_CH0_USED              = ACC_REG( 0x0A, 0x12),
    PHS_CH1_USED              = ACC_REG( 0x0B, 0x12),
    PHS_CH2_USED              = ACC_REG( 0x0C, 0x12),
    WDL_CFG                   = ACC_REG( 0x10, 0x12),
    WDL_FROZEN                = ACC_REG( 0x11, 0x12),
    DELOCK_DELAY              = ACC_REG( 0x12, 0x12),
    SWAP_PN                   = ACC_REG( 0x13, 0x12),
    OFFSET_CH0_SELPHS         = ACC_REG( 0x18, 0x12),
    OFFSET_CH1_SELPHS         = ACC_REG( 0x19, 0x12),
    OFFSET_CH2_SELPHS         = ACC_REG( 0x1A, 0x12),
    OFFSET_OSM_OVR            = ACC_REG( 0x1B, 0x12),
    OFFSET_CH0_OVRD           = ACC_REG( 0x1C, 0x12),
    OFFSET_CH1_OVRD           = ACC_REG( 0x1D, 0x12),
    OFFSET_CH2_OVRD           = ACC_REG( 0x1E, 0x12),
    OFFSET_CH0                = ACC_REG( 0x1F, 0x12),
    OFFSET_CH1                = ACC_REG( 0x20, 0x12),
    OFFSET_CH2                = ACC_REG( 0x21, 0x12),
    SW_OFFSETCAL_EN           = ACC_REG( 0x22, 0x12),
    OFFSETCAL_RES             = ACC_REG( 0x23, 0x12),
    GAIN_CFG1          		  = ACC_REG( 0x28, 0x12),
    GAIN_CFG2             	  = ACC_REG( 0x29, 0x12),
    GAIN_CFG3                 = ACC_REG( 0x2A, 0x12),
    GAIN_CH0                  = ACC_REG( 0x2B, 0x12),
    GAIN_CH1                  = ACC_REG( 0x2C, 0x12),
    GAIN_CH2                  = ACC_REG( 0x2D, 0x12),
    ACTIVITY                  = ACC_REG( 0x2E, 0x12),
    GAIN_TEMPKICK             = ACC_REG( 0x2F, 0x12),
    GAIN_OVR_EN               = ACC_REG( 0x30, 0x12),
    GAIN_CH0_OVRD             = ACC_REG( 0x31, 0x12),
    GAIN_CH1_OVRD             = ACC_REG( 0x32, 0x12),
    GAIN_CH2_OVRD             = ACC_REG( 0x33, 0x12),
    SW_GAINCAL_EN             = ACC_REG( 0x34, 0x12),
    GAINCAL_RES               = ACC_REG( 0x35, 0x12),
    PLL_RSV                   = ACC_REG( 0x38, 0x12),
    PLL_PFD_OVR               = ACC_REG( 0x39, 0x12),
    PLL_LOCKFILTER            = ACC_REG( 0x3A, 0x12),
    PLL_IPCP                  = ACC_REG( 0x3B, 0x12),
    PLL_TMDS_CTRL             = ACC_REG( 0x3C, 0x12),
    PLL_SEL_PHS               = ACC_REG( 0x3D, 0x12),
    PLL_STATUS                = ACC_REG( 0x3E, 0x12),
    ASU_STATE_CTRL            = ACC_REG( 0x40, 0x12),
    ASU_STATE_READ            = ACC_REG( 0x41, 0x12),
    ASU_EQ_CALTIME            = ACC_REG( 0x42, 0x12),
    ASU_NO_WAIT_ACT           = ACC_REG( 0x43, 0x12),
    ASU_USE_PLL       		  = ACC_REG( 0x44, 0x12),
    EQ_REFTIM_SLOWx4          = ACC_REG( 0x45, 0x12),
    EQ_REFTIM_MSB             = ACC_REG( 0x46, 0x12),
    EQ_REFTIM_LSB             = ACC_REG( 0x47, 0x12),
    MODE_TX_EN                = ACC_REG( 0x5A, 0x12),
    FREEZE_EN                 = ACC_REG( 0x5B, 0x12),
    MODE_DIG_EN               = ACC_REG( 0x5C, 0x12),
    TRM_INV_CMP               = ACC_REG( 0x60, 0x12),
    TRM_OVR            		  = ACC_REG( 0x61, 0x12),
    TRM_SW_CAL_EN             = ACC_REG( 0x62, 0x12),
    TRM_STATUS                = ACC_REG( 0x63, 0x12),
    CAL_RES_SEL_CH            = ACC_REG( 0x68, 0x12),
    RESULT_OFFSETCNT_MSB      = ACC_REG( 0x69, 0x12),
    RESULT_OFFSETCNT_LSB      = ACC_REG( 0x6A, 0x12),
    RESULT_STDDEV_MSB         = ACC_REG( 0x6B, 0x12),
    RESULT_STDDEV_LSB         = ACC_REG( 0x6C, 0x12),
    RESULT_EDGECOUNT_PH0_MSB  = ACC_REG( 0x6D, 0x12),
    RESULT_EDGECOUNT_PH0_LSB  = ACC_REG( 0x6E, 0x12),
    RESULT_EDGECOUNT_PH1_MSB  = ACC_REG( 0x6F, 0x12),
    RESULT_EDGECOUNT_PH1_LSB  = ACC_REG( 0x70, 0x12),
    RESULT_EDGECOUNT_PH2_MSB  = ACC_REG( 0x71, 0x12),
    RESULT_EDGECOUNT_PH2_LSB  = ACC_REG( 0x72, 0x12),
    RESULT_EDGECOUNT_PH3_MSB  = ACC_REG( 0x73, 0x12),
    RESULT_EDGECOUNT_PH3_LSB  = ACC_REG( 0x74, 0x12),
    RESULT_EDGECOUNT_PH4_MSB  = ACC_REG( 0x75, 0x12),
    RESULT_EDGECOUNT_PH4_LSB  = ACC_REG( 0x76, 0x12),
    RESULT_EDGECOUNT_PH5_MSB  = ACC_REG( 0x77, 0x12),
    RESULT_EDGECOUNT_PH5_LSB  = ACC_REG( 0x78, 0x12),
    RESULT_HISTOGRAM0_MSB     = ACC_REG( 0x79, 0x12),
    RESULT_HISTOGRAM0_LSB     = ACC_REG( 0x7A, 0x12),
    RESULT_HISTOGRAM1_MSB     = ACC_REG( 0x7B, 0x12),
    RESULT_HISTOGRAM1_LSB     = ACC_REG( 0x7C, 0x12),
    RESULT_HISTOGRAM2_MSB     = ACC_REG( 0x7D, 0x12),
    RESULT_HISTOGRAM2_LSB     = ACC_REG( 0x7E, 0x12),
    RESULT_HISTOGRAM3_MSB     = ACC_REG( 0x7F, 0x12),
    RESULT_HISTOGRAM3_LSB     = ACC_REG( 0x80, 0x12),
    RESULT_HISTOGRAM4_MSB     = ACC_REG( 0x81, 0x12),
    RESULT_HISTOGRAM4_LSB     = ACC_REG( 0x82, 0x12),
    RESULT_HISTOGRAM5_MSB     = ACC_REG( 0x83, 0x12),
    RESULT_HISTOGRAM5_LSB     = ACC_REG( 0x84, 0x12),
    RESULT_HISTOGRAM6_MSB     = ACC_REG( 0x85, 0x12),
    RESULT_HISTOGRAM6_LSB     = ACC_REG( 0x86, 0x12),
    RESULT_HISTOGRAM7_MSB     = ACC_REG( 0x87, 0x12),
    RESULT_HISTOGRAM7_LSB     = ACC_REG( 0x88, 0x12),
    DEBUG_TYPE                = ACC_REG( 0x90, 0x12),
    DEBUG_CTRL                = ACC_REG( 0x91, 0x12),
    DEBUG_NB_WORD			  = ACC_REG( 0x92, 0x12),
    DEBUG_TMDS_CLK_MSB		  = ACC_REG( 0x93, 0x12),
    DEBUG_TMDS_CLK_LSB		  = ACC_REG( 0x94, 0x12),
    DEBUG_TMDS_WORD_MSB		  = ACC_REG( 0x95, 0x12),
    DEBUG_TMDS_LSB_MSB		  = ACC_REG( 0x96, 0x12),
    DEBUG_MUX_SW_ENA		  = ACC_REG( 0x97, 0x12),
    PON_OVR_EN                = ACC_REG( 0xA0, 0x12),
    PON_CBIAS                 = ACC_REG( 0xA1, 0x12),
    PON_RESCAL                = ACC_REG( 0xA2, 0x12),
    PON_RES                   = ACC_REG( 0xA3, 0x12),
    PON_CLK                   = ACC_REG( 0xA4, 0x12),
    PON_PLL                   = ACC_REG( 0xA5, 0x12),
    PON_EQ                    = ACC_REG( 0xA6, 0x12),
    PON_DES                   = ACC_REG( 0xA7, 0x12),
    PON_OUT                   = ACC_REG( 0xA8, 0x12),
    PON_MUX                   = ACC_REG( 0xA9, 0x12),
    EQ_CALIBRATION            = ACC_REG( 0xAA, 0x12),
    BIST_CTRL1            	  = ACC_REG( 0xB0, 0x12),
    BIST_CTRL2            	  = ACC_REG( 0xB1, 0x12),
    BIST_CTRL3            	  = ACC_REG( 0xB2, 0x12),
    BIST_TCB_REG_TIMOUT    	  = ACC_REG( 0xB3, 0x12),
    BIST_CORREL_RESULT_MSB    = ACC_REG( 0xB4, 0x12),
    BIST_CORREL_RESULT_LSB    = ACC_REG( 0xB5, 0x12),
    BIST_CORREL_LENGTH3    	  = ACC_REG( 0xB6, 0x12),
    BIST_CORREL_LENGTH2       = ACC_REG( 0xB7, 0x12),
    BIST_CORREL_LENGTH1       = ACC_REG( 0xB8, 0x12),
    BIST_CORREL_LENGTH0       = ACC_REG( 0xB9, 0x12),
    BIST_GAIN_CH0             = ACC_REG( 0xBA, 0x12),
    BIST_GAIN_CH1             = ACC_REG( 0xBB, 0x12),
    BIST_GAIN_CH2             = ACC_REG( 0xBC, 0x12),
    BIST_STATUS               = ACC_REG( 0xBD, 0x12),
    INT_SET_TOP               = ACC_REG( 0xC0, 0x12),
    INT_SET_SUS               = ACC_REG( 0xC1, 0x12),
    INT_SET_DDC               = ACC_REG( 0xC2, 0x12),
    INT_SET_RATE              = ACC_REG( 0xC3, 0x12),
    INT_SET_MODE              = ACC_REG( 0xC4, 0x12),
    INT_SET_INFO              = ACC_REG( 0xC5, 0x12),
    INT_SET_AUDIO             = ACC_REG( 0xC6, 0x12),
    INT_SET_HDCP              = ACC_REG( 0xC7, 0x12),
    INT_SET_AFE               = ACC_REG( 0xC8, 0x12),
    AUDIO_LAYOUT              = ACC_REG( 0xD0, 0x12),
    SEL_RD_VAL                = ACC_REG( 0xD1, 0x12),
    FRAME_START_WIDTH         = ACC_REG( 0xD2, 0x12),
    DELTA_WR_RD_PNTR          = ACC_REG( 0xD3, 0x12),
    BCH_DECODER               = ACC_REG( 0xD4, 0x12),
    TX_BUFFER_CFG             = ACC_REG( 0xF6, 0x12),
    MODE_RECOVER_CFG1         = ACC_REG( 0xF8, 0x12),
    MODE_RECOVER_CFG2         = ACC_REG( 0xF9, 0x12),
    MODE_RECOVER_STATUS       = ACC_REG( 0xFA, 0x12),
    CURPAGE_ADR_12H           = ACC_REG( 0xFF, 0x12)
} tmbslTDA1997X_Reg_Page12;

typedef enum _tmbslTDA1997X_Reg_Page13
{
    /*************************************************************************/
    /** Rows formatted in "HDMI Driver - Register  TMDS"   updated 19972    **/
    /*************************************************************************/
    DEEP_COLOR_CTRL           		= ACC_REG( 0x00, 0x13),
    DEEP_COLOR_MAN            		= ACC_REG( 0x01, 0x13),
    DEEP_COLOR_GCP            		= ACC_REG( 0x02, 0x13),
    IVS_SEL		              		= ACC_REG( 0x03, 0x13),
    CGU_DEBUG_INV             		= ACC_REG( 0x04, 0x13),
    CGU_DEBUG_SEL             		= ACC_REG( 0x05, 0x13),
    X_OUTPUTS             			= ACC_REG( 0x06, 0x13),
    VP_DEBUG             			= ACC_REG( 0x07, 0x13),
    DEEP_COLOR_CLK_CTRL    			= ACC_REG( 0x08, 0x13),
    DC_PAT0_MSB             		= ACC_REG( 0x09, 0x13),
    DC_PAT1_MSB             		= ACC_REG( 0x0A, 0x13),
    DC_PAT2_MSB             		= ACC_REG( 0x0B, 0x13),
    DC_PAT3_MSB             		= ACC_REG( 0x0C, 0x13),
    DC_PAT4_MSB             		= ACC_REG( 0x0D, 0x13),
    DC_PAT3210_LSB             		= ACC_REG( 0x0E, 0x13),
    DEEP_COLOR_PAT_MAN         		= ACC_REG( 0x0F, 0x13),
    HDCP_DDC_ADDR             		= ACC_REG( 0x10, 0x13),
    HDCP_BKSV4                		= ACC_REG( 0x11, 0x13),
    HDCP_BKSV3                		= ACC_REG( 0x12, 0x13),
    HDCP_BKSV2                		= ACC_REG( 0x13, 0x13),
    HDCP_BKSV1                		= ACC_REG( 0x14, 0x13),
    HDCP_BKSV0                		= ACC_REG( 0x15, 0x13),
    HDCP_KIDX                 		= ACC_REG( 0x16, 0x13),
    HDCP_KEY6                 		= ACC_REG( 0x17, 0x13),
    HDCP_KEY5                 		= ACC_REG( 0x18, 0x13),
    HDCP_KEY4                 		= ACC_REG( 0x19, 0x13),
    HDCP_KEY3                 		= ACC_REG( 0x1A, 0x13),
    HDCP_KEY2                 		= ACC_REG( 0x1B, 0x13),
    HDCP_KEY1                 		= ACC_REG( 0x1C, 0x13),
    HDCP_KEY0                 		= ACC_REG( 0x1D, 0x13),
    /*
    DEEP_PLL0                 		= ACC_REG( 0x40, 0x13),
    DEEP_PLL1                 		= ACC_REG( 0x41, 0x13),
    DEEP_PLL2                 		= ACC_REG( 0x42, 0x13),
    DEEP_PLL3                 		= ACC_REG( 0x43, 0x13),
    DEEP_PLL4                 		= ACC_REG( 0x44, 0x13),
    DEEP_PLL5                 		= ACC_REG( 0x45, 0x13),
    DEEP_PLL6                 		= ACC_REG( 0x46, 0x13),
    DEEP_PLL7                 		= ACC_REG( 0x47, 0x13),
    SOFTAFC_MSB       	  	  		= ACC_REG( 0x50, 0x13),
    SOFTAFC_LSB         	  		= ACC_REG( 0x51, 0x13),
    FIFO_LATENCY_CTRL         		= ACC_REG( 0x52, 0x13),
    FIFO_LATENCY	          		= ACC_REG( 0x53, 0x13),
    FIFO_CTL_COEFF_W          		= ACC_REG( 0x54, 0x13),
    FIFO_CTL_COEFF_WR         		= ACC_REG( 0x55, 0x13),
    AFC_ODPRX_MSB         	  		= ACC_REG( 0x56, 0x13),
    AFC_ODPRX_LSB         	  		= ACC_REG( 0x57, 0x13),
    FIFO_CTL_DPRX_MIN         		= ACC_REG( 0x58, 0x13),
    FIFO_CTL_DPRX_MAX         		= ACC_REG( 0x59, 0x13),
    FIFO_CTL_DPRX_LSB         		= ACC_REG( 0x5A, 0x13),
    FIFO_LATENCY_RD           		= ACC_REG( 0x5B, 0x13),
    FIFO_CTL_COEFF_KP_LOW_RD  		= ACC_REG( 0x5C, 0x13),
    FIFO_CTL_COEFF_ZETA_UNMUTE_RD 	= ACC_REG( 0x5D, 0x13),
    FIFO_CTL_COEFF_ROUND_RD	  		= ACC_REG( 0x5E, 0x13),
    FIFO_CTL_COEFF_ROUND_WR	  		= ACC_REG( 0x5F, 0x13),
    */
    HMTP_T_RESET               		= ACC_REG( 0x60, 0x13),
    HMTP_T_ADDRESS_SETUP       		= ACC_REG( 0x61, 0x13),
    HMTP_T_ADDRESS_HOLD            	= ACC_REG( 0x62, 0x13),
    HMTP_T_READ_ENABLE         		= ACC_REG( 0x63, 0x13),
    HMTP_T_WRITE_ENABLE       		= ACC_REG( 0x64, 0x13),
    HMTP_T_INITIALIZATION      		= ACC_REG( 0x65, 0x13),
    HMTP_T_ERASE_PRG_PROGRESS   	= ACC_REG( 0x66, 0x13),
    HMTP_T_DISCHARGE       			= ACC_REG( 0x67, 0x13),
    HDCP_SRAM_REGS       			= ACC_REG( 0x68, 0x13),
    HDCP_DMA_CONF       			= ACC_REG( 0x69, 0x13),
    HMTP_ECC_REGISTERS       		= ACC_REG( 0x6A, 0x13),
    HMTP_ERR_ADDR       			= ACC_REG( 0x6B, 0x13),
    HDCP_MTP_TEST       			= ACC_REG( 0x6C, 0x13),
    HMTP_PARAM       				= ACC_REG( 0x6D, 0x13),
    HDCP_DE_CTRL	       			= ACC_REG( 0x70, 0x13),
    HDCP_EP_FILT_CTRL   			= ACC_REG( 0x71, 0x13),
    HDCP_EP_REM_CTRL       			= ACC_REG( 0x72, 0x13),
    HDCP_EP_READ      				= ACC_REG( 0x73, 0x13),
    HDCP_PA_CTRL       				= ACC_REG( 0x74, 0x13),
    HDCP_PA_READ       				= ACC_REG( 0x75, 0x13),
    HDCP_MISC       				= ACC_REG( 0x76, 0x13),
    HDMI_CTRL       				= ACC_REG( 0x77, 0x13),
    HDMI_POL       					= ACC_REG( 0x78, 0x13),
    HDCP_MODE_RECOVERY  			= ACC_REG( 0x79, 0x13),
    HMTP_CTRL  						= ACC_REG( 0x7A, 0x13),
    HMTP_ADDRESS  					= ACC_REG( 0x7B, 0x13),
    EMTP_T_RESET       				= ACC_REG( 0x80, 0x13),
    EMTP_T_ADDRESS_SETUP			= ACC_REG( 0x81, 0x13),
    EMTP_T_ADDRESS_HOLD 			= ACC_REG( 0x82, 0x13),
    EMTP_T_READ_ENABLE  			= ACC_REG( 0x83, 0x13),
    EMTP_T_WRITE_ENABLE 			= ACC_REG( 0x84, 0x13),
    EMTP_T_INITIALIZATION			= ACC_REG( 0x85, 0x13),
    EMTP_T_ERASE_PRG_PROGRESS      	= ACC_REG( 0x86, 0x13),
    EMTP_T_DISCHARGE        		= ACC_REG( 0x87, 0x13),
    DDC_HIDDEN0_REGISTER    		= ACC_REG( 0x90, 0x13),
    DDC_HIDDEN1_REGISTER    		= ACC_REG( 0x91, 0x13),
    DDC_DMA_CONF           			= ACC_REG( 0x92, 0x13),
    EMTP_ECC_CONF           		= ACC_REG( 0x93, 0x13),
    EMTP_ERR_ADDR         			= ACC_REG( 0x94, 0x13),
    HDCP_ACK_TIMER         			= ACC_REG( 0x96, 0x13),
    HDCP_NOACK_TIMER         		= ACC_REG( 0x97, 0x13),
    EMTP_PARAM         				= ACC_REG( 0x98, 0x13),
    DDC_MTP_VANALOG    				= ACC_REG( 0x99, 0x13),
    DDC_MTP_TEST       				= ACC_REG( 0x9A, 0x13),
    DDC_MST_CTRL           			= ACC_REG( 0xA0, 0x13),
    DDC_MST_CLK_DIV           		= ACC_REG( 0xA1, 0x13),
    DDC_MST_CLK_DIV_FOR_MST         = ACC_REG( 0xA2, 0x13),
    DDC_LINK_SEL           			= ACC_REG( 0xA3, 0x13),
    DDC_STATUS           			= ACC_REG( 0xA4, 0x13),
    CLOCK_CTRL           			= ACC_REG( 0xA5, 0x13),
    MONITOR_OFFSET_CH0_AB           = ACC_REG( 0xC0, 0x13),
    MONITOR_OFFSET_CH1_AB           = ACC_REG( 0xC1, 0x13),
    MONITOR_OFFSET_CH2_AB           = ACC_REG( 0xC2, 0x13),
    MONITOR_GAIN_CH0_AB           	= ACC_REG( 0xC3, 0x13),
    MONITOR_GAIN_CH1_AB           	= ACC_REG( 0xC4, 0x13),
    MONITOR_GAIN_CH2_AB           	= ACC_REG( 0xC5, 0x13),
    SUS_CTRL           				= ACC_REG( 0xC6, 0x13),
    SUS_CTRL2           			= ACC_REG( 0xC7, 0x13),
    RESET_REFTIM_MSB           		= ACC_REG( 0xC8, 0x13),
    RESET_REFTIM_LSB           		= ACC_REG( 0xC9, 0x13),
    TIMER_RT                   		= ACC_REG( 0xCA, 0x13),
    TIMER_A_MSB                   	= ACC_REG( 0xCB, 0x13),
    TIMER_B_MSB                     = ACC_REG( 0xCC, 0x13),
    TIMER_C_MSB                     = ACC_REG( 0xCD, 0x13),
    TIMER_ABC_LSB                   = ACC_REG( 0xCE, 0x13),
    TIMER_D                 		= ACC_REG( 0xCF, 0x13),
    SUS_SET_CAL0                 	= ACC_REG( 0xD0, 0x13),
    SUS_SET_CAL1                  	= ACC_REG( 0xD1, 0x13),
    SUS_SET_CAL2          			= ACC_REG( 0xD2, 0x13),
    SUS_SET_CAL3          			= ACC_REG( 0xD3, 0x13),
    SUS_SET_CAL4          			= ACC_REG( 0xD4, 0x13),
    SUS_SET_CAL5          			= ACC_REG( 0xD5, 0x13),
    SUS_SET_CAL6          			= ACC_REG( 0xD6, 0x13),
    SUS_SET_CAL7          			= ACC_REG( 0xD7, 0x13),
    SUS_SET_CAL8          			= ACC_REG( 0xD8, 0x13),
    SUS_SET_HDMI0          			= ACC_REG( 0xD9, 0x13),
    SUS_SET_HDMI1          			= ACC_REG( 0xDA, 0x13),
    SUS_SET_HDMI2          			= ACC_REG( 0xDB, 0x13),
    SUS_SET_HDMI3          			= ACC_REG( 0xDC, 0x13),
    SUS_SET_HDMI4        			= ACC_REG( 0xDD, 0x13),
    SUS_SET_HDMI5               	= ACC_REG( 0xDE, 0x13),
    SUS_SET_HDMI6		            = ACC_REG( 0xDF, 0x13),
    SUS_SET_HDMI7                  	= ACC_REG( 0xE0, 0x13),
    SUS_SET_RGB0                  	= ACC_REG( 0xE1, 0x13),
    SUS_SET_RGB1          			= ACC_REG( 0xE2, 0x13),
    SUS_SET_RGB2          			= ACC_REG( 0xE3, 0x13),
    SUS_SET_RGB3          			= ACC_REG( 0xE4, 0x13),
    SUS_SET_RGB4          			= ACC_REG( 0xE5, 0x13),
    MAN_SUS_CAL_SEL          		= ACC_REG( 0xE6, 0x13),
    MAN_CAL_SET          			= ACC_REG( 0xE7, 0x13),
    MAN_SUS_HDMI_SEL          		= ACC_REG( 0xE8, 0x13),
    MAN_HDMI_SET          			= ACC_REG( 0xE9, 0x13),
    MAN_SUS_RGB_SEL          		= ACC_REG( 0xEA, 0x13),
    MAN_RGB_SET          			= ACC_REG( 0xEB, 0x13),
    TIMER_ENCODED          			= ACC_REG( 0xEC, 0x13),
    RATE_REFTIM_MSB        			= ACC_REG( 0xED, 0x13),
    RATE_REFTIM_LSB               	= ACC_REG( 0xEE, 0x13),
    SUS_CLOCK_GOOD		            = ACC_REG( 0xEF, 0x13),
    SUS_FREQ_DIGITAL_MSB            = ACC_REG( 0xF0, 0x13),
    SUS_FREQ_DIGITAL_ISB            = ACC_REG( 0xF1, 0x13),
    SUS_FREQ_DIGITAL_LSB       		= ACC_REG( 0xF2, 0x13),
    SUS_GAIN_LIM          			= ACC_REG( 0xF3, 0x13),
    CURPAGE_ADR_13H           		= ACC_REG( 0xFF, 0x13)
} tmbslTDA1997X_Reg_Page13;


typedef enum _tmbslTDA1997X_Reg_Page14
{
    SOFT_AFC_CTRL			= ACC_REG( 0x00, 0x14),
    SOFT_AFC_LSB     		= ACC_REG( 0x01, 0x14),
    FIFO_LATENCY_CTRL		= ACC_REG( 0x02, 0x14),
    FIFO_LATENCY_VALUE		= ACC_REG( 0x03, 0x14),
    FIFO_CTRL_COEFF0		= ACC_REG( 0x04, 0x14),
    FIFO_CTRL_COEFF1		= ACC_REG( 0x05, 0x14),
    FIFO_CTRL_COEFF2		= ACC_REG( 0x06, 0x14),
    AFC_ODPRX_MSB			= ACC_REG( 0x07, 0x14),
    AFC_ODPRX_LSB			= ACC_REG( 0x08, 0x14),
    DPRX_MIN				= ACC_REG( 0x09, 0x14),
    DPRX_MAX	    		= ACC_REG( 0x0A, 0x14),
    DPRX_LSB_MIN_MAX		= ACC_REG( 0x0B, 0x14),
    FIFO_LATENCY_VALUE_RD	= ACC_REG( 0x0C, 0x14),
    FIFO_CTRL_COEFF0_RD	    = ACC_REG( 0x0D, 0x14),
    FIFO_CTRL_COEFF1_RD		= ACC_REG( 0x0E, 0x14),
    FIFO_CTRL_COEFF2_RD		= ACC_REG( 0x0F, 0x14),
    AUDIO_PWR_CTRL			= ACC_REG( 0x10, 0x14),
    AUDIO_CLOCK_MODE		= ACC_REG( 0x11, 0x14),
    AUDIO_HSPLL_CTRL		= ACC_REG( 0x12, 0x14),
    TEST_NCTS_CTRL			= ACC_REG( 0x15, 0x14),
    TEST_CTS_CHECKED_MSB	= ACC_REG( 0x1A, 0x14),
    TEST_CTS_CHECKED_ISB	= ACC_REG( 0x1B, 0x14),
    TEST_CTS_CHECKED_LSB	= ACC_REG( 0x1C, 0x14),
    TEST_N_CHECKED_MSB		= ACC_REG( 0x1D, 0x14),
    TEST_N_CHECKED_ISB		= ACC_REG( 0x1E, 0x14),
    TEST_N_CHECKED_LSB		= ACC_REG( 0x1F, 0x14),
    TEST_NCTS_CHECKED_VALID	= ACC_REG( 0x20, 0x14),
    TEST_FDET_QUOTIENT_MSB	= ACC_REG( 0x24, 0x14),
    TEST_FDET_QUOTIENT_LSB	= ACC_REG( 0x25, 0x14),
    TEST_AUDIO_FREQ			= ACC_REG( 0x26, 0x14),
    TEST_Q_DIV_SYNC			= ACC_REG( 0x2C, 0x14),
    TEST_R_DIV_SYNC_MSB		= ACC_REG( 0x2D, 0x14),
    TEST_R_DIV_SYNC_NSB 	= ACC_REG( 0x2E, 0x14),
    TEST_R_DIV_SYNC_ISB 	= ACC_REG( 0x2F, 0x14),
    TEST_R_DIV_SYNC_LSB 	= ACC_REG( 0x30, 0x14),
    TEST_N_DIV_SYNC_MSB 	= ACC_REG( 0x31, 0x14),
    TEST_N_DIV_SYNC_NSB 	= ACC_REG( 0x32, 0x14),
    TEST_N_DIV_SYNC_ISB 	= ACC_REG( 0x33, 0x14),
    TEST_N_DIV_SYNC_LSB 	= ACC_REG( 0x34, 0x14),
    TEST_MODE				= ACC_REG( 0x37, 0x14),
    NCTS_CHECKED_VALID_OUT	= ACC_REG( 0x3D, 0x14),
    CTS_CHECKED_MSB_OUT		= ACC_REG( 0x3E, 0x14),
    CTS_CHECKED_ISB_OUT		= ACC_REG( 0x3F, 0x14),
    CTS_CHECKED_LSB_OUT		= ACC_REG( 0x40, 0x14),
    N_CHECKED_MSB_OUT		= ACC_REG( 0x41, 0x14),
    N_CHECKED_ISB_OUT		= ACC_REG( 0x42, 0x14),
    N_CHECKED_LSB_OUT		= ACC_REG( 0x43, 0x14),
    FDET_QUOTIENT_MSB_OUT	= ACC_REG( 0x47, 0x14),
    FDET_QUOTIENT_LSB_OUT	= ACC_REG( 0x48, 0x14),
    FDET_REMAIN_MSB_OUT		= ACC_REG( 0x4A, 0x14),
    FDET_REMAIN_ISB_OUT		= ACC_REG( 0x4B, 0x14),
    FDET_REMAIN_LSB_OUT		= ACC_REG( 0x4C, 0x14),
    AUDIO_STATUS			= ACC_REG( 0x4D, 0x14),
    Q_DIV_SYNC_OUT			= ACC_REG( 0x51, 0x14),
    R_DIV_SYNC_MSB			= ACC_REG( 0x52, 0x14),
    R_DIV_SYNC_NSB			= ACC_REG( 0x53, 0x14),
    R_DIV_SYNC_ISB			= ACC_REG( 0x54, 0x14),
    R_DIV_SYNC_LSB			= ACC_REG( 0x55, 0x14),
    N_DIV_SYNC_MSB			= ACC_REG( 0x56, 0x14),
    N_DIV_SYNC_NSB			= ACC_REG( 0x57, 0x14),
    N_DIV_SYNC_ISB			= ACC_REG( 0x58, 0x14),
    N_DIV_SYNC_LSB			= ACC_REG( 0x59, 0x14),
    AUDIO_PLL_STATUS		= ACC_REG( 0x5A, 0x14),
    CTS_MSB					= ACC_REG( 0x5F, 0x14),
    CTS_ISB					= ACC_REG( 0x60, 0x14),
    CTS_LSB					= ACC_REG( 0x61, 0x14),
    N_MSB					= ACC_REG( 0x62, 0x14),
    N_ISB					= ACC_REG( 0x63, 0x14),
    N_LSB					= ACC_REG( 0x64, 0x14),
    CURPAGE_ADR_14H         = ACC_REG( 0xFF, 0x14)
} tmbslTDA1997X_Reg_Page14;


typedef enum _tmbslTDA1997X_Reg_Page20
{
    EDID_IN_BYTE0		= ACC_REG( 0x00, 0x20),
    EDID_IN_BYTE1     	= ACC_REG( 0x01, 0x20),
    EDID_IN_BYTE2		= ACC_REG( 0x02, 0x20),
    EDID_IN_BYTE3		= ACC_REG( 0x03, 0x20),
    EDID_IN_BYTE4		= ACC_REG( 0x04, 0x20),
    EDID_IN_BYTE5		= ACC_REG( 0x05, 0x20),
    EDID_IN_BYTE6		= ACC_REG( 0x06, 0x20),
    EDID_IN_BYTE7		= ACC_REG( 0x07, 0x20),
    EDID_IN_BYTE8		= ACC_REG( 0x08, 0x20),
    EDID_IN_BYTE9		= ACC_REG( 0x09, 0x20),
    EDID_IN_BYTE10	    = ACC_REG( 0x0A, 0x20),
    EDID_IN_BYTE11		= ACC_REG( 0x0B, 0x20),
    EDID_IN_BYTE12	    = ACC_REG( 0x0C, 0x20),
    EDID_IN_BYTE13	    = ACC_REG( 0x0D, 0x20),
    EDID_IN_BYTE14		= ACC_REG( 0x0E, 0x20),
    EDID_IN_BYTE15		= ACC_REG( 0x0F, 0x20),
    EDID_IN_BYTE16		= ACC_REG( 0x10, 0x20),
    EDID_IN_BYTE17		= ACC_REG( 0x11, 0x20),
    EDID_IN_BYTE18		= ACC_REG( 0x12, 0x20),
    EDID_IN_BYTE19		= ACC_REG( 0x13, 0x20),
    EDID_IN_BYTE20		= ACC_REG( 0x14, 0x20),
    EDID_IN_BYTE21		= ACC_REG( 0x15, 0x20),
    EDID_IN_BYTE22		= ACC_REG( 0x16, 0x20),
    EDID_IN_BYTE23		= ACC_REG( 0x17, 0x20),
    EDID_IN_BYTE24		= ACC_REG( 0x18, 0x20),
    EDID_IN_BYTE25		= ACC_REG( 0x19, 0x20),
    EDID_IN_BYTE26		= ACC_REG( 0x1A, 0x20),
    EDID_IN_BYTE27		= ACC_REG( 0x1B, 0x20),
    EDID_IN_BYTE28		= ACC_REG( 0x1C, 0x20),
    EDID_IN_BYTE29		= ACC_REG( 0x1D, 0x20),
    EDID_IN_BYTE30		= ACC_REG( 0x1E, 0x20),
    EDID_IN_BYTE31		= ACC_REG( 0x1F, 0x20),
    EDID_IN_BYTE32		= ACC_REG( 0x20, 0x20),
    EDID_IN_BYTE33		= ACC_REG( 0x21, 0x20),
    EDID_IN_BYTE34		= ACC_REG( 0x22, 0x20),
    EDID_IN_BYTE35		= ACC_REG( 0x23, 0x20),
    EDID_IN_BYTE36		= ACC_REG( 0x24, 0x20),
    EDID_IN_BYTE37		= ACC_REG( 0x25, 0x20),
    EDID_IN_BYTE38		= ACC_REG( 0x26, 0x20),
    EDID_IN_BYTE39		= ACC_REG( 0x27, 0x20),
    EDID_IN_BYTE40		= ACC_REG( 0x28, 0x20),
    EDID_IN_BYTE41		= ACC_REG( 0x29, 0x20),
    EDID_IN_BYTE42		= ACC_REG( 0x2A, 0x20),
    EDID_IN_BYTE43		= ACC_REG( 0x2B, 0x20),
    EDID_IN_BYTE44		= ACC_REG( 0x2C, 0x20),
    EDID_IN_BYTE45		= ACC_REG( 0x2D, 0x20),
    EDID_IN_BYTE46 		= ACC_REG( 0x2E, 0x20),
    EDID_IN_BYTE47 		= ACC_REG( 0x2F, 0x20),
    EDID_IN_BYTE48 		= ACC_REG( 0x30, 0x20),
    EDID_IN_BYTE49 		= ACC_REG( 0x31, 0x20),
    EDID_IN_BYTE50 		= ACC_REG( 0x32, 0x20),
    EDID_IN_BYTE51 		= ACC_REG( 0x33, 0x20),
    EDID_IN_BYTE52 		= ACC_REG( 0x34, 0x20),
    EDID_IN_BYTE53 		= ACC_REG( 0x35, 0x20),
    EDID_IN_BYTE54		= ACC_REG( 0x36, 0x20),
    EDID_IN_BYTE55		= ACC_REG( 0x37, 0x20),
    EDID_IN_BYTE56		= ACC_REG( 0x38, 0x20),
    EDID_IN_BYTE57		= ACC_REG( 0x39, 0x20),
    EDID_IN_BYTE58		= ACC_REG( 0x3A, 0x20),
    EDID_IN_BYTE59		= ACC_REG( 0x3B, 0x20),
    EDID_IN_BYTE60		= ACC_REG( 0x3C, 0x20),
    EDID_IN_BYTE61		= ACC_REG( 0x3D, 0x20),
    EDID_IN_BYTE62		= ACC_REG( 0x3E, 0x20),
    EDID_IN_BYTE63		= ACC_REG( 0x3F, 0x20),
    EDID_IN_BYTE64		= ACC_REG( 0x40, 0x20),
    EDID_IN_BYTE65		= ACC_REG( 0x41, 0x20),
    EDID_IN_BYTE66		= ACC_REG( 0x42, 0x20),
    EDID_IN_BYTE67		= ACC_REG( 0x43, 0x20),
    EDID_IN_BYTE68		= ACC_REG( 0x44, 0x20),
    EDID_IN_BYTE69		= ACC_REG( 0x45, 0x20),
    EDID_IN_BYTE70		= ACC_REG( 0x46, 0x20),
    EDID_IN_BYTE71		= ACC_REG( 0x47, 0x20),
    EDID_IN_BYTE72		= ACC_REG( 0x48, 0x20),
    EDID_IN_BYTE73		= ACC_REG( 0x49, 0x20),
    EDID_IN_BYTE74		= ACC_REG( 0x4A, 0x20),
    EDID_IN_BYTE75		= ACC_REG( 0x4B, 0x20),
    EDID_IN_BYTE76		= ACC_REG( 0x4C, 0x20),
    EDID_IN_BYTE77		= ACC_REG( 0x4D, 0x20),
    EDID_IN_BYTE78		= ACC_REG( 0x4E, 0x20),
    EDID_IN_BYTE79		= ACC_REG( 0x4F, 0x20),
    EDID_IN_BYTE80		= ACC_REG( 0x50, 0x20),
    EDID_IN_BYTE81		= ACC_REG( 0x51, 0x20),
    EDID_IN_BYTE82		= ACC_REG( 0x52, 0x20),
    EDID_IN_BYTE83		= ACC_REG( 0x53, 0x20),
    EDID_IN_BYTE84		= ACC_REG( 0x54, 0x20),
    EDID_IN_BYTE85		= ACC_REG( 0x55, 0x20),
    EDID_IN_BYTE86		= ACC_REG( 0x56, 0x20),
    EDID_IN_BYTE87		= ACC_REG( 0x57, 0x20),
    EDID_IN_BYTE88		= ACC_REG( 0x58, 0x20),
    EDID_IN_BYTE89		= ACC_REG( 0x59, 0x20),
    EDID_IN_BYTE90		= ACC_REG( 0x5A, 0x20),
    EDID_IN_BYTE91		= ACC_REG( 0x5B, 0x20),
    EDID_IN_BYTE92		= ACC_REG( 0x5C, 0x20),
    EDID_IN_BYTE93		= ACC_REG( 0x5D, 0x20),
    EDID_IN_BYTE94		= ACC_REG( 0x5E, 0x20),
    EDID_IN_BYTE95		= ACC_REG( 0x5F, 0x20),
    EDID_IN_BYTE96		= ACC_REG( 0x60, 0x20),
    EDID_IN_BYTE97		= ACC_REG( 0x61, 0x20),
    EDID_IN_BYTE98		= ACC_REG( 0x62, 0x20),
    EDID_IN_BYTE99		= ACC_REG( 0x63, 0x20),
    EDID_IN_BYTE100		= ACC_REG( 0x64, 0x20),
    EDID_IN_BYTE101		= ACC_REG( 0x65, 0x20),
    EDID_IN_BYTE102		= ACC_REG( 0x66, 0x20),
    EDID_IN_BYTE103		= ACC_REG( 0x67, 0x20),
    EDID_IN_BYTE104		= ACC_REG( 0x68, 0x20),
    EDID_IN_BYTE105		= ACC_REG( 0x69, 0x20),
    EDID_IN_BYTE106		= ACC_REG( 0x6A, 0x20),
    EDID_IN_BYTE107		= ACC_REG( 0x6B, 0x20),
    EDID_IN_BYTE108		= ACC_REG( 0x6C, 0x20),
    EDID_IN_BYTE109		= ACC_REG( 0x6D, 0x20),
    EDID_IN_BYTE110		= ACC_REG( 0x6E, 0x20),
    EDID_IN_BYTE111		= ACC_REG( 0x6F, 0x20),
    EDID_IN_BYTE112		= ACC_REG( 0x70, 0x20),
    EDID_IN_BYTE113		= ACC_REG( 0x71, 0x20),
    EDID_IN_BYTE114		= ACC_REG( 0x72, 0x20),
    EDID_IN_BYTE115		= ACC_REG( 0x73, 0x20),
    EDID_IN_BYTE116		= ACC_REG( 0x74, 0x20),
    EDID_IN_BYTE117		= ACC_REG( 0x75, 0x20),
    EDID_IN_BYTE118		= ACC_REG( 0x76, 0x20),
    EDID_IN_BYTE119		= ACC_REG( 0x77, 0x20),
    EDID_IN_BYTE120		= ACC_REG( 0x78, 0x20),
    EDID_IN_BYTE121		= ACC_REG( 0x79, 0x20),
    EDID_IN_BYTE122		= ACC_REG( 0x7A, 0x20),
    EDID_IN_BYTE123		= ACC_REG( 0x7B, 0x20),
    EDID_IN_BYTE124		= ACC_REG( 0x7C, 0x20),
    EDID_IN_BYTE125		= ACC_REG( 0x7D, 0x20),
    EDID_IN_BYTE126		= ACC_REG( 0x7E, 0x20),
    EDID_IN_BYTE127		= ACC_REG( 0x7F, 0x20),
    EDID_IN_VERSION		= ACC_REG( 0x80, 0x20),
    EDID_ENABLE			= ACC_REG( 0x81, 0x20),
    EDID_BLOCK_SELECT	= ACC_REG( 0x82, 0x20),
    HPD_POWER			= ACC_REG( 0x84, 0x20),
    HPD_AUTO_CTRL		= ACC_REG( 0x85, 0x20),
    HPD_DURATION		= ACC_REG( 0x86, 0x20),
    RX_HPD_HEAC			= ACC_REG( 0x87, 0x20),
    EDID_BLOCK0_TEST1	= ACC_REG( 0x8D, 0x20),
    EDID_BLOCK0_TEST2	= ACC_REG( 0x8E, 0x20),
    EDID_BLOCK0_TEST3	= ACC_REG( 0x8F, 0x20),
    CURPAGE_ADR_20H     = ACC_REG( 0xFF, 0x20)
} tmbslTDA1997X_Reg_Page20;

typedef enum _tmbslTDA1997X_Reg_Page21
{
    EDID_IN_BYTE128		= ACC_REG( 0x00, 0x21),
    EDID_IN_BYTE129     = ACC_REG( 0x01, 0x21),
    EDID_IN_BYTE130		= ACC_REG( 0x02, 0x21),
    EDID_IN_BYTE131		= ACC_REG( 0x03, 0x21),
    EDID_IN_BYTE132		= ACC_REG( 0x04, 0x21),
    EDID_IN_BYTE133		= ACC_REG( 0x05, 0x21),
    EDID_IN_BYTE134		= ACC_REG( 0x06, 0x21),
    EDID_IN_BYTE135		= ACC_REG( 0x07, 0x21),
    EDID_IN_BYTE136		= ACC_REG( 0x08, 0x21),
    EDID_IN_BYTE137		= ACC_REG( 0x09, 0x21),
    EDID_IN_BYTE138	    = ACC_REG( 0x0A, 0x21),
    EDID_IN_BYTE139		= ACC_REG( 0x0B, 0x21),
    EDID_IN_BYTE140	    = ACC_REG( 0x0C, 0x21),
    EDID_IN_BYTE141	    = ACC_REG( 0x0D, 0x21),
    EDID_IN_BYTE142		= ACC_REG( 0x0E, 0x21),
    EDID_IN_BYTE143		= ACC_REG( 0x0F, 0x21),
    EDID_IN_BYTE144		= ACC_REG( 0x10, 0x21),
    EDID_IN_BYTE145		= ACC_REG( 0x11, 0x21),
    EDID_IN_BYTE146		= ACC_REG( 0x12, 0x21),
    EDID_IN_BYTE147		= ACC_REG( 0x13, 0x21),
    EDID_IN_BYTE148		= ACC_REG( 0x14, 0x21),
    EDID_IN_BYTE149		= ACC_REG( 0x15, 0x21),
    EDID_IN_BYTE150		= ACC_REG( 0x16, 0x21),
    EDID_IN_BYTE151		= ACC_REG( 0x17, 0x21),
    EDID_IN_BYTE152		= ACC_REG( 0x18, 0x21),
    EDID_IN_BYTE153		= ACC_REG( 0x19, 0x21),
    EDID_IN_BYTE154		= ACC_REG( 0x1A, 0x21),
    EDID_IN_BYTE155		= ACC_REG( 0x1B, 0x21),
    EDID_IN_BYTE156		= ACC_REG( 0x1C, 0x21),
    EDID_IN_BYTE157		= ACC_REG( 0x1D, 0x21),
    EDID_IN_BYTE158		= ACC_REG( 0x1E, 0x21),
    EDID_IN_BYTE159		= ACC_REG( 0x1F, 0x21),
    EDID_IN_BYTE160		= ACC_REG( 0x20, 0x21),
    EDID_IN_BYTE161		= ACC_REG( 0x21, 0x21),
    EDID_IN_BYTE162		= ACC_REG( 0x22, 0x21),
    EDID_IN_BYTE163		= ACC_REG( 0x23, 0x21),
    EDID_IN_BYTE164		= ACC_REG( 0x24, 0x21),
    EDID_IN_BYTE165		= ACC_REG( 0x25, 0x21),
    EDID_IN_BYTE166		= ACC_REG( 0x26, 0x21),
    EDID_IN_BYTE167		= ACC_REG( 0x27, 0x21),
    EDID_IN_BYTE168		= ACC_REG( 0x28, 0x21),
    EDID_IN_BYTE169		= ACC_REG( 0x29, 0x21),
    EDID_IN_BYTE170		= ACC_REG( 0x2A, 0x21),
    EDID_IN_BYTE171		= ACC_REG( 0x2B, 0x21),
    EDID_IN_BYTE172		= ACC_REG( 0x2C, 0x21),
    EDID_IN_BYTE173		= ACC_REG( 0x2D, 0x21),
    EDID_IN_BYTE174 	= ACC_REG( 0x2E, 0x21),
    EDID_IN_BYTE175 	= ACC_REG( 0x2F, 0x21),
    EDID_IN_BYTE176 	= ACC_REG( 0x30, 0x21),
    EDID_IN_BYTE177 	= ACC_REG( 0x31, 0x21),
    EDID_IN_BYTE178 	= ACC_REG( 0x32, 0x21),
    EDID_IN_BYTE179 	= ACC_REG( 0x33, 0x21),
    EDID_IN_BYTE180 	= ACC_REG( 0x34, 0x21),
    EDID_IN_BYTE181 	= ACC_REG( 0x35, 0x21),
    EDID_IN_BYTE182		= ACC_REG( 0x36, 0x21),
    EDID_IN_BYTE183		= ACC_REG( 0x37, 0x21),
    EDID_IN_BYTE184		= ACC_REG( 0x38, 0x21),
    EDID_IN_BYTE185		= ACC_REG( 0x39, 0x21),
    EDID_IN_BYTE186		= ACC_REG( 0x3A, 0x21),
    EDID_IN_BYTE187		= ACC_REG( 0x3B, 0x21),
    EDID_IN_BYTE188		= ACC_REG( 0x3C, 0x21),
    EDID_IN_BYTE189		= ACC_REG( 0x3D, 0x21),
    EDID_IN_BYTE190		= ACC_REG( 0x3E, 0x21),
    EDID_IN_BYTE191		= ACC_REG( 0x3F, 0x21),
    EDID_IN_BYTE192		= ACC_REG( 0x40, 0x21),
    EDID_IN_BYTE193		= ACC_REG( 0x41, 0x21),
    EDID_IN_BYTE194		= ACC_REG( 0x42, 0x21),
    EDID_IN_BYTE195		= ACC_REG( 0x43, 0x21),
    EDID_IN_BYTE196		= ACC_REG( 0x44, 0x21),
    EDID_IN_BYTE197		= ACC_REG( 0x45, 0x21),
    EDID_IN_BYTE198		= ACC_REG( 0x46, 0x21),
    EDID_IN_BYTE199		= ACC_REG( 0x47, 0x21),
    EDID_IN_BYTE200		= ACC_REG( 0x48, 0x21),
    EDID_IN_BYTE201		= ACC_REG( 0x49, 0x21),
    EDID_IN_BYTE202		= ACC_REG( 0x4A, 0x21),
    EDID_IN_BYTE203		= ACC_REG( 0x4B, 0x21),
    EDID_IN_BYTE204		= ACC_REG( 0x4C, 0x21),
    EDID_IN_BYTE205		= ACC_REG( 0x4D, 0x21),
    EDID_IN_BYTE206		= ACC_REG( 0x4E, 0x21),
    EDID_IN_BYTE207		= ACC_REG( 0x4F, 0x21),
    EDID_IN_BYTE208		= ACC_REG( 0x50, 0x21),
    EDID_IN_BYTE209		= ACC_REG( 0x51, 0x21),
    EDID_IN_BYTE210		= ACC_REG( 0x52, 0x21),
    EDID_IN_BYTE211		= ACC_REG( 0x53, 0x21),
    EDID_IN_BYTE212		= ACC_REG( 0x54, 0x21),
    EDID_IN_BYTE213		= ACC_REG( 0x55, 0x21),
    EDID_IN_BYTE214		= ACC_REG( 0x56, 0x21),
    EDID_IN_BYTE215		= ACC_REG( 0x57, 0x21),
    EDID_IN_BYTE216		= ACC_REG( 0x58, 0x21),
    EDID_IN_BYTE217		= ACC_REG( 0x59, 0x21),
    EDID_IN_BYTE218		= ACC_REG( 0x5A, 0x21),
    EDID_IN_BYTE219		= ACC_REG( 0x5B, 0x21),
    EDID_IN_BYTE220		= ACC_REG( 0x5C, 0x21),
    EDID_IN_BYTE221		= ACC_REG( 0x5D, 0x21),
    EDID_IN_BYTE222		= ACC_REG( 0x5E, 0x21),
    EDID_IN_BYTE223		= ACC_REG( 0x5F, 0x21),
    EDID_IN_BYTE224		= ACC_REG( 0x60, 0x21),
    EDID_IN_BYTE225		= ACC_REG( 0x61, 0x21),
    EDID_IN_BYTE226		= ACC_REG( 0x62, 0x21),
    EDID_IN_BYTE227		= ACC_REG( 0x63, 0x21),
    EDID_IN_BYTE228		= ACC_REG( 0x64, 0x21),
    EDID_IN_BYTE229		= ACC_REG( 0x65, 0x21),
    EDID_IN_BYTE230		= ACC_REG( 0x66, 0x21),
    EDID_IN_BYTE231		= ACC_REG( 0x67, 0x21),
    EDID_IN_BYTE232		= ACC_REG( 0x68, 0x21),
    EDID_IN_BYTE233		= ACC_REG( 0x69, 0x21),
    EDID_IN_BYTE234		= ACC_REG( 0x6A, 0x21),
    EDID_IN_BYTE235		= ACC_REG( 0x6B, 0x21),
    EDID_IN_BYTE236		= ACC_REG( 0x6C, 0x21),
    EDID_IN_BYTE237		= ACC_REG( 0x6D, 0x21),
    EDID_IN_BYTE238		= ACC_REG( 0x6E, 0x21),
    EDID_IN_BYTE239		= ACC_REG( 0x6F, 0x21),
    EDID_IN_BYTE240		= ACC_REG( 0x70, 0x21),
    EDID_IN_BYTE241		= ACC_REG( 0x71, 0x21),
    EDID_IN_BYTE242		= ACC_REG( 0x72, 0x21),
    EDID_IN_BYTE243		= ACC_REG( 0x73, 0x21),
    EDID_IN_BYTE244		= ACC_REG( 0x74, 0x21),
    EDID_IN_BYTE245		= ACC_REG( 0x75, 0x21),
    EDID_IN_BYTE246		= ACC_REG( 0x76, 0x21),
    EDID_IN_BYTE247		= ACC_REG( 0x77, 0x21),
    EDID_IN_BYTE248		= ACC_REG( 0x78, 0x21),
    EDID_IN_BYTE249		= ACC_REG( 0x79, 0x21),
    EDID_IN_BYTE250		= ACC_REG( 0x7A, 0x21),
    EDID_IN_BYTE251		= ACC_REG( 0x7B, 0x21),
    EDID_IN_BYTE252		= ACC_REG( 0x7C, 0x21),
    EDID_IN_BYTE253		= ACC_REG( 0x7D, 0x21),
    EDID_IN_BYTE254		= ACC_REG( 0x7E, 0x21),
    EDID_IN_BYTE255		= ACC_REG( 0x7F, 0x21),
    EDID_IN_SPA_SUB		= ACC_REG( 0x80, 0x21),
    EDID_IN_SPA_AB_A	= ACC_REG( 0x81, 0x21),
    EDID_IN_SPA_CD_A	= ACC_REG( 0x82, 0x21),
    EDID_IN_CHECKSUM_A	= ACC_REG( 0x83, 0x21),
    EDID_IN_SPA_AB_B	= ACC_REG( 0x84, 0x21),
    EDID_IN_SPA_CD_B	= ACC_REG( 0x85, 0x21),
    EDID_IN_CHECKSUM_B	= ACC_REG( 0x86, 0x21),
    EDID_BLOCK1_TEST1	= ACC_REG( 0x8D, 0x21),
    EDID_BLOCK1_TEST2	= ACC_REG( 0x8E, 0x21),
    EDID_BLOCK1_TEST3	= ACC_REG( 0x8F, 0x21),
    CURPAGE_ADR_21H     = ACC_REG( 0xFF, 0x21)
} tmbslTDA1997X_Reg_Page21;

typedef enum _tmbslTDA1997X_Reg_Page22
{
    EDID_OUT_BYTE0		= ACC_REG( 0x00, 0x22),
    EDID_OUT_BYTE1     	= ACC_REG( 0x01, 0x22),
    EDID_OUT_BYTE2		= ACC_REG( 0x02, 0x22),
    EDID_OUT_BYTE3		= ACC_REG( 0x03, 0x22),
    EDID_OUT_BYTE4		= ACC_REG( 0x04, 0x22),
    EDID_OUT_BYTE5		= ACC_REG( 0x05, 0x22),
    EDID_OUT_BYTE6		= ACC_REG( 0x06, 0x22),
    EDID_OUT_BYTE7		= ACC_REG( 0x07, 0x22),
    EDID_OUT_BYTE8		= ACC_REG( 0x08, 0x22),
    EDID_OUT_BYTE9		= ACC_REG( 0x09, 0x22),
    EDID_OUT_BYTE10	    = ACC_REG( 0x0A, 0x22),
    EDID_OUT_BYTE11		= ACC_REG( 0x0B, 0x22),
    EDID_OUT_BYTE12	    = ACC_REG( 0x0C, 0x22),
    EDID_OUT_BYTE13	    = ACC_REG( 0x0D, 0x22),
    EDID_OUT_BYTE14		= ACC_REG( 0x0E, 0x22),
    EDID_OUT_BYTE15		= ACC_REG( 0x0F, 0x22),
    EDID_OUT_BYTE16		= ACC_REG( 0x10, 0x22),
    EDID_OUT_BYTE17		= ACC_REG( 0x11, 0x22),
    EDID_OUT_BYTE18		= ACC_REG( 0x12, 0x22),
    EDID_OUT_BYTE19		= ACC_REG( 0x13, 0x22),
    EDID_OUT_BYTE20		= ACC_REG( 0x14, 0x22),
    EDID_OUT_BYTE21		= ACC_REG( 0x15, 0x22),
    EDID_OUT_BYTE22		= ACC_REG( 0x16, 0x22),
    EDID_OUT_BYTE23		= ACC_REG( 0x17, 0x22),
    EDID_OUT_BYTE24		= ACC_REG( 0x18, 0x22),
    EDID_OUT_BYTE25		= ACC_REG( 0x19, 0x22),
    EDID_OUT_BYTE26		= ACC_REG( 0x1A, 0x22),
    EDID_OUT_BYTE27		= ACC_REG( 0x1B, 0x22),
    EDID_OUT_BYTE28		= ACC_REG( 0x1C, 0x22),
    EDID_OUT_BYTE29		= ACC_REG( 0x1D, 0x22),
    EDID_OUT_BYTE30		= ACC_REG( 0x1E, 0x22),
    EDID_OUT_BYTE31		= ACC_REG( 0x1F, 0x22),
    EDID_OUT_BYTE32		= ACC_REG( 0x20, 0x22),
    EDID_OUT_BYTE33		= ACC_REG( 0x21, 0x22),
    EDID_OUT_BYTE34		= ACC_REG( 0x22, 0x22),
    EDID_OUT_BYTE35		= ACC_REG( 0x23, 0x22),
    EDID_OUT_BYTE36		= ACC_REG( 0x24, 0x22),
    EDID_OUT_BYTE37		= ACC_REG( 0x25, 0x22),
    EDID_OUT_BYTE38		= ACC_REG( 0x26, 0x22),
    EDID_OUT_BYTE39		= ACC_REG( 0x27, 0x22),
    EDID_OUT_BYTE40		= ACC_REG( 0x28, 0x22),
    EDID_OUT_BYTE41		= ACC_REG( 0x29, 0x22),
    EDID_OUT_BYTE42		= ACC_REG( 0x2A, 0x22),
    EDID_OUT_BYTE43		= ACC_REG( 0x2B, 0x22),
    EDID_OUT_BYTE44		= ACC_REG( 0x2C, 0x22),
    EDID_OUT_BYTE45		= ACC_REG( 0x2D, 0x22),
    EDID_OUT_BYTE46 	= ACC_REG( 0x2E, 0x22),
    EDID_OUT_BYTE47 	= ACC_REG( 0x2F, 0x22),
    EDID_OUT_BYTE48 	= ACC_REG( 0x30, 0x22),
    EDID_OUT_BYTE49 	= ACC_REG( 0x31, 0x22),
    EDID_OUT_BYTE50 	= ACC_REG( 0x32, 0x22),
    EDID_OUT_BYTE51 	= ACC_REG( 0x33, 0x22),
    EDID_OUT_BYTE52 	= ACC_REG( 0x34, 0x22),
    EDID_OUT_BYTE53 	= ACC_REG( 0x35, 0x22),
    EDID_OUT_BYTE54		= ACC_REG( 0x36, 0x22),
    EDID_OUT_BYTE55		= ACC_REG( 0x37, 0x22),
    EDID_OUT_BYTE56		= ACC_REG( 0x38, 0x22),
    EDID_OUT_BYTE57		= ACC_REG( 0x39, 0x22),
    EDID_OUT_BYTE58		= ACC_REG( 0x3A, 0x22),
    EDID_OUT_BYTE59		= ACC_REG( 0x3B, 0x22),
    EDID_OUT_BYTE60		= ACC_REG( 0x3C, 0x22),
    EDID_OUT_BYTE61		= ACC_REG( 0x3D, 0x22),
    EDID_OUT_BYTE62		= ACC_REG( 0x3E, 0x22),
    EDID_OUT_BYTE63		= ACC_REG( 0x3F, 0x22),
    EDID_OUT_BYTE64		= ACC_REG( 0x40, 0x22),
    EDID_OUT_BYTE65		= ACC_REG( 0x41, 0x22),
    EDID_OUT_BYTE66		= ACC_REG( 0x42, 0x22),
    EDID_OUT_BYTE67		= ACC_REG( 0x43, 0x22),
    EDID_OUT_BYTE68		= ACC_REG( 0x44, 0x22),
    EDID_OUT_BYTE69		= ACC_REG( 0x45, 0x22),
    EDID_OUT_BYTE70		= ACC_REG( 0x46, 0x22),
    EDID_OUT_BYTE71		= ACC_REG( 0x47, 0x22),
    EDID_OUT_BYTE72		= ACC_REG( 0x48, 0x22),
    EDID_OUT_BYTE73		= ACC_REG( 0x49, 0x22),
    EDID_OUT_BYTE74		= ACC_REG( 0x4A, 0x22),
    EDID_OUT_BYTE75		= ACC_REG( 0x4B, 0x22),
    EDID_OUT_BYTE76		= ACC_REG( 0x4C, 0x22),
    EDID_OUT_BYTE77		= ACC_REG( 0x4D, 0x22),
    EDID_OUT_BYTE78		= ACC_REG( 0x4E, 0x22),
    EDID_OUT_BYTE79		= ACC_REG( 0x4F, 0x22),
    EDID_OUT_BYTE80		= ACC_REG( 0x50, 0x22),
    EDID_OUT_BYTE81		= ACC_REG( 0x51, 0x22),
    EDID_OUT_BYTE82		= ACC_REG( 0x52, 0x22),
    EDID_OUT_BYTE83		= ACC_REG( 0x53, 0x22),
    EDID_OUT_BYTE84		= ACC_REG( 0x54, 0x22),
    EDID_OUT_BYTE85		= ACC_REG( 0x55, 0x22),
    EDID_OUT_BYTE86		= ACC_REG( 0x56, 0x22),
    EDID_OUT_BYTE87		= ACC_REG( 0x57, 0x22),
    EDID_OUT_BYTE88		= ACC_REG( 0x58, 0x22),
    EDID_OUT_BYTE89		= ACC_REG( 0x59, 0x22),
    EDID_OUT_BYTE90		= ACC_REG( 0x5A, 0x22),
    EDID_OUT_BYTE91		= ACC_REG( 0x5B, 0x22),
    EDID_OUT_BYTE92		= ACC_REG( 0x5C, 0x22),
    EDID_OUT_BYTE93		= ACC_REG( 0x5D, 0x22),
    EDID_OUT_BYTE94		= ACC_REG( 0x5E, 0x22),
    EDID_OUT_BYTE95		= ACC_REG( 0x5F, 0x22),
    EDID_OUT_BYTE96		= ACC_REG( 0x60, 0x22),
    EDID_OUT_BYTE97		= ACC_REG( 0x61, 0x22),
    EDID_OUT_BYTE98		= ACC_REG( 0x62, 0x22),
    EDID_OUT_BYTE99		= ACC_REG( 0x63, 0x22),
    EDID_OUT_BYTE100	= ACC_REG( 0x64, 0x22),
    EDID_OUT_BYTE101	= ACC_REG( 0x65, 0x22),
    EDID_OUT_BYTE102	= ACC_REG( 0x66, 0x22),
    EDID_OUT_BYTE103	= ACC_REG( 0x67, 0x22),
    EDID_OUT_BYTE104	= ACC_REG( 0x68, 0x22),
    EDID_OUT_BYTE105	= ACC_REG( 0x69, 0x22),
    EDID_OUT_BYTE106	= ACC_REG( 0x6A, 0x22),
    EDID_OUT_BYTE107	= ACC_REG( 0x6B, 0x22),
    EDID_OUT_BYTE108	= ACC_REG( 0x6C, 0x22),
    EDID_OUT_BYTE109	= ACC_REG( 0x6D, 0x22),
    EDID_OUT_BYTE110	= ACC_REG( 0x6E, 0x22),
    EDID_OUT_BYTE111	= ACC_REG( 0x6F, 0x22),
    EDID_OUT_BYTE112	= ACC_REG( 0x70, 0x22),
    EDID_OUT_BYTE113	= ACC_REG( 0x71, 0x22),
    EDID_OUT_BYTE114	= ACC_REG( 0x72, 0x22),
    EDID_OUT_BYTE115	= ACC_REG( 0x73, 0x22),
    EDID_OUT_BYTE116	= ACC_REG( 0x74, 0x22),
    EDID_OUT_BYTE117	= ACC_REG( 0x75, 0x22),
    EDID_OUT_BYTE118	= ACC_REG( 0x76, 0x22),
    EDID_OUT_BYTE119	= ACC_REG( 0x77, 0x22),
    EDID_OUT_BYTE120	= ACC_REG( 0x78, 0x22),
    EDID_OUT_BYTE121	= ACC_REG( 0x79, 0x22),
    EDID_OUT_BYTE122	= ACC_REG( 0x7A, 0x22),
    EDID_OUT_BYTE123	= ACC_REG( 0x7B, 0x22),
    EDID_OUT_BYTE124	= ACC_REG( 0x7C, 0x22),
    EDID_OUT_BYTE125	= ACC_REG( 0x7D, 0x22),
    EDID_OUT_BYTE126	= ACC_REG( 0x7E, 0x22),
    EDID_OUT_BYTE127	= ACC_REG( 0x7F, 0x22),
    EDID_OUT_SEGM_ADDR	= ACC_REG( 0x80, 0x22),
    EDID_OUT_SEGM		= ACC_REG( 0x81, 0x22),
    EDID_OUT_ADDR		= ACC_REG( 0x82, 0x22),
    EDID_OUT_OFFSET		= ACC_REG( 0x83, 0x22),
    EDID_OUT_PAGE		= ACC_REG( 0x84, 0x22),
    EDID_OUT_CTRL		= ACC_REG( 0x85, 0x22),
    EDID_OUT_STATUS		= ACC_REG( 0x86, 0x22),
    EDID_OUT_WR_DATA	= ACC_REG( 0x90, 0x22),
    EDID_OUT_RD_DATA	= ACC_REG( 0x91, 0x22),
    CURPAGE_ADR_22H     = ACC_REG( 0xFF, 0x22)
} tmbslTDA1997X_Reg_Page22;

typedef enum _tmbslTDA1997X_Reg_Page23
{
    EDID_OUT_BYTE128	= ACC_REG( 0x00, 0x23),
    EDID_OUT_BYTE129    = ACC_REG( 0x01, 0x23),
    EDID_OUT_BYTE130	= ACC_REG( 0x02, 0x23),
    EDID_OUT_BYTE131	= ACC_REG( 0x03, 0x23),
    EDID_OUT_BYTE132	= ACC_REG( 0x04, 0x23),
    EDID_OUT_BYTE133	= ACC_REG( 0x05, 0x23),
    EDID_OUT_BYTE134	= ACC_REG( 0x06, 0x23),
    EDID_OUT_BYTE135	= ACC_REG( 0x07, 0x23),
    EDID_OUT_BYTE136	= ACC_REG( 0x08, 0x23),
    EDID_OUT_BYTE137	= ACC_REG( 0x09, 0x23),
    EDID_OUT_BYTE138	= ACC_REG( 0x0A, 0x23),
    EDID_OUT_BYTE139	= ACC_REG( 0x0B, 0x23),
    EDID_OUT_BYTE140	= ACC_REG( 0x0C, 0x23),
    EDID_OUT_BYTE141	= ACC_REG( 0x0D, 0x23),
    EDID_OUT_BYTE142	= ACC_REG( 0x0E, 0x23),
    EDID_OUT_BYTE143	= ACC_REG( 0x0F, 0x23),
    EDID_OUT_BYTE144	= ACC_REG( 0x10, 0x23),
    EDID_OUT_BYTE145	= ACC_REG( 0x11, 0x23),
    EDID_OUT_BYTE146	= ACC_REG( 0x12, 0x23),
    EDID_OUT_BYTE147	= ACC_REG( 0x13, 0x23),
    EDID_OUT_BYTE148	= ACC_REG( 0x14, 0x23),
    EDID_OUT_BYTE149	= ACC_REG( 0x15, 0x23),
    EDID_OUT_BYTE150	= ACC_REG( 0x16, 0x23),
    EDID_OUT_BYTE151	= ACC_REG( 0x17, 0x23),
    EDID_OUT_BYTE152	= ACC_REG( 0x18, 0x23),
    EDID_OUT_BYTE153	= ACC_REG( 0x19, 0x23),
    EDID_OUT_BYTE154	= ACC_REG( 0x1A, 0x23),
    EDID_OUT_BYTE155	= ACC_REG( 0x1B, 0x23),
    EDID_OUT_BYTE156	= ACC_REG( 0x1C, 0x23),
    EDID_OUT_BYTE157	= ACC_REG( 0x1D, 0x23),
    EDID_OUT_BYTE158	= ACC_REG( 0x1E, 0x23),
    EDID_OUT_BYTE159	= ACC_REG( 0x1F, 0x23),
    EDID_OUT_BYTE160	= ACC_REG( 0x20, 0x23),
    EDID_OUT_BYTE161	= ACC_REG( 0x21, 0x23),
    EDID_OUT_BYTE162	= ACC_REG( 0x22, 0x23),
    EDID_OUT_BYTE163	= ACC_REG( 0x23, 0x23),
    EDID_OUT_BYTE164	= ACC_REG( 0x24, 0x23),
    EDID_OUT_BYTE165	= ACC_REG( 0x25, 0x23),
    EDID_OUT_BYTE166	= ACC_REG( 0x26, 0x23),
    EDID_OUT_BYTE167	= ACC_REG( 0x27, 0x23),
    EDID_OUT_BYTE168	= ACC_REG( 0x28, 0x23),
    EDID_OUT_BYTE169	= ACC_REG( 0x29, 0x23),
    EDID_OUT_BYTE170	= ACC_REG( 0x2A, 0x23),
    EDID_OUT_BYTE171	= ACC_REG( 0x2B, 0x23),
    EDID_OUT_BYTE172	= ACC_REG( 0x2C, 0x23),
    EDID_OUT_BYTE173	= ACC_REG( 0x2D, 0x23),
    EDID_OUT_BYTE174 	= ACC_REG( 0x2E, 0x23),
    EDID_OUT_BYTE175 	= ACC_REG( 0x2F, 0x23),
    EDID_OUT_BYTE176 	= ACC_REG( 0x30, 0x23),
    EDID_OUT_BYTE177 	= ACC_REG( 0x31, 0x23),
    EDID_OUT_BYTE178 	= ACC_REG( 0x32, 0x23),
    EDID_OUT_BYTE179 	= ACC_REG( 0x33, 0x23),
    EDID_OUT_BYTE180 	= ACC_REG( 0x34, 0x23),
    EDID_OUT_BYTE181 	= ACC_REG( 0x35, 0x23),
    EDID_OUT_BYTE182	= ACC_REG( 0x36, 0x23),
    EDID_OUT_BYTE183	= ACC_REG( 0x37, 0x23),
    EDID_OUT_BYTE184	= ACC_REG( 0x38, 0x23),
    EDID_OUT_BYTE185	= ACC_REG( 0x39, 0x23),
    EDID_OUT_BYTE186	= ACC_REG( 0x3A, 0x23),
    EDID_OUT_BYTE187	= ACC_REG( 0x3B, 0x23),
    EDID_OUT_BYTE188	= ACC_REG( 0x3C, 0x23),
    EDID_OUT_BYTE189	= ACC_REG( 0x3D, 0x23),
    EDID_OUT_BYTE190	= ACC_REG( 0x3E, 0x23),
    EDID_OUT_BYTE191	= ACC_REG( 0x3F, 0x23),
    EDID_OUT_BYTE192	= ACC_REG( 0x40, 0x23),
    EDID_OUT_BYTE193	= ACC_REG( 0x41, 0x23),
    EDID_OUT_BYTE194	= ACC_REG( 0x42, 0x23),
    EDID_OUT_BYTE195	= ACC_REG( 0x43, 0x23),
    EDID_OUT_BYTE196	= ACC_REG( 0x44, 0x23),
    EDID_OUT_BYTE197	= ACC_REG( 0x45, 0x23),
    EDID_OUT_BYTE198	= ACC_REG( 0x46, 0x23),
    EDID_OUT_BYTE199	= ACC_REG( 0x47, 0x23),
    EDID_OUT_BYTE200	= ACC_REG( 0x48, 0x23),
    EDID_OUT_BYTE201	= ACC_REG( 0x49, 0x23),
    EDID_OUT_BYTE202	= ACC_REG( 0x4A, 0x23),
    EDID_OUT_BYTE203	= ACC_REG( 0x4B, 0x23),
    EDID_OUT_BYTE204	= ACC_REG( 0x4C, 0x23),
    EDID_OUT_BYTE205	= ACC_REG( 0x4D, 0x23),
    EDID_OUT_BYTE206	= ACC_REG( 0x4E, 0x23),
    EDID_OUT_BYTE207	= ACC_REG( 0x4F, 0x23),
    EDID_OUT_BYTE208	= ACC_REG( 0x50, 0x23),
    EDID_OUT_BYTE209	= ACC_REG( 0x51, 0x23),
    EDID_OUT_BYTE210	= ACC_REG( 0x52, 0x23),
    EDID_OUT_BYTE211	= ACC_REG( 0x53, 0x23),
    EDID_OUT_BYTE212	= ACC_REG( 0x54, 0x23),
    EDID_OUT_BYTE213	= ACC_REG( 0x55, 0x23),
    EDID_OUT_BYTE214	= ACC_REG( 0x56, 0x23),
    EDID_OUT_BYTE215	= ACC_REG( 0x57, 0x23),
    EDID_OUT_BYTE216	= ACC_REG( 0x58, 0x23),
    EDID_OUT_BYTE217	= ACC_REG( 0x59, 0x23),
    EDID_OUT_BYTE218	= ACC_REG( 0x5A, 0x23),
    EDID_OUT_BYTE219	= ACC_REG( 0x5B, 0x23),
    EDID_OUT_BYTE220	= ACC_REG( 0x5C, 0x23),
    EDID_OUT_BYTE221	= ACC_REG( 0x5D, 0x23),
    EDID_OUT_BYTE222	= ACC_REG( 0x5E, 0x23),
    EDID_OUT_BYTE223	= ACC_REG( 0x5F, 0x23),
    EDID_OUT_BYTE224	= ACC_REG( 0x60, 0x23),
    EDID_OUT_BYTE225	= ACC_REG( 0x61, 0x23),
    EDID_OUT_BYTE226	= ACC_REG( 0x62, 0x23),
    EDID_OUT_BYTE227	= ACC_REG( 0x63, 0x23),
    EDID_OUT_BYTE228	= ACC_REG( 0x64, 0x23),
    EDID_OUT_BYTE229	= ACC_REG( 0x65, 0x23),
    EDID_OUT_BYTE230	= ACC_REG( 0x66, 0x23),
    EDID_OUT_BYTE231	= ACC_REG( 0x67, 0x23),
    EDID_OUT_BYTE232	= ACC_REG( 0x68, 0x23),
    EDID_OUT_BYTE233	= ACC_REG( 0x69, 0x23),
    EDID_OUT_BYTE234	= ACC_REG( 0x6A, 0x23),
    EDID_OUT_BYTE235	= ACC_REG( 0x6B, 0x23),
    EDID_OUT_BYTE236	= ACC_REG( 0x6C, 0x23),
    EDID_OUT_BYTE237	= ACC_REG( 0x6D, 0x23),
    EDID_OUT_BYTE238	= ACC_REG( 0x6E, 0x23),
    EDID_OUT_BYTE239	= ACC_REG( 0x6F, 0x23),
    EDID_OUT_BYTE240	= ACC_REG( 0x70, 0x23),
    EDID_OUT_BYTE241	= ACC_REG( 0x71, 0x23),
    EDID_OUT_BYTE242	= ACC_REG( 0x72, 0x23),
    EDID_OUT_BYTE243	= ACC_REG( 0x73, 0x23),
    EDID_OUT_BYTE244	= ACC_REG( 0x74, 0x23),
    EDID_OUT_BYTE245	= ACC_REG( 0x75, 0x23),
    EDID_OUT_BYTE246	= ACC_REG( 0x76, 0x23),
    EDID_OUT_BYTE247	= ACC_REG( 0x77, 0x23),
    EDID_OUT_BYTE248	= ACC_REG( 0x78, 0x23),
    EDID_OUT_BYTE249	= ACC_REG( 0x79, 0x23),
    EDID_OUT_BYTE250	= ACC_REG( 0x7A, 0x23),
    EDID_OUT_BYTE251	= ACC_REG( 0x7B, 0x23),
    EDID_OUT_BYTE252	= ACC_REG( 0x7C, 0x23),
    EDID_OUT_BYTE253	= ACC_REG( 0x7D, 0x23),
    EDID_OUT_BYTE254	= ACC_REG( 0x7E, 0x23),
    EDID_OUT_BYTE255	= ACC_REG( 0x7F, 0x23),
    EDID_OUT_SPA_SUB	= ACC_REG( 0x80, 0x23),
    EDID_OUT_SPA_AB_A	= ACC_REG( 0x81, 0x23),
    EDID_OUT_SPA_CD_A	= ACC_REG( 0x82, 0x23),
    EDID_OUT_CHECKSUM_A	= ACC_REG( 0x83, 0x23),
    EDID_OUT_SPA_AB_B	= ACC_REG( 0x84, 0x23),
    EDID_OUT_SPA_CD_B	= ACC_REG( 0x85, 0x23),
    EDID_OUT_CHECKSUM_B	= ACC_REG( 0x86, 0x23),
    CURPAGE_ADR_23H     = ACC_REG( 0xFF, 0x23)
} tmbslTDA1997X_Reg_Page23;

typedef enum _tmbslTDA1997X_Reg_Page30
{
    RT_AUTO_CTRL        	= ACC_REG( 0x00, 0x30),
    EQ_MAN_CTRL0        	= ACC_REG( 0x01, 0x30),
    EQ_MAN_CTRL1        	= ACC_REG( 0x02, 0x30),
    OUTPUT_CFG          	= ACC_REG( 0x03, 0x30),
    MUTE_CTRL          		= ACC_REG( 0x04, 0x30),
    SLAVE_ADDR          	= ACC_REG( 0x05, 0x30),
    CMTP_REG6         		= ACC_REG( 0x06, 0x30),
    CMTP_REG7         		= ACC_REG( 0x07, 0x30),
    CMTP_REG8         		= ACC_REG( 0x08, 0x30),
    CMTP_REG9         		= ACC_REG( 0x09, 0x30),
    CMTP_REGA         		= ACC_REG( 0x0A, 0x30),
    CMTP_REGB         		= ACC_REG( 0x0B, 0x30),
    CMTP_REGC         		= ACC_REG( 0x0C, 0x30),
    CMTP_REGD         		= ACC_REG( 0x0D, 0x30),
    CMTP_REGE         		= ACC_REG( 0x0E, 0x30),
    CMTP_REGF        		= ACC_REG( 0x0F, 0x30),
    CMTP_REG10        		= ACC_REG( 0x10, 0x30),
    CMTP_REG11        		= ACC_REG( 0x11, 0x30),
    CMTP_REG12        		= ACC_REG( 0x12, 0x30),
    CMTP_REG13        		= ACC_REG( 0x13, 0x30),
    CMTP_REG14        		= ACC_REG( 0x14, 0x30),
    CMTP_REG15        		= ACC_REG( 0x15, 0x30),
    CMTP_REG16        		= ACC_REG( 0x16, 0x30),
    CMTP_REG17        		= ACC_REG( 0x17, 0x30),
    CMTP_REG18        		= ACC_REG( 0x18, 0x30),
    CMTP_REG19        		= ACC_REG( 0x19, 0x30),
    CMTP_REG1A        		= ACC_REG( 0x1A, 0x30),
    CMTP_REG1B         		= ACC_REG( 0x1B, 0x30),
    CMTP_REG1C         		= ACC_REG( 0x1C, 0x30),
    CMTP_REG1D         		= ACC_REG( 0x1D, 0x30),
    CMTP_REG1E         		= ACC_REG( 0x1E, 0x30),
    CMTP_REG1F         		= ACC_REG( 0x1F, 0x30),
    CMTP_REG20         		= ACC_REG( 0x20, 0x30),
    CMTP_REG21         		= ACC_REG( 0x21, 0x30),
    CMTP_REG22         		= ACC_REG( 0x22, 0x30),
    CMTP_REG23         		= ACC_REG( 0x23, 0x30),
    CMTP_REG24        		= ACC_REG( 0x24, 0x30),
    CMTP_REG25        		= ACC_REG( 0x25, 0x30),
    CMTP_REG26        		= ACC_REG( 0x26, 0x30),
    CMTP_REG27        		= ACC_REG( 0x27, 0x30),
    CMTP_REG28        		= ACC_REG( 0x28, 0x30),
    CMTP_REG29        		= ACC_REG( 0x29, 0x30),
    CMTP_REG2A        		= ACC_REG( 0x2A, 0x30),
    CMTP_REG2B        		= ACC_REG( 0x2B, 0x30),
    CMTP_REG2C        		= ACC_REG( 0x2C, 0x30),
    CMTP_REG2D        		= ACC_REG( 0x2D, 0x30),
    CMTP_REG2E        		= ACC_REG( 0x2E, 0x30),
    CMTP_REG2F        		= ACC_REG( 0x2F, 0x30),
    CMTP_RESET        		= ACC_REG( 0x33, 0x30),
    CMTP_ADDRESS_SETUP  	= ACC_REG( 0x34, 0x30),
    CMTP_ADDRESS_HOLD   	= ACC_REG( 0x35, 0x30),
    CMTP_READ_ENABLE    	= ACC_REG( 0x36, 0x30),
    CMTP_WRITE_ENABLE   	= ACC_REG( 0x37, 0x30),
    CMTP_INITIALIZATION 	= ACC_REG( 0x38, 0x30),
    CMTP_ERASE_PGM_PROGRESS = ACC_REG( 0x39, 0x30),
    CMTP_DISCHARGE        	= ACC_REG( 0x3A, 0x30),
    CMTP_VANALOG        	= ACC_REG( 0x3B, 0x30),
    CMTP_TEST        		= ACC_REG( 0x3C, 0x30),
    CMTP_MEM_REGS      		= ACC_REG( 0x40, 0x30),
    CMTP_DMA_CONF      		= ACC_REG( 0x41, 0x30),
    CMTP_ECC_REGISTERS 		= ACC_REG( 0x42, 0x30),
    CMTP_ERR_ADDR      		= ACC_REG( 0x43, 0x30),
    CMTP_PARAM        		= ACC_REG( 0x44, 0x30),
    CURPAGE_ADR_30H     	= ACC_REG( 0xFF, 0x30)
} tmbslTDA1997X_Reg_Page30;

typedef enum _tmbslTDA1997X_Reg_Page40
{
    MTP_SEED_INDEX      = ACC_REG( 0x00, 0x40),
    MTP_KEY_SPARE      	= ACC_REG( 0x01, 0x40),
    MTP_KEY39_LSB		= ACC_REG( 0x02, 0x40),
    MTP_BKSV4        	= ACC_REG( 0x03, 0x40),
    MTP_BKSV3        	= ACC_REG( 0x04, 0x40),
    MTP_BKSV2          	= ACC_REG( 0x05, 0x40),
    MTP_BKSV1          	= ACC_REG( 0x06, 0x40),
    MTP_BKSV0          	= ACC_REG( 0x07, 0x40),
    MTP_KEY0_MSB		= ACC_REG( 0x08, 0x40),
    MTP_KEY0_ISB1       = ACC_REG( 0x09, 0x40),
    MTP_KEY0_ISB2       = ACC_REG( 0x0A, 0x40),
    MTP_KEY0_ISB3       = ACC_REG( 0x0B, 0x40),
    MTP_KEY0_ISB4       = ACC_REG( 0x0C, 0x40),
    MTP_KEY0_ISB5       = ACC_REG( 0x0D, 0x40),
    MTP_KEY0_LSB        = ACC_REG( 0x0E, 0x40),
    MTP_KEY1_MSB        = ACC_REG( 0x0F, 0x40),
    MTP_KEY1_ISB1       = ACC_REG( 0x10, 0x40),
    MTP_KEY1_ISB2       = ACC_REG( 0x11, 0x40),
    MTP_KEY1_ISB3       = ACC_REG( 0x12, 0x40),
    MTP_KEY1_ISB4       = ACC_REG( 0x13, 0x40),
    MTP_KEY1_ISB5       = ACC_REG( 0x14, 0x40),
    MTP_KEY1_LSB        = ACC_REG( 0x15, 0x40),
    MTP_KEY2_MSB        = ACC_REG( 0x16, 0x40),
    MTP_KEY2_ISB1       = ACC_REG( 0x17, 0x40),
    MTP_KEY2_ISB2       = ACC_REG( 0x18, 0x40),
    MTP_KEY2_ISB3       = ACC_REG( 0x19, 0x40),
    MTP_KEY2_ISB4       = ACC_REG( 0x1A, 0x40),
    MTP_KEY2_ISB5       = ACC_REG( 0x1B, 0x40),
    MTP_KEY2_LSB        = ACC_REG( 0x1C, 0x40),
    MTP_KEY3_MSB        = ACC_REG( 0x1D, 0x40),
    MTP_KEY3_ISB1       = ACC_REG( 0x1E, 0x40),
    MTP_KEY3_ISB2       = ACC_REG( 0x1F, 0x40),
    MTP_KEY3_ISB3       = ACC_REG( 0x20, 0x40),
    MTP_KEY3_ISB4       = ACC_REG( 0x21, 0x40),
    MTP_KEY3_ISB5       = ACC_REG( 0x22, 0x40),
    MTP_KEY3_LSB        = ACC_REG( 0x23, 0x40),
    MTP_KEY4_MSB        = ACC_REG( 0x24, 0x40),
    MTP_KEY4_ISB1       = ACC_REG( 0x25, 0x40),
    MTP_KEY4_ISB2       = ACC_REG( 0x26, 0x40),
    MTP_KEY4_ISB3       = ACC_REG( 0x27, 0x40),
    MTP_KEY4_ISB4       = ACC_REG( 0x28, 0x40),
    MTP_KEY4_ISB5       = ACC_REG( 0x29, 0x40),
    MTP_KEY4_LSB        = ACC_REG( 0x2A, 0x40),
    MTP_KEY5_MSB        = ACC_REG( 0x2B, 0x40),
    MTP_KEY5_ISB1       = ACC_REG( 0x2C, 0x40),
    MTP_KEY5_ISB2       = ACC_REG( 0x2D, 0x40),
    MTP_KEY5_ISB3       = ACC_REG( 0x2E, 0x40),
    MTP_KEY5_ISB4       = ACC_REG( 0x2F, 0x40),
    MTP_KEY5_ISB5       = ACC_REG( 0x30, 0x40),
    MTP_KEY5_LSB        = ACC_REG( 0x31, 0x40),
    MTP_KEY6_MSB        = ACC_REG( 0x32, 0x40),
    MTP_KEY6_ISB1       = ACC_REG( 0x33, 0x40),
    MTP_KEY6_ISB2       = ACC_REG( 0x34, 0x40),
    MTP_KEY6_ISB3       = ACC_REG( 0x35, 0x40),
    MTP_KEY6_ISB4       = ACC_REG( 0x36, 0x40),
    MTP_KEY6_ISB5       = ACC_REG( 0x37, 0x40),
    MTP_KEY6_LSB        = ACC_REG( 0x38, 0x40),
    MTP_KEY7_MSB        = ACC_REG( 0x39, 0x40),
    MTP_KEY7_ISB1       = ACC_REG( 0x3A, 0x40),
    MTP_KEY7_ISB2       = ACC_REG( 0x3B, 0x40),
    MTP_KEY7_ISB3       = ACC_REG( 0x3C, 0x40),
    MTP_KEY7_ISB4       = ACC_REG( 0x3D, 0x40),
    MTP_KEY7_ISB5       = ACC_REG( 0x3E, 0x40),
    MTP_KEY7_LSB        = ACC_REG( 0x3F, 0x40),
    MTP_KEY8_MSB        = ACC_REG( 0x40, 0x40),
    MTP_KEY8_ISB1       = ACC_REG( 0x41, 0x40),
    MTP_KEY8_ISB2       = ACC_REG( 0x42, 0x40),
    MTP_KEY8_ISB3       = ACC_REG( 0x43, 0x40),
    MTP_KEY8_ISB4       = ACC_REG( 0x44, 0x40),
    MTP_KEY8_ISB5       = ACC_REG( 0x45, 0x40),
    MTP_KEY8_LSB        = ACC_REG( 0x46, 0x40),
    MTP_KEY9_MSB        = ACC_REG( 0x47, 0x40),
    MTP_KEY9_ISB1       = ACC_REG( 0x48, 0x40),
    MTP_KEY9_ISB2       = ACC_REG( 0x49, 0x40),
    MTP_KEY9_ISB3       = ACC_REG( 0x4A, 0x40),
    MTP_KEY9_ISB4       = ACC_REG( 0x4B, 0x40),
    MTP_KEY9_ISB5       = ACC_REG( 0x4C, 0x40),
    MTP_KEY9_LSB        = ACC_REG( 0x4D, 0x40),
    MTP_KEY10_MSB       = ACC_REG( 0x4E, 0x40),
    MTP_KEY10_ISB1      = ACC_REG( 0x4F, 0x40),
    MTP_KEY10_ISB2      = ACC_REG( 0x50, 0x40),
    MTP_KEY10_ISB3      = ACC_REG( 0x51, 0x40),
    MTP_KEY10_ISB4      = ACC_REG( 0x52, 0x40),
    MTP_KEY10_ISB5      = ACC_REG( 0x53, 0x40),
    MTP_KEY10_LSB       = ACC_REG( 0x54, 0x40),
    MTP_KEY11_MSB       = ACC_REG( 0x55, 0x40),
    MTP_KEY11_ISB1      = ACC_REG( 0x56, 0x40),
    MTP_KEY11_ISB2      = ACC_REG( 0x57, 0x40),
    MTP_KEY11_ISB3      = ACC_REG( 0x58, 0x40),
    MTP_KEY11_ISB4      = ACC_REG( 0x59, 0x40),
    MTP_KEY11_ISB5      = ACC_REG( 0x5A, 0x40),
    MTP_KEY11_LSB       = ACC_REG( 0x5B, 0x40),
    MTP_KEY12_MSB       = ACC_REG( 0x5C, 0x40),
    MTP_KEY12_ISB1      = ACC_REG( 0x5D, 0x40),
    MTP_KEY12_ISB2      = ACC_REG( 0x5E, 0x40),
    MTP_KEY12_ISB3      = ACC_REG( 0x5F, 0x40),
    CURPAGE_ADR_40H     = ACC_REG( 0xFF, 0x40)
} tmbslTDA1997X_Reg_Page40;

typedef enum _tmbslTDA1997X_Reg_Page41
{
    MTP_KEY12_ISB4      = ACC_REG( 0x00, 0x41),
    MTP_KEY12_ISB5      = ACC_REG( 0x01, 0x41),
    MTP_KEY12_LSB      	= ACC_REG( 0x02, 0x41),
    MTP_KEY13_MSB     	= ACC_REG( 0x03, 0x41),
    MTP_KEY13_ISB1      = ACC_REG( 0x04, 0x41),
    MTP_KEY13_ISB2      = ACC_REG( 0x05, 0x41),
    MTP_KEY13_ISB3      = ACC_REG( 0x06, 0x41),
    MTP_KEY13_ISB4      = ACC_REG( 0x07, 0x41),
    MTP_KEY13_ISB5      = ACC_REG( 0x08, 0x41),
    MTP_KEY13_LSB       = ACC_REG( 0x09, 0x41),
    MTP_KEY14_MSB       = ACC_REG( 0x0A, 0x41),
    MTP_KEY14_ISB1      = ACC_REG( 0x0B, 0x41),
    MTP_KEY14_ISB2      = ACC_REG( 0x0C, 0x41),
    MTP_KEY14_ISB3      = ACC_REG( 0x0D, 0x41),
    MTP_KEY14_ISB4      = ACC_REG( 0x0E, 0x41),
    MTP_KEY14_ISB5      = ACC_REG( 0x0F, 0x41),
    MTP_KEY14_LSB       = ACC_REG( 0x10, 0x41),
    MTP_KEY15_MSB       = ACC_REG( 0x11, 0x41),
    MTP_KEY15_ISB1      = ACC_REG( 0x12, 0x41),
    MTP_KEY15_ISB2      = ACC_REG( 0x13, 0x41),
    MTP_KEY15_ISB3      = ACC_REG( 0x14, 0x41),
    MTP_KEY15_ISB4      = ACC_REG( 0x15, 0x41),
    MTP_KEY15_ISB5      = ACC_REG( 0x16, 0x41),
    MTP_KEY15_LSB       = ACC_REG( 0x17, 0x41),
    MTP_KEY16_MSB       = ACC_REG( 0x18, 0x41),
    MTP_KEY16_ISB1      = ACC_REG( 0x19, 0x41),
    MTP_KEY16_ISB2      = ACC_REG( 0x1A, 0x41),
    MTP_KEY16_ISB3      = ACC_REG( 0x1B, 0x41),
    MTP_KEY16_ISB4      = ACC_REG( 0x1C, 0x41),
    MTP_KEY16_ISB5      = ACC_REG( 0x1D, 0x41),
    MTP_KEY16_LSB       = ACC_REG( 0x1E, 0x41),
    MTP_KEY17_MSB       = ACC_REG( 0x1F, 0x41),
    MTP_KEY17_ISB1      = ACC_REG( 0x20, 0x41),
    MTP_KEY17_ISB2      = ACC_REG( 0x21, 0x41),
    MTP_KEY17_ISB3      = ACC_REG( 0x22, 0x41),
    MTP_KEY17_ISB4      = ACC_REG( 0x23, 0x41),
    MTP_KEY17_ISB5      = ACC_REG( 0x24, 0x41),
    MTP_KEY17_LSB       = ACC_REG( 0x25, 0x41),
    MTP_KEY18_MSB       = ACC_REG( 0x26, 0x41),
    MTP_KEY18_ISB1      = ACC_REG( 0x27, 0x41),
    MTP_KEY18_ISB2      = ACC_REG( 0x28, 0x41),
    MTP_KEY18_ISB3      = ACC_REG( 0x29, 0x41),
    MTP_KEY18_ISB4      = ACC_REG( 0x2A, 0x41),
    MTP_KEY18_ISB5      = ACC_REG( 0x2B, 0x41),
    MTP_KEY18_LSB       = ACC_REG( 0x2C, 0x41),
    MTP_KEY19_MSB       = ACC_REG( 0x2D, 0x41),
    MTP_KEY19_ISB1      = ACC_REG( 0x2E, 0x41),
    MTP_KEY19_ISB2      = ACC_REG( 0x2F, 0x41),
    MTP_KEY19_ISB3      = ACC_REG( 0x30, 0x41),
    MTP_KEY19_ISB4      = ACC_REG( 0x31, 0x41),
    MTP_KEY19_ISB5      = ACC_REG( 0x32, 0x41),
    MTP_KEY19_LSB       = ACC_REG( 0x33, 0x41),
    MTP_KEY20_MSB       = ACC_REG( 0x34, 0x41),
    MTP_KEY20_ISB1      = ACC_REG( 0x35, 0x41),
    MTP_KEY20_ISB2      = ACC_REG( 0x36, 0x41),
    MTP_KEY20_ISB3      = ACC_REG( 0x37, 0x41),
    MTP_KEY20_ISB4      = ACC_REG( 0x38, 0x41),
    MTP_KEY20_ISB5      = ACC_REG( 0x39, 0x41),
    MTP_KEY20_LSB       = ACC_REG( 0x3A, 0x41),
    MTP_KEY21_MSB       = ACC_REG( 0x3B, 0x41),
    MTP_KEY21_ISB1      = ACC_REG( 0x3C, 0x41),
    MTP_KEY21_ISB2      = ACC_REG( 0x3D, 0x41),
    MTP_KEY21_ISB3      = ACC_REG( 0x3E, 0x41),
    MTP_KEY21_ISB4      = ACC_REG( 0x3F, 0x41),
    MTP_KEY21_ISB5      = ACC_REG( 0x40, 0x41),
    MTP_KEY21_LSB       = ACC_REG( 0x41, 0x41),
    MTP_KEY22_MSB       = ACC_REG( 0x42, 0x41),
    MTP_KEY22_ISB1      = ACC_REG( 0x43, 0x41),
    MTP_KEY22_ISB2      = ACC_REG( 0x44, 0x41),
    MTP_KEY22_ISB3      = ACC_REG( 0x45, 0x41),
    MTP_KEY22_ISB4      = ACC_REG( 0x46, 0x41),
    MTP_KEY22_ISB5      = ACC_REG( 0x47, 0x41),
    MTP_KEY22_LSB       = ACC_REG( 0x48, 0x41),
    MTP_KEY23_MSB       = ACC_REG( 0x49, 0x41),
    MTP_KEY23_ISB1      = ACC_REG( 0x4A, 0x41),
    MTP_KEY23_ISB2      = ACC_REG( 0x4B, 0x41),
    MTP_KEY23_ISB3      = ACC_REG( 0x4C, 0x41),
    MTP_KEY23_ISB4      = ACC_REG( 0x4D, 0x41),
    MTP_KEY23_ISB5      = ACC_REG( 0x4E, 0x41),
    MTP_KEY23_LSB       = ACC_REG( 0x4F, 0x41),
    MTP_KEY24_MSB       = ACC_REG( 0x50, 0x41),
    MTP_KEY24_ISB1      = ACC_REG( 0x51, 0x41),
    MTP_KEY24_ISB2      = ACC_REG( 0x52, 0x41),
    MTP_KEY24_ISB3      = ACC_REG( 0x53, 0x41),
    MTP_KEY24_ISB4      = ACC_REG( 0x54, 0x41),
    MTP_KEY24_ISB5      = ACC_REG( 0x55, 0x41),
    MTP_KEY24_LSB       = ACC_REG( 0x56, 0x41),
    MTP_KEY25_MSB       = ACC_REG( 0x57, 0x41),
    MTP_KEY25_ISB1      = ACC_REG( 0x58, 0x41),
    MTP_KEY25_ISB2      = ACC_REG( 0x59, 0x41),
    MTP_KEY25_ISB3      = ACC_REG( 0x5A, 0x41),
    MTP_KEY25_ISB4      = ACC_REG( 0x5B, 0x41),
    MTP_KEY25_ISB5      = ACC_REG( 0x5C, 0x41),
    MTP_KEY25_LSB       = ACC_REG( 0x5D, 0x41),
    MTP_KEY26_MSB       = ACC_REG( 0x5E, 0x41),
    MTP_KEY26_ISB1      = ACC_REG( 0x5F, 0x41),
    CURPAGE_ADR_41H     = ACC_REG( 0xFF, 0x41)
} tmbslTDA1997X_Reg_Page41;

typedef enum _tmbslTDA1997X_Reg_Page42
{
    MTP_KEY26_ISB2      = ACC_REG( 0x00, 0x42),
    MTP_KEY26_ISB3      = ACC_REG( 0x01, 0x42),
    MTP_KEY26_ISB4      = ACC_REG( 0x02, 0x42),
    MTP_KEY26_ISB5      = ACC_REG( 0x03, 0x42),
    MTP_KEY26_LSB       = ACC_REG( 0x04, 0x42),
    MTP_KEY27_MSB       = ACC_REG( 0x05, 0x42),
    MTP_KEY27_ISB1      = ACC_REG( 0x06, 0x42),
    MTP_KEY27_ISB2      = ACC_REG( 0x07, 0x42),
    MTP_KEY27_ISB3      = ACC_REG( 0x08, 0x42),
    MTP_KEY27_ISB4      = ACC_REG( 0x09, 0x42),
    MTP_KEY27_ISB5      = ACC_REG( 0x0A, 0x42),
    MTP_KEY27_LSB       = ACC_REG( 0x0B, 0x42),
    MTP_KEY28_MSB       = ACC_REG( 0x0C, 0x42),
    MTP_KEY28_ISB1      = ACC_REG( 0x0D, 0x42),
    MTP_KEY28_ISB2      = ACC_REG( 0x0E, 0x42),
    MTP_KEY28_ISB3      = ACC_REG( 0x0F, 0x42),
    MTP_KEY28_ISB4      = ACC_REG( 0x10, 0x42),
    MTP_KEY28_ISB5      = ACC_REG( 0x11, 0x42),
    MTP_KEY28_LSB       = ACC_REG( 0x12, 0x42),
    MTP_KEY29_MSB       = ACC_REG( 0x13, 0x42),
    MTP_KEY29_ISB1      = ACC_REG( 0x14, 0x42),
    MTP_KEY29_ISB2      = ACC_REG( 0x15, 0x42),
    MTP_KEY29_ISB3      = ACC_REG( 0x16, 0x42),
    MTP_KEY29_ISB4      = ACC_REG( 0x17, 0x42),
    MTP_KEY29_ISB5      = ACC_REG( 0x18, 0x42),
    MTP_KEY29_LSB       = ACC_REG( 0x19, 0x42),
    MTP_KEY30_MSB       = ACC_REG( 0x1A, 0x42),
    MTP_KEY30_ISB1      = ACC_REG( 0x1B, 0x42),
    MTP_KEY30_ISB2      = ACC_REG( 0x1C, 0x42),
    MTP_KEY30_ISB3      = ACC_REG( 0x1D, 0x42),
    MTP_KEY30_ISB4      = ACC_REG( 0x1E, 0x42),
    MTP_KEY30_ISB5      = ACC_REG( 0x1F, 0x42),
    MTP_KEY30_LSB       = ACC_REG( 0x20, 0x42),
    MTP_KEY31_MSB       = ACC_REG( 0x21, 0x42),
    MTP_KEY31_ISB1      = ACC_REG( 0x22, 0x42),
    MTP_KEY31_ISB2      = ACC_REG( 0x23, 0x42),
    MTP_KEY31_ISB3      = ACC_REG( 0x24, 0x42),
    MTP_KEY31_ISB4      = ACC_REG( 0x25, 0x42),
    MTP_KEY31_ISB5      = ACC_REG( 0x26, 0x42),
    MTP_KEY31_LSB       = ACC_REG( 0x27, 0x42),
    MTP_KEY32_MSB       = ACC_REG( 0x28, 0x42),
    MTP_KEY32_ISB1      = ACC_REG( 0x29, 0x42),
    MTP_KEY32_ISB2      = ACC_REG( 0x2A, 0x42),
    MTP_KEY32_ISB3      = ACC_REG( 0x2B, 0x42),
    MTP_KEY32_ISB4      = ACC_REG( 0x2C, 0x42),
    MTP_KEY32_ISB5      = ACC_REG( 0x2D, 0x42),
    MTP_KEY32_LSB       = ACC_REG( 0x2E, 0x42),
    MTP_KEY33_MSB       = ACC_REG( 0x2F, 0x42),
    MTP_KEY33_ISB1      = ACC_REG( 0x30, 0x42),
    MTP_KEY33_ISB2      = ACC_REG( 0x31, 0x42),
    MTP_KEY33_ISB3      = ACC_REG( 0x32, 0x42),
    MTP_KEY33_ISB4      = ACC_REG( 0x33, 0x42),
    MTP_KEY33_ISB5      = ACC_REG( 0x34, 0x42),
    MTP_KEY33_LSB       = ACC_REG( 0x35, 0x42),
    MTP_KEY34_MSB       = ACC_REG( 0x36, 0x42),
    MTP_KEY34_ISB1      = ACC_REG( 0x37, 0x42),
    MTP_KEY34_ISB2      = ACC_REG( 0x38, 0x42),
    MTP_KEY34_ISB3      = ACC_REG( 0x39, 0x42),
    MTP_KEY34_ISB4      = ACC_REG( 0x3A, 0x42),
    MTP_KEY34_ISB5      = ACC_REG( 0x3B, 0x42),
    MTP_KEY34_LSB       = ACC_REG( 0x3C, 0x42),
    MTP_KEY35_MSB       = ACC_REG( 0x3D, 0x42),
    MTP_KEY35_ISB1      = ACC_REG( 0x3E, 0x42),
    MTP_KEY35_ISB2      = ACC_REG( 0x3F, 0x42),
    MTP_KEY35_ISB3      = ACC_REG( 0x40, 0x42),
    MTP_KEY35_ISB4      = ACC_REG( 0x41, 0x42),
    MTP_KEY35_ISB5      = ACC_REG( 0x42, 0x42),
    MTP_KEY35_LSB       = ACC_REG( 0x43, 0x42),
    MTP_KEY36_MSB       = ACC_REG( 0x44, 0x42),
    MTP_KEY36_ISB1      = ACC_REG( 0x45, 0x42),
    MTP_KEY36_ISB2      = ACC_REG( 0x46, 0x42),
    MTP_KEY36_ISB3      = ACC_REG( 0x47, 0x42),
    MTP_KEY36_ISB4      = ACC_REG( 0x48, 0x42),
    MTP_KEY36_ISB5      = ACC_REG( 0x49, 0x42),
    MTP_KEY36_LSB       = ACC_REG( 0x4A, 0x42),
    MTP_KEY37_MSB       = ACC_REG( 0x4B, 0x42),
    MTP_KEY37_ISB1      = ACC_REG( 0x4C, 0x42),
    MTP_KEY37_ISB2      = ACC_REG( 0x4D, 0x42),
    MTP_KEY37_ISB3      = ACC_REG( 0x4E, 0x42),
    MTP_KEY37_ISB4      = ACC_REG( 0x4F, 0x42),
    MTP_KEY37_ISB5      = ACC_REG( 0x50, 0x42),
    MTP_KEY37_LSB       = ACC_REG( 0x51, 0x42),
    MTP_KEY38_MSB       = ACC_REG( 0x52, 0x42),
    MTP_KEY38_ISB1      = ACC_REG( 0x53, 0x42),
    MTP_KEY38_ISB2      = ACC_REG( 0x54, 0x42),
    MTP_KEY38_ISB3      = ACC_REG( 0x55, 0x42),
    MTP_KEY38_ISB4      = ACC_REG( 0x56, 0x42),
    MTP_KEY38_ISB5      = ACC_REG( 0x57, 0x42),
    MTP_KEY38_LSB       = ACC_REG( 0x58, 0x42),
    MTP_KEY39_MSB       = ACC_REG( 0x59, 0x42),
    MTP_KEY39_ISB1      = ACC_REG( 0x5A, 0x42),
    MTP_KEY39_ISB2      = ACC_REG( 0x5B, 0x42),
    MTP_KEY39_ISB3      = ACC_REG( 0x5C, 0x42),
    MTP_KEY39_ISB4      = ACC_REG( 0x5D, 0x42),
    MTP_KEY39_ISB5      = ACC_REG( 0x5E, 0x42),
    MTP_PRIVATE_AREA   	= ACC_REG( 0x5F, 0x42),
    CURPAGE_ADR_42H     = ACC_REG( 0xFF, 0x42)
} tmbslTDA1997X_Reg_Page42;

typedef enum _tmbslTDA1997X_Reg_CEC
{
    APRCSR				= ACC_REG( 0x00, 0x80),
    CER					= ACC_REG( 0x01, 0x80),
    CVR					= ACC_REG( 0x02, 0x80),
    CCR					= ACC_REG( 0x03, 0x80),
    ACKH				= ACC_REG( 0x04, 0x80),
    ACKL				= ACC_REG( 0x05, 0x80),
    CCONR				= ACC_REG( 0x06, 0x80),
    CDR0				= ACC_REG( 0x07, 0x80),
    CDR1				= ACC_REG( 0x08, 0x80),
    CDR2				= ACC_REG( 0x09, 0x80),
    CDR3				= ACC_REG( 0x0A, 0x80),
    CDR4				= ACC_REG( 0x0B, 0x80),
    CDR5				= ACC_REG( 0x0C, 0x80),
    CDR6				= ACC_REG( 0x0D, 0x80),
    CDR7				= ACC_REG( 0x0E, 0x80),
    CDR8				= ACC_REG( 0x0F, 0x80),
    CDR9				= ACC_REG( 0x10, 0x80),
    CDR10				= ACC_REG( 0x11, 0x80),
    CDR11				= ACC_REG( 0x12, 0x80),
    CDR12				= ACC_REG( 0x13, 0x80),
    CDR13				= ACC_REG( 0x14, 0x80),
    CDR14				= ACC_REG( 0x15, 0x80),
    CDR15				= ACC_REG( 0x16, 0x80),
    CDR16				= ACC_REG( 0x17, 0x80),
    CDR17				= ACC_REG( 0x18, 0x80),
    CDR18				= ACC_REG( 0x19, 0x80),
    I2C_HIGH_SPEED		= ACC_REG( 0xF0, 0x80),
    XTAL_CTRL			= ACC_REG( 0xF1, 0x80),
    RESET_CTRL			= ACC_REG( 0xF2, 0x80),
    CFG_CLK_CTRL		= ACC_REG( 0xF3, 0x80),
    PWR_CONTROL			= ACC_REG( 0xF4, 0x80),
    OSC_DIVIDER			= ACC_REG( 0xF5, 0x80),
    OSC_PERIOD_LSB		= ACC_REG( 0xF6, 0x80),
    OSC_PERIOD_MSB		= ACC_REG( 0xF7, 0x80),
    EN_OSC_PERIOD_LSB	= ACC_REG( 0xF8, 0x80),
    EN_OSC_PERIOD_MSB	= ACC_REG( 0xF9, 0x80),
    STATUS_RX_SENSE		= ACC_REG( 0xFA, 0x80),
    INTERRUPT			= ACC_REG( 0xFC, 0x80),
    ENABLE_INTERRUPT	= ACC_REG( 0xFD, 0x80),
    EN_RX_SENSE			= ACC_REG( 0xFE, 0x80),
    CONTROL				= ACC_REG( 0xFF, 0x80)
} tmbslTDA1997X_Reg_CEC;


/*============================================================================*/
/**
    \brief        Writing on I2C bus

    \param        unit          Unit number
    \param        firstRegister First register to write
    \param        lengthData    Length
    \param        * pBuffer     Pointer of data to be written

    \return     - TM_OK  : the call was successful
                - TMBSL_ERR_BSLHDMIRX_BAD_UNIT_NUMBER: the unit number is wrong or
                  the receiver instance is already initialised
                - TMBSL_ERR_BSLHDMIRX_I2C_WRITE: failed when writing the I2C bus
                - TMBSL_ERR_BSLHDMIRX_INCONSISTENT_PARAMS: an input parameter is
                  inconsistent

 ******************************************************************************/
tmErrorCode_t
tmbslTDA1997XWriteI2C
(
    tmUnitSelect_t          unit,
    UInt16                  firstRegister,
    UInt8                   lengthData,
    UInt8                   *pBuffer
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
tmErrorCode_t
tmbslTDA1997XReadI2C
(
    tmUnitSelect_t          unit,
    UInt16                  firstRegister,
    UInt8                   lengthData,
    UInt8                   *pBuffer
);


#ifdef __cplusplus
}
#endif

#endif /* TMBSLTDA1997X_LOCAL_H */

/*============================================================================*/
/*                            END OF FILE                                     */
/*============================================================================*/


