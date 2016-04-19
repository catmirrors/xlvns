/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: LvnsEtc.c,v 1.3 2001/08/05 10:58:09 go Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include "Lvns.h"

/**
 * セーブ位置情報初期化
 */
void
LvnsInitSavePoint(Lvns *lvns, struct SavePoint *sp)
{
#if 0
    dprintf((stderr, "InitSavePoint\n"));
#endif
    /* シナリオ位置初期化 */
    sp->scn = 0;
    sp->blk = 1;
    sp->scn_offset = 0;

    /* 画像/BGM 状態初期化 */
    sp->bg_type       = LVNS_BACKGROUND;
    sp->bg_no         = 0;
    sp->character_no[0] = NOCHARACTER;
    sp->character_no[1] = NOCHARACTER;
    sp->character_no[2] = NOCHARACTER;
    sp->current_music = 0;
}

/**
 * セーブ位置設定
 */
void
LvnsSetSavePoint(Lvns *lvns, struct SavePoint *sp)
{
#if 0
    dprintf((stderr, "SetSavePoint\n"));
#endif
    sp->bg_type       = lvns->bg_type;
    sp->bg_no         = lvns->bg_no;
    sp->character_no[0]  = lvns->character_no[0];
    sp->character_no[1]  = lvns->character_no[1];
    sp->character_no[2]  = lvns->character_no[2];
    sp->scn           = lvns->scn_current;
    sp->blk           = lvns->blk_current;
    sp->scn_offset    = lvns->scn_cur - lvns->scn_cur_head;
    sp->current_music = lvns->current_music;
}

