/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: LvnsHistory.c,v 1.3 2001/08/05 10:58:09 go Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Lvns.h"

/**
 * ヒストリ情報の登録
 */
void
LvnsAddHistory(Lvns *lvns, int no)
{
	if (lvns->history_pos >= lvns->history_size) {
		lvns->history_size += 100;
		lvns->history = realloc(lvns->history,
								sizeof(lvns->history[0]) * lvns->history_size + 1);
	}
	{
		LvnsHistoryData *hdat = &lvns->history[lvns->history_pos];
		hdat->scn = lvns->scn_current;
		hdat->blk = lvns->blk_current;
		hdat->no  = no;
	}
	lvns->history_pos++;
}

static void
checkCursor(Lvns *lvns)
{
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
}

void 
LvnsHistoryMode(Lvns *lvns)
{
	/* 状態の保存 */
	int scn = lvns->scn_current;
	int blk = lvns->blk_current;
	int fast_text = lvns->fast_text;
	int scn_offset = lvns->scn_cur - lvns->scn_cur_head;

	int pos = lvns->history_pos - 1;

	lvns->fast_text = True;
	lvns->current_tvram = 1;      /* 裏 */
	LvnsClearText(lvns);

	lvns->text_cursor_state = 0;
	lvns->select        = False;
	lvns->cursor_up     = False;
	lvns->cursor_down   = False;
	lvns->motion        = False;

	LVNS->dispHistory(lvns, pos);
	checkCursor(lvns);

	while (1) {
		LvnsFlip(lvns, True);
		
		if (lvns->cancel) {
			break;
		}

		if (lvns->motion) {
			checkCursor(lvns);
			lvns->motion = False;
		} 

		if (lvns->select) {
			switch (lvns->text_cursor_state) {
			case 1:
				lvns->cursor_up = True;
				break;
			case 2:
				lvns->cursor_down = True;
				break;
			default:
				break;
			}
			lvns->select = False;
		} 

		if (lvns->cursor_up) {
			if (pos > 0) {
				pos --;
				lvns->text_cursor_state = 0;
				LVNS->dispHistory(lvns, pos);
				checkCursor(lvns);
			}
			lvns->cursor_up = False;
		} 

		if (lvns->cursor_down) {
			if (pos < lvns->history_pos-1) {
				pos ++;
				lvns->text_cursor_state = 0;
				LVNS->dispHistory(lvns, pos);
				checkCursor(lvns);
			} else {
				lvns->cancel = True;
			}
			lvns->cursor_down = False;
		} 
	} /* while */

	lvns->select        = False;
	lvns->cancel        = False;
	lvns->cursor_up     = False;
	lvns->cursor_down   = False;
	lvns->motion        = False;

	/* 状態を復帰させる */
	lvns->fast_text     = fast_text;
	LvnsLoadScenario(lvns, scn, blk);
	lvns->scn_cur = lvns->scn_cur_head + scn_offset;

	lvns->current_tvram = 0;    /* 表 */
	LvnsDispWindow(lvns);

	return;
}
