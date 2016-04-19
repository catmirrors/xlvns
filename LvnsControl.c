/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: LvnsControl.c,v 1.21 2001/08/05 12:01:20 go Exp $
 */

/*
 * 外部から LVNSモジュールを制御するためのインターフェース
 */
#include <stdio.h>
#include <stdlib.h>
#include "Lvns.h"

/*
 * マウスカーソルの移動
 */
void
LvnsMotion(Lvns *lvns, int x, int y)
{
	lvns->motion = True;
	lvns->motion_x = x;
	lvns->motion_y = y;
}

void
LvnsSelect(Lvns *lvns)
{
	lvns->select    = True;
	lvns->skip      = False;
}

void
LvnsCancel(Lvns *lvns)
{
	lvns->cancel    = True;
	lvns->skip      = False;
}

void
LvnsInputNumber(Lvns *lvns, int no) {
	lvns->number = no;
}


/*
 * イメージ表示モードに遷移(トリガ駆動)
 */
void
LvnsSetImageMode(Lvns *lvns)
{
	if (lvns->image_mode)
		lvns->image_mode = False;
	else
		lvns->image_mode = True;
}

/*
 * 選択肢まで飛ばす
 */
void
LvnsSkipTillSelect(Lvns *lvns)
{
    if (lvns->seen) {
		lvns->skip = True; // スキップON
    }
}

void
LvnsCursorUp(Lvns *lvns)
{
	lvns->cursor_up = True;
}

void
LvnsCursorDown(Lvns *lvns)
{
	lvns->cursor_down = True;
}

/**
 * 文字消去モード
 */
void
LvnsImageMode(Lvns *lvns)
{
	lvns->image_mode    = True;
	lvns->select        = False;
	lvns->cancel        = False;
	
	LvnsTextOff(lvns);
	LvnsLighten(lvns);

	while (lvns->image_mode) {
		LvnsFlip(lvns, True);
		if (lvns->select || lvns->cancel || lvns->skip)
			break;
	}
	lvns->select        = False;
	lvns->cancel        = False;    
	lvns->image_mode    = False;

	LvnsDispText(lvns);
}

/*
 * 改ページ待ち
 * (メニュー起動有り)
 */
void
LvnsWaitPage(Lvns *lvns)
{
	int demo_cnt = 0;
	int flip_cnt = 0;

	lvns->inside_state = LVNS_WAIT_PAGE;

	lvns->select        = False;
	lvns->cancel        = False;
	lvns->image_mode    = False;
	lvns->cursor_up     = False;

	if (!lvns->skip) {
		while (1) {
			LvnsFlip(lvns, True);
			if (lvns->select || 
				lvns->skip ||
				(lvns->demo_mode && ++demo_cnt > 120)) {
				break;
			} else if (lvns->cancel) {
				/* メニュー起動 */
				LvnsTextOff(lvns);
				LvnsMenu(lvns, LVNS->main_menu, True);
				LvnsTextOn(lvns);
				LvnsDispWindow(lvns);       /* 描画状態を戻す */
				lvns->cancel = False;
			} else if (lvns->image_mode) {
				LvnsImageMode(lvns);
			} else if (lvns->cursor_up) {
				LvnsHistoryMode(lvns);
			}
			if (flip_cnt == 0)
				LvnsDrawCursor(lvns, LVNS_CURSOR_PAGE);
			flip_cnt = ++flip_cnt % 6;
		}
		LvnsClearCursor(lvns);
		lvns->select        = False;
	}

	/* 文字消去 */
	LvnsClearText(lvns);
	LvnsDispWindow(lvns);
}
	
/*
 * キー入力待ち
 * (メニュー起動有り)
 */
void
LvnsWaitKey(Lvns *lvns)
{
	int demo_cnt = 0;
	int flip_cnt = 0;

	lvns->inside_state = LVNS_WAIT_KEY;

	lvns->select        = False;
	lvns->cancel        = False;
	lvns->image_mode    = False;
	lvns->cursor_up     = False;

	if (!lvns->skip) {
		while (1) {
			LvnsFlip(lvns, True);
			if (lvns->select || 
				lvns->skip ||
				(lvns->demo_mode && ++demo_cnt > 60)) {
				break;
			} else if (lvns->cancel) {
				/* メニュー起動 */
				LvnsTextOff(lvns);
				LvnsMenu(lvns, LVNS->main_menu, True);
				LvnsTextOn(lvns);
				LvnsDispWindow(lvns); /* 描画状態を戻す */
			} else if (lvns->image_mode) {
				LvnsImageMode(lvns);
			} else if (lvns->cursor_up) {
				LvnsHistoryMode(lvns);
			}
			if (flip_cnt == 0)
				LvnsDrawCursor(lvns, LVNS_CURSOR_KEY);
			flip_cnt = ++flip_cnt % 6;
		}
		LvnsClearCursor(lvns);
		lvns->select        = False;
	}
}

