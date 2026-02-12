/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : Config_MTU3.c
* Component Version: 1.12.0
* Device(s)        : R5F52315AxFL
* Description      : This file implements device driver for Config_MTU3.
* Creation Date    : 2026-02-02
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
#include "Config_MTU3.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_MTU3_Create
* Description  : This function initializes the MTU3 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MTU3_Create(void)
{
    /* Release MTU channel 3 from stop state */
    MSTP(MTU3) = 0U;

    /* Enable read/write to MTU3 registers */
    MTU.TRWER.BIT.RWE = 1U;

    /* Stop MTU channel 3 counter */
    MTU.TSTR.BIT.CST3 = 0U;

    /* Set TGIA/TGIB/TGIC/TGID interrupt priority level */
    IPR(MTU3, TGIA3) = _0D_MTU_PRIORITY_LEVEL13;

    /* MTU channel 3 is used as PWM mode 1 */
    MTU.TSYR.BIT.SYNC3 = 0U;
    MTU3.TCR.BYTE = _02_MTU_PCLK_16 | _00_MTU_CKEG_RISE | _20_MTU_CKCL_A;
    MTU3.TIER.BYTE = _00_MTU_TGIEA_DISABLE | _02_MTU_TGIEB_ENABLE | _00_MTU_TCIEV_DISABLE | _00_MTU_TTGE_DISABLE;
    MTU3.TBTM.BYTE = _00_MTU_TTSA_CMMA | _00_MTU_TTSB_CMMB;
    MTU3.TMDR.BYTE = _02_MTU_PWM1 | _10_MTU_BFA_BUFFER | _20_MTU_BFB_BUFFER;
    MTU3.TIORH.BYTE = _01_MTU_IOA_LL | _60_MTU_IOB_HH;
    MTU3.TGRA = _09C4_TGRA3_VALUE;
    MTU3.TGRB = _0032_TGRB3_VALUE;
    MTU3.TGRC = _09C4_TGRC3_VALUE;
    MTU3.TGRD = _0064_TGRD3_VALUE;

    /* Disable read/write to MTU3 registers */
    MTU.TRWER.BIT.RWE = 0U;

    /* Set MTIOC3A pin */
    MPC.P14PFS.BYTE = 0x01U;
    PORT1.PMR.BYTE |= 0x10U;

    R_Config_MTU3_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_Config_MTU3_Start
* Description  : This function starts the MTU3 channel counter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MTU3_Start(void)
{
    /* Enable TGIB3 interrupt in ICU */
    IEN(MTU3, TGIB3) = 1U;
    
    /* Start MTU channel 3 counter */
    MTU.TSTR.BIT.CST3 = 1U;
}

/***********************************************************************************************************************
* Function Name: R_Config_MTU3_Stop
* Description  : This function stops the MTU3 channel counter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MTU3_Stop(void)
{
    /* Disable TGIB3 interrupt in ICU */
    IEN(MTU3, TGIB3) = 0U;
    
    /* Stop MTU channel 3 counter */
    MTU.TSTR.BIT.CST3 = 0U;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
