/*
 * $Id: LvnsMenu.c,v 1.12 2001/08/12 11:35:44 tf Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include "Lvns.h"

/**
 * デフォルトの表示処理
 */
void
LvnsDefaultMenuDisp(Lvns *lvns, MenuData *data)
{
	MenuLine *line = data->line;
	while (line->str != NULL) {
		LvnsDrawString(lvns, line->xpos, line->ypos, line->str, 
					   line->attr == lvns->text_cursor_state ? 0 : 1);
		line++;
	}
}

/**
 * ポインタの位置チェック
 */
static int
check_pos(Lvns *lvns, MenuData *data)
{
	Bool changed = False;
	MenuLine *line = data->line;
	while (line->str != NULL) {
		if (lvns->motion_x >= line->xpos &&
			lvns->motion_y >= line->ypos &&
			lvns->motion_x <  line->xpos + line->width && 
			lvns->motion_y <  line->ypos + line->height) {
			lvns->text_cursor_state = line->attr;
			changed = True;
			break;
		}
		line++;
	}
	return changed;
}

/**
 * メニュー処理
 */
int
LvnsMenu(Lvns *lvns, MenuData *data, int cancelok)
{
	int ret;
	int demo_cnt;
	Bool changed;

	lvns->inside_state = LVNS_WAIT_MENU;
	
 start:
	lvns->latitude      = lvns->latitude_dark;
	
	demo_cnt = 0;
	lvns->text_cursor_state = 0;
	lvns->select        = False;
	lvns->cancel        = False;
	lvns->cursor_up     = False;
	lvns->cursor_down   = False;
	lvns->motion        = False;
	lvns->number        = 0;

	
	LvnsSetDispFunc(lvns, data->disp ? data->disp: LvnsDefaultMenuDisp, data);

	check_pos(lvns, data);
	LvnsDispWindow(lvns);

	changed = False;
	while (1) {

		LvnsFlip(lvns, True);

		if ((!lvns->effect_back || !lvns->enable_effect_back)
		    && changed) {
			/* 再描画処理 */
			LvnsDispWindow(lvns);
			changed = False;
		}
	
		if (lvns->select) {
			if (lvns->text_cursor_state)
				break;
			else
				lvns->select = False;
		} else if (lvns->cancel) {
			if (cancelok) {
				lvns->text_cursor_state = 0;
				break;
			} else
				lvns->cancel = False;
		} else if (lvns->motion) {
			if (check_pos(lvns, data))
				changed = True;
			lvns->motion = False;
		} else if (lvns->cursor_up) {
			if (lvns->text_cursor_state == 0) {
				lvns->text_cursor_state = data->select_num;
			} else if (lvns->text_cursor_state > 1) {
				lvns->text_cursor_state--;
			}
			changed = True;
			lvns->cursor_up = False;
		} else if (lvns->cursor_down) {
			if (lvns->text_cursor_state < data->select_num) {
				lvns->text_cursor_state++;
			}
			changed = True;
			lvns->cursor_down = False;
		} else if (lvns->number > 0) {
			if (lvns->number <= data->select_num) {
				lvns->text_cursor_state = lvns->number;
				changed = True;
				break;
			} else {
				lvns->number = 0;
			}
		} else if (lvns->demo_mode) {
			demo_cnt++;
			if (demo_cnt == 30) {
				lvns->text_cursor_state = data->demo_choice;
				changed = True;
			} else  if (demo_cnt > 120) { 
				break;
			}
		}
	}

	// 表示処理解除
	LvnsSetDispFunc(lvns, NULL, NULL);
	LvnsDispWindow(lvns);

	if (!lvns->text_cursor_state) {
		/* キャンセル */
		ret =  -1;
	} else if (data->menu) {
		ret = data->menu(lvns, lvns->text_cursor_state);
		if (ret == 0) 
			goto start;
	} else 
		ret = lvns->text_cursor_state;

	lvns->select        = False;
	lvns->cancel        = False;
	lvns->cursor_up     = False;
	lvns->cursor_down   = False;
	lvns->motion        = False;
	
	return ret;
}



