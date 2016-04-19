/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: sizuku_jingle.c,v 1.26 2001/08/05 10:58:10 go Exp $
 *
 */

/* 
 * 雫 ジングル
 */

#include <stdio.h>
#include "sizuku.h"

static int
ClearWhite(Lvns *lvns)
{
#ifndef USE_MGL
    lvnsimage_clear_color(lvns->vram, 255, 255, 255);
#else
    mglimage_clear_color(lvns->vram, COLOR_WHITE);
#endif
    return True;
}

static int
LoadTitle(Lvns *lvns)
{
    LvnsImage *image = LvnsLoadImage(lvns, "LEAF.LFG", NULL);
#ifndef USE_MGL
    lvnsimage_set_palette(lvns->vram, image->palette, 16);
    lvnsimage_clear_index(lvns->vram, 15);
#endif
    lvnsimage_add2(image, lvns->vram, 80, 144);
    lvnsimage_delete(image);
    return True;
}

static int
LoadTitle2(Lvns *lvns, LvnsScript *scr)
{
    LvnsImage *image = LvnsLoadImage(lvns, "LEAF.LFG", NULL);
#ifndef USE_MGL
    lvnsimage_set_palette(lvns->vram, image->palette, 16);
    lvnsimage_clear_index(lvns->vram, 0);
    lvnsimage_boxfill(lvns->vram, 15, 0, 80, 639, 319);
    lvnsimage_add2(image, lvns->vram, 80, 144);
    lvnsimage_delete(image);
    lvns->setPalette(lvns);
#else
    lvnsimage_set_palette(lvns->vram, image->palette, 16);
    lvnsimage_clear(lvns->vram);
    lvnsimage_boxfill(lvns->vram, COLOR_WHITE, 0, 0, 319, 199);
    mglimage_add(image, lvns->vram, 40, 72);
    mglimage_delete(image);
#endif
    lvns->drawWindow(lvns);
    scr->state = 0;
    return True;
}

/*
 * 最後の左右からのスライド部分
 */
static int
Slide(Lvns *lvns, LvnsScript *scr)
{
#ifndef USE_MGL
    lvns->flushWindowArea(lvns, scr->state * 20, 0, 20, 200);
    lvns->flushWindowArea(lvns, 620 - scr->state * 20, 200, 20,200);
    if (++scr->state * 20 >= WIDTH) {
#else
    lvns->clearWindowArea(lvns, scr->state * 10, 0, 10, 40);
    lvns->clearWindowArea(lvns, 310 - scr->state * 10, 160, 10, 40);
    if (++scr->state * 10 >= WIDTH) {
#endif
        return True;                  /* 処理終了 */
    } else {
        return False;
    }
}

/* ジングルスクリプト */
static LvnsScriptData jingledata[] = {
    { LVNS_SCRIPT_FUNC, ClearWhite },
    { LVNS_SCRIPT_MUSIC, (void*)2 },
    { LVNS_SCRIPT_TIMER_INIT },
    { LVNS_SCRIPT_DISP_VRAM, (void*)LVNS_EFFECT_FADE_PALETTE },
    { LVNS_SCRIPT_FUNC, LoadTitle },
	{ LVNS_SCRIPT_WHITEIN },
    { LVNS_SCRIPT_WAIT, (void*)120 }, 
    { LVNS_SCRIPT_FUNC, LoadTitle2 },
    { LVNS_SCRIPT_LOOP_FUNC, Slide },
    { LVNS_SCRIPT_TIMER_WAIT, (void*)6000 },
    { LVNS_SCRIPT_CLICK_JUMP },
    { LVNS_SCRIPT_CLEAR, (void*)LVNS_EFFECT_FADE_PALETTE },
    { LVNS_SCRIPT_END }
};

/*
 * 雫 ジングル
 */
void
SizukuJingle(Lvns *lvns)
{
	LvnsScriptRun(lvns, jingledata);
}


