/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: LvnsScript.c,v 1.15 2001/08/05 10:58:09 go Exp $
 *
 */

/*
 * Lvns スクリプト処理
 */

#include <stdio.h>
#include "LvnsScript.h"
#include "LvnsEffect.h"
#include "LvnsAnim.h"

/**
 * スクリプト処理を実行する
 */
static int
ScriptStep(Lvns *lvns, LvnsScript *scr)
{
	LvnsScriptData *script;

	if (lvns->select) {
		int n = scr->cur;
		LvnsScriptData *script = scr->data + n;
		while (script->type !=  LVNS_SCRIPT_END) {
			if (script->type == LVNS_SCRIPT_CLICK_JUMP) {
				scr->cur = n;
				break;
			}
			script++;
			n++;
		}
	}
	
	script = scr->data + scr->cur;
	
	switch (script->type) {
		
	case LVNS_SCRIPT_BG:
		LVNS->loadBG(lvns, (int)script->data0);
		break;
		
	case LVNS_SCRIPT_DISP:
		lvnsimage_copy(lvns->background, lvns->vram);
		LVNS->mergeCharacter(lvns);
		LvnsDispLow(lvns, (LvnsEffectType)script->data0);
		LvnsWait(lvns, (int)script->data1 * INTERVAL / 1000);
		break;
		
	case LVNS_SCRIPT_DISP_VRAM:
		LvnsDispLow(lvns, (LvnsEffectType)script->data0);
		LvnsWait(lvns, (int)script->data1 * INTERVAL / 1000);
		break;
		
	case LVNS_SCRIPT_CLEAR:
		LvnsClearText(lvns);
		LvnsClearLow(lvns, (LvnsEffectType)script->data0);
		break;
		
	case LVNS_SCRIPT_MUSIC:
		LvnsStartMusic(lvns, (int)script->data0);
		break;
		
	case LVNS_SCRIPT_MUSIC_FADE:
		LvnsFadeMusic(lvns);
		break;
		
	case LVNS_SCRIPT_WAIT_MUSIC:
		LvnsWaitMusic(lvns);
		break;
		
	case LVNS_SCRIPT_ANIM:
		LvnsAnimation(lvns, script->data0);
		break;
		
	case LVNS_SCRIPT_WAIT_CLICK:
		LvnsWaitClick(lvns);
		break;
		
	case LVNS_SCRIPT_WAIT:
		LvnsWait(lvns, (int)script->data0 * INTERVAL / 1000);
		break;
		
	case LVNS_SCRIPT_TIMER_INIT:
		lvns->resetTimer(lvns, LVNSTIMER_SCRIPT);
		break;
		
	case LVNS_SCRIPT_TIMER_WAIT:
		while (lvns->getTimer(lvns, LVNSTIMER_SCRIPT) < (int)script->data0) {
			LvnsFlip(lvns, True);
		}
		break;
		
	case LVNS_SCRIPT_LOOP_FUNC:
		scr->state = 0;
		while (!((LvnsScriptFunc*)script->data0)(lvns, scr, script->data1, script->data2)) {
			LvnsFlip(lvns, True);
		}
		LvnsFlip(lvns, True);
		break;

	case LVNS_SCRIPT_FUNC:
		((LvnsScriptFunc*)script->data0)(lvns, scr, script->data1, script->data2);
		break;
		
	case LVNS_SCRIPT_CLICK_JUMP:
		break;

	case LVNS_SCRIPT_WHITEOUT:
		LvnsWhiteOut(lvns);
		break;
		
	case LVNS_SCRIPT_WHITEIN:
		LvnsWhiteIn(lvns);
		break;
		
	case LVNS_SCRIPT_EXIT:
	case LVNS_SCRIPT_END:
		return True;
	}
	scr->cur++;
	
    return False;
}

void
LvnsScriptRun(Lvns *lvns, LvnsScriptData *data)
{
	LvnsScript dat;
	dat.data  = data;
	dat.cur   = 0;
	dat.state = 0;
	lvns->select = False;
	while (!ScriptStep(lvns, &dat));
}
