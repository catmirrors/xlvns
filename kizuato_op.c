/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: kizuato_op.c,v 1.39 2001/08/05 10:58:09 go Exp $
 *
 */

/* 
 * 痕 オープニング/タイトル
 */

#include <stdio.h>
#include <stdlib.h>
#include "kizuato.h"

/* ------------------------------------------------------------ */
/** ゲーム初期化用メニュー */

static MenuLine from_first_menu_line[] = {
	MENULINE(5, "初めから始める", 1),
    MENULINE(6, "続きから始める", 2),
	MENULINE(0, NULL, 0),
};

static MenuData from_first_menu = {
	from_first_menu_line,
	2, 1,
	NULL,
};

/* ------------------------------------------------------------ */
/** しおり選択用メニュー */

static MenuLine siori_select_menu_line[] = {
	MENULINE(5, "しおり　１", 1),
	MENULINE(6, "しおり　２", 2),
	MENULINE(7, "しおり　３", 3),
	MENULINE(0, NULL, 0),
};

static int
SioriSelectMenuEngine(Lvns *lvns, int select)
{
    KizuatoState *state = (KizuatoState *)lvns->system_state;
	
	state->siori_no = select;
	KizuatoLoad(lvns);
	if (lvns->savepoint.scn) { /* シナリオの先頭か? */
		switch (LvnsMenu(lvns, &from_first_menu, True)) {
		case -1: /* cancel */
			return 0;
		case 1:
			KizuatoScenarioInit(lvns);
		case 2:			
			break;
		}
	}
	LvnsClear(lvns, LVNS_EFFECT_FADE_PALETTE);
	LvnsJump(lvns, LVNS_JUMP_RESTART);

	return select;
}

static MenuData siori_select_menu = {
	siori_select_menu_line,
	3, 1,
	SioriSelectMenuEngine
};

/* ------------------------------------------------------------ */
/** しおり初期化用メニュー */

static MenuLine siori_init_menu_line[] = {
	MENULINE(6, "初期化するしおりを選択して下さい",0),
	MENULINE(7, "しおり　１",1),
	MENULINE(8, "しおり　２",2),
    MENULINE(9, "しおり　３",3),
	MENULINE(0, NULL, 0),
};

static int
SioriInitMenuEngine(Lvns *lvns, int select)
{
    KizuatoState *state = (KizuatoState *)lvns->system_state;
	state->siori_no = select;
	KizuatoSioriInit(lvns);
	KizuatoSave(lvns);
    return select;
}

static MenuData siori_init_menu = {
	siori_init_menu_line,
	3, 1, 
	SioriInitMenuEngine
};

/* ------------------------------------------------------------ */

static int
LoadImage(Lvns *lvns, LvnsScript *scr, 
          void * param1, void * param2) 
{
    (void)LvnsLoadImage(lvns, param1, lvns->vram);
    return True;
}

static int
SetPalette(Lvns *lvns, LvnsScript *scr, 
          void * param1, void * param2) 
{
    lvns->setPalette(lvns);
    return True;
}

static int
AddImage(Lvns *lvns, LvnsScript *scr, 
         void * param1, void * param2) 
{
    LvnsImage *image = LvnsLoadImage(lvns, param1, NULL);
    if (image)
        lvnsimage_add(image, lvns->vram, 0, 0);
    lvnsimage_delete(image);
    return True;
}

/*
 * SDT 画像のロード
 */
