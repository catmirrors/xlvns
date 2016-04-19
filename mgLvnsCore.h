/*
 * LEAF Visual Novel System For MGL2
 * (c) Copyright 2001 TF mailto:tf@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: mgLvnsCore.h,v 1.1 2001/08/06 13:56:58 tf Exp $
 *
 */
#ifndef __INCLUDE_MGLVNSCORE_H
#define __INCLUDE_MGLVNSCORE_H

#include <sys/time.h>

#include "Lvns.h"
#include "cdinfo.h"

#ifndef CDDEVICE
#define CDDEVICE "/dev/hdc"
#endif
#ifndef AUDIODEVICE
#define AUDIODEVICE "/dev/dsp"
#endif

typedef struct {

    Lvns *info;         /* 基本情報 */

    /* ---------------------------------------------------------- */
    /* CD-DA 演奏用データ */
    CDInfo *cdinfo;
	Music music;

    /* -------------------------------------------------------- */
    /* 高精度タイマ用 */
    struct timeval timer[10];
    Bool noshared;

    /* -------------------------------------------------------- */
	/* Flip 処理用 */
	long wait_clock;
	long wait_count;

} LvnsCorePart;

typedef struct {
  LvnsCorePart lvnscore;
} LvnsCoreRec;

typedef LvnsCoreRec *LvnsCoreWidget;

#define LCW (lcw->lvnscore)

void LvnsCoreInitMusic(LvnsCoreWidget lcw);

#endif
