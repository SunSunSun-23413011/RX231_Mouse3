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
//***************************************************************
//  ユニバーサルキット 3.0.1 メインプログラム RX231版
//                                          2016/09/26 H. Suzuki   H8版
//                                          2022/09/23 H. Ito      RX220移植
//                                          2026/02/03 T. Kawabata RX231移植
//
//  2026/01/25  Sample. 02 ：LED点滅
//  2026/01/25  Sample. 03 ：LCD表示
//  2026/01/25  Sample. 04 ：Timer W - Dのみ
//  2026/01/25  Sample. 05 ：SW 追加
//  2026/01/25  Sample. 06 ：Mode System 追加
//  2026/01/25  Sample. 07 ：A/D 追加
//  2026/01/28  Sample. 08 ：Sound 追加
//  2026/01/28  Sample. 09 ：Sensor 追加
//  2026/01/28  Sample. 10 ：Motor1(手動駆動) 追加
//  2026/01/28  Sample. 11 ：Motor2(割り込み駆動) 追加
//  2026/01/29  Sample. 12 ：Motor3(姿勢制御), MotorTest 追加
//  2026/01/30  Sample. 13 ：STEP, 1&N区間前進 追加
//  2026/01/31  Sample. 14 ：TURN 追加
//  2026/01/31  Sample. 15 ：迷路周回(探索関数) 追加
//  2026/01/31  Sample. 16 ：左手法 追加
//  2026/01/31  Sample. 17 ：座標更新 追加
//  2026/01/31  Sample. 18 ：マッピング 追加，探索法関数化
//  2026/02/02  Sample. 19 ：拡張左手法 追加
//  2026/02/02  Sample. 20 ：足立法 追加 A/D 修正
//  2026/02/02  Sample. 21 ：二次走行 追加
//  2026/02/04  Modified. 01 ：ゴール選択 追加
//  2026/02/06  Modified. 02 ：Sound 修正
//  2026/02/06  Modified. 03 ：モード分岐を関数ポインタのテーブル化
//  2026/02/06  Modified. 04 ：地図データ,ゴール座標のDF保存/読出し 追加
//  2026/02/09  Modified. 05 ：地図データ削除 追加
//  2026/02/09  Modified. 06 ：速度選択 追加
//  2026/02/12  Modified. 07 ：速度制限 追加
//  2026/02/12  Modified. 08 ：後ろ壁当て, 前壁補正 追加
//  2026/02/15  Modified. 09 ：スラローム探索 追加
//
//***************************************************************
#include "typedefine.h"
#include "Pin.h"
#include "iodefine.h"
#include "LCDrx231.h"
#include "Config_S12AD0.h"
#include <stdint.h>
#include "src/smc_gen/r_datfrx_rx/r_flash_dm_rx_if.h"
#include "Sound.h"
#include "Coconut_Mall_2.h"
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
typedef uint16_t u16;
typedef uint32_t u32;
typedef void (*mode_func_t)(int);
typedef void (*search_func_t)(int, int, int, int);

//-------------------------------------------------------------------------
//  マクロ定義
//-------------------------------------------------------------------------
// スイッチ関連
#define   SW_ON       0    // スイッチON (Active Low)
#define   SW_OFF      1    // スイッチOFF
#define   KEY_OFF   200    // スイッチ用チャタリングキャンセル時間
// モード関連
#define   DISP        0    // モード表示
#define   EXEC        1    // モード実行
#define   ModeMax     ( g_mode_table_count ) // 動作モード数
// センサ関連
#define   LED_ON      1    // センサ用LED点燈
#define   LED_OFF     0    // センサ用LED消灯bd
// モータ関連
#define   LeftGo      1    // 左モータ前進
#define   LeftBack    0    // 左モータ後進
#define   RightGo     0    // 右モータ前進
#define   RightBack   1    // 右モータ後進
#define ACC_TABLE_SIZE   (2000u)
#define ACC_TABLE_BASE   (300u)
#define ACC_TABLE_STEP   (4u)
#define ACC_TABLE_MIN    (ACC_TABLE_BASE)
#define ACC_TABLE_MAX    (ACC_TABLE_BASE + (ACC_TABLE_SIZE - 1u) * ACC_TABLE_STEP)
// 探索関連
#define   S_MODE      0    // Search Mode : 未探索区間は壁無しとして扱う
#define   T_MODE      1    // Try Mode    : 未探索区間は壁有りとして扱う
#define   SEARCH_MOUSE   0
#define   SEARCH_SLALOM  1
#define MAP_DATA_NO (0)
#define GOAL_DATA_NO (1)
#define MAP_DATA_SLOTS (1u)
#define MAP_DATA_BYTES (16u*16u)
#define GOAL_DATA_MAGIC (0xA0u)
#define GOAL_DATA_MAGIC_MASK (0xE0u)
#define GOAL_DATA_INDEX_MASK (0x1Fu)
#define FLASH_DM_WORK_WORDS (64u)

//---------------------------------------------------------------
//  グローバル変数定義
//---------------------------------------------------------------
vushort  wait_timer = 0;   // 内部時計( msec ) : wait関数用カウンタ
ushort   SENSOR_PT;        // 割り込み回数カウント用ポインタ
int      MODE = 0;         // 現在モード格納用
volatile uint16_t g_s12ad0_ch000_value;
volatile uint16_t g_s12ad0_ch001_value;
volatile uint16_t g_s12ad0_ch002_value;
volatile uint16_t g_s12ad0_ch017_value;
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
// センサのしきい値
short    R_REF;            // 右センサしきい値
short    L_REF;            // 左センサしきい値
short    F_REF;            // 前センサしきい値
// 壁の有無判定用しきい値
short    R_LIM;            // 右壁有無しきい値
short    L_LIM;            // 左壁有無しきい値
short    F_LIM;            // 前壁有無しきい値
short    F_LIM2;           // 2マス先前壁有無しきい値
short    F_LIM_SLA;        // スラローム用前壁有無しきい値
// モータ関連
ushort   timerL;           // 左タイマー設定値
ushort   timerR;           // 右タイマー設定値
short    ldir;             // 左モータ回転方向
short    rdir;             // 右モータ回転方向
short    speed;            // 目標速度
short    speed_now;        // 現在速度
short    MotorTimer;       // モータ電源コントロールタイマー
short    control_mode;     // 姿勢制御モード  0:なし  1:あり
short    Global_Speed;     // グローバル速度
short    SLALOM_INNER_SPEED;  // スラローム内輪速度
// 走行関連
short    STEP;             // モータのステップ数
short    GO_STEP;          // 1区間のステップ数
short    SLA_GO_STEP;      //スラローム時1区間ステップ数
short    HALF_STEP;        // 半区間のステップ数
short    TURN_STEP;        // 超信旋回ステップ数
short    SLALOM_STEP_FORWARD;   // スラローム旋回ステップ数（内側）
short    SLALOM_STEP_OUT;  // スラローム旋回ステップ数（外側）
short    zerozero;         // (0,0)スタートフラグ
//ステップ数(割り込み内でカウントアップ) 
volatile unsigned int step_r;		//右モータ用
volatile unsigned int step_l;			//左モータ用
short stepf_r = 1;
short stepf_l = 1;
// 探索関連
uchar    head;             // マウスの進行方向 0:北 1:東 2:南 3:西
uchar    head_change;      // 進行方向更新用変数 0:前 1:右 2:後 3:左
uchar    pos_x;            // マウスの現在座標 x
uchar    pos_y;            // マウスの現在座標 y
uchar    map[16][16];      // MAPデータ
uchar    p_map[16][16];    // ポテンシャルMAPデータ
int      goal[2] = {8,8}; // ゴール座標
int      goals[][2] = { {3,3}, {7,7}, {7,8}, {8,7}, {8,8} }; // ゴール座標リスト
int      GOAL_NUM = sizeof(goals) / (sizeof(goals[0])); // ゴール数
static	volatile	e_flash_dm_status_t	g_flash_dm_last	=	FLASH_DM_SUCCESS;
static	volatile	uint8_t	g_flash_dm_done	=	0;
static	uint8_t	g_flash_dm_ready	=	0;
static	uint32_t	g_flash_dm_work[FLASH_DM_WORK_WORDS];
//---------------------------------------------------------------
//  関数プロトタイプ宣言
//---------------------------------------------------------------
void IO_init( void );
void load_param( void );
void pause( int x );
void int_timerw( void );
void int_mot_r(void);
void int_mot_l(void); 
void WaitKeyOff( void );
void change_mode( int x );
void exec_mode( void );
void mode0( int x );
void mode1( int x );
void mode2( int x );
void mode3( int x );
void mode4( int x );
void mode5( int x );
void mode6( int x );
void mode7( int x );
void mode8( int x );
void mode9( int x );
void modeA( int x );
void mouse_search( int goal_x, int goal_y, int speed, int mode );
void slalom_search( int goal_x, int goal_y, int speed, int mode );
void Int_MTU1_TGIA1(void); // MTU1割り込み関数プロトタイプ
u16 AccTableGet(u16 index); // 加速テーブル取得関数プロトタイプ
void com_go( int n );
void com_go_half( int n );
void com_stop( void );
void com_back( int n );
void com_turn( int t_mode );
void com_slalom_turn( int t_mode );
void back_wall_set( void );
void kbat_lf_turn( void );
void goal_kbat_turn( void );
void countdown( void );
int get_wall_data( void );
void clear_map( void );
void make_map_data( void );
void make_potential( int goal_x, int goal_y, int mode );
int search_left_hand( void );
int search_ex_left_hand( void );
int search_adachi( void );
int map_writeDF( short no );
void map_DFread( short no );
int goal_writeDF( void );
int goal_DFread( int *gx, int *gy );
int goal_find_index( int gx, int gy );
void select_goal(int *gx, int *gy); // ゴール選択関数プロトタイプ
void select_speed(short *speed);      // 速度選択関数プロトタイプ
void select_search( short *select);   // 探索方法選択関数プロトタイプ

//---------------------------------------------------------------
//  メインプログラム
//---------------------------------------------------------------
void main(void){
    IO_init();      // RX231初期化
    LCD_init();    // LCD初期化
    PIN_WRITE(CPU_LED) = 1;    // LED消灯
    PIN_WRITE(LED) = LED_OFF;                        // LEDを消灯
    Start_Sound(98); // 起動音再生
    // タイトル表示
    LCD_print( 0, "Modded09" );

    // 電圧表示
    LCD_print( 8, "   .  v " );
    LCD_dec_out( 9, Batt/100, 1);    // 十の位を表示
    Batt %= 100;                     // 十の位を削除
    LCD_dec_out(10, Batt/10 , 1);    // 一の位を表示
    Batt %= 10;                      // 一の位を削除
    LCD_dec_out(12, Batt    , 1);    // 残った小数値を表示
    pause( 2000 );
    clear_map();                     // MAPデータ初期化
    load_param();                    // load parameters
    if(!goal_DFread(&goal[0], &goal[1])){ (void)goal_writeDF(); } // load goal from DF
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
        Start_Sound(99); // 実行音再生
        exec_mode();                 // モード実行
        MODE = 0;
        change_mode( 0 );            // 実行後は初期画面に戻す
      }
      // モードが0ならセンサデータをLCD表示
      if( MODE == 0 ){
        LCD_dec_out(  2, F_SEN, 4 ); // 前センサ値をLCD上中央に表示
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
    PIN_WRITE(MOTOR_EN) = 0; // モータOFF
    R_Config_CMT0_Start(); // CMT0開始

    R_Config_S12AD0_Start(); // ADC開始
    while (S12AD.ADCSR.BIT.ADST); // AD変換完了待ち
    R_Config_S12AD0_Get_ValueResult(ADCHANNEL17, (uint16_t *)&g_s12ad0_ch017_value);
    Batt = g_s12ad0_ch017_value / 8.4; // 電池電圧値取得
    R_SW = LED_ON;             // 右センサON
    L_SW = LED_ON;             // 左センサON
    F_SW = LED_ON;             // 前センサON
}

//---------------------------------------------------------------
//  パラメータ読み込み
//---------------------------------------------------------------
void load_param( void ){
  // センサしきい値の決め打ち
  R_REF   = 350;    // 区画中央での右センサ値
  L_REF   = 300;    // 区画中央での左センサ値
  F_REF   = 1600;   // 区画中央での前センサ値
  // 壁の有無判定用しきい値:各センサ壁あり最小値と壁なし値の中間値
  R_LIM   = 150;    // 右
  L_LIM   = 150;    // 左
  F_LIM   = 240;    // 前
  F_LIM2  = 200;    // 2マス先前
  F_LIM_SLA = 480;  // スラローム用前壁
  //走行パラメータ
  GO_STEP = 1610;   // 1区間のステップ数
  SLA_GO_STEP = 1580; //スラローム時1区間のステップ数
  HALF_STEP = 600; // 半区間のステップ数
  TURN_STEP = 520;  // 旋回ステップ数
  SLALOM_STEP_FORWARD = 1; // スラローム内側ステップ数
  SLALOM_STEP_OUT = 600; // スラローム外側ステップ数
  SLALOM_INNER_SPEED = 1; // スラローム内輪速度
  Global_Speed = 900; // グローバル速度
  zerozero = 0; // (0,0)スタートフラグ初期化
}

//---------------------------------------------------------------
//  Timer W 割り込み(200us毎にこの関数が勝手に優先して実行される)
//---------------------------------------------------------------
//Config_CMT0_user.cから呼び出される
void int_timerw( void ){
    int err_l, err_r;
    ushort acc_num, lspeed, rspeed;
    //左モーター割り込み
    MTU3.TGRC = timerL; //次の速度設定
    if(speed){
      R_Config_MTU3_Start(); // カウント開始
    }else{ //停止時
      R_Config_MTU3_Stop(); // カウント停止
    }
    if(ldir == 0){
      PIN_WRITE(L_MOT_MODE) = LeftGo; //正転
    }else{
      PIN_WRITE(L_MOT_MODE) = LeftBack; //反転
    }
    //右モーター割り込み
    MTU4.TGRC = timerR; //次の速度設定
    if(speed){
      R_Config_MTU4_Start(); // カウント開始
    }else{ //停止時
      R_Config_MTU4_Stop(); // カウント停止
    }
    if(rdir == 0){
      PIN_WRITE(R_MOT_MODE) = RightGo; //正転
    }else{
      PIN_WRITE(R_MOT_MODE) = RightBack; //反転
    }
    //モータスピード割り込み
    if((stepf_r == 1) || (stepf_l == 1)){
      stepf_l = 0;
      stepf_r = 0;
      //モータの加速処理
      if(speed == 0){ //モータ停止中の処理
        speed_now = 0; //速度を0にする
        timerL = 2500; //左 割り込み周期2ms 1/(0.8e-6x2500) = 500Hz
        MTU3.TGRC = timerL;
        timerR = 2500; //右
        MTU4.TGRC = timerR;
      }else{
        if(speed > speed_now) speed_now++; //加速
        else if(speed < speed_now) speed_now--; //減速
        if(speed_now >= 2000) speed_now = 1999; //最高速度
        if(speed_now < 0) speed_now = 0; //最低速度
        acc_num = AccTableGet((u16)speed_now); //加速度テーブルから値取得
        //姿勢制御
        if(control_mode == 1){
          //偏差を計算
          err_l = L_SEN - L_REF; //左偏差を計算
          err_r = R_SEN - R_REF; //右偏差を計算
          //壁情報から偏差を加工
          if(L_SEN > L_LIM || R_SEN > R_LIM){
            //どちらかに壁がある:偏差が大きい側を優先して補正
            if(err_l > err_r)
              err_r = -1 * err_l;
            else
              err_l = -1 * err_r;
          }else{
            //両方壁なし:補正なし
            err_l = 0;
            err_r = 0;
          }
          //偏差を用いて補正
          if (speed_now > 600){
            err_l *= 3;
            err_r *= 3;
          }
          lspeed = acc_num + err_l;
          rspeed = acc_num + err_r;
      }else if(control_mode == 2){ // 右旋回スラローム
          lspeed = acc_num;
          rspeed = SLALOM_INNER_SPEED;
      }else if(control_mode == 3){ // 左旋回スラローム
          lspeed = SLALOM_INNER_SPEED;
          rspeed = acc_num;
      }else{  // control_mode = 0
          lspeed = acc_num;
          rspeed = acc_num;
      }
      //タイマー設定値計算
      timerL =  1500000L / lspeed; // lspeedが大きい程周期が短くなる
      timerR =  1500000L / rspeed; // rspeedが大きい程周期が短くなる
      }
    }
    // センサ処理

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
      case 3: if ( (R_SW == LED_OFF) && (L_SW == LED_OFF) && (F_SW == LED_OFF) ) break;
               // センサ値取得処理
              R_Config_S12AD0_Start(); // ADC開始
               while (S12AD.ADCSR.BIT.ADST); // AD変換完了待ち
               if(R_SW == LED_ON){
                R_Config_S12AD0_Get_ValueResult(ADCHANNEL0, (uint16_t *)&g_s12ad0_ch000_value);
                R_PRE = g_s12ad0_ch000_value;
                }
               if(L_SW == LED_ON){
                R_Config_S12AD0_Get_ValueResult(ADCHANNEL1, (uint16_t *)&g_s12ad0_ch001_value);
                L_PRE = g_s12ad0_ch001_value;
                }
               if(F_SW == LED_ON){
                R_Config_S12AD0_Get_ValueResult(ADCHANNEL2, (uint16_t *)&g_s12ad0_ch002_value);
                F_PRE = g_s12ad0_ch002_value;
                }
                PIN_WRITE(LED) = LED_ON;                         // LEDを点灯
                LCD_wait(20);                         // しばらく待つ
              R_Config_S12AD0_Start(); // ADC開始
               while (S12AD.ADCSR.BIT.ADST); // AD変換完了待ち
               if(R_SW == LED_ON){
                R_Config_S12AD0_Get_ValueResult(ADCHANNEL0, (uint16_t *)&g_s12ad0_ch000_value);
                R_PRE = ((g_s12ad0_ch000_value) - R_PRE) / 2;     //11bit化
                if( R_PRE < 0 )        R_SEN = 0;    // 表示上限処理
                else if( R_PRE <= 999 ) R_SEN = R_PRE;
                else                R_SEN = 999;
                }
               if(L_SW == LED_ON){
                R_Config_S12AD0_Get_ValueResult(ADCHANNEL1, (uint16_t *)&g_s12ad0_ch001_value);
                L_PRE = ((g_s12ad0_ch001_value) - L_PRE) / 4;   //10bit化 ← 左センサ飽和したため。機体に合わせて要変更。
                if( L_PRE < 0 )        L_SEN = 0;    // 表示上限処理
                else if( L_PRE <= 999 ) L_SEN = L_PRE;
                else                L_SEN = 999;
                }
               if(F_SW == LED_ON){
                R_Config_S12AD0_Get_ValueResult(ADCHANNEL2, (uint16_t *)&g_s12ad0_ch002_value);
                F_PRE = ((g_s12ad0_ch002_value) - F_PRE) / 2;  //11bit化
                if( F_PRE < 0 )         F_SEN = 0;
                else if( F_PRE <= 9999 ) F_SEN = F_PRE;
                else                F_SEN = 9999;        // 表示上限処理
                }
                PIN_WRITE(LED) = LED_OFF;                        // LEDを消灯
               break;
      case 4:  // モータ用電源コントロール
               if( speed != 0 ) MotorTimer = 3000;   // モータ動作時はタイマーセット
               else             MotorTimer--;        // モータ停止時はカウントダウン
               if( MotorTimer < 0 )  MotorTimer =  0;
               // モータを動かさない時は電源をOFF(モータ停止から3秒後)
               if( MotorTimer == 0 )  PIN_WRITE(MOTOR_EN)   =  0;  // OFF
               else                   PIN_WRITE(MOTOR_EN)   =  1;  // ON
               break;
      default: break;
    }
}

//---------------------------------------------------------------
//  モータステップ数カウント関数
//---------------------------------------------------------------
void int_mot_r(void){	//右モータが１ステップ進む毎の割り込み
	step_r++;			//ステップ数をカウント
  stepf_r = 1;			// 
  STEP++;                     // 距離カウンタ更新 
}

void int_mot_l(void){	//左モータが１ステップ進む毎の割り込み
	step_l++;			//ステップ数をカウント
  stepf_l = 1;			//  
  STEP++;                    // 距離カウンタ更新 
}

//---------------------------------------------------------------
//  MTU1 割り込み(38us毎にこの関数が勝手に優先して実行される)
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
  while((  PIN_READ( SW_UP ) == SW_ON )||( PIN_READ( SW_DOWN ) == SW_ON )||( PIN_READ( SW_EXEC ) == SW_ON )||( PIN_READ( SW_RETURN ) == SW_ON ));
}

//-------------------------------------------------------------------------
//  モード表示
//-------------------------------------------------------------------------
//  モードテーブル
static const mode_func_t g_mode_table[] = {
  mode0, mode1, mode2, mode3, mode4, mode5, mode6, mode7, mode8, mode9, modeA
};
static const int g_mode_table_count = (int)(sizeof(g_mode_table) / sizeof(g_mode_table[0]));
static const search_func_t g_search_table[] = {
  mouse_search, slalom_search
};

void change_mode( int x ){
  MODE += x;                            // モード更新
  if( MODE >= ModeMax ) MODE = 0;       // モードが超えている場合は0に戻す
  if( MODE < 0 )  MODE = ModeMax - 1;   // モードが負の場合はモードを最大値に設定

  g_mode_table[ MODE ]( DISP );
}

//-------------------------------------------------------------------------
//  モード処理
//-------------------------------------------------------------------------
void exec_mode( void ){
  g_mode_table[ MODE ]( EXEC );
}

//-------------------------------------------------------------------------
//  Mode0 : センサチェック
//-------------------------------------------------------------------------
void mode0( int x ){
  if( x == DISP ){  // DISPモードの場合
    // モード内容表示
    LCD_print( 0, "0:Sensor" );
    LCD_print( 8, "        " );
    pause(1000);
    LCD_print( 0, " F      " );
    LCD_print( 8, "L   R   " );
    return;                     // 以下の実行処理をしないで戻る
  }

  // 実行モードの場合
  L_REF = 0; R_REF = 0;         // データ初期化

  for( x = 0; x < 32; x++ ){     // データ測定(64ポイント)
    L_REF += L_SEN;
    R_REF += R_SEN;
    pause(1);
  }

  R_REF = R_REF / 32;           // 測定データを平均化
  L_REF = L_REF / 32;

  LCD_print( 0, " L    R " );
  LCD_print( 8, "        " );
  LCD_dec_out(  8, L_REF, 3 );  // 左センサ値をLCDに表示
  LCD_dec_out( 13, R_REF, 3 );  // 右センサ値をLCDに表示

  pause( 2000 );                // 2秒間表示
}

//-------------------------------------------------------------------------
//  Mode1 : 
//-------------------------------------------------------------------------
void mode1(int x){
  if( x == DISP ){  // DISPモードの場合
    // モード内容表示
    LCD_print( 0, "1:M-TEST" );
    LCD_print( 8, "        " );
    return;                     // 以下の実行処理をしないで戻る
  }

  // 実行モードの場合
  LCD_print( 8,"SPD=     ");
  rdir = 0; ldir = 0;           // 回転方向を直進
  control_mode = 1;             // 直線走行用姿勢制御あり
  while(1){
    LCD_dec_out( 12, speed, 4 );
    if( PIN_READ(SW_UP)   == 0 ) { speed += 100; WaitKeyOff(); }
    if( PIN_READ(SW_DOWN) == 0 ) { speed -= 100; WaitKeyOff(); }
    if( PIN_READ(SW_EXEC) == 0 ) { speed = 0; break; }
    if( speed > 2000 )    speed = 2000;
    else if( speed < 0 )  speed = 0;
  }
  return;
}

//-------------------------------------------------------------------------
//  Mode2 : 
//-------------------------------------------------------------------------
void mode2(int x){
  if( x == DISP ){  // DISPモードの場合
    // モード内容表示
    LCD_print( 0, "2: 1 GO " );
    LCD_print( 8, "STEP    " );
    return;                     // 以下の実行処理をしないで戻る
  }

  // 実行モードの場合
  while(1){
    LCD_dec_out( 12, GO_STEP, 4 );
    if( PIN_READ(SW_UP)   == 0 ) { GO_STEP += 10; WaitKeyOff(); }
    if( PIN_READ(SW_DOWN) == 0 ) { GO_STEP -= 10; WaitKeyOff(); }
    if( PIN_READ(SW_EXEC) == 0 ) { WaitKeyOff();  com_go( 1 );  com_stop(); }
    if( PIN_READ(SW_RETURN) == 0 ) { break; }
  }
  return;
}

//-------------------------------------------------------------------------
//  Mode3 : N区間前進
//-------------------------------------------------------------------------
void mode3(int x){
  int n = 5;                    // 前進区間数：初期値 5
  if( x == DISP ){  // DISPモードの場合
    // モード内容表示
    LCD_print( 0, "3: N GO " );
    LCD_print( 8, "     N  " );
    return;                     // 以下の実行処理をしないで戻る
  }

  // 実行モードの場合
  LCD_dec_out( 8, GO_STEP, 4 );
  while(1){
    LCD_dec_out( 14, n, 2 );
    if( PIN_READ(SW_UP)   == 0 ) { n++; WaitKeyOff(); }
    if( PIN_READ(SW_DOWN) == 0 ) { n--; WaitKeyOff(); }
    if( PIN_READ(SW_EXEC) == 0 ) { WaitKeyOff();  com_go( n );  com_stop(); }
    if( PIN_READ(SW_RETURN) == 0 ) { break; }
  }
  return;
}

//-------------------------------------------------------------------------
//  Mode4 : 180ターンR
//-------------------------------------------------------------------------
void mode4( int x ){
  if( x == DISP )  // DISPモードの場合
  {
    // モード内容表示
    LCD_print( 0, "4: TURN " );
    LCD_print( 8, "        " );
    return;                     // 以下の実行処理をしないで戻る
  }

  // 実行モードの場合
  while(1){
    LCD_dec_out( 10, TURN_STEP, 4 );
    if( PIN_READ(SW_UP)   == 0 ) { TURN_STEP += 10; WaitKeyOff(); }
    if( PIN_READ(SW_DOWN) == 0 ) { TURN_STEP -= 10; WaitKeyOff(); }
    if( PIN_READ(SW_EXEC) == 0 ) { com_turn(2); com_stop(); }
    if( PIN_READ(SW_RETURN) == 0 ) { break; }
  }
  return;
}

//-------------------------------------------------------------------------
//  Mode5 : 探索走行
//-------------------------------------------------------------------------
void mode5( int x ){
  search_func_t search;
  short select;
  if( x == DISP ){  // DISPモードの場合
    // モード内容表示
    LCD_print( 0, "5:Search" );
    LCD_print( 8, "Spd     " );
    LCD_dec_out(12, Global_Speed, 4);
    return;                     // 以下の実行処理をしないで戻る
  }

  // 実行モードの場合
  select_search(&select);
  select_speed(&Global_Speed); // 速度選択
  countdown();               // カウントダウン
  pos_x = 0; pos_y = 0; head = 0; // 
  search = g_search_table[select];
  Start_Sound(3);
  search( goal[0], goal[1], Global_Speed, S_MODE );
  (void)map_writeDF(MAP_DATA_NO);
  search(0, 0, Global_Speed, S_MODE);
  (void)map_writeDF(MAP_DATA_NO);
  Start_Sound(96);
}

//-------------------------------------------------------------------------
//  Mode6 : 二次走行
//-------------------------------------------------------------------------
void mode6( int x ){
  search_func_t search;
  if( x == DISP ){  // DISPモードの場合
    // モード内容表示
    LCD_print( 0, "6:Try   " );
    LCD_print( 8, "Spd     " );
    LCD_dec_out(12, Global_Speed, 4);
    return;                     // 以下の実行処理をしないで戻る
  }

  // 実行モードの場合
  select_speed(&Global_Speed); // 速度選択
  // 二次走行
  countdown();           // カウントダウン
  pos_x = 0; pos_y = 0; head = 0;
  search = g_search_table[SEARCH_SLALOM];
  Start_Sound(3);
  map_DFread(MAP_DATA_NO);
  search( goal[0], goal[1], Global_Speed, T_MODE );
  search( 0, 0, Global_Speed, T_MODE );
  Start_Sound(96);
}


//-------------------------------------------------------------------------
//  Mode7 : Sound再生
//-------------------------------------------------------------------------
void mode7(int x){
  int sound_no = 1;          // 再生サウンド番号：初期値 1
  if( x == DISP ){  // DISPモードの場合
    // モード内容表示
    LCD_print( 0, "7: Sound" );
    LCD_print( 8, "No.     " );
    return;                     // 以下の実行処理をしないで戻る
  }

  // 実行モードの場合
  while(1){
    LCD_dec_out( 12, sound_no, 4 );
    if( PIN_READ(SW_UP)   == 0 ) {
      if(sound_no < 99) sound_no++;
      else if(sound_no == 99) sound_no = 1;
      WaitKeyOff(); 
    }
    if( PIN_READ(SW_DOWN) == 0 ) { 
      if(sound_no > 1) sound_no--;
      else if(sound_no == 1) sound_no = 99;
      WaitKeyOff(); 
    }
    if( PIN_READ(SW_EXEC) == 0 ) { Start_Sound(sound_no); WaitKeyOff(); }
    if( PIN_READ(SW_RETURN) == 0 ) { break; }
  }
  return;
}

//-------------------------------------------------------------------------
//  Mode8 : ゴール選択
//-------------------------------------------------------------------------
void mode8(int x){
  int old_goal_x;
  int old_goal_y;
  if( x == DISP ){  // DISPモードの場合
    // モード内容表示
    LCD_print( 0, "8: Goal " );
    LCD_print(8, "x:  y:  ");
    LCD_dec_out(10, goal[0], 1);
    LCD_dec_out(14, goal[1], 1);
    return;                     // 以下の実行処理をしないで戻る
  }

  // 実行モードの場合
  old_goal_x = goal[0];
  old_goal_y = goal[1];
  select_goal(&goal[0], &goal[1]);
  if( goal[0] != old_goal_x ){
    (void)goal_writeDF();
  }else if( goal[1] != old_goal_y ){
    (void)goal_writeDF();
  }
  return;
}

//-------------------------------------------------------------------------
//  Mode9 : MAP clear (RAM/DF)
//-------------------------------------------------------------------------
void mode9(int x){
  if( x == DISP ){  // DISP mode
    LCD_print( 0, "9:MapClr" );
    LCD_print( 8, "RAM+ROM " );
    return;
  }

  // EXEC mode
  LCD_print( 0, "Map Clr " );
  clear_map();  // clear RAM map
  if(map_writeDF(MAP_DATA_NO)){  // overwrite DF map with initial map
    LCD_print( 8, "Done    " );
    Start_Sound(99);
  }else{
    LCD_print( 8, "NG      " );
  }
  pause(800);
  return;
}

//-------------------------------------------------------------------------
//  ModeA : 自立走行
//-------------------------------------------------------------------------
void modeA( int x ){
  search_func_t search;
  short select;
  if( x == DISP ){  // DISPモードの場合
    // モード内容表示
    LCD_print( 0, "A:Search" );
    LCD_print( 8, "Spd     " );
    LCD_dec_out(12, Global_Speed, 4);
    return;                     // 以下の実行処理をしないで戻る
  }

  // 実行モードの場合
  select_search(&select);
  select_speed(&Global_Speed); // 速度選択
  countdown();               // カウントダウン
  pos_x = 0; pos_y = 0; head = 0; // 
  search = g_search_table[select];
  Start_Sound(3);
  search( goal[0], goal[1], 500, S_MODE );
  (void)map_writeDF(MAP_DATA_NO);
  search(0, 0, 500, S_MODE);
  (void)map_writeDF(MAP_DATA_NO);
  for(int i = 0; i < 4; i++){
    pos_x = 0; pos_y = 0; head = 0;
    search( goal[0], goal[1], Global_Speed, T_MODE );
    search( 0, 0, Global_Speed, T_MODE );
  }
  Start_Sound(96);
}


//-------------------------------------------------------------------------
//  探索関数
//-------------------------------------------------------------------------
void mouse_search( int goal_x, int goal_y, int spd, int mode ){
  short x, y, block_count, motion, next_motion;
  if( pos_x == 0 && pos_y == 0 ){
    back_wall_set();
    zerozero = 1;
  }
  while( 1 ){
    // １つのループは区間中心から次の区間中心まで
    // 最初に半区画直進
    control_mode = 1;             // 姿勢制御ON
    rdir = 0; ldir = 0;           // 回転方向を直進
    STEP = 0;                     // 距離カウンタリセット

    // 座標更新
    if     ( head == 0 ) pos_y++; // 北向き y+1
    else if( head == 1 ) pos_x++; // 東向き x+1
    else if( head == 2 ) pos_y--; // 南向き y-1
    else if( head == 3 ) pos_x--; // 西向き x-1

        // ポテンシャルMAP計算
    make_potential( goal_x, goal_y, mode );

    next_motion = search_adachi();  // 次の行動予測
    if ( (F_SEN > F_LIM2 || next_motion == 1 || next_motion == 3) && spd > 700 ) speed = 700; // 2マス先に壁がある場合は速度制限
    else  speed = spd;                  // 速度設定

    if( zerozero == 1 ){  // (0,0)スタート時のみ
      while( STEP < HALF_STEP );  // 半区間進む
      step_l = 0;                        //左ステップ数をリセット
      step_r = 0;                        //右ステップ数をリセット
      STEP = 0;                     // 距離カウンタリセット
      zerozero = 0;
    }
    while( STEP < GO_STEP / 2 );  // 半区間進む

    // 柱まで進んだら
    // 壁情報取得＆MAPデータ上書き
    if(mode == S_MODE) make_map_data();

    // 左手法で探索して行動決定
    motion = search_adachi();

    // ゴール時の例外処理（上で決めた行動が上書きされる）
    if( pos_x == goal_x && pos_y == goal_y )
      motion = 4;                       // ゴール到達：反転停止

    // 行動を実行
    switch( motion ){
      // 直進
      case  0 : while( STEP < GO_STEP );  // 残り半区間進む
                head_change = 0;          // 進行方向更新変数を前に設定
                break;
      // 右折
      case  1 : while( STEP < GO_STEP - speed_now * 2 && F_SEN < F_REF );  // 減速域を残して直進
                speed = 1;
                while( STEP < GO_STEP && F_SEN < F_REF );  // 残りステップ数で減速
                com_turn( 0 );            // 右90度旋回
                head_change = 1;          // 進行方向更新変数を右に設定
                break;
      // 反転
      case  2 : while( STEP < GO_STEP - speed_now * 2 && F_SEN < F_REF );  // 減速域を残して直進
                speed = 1;
                while( STEP < GO_STEP && F_SEN < F_REF );  // 残りステップ数で減速
                kbat_lf_turn();            // 反転
                head_change = 2;          // 進行方向更新変数を後に設定
                break;
      // 左折
      case  3 : while( STEP < GO_STEP - speed_now * 2 && F_SEN < F_REF );  // 減速域を残して直進
                speed = 1;
                while( STEP < GO_STEP && F_SEN < F_REF );  // 残りステップ数で減速
                com_turn( 1 );            // 左90度旋回
                head_change = 3;          // 進行方向更新変数を左に設定
                break;
      // 反転停止
      case  4 : while( STEP < GO_STEP - speed_now * 2 && F_SEN < F_REF );  // 減速域を残して直進
                speed = 1;
                while( STEP < GO_STEP && F_SEN < F_REF );  // 残りステップ数で減速
                goal_kbat_turn();         // 反転(ゴール壁当て)
                com_stop();               // 停止
                head_change = 2;          // 進行方向更新変数を後に設定
                head = ( head + head_change ) & 0x03; // 詳細は下を参照
                // MAPデータ確認（既探索の区画数を表示してみる）
                block_count = 0;
                for( y = 0 ; y < 16 ; y++ ){
                  for( x = 0 ; x < 16 ; x++ ){
                    if( ( map[ x ][ y ] & 0xf0 ) == 0xf0 )
                      block_count++;
                  }
                }
                // LCD_print( 0, "        " );
                // LCD_print( 8, "  Blocks" );
                // LCD_dec_out(  2, block_count, 3 );  // 既探索数を表示
                // pause( 3000 );

                return;                   // ループ終了
                break;
      // その他
      default : com_stop();               // 停止
                head_change = 0;          // 進行方向更新変数を前に設定
                head = ( head + head_change ) & 0x03; // 詳細は下を参照
                return;                   // ループ終了
                break;
    }
    
    // 進行方向更新変数head_changeを用いて現在の進行方向headを更新
    head = ( head + head_change ) & 0x03; // 更新数値を加算して2進数下2桁でマスク
                                          // 00 -> 01 -> 10 -> 11 -(マスク)-> 00
  }
}

//-------------------------------------------------------------------------
//  スラローム探索関数
//-------------------------------------------------------------------------
void slalom_search( int goal_x, int goal_y, int spd, int mode ){
  short x, y, block_count, motion, next_motion, next_next_motion, prev_motion;
  uchar save_pos_x, save_pos_y, save_head;
  prev_motion = 0;
  if( pos_x == 0 && pos_y == 0 ){
    back_wall_set();
    zerozero = 1;
  }
  while( 1 ){
    control_mode = 1;             // 姿勢制御ON
    rdir = 0; ldir = 0;           // 回転方向を直進
    STEP = 0;                     // 距離カウンタリセット

    // 座標更新
    if     ( head == 0 ) pos_y++;
    else if( head == 1 ) pos_x++;
    else if( head == 2 ) pos_y--;
    else if( head == 3 ) pos_x--;

    // ポテンシャルMAP計算
    make_potential( goal_x, goal_y, mode );
    next_motion = search_adachi();  // 次の行動予測
    next_next_motion = 2;           // default: not straight
    if( mode == T_MODE && next_motion == 0 ){
      // Predict the motion after next by virtual one-step forward
      save_pos_x = pos_x;
      save_pos_y = pos_y;
      save_head = head;

      if     ( head == 0 ) pos_y++;
      else if( head == 1 ) pos_x++;
      else if( head == 2 ) pos_y--;
      else if( head == 3 ) pos_x--;

      next_next_motion = search_adachi();

      pos_x = save_pos_x;
      pos_y = save_pos_y;
      head  = save_head;
    }

    if( mode == T_MODE && next_motion == 0 && speed < 600 ) speed = 600; // speed setting
    else if( mode == T_MODE && next_motion == 0 && next_next_motion == 0 && speed < 700 ) speed = 700; // speed setting
    else if( mode == T_MODE && next_motion == 0 && next_next_motion == 0 ) speed = spd; // speed setting
    else if( mode == T_MODE && next_motion == 0 && speed > 600 ) speed = 600; // speed setting
    else speed = 500;
    if( zerozero == 1 ){
      while( STEP < HALF_STEP );
      step_l = 0;
      step_r = 0;
      STEP = 0;
      zerozero = 0;
    }

    // 旋回前の進入距離をスラローム向けに調整
    if( prev_motion == 1 || prev_motion == 3 ){
      while( STEP < GO_STEP * 0.45 && F_SEN < F_LIM_SLA );
    }else{
      while( STEP < GO_STEP / 2 && F_SEN < F_LIM_SLA );
    }

    if( mode == S_MODE ) make_map_data();

    motion = search_adachi();
    if( pos_x == goal_x && pos_y == goal_y )
      motion = 4;

    switch( motion ){
      case  0 :
        while( STEP < SLA_GO_STEP );
        head_change = 0;
        break;
      case  1 :
        com_slalom_turn( 0 );
        head_change = 1;
        break;
      case  2 :
        while( STEP < GO_STEP - speed_now * speed_now / 300 && F_SEN < F_REF );
        speed = 1;
        while( STEP < GO_STEP && F_SEN < F_REF );
        kbat_lf_turn();
        head_change = 2;
        break;
      case  3 :
        com_slalom_turn( 1 );
        head_change = 3;
        break;
      case  4 :
        while( STEP < GO_STEP - speed_now * speed_now / 300 && F_SEN < F_REF );
        speed = 1;
        while( STEP < GO_STEP && F_SEN < F_REF );
        goal_kbat_turn();
        com_stop();
        head_change = 2;
        head = ( head + head_change ) & 0x03;
        block_count = 0;
        for( y = 0 ; y < 16 ; y++ ){
          for( x = 0 ; x < 16 ; x++ ){
            if( ( map[ x ][ y ] & 0xf0 ) == 0xf0 )
              block_count++;
          }
        }
        return;
      default :
        com_stop();
        head_change = 0;
        head = ( head + head_change ) & 0x03;
        return;
    }

    prev_motion = motion;
    head = ( head + head_change ) & 0x03;
  }
}


