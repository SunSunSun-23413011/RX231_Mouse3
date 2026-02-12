/*
* Copyright (c) 2006(2025) Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/***********************************************************************/
/*                                                                     */
/*  FILE        :Main.c or Main.cpp                                    */
/*  DESCRIPTION :Main Program                                          */
/*  CPU TYPE    :                                                      */
/*                                                                     */
/*  History     : 1.00  (2006-10-31)                                   */
/*              : 1.00A (2025-04-11)                                   */
/*                                                                     */
/*  NOTE:THIS IS A TYPICAL EXAMPLE.                                    */
/*                                                                     */
/***********************************************************************/
//#include "typedefine.h"
#include "Pin.h"
#include "iodefine.h"
#ifdef __cplusplus
//#include <ios>                        // Remove the comment when you use ios
//_SINT ios_base::Init::init_cnt;       // Remove the comment when you use ios
#endif

void main(void);
#ifdef __cplusplus
extern "C" {
void abort(void);
}
#endif

//---------------------------------------------------------------
//  型宣言
//---------------------------------------------------------------
#define  uchar    unsigned char
#define  ushort   unsigned short
#define  vshort   volatile short
#define  vushort  volatile unsigned short

//---------------------------------------------------------------
//  関数プロトタイプ宣言
//---------------------------------------------------------------
void IO_init( void );

//---------------------------------------------------------------
//  メインプログラム
//---------------------------------------------------------------
void main(void)
{
    IO_init();      // RX231初期化
    while(1)
    {
    int led_wait1, led_wait2;
    PIN_WRITE(CPU_LED) = 0;          // LED 点灯
    for( led_wait1 = 0 ; led_wait1 < 1000 ; led_wait1++ ){
      for( led_wait2 = 0 ; led_wait2 < 100 ; led_wait2++ ){
      }
    }
    PIN_WRITE(CPU_LED) = 1;          // LED 消灯
    for( led_wait1 = 0 ; led_wait1 < 1000 ; led_wait1++ ){
      for( led_wait2 = 0 ; led_wait2 < 100 ; led_wait2++ ){
      }
    }
    }
}

//---------------------------------------------------------------
//  RX231初期化
//---------------------------------------------------------------
void IO_init( void )
{
  R_Systeminit();
}


#ifdef __cplusplus
void abort(void)
{

}
#endif
