/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: kizuato_jingle.c,v 1.29 2001/08/05 10:58:09 go Exp $
 *
 */

/*
 * Leaf 新ジングル (痕/ToHeart共通)
 */

#include <stdio.h>
#include "kizuato.h"

#ifndef USE_MGL
#define SLIDE_H 192
#else
#define SLIDE_H 96
#endif
static int
Slide(Lvns *lvns, LvnsScript *scr)
{
    int i;

    for (i=0;i<6;i++) {
        int y = scr->state * 6 - i * 5;
        if (y >= 0) {
            lvnsimage_clear_area(lvns->vram, 0, y, WIDTH, 1);
            lvnsimage_clear_area(lvns->vram, 0, HEIGHT-1-y, WIDTH, 1);
        }
    }
    lvns->drawWindow(lvns);
    lvns->flushWindow(lvns);

    if (++scr->state == ((HEIGHT - SLIDE_H)/2/6 + 1)) {
        return True;                  /* 処理終了 */
    } else {
        return False;
    }
}
#undef SLIDE_H

/* ジングル用パレットデータ */
static u_char pal[][3] = {
    { 0xff, 0xff, 0xff },
    { 0xff, 0xff, 0xff },
    { 0xff, 0xff, 0xff },
    { 0xff, 0xff, 0xff },
    { 0xff, 0xff, 0xff },
    { 0xff, 0xff, 0xff },
    { 0xff, 0xff, 0xff },
    { 0xff, 0xff, 0xff },
    { 0xff, 0xff, 0xee },
    { 0xff, 0xff, 0xdd },
    { 0xff, 0xff, 0xcc },
    { 0xff, 0xff, 0xbb },
    { 0xff, 0xff, 0xaa },
    { 0xff, 0xff, 0x99 },
    { 0xee, 0xff, 0xaa },
    { 0xdd, 0xff, 0xbb },
    { 0xcc, 0xff, 0xcc },
    { 0xbb, 0xff, 0xdd },
    { 0xaa, 0xff, 0xee },
    { 0xbb, 0xee, 0xee },
    { 0xcc, 0xdd, 0xee },
    { 0xdd, 0xcc, 0xee },
    { 0xee, 0xbb, 0xee },
    { 0xff, 0xaa, 0xee },
    { 0xff, 0xaa, 0xdd },
    { 0xff, 0xaa, 0xcc },
    { 0xff, 0xaa, 0xbb },
    { 0xff, 0xaa, 0xaa },
    { 0xff, 0xaa, 0x99 },
    { 0xee, 0xaa, 0xaa },
    { 0xdd, 0xaa, 0xbb },
    { 0xcc, 0xaa, 0xcc },
    { 0xbb, 0xaa, 0xdd },
    { 0xaa, 0xaa, 0xee },
    { 0xaa, 0xaa, 0xee },
    { 0xaa, 0xaa, 0xee },
    { 0xaa, 0xaa, 0xee },
    { 0xaa, 0xaa, 0xee },
    { 0xaa, 0xaa, 0xee },
    { 0xaa, 0xaa, 0xee },
    { 0xaa, 0xaa, 0xee },
    { 0xaa, 0xaa, 0xee },
    { 0xaa, 0xaa, 0xee },
};

static int
ClearWhite(Lvns *lvns, LvnsScript *scr)
{
#ifndef USE_MGL
    lvnsimage_clear_color(lvns->vram, 255, 255, 255);
#else
    mglimage_clear_color(lvns->vram, COLOR_WHITE);
#endif
    return True;
}

static int
LoadTitle(Lvns *lvns, LvnsScript *scr)
{
    int i;
    LvnsImage *image;

    /* ロゴデータ準備 */
    image = LvnsLoadImage(lvns, "LEAF.LFG", NULL);
    /* 葉っぱ以外の部分を白に */
    for (i=5; i<16; i++) {
        image->palette[i][0] =
        image->palette[i][1] = 
        image->palette[i][2] = 255;
    }

    /* 画像準備 */
#ifndef USE_MGL
    image->yoffset += ((HEIGHT - 400) / 2);
    image->height  += ((HEIGHT - 400) / 2);
    lvnsimage_copy2(image, lvns->vram, 15);
#else
    image->yoffset += ((HEIGHT - 200) / 2);
    image->height  += ((HEIGHT - 200) / 2);
    mglimage_add(image, lvns->vram, 0, 0);
#endif
    lvnsimage_delete(image);

    return True;
}

static int
Palette(Lvns *lvns, LvnsScript *scr)
{
    /* パレット制御しながら葉っぱを表示 */
    int i;
    int cnt = scr->state;

    for (i=0; i<9; i++) {
        lvns->vram->palette[5+i][0] = pal[cnt+i][0];
        lvns->vram->palette[5+i][1] = pal[cnt+i][1];
        lvns->vram->palette[5+i][2] = pal[cnt+i][2];
    }

    if (cnt == 30) {
        lvns->vram->palette[14][0] = 0xdd;
        lvns->vram->palette[14][1] = 0xdd;
        lvns->vram->palette[14][2] = 0xff;
    }

    lvns->setPalette(lvns);
    lvns->drawWindow(lvns);
    lvns->flushWindow(lvns);
    scr->state++;

	LvnsWait(lvns, 3);
    
    if (scr->state > 34) {
        scr->state = 0;
        return True;
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
    { LVNS_SCRIPT_TIMER_WAIT, (void*)1000 },
    { LVNS_SCRIPT_LOOP_FUNC, Palette },
    { LVNS_SCRIPT_TIMER_WAIT, (void*)8000 },
    { LVNS_SCRIPT_LOOP_FUNC, Slide },
    { LVNS_SCRIPT_TIMER_WAIT, (void*)12000 },
    { LVNS_SCRIPT_CLICK_JUMP },
    { LVNS_SCRIPT_CLEAR, (void*)LVNS_EFFECT_FADE_PALETTE },
    { LVNS_SCRIPT_END }
};

void
KizuatoJingle(Lvns *lvns)
{
	LvnsScriptRun(lvns, jingledata);
}

