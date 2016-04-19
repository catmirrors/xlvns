/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: sin_effect2.c,v 1.2 2001/07/25 14:36:49 tf Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Lvns.h"

#define DEGMAX 400
#define RAD(a) (M_PI)*((a)%DEGMAX)/(DEGMAX/2)
#define A 200

/* 状態遷移 */
static void
SinEffectSetState2(Lvns *lvns)
{
    lvns->effect_back_state += 8;
    if (lvns->effect_back_state >= DEGMAX) 
        lvns->effect_back_state -= DEGMAX;
}    

static void
SinEffect2(Lvns *lvns)
{
    int i, p, dy;
	double prev_y = 0;
	double y      = 0;
	double len    = 0;

    lvnsimage_copy_palette(lvns->background, lvns->vram);
	prev_y = A * sin(RAD(lvns->effect_back_state));

	p = lvns->effect_back_state;
    for (i=0; i<400; i++) {
		int y2 = len/1.5-100;
		if (y2>=0 && y2<400) {
			lvnsimage_copy_area(lvns->background, 
								lvns->vram,
								0, y2, 640, 1, 0, i);
		} else {
			lvnsimage_clear_area(lvns->vram,
								 0, i, 640, 1);
		}

        if (++p >= DEGMAX)
            p -= DEGMAX;

		y  = A * sin(RAD(p));
		dy = y - prev_y;
		prev_y = y;

		len += sqrt(1+dy*dy);
    }
    LVNS->mergeCharacter(lvns);
}   

LvnsBackEffectInfo lvnsSinEffect2 = {
    SinEffectSetState2,
    SinEffect2
};


