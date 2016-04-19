/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: kizuato_etc.c,v 1.32 2001/08/20 14:15:26 tf Exp $
 */

/* 
 * 痕 各種処理ルーチン
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kizuato.h"

/* 痕 キャラ/背景合成処理 */
void
KizuatoMergeCharacter(Lvns *lvns)
{
    KizuatoState *state = (KizuatoState*)lvns->system_state;

    /* キャラクタを合成 */
    lvnsimage_add2(state->vram_char, lvns->vram, 0, 0);

    /* パレット置換 */
    if (state->change_palette) {
        int i;
        for (i=0;i<15;i++) {
            if (state->change_palette == 5) { /* 反転 */
                lvns->vram->palette[i][0] ^= 0xff;
                lvns->vram->palette[i][1] ^= 0xff;
                lvns->vram->palette[i][2] ^= 0xff;
            } else {
                int l = (76 * lvns->vram->palette[i][0] +
                         150 * lvns->vram->palette[i][1] +
                         30 * lvns->vram->palette[i][2]) / 256;
                switch (state->change_palette) {
                case 1: /* 白 */
                    lvns->vram->palette[i][0] = 
                        lvns->vram->palette[i][1] = 
                            lvns->vram->palette[i][2] = l;
                    break;
                case 2: /* 赤 */
                    lvns->vram->palette[i][0] = l;
                    lvns->vram->palette[i][1] = 0;
                    lvns->vram->palette[i][2] = 0;
                    break;
                case 3: /* 緑 */
                    lvns->vram->palette[i][0] = 0;
                    lvns->vram->palette[i][1] = l;
                    lvns->vram->palette[i][2] = 0;
                    break;
                case 4: /* 青 */
                    lvns->vram->palette[i][0] = 0;
                    lvns->vram->palette[i][1] = 0;
                    lvns->vram->palette[i][2] = l;
                    break;
                default:
                    fprintf(stderr, "unknown color effect...\n");
                }
            }
        }
    }

    /* パレット反転 */
    if (state->reverse_palette) {
	int i;
	for (i=0;i<16;i++) {
	    lvns->vram->palette[i][0] ^= 0xff;
	    lvns->vram->palette[i][1] ^= 0xff;
	    lvns->vram->palette[i][2] ^= 0xff;
	}
    }
}

/* 
 * 痕 画像消去
 */
void
KizuatoClearScreen(Lvns *lvns)
{
    KizuatoLoadBG(lvns, 0);
    KizuatoClearCharacter(lvns, 3);
}

/*
 * 痕 キャラクタロード
 */
void
KizuatoLoadCharacter(Lvns *lvns, int no, int pos)
{
	static int xpos[3] = { 0, 320, 160 };
    KizuatoState *state = (KizuatoState*)lvns->system_state;
    if (no != 0xff) {

        LvnsImage *image;
        char name[100];
        sprintf(name, "C%02x.LFG", no);

        image = LvnsLoadImage(lvns, name, NULL);
		
		if (image)
			lvnsimage_add2(image, state->vram_char, xpos[pos], 0);
		lvns->character_no[pos] = no;
        lvnsimage_delete(image);
    }
}


/*
 * 痕 キャラクタ消去
 */
void
KizuatoClearCharacter(Lvns *lvns, int pos)
{
#ifndef USE_MGL
	static int xpos[] = { 0, 320, 160 };
#define YPOS1 320
#else
	static int xpos[] = { 0, 160, 80 };
#define YPOS1 160
#endif
    KizuatoState *state = (KizuatoState*)lvns->system_state;

	if (pos > 2) {
        state->vram_char->palette_num = 15;
        state->vram_char->transparent = 15;
        lvnsimage_clear_transparent(state->vram_char);
        lvns->character_no[0] = NOCHARACTER;
        lvns->character_no[1] = NOCHARACTER;
        lvns->character_no[2] = NOCHARACTER;
	} else {
		lvnsimage_clear_area_transparent(state->vram_char, xpos[pos], 0, YPOS1, HEIGHT);
		lvns->character_no[pos] = NOCHARACTER;
	}
#undef XPOS1
#undef XPOS2
#undef YPOS1
}

/**
 * パレット置換テーブル
 */
