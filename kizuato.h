/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: kizuato.h,v 1.23 2001/08/05 10:58:09 go Exp $
 *
 */

#ifndef __kizuato_h
#define __kizuato_h

#include "Lvns.h"
#include "LvnsEffect.h"
#include "LvnsAnim.h"
#include "LvnsScript.h"

#define KIZUATO_SCN_NO 177   /* シナリオの個数 */
#define KIZUATO_FLAG_NO 28  /* フラグの個数   */

#define KIZUATO_COL_BLACK 0
#define KIZUATO_COL_WHITE 16
#define KIZUATO_COL_GRAY  17

#define KIZUATO_COLOR_REVERSE 100

typedef struct {
    LvnsSystemState state;

    int seen_flag[KIZUATO_SCN_NO];  /* シナリオ既読フラグ         */
    int ending[17];            /* エンディングをみたかどうか */
    int flag[KIZUATO_FLAG_NO];   /* シナリオ制御用フラグ */
    int flag_save[KIZUATO_FLAG_NO];   /* シナリオ制御用フラグ */
    int flag_select[KIZUATO_FLAG_NO];   /* シナリオ制御用フラグ */

    int change_palette;   /* 強制パレット置換モード */
    int reverse_palette;  /* 色反転 */
    int change_palette2;  /* 表示時にパレット置換開始… */

    LvnsImage *vram_char;  /* キャラクタ合成作業用    */
    int siori_no;          /* 現在のしおり番号        */

    /* OP用共通作業データ */
    LvnsImage *chizuru;
    LvnsImage *azusa;
    LvnsImage *kaede;
    LvnsImage *hatsune;
    LvnsImage *moon;
    LvnsImage *shadow;
    int moon_pos;

} KizuatoState;

void KizuatoScenarioInit(Lvns *lvns);
void KizuatoSioriInit(Lvns *lvns);
void KizuatoLoad(Lvns *lvns);
void KizuatoSave(Lvns *lvns);

void KizuatoStart(Lvns *lvns);
void KizuatoJingle(Lvns *lvns);
void KizuatoOpening(Lvns *lvns);
void KizuatoTitle(Lvns *lvns);
void KizuatoMain(Lvns *lvns);
void KizuatoEnding(Lvns *lvns);

void KizuatoMergeCharacter(Lvns *lvns);
void KizuatoClearScreen(Lvns *lvns);

void KizuatoLoadCharacter(Lvns *lvns, int no, int pos);
void KizuatoClearCharacter(Lvns *lvns, int pos);
void KizuatoLoadBG(Lvns *lvns, int no);
void KizuatoLoadVisual(Lvns *lvns, int no);
void KizuatoLoadHVisual(Lvns *lvns, int no);

void KizuatoDisp(Lvns *lvns, LvnsEffectType effect_disp);

void KizuatoLoadScenarioText(Lvns *lvns, int no);

void KizuatoPutsVRAM(Lvns *lvns, int x, int y, const u_char *str, int idx);

void KizuatoDispHistory(Lvns *lvns, int pos);

#endif

