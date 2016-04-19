/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: LvnsFile.c,v 1.9 2001/08/11 19:59:36 tf Exp $
 *
 */

/*
 * Lvns ファイル処理まわり
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Lvns.h"

/* 単純データロード */
u_char *
LvnsLoadData(Lvns *lvns, const char *name, size_t *size)
{
    int index;

    if ((index  = leafpack_find(lvns->leafpack, name)) < 0)
        return NULL;

    return leafpack_extract(lvns->leafpack, index, size);
}

/*
 * 画像のロード 
 */
LvnsImage *
LvnsLoadImage(Lvns *lvns, const char *name, LvnsImage *over)
{
    int index;
    u_char *data;
    LvnsImage *ret;

    if ((index  = leafpack_find(lvns->leafpack, name)) < 0) {
        fprintf(stderr, "can't find image:%s\n", name);
        return NULL;
    }

    if ((data = leafpack_extract(lvns->leafpack, index, NULL)) == NULL) {
        fprintf(stderr, "can't extract image:%s\n", name);
        return NULL;
    }

    /* 拡張子判定... */
    {
        char *p = strchr(name, '.') + 1;
        if (strcasecmp(p, "lfg") == 0) {
            ret = lvnsimage_load_lfg(data, over);
        } else if (strcasecmp(p, "lf2") == 0) {
            ret = lvnsimage_load_lf2(data, over);
#ifdef USE_MGL
	} else if (strcasecmp(p, "sdt") == 0) {
	    ret = mglimage_load_lfg(data, NULL);
#endif
        } else {
            fprintf(stderr, "unknown format: %s", name);
            ret = NULL;
        }
    }

    free(data);

    return ret;
}

#define GET_SHORT(p) (((p)[1]<<8)|(p)[0])
#define GET_LONG(p) ((p)[3]<<24|(p)[2]<<16|(p)[1]<<8|(p)[0])

/* シナリオデータのよみこみ */
void
LvnsLoadScenario(Lvns *lvns, int scn, int blk)
{
    int index;
    char name[100];
    u_char *data;

    /* シナリオファイル名決定 */
    sprintf(name, LVNS->scn_name, scn);

    dprintf((stderr, "scenario: %s %d\n", name, blk));

    if (scn != lvns->scn_current || blk != lvns->blk_current) {

        if ((index  = leafpack_find(lvns->scnpack, name)) < 0 ||
            (data = leafpack_extract(lvns->scnpack, index, NULL)) == NULL) {
            fprintf(stderr, "can't load scenario\n");
            exit(1);
        }

        /* 展開 */
        {
            u_char *p_scn  = data + GET_SHORT(data) * 16;
            u_char *p_text = data + GET_SHORT(data+2) * 16;
            size_t size_scn  = GET_LONG(p_scn);
            size_t size_text = GET_LONG(p_text);

#if 0
            fprintf(stderr, "size_scn: %d\n", size_scn);
            fprintf(stderr, "size_text: %d\n", size_text);
#endif

            lvns->scn_data = realloc(lvns->scn_data, size_scn);
            lvns->scn_text = realloc(lvns->scn_text, size_text);

            /* lzs 展開 */
            leafpack_lzs2(p_scn  + 4, lvns->scn_data, size_scn);
            leafpack_lzs2(p_text + 4, lvns->scn_text, size_text);
        }

/*
 nn nn 個数
 aa aa 00index
 ....  01index
 00データ ....

 シナリオデータはたぶん全部 01 にはいっている…
*/
        lvns->scn_current = scn;
        lvns->blk_current = blk;

        free(data);
    }

    lvns->scn_cur_head = 
    lvns->scn_cur = lvns->scn_data + GET_SHORT(lvns->scn_data + (blk+1) * 2);
    lvns->savepoint_flag = True;
}

void
LvnsLoadScenarioBlock(Lvns *lvns, int blk)
{
    lvns->scn_cur = lvns->scn_data + GET_SHORT(lvns->scn_data + (blk+1) * 2);
}

/* テキスト設定 */
u_char *
LvnsGetScenarioText(Lvns *lvns, int no)
{
#if 0
    int num = GET_SHORT(lvns->snc_text);
    if (no > num) {
        fprintf(stderr, "warn: invalid message number");
        no = 0;
    }
#endif
    return lvns->scn_text + GET_SHORT(lvns->scn_text + (no+1) * 2);
}

/*
 * 背景画像を設定する(番号指定)
 */
void
LvnsLoadBackground(Lvns *lvns, const char *basename, int no)
{
    char name[20];
    sprintf(name, basename, no);

    if (lvns->sepia_mode) {
      lvns->sepia_mode = False;
    }
    if (lvns->sepia_mode_next) {
      lvns->sepia_mode_next = False;
      lvns->sepia_mode = True;
    }
    (void)LvnsLoadImage(lvns, name, lvns->background);
}