u_char kizuato_haikei_palette[][16][3] = {
    /* 朝 */
    { {0x00,0x00,0x00},{0x33,0x44,0x33},{0x77,0x99,0x88},{0xdd,0xff,0xcc},
      {0xff,0xff,0xff},{0x22,0x33,0x55},{0x66,0x66,0xbb},{0xbb,0xcc,0xff}, 
      {0xff,0xee,0x77},{0xff,0x33,0x44},{0x55,0x33,0x22},{0x99,0x66,0x55},
      {0xee,0x99,0x77},{0xff,0xbb,0xaa},{0xff,0xdd,0xcc},{0x88,0xaa,0x99}},
    /* 昼 */
    { {0x00,0x00,0x00},{0x66,0x33,0x22},{0xbb,0x77,0x33},{0xff,0xcc,0x55},
      {0xff,0xff,0xff},{0x22,0x33,0x55},{0x66,0x66,0xbb},{0xbb,0xcc,0xff}, 
      {0xff,0xee,0x77},{0xff,0x33,0x44},{0x55,0x33,0x22},{0x99,0x66,0x55},
      {0xee,0x99,0x77},{0xff,0xbb,0xaa},{0xff,0xdd,0xcc},{0x88,0xaa,0x99}},
    /* 夜 */
    { {0x00,0x00,0x00},{0x33,0x22,0x55},{0x44,0x44,0x88},{0x66,0x66,0xcc},
      {0xff,0xff,0xff},{0x22,0x33,0x55},{0x66,0x66,0xbb},{0xbb,0xcc,0xff}, 
      {0xff,0xee,0xaa},{0xdd,0x33,0x77},{0x33,0x33,0x55},{0x77,0x66,0x88},
      {0xbb,0x99,0xaa},{0xdd,0xbb,0xff},{0xff,0xdd,0xff},{0x66,0x66,0xaa}},
    /* */
    { {0x00,0x00,0x00},{0x33,0x44,0x22},{0x77,0x88,0x55},{0xee,0xdd,0x88}, 
      {0xff,0xff,0xff},{0x22,0x33,0x55},{0x66,0x66,0xbb},{0xbb,0xcc,0xff}, 
      {0xff,0xee,0x77},{0xff,0x33,0x44},{0x55,0x33,0x22},{0x99,0x66,0x55},
      {0xee,0x99,0x77},{0xff,0xbb,0xaa},{0xff,0xdd,0xcc},{0x88,0xaa,0x99}},
    /* ? */
    { {0xff,0x00,0x00},{0xff,0x00,0x00},{0xff,0x00,0x00},{0xff,0x00,0x00},
      {0xff,0xff,0xff},{0x22,0x33,0x55},{0x66,0x66,0xbb},{0xbb,0xcc,0xff}, 
      {0xff,0xee,0x77},{0xff,0x33,0x44},{0x55,0x33,0x22},{0x99,0x66,0x55},
      {0xee,0x99,0x77},{0xff,0xbb,0xaa},{0xff,0xdd,0xcc},{0x88,0xaa,0x99}},
    /* ? */
    { {0xff,0x00,0x00},{0xff,0x00,0x00},{0xff,0x00,0x00},{0xff,0x00,0x00},
      {0xff,0xff,0xff},{0x22,0x33,0x55},{0x66,0x66,0xbb},{0xbb,0xcc,0xff}, 
      {0xff,0xee,0x77},{0xff,0x33,0x44},{0x55,0x33,0x22},{0x99,0x66,0x55},
      {0xee,0x99,0x77},{0xff,0xbb,0xaa},{0xff,0xdd,0xcc},{0x88,0xaa,0x99}},
};

#ifdef USE_MGL
int kizuato_haikei_palette_hsb[][3][16] = {
    /* 朝 */
  {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}},
    /* 昼 */
  {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}},
    /* 夜 */
  {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}},
    /* */
  {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}},
    /* ? */
  {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}},
    /* ? */
  {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}},
};
#endif

/*
 * 痕 背景画像ロード
 */
void
KizuatoLoadBG(Lvns *lvns, int no)
{
    KizuatoState *state = (KizuatoState*)lvns->system_state;

    int pal = no / 0x32;
    lvns->bg_type = LVNS_BACKGROUND;
    lvns->bg_no   = no;

    state->reverse_palette = False;
    state->change_palette = False;

    if (no == 254) {
#ifndef USE_MGL
        memset(lvns->background->data, 4, 
               lvns->background->rwidth * lvns->background->rheight);
        lvnsimage_set_palette(lvns->background,kizuato_haikei_palette[0],16);
#else
        memset(lvns->background->hsb_data, COLOR_BLACK, 
               lvns->background->rwidth * lvns->background->rheight);
#endif
    } else {
        no = no % 0x32;
        if (no == 0) {
            lvnsimage_clear(lvns->background);
        } else {
            LvnsLoadBackground(lvns, "S%02d.LFG", no);
        }
        switch (no) {
        case 33:
        case 34:
        case 38:
        case 39:
        case 40:
        case 41:
        case 42:
        case 43:
        case 49:
            break;
        default:
            if (pal > 3) {
                fprintf(stderr, "UNKNOWN pal: %d\n", pal);
            }
#ifndef USE_MGL
            lvnsimage_set_palette(lvns->background,kizuato_haikei_palette[pal],16);
#else
	    //mglimage_set_palette(lvns->background, kizuato_haikei_palette_hsb[no]);
#endif
            break;
        }
    }
    KizuatoClearCharacter(lvns, 3);
}

