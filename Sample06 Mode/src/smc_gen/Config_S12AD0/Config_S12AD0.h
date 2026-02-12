/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : Config_S12AD0.h
* Component Version: 2.5.0
* Device(s)        : R5F52315AxFL
* Description      : This file implements device driver for Config_S12AD0.
* Creation Date    : 2026-01-25
***********************************************************************************************************************/

#ifndef CFG_Config_S12AD0_H
#define CFG_Config_S12AD0_H

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_s12ad.h"


/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define _08_AD0_SAMPLING_STATE_0           (0x08U) /* AN000 sampling time setting */
#define _08_AD0_SAMPLING_STATE_1           (0x08U) /* AN001 sampling time setting */
#define _08_AD0_SAMPLING_STATE_2           (0x08U) /* AN002 sampling time setting */
#define _08_AD0_SAMPLING_STATE_L           (0x08U) /* AN016-AN031 sampling time setting */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_Config_S12AD0_Create(void);
void R_Config_S12AD0_Create_UserInit(void);
void R_Config_S12AD0_Start(void);
void R_Config_S12AD0_Stop(void);
void R_Config_S12AD0_Get_ValueResult(ad_channel_t channel, uint16_t * const buffer);
/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif
