 /******************************************************************************/
 /***                              サウンド出力                               ***/
 /******************************************************************************/
#ifndef INCLUDED_SOUND_H
#define INCLUDED_SOUND_H

#include "typedefine.h"
#include <stdint.h>

#pragma bit_order right
/******************************************************************************/
typedef union {
    BYTE Byte;
    struct {
        BYTE SOUND_ON:1; /* b0:サウンド発音中 */
        BYTE SOUND_SP:1; /* b1:発音が一巡した後の空白時間実行中 */
        BYTE SOUND_IT:1; /* b2:１音発音した後の区切りの空白実行中 */
        BYTE B3:1; /* b3: */
        BYTE B4:1; /* b4: */
        BYTE B5:1; /* b5: */
        BYTE B6:1; /* b6: */
        BYTE B7:1; /* b7: */
    } BIT;
} SOUNDSTAT_T;

/* 注意 : PCLK=25MHz,MTUn.TCR=PCLK/4(=6.25MHz)であること。 */
/* カウント値, 音階 */
/* --- Octave 3 (C3?B3): m1 --- */
#define DOm1_ (19111) /* ド (C3) */
#define DOm1s (18039) /* (C#3) */
#define REm1_ (17026) /* レ (D3) */
#define REm1s (16071) /* (D#3) */
#define MIm1_ (15169) /* ミ (E3) */
#define FAm1_ (14317) /* ファ (F3) */
#define FAm1s (13514) /* (F#3) */
#define SOm1_ (12755) /* ソ (G3) */
#define SOm1s (12039) /* (G#3) */
#define RAm1_ (11364) /* ラ (A3) */
#define RAm1s (10726) /* (A#3) */
#define SIm1_ (10124) /* シ (B3) */

/* --- Octave 4 (C4?G#4): 0 --- */
#define DO0_  ( 9556) /* ド (C4) */
#define DO0s  ( 9019) /* (C#4) */
#define RE0_  ( 8513) /* レ (D4) */
#define RE0s  ( 8035) /* (D#4) */
#define MI0_  ( 7584) /* ミ (E4) */
#define FA0_  ( 7159) /* ファ (F4) */
#define FA0s  ( 6757) /* (F#4) */
#define SO0_  ( 6378) /* ソ (G4) */
#define SO0s  ( 6020) /* (G#4) */
#define RA0_  (5681) /* ラ (A4) */
#define RA0s  (5363) /* (A#4) */
#define SI0_  (5061) /* シ (B4) */
#define DO1_  (4777) /* ド (C5) */
#define DO1s  (4509) /* (C#5) */
#define RE1_  (4256) /* レ (D5) */
#define RE1s  (4017) /* (D#5) */
#define MI1_  (3792) /* ミ (E5) */
#define FA1_  (3579) /* ファ (F5) */
#define FA1s  (3378) /* (F#5) */
#define SO1_  (3188) /* ソ (G5) */
#define SO1s  (3009) /* (G#5) */
#define RA1_  (2840) /* ラ (A5) */
#define RA1s  (2681) /* (A#5) */
#define SI1_  (2531) /* シ (B5) */
#define DO2_  (2388) /* ド (C6) */
#define DO2s  (2255) /* (C#6) */
#define RE2_  (2128) /* レ (D6) */
#define RE2s  (2008) /* (D#6) */
#define MI2_  (1896) /* ミ (E6) */
#define FA2_  (1789) /* ファ (F6) */
#define FA2s  (1689) /* (F#6) */
#define SO2_  (1594) /* ソ (G6) */
#define SO2s  (1504) /* (G#6) */
#define RA2_  (1420) /* ラ (A6) */
#define RA2s  (1340) /* (A#6) */
#define SI2_  (1265) /* シ (B6) */
#define DO3_  (1194) /* ド (C7) */
#define DO3s  (1127) /* (C#7) */
#define RE3_  (1064) /* レ (D7) */
#define RE3s  (1004) /* (D#7) */
#define MI3_  ( 948) /* ミ (E7) */
#define FA3_  ( 895) /* ファ (F7) */
#define FA3s  ( 844) /* (F#7) */
#define SO3_  ( 797) /* ソ (G7) */
#define SO3s  ( 752) /* (G#7) */
#define RA3_  ( 710) /* ラ (A7) */
#define RA3s  ( 670) /* (A#7) */
#define SI3_  ( 632) /* シ (B7) */
#define DO4_  ( 597) /* ド (C8) */
#define DO4s  ( 564) /* (C#8) */
#define RE4_  ( 532) /* レ (D8) */
#define RE4s  ( 502) /* (D#8) */
#define MI4_  ( 474) /* ミ (E8) */
#define FA4_  ( 447) /* ファ (F8) */
#define FA4s  ( 422) /* (F#8) */
#define SO4_  ( 398) /* ソ (G8) */
#define SO4s  ( 376) /* (G#8) */
#define RA4_  ( 355) /* ラ (A8) */
#define RA4s  ( 335) /* (A#8) */
#define SI4_  ( 316) /* シ (B8) */
#define DO5_  ( 298) /* ド (C9) */
#define DO5s  ( 282) /* (C#9) */
#define RE5_  ( 266) /* レ (D9) */
#define RE5s  ( 251) /* (D#9) */
#define MI5_  ( 237) /* ミ (E9) */
#define FA5_  ( 224) /* ファ (F9) */
#define FA5s  ( 211) /* (F#9) */
#define SO5_  ( 199) /* ソ (G9) */
#define SO5s  ( 188) /* (G#9) */
#define RA5_  ( 177) /* ラ (A9) */
#define RA5s  ( 167) /* (A#9) */
#define SI5_  ( 158) /* シ (B9) */
#define DO6_  ( 149) /* ド (C10) */
#define DO6s  ( 141) /* (C#10) */
#define RE6_  ( 133) /* レ (D10) */
#define RE6s  ( 125) /* (D#10) */

#define RST_  (15) /* 休符を示す */
#define RPT_  (1)  /* 繰り返しマーク */
#define STP_  (0)  /* 終端マーク */

/* PCLK/1024(=24.4140KHz, 40.96uS)をカウントして生成する時間 */
/* 注意 : これは TEMPO=120 の時の値。（カッコ）内はその時間（秒） */
#define T00_  (156250UL) /* 全音符・休符(2秒) */
#define T02_  ( 78125UL) /* 2分音符・休符(1秒) */
#define T04_  ( 39062UL) /* 4分音符・休符(0.5秒) */
#define T08_  ( 19531UL) /* 8分音符・休符(0.25秒) */
#define T16_  (  9765UL) /* 16分音符・休符(0.125秒) */
#define T32_  (  4882UL) /* 32分音符・休符(0.0625秒) */
#define TIT_  (  2441UL) /* 音の区切り時間(0.03125秒) */
#define T128  (  1220UL) /* 128分音符・休符(0.015625秒) */
#define T256  (   610UL) /* 256分音符・休符(0.0078125秒) */
 
typedef struct {
    WORD NOTES;
    uint32_t TIME;
} SOUND_T;
 
/******************************************************************************/
 /* 関数プロトタイプ */
 
 void Init_Sound(void);
 BOOL Start_Sound(int nMelodyNum);
 void Stop_Sound(void);
 
 
#endif /* INCLUDED_SOUND_H */