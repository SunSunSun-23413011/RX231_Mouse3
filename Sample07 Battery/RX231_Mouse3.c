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
#include "typedefine.h"
#include "Pin.h"
#include "iodefine.h"
#include "LCDrx231.h"
#include "Config_S12AD0.h"
#include <stdint.h>
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
// スイッチ関連
#define   SW_ON       0    // スイッチON (Active Low)
#define   SW_OFF      1    // スイッチOFF
#define   KEY_OFF   200    // スイッチ用チャタリングキャンセル時間
// モード関連
#define   ModeMax     3    // 動作モード数
#define   DISP        0    // モード表示
#define   EXEC        1    // モード実行

//---------------------------------------------------------------
//  グローバル変数定義
//---------------------------------------------------------------
vushort  wait_timer = 0;   // 内部時計( msec ) : wait関数用カウンタ
ushort   SENSOR_PT;        // 割り込み回数カウント用ポインタ
int      MODE = 0;         // 現在モード格納用
volatile uint16_t g_s12ad0_ch017_value;
vshort   Batt;             // 電池の電圧

//---------------------------------------------------------------
//  関数プロトタイプ宣言
//---------------------------------------------------------------
void IO_init( void );
void pause( int x );
void int_timerw( void );
void WaitKeyOff( void );
void change_mode( int x );
void exec_mode( void );
void mode0( int x );
void mode1( int x );
void mode2( int x );
//---------------------------------------------------------------
//  メインプログラム
//---------------------------------------------------------------
void main(void){
    int count = 0; // カウンター用

    IO_init();      // RX231初期化
    LCD_init();    // LCD初期化
    PIN_WRITE(CPU_LED) = 1;    // LED消灯
    // タイトル表示
    LCD_print( 0, "Sample07" );

    // 電圧表示
    LCD_print( 8, "   .  v " );
    LCD_dec_out( 9, Batt/100, 1);    // 十の位を表示
    Batt %= 100;                     // 十の位を削除
    LCD_dec_out(10, Batt/10 , 1);    // 一の位を表示
    Batt %= 10;                      // 一の位を削除
    LCD_dec_out(12, Batt    , 1);    // 残った小数値を表示
    pause( 2000 );
    
    change_mode( 0 );                // まず初期画面にする = Mode0
    
    // メインループ
    while( 1 ){
        if( PIN_READ( SW_UP ) == SW_ON ){          // 上SWが押されている場合
        WaitKeyOff();                // チャタリング防止処理
        change_mode(+1);             // モード+1
        }else if( PIN_READ( SW_DOWN ) == SW_ON ){  // 下SWが押されている場合
        WaitKeyOff();                // チャタリング防止処理
        change_mode(-1);             // モード-1
        }else if( PIN_READ( SW_RETURN ) == SW_ON ){  // 実行SWが押されている場合
        WaitKeyOff();                // チャタリング防止処理
        exec_mode();                 // モード実行
        MODE = 0;
        change_mode( 0 );            // 実行後は初期画面に戻す
    }
  }
}

//---------------------------------------------------------------
//  RX231初期化
//---------------------------------------------------------------
void IO_init( void ){
    R_Systeminit();
    PIN_WRITE(CPU_LED) = 0;    // LED点灯
    R_Config_CMT0_Start(); // CMT0開始

    R_Config_S12AD0_Start(); // ADC開始
    while (S12AD.ADCSR.BIT.ADST); // AD変換完了待ち
    R_Config_S12AD0_Get_ValueResult(ADCHANNEL17, (uint16_t *)&g_s12ad0_ch017_value);
    Batt = g_s12ad0_ch017_value / 8.19; // 電池電圧値取得
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
void pause( int x ){
  wait_timer = 0;
  while( wait_timer != x ); // 終了時間まで待つ
}

//-------------------------------------------------------------------------
//  キーオフ処理
//-------------------------------------------------------------------------
void WaitKeyOff( void ){
  // チャタリング防止処理
  pause( KEY_OFF );             // 設定した時間([ms])待つ
  // 全てのスイッチがOFFになるまでループして待つ
  while((  PIN_READ( SW_UP ) == SW_ON )||( PIN_READ( SW_DOWN ) == SW_ON )||( PIN_READ( SW_RETURN ) == SW_ON ));
}

//-------------------------------------------------------------------------
//  モード表示
//-------------------------------------------------------------------------
void change_mode( int x ){
  MODE += x;                            // モード更新
  if( MODE >= ModeMax ) MODE = 0;       // モードが超えている場合は0に戻す
  if( MODE < 0 )  MODE = ModeMax - 1;   // モードが負の場合はモードを最大値に設定

  if     ( MODE == 0 ) mode0( DISP );   // Mode0:
  else if( MODE == 1 ) mode1( DISP );   // Mode1:
  else if( MODE == 2 ) mode2( DISP );   // Mode2:
}

//-------------------------------------------------------------------------
//  モード処理
//-------------------------------------------------------------------------
void exec_mode( void ){
  if     ( MODE == 0 ) mode0( EXEC );   // Mode0:
  else if( MODE == 1 ) mode1( EXEC );   // Mode1:
  else if( MODE == 2 ) mode2( EXEC );   // Mode2:
}

//-------------------------------------------------------------------------
//  Mode0 : 
//-------------------------------------------------------------------------
void mode0( int x ){
  if( x == DISP )  // DISPモードの場合
  {
    // モード内容表示
    LCD_print( 0, "0: Mode0" );
    LCD_print( 8, "        " );
    return;                     // 以下の実行処理をしないで戻る
  }

  // 実行モードの場合
  LCD_print( 8, "OK!!!!!!" );
  pause( 1000 );
}

//-------------------------------------------------------------------------
//  Mode1 : 
//-------------------------------------------------------------------------
void mode1(int x){
  if( x == DISP )  // DISPモードの場合
  {
    // モード内容表示
    LCD_print( 0, "1: Mode1" );
    LCD_print( 8, "        " );
    return;                     // 以下の実行処理をしないで戻る
  }

  // 実行モードの場合
  LCD_print( 8, "OK!!!!!!" );
  pause( 1000 );
}

//-------------------------------------------------------------------------
//  Mode2 : 
//-------------------------------------------------------------------------
void mode2(int x){
  if( x == DISP )  // DISPモードの場合
  {
    // モード内容表示
    LCD_print( 0, "2: Mode2" );
    LCD_print( 8, "        " );
    return;                     // 以下の実行処理をしないで戻る
  }

  // 実行モードの場合
  LCD_print( 8, "OK!!!!!!" );
  pause( 1000 );
}

#ifdef __cplusplus
void abort(void){

}
#endif
