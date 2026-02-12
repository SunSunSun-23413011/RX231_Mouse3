/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : Config_MTU0.h
* Component Version: 1.12.0
* Device(s)        : R5F52315AxFL
* Description      : This file implements device driver for Config_MTU0.
* Creation Date    : 2026-01-25
***********************************************************************************************************************/

#ifndef CFG_Config_MTU0_H
#define CFG_Config_MTU0_H

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_mtu2.h"

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define MTU0_PCLK_COUNTER_DIVISION      (64)
#define _001E_TGRA0_VALUE               (0x001EU) /* TGRA0 value */
#define _001E_TGRB0_VALUE               (0x001EU) /* TGRB0 value */
#define _001E_TGRC0_VALUE               (0x001EU) /* TGRC0 value */
#define _001E_TGRD0_VALUE               (0x001EU) /* TGRD0 value */
#define _001E_TGRE0_VALUE               (0x001EU) /* TGRE0 value */
#define _001E_TGRF0_VALUE               (0x001EU) /* TGRF0 value */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_Config_MTU0_Create(void);
void R_Config_MTU0_Create_UserInit(void);
void R_Config_MTU0_Start(void);
void R_Config_MTU0_Stop(void);
/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif
