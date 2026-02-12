/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : Config_MTU1_user.c
* Component Version: 1.12.0
* Device(s)        : R5F52315AxFL
* Description      : This file implements device driver for Config_MTU1.
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
#include "Config_MTU1.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_MTU1_Create_UserInit
* Description  : This function adds user code after initializing the MTU1 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MTU1_Create_UserInit(void)
{
    /* Start user code for user init. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_Config_MTU1_tgia1_interrupt
* Description  : This function is TGIA1 interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

#if FAST_INTERRUPT_VECTOR == VECT_MTU1_TGIA1
#pragma interrupt r_Config_MTU1_tgia1_interrupt(vect=VECT(MTU1,TGIA1),fint)
#else
#pragma interrupt r_Config_MTU1_tgia1_interrupt(vect=VECT(MTU1,TGIA1))
#endif
static void r_Config_MTU1_tgia1_interrupt(void)
{
    
    /* Start user code for r_Config_MTU1_tgia1_interrupt. Do not edit comment generated here */
    Int_MTU1_TGIA1( );
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
