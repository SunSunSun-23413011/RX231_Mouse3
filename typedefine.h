/*
* Copyright (c) 2009(2025) Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/*********************************************************************
*
* Device     : RX
*
* File Name  : typedefine.h
*
* Abstract   : Aliases of Integer Type.
*
* History    : 1.00  (2009-08-07)
*            : 1.00A (2025-04-11)
*
* NOTE       : THIS IS A TYPICAL EXAMPLE.
*
*********************************************************************/

typedef signed char _SBYTE;
typedef unsigned char _UBYTE;
typedef signed short _SWORD;
typedef unsigned short _UWORD;
typedef signed int _SINT;
typedef unsigned int _UINT;
typedef signed long _SDWORD;
typedef unsigned long _UDWORD;
typedef signed long long _SQWORD;
typedef unsigned long long _UQWORD;

/* Project-level aliases used in application headers. */
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned char BOOL;
#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif
