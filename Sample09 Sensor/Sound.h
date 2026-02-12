 /******************************************************************************/
 /***                              サウンド出力                               ***/
 /******************************************************************************/
#ifndef INCLUDED_SOUND_H
#define INCLUDED_SOUND_H

#include "typedefine.h"

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
#define RA0_ (7102) /* ラ (A4) */
#define RA0s (6704) /* (A#4) */
#define SI0_ (6327) /* シ (B4) */
#define DO1_ (5972) /* ド (C5) */
#define DO1s (5637) /* (C#5) */
#define RE1_ (5321) /* レ (D5) */
#define RE1s (5022) /* (D#5) */
#define MI1_ (4740) /* ミ (E5) */
#define FA1_ (4474) /* ファ (F5) */
#define FA1s (4223) /* (F#5) */
#define SO1_ (3986) /* ソ (G5) */
#define SO1s (3762) /* (G#5) */
#define RA1_ (3551) /* ラ (A5) */
#define RA1s (3352) /* (A#5) */
#define SI1_ (3164) /* シ (B5) */
#define DO2_ (2986) /* ド (C6) */
#define DO2s (2819) /* (C#6) */
#define RE2_ (2660) /* レ (D6) */
#define RE2s (2511) /* (D#6) */
#define MI2_ (2370) /* ミ (E6) */
#define FA2_ (2237) /* ファ (F6) */
#define FA2s (2112) /* (F#6) */
#define SO2_ (1993) /* ソ (G6) */
#define SO2s (1881) /* (G#6) */
#define RA2_ (1776) /* ラ (A6) */
#define RA2s (1676) /* (A#6) */
#define SI2_ (1582) /* シ (B6) */
#define DO3_ (1493) /* ド (C7) */
#define DO3s (1409) /* (C#7) */
#define RE3_ (1330) /* レ (D7) */
#define RE3s (1256) /* (D#7) */
#define MI3_ (1185) /* ミ (E7) */
#define FA3_ (1119) /* ファ (F7) */
#define FA3s (1056) /* (F#7) */
#define SO3_ ( 997) /* ソ (G7) */
#define SO3s ( 941) /* (G#7) */
#define RA3_ ( 888) /* ラ (A7) */
#define RA3s ( 838) /* (A#7) */
#define SI3_ ( 791) /* シ (B7) */
#define DO4_ ( 747) /* ド (C8) */
#define DO4s ( 705) /* (C#8) */
#define RE4_ ( 665) /* レ (D8) */
#define RE4s ( 628) /* (D#8) */
#define MI4_ ( 593) /* ミ (E8) */
#define FA4_ ( 559) /* ファ (F8) */
#define FA4s ( 528) /* (F#8) */
#define SO4_ ( 498) /* ソ (G8) */
#define SO4s ( 470) /* (G#8) */
#define RA4_ ( 444) /* ラ (A8) */
#define RA4s ( 419) /* (A#8) */
#define SI4_ ( 395) /* シ (B8) */
#define DO5_ ( 373) /* ド (C9) */
#define RST_ (15) /* 休符を示す */
#define RPT_ (1) /* 繰り返しマーク */
#define STP_ (0) /* 終端マーク */

/* PCLK/1024(=24.4140KHz, 40.96uS)をカウントして生成する時間 */
/* 注意 : これは TEMPO=120 の時の値。（カッコ）内はその時間（秒） */
#define T00_ (48828 * 16 ) /* 全音符・休符（２秒） */
#define T02_ (24414 * 16 ) /* ２分音符・休符（１秒） */
#define T04_ (12207 * 16 ) /* ４分音符・休符（0.5秒） */
#define T08_ (6103 * 16 ) /* ８分音符・休符（0.25秒） */
#define T16_ (3052 * 16 ) /* １６分音符・休符（0.125秒） */
#define T32_ (1526 * 16 ) /* ３２分音符・休符（0.0625秒） */
#define TIT_ (763 * 16 ) /* 音の区切り時間（0.03125秒）*/
 
typedef struct {
    WORD NOTES;
    WORD TIME;
} SOUND_T;
 
/******************************************************************************/
 /* 関数プロトタイプ */
 
 void Init_Sound(void);
 BOOL Start_Sound(int nMelodyNum);
 void Stop_Sound(void);
 
 
#endif /* INCLUDED_SOUND_H */