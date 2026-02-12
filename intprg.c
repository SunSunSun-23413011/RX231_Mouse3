/*
* Copyright (c) 2014(2015,2025) Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/************************************************************************
*
* Device     : RX/RX200/RX231
*
* File Name  : intprg.c
*
* Abstract   : Interrupt Program.
*
* History    : 0.50  (2014-09-18)  [Hardware Manual Revision : 0.50]
*            : 1.00  (2015-05-18)  [Hardware Manual Revision : 1.00]
*            : 1.00A (2025-04-11)  [Hardware Manual Revision : 1.00]
*
* NOTE       : THIS IS A TYPICAL EXAMPLE.
*
*********************************************************************/
#include <machine.h>
#include "vect.h"

#pragma section IntPRG

// Exception(Supervisor Instruction)
void Excep_SuperVisorInst(void){/* brk(); */}

// Exception(Access Instruction)
void Excep_AccessInst(void){/* brk(); */}

// Exception(Undefined Instruction)
void Excep_UndefinedInst(void){/* brk(); */}

// Exception(Floating Point)
void Excep_FloatingPoint(void){/* brk(); */}

// NMI
void NonMaskableInterrupt(void){/* brk(); */}

// Dummy
void Dummy(void){/* brk(); */}

// BRK
void Excep_BRK(void){ wait(); }

// BSC BUSERR
void Excep_BSC_BUSERR(void){/* brk(); */}

// FCU FRDYI
void Excep_FCU_FRDYI(void){/* brk(); */}

// ICU SWINT
void Excep_ICU_SWINT(void){/* brk(); */}

// CMT0 CMI0
void Excep_CMT0_CMI0(void){/* brk(); */}

// CMT1 CMI1
void Excep_CMT1_CMI1(void){/* brk(); */}

// CMT2 CMI2
void Excep_CMT2_CMI2(void){/* brk(); */}

// CMT3 CMI3
void Excep_CMT3_CMI3(void){/* brk(); */}

// CAC FERRF
void Excep_CAC_FERRF(void){/* brk(); */}

// CAC MENDF
void Excep_CAC_MENDF(void){/* brk(); */}

// CAC OVFF
void Excep_CAC_OVFF(void){/* brk(); */}

// USB0 D0FIFO0
void Excep_USB0_D0FIFO0(void){/* brk(); */}

// USB0 D1FIFO0
void Excep_USB0_D1FIFO0(void){/* brk(); */}

// USB0 USBI0
void Excep_USB0_USBI0(void){/* brk(); */}

// SDHI SBFAI
void Excep_SDHI_SBFAI(void){/* brk(); */}

// SDHI CDETI
void Excep_SDHI_CDETI(void){/* brk(); */}

// SDHI CACI
void Excep_SDHI_CACI(void){/* brk(); */}

// SDHI SDACI
void Excep_SDHI_SDACI(void){/* brk(); */}

// RSPI0 SPEI0
void Excep_RSPI0_SPEI0(void){/* brk(); */}

// RSPI0 SPRI0
void Excep_RSPI0_SPRI0(void){/* brk(); */}

// RSPI0 SPTI0
void Excep_RSPI0_SPTI0(void){/* brk(); */}

// RSPI0 SPII0
void Excep_RSPI0_SPII0(void){/* brk(); */}

// RSCAN COMFRXINT
void Excep_RSCAN_COMFRXINT(void){/* brk(); */}

// RSCAN RXFINT
void Excep_RSCAN_RXFINT(void){/* brk(); */}

// RSCAN TXINT
void Excep_RSCAN_TXINT(void){/* brk(); */}

// RSCAN CHERRINT
void Excep_RSCAN_CHERRINT(void){/* brk(); */}

// RSCAN GLERRINT
void Excep_RSCAN_GLERRINT(void){/* brk(); */}

// DOC DOPCF
void Excep_DOC_DOPCF(void){/* brk(); */}

// CMPB CMPB0
void Excep_CMPB_CMPB0(void){/* brk(); */}

// CMPB CMPB1
void Excep_CMPB_CMPB1(void){/* brk(); */}

// CTSU CTSUWR
void Excep_CTSU_CTSUWR(void){/* brk(); */}

// CTSU CTSURD
void Excep_CTSU_CTSURD(void){/* brk(); */}

// CTSU CTSUFN
void Excep_CTSU_CTSUFN(void){/* brk(); */}