#ifndef USE_MGL
static LvnsImage *
LoadSDT(Lvns *lvns, const char *name, int width, int height)
{
    int index;
    u_char *data;
    size_t size;
    LvnsImage *ret;

    if ((index  = leafpack_find(lvns->leafpack, name)) < 0) {
        fprintf(stderr, "can't find image:%s\n", name);
        return NULL;
    }

    if ((data = leafpack_extract(lvns->leafpack, index, NULL)) == NULL) {
        fprintf(stderr, "can't extract image:%s\n", name);
        return NULL;
    }

    ret = lvnsimage_new(width, height, 0, 0);

    size = data[0] | (data[1]<<8) | (data[2]<<16) | (data[3]<<24);
    leafpack_lzs2(data + 4, ret->data, size);

    /* 上下反転 */
    {
        int i, j;
        for (i=0; i<height/2; i++) {
            u_char *p = ret->data + i * width;
            u_char *q = ret->data + (height - i - 1) * width;
            for (j=0; j<width; j++, p++, q++) {
                u_char c;
                c = *p;
                *p = *q;
                *q = c;
            }
        }
    }

    free(data);
    return ret;
}
#else
static LvnsImage *
LoadSDT(Lvns *lvns, const char *name, int width, int height)
{
    int index;
    LvnsImage *ret;
    u_char *data;

    if ((index  = leafpack_find(lvns->leafpack, name)) < 0) {
        fprintf(stderr, "can't find image:%s\n", name);
        return NULL;
    }

    if ((data = leafpack_extract(lvns->leafpack, index, NULL)) == NULL) {
        fprintf(stderr, "can't extract image:%s\n", name);
        return NULL;
    }

    ret = mglimage_load_lfg(data, NULL);

    return ret;
}
#endif

#ifndef USE_MGL
/**
 * 画像の index を or 合成する
 * ※「痕」の OP で使用
 * はみだしのチェックも行っている。
 */
static void
lvnsimage_or(LvnsImage *src, LvnsImage *dst, int x, int y)
{
    int i, j;
    int srcx, srcy;
    int dstx, dsty;
    int width, height;

    width = src->rwidth;
    height = src->rheight;

    x += src->xoffset;
    y += src->yoffset;

    /* はみでる部分の調整 */
    if (x < 0)  {
        srcx = -x;
        dstx = 0;
        width += x;
    } else {
        srcx = 0;
        dstx = x;
    }
    if (y < 0) {
        srcy = -y;
        dsty = 0;
        height += y;
    } else {
        srcy = 0;
        dsty = y;
    }
    if (dstx + width > dst->rwidth) {
        width = dst->rwidth - dstx;
    }
    if (dsty + height > dst->rheight) {
        height = dst->rheight  - dsty;
    }

    /* or 合成 */
    for (i = 0; i < height; i++) {
        u_char *p = src->data + (srcy + i) * src->rwidth + srcx;
        u_char *q = dst->data + (dsty + i) * dst->rwidth + dstx;
        for (j=0; j < width; j++, q++, p++) {
            *q |= *p;
        }
    }
}
#endif

/*
 * タイトル用雲合成用
 */
#ifndef USE_MGL
static void
OpShadow(LvnsImage *src, LvnsImage *dst, int x, int y)
{
    int i, j;
    int srcx, srcy;
    int dstx, dsty;
    int width, height;

    width = src->rwidth;
    height = src->rheight;

    x += src->xoffset;
    y += src->yoffset;

    /* はみでる部分の調整 */
    if (x < 0)  {
        srcx = -x;
        dstx = 0;
        width += x;
    } else {
        srcx = 0;
        dstx = x;
    }
    if (y < 0) {
        srcy = -y;
        dsty = 0;
        height += y;
    } else {
        srcy = 0;
        dsty = y;
    }
    if (dstx + width > dst->rwidth) {
        width = dst->rwidth - dstx;
    }
    if (dsty + height > dst->rheight) {
        height = dst->rheight  - dsty;
    }

    /* or 合成 */
    for (i = 0; i < height; i++) {
        u_char *p = src->data + (srcy + i) * src->rwidth + srcx;
        u_char *q = dst->data + (dsty + i) * dst->rwidth + dstx;
        for (j=0; j < width; j++, q++, p++) {
            if (*q == 15)
                *q = 3;
            else
                *q |= *p;
        }
    }
}
#else
static void
OpShadow(LvnsImage *src, LvnsImage *dst, int x, int y)
{
  lvnsimage_boxfill(dst, 0x955, 0, 0, 320, 25);
  mglimage_or(src, dst, x / 2, y);
}
#endif

