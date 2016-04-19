/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: LvnsCoreP.h,v 1.54 2001/07/18 20:39:15 go Exp $
 *
 */

#ifndef __LvnsCoreP_h
#define __LvnsCoreP_h

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/CoreP.h>

#include <sys/time.h>

#include "LvnsCore.h"
#include "Lvns.h"

#include "sximage.h"
#include "cdinfo.h"

typedef struct {
	XtPointer extension;
} LvnsCoreClassPart;

typedef struct _LayerCoreClassRec {
    CoreClassPart   core_class;
    LvnsCoreClassPart lvnscore_class;
} LvnsCoreClassRec;

extern LvnsCoreClassRec lvnsCoreClassRec;

#ifndef CDDEVICE
#define CDDEVICE "/dev/cd0a"
#endif
#ifndef AUDIODEVICE
#define AUDIODEVICE "/dev/audio"
#endif

typedef struct _ImageCorePart {

    Lvns *info;         /* 基本情報 */

    /* ----------------------------------------------------------- */
    SuperXImage *sximage;     /* イメージ表示作業用 ximage + pixmap */
    int xoff, yoff;

    /* ---------------------------------------------------------- */
    /* CD-DA 演奏用データ */
    CDInfo *cdinfo;
	Music music;

    /* -------------------------------------------------------- */
    /* サウンド再生用 */
    XtInputId sound_ctl_id;     /* X入力処理用      */
    int sound_ctl_fd;           /* プロセス間通信用 */
	Sound sound;

    /* -------------------------------------------------------- */
    /* リソース指定用 */
    int  type;                    /* シナリオエンジン種別 */
    Bool            root_mode;    /* ルートウィンドウに表示する */
    Bool            seen_mode;    /* 全文既読とみなすモード(for DEBUG) */
    Bool            demo_mode;    /* オートデモモード */
    int         latitude_dark;    /* 画面が暗い時の輝度の指定          */
    Bool        key_click_fast;   /* 文字表示中断 */
#ifdef DEBUG
	int         scenario_number;  /* シナリオ番号指定用 */
#endif
    /* -------------------------------------------------------- */
    /* 高精度タイマ用 */
    struct timeval timer[10];
    Bool noshared;

    /* -------------------------------------------------------- */
	/* Flip 処理用 */
	long wait_clock;
	long wait_count;

} LvnsCorePart;

typedef struct _LvnsCoreRec {
    CorePart        core;
    LvnsCorePart    lvnscore;
} LvnsCoreRec;

/* convinient defines */

#define COREWIDTH  (lcw->core.width)
#define COREHEIGHT (lcw->core.height)
#define LCW        (lcw->lvnscore)

/* semi public functions */
void LvnsCoreInitMusic(LvnsCoreWidget lcw);
void LvnsCoreInitSound(LvnsCoreWidget lcw);

#endif
