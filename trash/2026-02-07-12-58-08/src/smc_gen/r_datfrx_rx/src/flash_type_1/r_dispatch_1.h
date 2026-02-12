/*
* Copyright (c) 2018(2025) Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/************************************************************************************************
* File Name    : r_dispatch_1.h
* Version      : 2.31
* Description  : DATFRX interface header file
*************************************************************************************************/
/************************************************************************************************
* History      : DD.MM.YYYY Version  Description
*              : 28.09.2018 2.00     First Release
*              : 20.03.2025 2.31     Changed the disclaimer.
*************************************************************************************************/

/************************************************************************************************
Includes <System Includes> , "Project Includes"
*************************************************************************************************/
#include "r_datfrx_rx_config.h"
#include "r_flash_rx_if.h"
#include "r_flash_dm_private.h"

/************************************************************************************************
Macro definitions
*************************************************************************************************/
#ifndef R_DISPATCH_1_H
#define R_DISPATCH_1_H

/************************************************************************************************
Global Typedef definitions
*************************************************************************************************/

/************************************************************************************************
Exported global variables
*************************************************************************************************/
extern uint32_t g_flash_dm_err_flg;

/************************************************************************************************
Exported global functions (to be accessed by other files)
*************************************************************************************************/
#if(FLASH_TYPE == FLASH_TYPE_1)
/* user callback */
extern p_flash_dm_callback r_flash_dm_callbackfunc;
extern flash_dm_ptr_t      r_flash_dm_drvif_get_dataaddr(uint8_t data_no);
extern uint32_t            r_flash_dm_drvif_get_dataaddr_of_addr(uint8_t data_no);
extern uint32_t            r_flash_dm_drvif_get_datanum(void);
extern uint16_t            r_flash_dm_drvif_get_datasize(uint32_t data_no);
extern void                r_flash_dm_block_set_program_addr(flash_dm_ptr_t p_addr);
extern flash_dm_ptr_t      r_flash_dm_block_get_program_addr(void);
e_flash_dm_status_t        r_flash_dm_dispatch(e_flash_dm_driver_status_t pc);

#endif /* FLASH_TYPE */

#endif /* R_DISPATCH_1_H */

/* End of File */