// RTC CUP
void Excep_RTC_CUP(void){/* brk(); */}

// ICU IRQ0
void Excep_ICU_IRQ0(void){/* brk(); */}

// ICU IRQ1
void Excep_ICU_IRQ1(void){/* brk(); */}

// ICU IRQ2
void Excep_ICU_IRQ2(void){/* brk(); */}

// ICU IRQ3
void Excep_ICU_IRQ3(void){/* brk(); */}

// ICU IRQ4
void Excep_ICU_IRQ4(void){/* brk(); */}

// ICU IRQ5
void Excep_ICU_IRQ5(void){/* brk(); */}

// ICU IRQ6
void Excep_ICU_IRQ6(void){/* brk(); */}

// ICU IRQ7
void Excep_ICU_IRQ7(void){/* brk(); */}

// ELC ELSR8I
void Excep_ELC_ELSR8I(void){/* brk(); */}

// LVD LVD1
void Excep_LVD_LVD1(void){/* brk(); */}

// LVD LVD2
void Excep_LVD_LVD2(void){/* brk(); */}

// CMPA CMPA1
//void Excep_CMPA_CMPA1(void){/* brk(); */}

// CMPA CMPA2
//void Excep_CMPA_CMPA2(void){/* brk(); */}

// USB0 USBR0
void Excep_USB0_USBR0(void){/* brk(); */}

// VBATT VBTLVDI
void Excep_VBATT_VBTLVDI(void){/* brk(); */}

// RTC ALM
void Excep_RTC_ALM(void){/* brk(); */}

// RTC PRD
void Excep_RTC_PRD(void){/* brk(); */}

// S12AD S12ADI0
void Excep_S12AD_S12ADI0(void){/* brk(); */}

// S12AD GBADI
void Excep_S12AD_GBADI(void){/* brk(); */}

// CMPB1 CMPB2
void Excep_CMPB1_CMPB2(void){/* brk(); */}

// CMPB1 CMPB3
void Excep_CMPB1_CMPB3(void){/* brk(); */}

// ELC ELSR18I
void Excep_ELC_ELSR18I(void){/* brk(); */}

// ELC ELSR19I
void Excep_ELC_ELSR19I(void){/* brk(); */}

// SSI0 SSIF0
void Excep_SSI0_SSIF0(void){/* brk(); */}

// SSI0 SSIRXI0
void Excep_SSI0_SSIRXI0(void){/* brk(); */}

// SSI0 SSITXI0
void Excep_SSI0_SSITXI0(void){/* brk(); */}

// SECURITY RD
void Excep_SECURITY_RD(void){/* brk(); */}

// SECURITY WR
void Excep_SECURITY_WR(void){/* brk(); */}

// SECURITY ERR
void Excep_SECURITY_ERR(void){/* brk(); */}

// MTU0 TGIA0
void Excep_MTU0_TGIA0(void){/* brk(); */}

// MTU0 TGIB0
void Excep_MTU0_TGIB0(void){/* brk(); */}

// MTU0 TGIC0
void Excep_MTU0_TGIC0(void){/* brk(); */}

// MTU0 TGID0
void Excep_MTU0_TGID0(void){/* brk(); */}

// MTU0 TCIV0
void Excep_MTU0_TCIV0(void){/* brk(); */}

// MTU0 TGIE0
void Excep_MTU0_TGIE0(void){/* brk(); */}

// MTU0 TGIF0
void Excep_MTU0_TGIF0(void){/* brk(); */}

// MTU1 TGIA1
void Excep_MTU1_TGIA1(void){/* brk(); */}

// MTU1 TGIB1
void Excep_MTU1_TGIB1(void){/* brk(); */}

// MTU1 TCIV1
void Excep_MTU1_TCIV1(void){/* brk(); */}

// MTU1 TCIU1
void Excep_MTU1_TCIU1(void){/* brk(); */}

// MTU2 TGIA2
void Excep_MTU2_TGIA2(void){/* brk(); */}

// MTU2 TGIB2
void Excep_MTU2_TGIB2(void){/* brk(); */}

// MTU2 TCIV2
void Excep_MTU2_TCIV2(void){/* brk(); */}

// MTU2 TCIU2
void Excep_MTU2_TCIU2(void){/* brk(); */}

// MTU3 TGIA3
void Excep_MTU3_TGIA3(void){/* brk(); */}

// MTU3 TGIB3
void Excep_MTU3_TGIB3(void){/* brk(); */}