static void
disp(Lvns *lvns, int latitude)
{
    lvns->setPaletteMulti(lvns, latitude);
    lvns->drawWindow(lvns);
    lvns->flushWindow(lvns);
}

/* パレット情報 「月」 */
static u_char pal0[][3] = {
    {51,0,85},
    {34,0,51},
    {17,0,34},
    {0,0,17},
    
    {153,102,187},
    {85,51,119},
    {85,51,119},
    {85,51,119},
    
    {119,68,170},
    {68,34,102},
    {68,34,102},
    {68,34,102},
    
    {85,34,153},
    {34,17,85},
    {34,17,85},
	{34,17,85},
};

/* パレット情報 千鶴 & 梓 */
static u_char pal1[][3] = {
    {0, 0, 0},
    {0, 192, 192},
    {0, 128, 128},
    {0, 64, 64},
    {192, 0, 0},
    {192, 192, 192},
    {192, 128, 128},
    {192, 64, 64},
    {128, 0, 0},
    {128, 192, 192},
    {128, 128, 128},
    {128, 64, 64},
    {64, 0, 0},
    {64, 192, 192},
    {64, 128, 128},
    {64, 64, 64},
};

/* パレット情報 楓 & 初音 */
static u_char pal2[][3] = {
    {0, 0, 0},
    {192, 0, 0},
    {128, 0, 0},
    {64, 0, 0},
    {0, 96, 192},
    {192, 96, 192},
    {64, 96, 192},
    {128, 96, 192},
    {0, 64, 128},
    {192, 64, 128},
    {128, 64, 128},
    {64, 64, 128},
    {0, 32, 64},
    {192, 32, 64},
    {128, 32, 64},
    {64, 32, 64},
};

/* 初期化 */
static int
init(Lvns *lvns, LvnsScript *scr)
{
    KizuatoState *state = (KizuatoState*)lvns->system_state;

    /* OP 用画像データロード */
    state->chizuru = LoadSDT(lvns, "OP1W.SDT", 336, 400); /* 千鶴 */
    state->azusa   = LoadSDT(lvns, "OP2W.SDT", 448, 512); /*  梓  */
    state->kaede   = LoadSDT(lvns, "OP3W.SDT", 448, 512); /*  楓  */
    state->hatsune = LoadSDT(lvns, "OP4W.SDT", 336, 400); /* 初音 */
    state->moon    = LvnsLoadImage(lvns, "OP2_MN_W.LFG", NULL); /* 月   */
    state->shadow  = LoadSDT(lvns, "OP2_KM_W.SDT", 1280, 400);   /* 影   */
    state->moon_pos = 0;

    /* 月のパレットを補正 */
#ifndef USE_MGL
    {
        int i;
        for (i=0; i < sizeof pal1 / sizeof pal1[0]; i++) {
            state->moon->palette[i][0] = pal0[i][0];
            state->moon->palette[i][1] = pal0[i][1];
            state->moon->palette[i][2] = pal0[i][2];
        }
    }
#endif
    return True;
}

/* 月その1 */
static int
moon(Lvns *lvns, LvnsScript *scr)
{
    KizuatoState *state = (KizuatoState*)lvns->system_state; 
    lvnsimage_copy(state->moon, lvns->vram);
    OpShadow(state->shadow, lvns->vram, state->moon_pos * -4, 0);

    if (scr->state < 16) 
        disp(lvns, scr->state);
    else if (scr->state > 24) 
        disp(lvns, (16 - (scr->state - 24)));
    else 
        disp(lvns, 16);

    state->moon_pos++;
    
    if (++scr->state >= 40) {
        return True;
    } else {
        return False;
    }
}

