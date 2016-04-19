/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: sizuku_file.c,v 1.16 2001/08/05 10:58:10 go Exp $
 *
 */

/* 
 * 雫 しおり処理
 */

#include <stdio.h>
#include "sizuku.h"

/**
 * ファイル名の決定
 */
static void
SizukuFileName(Lvns *lvns, char *dst, size_t len, int no)
{
    snprintf(dst, len, "%s/sizuku%d.dat", lvns->savedata_path, no);
}


#define PUTLONG(d) putc(((d) >> 24) & 0xff, fp); putc(((d) >> 16) & 0xff, fp); putc(((d) >> 8) & 0xff, fp); putc(((d) >> 0) & 0xff, fp)


/**
 * 現在のしおりへの状態の保存
 */
void
SizukuSave(Lvns *lvns)
{
    char name[FILENAME_MAX];
    FILE *fp;
    int i;
    SizukuState *state = (SizukuState *)lvns->system_state;

    SizukuFileName(lvns, name, sizeof name, state->siori_no);
    dprintf((stderr, "save: %s\n", name));

    if ((fp = fopen(name, "w")) == NULL) {
	perror("SizukuSave");
	return;
    }

    /* シナリオ番号    */
    putc(lvns->savepoint.scn, fp);

    /* ブロック番号 */  
    putc(lvns->savepoint.blk, fp);

    /* シナリオのオフセット */
    PUTLONG(lvns->savepoint.scn_offset);

    /* 画像状態 */
    putc(lvns->savepoint.bg_type, fp);
    putc(lvns->savepoint.bg_no, fp);
    putc(0, fp); /* BG PAL */
    putc(lvns->savepoint.character_no[0], fp);
    putc(lvns->savepoint.character_no[1], fp);
    putc(lvns->savepoint.character_no[2], fp);

    /* 未見フラグ */
    for (i=0; i<SIZUKU_SCN_NO;i++) {
	putc(state->seen_flag[i], fp);
    }

    /* 制御フラグ */
    for (i=0; i<SIZUKU_FLAG_NO;i++) {
	putc(state->flag_save[i], fp);
    }

    /* BGM の状態 */
    putc(lvns->savepoint.current_music, fp);

    fclose(fp);
}

/**
 * 現在のしおりからの状態のロード
 */
void
SizukuLoad(Lvns *lvns)
{
    char name[FILENAME_MAX];
    FILE *fp;
    int i;
    SizukuState *state = (SizukuState *)lvns->system_state;

    SizukuFileName(lvns, name, sizeof name, state->siori_no);
    dprintf((stderr, "load: %s\n", name));

    if ((fp = fopen(name, "r")) == NULL) {
        SizukuSioriInit(lvns);
	return;
    }

    /* 現在のシナリオ番号    */
    lvns->savepoint.scn = fgetc(fp);

    /* 現在のブロック番号    */
    lvns->savepoint.blk = fgetc(fp);

    /* シナリオオフセット */
    {
        int a,b,c,d;
        a = getc(fp);
        b = getc(fp);
        c = getc(fp);
        d = getc(fp);
        lvns->savepoint.scn_offset = (a <<24) + (b<<16) + (c<<8) + d;
    }

    /* 画像状態 */
    lvns->savepoint.bg_type    = getc(fp);
    lvns->savepoint.bg_no = getc(fp);
    getc(fp); /* BG PAL */
    lvns->savepoint.character_no[0] = getc(fp);
    lvns->savepoint.character_no[1] = getc(fp);
    lvns->savepoint.character_no[2] = getc(fp);

    /* 未見フラグ */
    for (i=0; i<SIZUKU_SCN_NO;i++) {
	state->seen_flag[i] = getc(fp);
    }

    /* 制御フラグ */
    for (i=0; i<SIZUKU_FLAG_NO;i++) {
	state->flag_save[i] = getc(fp);
    }

    /* BGM の状態 */
    lvns->savepoint.current_music = getc(fp);

    fclose(fp);
}