/*
 * 痕 通常ビジュアルシーンロード
 */
void 
KizuatoLoadVisual(Lvns *lvns, int no)
{
    KizuatoState *state = (KizuatoState*)lvns->system_state;

    state->reverse_palette = False;
    state->change_palette = False;

    lvns->bg_type = LVNS_VISUAL;
    lvns->bg_no   = no;
    LvnsLoadBackground(lvns, "V%02d.LFG",  no);
    KizuatoClearCharacter(lvns, 3);
}

/*
 * 痕 Hビジュアルシーンロード
 */
void
KizuatoLoadHVisual(Lvns *lvns, int no)
{
    KizuatoState *state = (KizuatoState*)lvns->system_state;

    state->reverse_palette = False;
    state->change_palette = False;

    lvns->bg_type = LVNS_HCG;
    lvns->bg_no   = no;
    LvnsLoadBackground(lvns, "H%02d.LFG", no);
    KizuatoClearCharacter(lvns, 3);
}

/*
 * 痕 シナリオのテキストロード 
 */
void
KizuatoSetTextScenarioState(Lvns *lvns, int no)
{
    KizuatoState *state = (KizuatoState*)lvns->system_state;

    /* 既読情報登録 */
    if (!lvns->seen_mode && no >= state->seen_flag[lvns->scn_current]) {
        state->seen_flag[lvns->scn_current] = no+1;
		lvns->seen = False;
        if (!lvns->force_skip)
            lvns->skip = False; /* すっ飛ばし解除 */
        lvns->fast_text = False;
    } else {
		lvns->seen = True;
		if (lvns->fast_when_seen)
			lvns->fast_text = True; /* 見た文書は速く表示する */
	}
}

static
LvnsCommandInfo command[] = {
    { "select",  False },   /* 次の選択肢までとばす */
    { "image",  False },    /* 画像表示             */
    { "save",   False },    /* データ保存     */
    { "load",   False },    /* データ読み込み */
    { "before", False },    /* 前の選択肢まで戻る */
    { "title",  False },    /* タイトルに戻る */
    { NULL,  False }
};

LvnsCommandInfo *
KizuatoGetCommandList(Lvns *lvns)
{
    command[0].enable =
	command[1].enable =
	command[2].enable =
	command[3].enable =
	command[4].enable =
	command[5].enable = False;
#if 0    
    if (lvns->scn_func == KizuatoScenarioEngine) {
        if (LVNSSTATE == LVNS_WAIT_KEY ||
            LVNSSTATE == LVNS_WAIT_PAGE ||
            LVNSSTATE == LVNS_WAIT_SELECT) {
            if (!lvns->select_mode) {
                command[0].enable = True;
            }
            command[1].enable = 
                command[2].enable =
                    command[3].enable =
                        command[4].enable =
                            command[5].enable = True;
        }
    }
#endif
    return command;
}

void
KizuatoExecCommand(Lvns *lvns, const char *command)
{
    if (!strcmp(command, "select")) {
		LvnsSkipTillSelect(lvns);
    } else if (!strcmp(command, "image")) {
		LvnsImageMode(lvns);
    } else if (!strcmp(command, "save")) {
        KizuatoSave(lvns);
    } else if (!strcmp(command, "load")) {
        KizuatoLoad(lvns);
    } else if (!strcmp(command, "before")) {
        /* 選択肢に戻る */
		KizuatoState *state = (KizuatoState*)lvns->system_state;
        lvns->savepoint = lvns->selectpoint;
        memcpy(state->flag_save, state->flag_select, sizeof state->flag);
    } else if (!strcmp(command, "title")) {
        /* タイトルに戻る */
		// KizuatoToTitle(lvns);
    }
}

/*
 * 仮想 VRAM に直接 EUC 文字列を描画
 */
