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
#include "LCDrx231.h"
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

//-------------------------------------------------------------------------
//  マクロ定義
//-------------------------------------------------------------------------
#define   SW_ON       0    // スイッチON (Active Low)
#define   SW_OFF      1    // スイッチOFF
#define   KEY_OFF   200    // スイッチ用チャタリングキャンセル時間

//---------------------------------------------------------------
//  グローバル変数定義
//---------------------------------------------------------------
vushort  wait_timer = 0;   // 内部時計( msec ) : wait関数用カウンタ
ushort   SENSOR_PT;        // 割り込み回数カウント用ポインタ
vushort  CMT0_FLAG;   // CMT0割り込み発生フラグ

//---------------------------------------------------------------
//  関数プロトタイプ宣言
//---------------------------------------------------------------
void IO_init( void );
void pause( int x );
void int_timerw( void );
void WaitKeyOff( void );

//---------------------------------------------------------------
//  メインプログラム
//---------------------------------------------------------------
void main(void)
{
    int count = 0; // カウンター用

    IO_init();      // RX231初期化
    LCD_init();    // LCD初期化
    PIN_WRITE(CPU_LED) = 1;    // LED消灯
    // タイトル表示
    LCD_print( 0, "Sample05" );
    LCD_print( 8, "   SW   " );
    pause( 1000 );

    // メインループ
    LCD_print( 8, "        " );      // 表示領域をクリア
    while( 1 ){
        if( PIN_READ( SW_UP ) == SW_ON ){          // 上SWが押されている場合
        WaitKeyOff();                // チャタリング防止処理
        count++;                     // カウンタ+1
        }else if( PIN_READ( SW_DOWN ) == SW_ON ){  // 下SWが押されている場合
        WaitKeyOff();                // チャタリング防止処理
        count--;                     // カウンタ-1
        }else if( PIN_READ( SW_RETURN ) == SW_ON ){  // 実行SWが押されている場合
        WaitKeyOff();                // チャタリング防止処理
        count = 0;                   // カウンタリセット
    }
    LCD_dec_out( 10, count, 3);    // 現在のカウントを表示
  }
}

//---------------------------------------------------------------
//  RX231初期化
//---------------------------------------------------------------
void IO_init( void )
{
  R_Systeminit();
  PIN_WRITE(CPU_LED) = 0;    // LED点灯
  R_Config_CMT0_Start(); // CMT0開始
}

//---------------------------------------------------------------
//  Timer W 割り込み(200us毎にこの関数が勝手に優先して実行される)
//---------------------------------------------------------------
//Config_CMT0_user.cから呼び出される
void int_timerw( void ){
    IR(CMT0,CMI0)=0;				//割り込みステータフラグをクリア
                                 // 忘れると次の割り込みがかからない

    SENSOR_PT++;                 // タスクポインタの更新
    if( SENSOR_PT == 5 ) SENSOR_PT = 0;  // 0-4の5カウント:200us*5=1ms
                               // 各処理は1ms周期で実行される
    switch( SENSOR_PT )          // タスクポインタに従って処理を行う
    {
        case 0:  // 1msecタイマー&LCDの更新
                wait_timer++;     // wait関数用カウンタ
                LCD();            // LCD更新処理
                break;

    default: break;
    }
}

//---------------------------------------------------------------
//  wait関数(1msタイマー)
//---------------------------------------------------------------
void pause( int x )
{
  wait_timer = 0;
  while( wait_timer != x ); // 終了時間まで待つ
}

//-------------------------------------------------------------------------
//  キーオフ処理
//-------------------------------------------------------------------------
void WaitKeyOff( void )
{
  // チャタリング防止処理
  pause( KEY_OFF );             // 設定した時間([ms])待つ
  // 全てのスイッチがOFFになるまでループして待つ
  while((  PIN_READ( SW_UP ) == SW_ON )||( PIN_READ( SW_DOWN ) == SW_ON )||( PIN_READ( SW_RETURN ) == SW_ON ));
}


#ifdef __cplusplus
void abort(void)
{

}
#endif
