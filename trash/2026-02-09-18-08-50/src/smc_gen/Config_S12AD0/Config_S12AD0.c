/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : Config_S12AD0.c
* Component Version: 2.5.0
* Device(s)        : R5F52315AxFL
* Description      : This file implements device driver for Config_S12AD0.
* Creation Date    : 2026-02-07
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "Config_S12AD0.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_S12AD0_Create
* Description  : This function initializes the S12AD0 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_S12AD0_Create(void)
{
    /* Cancel S12AD0 module stop state */
    MSTP(S12AD) = 0U;  

    /* Disable and clear interrupt flags of S12AD0 module */
    S12AD.ADCSR.BIT.ADIE = 0U;

    /* Set S12AD0 control registers */
    S12AD.ADDISCR.BYTE = _00_AD_DISCONECT_UNUSED;
    S12AD.ADCSR.WORD = _0000_AD_DBLTRIGGER_DISABLE | _0000_AD_SYNCASYNCTRG_DISABLE | 
                       _0000_AD_SCAN_END_INTERRUPT_DISABLE | _0000_AD_SINGLE_SCAN_MODE;
    S12AD.ADHVREFCNT.BYTE |= (_00_AD_HIGH_POTENTIAL_AVCC0 | _00_AD_LOW_POTENTIAL_AVSS0);
    S12AD.ADBUFEN.BYTE |= _00_AD_STORAGE_BUFF_UNUSED;
    S12AD.ADCER.WORD = _0000_AD_AUTO_CLEARING_DISABLE | _0000_AD_SELFTDIAGST_DISABLE | _0000_AD_RIGHT_ALIGNMENT;
    S12AD.ADADC.BYTE = _03_AD_4_TIME_CONVERSION | _80_AD_AVERAGE_MODE;

    /* Set channels and sampling time */
    S12AD.ADANSA0.WORD = _0001_AD_AN000_USED | _0002_AD_AN001_USED | _0004_AD_AN002_USED;
    S12AD.ADANSA1.WORD = _0002_AD_AN017_USED;
    S12AD.ADSSTR0 = _14_AD0_SAMPLING_STATE_0;
    S12AD.ADSSTR1 = _14_AD0_SAMPLING_STATE_1;
    S12AD.ADSSTR2 = _14_AD0_SAMPLING_STATE_2;
    S12AD.ADSSTRL = _14_AD0_SAMPLING_STATE_L;

    /* Set compare control register */
    S12AD.ADCMPCR.WORD = _0000_AD_WINDOWB_DISABLE | _0000_AD_WINDOWA_DISABLE | _0000_AD_WINDOWFUNCTION_DISABLE;

    /* Set ELC scan end event generation condition */
    S12AD.ADELCCR.BYTE = _02_ALL_SCAN_COMPLETION;

    /* Set AN000 pin */
    PORT4.PMR.BYTE &= 0xFEU;
    PORT4.PDR.BYTE &= 0xFEU;
    MPC.P40PFS.BYTE = 0x80U;

    /* Set AN001 pin */
    PORT4.PMR.BYTE &= 0xFDU;
    PORT4.PDR.BYTE &= 0xFDU;
    MPC.P41PFS.BYTE = 0x80U;

    /* Set AN002 pin */
    PORT4.PMR.BYTE &= 0xFBU;
    PORT4.PDR.BYTE &= 0xFBU;
    MPC.P42PFS.BYTE = 0x80U;

    /* Set AN017 pin */
    PORTE.PMR.BYTE &= 0xFDU;
    PORTE.PDR.BYTE &= 0xFDU;
    MPC.PE1PFS.BYTE = 0x80U;

    R_Config_S12AD0_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_Config_S12AD0_Start
* Description  : This function starts the AD0 converter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_S12AD0_Start(void)
{
    S12AD.ADCSR.BIT.ADST = 1U;
}

/***********************************************************************************************************************
* Function Name: R_Config_S12AD0_Stop
* Description  : This function stop the AD0 converter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_S12AD0_Stop(void)
{
    S12AD.ADCSR.BIT.ADST = 0U;
}

/***********************************************************************************************************************
* Function Name: R_Config_S12AD0_Get_ValueResult
* Description  : This function gets result from the AD0 converter
* Arguments    : channel -
*                    channel of data register to be read
*                buffer -
*                    buffer pointer
* Return Value : None
***********************************************************************************************************************/

void R_Config_S12AD0_Get_ValueResult(ad_channel_t channel, uint16_t * const buffer)
{
    switch (channel)
    {
        case ADSELFDIAGNOSIS:
        {
            *buffer = (uint16_t)(S12AD.ADRD.WORD);
            break;
        }
        case ADCHANNEL0:
        {
            *buffer = (uint16_t)(S12AD.ADDR0);
            break;
        }
        case ADCHANNEL1:
        {
            *buffer = (uint16_t)(S12AD.ADDR1);
            break;
        }
        case ADCHANNEL2:
        {
            *buffer = (uint16_t)(S12AD.ADDR2);
            break;
        }
        case ADCHANNEL3:
        {
            *buffer = (uint16_t)(S12AD.ADDR3);
            break;
        }
        case ADCHANNEL4:
        {
            *buffer = (uint16_t)(S12AD.ADDR4);
            break;
        }
        case ADCHANNEL5:
        {
            *buffer = (uint16_t)(S12AD.ADDR5);
            break;
        }
        case ADCHANNEL6:
        {
            *buffer = (uint16_t)(S12AD.ADDR6);
            break;
        }
        case ADCHANNEL7:
        {
            *buffer = (uint16_t)(S12AD.ADDR7);
            break;
        }
        case ADCHANNEL16:
        {
            *buffer = (uint16_t)(S12AD.ADDR16);
            break;
        }
        case ADCHANNEL17:
        {
            *buffer = (uint16_t)(S12AD.ADDR17);
            break;
        }
        case ADCHANNEL18:
        {
            *buffer = (uint16_t)(S12AD.ADDR18);
            break;
        }
        case ADCHANNEL19:
        {
            *buffer = (uint16_t)(S12AD.ADDR19);
            break;
        }
        case ADCHANNEL20:
        {
            *buffer = (uint16_t)(S12AD.ADDR20);
            break;
        }
        case ADCHANNEL21:
        {
            *buffer = (uint16_t)(S12AD.ADDR21);
            break;
        }
        case ADCHANNEL22:
        {
            *buffer = (uint16_t)(S12AD.ADDR22);
            break;
        }
        case ADCHANNEL23:
        {
            *buffer = (uint16_t)(S12AD.ADDR23);
            break;
        }
        case ADCHANNEL24:
        {
            *buffer = (uint16_t)(S12AD.ADDR24);
            break;
        }
        case ADCHANNEL25:
        {
            *buffer = (uint16_t)(S12AD.ADDR25);
            break;
        }
        case ADCHANNEL26:
        {
            *buffer = (uint16_t)(S12AD.ADDR26);
            break;
        }
        case ADCHANNEL27:
        {
            *buffer = (uint16_t)(S12AD.ADDR27);
            break;
        }
        case ADCHANNEL28:
        {
            *buffer = (uint16_t)(S12AD.ADDR28);
            break;
        }
        case ADCHANNEL29:
        {
            *buffer = (uint16_t)(S12AD.ADDR29);
            break;
        }
        case ADCHANNEL30:
        {
            *buffer = (uint16_t)(S12AD.ADDR30);
            break;
        }
        case ADCHANNEL31:
        {
            *buffer = (uint16_t)(S12AD.ADDR31);
            break;
        }
        case ADTEMPSENSOR:
        {
            *buffer = (uint16_t)(S12AD.ADTSDR);
            break;
        }
        case ADINTERREFVOLT:
        {
            *buffer = (uint16_t)(S12AD.ADOCDR);
            break;
        }
        case ADDATADUPLICATION:
        {
            *buffer = (uint16_t)(S12AD.ADDBLDR);
            break;
        }
        default:
        {
            break;
        }
    }
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
