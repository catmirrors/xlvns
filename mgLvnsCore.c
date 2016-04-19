/*
 * LEAF Visual Novel System For MGL2
 * (c) Copyright 2001 TF mailto:tf@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: mgLvnsCore.c,v 1.7 2001/08/05 15:43:47 tf Exp $
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "Lvns.h"
#include "mgLvnsCore.h"

/*
 * 描画シンクロ待ち処理
 */

#define TICKSPERSEC 1000

static void SetPaletteMulti(Lvns *, int);

LvnsCoreWidget lcw;

static int offscreen[76800];

u_long 
getCurrentTime(void)
{
    struct timeval current;
    gettimeofday(&current, NULL);
    return current.tv_sec * 1000 + current.tv_usec / 1000;
}

/**
 * 矩形を描画する
 * @param x,y,w,h 位置とサイズ
 * @param 色の HSB 値
 */
static void
DrawBox(Lvns *lvns, int x, int y, int w, int h, int color)
{
  mglimage_box(lvns->vram, x/2, y/2, w/2, h/2, color);
}

#include <sys/types.h>
#include <sys/stat.h>

/** 
 * 指定したファイルの更新日付を取得する
 */
static time_t
GetFileTime(Lvns *lvns, const char *path)
{
    struct stat sb;
    if (stat(path, &sb) < 0) {
        perror(path);
        return 0;
    }
    return sb.st_mtime;
}

static void
Flip(Lvns *lvns)
{
	// タイミング補正処理
		int wait, sa;
		wait = LCW.wait_count / 60;
		if (wait > 0) {
			u_long tw;
			sa = getCurrentTime() - LCW.wait_clock;
			if (sa > 0) wait -= sa;
#ifndef USE_MGL
			if (wait > 0) usleep(wait);
#else
			if (wait > 0) {
			  MglGetEvent(lvns, wait);
			} else {
			  MglGetEvent(lvns, 5);
			}
#endif
			tw = getCurrentTime();
			sa = tw - LCW.wait_clock;
			if (sa > 0) 
				LCW.wait_count -= sa * 60;
			else 
				LCW.wait_count = 0;
			LCW.wait_clock = tw;
		}
		LCW.wait_count += TICKSPERSEC;
		if (LCW.wait_count < 0) {
			LCW.wait_count = 0;
		}
}

static void
FlushWindow(Lvns *lvns)
{

}

static void
FlushWindowArea(Lvns *lvns, int x, int y, int w, int h)
{

}

static void
DrawWindow(Lvns *lvns)
{
    if (!lvns->vram->is_put) {
      SetPaletteMulti(lvns, lvns->latitude);
      mglimage_draw_area(lvns->vram, offscreen, 0, 0, WIDTH, HEIGHT, 0, 0);
      mglimage_put_pixstream_rect(0, 0, offscreen, WIDTH * HEIGHT, DIR_NORTH, WIDTH);
    }
    lvns->vram->is_put = False;
}

static void
DrawWindowArea(Lvns *lvns, int x, int y, int w, int h, int x2, int y2)
{
    int i;

    if (x < 0 || y < 0 || x > WIDTH || y > HEIGHT) {
      return;
    }
    if (x + w > WIDTH) {
      w = WIDTH - x;
    }
    if (y + h > HEIGHT) {
      h = HEIGHT - y;
    }

    if (!lvns->vram->is_put) {
      SetPaletteMulti(lvns, lvns->latitude);
      mglimage_draw_area(lvns->vram, offscreen, x, y, w, h, x2, y2);
      for (i = 0; i < h; i++) {
	mglimage_put_pixstream_rect(x, y + i, offscreen + WIDTH * (y + i) + x, w, DIR_NORTH, w);
      }
    }
    lvns->vram->is_put = False;
}

static void
ClearWindow(Lvns *lvns)
{
  lvnsimage_clear(lvns->vram);
  DrawWindow(lvns);
  FlushWindow(lvns);
}

static void
ClearWindowArea(Lvns *lvns, int x, int y, int w, int h)
{
  lvnsimage_clear_area(lvns->vram, x, y, w, h);
  DrawWindowArea(lvns, x, y, w, h, x, y);
  FlushWindowArea(lvns, x, y, w, h);
}

static void
SetPaletteIndex(Lvns *lvns, int index, int r, int g, int b)
{
  // dprintf((stderr, "SetPaletteIndex (not implemented).\n"));
}

static void
SetPalette(Lvns *lvns)
{
  mglimage_set_pal_to_image(lvns->vram, offscreen);
}

static void
SetPaletteMulti(Lvns *lvns, int par16)
{
  lvns->latitude = par16;
  mglimage_set_multiple(lvns->vram, offscreen, par16);
}

