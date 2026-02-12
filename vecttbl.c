/*
* Copyright (c) 2014(2025) Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/************************************************************************
*
* Device     : RX/RX200/RX231
*
* File Name  : vecttbl.c
*
* Abstract   : Initialize of Vector Table.
*
* History    : 0.50  (2014-09-05)  [Hardware Manual Revision : 0.50]
*            : 0.51  (2014-10-01)  [Hardware Manual Revision : 0.50]
*            : 1.00  (2025-04-11)  [Hardware Manual Revision : 1.00]
*
* NOTE       : THIS IS A TYPICAL EXAMPLE.
*
************************************************************************/

#include "vect.h"

// DO NOT MOVE THE ADDESS OF EXCEPTVECT
#pragma section C EXCEPTVECT
void (*const Except_Vectors[])(void) = {
//;0xffffff80  MDE register
#ifdef __BIG
    (void (*)(void))0xfffffff8, // big
#else
    (void (*)(void))0xffffffff, // little
#endif
//;0xffffff84  Reserved
    Dummy,
//;0xffffff88  OFS1 register
    (void (*)(void))0xffffffff, // OFS1
//;0xffffff8c  OFS0 register
    (void (*)(void))0xffffffff, // OFS0
//;0xffffff90  Reserved
    Dummy,
//;0xffffff94  Reserved
    Dummy,
//;0xffffff98  Reserved
    Dummy,
//;0xffffff9c  Reserved
    Dummy,
//;0xffffffa0  ID
    (void (*)(void))0xffffffff,
//;0xffffffa4  ID
    (void (*)(void))0xffffffff,
//;0xffffffa8  ID
    (void (*)(void))0xffffffff,
//;0xffffffac  ID
    (void (*)(void))0xffffffff,
//;0xffffffb0  Reserved
    Dummy,
//;0xffffffb4  Reserved
    Dummy,
//;0xffffffb8  Reserved
    Dummy,
//;0xffffffbc  Reserved
    Dummy,
//;0xffffffc0  Reserved
    Dummy,
//;0xffffffc4  Reserved
    Dummy,
//;0xffffffc8  Reserved
    Dummy,
//;0xffffffcc  Reserved
    Dummy,
//;0xffffffd0  Exception(Supervisor Instruction)
    Excep_SuperVisorInst,
//;0xffffffd4  Exception(Access Instruction)
    Excep_AccessInst,
//;0xffffffd8  Reserved
    Dummy,
//;0xffffffdc  Exception(Undefined Instruction)
    Excep_UndefinedInst,
//;0xffffffe0  Reserved
    Dummy,
//;0xffffffe4  Exception(Floating Point)
    Excep_FloatingPoint,
//;0xffffffe8  Reserved
    Dummy,
//;0xffffffec  Reserved
    Dummy,
//;0xfffffff0  Reserved
    Dummy,
//;0xfffffff4  Reserved
    Dummy,
//;0xfffffff8  NMI
    NonMaskableInterrupt,
};

#pragma section C RESETVECT
void (*const Reset_Vectors[])(void) = {
//;<<VECTOR DATA START (POWER ON RESET)>>
//;Power On Reset PC
    /*(void*)*/ PowerON_Reset_PC                                                                                                                 
//;<<VECTOR DATA END (POWER ON RESET)>>
};
