/*
 * LEAF Visual Novel System For MGL2 - Font Plus Extension
 * (c) Copyright 2001 Hiroshi Kuwagata <kgt@topaz.ocn.ne.jp>
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: mgFont.h,v 1.3 2001/08/12 11:35:44 tf Exp $
 *
 */
/*
 * vi:ts=4:sw=4;
 */
#include <stdio.h>
#include "Lvns.h"

#ifdef FONT_PLUS
#ifndef __MGFONT_H__
#define __MGFONT_H__

typedef struct {
//	int	width;
//	int	height;

	int* draw_buf;
	u_char*	font_data;
} LvnsFont; 

LvnsFont* lvnsfont_new( const u_char* dat);
LvnsFont* lvnsfont_load( const u_char* fname);
void lvnsfont_discard( LvnsFont* fnt);
void lvnsfont_draw( LvnsFont* fnt, int x, int y, int code, int* cmap);
#endif /* __MGFONT_H__ */
#endif /* FONT_PLUS */