/* 千鶴 & 梓 */
static int
CandA(Lvns *lvns, LvnsScript *scr)
{
    KizuatoState *state = (KizuatoState*)lvns->system_state;
    lvnsimage_clear(lvns->vram);
#ifndef USE_MGL
    if (scr->state == 0) {
        int i;
        for (i=0; i < sizeof pal1 / sizeof pal1[0]; i++) {
            lvns->vram->palette[i][0] = pal1[i][0];
            lvns->vram->palette[i][1] = pal1[i][1];
            lvns->vram->palette[i][2] = pal1[i][2];
        }       
    }
    lvns->vram->palette_num = 16;
    lvnsimage_or(state->chizuru, lvns->vram, 640 - 336 + (scr->state - 32) * -4, 0);
    lvnsimage_or(state->azusa, lvns->vram, 0, (scr->state - 32 ) * 4);
#else
    mglimage_or(state->chizuru, lvns->vram, 320 - 168 + (scr->state - 32) * -2, 0);
    mglimage_or(state->azusa, lvns->vram, 0, (scr->state - 32) * 2);
#endif
    disp(lvns, (16 - abs(scr->state - 16)));

    if (++scr->state >= 32) {
        return True;
    } else {
        return False;
    }
}

/* 楓 & 初音 */
static int
KandH(Lvns *lvns, LvnsScript *scr)
{
    KizuatoState *state = (KizuatoState*)lvns->system_state;
    lvnsimage_clear(lvns->vram);
#ifndef USE_MGL
    if (scr->state == 0) {
        int i;
        for (i=0; i < sizeof pal1 / sizeof pal1[0]; i++) {
            lvns->vram->palette[i][0] = pal2[i][0];
            lvns->vram->palette[i][1] = pal2[i][1];
            lvns->vram->palette[i][2] = pal2[i][2];
        }
    }
    lvns->vram->palette_num = 16;
    lvnsimage_or(state->kaede, lvns->vram, 640-448, (scr->state - 32)*4);
    lvnsimage_or(state->hatsune, lvns->vram, (scr->state - 32)*4, 0);
#else
    mglimage_or(state->kaede, lvns->vram, 320-224, (scr->state - 32)*2);
    mglimage_or(state->hatsune, lvns->vram, (scr->state - 32)*2, 0);
#endif
    disp(lvns, (16 - abs(scr->state - 16)));

    if (++scr->state >= 32) {
        return True;
    } else {
        return False;
    }
}

/* 月その2 */
static int
moon2(Lvns *lvns, LvnsScript *scr)
{
    KizuatoState *state = (KizuatoState*)lvns->system_state;
    lvnsimage_copy(state->moon, lvns->vram);
    OpShadow(state->shadow, lvns->vram, state->moon_pos  * -4, 0);

    if (scr->state < 16) 
        disp(lvns, scr->state);
    else 
        disp(lvns, 16);

    state->moon_pos++;

    if (++scr->state > 80) {
        return True;
    } else {
        return False;
    }
}

static int
end(Lvns *lvns, LvnsScript *scr)
{       
    KizuatoState *state = (KizuatoState*)lvns->system_state;
    lvnsimage_delete(state->chizuru);
    lvnsimage_delete(state->azusa);
    lvnsimage_delete(state->kaede);
    lvnsimage_delete(state->hatsune);
    lvnsimage_delete(state->moon);
    lvnsimage_delete(state->shadow);
    return True;
}

static LvnsScriptData opdata[] = {
    { LVNS_SCRIPT_FUNC, init },
    { LVNS_SCRIPT_MUSIC, (void *)3 },
    { LVNS_SCRIPT_LOOP_FUNC, moon },
	{ LVNS_SCRIPT_LOOP_FUNC, CandA },
    { LVNS_SCRIPT_LOOP_FUNC, moon },
    { LVNS_SCRIPT_LOOP_FUNC, KandH },
    { LVNS_SCRIPT_LOOP_FUNC, moon2 },
    { LVNS_SCRIPT_CLICK_JUMP },
    { LVNS_SCRIPT_FUNC, LoadImage,  "OP2_BACK.LFG" },
    { LVNS_SCRIPT_DISP_VRAM, (void *)LVNS_EFFECT_NORMAL },
    { LVNS_SCRIPT_FUNC, SetPalette },
    { LVNS_SCRIPT_FUNC, AddImage,  "TITLE.LFG" },
    { LVNS_SCRIPT_FUNC, AddImage,  "TITLE2.LFG" },
    { LVNS_SCRIPT_DISP_VRAM, (void *)LVNS_EFFECT_FADE_MASK },
    { LVNS_SCRIPT_WAIT_CLICK }, 
    { LVNS_SCRIPT_FUNC, AddImage,  "TITLE2_.LFG" },
    { LVNS_SCRIPT_DISP_VRAM, (void *)LVNS_EFFECT_FADE_MASK },
    { LVNS_SCRIPT_FUNC, end   },
    { LVNS_SCRIPT_END }
};

