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
#include "Sound.h"
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
// センサ関連
#define   LED_ON      1    // センサ用LED点燈
#define   LED_OFF     0    // センサ用LED消灯

//---------------------------------------------------------------
//  グローバル変数定義
//---------------------------------------------------------------
vushort  wait_timer = 0;   // 内部時計( msec ) : wait関数用カウンタ
ushort   SENSOR_PT;        // 割り込み回数カウント用ポインタ
int      MODE = 0;         // 現在モード格納用
volatile uint16_t g_s12ad0_ch017_value;
volatile uint16_t g_s12ad0_ch000_value;
volatile uint16_t g_s12ad0_ch001_value;
volatile uint16_t g_s12ad0_ch002_value;
vshort   Batt;             // 電池の電圧
static int st_nSoundRPos;
static SOUND_T* st_pSound;
static SOUNDSTAT_T SoundStatus;
// センサの事前値格納用
int16_t  R_PRE;           // 右センサの値
int16_t  L_PRE;           // 左センサの値
int16_t  F_PRE;           // 前センサの値
// センサの現在値格納用
int16_t  R_SEN;           // 右センサの値
int16_t  L_SEN;           // 左センサの値
int16_t  F_SEN;           // 前センサの値
// センサのON/OFF用
short    R_SW;            // 右センサのスイッチ
short    L_SW;            // 左センサのスイッチ
short    F_SW;            // 前センサのスイッチ

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
void Int_MTU1_TGIA1(void); // MTU1割り込み関数プロトタイプ
//---------------------------------------------------------------
//  メインプログラム
//---------------------------------------------------------------
void main(void){
    int count = 0; // カウンター用

    IO_init();      // RX231初期化
    LCD_init();    // LCD初期化
    PIN_WRITE(CPU_LED) = 1;    // LED消灯
    PIN_WRITE(LED) = LED_OFF;                        // LEDを消灯
    Start_Sound(98); // 起動音再生
    // タイトル表示
    LCD_print( 0, "Sample09" );

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
      }else if( PIN_READ( SW_EXEC ) == SW_ON ){  // 実行SWが押されている場合
        WaitKeyOff();                // チャタリング防止処理
        exec_mode();                 // モード実行
        MODE = 0;
        change_mode( 0 );            // 実行後は初期画面に戻す
      }
      // モードが0ならセンサデータをLCD表示
      if( MODE == 0 ){
        LCD_dec_out(  3, F_SEN, 3 ); // 前センサ値をLCD上中央に表示
        LCD_dec_out(  9, L_SEN, 3 ); // 左センサ値をLCD左下に表示
        LCD_dec_out( 13, R_SEN, 3 ); // 右センサ値をLCD右下に表示
      }
    }
}

//---------------------------------------------------------------
//  RX231初期化
//---------------------------------------------------------------
void IO_init( void ){
    R_Systeminit();
    R_Pins_Create();
    PIN_WRITE(CPU_LED) = 0;    // LED点灯
    R_Config_CMT0_Start(); // CMT0開始

    R_Config_S12AD0_Start(); // ADC開始
    while (S12AD.ADCSR.BIT.ADST); // AD変換完了待ち
    R_Config_S12AD0_Get_ValueResult(ADCHANNEL17, (uint16_t *)&g_s12ad0_ch017_value);
    Batt = g_s12ad0_ch017_value / 8.19; // 電池電圧値取得
    R_SW = LED_ON;             // 右センサON
    L_SW = LED_ON;             // 左センサON
    F_SW = LED_ON;             // 前センサON
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
    switch( SENSOR_PT ){          // タスクポインタに従って処理を行う
      case 0:  // 1msecタイマー&LCDの更新
              wait_timer++;     // wait関数用カウンタ
              LCD();            // LCD更新処理
              break;
      case 1: break;
      case 2: break;
      case 3: R_Config_S12AD0_Start(); // ADC開始
               while (S12AD.ADCSR.BIT.ADST); // AD変換完了待ち
               if(R_SW == LED_ON){
                R_Config_S12AD0_Get_ValueResult(ADCHANNEL0, (uint16_t *)&g_s12ad0_ch000_value);
                R_PRE = g_s12ad0_ch000_value / 2; // 10bit化
                }
               if(L_SW == LED_ON){
                R_Config_S12AD0_Get_ValueResult(ADCHANNEL1, (uint16_t *)&g_s12ad0_ch001_value);
                L_PRE = g_s12ad0_ch001_value / 2; // 10bit化
                }
               if(F_SW == LED_ON){
                R_Config_S12AD0_Get_ValueResult(ADCHANNEL2, (uint16_t *)&g_s12ad0_ch002_value);
                F_PRE = g_s12ad0_ch002_value / 2; // 10bit化
                }
                LCD_wait(20);                         // しばらく待つ
                PIN_WRITE(LED) = LED_ON;                         // LEDを点灯
              R_Config_S12AD0_Start(); // ADC開始
               while (S12AD.ADCSR.BIT.ADST); // AD変換完了待ち
               if(R_SW == LED_ON){
                R_Config_S12AD0_Get_ValueResult(ADCHANNEL0, (uint16_t *)&g_s12ad0_ch000_value);
                R_PRE = (g_s12ad0_ch000_value / 2) - R_PRE; // 10bit化
                if( R_PRE < 0 )        R_SEN = 0;    // 表示上限処理
                else if( R_PRE <= 999 ) R_SEN = R_PRE;
                else                R_SEN = 999;
                }
               if(L_SW == LED_ON){
                R_Config_S12AD0_Get_ValueResult(ADCHANNEL1, (uint16_t *)&g_s12ad0_ch001_value);
                L_PRE = (g_s12ad0_ch001_value / 2) - L_PRE; // 10bit化
                if( L_PRE < 0 )        L_SEN = 0;    // 表示上限処理
                else if( L_PRE <= 999 ) L_SEN = L_PRE;
                else                L_SEN = 999;
                }
               if(F_SW == LED_ON){
                R_Config_S12AD0_Get_ValueResult(ADCHANNEL2, (uint16_t *)&g_s12ad0_ch002_value);
                F_PRE = (g_s12ad0_ch002_value / 2) - F_PRE; // 10bit化
                if( F_PRE < 0 )         F_SEN = 0;
                else if( F_PRE <= 9999 ) F_SEN = F_PRE;
                else                F_SEN = 9999;        // 表示上限処理
                }
                PIN_WRITE(LED) = LED_OFF;                        // LEDを消灯
               break;
      default: break;
    }
}

