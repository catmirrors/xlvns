/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: sizuku.h,v 1.19 2001/08/05 10:58:10 go Exp $
 *
 */

#ifndef __sizuku_h
#define __sizuku_h

#include "Lvns.h"
#include "LvnsEffect.h"
#include "LvnsAnim.h"
#include "LvnsScript.h"

#define SIZUKU_SCN_NO  205  /* シナリオの個数 */
#define SIZUKU_FLAG_NO  14  /* フラグの個数   */

#define SIZUKU_COL_BLACK 0
#define SIZUKU_COL_WHITE 16
#define SIZUKU_COL_GRAY  17

/*
 * 専用パレット
 */
#define SIZUKU_PAL_DAY       0
#define SIZUKU_PAL_YUUGATA   1
#define SIZUKU_PAL_NIGHT     2
#define SIZUKU_PAL_MIDNIGHT2 3
#define SIZUKU_PAL_MIDNIGHT  4
#define SIZUKU_PAL_YUUYAKE   5
#define SIZUKU_PAL_DARK      6
#define SIZUKU_PAL_NIGHT2    7
#define SIZUKU_PAL_YUUGATA2  8

typedef struct {
    LvnsSystemState state;

    int seen_flag[SIZUKU_SCN_NO];    /* シナリオ既読フラグ   */
    int flag[SIZUKU_FLAG_NO];        /* シナリオ制御用フラグ */
    int flag_save[SIZUKU_FLAG_NO];   /* 保存用 */
    int flag_select[SIZUKU_FLAG_NO]; /* 選択肢に戻る保存用   */
    LvnsImage *vram_char;            /* キャラクタ合成作業用 */
    int siori_no;                    /* 現在のしおり番号     */

} SizukuState;

void SizukuScenarioInit(Lvns *lvns);
void SizukuSioriInit(Lvns *lvns);
void SizukuLoad(Lvns *lvns);
void SizukuSave(Lvns *lvns);

void SizukuStart(Lvns *lvns);
void SizukuJingle(Lvns *lvns);
void SizukuOpening(Lvns *lvns);
void SizukuTitle(Lvns *lvns);
void SizukuMain(Lvns *lvns);
void SizukuEnding(Lvns *lvns);

void SizukuMergeCharacter(Lvns *lvns);
void SizukuClearScreen(Lvns *lvns);

void SizukuLoadCharacter(Lvns *lvns, int no, int pos);
void SizukuClearCharacter(Lvns *lvns, int pos);
void SizukuLoadBG(Lvns *lvns, int no);
void SizukuLoadVisual(Lvns *lvns52, int no);
void SizukuLoadHVisual(Lvns *lvns, int no);

void SizukuSetTextScenarioState(Lvns *lvns, int no);

void SizukuPuts(Lvns *lvns, int x, int y, 
                const u_char *str, int idx, int shadow);

void SizukuPutsVRAM(Lvns *lvns, int x, int y, const u_char *str, int idx);

void SizukuDispHistory(Lvns *lvns, int pos);

#endif


