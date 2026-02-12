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
* Creation Date    : 2026-02-06
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
#define MTU0_PCLK_COUNTER_DIVISION      (4)
#define _031F_TGRA0_VALUE               (0x031FU) /* TGRA0 value */
#define _031F_TGRB0_VALUE               (0x031FU) /* TGRB0 value */
#define _031F_TGRC0_VALUE               (0x031FU) /* TGRC0 value */
#define _031F_TGRD0_VALUE               (0x031FU) /* TGRD0 value */
#define _031F_TGRE0_VALUE               (0x031FU) /* TGRE0 value */
#define _031F_TGRF0_VALUE               (0x031FU) /* TGRF0 value */

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
