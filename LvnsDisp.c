/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: LvnsDisp.c,v 1.16 2001/08/11 19:59:36 tf Exp $
 *
 */

/*
 * Lvns 表示処理周辺
 */

#include <stdio.h>
#include "Lvns.h"
#include "LvnsEffect.h"

/**
 * 現在のテキストレイヤを全部再描画する
 */
static void
DrawTextLayerAll(Lvns *lvns)
{
    int i, j;
    for (i=0; i<TEXT_HEIGHT;i++) {
        for (j=0; j<TEXT_WIDTH; j++) {
            struct TextVram *c = 
                &lvns->tvram[lvns->current_tvram].row[i].column[j];
            if (c->code) {
                int x = XPOS(j,i);
                int y = YPOS(i);
				LVNS->drawChar(lvns, x, y, c->code, c->attribute);
            }
        }
    }
}

/*
 * カーソルの消去
 */
void
LvnsClearCursor(Lvns *lvns)
{
    if (lvns->cursor_state) {
        int x = XPOS(lvns->tvram[lvns->current_tvram].o_cur_x, 
                     lvns->tvram[lvns->current_tvram].o_cur_y);
        int y = YPOS(lvns->tvram[lvns->current_tvram].o_cur_y); 
        lvns->drawWindowArea(lvns, x, y, CHARDRAWSIZE, CHARDRAWSIZE, x, y);
        lvns->flushWindowArea(lvns, x, y, CHARDRAWSIZE, CHARDRAWSIZE);
        lvns->cursor_state = 0;
    }
}

/*
 * カーソルの描画
 */
void
LvnsDrawCursor(Lvns *lvns, LvnsCursorType cursor_type)
{       
    int cursor;

    if (cursor_type == LVNS_CURSOR_PAGE) {
        cursor = LVNS->cursor_page;
    } else
        cursor = LVNS->cursor_key;

    if (lvns->cursor_state == 0) {
        int x = XPOS(lvns->tvram[lvns->current_tvram].cur_x,
                     lvns->tvram[lvns->current_tvram].cur_y);
        int y = YPOS(lvns->tvram[lvns->current_tvram].cur_y);

        LVNS->drawChar(lvns, x, y, cursor, 0);
        lvns->flushWindowArea(lvns, x, y, CHARDRAWSIZE, CHARDRAWSIZE);

        lvns->tvram[lvns->current_tvram].o_cur_x = 
            lvns->tvram[lvns->current_tvram].cur_x;
        lvns->tvram[lvns->current_tvram].o_cur_y = 
            lvns->tvram[lvns->current_tvram].cur_y;
        
        lvns->cursor_state = 1;

    } else {
        LvnsClearCursor(lvns);
    }
}

/**
 * テキストカーソルの消去
 */
void
LvnsClearTextCursor(Lvns *lvns)
{
    int i, j;
    for (i=0; i<TEXT_HEIGHT;i++) {
        for (j=0; j<TEXT_WIDTH; j++) {
            struct TextVram *c = 
                &lvns->tvram[lvns->current_tvram].row[i].column[j];
            if (c->code && c->attribute == lvns->text_cursor_state) {
                int x = XPOS(j,i);
                int y = YPOS(i);
                LVNS->drawChar(lvns, x, y, c->code, 1);
                lvns->flushWindowArea(lvns, x, y, CHARDRAWSIZE, CHARDRAWSIZE);
            }
        }
    }
}

/**
 * テキストカーソルの表示
 */
void
LvnsDrawTextCursor(Lvns *lvns)
{
    int i, j;
    for (i=0; i<TEXT_HEIGHT;i++) {
        for (j=0; j<TEXT_WIDTH; j++) {
            struct TextVram *c = 
                &lvns->tvram[lvns->current_tvram].row[i].column[j];
            if (c->code && c->attribute == lvns->text_cursor_state) {
                int x = XPOS(j,i);
                int y = YPOS(i);
                LVNS->drawChar(lvns, x, y, c->code, 0);
                lvns->flushWindowArea(lvns, x, y, CHARDRAWSIZE, CHARDRAWSIZE);
            }
        }
    }
}

