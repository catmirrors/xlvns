/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: LvnsCoreMusic.c,v 1.21 2001/08/05 15:43:47 tf Exp $
 *
 */

/*
 * LvnsCore 音楽処理回り
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef USE_MGL
#include "LvnsCoreP.h"
#else
#include "mgLvnsCore.h"
#endif

/* ボリューム設定 */
static void
SetMusicVolume(LvnsCoreWidget lcw, int no)
{
    cdinfo_set_volume(LCW.cdinfo, no);
}

/* BGM 演奏開始 */
static void
StartMusic(LvnsCoreWidget lcw, int no)
{
    cdinfo_play(LCW.cdinfo, no);
}

static void
StopMusic(LvnsCoreWidget lcw)
{
    cdinfo_stop(LCW.cdinfo);
}

/* BGM 一時停止 */
static void
PauseMusic(LvnsCoreWidget lcw)
{
    cdinfo_pause(LCW.cdinfo);
}

static int
GetMusicState(LvnsCoreWidget lcw)
{
    CDTimeInfo cur;
    return cdinfo_get_current_time(LCW.cdinfo, &cur);
}

static void
CloseMusic(LvnsCoreWidget lcw)
{
    cdinfo_delete(LCW.cdinfo);
	LCW.cdinfo = NULL;
}

static void
OpenMusic(LvnsCoreWidget lcw)
{
    char *disc = getenv("CDDEVICE");
    if (!disc) 
        disc = CDDEVICE;
    if ((LCW.cdinfo = cdinfo_new(disc)) == NULL) {
        perror(disc);
    } 
}

void 
LvnsCoreInitMusic(LvnsCoreWidget lcw)
{
	LCW.music.depend     = lcw;
	LCW.music.open       = OpenMusic;
	LCW.music.close      = CloseMusic;
	LCW.music.start      = StartMusic;
	LCW.music.stop       = StopMusic;
	LCW.music.pause      = PauseMusic;
	LCW.music.setVolume  = SetMusicVolume;
	LCW.music.getState   = GetMusicState;

	LCW.cdinfo = NULL;
}