static void
SetPaletteScreen(Lvns *lvns, int par16)
{
  mglimage_set_multiple(lvns->vram, offscreen, lvns->latitude);
  mglimage_set_whiteout(lvns->vram, offscreen, par16);
}

static void
DrawWindowMask(Lvns *lvns, int x, int y, int state)
{
  mglimage_copy_mask_unit_to_sximage(lvns->vram, x, y, x, y, state);
  lvns->vram->is_put = True;
}

static void
DrawWindowSquareMask(Lvns *lvns, int x, int y, int state)
{
  mglimage_copy_squaremask_unit_to_sximage(lvns->vram, x, y, x, y, state);
  lvns->vram->is_put = False;
}

static void
SetDispOffset(Lvns *lvns, int xoff, int yoff, int maxoff)
{
  int w, h, x0, y0, x1, y1;

  if (xoff == 0) {
    w = WIDTH;
    x0 = 0;
    x1 = 0;
  } else if (xoff < 0) {
    xoff = -maxoff * 2 - xoff;
    w = WIDTH + xoff;
    x0 = -xoff;
    x1 = 0;
  } else {
    xoff = maxoff * 2 - xoff;
    w = WIDTH - xoff;
    x0 = 0;
    x1 = xoff;
  }
  if (yoff == 0) {
    h = HEIGHT;
    y0 = 0;
    y1 = 0;
  } else if (yoff < 0) {
    yoff = -maxoff * 2 - yoff;
    h = HEIGHT + yoff;
    y0 = -yoff;
    y1 = 0;
  } else {
    yoff = maxoff * 2 - yoff;
    h = HEIGHT - yoff;
    y0 = 0;
    y1 = yoff;
  }
  mglimage_copy_area_win2(lvns->vram, offscreen, x0, y0, w, h, x1, y1);
  lvns->vram->is_put = True;
}

static void
PutPattern(Lvns *lvns, int x, int y, int index, u_char *data)
{
  dprintf((stderr, "PutPattern (not implemented).\n"));
}

static void
PutPattern2(Lvns *lvns, int x, int y, int index, u_char *data)
{
  dprintf((stderr, "PutPattern2 (not implemented).\n"));
}

/* 
 * タイマをリセットする
 * @param no タイマ番号
 */
static void
ResetTimer(Lvns *lvns, int no)
{
    gettimeofday(&LCW.timer[no], NULL);
}

/*
 * タイマ値を取得する
 * @param no タイマ番号
 * @return タイマ値 (msec単位)
 */
static long 
GetTimer(Lvns *lvns, int no)
{
    struct timeval current;
    gettimeofday(&current, NULL);
    return (current.tv_sec - LCW.timer[no].tv_sec) * 1000 +
        (current.tv_usec - LCW.timer[no].tv_usec) / 1000;
}


/*
 * LVNS 情報の接続
 */ 
void
mgLvnsCoreSetLvns(Lvns *lvns)
{
    lcw = (LvnsCoreRec *)malloc(sizeof(LvnsCoreRec));

    LCW.info = lvns;

    LCW.wait_clock = getCurrentTime();
    LCW.wait_count = TICKSPERSEC;

    LvnsCoreInitMusic(lcw);
    //LvnsCoreInitSound(lcw);

    /* SE 関連初期化設定 */
    //lvns->sound = &LCW.sound;
    
    /* BGM関連初期化 */
    lvns->music = &LCW.music;

    /* システム依存関数群の設定 */
    lvns->flushWindow          = FlushWindow;
    lvns->flushWindowArea      = FlushWindowArea;
    lvns->drawWindow           = DrawWindow;
    lvns->drawWindowArea       = DrawWindowArea;
    lvns->clearWindow          = ClearWindow;
    lvns->clearWindowArea      = ClearWindowArea;
    lvns->setPaletteIndex      = SetPaletteIndex;
    lvns->setPalette           = SetPalette;
    lvns->setPaletteMulti      = SetPaletteMulti;
    lvns->setPaletteScreen     = SetPaletteScreen;
    lvns->drawWindowMask       = DrawWindowMask;
    lvns->drawWindowSquareMask = DrawWindowSquareMask;
    lvns->setDispOffset        = SetDispOffset;
    lvns->putPattern           = PutPattern;
    lvns->putPattern2          = PutPattern2;
    lvns->resetTimer           = ResetTimer;
    lvns->getTimer             = GetTimer;
    lvns->drawBox              = DrawBox;
    lvns->getFileTime          = GetFileTime;
    lvns->flip                 = Flip;

    memset(offscreen, 0, 320 * 240 * sizeof(int));
}