#ifndef USE_MGL
void
KizuatoPutsVRAM(Lvns *lvns, int x, int y, const u_char *str, int idx) 
{
    int code;
    u_char *data;
    while (*str) {
        code = ((str[0]&0x7f)-33) * 94 + (str[1]&0x7f)-33;
        code = lvns->jis_to_leaf[code];
		if (code) {
			data = lvns->leaf_font_data + (code - 1) * 24 * 3;
			lvnsimage_put_pattern24_2(lvns->vram, x, y, idx, data);
		}
        x += 24;
        str += 2;
    }
}
#else
void
KizuatoPutsVRAM(Lvns *lvns, int x, int y, const u_char *str, int idx) 
{
#define EDCOL 4
#define EDSHA 0
    int color;

    switch (idx) {
    case EDCOL:
      color = COLOR_WHITE;
      break;
    case EDSHA:
      color = COLOR_BLACK;
      break;
    default:
      dprintf((stderr, "Unknown index [%d]\n", idx));
    }
    mglimage_draw_string(lvns->vram, x, y, str, color);
}
#endif

#ifndef USE_MGL
static void
drawChar(Lvns *lvns, int x, int y, int code, int attr) 
{
    u_char *data = lvns->leaf_font_data + (code - 1) * 24 * 3;
    lvns->putPattern(lvns, x+1, y+1, KIZUATO_COL_BLACK, data);
    lvns->putPattern(lvns, x+2, y+1, KIZUATO_COL_BLACK, data);
    if (!attr)
        lvns->putPattern(lvns, x, y, KIZUATO_COL_WHITE, data);
    else
        lvns->putPattern(lvns, x, y, KIZUATO_COL_GRAY, data);
}
#else
static void
drawChar(Lvns * lvns, int x, int y, int code, int attr)
{
    static int black[] = {
	 COLOR_TRANSPARENT, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK
    };

    static int gray[] = {
	 COLOR_TRANSPARENT, 0x00000006, 0x00000007, 0x0000008
    };

    static int white[] = {
	 COLOR_TRANSPARENT, 0x00000008, 0x0000000c, 0x000000f
    };
#ifdef FONT_PLUS
    if( lvns->font != NULL){
       x += MGL_XOFFSET;
       y += MGL_YOFFSET;
       code--;

       lvnsfont_draw( lvns->font, x + 1, y + 1, code, black);
       if( !attr){
           lvnsfont_draw( lvns->font, x, y, code, white);
       } else {
           lvnsfont_draw( lvns->font, x, y, code, gray);
       }
    } else {
#endif /* FONT_PLUS */
    char buf[3];

#if 0
    if (lvns->skip && !attr)
      MglGetEvent(lvns, 1);
#endif

    buf[0] = lvns->leaf_to_euc[code * 2];
    buf[1] = lvns->leaf_to_euc[code * 2 + 1];
    buf[2] = 0;

    x += MGL_XOFFSET;
    y += MGL_YOFFSET;

    set_font(12, 0);

    set_color(COLOR_BLACK);
    draw_string(x + 1, y + 1, buf, DIR_NORTH);
    draw_string(x + 2, y + 1, buf, DIR_NORTH);

    if (!attr) {
	set_color(COLOR_WHITE);
    } else {
	set_color(COLOR_LIGHTGRAY);
    }
    draw_string(x, y, buf, DIR_NORTH);
#ifdef FONT_PLUS
    }
#endif /* FONT_PLUS */
}
#endif

void
KizuatoDisp(Lvns *lvns, LvnsEffectType effect_disp)
{
    KizuatoState *state = (KizuatoState*)lvns->system_state;
    if (state->change_palette2) {
        state->change_palette = state->change_palette2;
        state->change_palette2 = 0;
    }
    LvnsDisp(lvns, effect_disp);
}

extern MenuData kizuato_menu;

static LvnsSystemState kizuato_state_base = {
#ifndef USE_MGL
    640,
    400,
#else
    320,
    200,
#endif
    18, /* XXX */
    "MAX2DATA.PAK",
    NULL,
    "%03d.scn",
    "kizfont.tbl",
	
	&kizuato_menu,

	KizuatoStart,
	KizuatoJingle,
	KizuatoOpening,
	KizuatoTitle,
	KizuatoMain,

    KizuatoMergeCharacter,
    KizuatoClearScreen,
    KizuatoLoadBG,
    KizuatoGetCommandList,
    KizuatoExecCommand,
    drawChar,
	KizuatoDispHistory,

#if !defined(USE_MGL) || defined(FONT_PLUS)
    223, /* キー入力待ち */
    226, /* 改ページ待ち */
#else
    2084, /* キー入力待ち */
    1, /* 改ページ待ち */
#endif
};

void
KizuatoInitialize(Lvns *lvns)
{
    KizuatoState *state = (KizuatoState*)lvns->system_state 
		= malloc(sizeof(KizuatoState));
    state->state = kizuato_state_base;
    state->vram_char = lvnsimage_new(WIDTH, HEIGHT, 0, 0);
    KizuatoSioriInit(lvns);
}
