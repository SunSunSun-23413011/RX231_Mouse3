/*
* Copyright (C) 2011 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/************************************************************************************
* File Name     : r_mw_version.h
* Version       :
* Device(s)     : Renesas MCUs
* Tool-Chain    :
* H/W Platform  :
* Description   : Renesas Middleware library version information header file
*************************************************************************************/
#ifndef _R_MW_VERSION_H_
#define _R_MW_VERSION_H_

/**********************************************************************************
Macro Defines
***********************************************************************************/
/* Middleware version charactor size */
#define __VER_STIRNG_MAX__ 128

/**********************************************************************************
Type Defines
***********************************************************************************/
/* Middleware version information structure*/
typedef struct mw_version_tag
{
    uint32_t complier;
    uint8_t  library[__VER_STIRNG_MAX__];
}mw_version_t;

#endif /* _R_MW_VERSION_H_ */