/*
 * 選択待ち
 * (メニュー起動有り)
 */
int
LvnsWaitSelect(Lvns *lvns, int select_num)
{
	int demo_cnt      = 0;

	lvns->inside_state = LVNS_WAIT_SELECT;

	lvns->force_skip = False;
	lvns->skip       = False;	/* 「選択肢まで進む」の解除 */
	
	lvns->text_cursor_state = 0;
	lvns->select        = False;
	lvns->cancel        = False;
	lvns->cursor_up     = False;
	lvns->cursor_down   = False;
	lvns->motion        = False;
	lvns->number        = 0;

	while (1) {
		LvnsFlip(lvns, True);
		if (lvns->select) {
			if (lvns->text_cursor_state)
				break;
			else
				lvns->select = False;
		} else if (lvns->cancel) {
			/* メニュー起動 */
			LvnsTextOff(lvns);
			LvnsMenu(lvns, LVNS->main_menu, True);
			LvnsTextOn(lvns);
			LvnsDispWindow(lvns);       /* 描画状態を戻す */
			lvns->cancel = False;
		} else if (lvns->motion) {
			struct TextVram *c;
			int y = R_YPOS(lvns->motion_y);
			int x = R_XPOS(lvns->motion_x, y);
			if (x >= 0 && y >=0 && x < TEXT_WIDTH && y < TEXT_HEIGHT) {
				c = &lvns->tvram[lvns->current_tvram].row[y].column[x];
				if (lvns->text_cursor_state != c->attribute) {
					if (lvns->text_cursor_state != 0)
						LvnsClearTextCursor(lvns);
					if ((lvns->text_cursor_state = c->attribute) != 0) {
						LvnsDrawTextCursor(lvns);
					}
				}
			}
			lvns->motion = False;
		} else if (lvns->cursor_up) {
			if (lvns->text_cursor_state == 0) {
				lvns->text_cursor_state = select_num;
				LvnsDrawTextCursor(lvns);
			} else if (lvns->text_cursor_state > 1) {
				LvnsClearTextCursor(lvns);
				lvns->text_cursor_state--;
				LvnsDrawTextCursor(lvns);
			}
			lvns->cursor_up = False;
		} else if (lvns->cursor_down) {
			if (lvns->text_cursor_state == 0) {
				lvns->text_cursor_state = 1;
				LvnsDrawTextCursor(lvns);
			} else if (lvns->text_cursor_state < select_num) {
				LvnsClearTextCursor(lvns);
				lvns->text_cursor_state++;
				LvnsDrawTextCursor(lvns);
			}
			lvns->cursor_down = False;
		} else if (lvns->number > 0) {
			if (lvns->number <= select_num) {
				lvns->text_cursor_state = lvns->number;
				break;
			} else {
				lvns->number = 0;
			}
		} else if (lvns->demo_mode) {
			demo_cnt++;
			if (demo_cnt == 30) {
				if (lvns->text_cursor_state != 0)
					LvnsClearTextCursor(lvns);
				lvns->text_cursor_state = (1.0 / (RAND_MAX + 1.0)) * rand() 
					* select_num + 1;
				LvnsDrawTextCursor(lvns);
			} else if (demo_cnt > 240) { 
				break;
			}
		}
	} /* while */

	lvns->select        = False;
	lvns->cancel        = False;
	lvns->cursor_up     = False;
	lvns->cursor_down   = False;
	lvns->motion        = False;

	/* 文字消去 */
	LvnsClearText(lvns);
	LvnsDispWindow(lvns);

	return lvns->text_cursor_state - 1;
}

/*
 * クリック待ち
 */
void
LvnsWaitClick(Lvns *lvns)
{
	int demo_cnt = 0;
	lvns->select = False;
	lvns->cancel = False;
	while(1) {
		LvnsFlip(lvns, True);
		if (lvns->select ||
			lvns->cancel ||
			(lvns->demo_mode && ++demo_cnt > 60)) {
			break;
		}
	}
	lvns->select = False;
	lvns->cancel = False;
}