/* ------------------------------------------------------------ */
/**  タイトルメニュー */

/* 痕ゲーム開始用スクリプト */
static LvnsScriptData gamestart[] = {
    { LVNS_SCRIPT_FUNC, LoadImage, "OP2_BACK.LFG" },
    { LVNS_SCRIPT_DISP_VRAM, (void *)LVNS_EFFECT_FADE_MASK },
    { LVNS_SCRIPT_END }
};

static MenuLine title_menu_line[] = {
	MENULINE(7, "ゲームを始める",1),
	MENULINE(8, "しおりの初期化",2),
#if 0
	MENULINE(9, "回想モード",3),
    MENULINE(10, "次回予告",4),
#endif
	MENULINE(9, "終了する",3),
	MENULINE(0, NULL, 0),
};

/*
 * タイトルメニューエンジン
 */
static int
TitleMenuEngine(Lvns *lvns, int select)
{
    switch (select) {
    case 1: /*ゲーム開始 */
		LvnsScriptRun(lvns, gamestart);
		return LvnsMenu(lvns, &siori_select_menu, True) > 0 ? 1: 0;
        
    case 2: /* しおり初期化 */
		LvnsMenu(lvns, &siori_init_menu, True);
		return 0;

    case 3: /* 終了 */
		LvnsClearLow(lvns, LVNS_EFFECT_FADE_MASK);
        LvnsJump(lvns, LVNS_JUMP_END);
    }

    return 0;
}

static MenuData title_menu = {
	title_menu_line,
	3, 1,
	TitleMenuEngine
};

/* ------------------------------------------------------------ */

/* 痕 オープニングエンジン */
void
KizuatoOpening(Lvns *lvns)
{
    KizuatoState *state = (KizuatoState*)lvns->system_state;

    state->chizuru = NULL;
    state->azusa   = NULL;
    state->kaede   = NULL;
    state->hatsune = NULL;
    state->moon    = NULL;
    state->shadow  = NULL;

	LvnsScriptRun(lvns, opdata);
	LvnsMenu(lvns, &title_menu, False);
    LvnsClearLow(lvns, LVNS_EFFECT_WIPE_TTOB);
}

/* ------------------------------------------------------------ */

/* 痕タイトル用スクリプト */
static LvnsScriptData titledata[] = {
    { LVNS_SCRIPT_FUNC, LoadImage, "OP2_BACK.LFG" },
    { LVNS_SCRIPT_FUNC, SetPalette },
    { LVNS_SCRIPT_FUNC, AddImage,  "TITLE.LFG" },
    { LVNS_SCRIPT_FUNC, AddImage,  "TITLE2.LFG" },
    { LVNS_SCRIPT_DISP_VRAM, (void *)LVNS_EFFECT_FADE_PALETTE },
    { LVNS_SCRIPT_WAIT_CLICK },
    { LVNS_SCRIPT_FUNC, AddImage,  "TITLE2_.LFG" },
    { LVNS_SCRIPT_DISP_VRAM, (void *)LVNS_EFFECT_FADE_MASK },
    { LVNS_SCRIPT_END }
};

/* 痕タイトルエンジン */
void
KizuatoTitle(Lvns *lvns)
{
	LvnsScriptRun(lvns, titledata);
	LvnsMenu(lvns, &title_menu, False);
}


