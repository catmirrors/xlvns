/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: LvnsSound.c,v 1.7 2001/08/02 20:05:30 go Exp $
 *
 */

/*
 * Lvns サウンド処理回り
 */

#include <stdio.h>
#include "Lvns.h"

/*
 * データ読み込み 
 */
void
LvnsLoadSound2(Lvns *lvns, const char* name)
{
    LvnsStopSound(lvns);
	SoundLoad(lvns->sound, name);
}

/*
 * データ読み込み
 */
void
LvnsLoadSound(Lvns *lvns, const char* basename, int no)
{
    char name[100];
    snprintf(name, sizeof name, basename, no);
    LvnsLoadSound2(lvns, name);
}

/*
 * 演奏完了(データを破棄)
 */
void
LvnsStopSound(Lvns *lvns)
{
	SoundStop(lvns->sound);
    lvns->sound_loop = False;
    lvns->sound_count = 0;
}

/*
 * 演奏開始
 */
void
LvnsStartSound(Lvns *lvns, int sound_count)
{
    if (sound_count == 0) {
		lvns->sound_loop = True;
    }
    lvns->sound_count = sound_count;
	SoundStart(lvns->sound);
}

/* 終了待ち処理 */
void
LvnsWaitSound(Lvns *lvns)
{
	if (lvns->sound == NULL || lvns->sound->getState == NULL)
		return;

	lvns->select = False;
	while (SoundGetState(lvns->sound) == True && !lvns->select) {
		LvnsFlip(lvns, True);
	}
	lvns->sound_loop = False;
	lvns->sound_count = 0;
}

/* ループ処理 */
void
LvnsLoopSound(Lvns *lvns)
{
    if (SoundGetState(lvns->sound)) {
		if (lvns->sound_loop || lvns->sound_count > 1) {
			if (lvns->sound_loop) {
				LvnsStartSound(lvns, 0);
			} else {
				LvnsStartSound(lvns, --lvns->sound_count);
			}
		}
    } 
}