//-------------------------------------------------------------------------
//  直進モジュール (N区間前進)
//-------------------------------------------------------------------------
void com_go( int n ){
  control_mode = 1;                       // 直線走行用姿勢制御
  step_l = 0; step_r = 0;               // モータステップ数カウンタクリア
  STEP = 0;                               // 距離カウンタクリア
  rdir = 0; ldir = 0;                     // 回転方向を直進

  // 加速モード
  speed = 200;        // 目標速度設定
  while( speed > speed_now );                   // 目標速度になるまで加速
  // 定速モード
  speed = speed_now;  // 加速後の速度
  while( STEP < GO_STEP * n - speed_now * 2 );  // 減速ステップ数を残して定速移動
                                                // 全体ステップ数-減速用ステップ数
  // 減速モード
  speed = 1;          // 最低速度設定
  while( STEP < GO_STEP * n );                  // 残りのステップ数で減速
}

//-------------------------------------------------------------------------
//  直進モジュール (四半区間前進)
//-------------------------------------------------------------------------
void com_go_half( int n ){
  control_mode = 0;
  STEP = 0;
  rdir = 0; ldir = 0;
  speed = 100;
  while( speed > speed_now );
  speed = speed_now;
  while( STEP < HALF_STEP * n - speed_now * 2 );
  speed = 1;
  while( STEP < HALF_STEP * n );
}

//-------------------------------------------------------------------------
//  停止モジュール
//-------------------------------------------------------------------------
void com_stop( void ){
  control_mode = 0;           // 姿勢制御無し
  rdir = 0; ldir = 0;         // モータの回転方向を前進
  step_l = 0; step_r = 0; // モータステップ数カウンタをリセット
  STEP = 0;                   // 距離カウンタをリセット
  speed = 0;  speed_now = 0;  // モータの制御用の変数をリセット
  pause(100);                 // 0.1秒モータを停止
}

//-------------------------------------------------------------------------
//  後進モジュール
//-------------------------------------------------------------------------
void com_back( int n ){
  short BACK_STEP = GO_STEP / 2; // 後進ステップ数設定
  control_mode = 0;
  rdir = 1; ldir = 1;           // 回転方向を後進
  STEP = 0;                     // 距離カウンタリセット
  speed = 100;        // 目標速度設定
  while( speed > speed_now );                   // 目標速度になるまで加速
  speed = speed_now;  // 加速後の速度
  while( STEP < BACK_STEP * n - speed_now * 2 );  // 減速ステップ数を残して定速移動
                                                // 全体ステップ数-減速用ステップ数
  // 減速モード
  speed = 1;          // 最低速度設定
  while( STEP < BACK_STEP * n );                  // 残りのステップ数で減速
}

//-------------------------------------------------------------------------
//  旋回モジュール (0:R90 1:L90 2:R180 3:L180)
//-------------------------------------------------------------------------
void com_turn( int t_mode ){
  short T_STEP;

  com_stop();                                             // 停止
  control_mode = 0;                                       // 姿勢制御なし
  if     ( t_mode == 0 ) { T_STEP = TURN_STEP; rdir = 1; ldir = 0; } // 右９０度
  else if( t_mode == 1 ) { T_STEP = TURN_STEP; rdir = 0; ldir = 1; } // 左９０度
  else if( t_mode == 2 ) { T_STEP = TURN_STEP * 2; rdir = 1; ldir = 0; } // 右反転
  else if( t_mode == 3 ) { T_STEP = TURN_STEP * 2; rdir = 0; ldir = 1; } // 左反転

  // 加速モード
  speed = 100;        // 目標速度設定
  while( speed > speed_now );                   // 目標速度になるまで加速
  // 定速モード
  speed = speed_now;  // 加速後の速度
  while( STEP < T_STEP - speed_now * 2 );       // 減速ステップ数を残して定速移動
                                                // 全体ステップ数-減速用ステップ数
  // 減速モード
  speed = 1;          // 最低速度設定
  while( STEP < T_STEP );                       // 残りのステップ数で減速
}

//-------------------------------------------------------------------------
//  スラロームモジュール (0:R90 1:L90 2:R180 3:L180) 
//-------------------------------------------------------------------------
void com_slalom_turn( int t_mode ){
  control_mode = 0;                       // 姿勢制御無し
  step_r = 0;                               //右ステップ数をリセット
  step_l = 0;                               //左ステップ数をリセット
  STEP = 0;                               // 距離カウンタクリア
  rdir = 0; ldir = 0;                     // 回転方向を直進
  speed = 300;
  if( t_mode == 0 ) {
    while( speed > speed_now && F_SEN < F_LIM_SLA );
    speed = speed_now;
    while( (step_r < SLALOM_STEP_FORWARD) && (F_SEN < F_LIM_SLA) );
    if ( R_SEN > R_LIM){
      while( R_SEN > R_LIM );
      step_r = 0;                               //右ステップ数をリセット
      step_l = 0;                               //左ステップ数をリセット
      STEP = 0;                               // 距離カウンタクリア
      while( STEP < HALF_STEP && F_SEN < F_LIM_SLA );
    }
    control_mode = 2;           // スラローム用姿勢制御
    step_r = 0;                               //右ステップ数をリセット
    step_l = 0;                               //左ステップ数をリセット
    STEP = 0;                               // 距離カウンタクリア
    while( step_l < SLALOM_STEP_OUT );
  }
  else if( t_mode == 1 ) {
    while( speed > speed_now && F_SEN < F_LIM_SLA );
    speed = speed_now;
    while( (step_l < SLALOM_STEP_FORWARD) && (F_SEN < F_LIM_SLA) );
    if ( L_SEN > L_LIM){
      while( L_SEN > L_LIM );
      step_r = 0;                               //右ステップ数をリセット
      step_l = 0;                               //左ステップ数をリセット
      STEP = 0;                               // 距離カウンタクリア
      while( STEP < HALF_STEP && F_SEN < F_LIM_SLA );
    }
    control_mode = 3;           // スラローム用姿勢制御
    step_r = 0;                               //右ステップ数をリセット
    step_l = 0;                               //左ステップ数をリセット
    STEP = 0;                               // 距離カウンタクリア
    while( step_r < SLALOM_STEP_OUT );
  }
  //else if( t_mode == 2 ) { T_STEP *= 2; rdir = 1; ldir = 0; }
  //else if( t_mode == 3 ) { T_STEP *= 2; rdir = 0; ldir = 1; }
}

//-------------------------------------------------------------------------
//  後ろ壁当て
//-------------------------------------------------------------------------
void back_wall_set( void ){
  control_mode = 0;           // 姿勢制御無し
  com_stop();
  com_back( 1 );
  com_stop();
}

//-------------------------------------------------------------------------
//  ゴール壁当て
//-------------------------------------------------------------------------
void goal_kbat_turn( void ){
  if( F_SEN > F_LIM ){
    if( R_SEN > R_LIM + 100){
      com_stop();
      com_turn( 1 );
      com_stop();
      com_back( 1 );
      com_stop();
      com_go_half( 1 );
      com_stop();
      com_turn( 1 );
      com_stop();
      com_back( 1 );
      com_stop();
      zerozero = 1;
    }else if( L_SEN > L_LIM ){
      com_stop();
      com_turn( 0 );
      com_stop();
      com_back( 1 );
      com_stop();
      com_go_half( 1 );
      com_stop();
      com_turn( 0 );
      com_stop();
      com_back( 1 );
      com_stop();
      zerozero = 1;
    }else{
      com_turn( 3 );
      back_wall_set();
      zerozero = 1;
    }
  }else{
    if( R_SEN > R_LIM ){
      com_stop();
      com_turn( 1 );
      com_stop();
      com_back( 1 );
      com_stop();
      com_go_half( 1 );
      com_stop();
      com_turn( 1 );
    }else if( L_SEN > L_LIM ){
      com_stop();
      com_turn( 0 );
      com_stop();
      com_back( 1 );
      com_stop();
      com_go_half( 1 );
      com_stop();
      com_turn( 0 );
    }else{
      com_turn( 3 );
    }
  }
  com_stop();
}