// MTU3 TGIC3
void Excep_MTU3_TGIC3(void){/* brk(); */}

// MTU3 TGID3
void Excep_MTU3_TGID3(void){/* brk(); */}

// MTU3 TCIV3
void Excep_MTU3_TCIV3(void){/* brk(); */}

// MTU4 TGIA4
void Excep_MTU4_TGIA4(void){/* brk(); */}

// MTU4 TGIB4
void Excep_MTU4_TGIB4(void){/* brk(); */}

// MTU4 TGIC4
void Excep_MTU4_TGIC4(void){/* brk(); */}

// MTU4 TGID4
void Excep_MTU4_TGID4(void){/* brk(); */}

// MTU4 TCIV4
void Excep_MTU4_TCIV4(void){/* brk(); */}

// MTU5 TGIU5
void Excep_MTU5_TGIU5(void){/* brk(); */}

// MTU5 TGIV5
void Excep_MTU5_TGIV5(void){/* brk(); */}

// MTU5 TGIW5
void Excep_MTU5_TGIW5(void){/* brk(); */}

// TPU0 TGI0A
void Excep_TPU0_TGI0A(void){/* brk(); */}

// TPU0 TGI0B
void Excep_TPU0_TGI0B(void){/* brk(); */}

// TPU0 TGI0C
void Excep_TPU0_TGI0C(void){/* brk(); */}

// TPU0 TGI0D
void Excep_TPU0_TGI0D(void){/* brk(); */}

// TPU0 TCI0V
void Excep_TPU0_TCI0V(void){/* brk(); */}

// TPU1 TGI1A
void Excep_TPU1_TGI1A(void){/* brk(); */}

// TPU1 TGI1B
void Excep_TPU1_TGI1B(void){/* brk(); */}

// TPU1 TCI1V
void Excep_TPU1_TCI1V(void){/* brk(); */}

// TPU1 TCI1U
void Excep_TPU1_TCI1U(void){/* brk(); */}

// TPU2 TGI2A
void Excep_TPU2_TGI2A(void){/* brk(); */}

// TPU2 TGI2B
void Excep_TPU2_TGI2B(void){/* brk(); */}

// TPU2 TCI2V
void Excep_TPU2_TCI2V(void){/* brk(); */}

// TPU2 TCI2U
void Excep_TPU2_TCI2U(void){/* brk(); */}

// TPU3 TGI3A
void Excep_TPU3_TGI3A(void){/* brk(); */}

// TPU3 TGI3B
void Excep_TPU3_TGI3B(void){/* brk(); */}

// TPU3 TGI3C
void Excep_TPU3_TGI3C(void){/* brk(); */}

// TPU3 TGI3D
void Excep_TPU3_TGI3D(void){/* brk(); */}

// TPU3 TCI3V
void Excep_TPU3_TCI3V(void){/* brk(); */}

// TPU4 TGI4A
void Excep_TPU4_TGI4A(void){/* brk(); */}

// TPU4 TGI4B
void Excep_TPU4_TGI4B(void){/* brk(); */}

// TPU4 TCI4V
void Excep_TPU4_TCI4V(void){/* brk(); */}

// TPU4 TCI4U
void Excep_TPU4_TCI4U(void){/* brk(); */}

// TPU5 TGI5A
void Excep_TPU5_TGI5A(void){/* brk(); */}

// TPU5 TGI5B
void Excep_TPU5_TGI5B(void){/* brk(); */}

// TPU5 TCI5V
void Excep_TPU5_TCI5V(void){/* brk(); */}

// TPU5 TCI5U
void Excep_TPU5_TCI5U(void){/* brk(); */}

// POE OEI1
void Excep_POE_OEI1(void){/* brk(); */}

// POE OEI2
void Excep_POE_OEI2(void){/* brk(); */}

// TMR0 CMIA0
void Excep_TMR0_CMIA0(void){/* brk(); */}

// TMR0 CMIB0
void Excep_TMR0_CMIB0(void){/* brk(); */}

// TMR0 OVI0
void Excep_TMR0_OVI0(void){/* brk(); */}

// TMR1 CMIA1
void Excep_TMR1_CMIA1(void){/* brk(); */}

// TMR1 CMIB1
void Excep_TMR1_CMIB1(void){/* brk(); */}

// TMR1 OVI1
void Excep_TMR1_OVI1(void){/* brk(); */}