/**
 * 現在の画面(背景 + 文字)を 仮想 vram から描画させる
 */
void
LvnsDispWindow(Lvns *lvns)
{
	/* グラフィック部 */
    lvns->setPaletteMulti(lvns, lvns->latitude);
    lvns->drawWindow(lvns);

	/* 追加表示処理(メニューなどで利用) */
	if (lvns->disp_func) 
		lvns->disp_func(lvns, lvns->disp_func_param);

	/* テキスト表示 */
    if (lvns->text_mode) {
		DrawTextLayerAll(lvns);
    }

	lvns->flushWindow(lvns);
}

/* 文字消去処理 */
void
LvnsUndispText(Lvns *lvns)
{
	LvnsFlip(lvns, True); // 描画同期のため…
	lvns->text_mode = False;
	if (lvns->skip) {
		lvns->latitude = 16;
		LvnsDispWindow(lvns);
	} else {
		LvnsLighten(lvns);
	}
}

/* 文字再表示処理 */
void
LvnsDispText(Lvns *lvns)
{
	if (!lvns->text_mode) {
		lvns->text_mode = True;
		if (lvns->skip) {
			lvns->latitude  = lvns->latitude_dark;
		} else {
			LvnsDarken(lvns);
		}
		LvnsDispWindow(lvns);
		LvnsFlip(lvns, True); // 描画同期のため…
	}
}

/**
 * 背景とキャラクタの状態から画像を合成して表示する
 * 表示後しばらく待ちが入る
 */
void
LvnsDisp(Lvns *lvns, LvnsEffectType effect)
{
	/* BGM 変更チェック */
	LvnsStartNextMusic(lvns);

	/* 背景エフェクト指定 */
	if (lvns->effect_back_next) {
		lvns->effect_back = lvns->effect_back_next;
		lvns->effect_back_next = NULL;
	}

	/* 絵を仮想VRAMに設定 */
	lvnsimage_copy(lvns->background, lvns->vram);
	LVNS->mergeCharacter(lvns);

	/* セピア調表示ならパレット修正 */
	if (lvns->sepia_mode) {
	  lvnsimage_set_sepia(lvns->vram);
	}

	if (lvns->skip) {
		LvnsDispWindow(lvns);
	} else {
		LvnsDispLow(lvns, effect);
		LvnsWait(lvns, 500 * INTERVAL / 1000);
	}
}

/** 
 * 消去処理を行う
 * 
 */
void
LvnsClear(Lvns *lvns, LvnsEffectType effect) 
{
	if (lvns->skip) {
		lvnsimage_clear(lvns->vram);
		LvnsDispWindow(lvns);
	} else {
		LvnsClearLow(lvns, effect);
	}
	LvnsSetBackEffect(lvns, NULL);
}

/**
 * 直接テキスト描画
 */
void
LvnsDrawString(Lvns *lvns, int x, int y, const u_char *str, int attr)
{
    int code;  
    while (*str) {
        code = ((str[0]&0x7f)-33) * 94 + (str[1]&0x7f)-33;
        code = lvns->jis_to_leaf[code];
		if (code)
			LVNS->drawChar(lvns, x, y, code, attr);
        str += 2;
        x += CHARSIZE;
    }
}

/*
 * 直接テキスト描画(センタリング)
 */
void
LvnsDrawStringCenter(Lvns *lvns, int y, const u_char *str, int attr)
{
	LvnsDrawString(lvns, (WIDTH - strlen(str)/2 * CHARSIZE)/2,
				   y, str, attr);
}

void
LvnsTextOff(Lvns *lvns)
{
	lvns->text_mode = False;
}

void
LvnsTextOn(Lvns *lvns)
{
	lvns->text_mode = True;
}
