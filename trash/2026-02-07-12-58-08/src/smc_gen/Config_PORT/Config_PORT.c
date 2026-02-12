/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : Config_PORT.c
* Component Version: 2.4.1
* Device(s)        : R5F52315AxFL
* Description      : This file implements device driver for Config_PORT.
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
#include "Config_PORT.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_PORT_Create
* Description  : This function initializes the PORT
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_PORT_Create(void)
{
    /* Set PORT1 registers */
    PORT1.PODR.BYTE = _40_Pm6_OUTPUT_1 | _80_Pm7_OUTPUT_1;
    PORT1.ODR1.BYTE = _00_Pm4_CMOS_OUTPUT | _00_Pm5_CMOS_OUTPUT | _00_Pm6_CMOS_OUTPUT | _00_Pm7_CMOS_OUTPUT;
    PORT1.DSCR.BYTE = _00_Pm4_HIDRV_OFF | _00_Pm5_HIDRV_OFF | _00_Pm6_HIDRV_OFF | _00_Pm7_HIDRV_OFF;
    PORT1.PMR.BYTE = _00_Pm6_PIN_GPIO | _00_Pm7_PIN_GPIO;
    PORT1.PDR.BYTE = _40_Pm6_MODE_OUTPUT | _80_Pm7_MODE_OUTPUT | _0F_PDR1_DEFAULT;

    /* Set PORT2 registers */
    PORT2.ODR1.BYTE = _00_Pm6_CMOS_OUTPUT;
    PORT2.PCR.BYTE = _80_Pm7_PULLUP_ON;
    PORT2.DSCR.BYTE = _00_Pm6_HIDRV_OFF;
    PORT2.PMR.BYTE = _00_Pm7_PIN_GPIO;
    PORT2.PDR.BYTE = _00_Pm7_MODE_INPUT | _3F_PDR2_DEFAULT;

    /* Set PORT3 registers */
    PORT3.ODR0.BYTE = _00_Pm0_CMOS_OUTPUT;
    PORT3.ODR1.BYTE = _00_Pm6_CMOS_OUTPUT | _00_Pm7_CMOS_OUTPUT;
    PORT3.PCR.BYTE = _02_Pm1_PULLUP_ON;
    PORT3.DSCR.BYTE = _00_Pm0_HIDRV_OFF;
    PORT3.PMR.BYTE = _00_Pm1_PIN_GPIO | _00_Pm5_PIN_GPIO;
    PORT3.PDR.BYTE = _00_Pm1_MODE_INPUT | _00_Pm5_MODE_INPUT | _1C_PDR3_DEFAULT;

    /* Set PORTA registers */
    PORTA.PODR.BYTE = _00_Pm1_OUTPUT_0 | _00_Pm3_OUTPUT_0 | _00_Pm4_OUTPUT_0 | _00_Pm6_OUTPUT_0;
    PORTA.ODR0.BYTE = _00_Pm1_CMOS_OUTPUT | _00_Pm3_CMOS_OUTPUT;
    PORTA.ODR1.BYTE = _00_Pm4_CMOS_OUTPUT | _00_Pm6_CMOS_OUTPUT;
    PORTA.DSCR.BYTE = _00_Pm1_HIDRV_OFF | _00_Pm3_HIDRV_OFF | _00_Pm4_HIDRV_OFF | _00_Pm6_HIDRV_OFF;
    PORTA.PMR.BYTE = _00_Pm1_PIN_GPIO | _00_Pm3_PIN_GPIO | _00_Pm4_PIN_GPIO | _00_Pm6_PIN_GPIO;
    PORTA.PDR.BYTE = _02_Pm1_MODE_OUTPUT | _08_Pm3_MODE_OUTPUT | _10_Pm4_MODE_OUTPUT | _40_Pm6_MODE_OUTPUT | 
                     _A5_PDRA_DEFAULT;

    /* Set PORTB registers */
    PORTB.ODR0.BYTE = _00_Pm0_CMOS_OUTPUT | _00_Pm1_CMOS_OUTPUT | _00_Pm3_CMOS_OUTPUT;
    PORTB.ODR1.BYTE = _00_Pm5_CMOS_OUTPUT;
    PORTB.DSCR.BYTE = _00_Pm0_HIDRV_OFF | _00_Pm1_HIDRV_OFF | _00_Pm3_HIDRV_OFF | _00_Pm5_HIDRV_OFF;

    /* Set PORTC registers */
    PORT.PSRB.BYTE = _01_PORT_PSEL0_PC0 | _02_PORT_PSEL1_PC1 | _20_PORT_PSEL5_PC3;
    PORTC.PODR.BYTE = _00_Pm0_OUTPUT_0 | _00_Pm1_OUTPUT_0 | _08_Pm3_OUTPUT_1 | _20_Pm5_OUTPUT_1 | _40_Pm6_OUTPUT_1;
    PORTC.ODR0.BYTE = _00_Pm0_CMOS_OUTPUT | _00_Pm1_CMOS_OUTPUT | _00_Pm2_CMOS_OUTPUT | _00_Pm3_CMOS_OUTPUT;
    PORTC.ODR1.BYTE = _00_Pm4_CMOS_OUTPUT | _00_Pm5_CMOS_OUTPUT | _00_Pm6_CMOS_OUTPUT | _00_Pm7_CMOS_OUTPUT;
    PORTC.DSCR.BYTE = _01_Pm0_HIDRV_ON | _02_Pm1_HIDRV_ON | _00_Pm2_HIDRV_OFF | _00_Pm3_HIDRV_OFF | 
                      _00_Pm4_HIDRV_OFF | _00_Pm5_HIDRV_OFF | _00_Pm6_HIDRV_OFF | _00_Pm7_HIDRV_OFF;
    PORTC.PMR.BYTE = _00_Pm0_PIN_GPIO | _00_Pm1_PIN_GPIO | _00_Pm3_PIN_GPIO | _00_Pm5_PIN_GPIO | _00_Pm6_PIN_GPIO;
    PORTC.PDR.BYTE = _01_Pm0_MODE_OUTPUT | _02_Pm1_MODE_OUTPUT | _08_Pm3_MODE_OUTPUT | _20_Pm5_MODE_OUTPUT | 
                     _40_Pm6_MODE_OUTPUT | _04_PDRC_DEFAULT;

    /* Set PORTE registers */
    PORTE.PODR.BYTE = _00_Pm4_OUTPUT_0;
    PORTE.ODR0.BYTE = _00_Pm1_CMOS_OUTPUT | _00_Pm2_CMOS_OUTPUT;
    PORTE.ODR1.BYTE = _00_Pm4_CMOS_OUTPUT;
    PORTE.PCR.BYTE = _08_Pm3_PULLUP_ON;
    PORTE.DSCR.BYTE = _00_Pm1_HIDRV_OFF | _00_Pm2_HIDRV_OFF | _00_Pm4_HIDRV_OFF;
    PORTE.PMR.BYTE = _00_Pm3_PIN_GPIO | _00_Pm4_PIN_GPIO;
    PORTE.PDR.BYTE = _00_Pm3_MODE_INPUT | _10_Pm4_MODE_OUTPUT | _E1_PDRE_DEFAULT;

    R_Config_PORT_Create_UserInit();
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