//---------------------------------------------------------------
//  MTU1 割り込み(41us毎にこの関数が勝手に優先して実行される)
//---------------------------------------------------------------
//config_MTU1_user.cから呼び出される
void Int_MTU1_TGIA1(void){
  if(SoundStatus.BIT.SOUND_SP == 1){
    SoundStatus.BIT.SOUND_SP = 0;
    SoundStatus.BIT.SOUND_IT = 0;
    st_nSoundRPos = 0;
  }
  if(st_pSound[st_nSoundRPos].NOTES == STP_){//終端マークで終了
    Stop_Sound();
  }else if (st_pSound[st_nSoundRPos].NOTES == RPT_){//繰り返しマークでSPACEフラグセット
    SoundStatus.BIT.SOUND_SP = 1;
    MTU0.TIORH.BIT.IOB = 0; // MTIOC0B端子を出力禁止にする（Hi-Zになる）
    MTU1.TGRA = T02_; // １秒の無音時間
  }else {
    if(SoundStatus.BIT.SOUND_IT == 0){//発音中の場合
      SoundStatus.BIT.SOUND_IT = 1;
      if(st_pSound[st_nSoundRPos].NOTES == RST_){//休符ならMTIOC0B端子の出力禁止
        MTU0.TIORH.BIT.IOB = 0; // MTIOC0B端子を出力禁止にする（Hi-Zになる）
      }else{//音符なら指定の音階を出力
        uint16_t note = (uint16_t)st_pSound[st_nSoundRPos].NOTES;
        /* TGRAを周期、TGRBを50%デューティに設定 */
        MTU0.TGRA = note;
        MTU0.TGRB = (uint16_t)(note / 2U);
        MTU0.TIORH.BIT.IOB = 3; // MTIOC0B端子をトグル出力にする
      }
      MTU1.TGRA = (uint16_t)(st_pSound[st_nSoundRPos].TIME - TIT_); // 音の時間（長さ）をセット
      st_nSoundRPos++;
    }else{//区切り
      SoundStatus.BIT.SOUND_IT = 0;
      MTU0.TIORH.BIT.IOB = 0; // MTIOC0B端子を出力禁止にする（Hi-Zになる）
      MTU1.TGRA = TIT_;
    }
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
  while((  PIN_READ( SW_UP ) == SW_ON )||( PIN_READ( SW_DOWN ) == SW_ON )||( PIN_READ( SW_EXEC ) == SW_ON ));
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
  Stop_Sound();
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
  Start_Sound(1);
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
  Start_Sound(2);
  LCD_print( 8, "OK!!!!!!" );
  pause( 1000 );
}

//-------------------------------------------------------------------------
//  時報音
//-------------------------------------------------------------------------
SOUND_T Chime_1[]={
{RA3_, T16_}, /* ラ (A7) */
{STP_, T00_},
};

//-------------------------------------------------------------------------
//  起動音
//-------------------------------------------------------------------------
SOUND_T Chime_2[]={
{RE2_, T16_}, /* レ (D6) */
{SO2_, T16_}, /* ソ (A6) */
{STP_, T00_},
};

//-------------------------------------------------------------------------
//  ビープ音 
//-------------------------------------------------------------------------
SOUND_T Melody_1[]={
{DO3_, T16_},
{RST_, T32_},
{DO3_, T16_},
{RST_, T32_},
{DO3_, T16_},
{RST_, T32_},
{DO3_, T16_},
{RST_, T32_},
{DO3_, T16_},
{RST_, T32_},
{DO3_, T16_},
{RST_, T32_},
{RPT_, T00_},
};
 
//-------------------------------------------------------------------------
//  ロンドン橋
//-------------------------------------------------------------------------
SOUND_T Melody_2[]={
{SO1_, T08_ + (T08_/2)},
{RA1_, T16_},
{SO1_, T08_},
{FA1_, T08_},
{MI1_, T08_},
{FA1_, T08_},
{SO1_, T04_},
{RE1_, T08_},
{MI1_, T08_},
{FA1_, T04_},
{MI1_, T08_},
{FA1_, T08_},
{SO1_, T04_},
{SO1_, T08_ + (T08_/2)},
{RA1_, T16_},
{SO1_, T08_},
{FA1_, T08_},
{MI1_, T08_},
{FA1_, T08_},
{SO1_, T04_},
{RE1_, T04_},
{SO1_, T04_},
{MI1_, T08_},
{DO1_, T04_ + (T04_/2)},

{SO1_, T08_ + (T08_/2)},
{RA1_, T16_},
{SO1_, T08_},
{FA1_, T08_},
{MI1_, T08_},
{FA1_, T08_},
{SO1_, T04_},
{RE1_, T08_},
{MI1_, T08_},
{FA1_, T04_},
{MI1_, T08_},
{FA1_, T08_},
{SO1_, T04_},
{SO1_, T08_ + (T08_/2)},
{RA1_, T16_},
{SO1_, T08_},
{FA1_, T08_},
{MI1_, T08_},
{FA1_, T08_},
{SO1_, T04_},
{RE1_, T04_},
{SO1_, T04_},
{MI1_, T08_},
{DO1_, T04_ + (T04_/2)},
{RPT_, T00_},
};

/******************************************************************************/
/*                            サウンド発音開始                                 */
/******************************************************************************/
/* 引数 : nMelodyNum = サウンド・メロディ番号（1,2,99） */
/* 戻値 : Start_Sound() == TRUE : サウンド発生開始 */
/* : == FALSE : サウンド発生なし */

BOOL Start_Sound(int nMelodyNum){
/* サウンド出力中なら停止する */
if(SoundStatus.BIT.SOUND_ON == 1){
  Stop_Sound();
  }
  if(nMelodyNum == 1){
  st_pSound = &Melody_1[0]; /* ビープ音 */
  }
  else if(nMelodyNum == 2){
  st_pSound = &Melody_2[0]; /* ロンドン橋 */
  }else if(nMelodyNum == 98){
  st_pSound = &Chime_2[0]; /* 起動音 */
  }else if(nMelodyNum == 99){
  st_pSound = &Chime_1[0]; /* 時報音 */
  }
  else{
  return FALSE;
  }
  st_nSoundRPos = 0;
  /* 最初から終端マークまたは繰り返しマークだったら何もしない */
  if((st_pSound[st_nSoundRPos].NOTES == RPT_) || (st_pSound[st_nSoundRPos].NOTES == STP_)){
  return FALSE;
  }
  /* サウンド出力中を示す */
  SoundStatus.BIT.SOUND_ON = 1;
  SoundStatus.BIT.SOUND_SP = 0;
  SoundStatus.BIT.SOUND_IT = 0;
  /* タイマリードライト許可レジスタ（MTU3,MTU4に必要） */
  // MTU.TRWER.BYTE = 0x01;
  /* 音の時間（長さ）をセット */
  MTU1.TCNT = 0x0000;
  MTU1.TGRA = T16_; /* 0.125秒後に発音開始 */
  /* MTU0,MTU1カウントスタート */
  R_Config_MTU0_Start();
  R_Config_MTU1_Start();
  /* MTU1コンペアマッチA割り込み許可 */
  IEN(MTU1, TGIA1) = 1;
 
 return TRUE;
 }
 
 /******************************************************************************/
/* サウンド停止 */
 
 /******************************************************************************/
 
void Stop_Sound(void)
{
 /* MTU1コンペアマッチA割り込み禁止 */
  IEN(MTU1, TGIA1) = 0;
  /* MTIOC0B端子を出力禁止にする（Hi-Zになる） */
  MTU0.TIORH.BIT.IOB = 0;
  /* MTU0,MTU1カウント停止 */
  R_Config_MTU0_Stop();
  R_Config_MTU1_Stop();
  /* 発音停止中を示す */
  SoundStatus.BIT.SOUND_ON = 0;
  SoundStatus.BIT.SOUND_SP = 0;
  }

#ifdef __cplusplus
void abort(void){

}
#endif
