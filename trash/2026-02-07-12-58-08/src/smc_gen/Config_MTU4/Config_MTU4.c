/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : Config_MTU4.c
* Component Version: 1.12.0
* Device(s)        : R5F52315AxFL
* Description      : This file implements device driver for Config_MTU4.
* Creation Date    : 2026-02-06
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
#include "Config_MTU4.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_MTU4_Create
* Description  : This function initializes the MTU4 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MTU4_Create(void)
{
    /* Release MTU channel 4 from stop state */
    MSTP(MTU4) = 0U;

    /* Enable read/write to MTU4 registers */
    MTU.TRWER.BIT.RWE = 1U;

    /* Stop MTU channel 4 counter */
    MTU.TSTR.BIT.CST4 = 0U;

    /* Set TGIA/TGIB/TGIC/TGID interrupt priority level */
    IPR(MTU4, TGIA4) = _0C_MTU_PRIORITY_LEVEL12;

    /* MTU channel 4 is used as PWM mode 1 */
    MTU.TSYR.BIT.SYNC4 = 0U;
    MTU.TOER.BYTE |= (_C2_MTU_OE4A_ENABLE);
    MTU4.TCR.BYTE = _02_MTU_PCLK_16 | _00_MTU_CKEG_RISE | _20_MTU_CKCL_A;
    MTU4.TIER.BYTE = _00_MTU_TGIEA_DISABLE | _02_MTU_TGIEB_ENABLE | _00_MTU_TCIEV_DISABLE | _00_MTU_TTGE_DISABLE;
    MTU4.TBTM.BYTE = _00_MTU_TTSA_CMMA | _00_MTU_TTSB_CMMB;
    MTU4.TMDR.BYTE = _02_MTU_PWM1 | _10_MTU_BFA_BUFFER | _20_MTU_BFB_BUFFER;
    MTU4.TIORH.BYTE = _01_MTU_IOA_LL | _60_MTU_IOB_HH;
    MTU4.TGRA = _09C4_TGRA4_VALUE;
    MTU4.TGRB = _0032_TGRB4_VALUE;
    MTU4.TGRC = _09C4_TGRC4_VALUE;
    MTU4.TGRD = _0064_TGRD4_VALUE;

    /* Disable read/write to MTU4 registers */
    MTU.TRWER.BIT.RWE = 0U;

    /* Set MTIOC4A pin */
    MPC.PB3PFS.BYTE = 0x02U;
    PORTB.PMR.BYTE |= 0x08U;

    R_Config_MTU4_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_Config_MTU4_Start
* Description  : This function starts the MTU4 channel counter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MTU4_Start(void)
{
    /* Enable TGIB4 interrupt in ICU */
    IEN(MTU4, TGIB4) = 1U;
    
    /* Start MTU channel 4 counter */
    MTU.TSTR.BIT.CST4 = 1U;
}

/***********************************************************************************************************************
* Function Name: R_Config_MTU4_Stop
* Description  : This function stops the MTU4 channel counter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MTU4_Stop(void)
{
    /* Disable TGIB4 interrupt in ICU */
    IEN(MTU4, TGIB4) = 0U;
    
    /* Stop MTU channel 4 counter */
    MTU.TSTR.BIT.CST4 = 0U;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
