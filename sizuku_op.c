/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: sizuku_op.c,v 1.45 2001/08/05 10:58:10 go Exp $
 *
 */

/* 
 * 雫 オープニング/タイトル
 */

#include <stdio.h>
#include "sizuku.h"

/* OP 用アニメデータ「涙の雫」 */
static LvnsAnimationData sizuku[] = {
    { LVNS_ANIM_IMAGE, "OP_S00.LFG", 50, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S01.LFG", 50, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S02.LFG", 50, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S03.LFG", 50, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S04.LFG", 50, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S05.LFG", 50, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S06.LFG", 50, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S07.LFG", 50, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S08.LFG", 50, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S09.LFG", 50, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S10.LFG", 50, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S11.LFG", 50, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S12.LFG", 50, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S13.LFG", 50, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S14.LFG", 50, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S15.LFG", 50, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S16.LFG", 50, 0, 160 },
    { LVNS_ANIM_NONE, NULL, 0 }
};

/* OP 用アニメデータ「瑠璃子」 */
static LvnsAnimationData ruriko[] = {
    { LVNS_ANIM_IMAGE, "OP_L0.LFG", 50, 0, 0 },
    { LVNS_ANIM_IMAGE, "OP_L1.LFG", 50, 0, 0 },
    { LVNS_ANIM_IMAGE, "OP_L2.LFG", 50, 0, 0 },
    { LVNS_ANIM_IMAGE, "OP_L1.LFG", 50, 0, 0 },

    { LVNS_ANIM_IMAGE, "OP_L0.LFG", 50, 0, 0 },
    { LVNS_ANIM_IMAGE, "OP_L1.LFG", 50, 0, 0 },
    { LVNS_ANIM_IMAGE, "OP_L2.LFG", 50, 0, 0 },
    { LVNS_ANIM_IMAGE, "OP_L1.LFG", 50, 0, 0 },

    { LVNS_ANIM_IMAGE, "OP_L0.LFG", 50, 0, 0 },
    { LVNS_ANIM_IMAGE, "OP_L1.LFG", 50, 0, 0 },
    { LVNS_ANIM_IMAGE, "OP_L2.LFG", 50, 0, 0 },
    { LVNS_ANIM_IMAGE, "OP_L1.LFG", 50, 0, 0 },

    { LVNS_ANIM_IMAGE, "OP_L0.LFG", 50, 0, 0 },
    { LVNS_ANIM_IMAGE, "OP_L1.LFG", 50, 0, 0 },
    { LVNS_ANIM_IMAGE, "OP_L2.LFG", 50, 0, 0 },
    { LVNS_ANIM_IMAGE, "OP_L1.LFG", 50, 0, 0 },

    { LVNS_ANIM_IMAGE, "OP_L0.LFG", 50, 0, 0 },
    { LVNS_ANIM_IMAGE, "OP_L1.LFG", 50, 0, 0 },
    { LVNS_ANIM_IMAGE, "OP_L2.LFG", 50, 0, 0 },
    { LVNS_ANIM_IMAGE, "OP_L1.LFG", 50, 0, 0 },

    { LVNS_ANIM_IMAGE, "OP_L0.LFG", 50, 0, 0 },
    { LVNS_ANIM_IMAGE, "OP_L1.LFG", 50, 0, 0 },
    { LVNS_ANIM_IMAGE, "OP_L2.LFG", 50, 0, 0 },
    { LVNS_ANIM_IMAGE, "OP_L1.LFG", 50, 0, 0 },

    { LVNS_ANIM_IMAGE, "OP_L0.LFG", 50, 0, 0 },
    { LVNS_ANIM_IMAGE, "OP_L3.LFG", 50, 0, 0 },
    { LVNS_ANIM_IMAGE, "OP_L4.LFG", 50, 0, 0 },
    { LVNS_ANIM_IMAGE, "OP_L5.LFG", 50, 0, 0 },

    { LVNS_ANIM_IMAGE, "OP_L6.LFG", 50, 0, 0 },
    { LVNS_ANIM_IMAGE, "OP_L7.LFG", 50, 0, 0 },
    { LVNS_ANIM_IMAGE, "OP_L8.LFG", 50, 0, 0 },
    { LVNS_ANIM_IMAGE, "OP_L7.LFG", 50, 0, 0 },

    { LVNS_ANIM_IMAGE, "OP_L6.LFG", 50, 0, 0 },
    { LVNS_ANIM_IMAGE, "OP_L7.LFG", 50, 0, 0 },
    { LVNS_ANIM_IMAGE, "OP_L8.LFG", 50, 0, 0 },
    { LVNS_ANIM_IMAGE, "OP_L7.LFG", 50, 0, 0 },

    { LVNS_ANIM_IMAGE, "OP_L6.LFG", 50, 0, 0 },
    { LVNS_ANIM_IMAGE, "OP_L7.LFG", 50, 0, 0 },
    { LVNS_ANIM_IMAGE, "OP_L8.LFG", 50, 0, 0 },
    { LVNS_ANIM_IMAGE, "OP_L7.LFG", 50, 0, 0 },

    { LVNS_ANIM_NONE, NULL, 0 }
};

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
    SizukuState *state = (SizukuState *)lvns->system_state;
	
	state->siori_no = select;
	SizukuLoad(lvns);
	if (lvns->savepoint.scn) { /* シナリオの先頭か? */
		switch (LvnsMenu(lvns, &from_first_menu, True)) {
		case -1: /* cancel */
			return 0;
		case 1:
			SizukuScenarioInit(lvns);
		case 2:			
			break;
		}
	}
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
    SizukuState *state = (SizukuState *)lvns->system_state;
	state->siori_no = select;
	SizukuSioriInit(lvns);
	SizukuSave(lvns);
    return select;
}