// TMR2 CMIA2
void Excep_TMR2_CMIA2(void){/* brk(); */}

// TMR2 CMIB2
void Excep_TMR2_CMIB2(void){/* brk(); */}

// TMR2 OVI2
void Excep_TMR2_OVI2(void){/* brk(); */}

// TMR3 CMIA3
void Excep_TMR3_CMIA3(void){/* brk(); */}

// TMR3 CMIB3
void Excep_TMR3_CMIB3(void){/* brk(); */}

// TMR3 OVI3
void Excep_TMR3_OVI3(void){/* brk(); */}

// DMAC DMAC0I
void Excep_DMAC_DMAC0I(void){/* brk(); */}

// DMAC DMAC1I
void Excep_DMAC_DMAC1I(void){/* brk(); */}

// DMAC DMAC2I
void Excep_DMAC_DMAC2I(void){/* brk(); */}

// DMAC DMAC3I
void Excep_DMAC_DMAC3I(void){/* brk(); */}

// SCI0 ERI0
void Excep_SCI0_ERI0(void){/* brk(); */}

// SCI0 RXI0
void Excep_SCI0_RXI0(void){/* brk(); */}

// SCI0 TXI0
void Excep_SCI0_TXI0(void){/* brk(); */}

// SCI0 TEI0
void Excep_SCI0_TEI0(void){/* brk(); */}

// SCI1 ERI1
void Excep_SCI1_ERI1(void){/* brk(); */}

// SCI1 RXI1
void Excep_SCI1_RXI1(void){/* brk(); */}

// SCI1 TXI1
void Excep_SCI1_TXI1(void){/* brk(); */}

// SCI1 TEI1
void Excep_SCI1_TEI1(void){/* brk(); */}

// SCI5 ERI5
void Excep_SCI5_ERI5(void){/* brk(); */}

// SCI5 RXI5
void Excep_SCI5_RXI5(void){/* brk(); */}

// SCI5 TXI5
void Excep_SCI5_TXI5(void){/* brk(); */}

// SCI5 TEI5
void Excep_SCI5_TEI5(void){/* brk(); */}

// SCI6 ERI6
void Excep_SCI6_ERI6(void){/* brk(); */}

// SCI6 RXI6
void Excep_SCI6_RXI6(void){/* brk(); */}

// SCI6 TXI6
void Excep_SCI6_TXI6(void){/* brk(); */}

// SCI6 TEI6
void Excep_SCI6_TEI6(void){/* brk(); */}

// SCI8 ERI8
void Excep_SCI8_ERI8(void){/* brk(); */}

// SCI8 RXI8
void Excep_SCI8_RXI8(void){/* brk(); */}

// SCI8 TXI8
void Excep_SCI8_TXI8(void){/* brk(); */}

// SCI8 TEI8
void Excep_SCI8_TEI8(void){/* brk(); */}

// SCI9 ERI9
void Excep_SCI9_ERI9(void){/* brk(); */}

// SCI9 RXI9
void Excep_SCI9_RXI9(void){/* brk(); */}

// SCI9 TXI9
void Excep_SCI9_TXI9(void){/* brk(); */}

// SCI9 TEI9
void Excep_SCI9_TEI9(void){/* brk(); */}

// SCI12 ERI12
void Excep_SCI12_ERI12(void){/* brk(); */}

// SCI12 RXI12
void Excep_SCI12_RXI12(void){/* brk(); */}

// SCI12 TXI12
void Excep_SCI12_TXI12(void){/* brk(); */}

// SCI12 TEI12
void Excep_SCI12_TEI12(void){/* brk(); */}

// SCI12 SCIX0
void Excep_SCI12_SCIX0(void){/* brk(); */}

// SCI12 SCIX1
void Excep_SCI12_SCIX1(void){/* brk(); */}

// SCI12 SCIX2
void Excep_SCI12_SCIX2(void){/* brk(); */}

// SCI12 SCIX3
void Excep_SCI12_SCIX3(void){/* brk(); */}

// RIIC0 EEI0
void Excep_RIIC0_EEI0(void){/* brk(); */}

// RIIC0 RXI0
void Excep_RIIC0_RXI0(void){/* brk(); */}

// RIIC0 TXI0
void Excep_RIIC0_TXI0(void){/* brk(); */}

// RIIC0 TEI0
void Excep_RIIC0_TEI0(void){/* brk(); */}