//-------------------------------------------------------------------------
//  壁当てターン
//-------------------------------------------------------------------------
void kbat_lf_turn( void ){
  int kabe;
  kabe = search_left_hand();
  switch( kabe ){
    case 0:
    case 1:
      com_stop();
      com_turn( 0 );
      com_stop();
      com_back( 1 );
      com_stop();
      com_go_half( 1 );
      com_stop();
      com_turn( 0 );
      break;
    case 2:
      com_stop();
      com_turn( 0 );
      com_stop();
      com_back( 1 );
      com_stop();
      com_go_half( 1 );
      com_stop();
      com_turn( 0 );
      com_stop();
      com_back( 1 );
      com_stop();
      zerozero = 1;
      break;
    default :
      if( F_SEN > F_LIM ){
        com_turn( 2 );
        com_stop();
        com_back( 1 );
        com_stop();
        zerozero = 1;
      }else{
        com_turn( 2 );
      }
  }
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
//  カウントダウン
//-------------------------------------------------------------------------
SOUND_T Countdown[]={
{RA1_, T04_}, /* ラ (A5) */
{RST_, T08_}, /* 休符 */
{RA1_, T04_}, /* ラ (A5) */
{RST_, T08_}, /* 休符 */
{RA1_, T04_}, /* ラ (A5) */
{RST_, T08_}, /* 休符 */
{RA2_, T02_}, /* ラ (A6) */
{STP_, T00_},
};

//-------------------------------------------------------------------------
// ゴール
//-------------------------------------------------------------------------
SOUND_T Goal[]={
{SO2_, T32_}, /* ソ (G6) */
{RST_, T32_}, /* 休符 */
{SO2_, T32_}, /* ソ (G6) */
{RST_, TIT_}, /* 休符 */
{DO3_, T16_}, /* ド (C7) */
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

//-------------------------------------------------------------------------
//  サウンド発音開始
//-------------------------------------------------------------------------
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
  }else if(nMelodyNum == 3){
  st_pSound = &ccnt2[0]; /* ココナッツモール */
  }else if(nMelodyNum == 96){
  st_pSound = &Goal[0]; /* ゴール */
  }else if(nMelodyNum == 97){
  st_pSound = &Countdown[0]; /* カウントダウン */
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
 
//-------------------------------------------------------------------------
//  サウンド停止
//-------------------------------------------------------------------------
void Stop_Sound(void){
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

//-------------------------------------------------------------------------
//  加速テーブル取得
//-------------------------------------------------------------------------
u16 AccTableGet(u16 index){
    u32 v;

    if (index >= ACC_TABLE_SIZE) {
        index = (u16)(ACC_TABLE_SIZE - 1u);
    }

    v = (u32)ACC_TABLE_BASE + (u32)index * (u32)ACC_TABLE_STEP;
    return (u16)v; /* 300..8296なのでu16に収まる */
}

static	void	flash_dm_callback(void*event){
	g_flash_dm_last=(e_flash_dm_status_t)event;
	g_flash_dm_done=1;
}

static	e_flash_dm_status_t	flash_dm_wait(void){
	while(!g_flash_dm_done){}
	g_flash_dm_done=0;
	return	g_flash_dm_last;
}

static	int	flash_dm_format(void){
	e_flash_dm_status_t	st;
	g_flash_dm_done=0;
	st=R_FLASH_DM_Format();
	if(st==FLASH_DM_ACCEPT){
		st=flash_dm_wait();
	}
	if(st==FLASH_DM_FINISH_FORMAT){
		return	1;
	}
	return	0;
}

static	int	flash_dm_erase(void){
	e_flash_dm_status_t	st;
	g_flash_dm_done=0;
	st=R_FLASH_DM_Erase();
	if(st==FLASH_DM_NO_INVALID_BLOCK){
		return	1;
	}
	if(st==FLASH_DM_ACCEPT){
		st=flash_dm_wait();
	}
	if(st==FLASH_DM_FINISH_ERASE){
		return	1;
	}
	return	0;
}

static	int	flash_dm_reclaim(void){
	e_flash_dm_status_t	st;
#if(FLASH_TYPE == FLASH_TYPE_1)
	g_flash_dm_done=0;
	st=R_FLASH_DM_Reclaim();
	if(st==FLASH_DM_ERR_REQUEST_ERASE){
		if(!flash_dm_erase()){
			return	0;
		}
		g_flash_dm_done=0;
		st=R_FLASH_DM_Reclaim();
	}
	if(st==FLASH_DM_ACCEPT){
		st=flash_dm_wait();
	}
	if((st==FLASH_DM_FINISH_RECLAIM)||(st==FLASH_DM_SUCCESS)){
		return	1;
	}
	return	0;
#else
	(void)st;
	return	flash_dm_erase();
#endif
}

//-------------------------------------------------------------------------
//  データフラッシュ初期化 (周辺クロックをFCUに通知する関数)   1/31
//-------------------------------------------------------------------------
static	int	flash_dm_init_once(void){
	e_flash_dm_status_t	st;
	if(g_flash_dm_ready){
		return	1;
	}
	st=R_FLASH_DM_Open(g_flash_dm_work,flash_dm_callback);
	if(st!=FLASH_DM_SUCCESS){
		return	0;
	}
	for(;;){
		st=R_FLASH_DM_Init();
		if(st==FLASH_DM_SUCCESS){
			g_flash_dm_ready=1;
			return	1;
		}
		if(st==FLASH_DM_SUCCESS_REQUEST_ERASE){
			if(!flash_dm_erase()){
				return	0;
			}
			continue;
		}
		if(st==FLASH_DM_ERR_REQUEST_FORMAT){
			if(!flash_dm_format()){
				return	0;
			}
			continue;
		}
		if(st==FLASH_DM_ERR_BUSY){
			continue;
		}
		if(st==FLASH_DM_ERR_REQUEST_INIT){
			continue;
		}
		return	0;
	}
}

//-------------------------------------------------------------------------
//  データフラッシュメモリ1ブロック(128byte)書込み  
//  書込みブロック先頭:block 書込みデータ:data
//-------------------------------------------------------------------------
static	int	flash_dm_write(uint8_t	data_no,const	uint8_t*data){
	st_flash_dm_info_t	info;
	e_flash_dm_status_t	st;
	info.data_no=data_no;
	info.p_data=(uint8_t*)data;
	for(;;){
		g_flash_dm_done=0;
		st=R_FLASH_DM_Write(&info);
		if(st==FLASH_DM_ERR_REQUEST_RECLAIM){
			if(!flash_dm_reclaim()){
				return	0;
			}
			continue;
		}
		if(st==FLASH_DM_ERR_REQUEST_ERASE){
			if(!flash_dm_erase()){
				return	0;
			}
			continue;
		}
		if(st==FLASH_DM_ERR_REQUEST_INIT){
			if(!flash_dm_init_once()){
				return	0;
			}
			continue;
		}
		if(st==FLASH_DM_ERR_BUSY){
			continue;
		}
		if(st==FLASH_DM_ACCEPT){
			st=flash_dm_wait();
		}
		if((st==FLASH_DM_FINISH_WRITE)||(st==FLASH_DM_SUCCESS)){
			return	1;
		}
		return	0;
	}
}

//-------------------------------------------------------------------------
// MAPデータをDataFlashから読出し
// MAPデータの見方:map[ pos_x ][ pos_y ] 何個目のMAPか:no
//-------------------------------------------------------------------------
static	int	flash_dm_read(uint8_t	data_no,uint8_t*data){
	st_flash_dm_info_t	info;
	e_flash_dm_status_t	st;
	info.data_no=data_no;
	info.p_data=data;
	st=R_FLASH_DM_Read(&info);
	if(st==FLASH_DM_ERR_REQUEST_INIT){
		if(!flash_dm_init_once()){
			return	0;
		}
		st=R_FLASH_DM_Read(&info);
	}
	if(st==FLASH_DM_SUCCESS){
		return	1;
	}
	return	0;
}

//-------------------------------------------------------------------------
// MAPデータをDataFlashへ書込み   
// MAPデータの見方:map[ pos_x ][ pos_y ] 何個目のMAPにするか:no
//-------------------------------------------------------------------------
int	map_writeDF(short	no){
	uint8_t	buf[MAP_DATA_BYTES];
	uint8_t	x;
	uint8_t	y;
	uint16_t	idx;
	if((no<0)||(no>=(short)MAP_DATA_SLOTS)){
		return	0;
	}
	if(!flash_dm_init_once()){
		return	0;
	}
	for(y=0;y<16;y++){
		for(x=0;x<16;x++){
			idx=(uint16_t)y*16u+(uint16_t)x;
			buf[idx]=map[x][y];
		}
	}
	return	flash_dm_write((uint8_t)no,buf);
}

void	map_DFread(short	no){
	uint8_t	buf[MAP_DATA_BYTES];
	uint8_t	x;
	uint8_t	y;
	uint16_t	idx;
	if((no<0)||(no>=(short)MAP_DATA_SLOTS)){
		return;
	}
	if(!flash_dm_init_once()){
		return;
	}
	if(!flash_dm_read((uint8_t)no,buf)){
		return;
	}
	for(y=0;y<16;y++){
		for(x=0;x<16;x++){
			idx=(uint16_t)y*16u+(uint16_t)x;
			map[x][y]=buf[idx];
		}
	}
}

//-------------------------------------------------------------------------
//  壁のセンシング
//-------------------------------------------------------------------------
int goal_find_index( int gx, int gy ){
  int i;
  for(i=0;i<GOAL_NUM;i++){
    if(goals[i][0]==gx){
      if(goals[i][1]==gy){
        return i;
      }
    }
  }
  return -1;
}

int goal_writeDF( void ){
  uint8_t buf[1];
  int goal_idx;
  goal_idx = goal_find_index(goal[0], goal[1]);
  if(goal_idx < 0){
    return 0;
  }
  if(goal_idx > (int)GOAL_DATA_INDEX_MASK){
    return 0;
  }
  if(!flash_dm_init_once()){
    return 0;
  }
  buf[0] = (uint8_t)(GOAL_DATA_MAGIC + (uint8_t)goal_idx);
  return flash_dm_write((uint8_t)GOAL_DATA_NO, buf);
}

int goal_DFread( int *gx, int *gy ){
  uint8_t buf[1];
  int goal_idx;
  if(gx==0){
    return 0;
  }
  if(gy==0){
    return 0;
  }
  if(!flash_dm_init_once()){
    return 0;
  }
  if(!flash_dm_read((uint8_t)GOAL_DATA_NO, buf)){
    return 0;
  }
  if(((buf[0]/32u)*32u) != GOAL_DATA_MAGIC){
    return 0;
  }
  goal_idx = (int)(buf[0] - ((buf[0]/32u)*32u));
  if(goal_idx >= GOAL_NUM){
    return 0;
  }
  *gx = goals[goal_idx][0];
  *gy = goals[goal_idx][1];
  return 1;
}

int get_wall_data( void ){
  short wall;

  // センサデータを入力，閾値と比較して壁の有無を判定
  wall = 0;
  if( F_SEN > F_LIM )  wall |= 0x01; // 前壁あり
  if( R_SEN > R_LIM )  wall |= 0x02; // 右壁あり
  if( L_SEN > L_LIM )  wall |= 0x08; // 左壁あり
  // 後壁はあるわけないので見ない

  return( wall );
}

//-------------------------------------------------------------------------
// MAPデータの見方
// 探索記録 bit 7 6 5 4 = 西 南 東 北 / 値 = 1:既探索 0:未探索
// 壁情報   bit 3 2 1 0 = 西 南 東 北 / 値 = 1:壁有り 0:壁無し
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
//  MAPデータ初期化
//-------------------------------------------------------------------------
void clear_map( void ){
  int x, y;
  // 全ての区間を壁なし＆未探索に初期化
  for( y = 0 ; y < 16 ; y++ )
    for( x = 0 ; x < 16 ; x++ )
      map[ x ][ y ] = 0x00;

  // 西側の外壁（x=0，y=0～15）を上書き
  for( y = 0 ; y < 16 ; y++ )
    map[ 0 ][ y ] = 0x88;  // 西のみ既探索(8)＆西のみ壁あり(8)
  // 南側の外壁（x=0～15，y=0）を上書き
  for( x = 0 ; x < 16 ; x++ )
    map[ x ][ 0 ] = 0x44;  // 南のみ既探索(4)＆南のみ壁あり(4)
  // 東側の外壁（x=15，y=0～15）を上書き
  for( y = 0 ; y < 16 ; y++ )
    map[ 15 ][ y ] = 0x22; // 東のみ既探索(2)＆東のみ壁あり(2)
  // 北側の外壁（x=0～15，y=15）を上書き
  for( x = 0 ; x < 16 ; x++ )
    map[ x ][ 15 ] = 0x11; // 北のみ既探索(1)＆北のみ壁あり(1)

  // スタート区間（x=0，y=0）の上書き
  map[ 0 ][ 0 ] = 0xfe;  // 西南東北既探索(8+4+2+1=f)＆西南東壁あり(8+4+2=e)
  // スタート区間1つ右（x=1，y=0）の上書き
  map[ 1 ][ 0 ] = 0xcc;  // 西南既探索(8+4=c)＆西南壁あり(8+4=c)

  // 左上角（x=0，y=15）の上書き
  map[ 0 ][ 15 ] = 0x99; // 西北既探索(8+1=9)＆西北壁あり(8+1=9)
  // 右下角（x=15，y=0）の上書き
  map[ 15 ][ 0 ] = 0x66; // 南東既探索(4+2=6)＆南東壁あり(4+2=6)
  // 右上角（x=15，y=15）の上書き
  map[ 15 ][ 15 ] = 0x33;// 東北既探索(2+1=3)＆東北壁あり(2+1=3)
}

//-------------------------------------------------------------------------
//  センサ情報から探索記録＆壁情報をMAPデータに更新
//-------------------------------------------------------------------------
void make_map_data( void ){
  uchar wall;

  // 壁情報取得
  wall = get_wall_data();
  // 方向合わせ処理のために上位4bitに下位4bitの壁情報をコピー
  wall = ( wall & 0x0f ) | ( wall << 4 );
  // マウスの進行方向にあわせて壁データを加工
  if     ( head == 1 ) wall = wall >> 3; // 北が前の情報を東が前に加工
  else if( head == 2 ) wall = wall >> 2; // 北が前の情報を南が前に加工
  else if( head == 3 ) wall = wall >> 1; // 北が前の情報を西が前に加工
  // 西南東北を探索済みにする
  wall |= 0xf0;
  // 壁情報をMAPデータに上書き
  map[ pos_x ][ pos_y ] = wall;

  // 西南東北の隣区画のMAPデータを上書き

  // 現在区画の東壁情報を1つ右区画の西壁情報として上書きする処理
  // ここだけ詳細に説明．残り3つ（下，左，上）はまとめて記述
  if( pos_x != 15 ){  // 一番東側の区画の時以外
    // 右区画の西側情報（探索記録＆壁情報）を消去
    map[ pos_x + 1 ][ pos_y ] &= 0x77;
    // 右区画の西側探索記録を既探索とする
    map[ pos_x + 1 ][ pos_y ] |= 0x80;
    // 現在区画の東側情報を西側情報に変換して右区画の西側壁情報に上書き
    map[ pos_x + 1 ][ pos_y ] |= ( map[ pos_x ][ pos_y ] << 2 ) & 0x08;
  }
  
  // 現在区画の南壁情報を1つ下区画の北壁情報として上書きする処理
  if(pos_y!=0) map[pos_x][pos_y-1]=(map[pos_x][pos_y-1]&0xee)|0x10|((wall>>2)&0x01);
  // 現在区画の西壁情報を1つ左区画の東壁情報として上書きする処理
  if(pos_x!=0) map[pos_x-1][pos_y]=(map[pos_x-1][pos_y]&0xdd)|0x20|((wall>>2)&0x02);
  // 現在区画の北壁情報を1つ上区画の南壁情報として上書きする処理
  if(pos_y!=15)map[pos_x][pos_y+1]=(map[pos_x][pos_y+1]&0xbb)|0x40|((wall<<2)&0x04);
}

//-------------------------------------------------------------------------
//  等高線（ポテンシャル場）作成
//-------------------------------------------------------------------------
void make_potential( int gx, int gy, int mode )
{
  uchar check_num, flg;
  uchar x,y;

  // ポテンシャルMAP初期化(全て最大値255にする)
  for( y = 0 ; y < 16 ; y++ )
    for( x = 0 ; x < 16 ; x++ )
      p_map[ x ][ y ] = 255;

  // ゴール座標にポテンシャル0を書き込む
  p_map[ gx ][ gy ] = 0;

  check_num = 0;
  do{
    flg = 0;  // 変更フラグ初期化
    for( y = 0 ; y < 16 ; y++ ){
      for( x = 0 ; x < 16 ; x++ ){
        if( p_map[ x ][ y ] == check_num ){  // 今回対象区画とするポテンシャル
          if( mode == S_MODE ){

            // 探索走行(Search Mode)
            // 北側の壁がない場合：北側のポテンシャルを対象区画のポテンシャルより+1
            if((( map[ x ][ y ] & 0x01 ) == 0 ) && ( y != 15 )){
              if( p_map[ x ][ y + 1 ] == 255 ){// まだポテンシャルを書いてなければ
                p_map[ x ][ y + 1 ] = check_num + 1;
                flg = 1;  // 変更したのでフラグON
              }
            }
            // 東側の壁も同様に処理
            if((( map[ x ][ y ] & 0x02 ) == 0 ) && ( x != 15 ))
              if(p_map[x+1][y]==255){p_map[x+1][y]=check_num+1;flg=1;}
            // 南側の壁も同様に処理
            if((( map[ x ][ y ] & 0x04 ) == 0 ) && ( y != 0 ))
              if(p_map[x][y-1]==255){p_map[x][y-1]=check_num+1;flg=1;}
            // 西側の壁も同様に処理
            if((( map[ x ][ y ] & 0x08 ) == 0 ) && ( x != 0 ))
              if(p_map[x-1][y]==255){p_map[x-1][y]=check_num+1;flg=1;}

          }else{

           // 二次走行(Try Mode)
           // 北側が壁なし＆既探索の場合(壁なしでも未探索はポテンシャル255のまま)
            // 北側のポテンシャルを対象区画のポテンシャルより+1
            if((( map[ x ][ y ] & 0x11 ) == 0x10 ) && ( y != 15 ) && (( map[ x ][ y + 1 ] & 0xf0 ) == 0xf0 )){
              if( p_map[ x ][ y + 1 ] == 255 ){// まだポテンシャルを書いてなければ
                p_map[ x ][ y + 1 ] = check_num + 1;
                flg = 1;  // 変更したのでフラグON
              }
            }
            // 東側の壁も同様に処理
            if((( map[ x ][ y ] & 0x22 ) == 0x20 ) && ( x != 15 ) && (( map[ x + 1 ][ y ] & 0xf0 ) == 0xf0 ))
              if(p_map[x+1][y]==255){p_map[x+1][y]=check_num+1;flg=1;}
            // 南側の壁も同様に処理
            if((( map[ x ][ y ] & 0x44 ) == 0x40 ) && ( y != 0 ) && (( map[ x ][ y - 1 ] & 0xf0 ) == 0xf0 ))
              if(p_map[x][y-1]==255){p_map[x][y-1]=check_num+1;flg=1;}
            // 西側の壁も同様に処理
            if((( map[ x ][ y ] & 0x88 ) == 0x80 ) && ( x != 0 ) && (( map[ x - 1 ][ y ] & 0xf0 ) == 0xf0 ))
              if(p_map[x-1][y]==255){p_map[x-1][y]=check_num+1;flg=1;}

          }
        }
      }
    }
    check_num++;      // 次のループのために対象ポテンシャルを+1
  }while( flg != 0 ); // 今回のループで変更箇所が無ければ作成完了
}

//-------------------------------------------------------------------------
//  探索：左手法
//-------------------------------------------------------------------------
int search_left_hand( void ){
  short wall_data, motion;

  wall_data = get_wall_data();  // 壁情報取得

  // 壁情報を用いて次の行動を決定（左手法）
    // motion = 0:直進 / 1:右折 / 2:反転 / 3:左折 / 4:反転停止
    switch( wall_data ){
      case  0x00  : motion = 3; break;  // 左に壁なし:左折
      case  0x01  : motion = 3; break;  // 左に壁なし:左折
      case  0x02  : motion = 3; break;  // 左に壁なし:左折
      case  0x03  : motion = 3; break;  // 左に壁なし:左折
      case  0x04  : motion = 3; break;  // 左に壁なし:左折
      case  0x05  : motion = 3; break;  // 左に壁なし:左折
      case  0x06  : motion = 3; break;  // 左に壁なし:左折
      case  0x07  : motion = 3; break;  // 左に壁なし:左折
      case  0x08  : motion = 0; break;  // 左に壁,前に壁なし:直進
      case  0x09  : motion = 1; break;  // 左に壁,前に壁,右に壁なし:右折
      case  0x0a  : motion = 0; break;  // 左に壁,前に壁なし:直進
      case  0x0b  : motion = 2; break;  // 左に壁,前に壁,右に壁:反転
      default     : motion = 4; break;  // 後ろに壁:あり得ないので停止
  }
  return( motion );
}

//-------------------------------------------------------------------------
//  探索：拡張左手法
//-------------------------------------------------------------------------
int search_ex_left_hand( void ){
  short wall_data, motion, val, min_val;

  wall_data = map[ pos_x ][ pos_y ];  // 現在区画の壁情報取得

  min_val = 8;  // 計算される優先度の最大値+1を初期値に設定

  // 北方向の優先度の計算
  if(( wall_data & 0x01 ) == 0 ){     // 北方向に壁が無いとき
    // 1.方向による優先度の計算
    // マウスから見たこの壁の方向と優先度 左:0, 前:1，右:2，後:3
    // head=0（マウスの頭が北）の場合は優先度1（北は前）
    // head=1（マウスの頭が東）の場合は優先度0（北は左）
    // head=2（マウスの頭が南）の場合は優先度3（北は後）
    // head=3（マウスの頭が西）の場合は優先度2（北は右）
    val = (( 3 - head ) + 2 ) & 0x03;

    // 2.未探索／既探索による優先度の計算
    // 未探索:0，既探索:+4
    if(( map[ pos_x ][ pos_y + 1 ] & 0xf0 ) == 0xf0 ) val += 4;

    // 3.この壁が優先かどうかを判断
    // ここまでの計算で以下の優先度のどれかになる
    // 0:北方向がマウスの左側で未探索
    // 1:北方向がマウスの前側で未探索
    // 2:北方向がマウスの右側で未探索
    // 3:北方向がマウスの後側で未探索（※有り得ない）
    // 4:北方向がマウスの左側で既探索
    // 5:北方向がマウスの前側で既探索
    // 6:北方向がマウスの右側で既探索
    // 7:北方向がマウスの後側で既探索

    if( val < min_val ){
      min_val = val;  // 最小値の更新
      motion = 0;     // 移動すべき方向を北に設定
    }
  }

  // 東方向の優先度の計算
  if(( wall_data & 0x02 ) == 0 ){     // 東方向に壁が無いとき
    val = (( 3 - head ) + 3 ) & 0x03;
    if(( map[ pos_x + 1 ][ pos_y ] & 0xf0 ) == 0xf0 ) val += 4;
    if( val < min_val ){
      min_val = val;  // 最小値の更新
      motion = 1;     // 移動すべき方向を東に設定
    }
  }

  // 南方向の優先度の計算
  if(( wall_data & 0x04 ) == 0 ){     // 南方向に壁が無いとき
    val = (( 3 - head ) + 0 ) & 0x03;
    if(( map[ pos_x ][ pos_y - 1 ] & 0xf0 ) == 0xf0 ) val += 4;
    if( val < min_val ){
      min_val = val;  // 最小値の更新
      motion = 2;     // 移動すべき方向を南に設定
    }
  }

  // 西方向の優先度の計算
  if(( wall_data & 0x08 ) == 0 ){     // 西方向に壁が無いとき
    val = (( 3 - head ) + 1 ) & 0x03;
    if(( map[ pos_x - 1 ][ pos_y ] & 0xf0 ) == 0xf0 ) val += 4;
    if( val < min_val ){
      min_val = val;  // 最小値の更新
      motion = 3;     // 移動すべき方向を西に設定
    }
  }

  // 移動すべき方向から行動を決定
  // (目標方向-現在方向)を2進数下2桁でマスク
  // 引き算の結果が負の場合は2の補数でマスクされる
  // 11 -> 10 -> 01 -> 00 -> (-1)=11 -> (-2)=10 ...
  motion = ( motion - head ) & 0x03;

  return( motion );
}

//-------------------------------------------------------------------------
//  探索：足立法
//-------------------------------------------------------------------------
int search_adachi( void ){
  uchar wall_data, motion;
  short val, min_val;

  // 現在区画の壁情報取得
  wall_data = map[ pos_x ][ pos_y ];

  // 計算される優先度の最大値を初期値に設定
  min_val = 1025;  // 区画ポテンシャル最大値+1 255*4+4 +1 =1025

  // 周囲４つの方向に対して優先度を計算し，
  // 一番優先度が高い（値が小さい）区画に移動する．
  // 優先度はポテンシャル，未／既探索，直進方向の順．
  // 例：ポテンシャルが0の場合＝基本優先度は0*4+4=4
  // ※未探索なら-2，直進なら-1の減算方式
  // 4:既探索＆直進以外
  // 3:既探索＆直進
  // 2:未探索＆直進以外
  // 1:未探索＆直進
  // 優先度が同じ結果の場合は北東南西の順に優先される

  // 北方向の優先度の計算
  if(( wall_data & 0x01 ) == 0 ){     // 北方向に壁が無いとき
    // 1.ポテンシャルを元に基本優先度を計算
    val = p_map[ pos_x ][ pos_y + 1 ] * 4 + 4;
    // 2.方向による優先度の計算
    // 北方向が進行方向だった場合：-1(優先度を1上げる)
    if( head == 0 )  val -= 1;
    // 3.未探索／既探索による優先度の計算
    // 未探索:-2(優先度を2上げる)，既探索:0
    if(( map[ pos_x ][ pos_y + 1 ] & 0xf0 ) != 0xf0 )  val -= 2;
    // 最小値の更新
    if( val < min_val ){
      min_val = val;
      motion = 0;  // 移動すべき方向を北に設定
    }
  }

  // 東方向の優先度の計算
  if(( wall_data & 0x02 ) == 0 ){     // 東方向に壁が無いとき
    val = p_map[ pos_x + 1 ][ pos_y ] * 4 + 4;
    if( head == 1 )  val -= 1;
    if(( map[ pos_x + 1 ][ pos_y ] & 0xf0 ) != 0xf0 )  val -= 2;
    if( val < min_val ){
      min_val = val;
      motion = 1;  // 移動すべき方向を東に設定
    }
  }

  // 南方向の優先度の計算
  if(( wall_data & 0x04 ) == 0 ){     // 南方向に壁が無いとき
    val = p_map[ pos_x ][ pos_y - 1 ] * 4 + 4;
    if( head == 2 )  val -= 1;
    if(( map[ pos_x ][ pos_y - 1 ] & 0xf0 ) != 0xf0 )  val -= 2;
    if( val < min_val ){
      min_val = val;
      motion = 2;  // 移動すべき方向を南に設定
    }
  }

  // 西方向の優先度の計算
  if(( wall_data & 0x08 ) == 0 ){     // 西方向に壁が無いとき
    val = p_map[ pos_x - 1 ][ pos_y ] * 4 + 4;
    if( head == 3 )  val -= 1;
    if(( map[ pos_x - 1 ][ pos_y ] & 0xf0 ) != 0xf0 )  val -= 2;
    if( val < min_val ){
      min_val = val;
      motion = 3;  // 移動すべき方向を西に設定
    }
  }

  // 移動すべき方向から行動を決定
  motion = ( motion - head ) & 0x03;

  return( motion );
}

//-------------------------------------------------------------------------
//  カウントダウン
//-------------------------------------------------------------------------
void countdown( void ){  
  PIN_WRITE(LED) = LED_OFF;                       // LEDを消灯
  R_SW = LED_OFF;        // 右センサOFF
  L_SW = LED_OFF;        // 左センサOFF
  F_SW = LED_OFF;        // 前センサOFF
  Start_Sound(97);    // カウントダウン音開始
  while( SoundStatus.BIT.SOUND_ON); // 音が鳴り終わるまで待つ
  PIN_WRITE(LED) = LED_ON;                        // LEDを点灯
  R_SW = LED_ON;         // 右センサON
  L_SW = LED_ON;         // 左センサON
  F_SW = LED_ON;         // 前センサON
}

//-------------------------------------------------------------------------
//  速度選択
//-------------------------------------------------------------------------
void select_speed(short *speed){
  short set_speed = *speed;
  LCD_print(8, "V:      ");
  while (1) {
    LCD_dec_out(10, set_speed, 4);
    if ( PIN_READ(SW_UP) == SW_ON ) { // UPボタン
      set_speed += 100;
      if ( set_speed > ACC_TABLE_SIZE ) set_speed = 0;
      WaitKeyOff(); // チャタリング防止
    }
    if ( PIN_READ(SW_DOWN) == SW_ON ) { // DOWNボタン
      set_speed -= 100;
      if ( set_speed < 0 ) set_speed = ACC_TABLE_SIZE;
      WaitKeyOff(); // チャタリング防止
    }
    if ( PIN_READ(SW_EXEC) == SW_ON ) { // 決定ボタン
      WaitKeyOff(); // チャタリング防止
      Start_Sound(99); // 決定音開始
      *speed = set_speed;
      break;
    }
    if ( PIN_READ(SW_RETURN) == SW_ON ) { // 戻るボタン
      break;
    }
  }
  return;
}

//-------------------------------------------------------------------------
//  スラローム選択
//-------------------------------------------------------------------------
void select_search(short *select){
  short set_select = *select;
  LCD_print(8, "        ");
  while (1) {
    if ( PIN_READ(SW_UP) == SW_ON ) { // UPボタン
      set_select = SEARCH_MOUSE;
      LCD_print(8, " NORMAL ");
      WaitKeyOff(); // チャタリング防止
    }
    if ( PIN_READ(SW_DOWN) == SW_ON ) { // DOWNボタン
      set_select = SEARCH_SLALOM;
      LCD_print(8, " SLALOM ");
      WaitKeyOff(); // チャタリング防止
    }
    if ( PIN_READ(SW_EXEC) == SW_ON ) { // 決定ボタン
      WaitKeyOff(); // チャタリング防止
      Start_Sound(99); // 決定音開始
      *select = set_select;
      break;
    }
    if ( PIN_READ(SW_RETURN) == SW_ON ) { // 戻るボタン
      break;
    }
  }
  return;
}

//-------------------------------------------------------------------------
//  ゴール選択
//-------------------------------------------------------------------------
void select_goal(int *gx, int *gy){
  // ゴール座標設定
  int goal_num = goal_find_index(*gx, *gy);
  if( goal_num < 0 ) goal_num = 0;
  LCD_print(8, "x:  y:  ");
  while (1) {
    LCD_dec_out(10, goals[goal_num][0], 1);
    LCD_dec_out(14, goals[goal_num][1], 1);
    if ( PIN_READ(SW_UP) == SW_ON ) { // UPボタン
      goal_num++;
      if ( goal_num >= GOAL_NUM ) goal_num = 0;
      WaitKeyOff(); // チャタリング防止
    }
    if ( PIN_READ(SW_DOWN) == SW_ON ) { // DOWNボタン
      goal_num--;
      if ( goal_num < 0 ) goal_num = GOAL_NUM - 1;
      WaitKeyOff(); // チャタリング防止
    }
    if ( PIN_READ(SW_EXEC) == SW_ON ) { // 決定ボタン
      WaitKeyOff(); // チャタリング防止
      *gx = goals[goal_num][0];
      *gy = goals[goal_num][1];
      Start_Sound(99); // 決定音開始
      break;
    }
    if ( PIN_READ(SW_RETURN) == SW_ON ) { // 戻るボタン
      break;
    }
  }
  return;
}

#ifdef __cplusplus
void abort(void){

}
#endif