static MenuData siori_init_menu = {
	siori_init_menu_line,
	3, 1, 
	SioriInitMenuEngine
};

/* ------------------------------------------------------------ */
/**  タイトルメニュー */

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
		LvnsClearLow(lvns, LVNS_EFFECT_FADE_MASK);
		return LvnsMenu(lvns, &siori_select_menu, True) > 0 ? 1: 0;
        
    case 2: /* しおり初期化 */
		LvnsMenu(lvns, &siori_init_menu, True);
		return 0;

    case 3: /* 終了 */ 
		LvnsClearLow(lvns, LVNS_EFFECT_FADE_MASK);
		LvnsJump(lvns, LVNS_JUMP_END);
    }

    return select;
}

static MenuData title_menu = {
	title_menu_line,
	3, 1,
	TitleMenuEngine
};

/* ------------------------------------------------------------ */

static int
LoadImage(Lvns *lvns, LvnsScript *scr, 
          void * param1, void * param2) 
{
#ifndef USE_MGL
    LvnsLoadImage(lvns, param1, lvns->background);
    lvnsimage_copy(lvns->background, lvns->vram);
    lvns->setPalette(lvns);
#else
    mgImage *image = LvnsLoadImage(lvns, param1, NULL);
  
    if (image) {
      lvnsimage_clear(lvns->vram);
      mglimage_add(image, lvns->vram, 0, 0);
      mglimage_delete(image);
    }
#endif
    return True;
}

static int
AddImage(Lvns *lvns, LvnsScript *scr, 
         void * param1, void * param2) 
{
    LvnsImage *image = LvnsLoadImage(lvns, param1, NULL);
    lvnsimage_add2(image, lvns->vram, 0, 0);
    lvnsimage_delete(image);
    return True;
}

/* 雫 オープニングスクリプト */
static LvnsScriptData opdata[] = {
    { LVNS_SCRIPT_MUSIC, (void *)16 },
    { LVNS_SCRIPT_ANIM, sizuku },
    { LVNS_SCRIPT_CLEAR, (void *)LVNS_EFFECT_NORMAL },
    { LVNS_SCRIPT_FUNC, LoadImage, "OP_V0.LFG" },
    { LVNS_SCRIPT_DISP_VRAM, (void *)LVNS_EFFECT_FADE_PALETTE },
    { LVNS_SCRIPT_WAIT, (void *)2000 },
    { LVNS_SCRIPT_CLEAR, (void *)LVNS_EFFECT_FADE_PALETTE },
    { LVNS_SCRIPT_ANIM, sizuku },
    { LVNS_SCRIPT_CLEAR, (void *)LVNS_EFFECT_NORMAL },
    { LVNS_SCRIPT_FUNC, LoadImage, "OP_V1.LFG" },
    { LVNS_SCRIPT_DISP_VRAM, (void *)LVNS_EFFECT_FADE_PALETTE },
    { LVNS_SCRIPT_WAIT, (void *)2000 },
    { LVNS_SCRIPT_CLEAR, (void *)LVNS_EFFECT_FADE_PALETTE },
    { LVNS_SCRIPT_ANIM, sizuku },
    { LVNS_SCRIPT_CLEAR, (void *)LVNS_EFFECT_NORMAL },
    { LVNS_SCRIPT_ANIM, ruriko },
    { LVNS_SCRIPT_CLICK_JUMP },
    { LVNS_SCRIPT_CLEAR, (void *)LVNS_EFFECT_FADE_PALETTE },
    { LVNS_SCRIPT_FUNC, LoadImage, "TITLE0.LFG"},
    { LVNS_SCRIPT_DISP_VRAM, (void *)LVNS_EFFECT_FADE_PALETTE },
    { LVNS_SCRIPT_WAIT, (void *)1000 },
    { LVNS_SCRIPT_FUNC, AddImage,  "TITLE.LFG" },
    { LVNS_SCRIPT_DISP_VRAM, (void *)LVNS_EFFECT_FADE_MASK },
    { LVNS_SCRIPT_WAIT_CLICK },
    { LVNS_SCRIPT_END }
};

/* 雫 オープニング */
void
SizukuOpening(Lvns *lvns)
{
    LvnsInitAnimation(lvns, sizuku);
    LvnsInitAnimation(lvns, ruriko);
	LvnsScriptRun(lvns, opdata);
	LvnsMenu(lvns, &title_menu, False);
}

/* ------------------------------------------------------------ */

/* 雫タイトル用スクリプト */
static LvnsScriptData titledata[] = {
    { LVNS_SCRIPT_FUNC, LoadImage, "TITLE0.LFG" },
    { LVNS_SCRIPT_FUNC, AddImage,  "TITLE.LFG" },
    { LVNS_SCRIPT_DISP_VRAM, (void *)LVNS_EFFECT_FADE_MASK },
    { LVNS_SCRIPT_WAIT_CLICK },
    { LVNS_SCRIPT_END }
};

void
SizukuTitle(Lvns *lvns)
{
	LvnsScriptRun(lvns, titledata);
	LvnsMenu(lvns, &title_menu, False);
}


