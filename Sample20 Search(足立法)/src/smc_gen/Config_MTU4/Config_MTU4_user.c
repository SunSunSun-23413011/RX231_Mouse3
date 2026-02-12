/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : Config_MTU4_user.c
* Component Version: 1.12.0
* Device(s)        : R5F52315AxFL
* Description      : This file implements device driver for Config_MTU4.
* Creation Date    : 2026-01-28
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
* Function Name: R_Config_MTU4_Create_UserInit
* Description  : This function adds user code after initializing the MTU4 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MTU4_Create_UserInit(void)
{
    /* Start user code for user init. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_Config_MTU4_tgib4_interrupt
* Description  : This function is TGIB4 interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

#if FAST_INTERRUPT_VECTOR == VECT_MTU4_TGIB4
#pragma interrupt r_Config_MTU4_tgib4_interrupt(vect=VECT(MTU4,TGIB4),fint)
#else
#pragma interrupt r_Config_MTU4_tgib4_interrupt(vect=VECT(MTU4,TGIB4))
#endif
static void r_Config_MTU4_tgib4_interrupt(void)
{
    /* Set bit PSW.I = 1 to allow multiple interrupts */
    R_BSP_SETPSW_I();

    /* Start user code for r_Config_MTU4_tgib4_interrupt. Do not edit comment generated here */
    int_mot_r( );
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
