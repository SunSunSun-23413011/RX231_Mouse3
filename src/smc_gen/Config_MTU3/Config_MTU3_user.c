/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : Config_MTU3_user.c
* Component Version: 1.12.0
* Device(s)        : R5F52315AxFL
* Description      : This file implements device driver for Config_MTU3.
* Creation Date    : 2026-02-09
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
* Function Name: R_Config_MTU3_Create_UserInit
* Description  : This function adds user code after initializing the MTU3 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MTU3_Create_UserInit(void)
{
    /* Start user code for user init. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_Config_MTU3_tgib3_interrupt
* Description  : This function is TGIB3 interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

#if FAST_INTERRUPT_VECTOR == VECT_MTU3_TGIB3
#pragma interrupt r_Config_MTU3_tgib3_interrupt(vect=VECT(MTU3,TGIB3),fint)
#else
#pragma interrupt r_Config_MTU3_tgib3_interrupt(vect=VECT(MTU3,TGIB3))
#endif
static void r_Config_MTU3_tgib3_interrupt(void)
{
    /* Set bit PSW.I = 1 to allow multiple interrupts */
    R_BSP_SETPSW_I();

    /* Start user code for r_Config_MTU3_tgib3_interrupt. Do not edit comment generated here */
    int_mot_l( );
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
