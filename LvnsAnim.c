/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: LvnsAnim.c,v 1.9 2001/08/03 01:55:25 go Exp $
 *
 */

/*
 * Lvns ぱらぱらアニメ処理
 */

#include <stdio.h>
#include <stdlib.h>
#include "LvnsAnim.h"

/*
 * アニメーションデータの初期化
 */
void
LvnsInitAnimation(Lvns *lvns, LvnsAnimationData *data) 
{
    LvnsAnimationData *p = data;
    while (p->type != LVNS_ANIM_NONE) {
        if (p->image == NULL && 
            (p->type == LVNS_ANIM_IMAGE ||
             p->type == LVNS_ANIM_IMAGE2 ||
             p->type == LVNS_ANIM_IMAGE_ADD)) {
            if (p->name) {
                p->image = LvnsLoadImage(lvns, p->name, NULL);
            }
        }
        p++;
    }
}

/*
 * アニメーション再生処理
 */
static Bool
Animation(Lvns *lvns, LvnsAnimationData **data, int *wait_time)
{
	LvnsAnimationData *anim_data = *data;

 start:
    switch (anim_data->type) {
    case LVNS_ANIM_IMAGE:
		lvnsimage_clear(lvns->vram);
		
    case LVNS_ANIM_IMAGE_ADD:
		lvnsimage_add(anim_data->image, 
					  lvns->vram, anim_data->x, anim_data->y);
		lvns->setPalette(lvns);
		lvns->drawWindow(lvns);
		lvns->flushWindow(lvns);
		*wait_time = anim_data->time * INTERVAL / 1000;
        break;
		
    case LVNS_ANIM_IMAGE2:
		lvnsimage_clear(lvns->vram);
		lvnsimage_add(anim_data->image, 
					  lvns->vram, anim_data->x, anim_data->y);
		lvns->setPalette(lvns);
		anim_data++;
		goto start;
        break;
		
    case LVNS_ANIM_IMAGE_ADD2:
		lvnsimage_add2(anim_data->image, 
					   lvns->vram, anim_data->x, anim_data->y);
		anim_data++;
		goto start;
        break;
		
    case LVNS_ANIM_DISP:
		lvns->drawWindow(lvns);
		lvns->flushWindow(lvns);
        *wait_time = anim_data->time * INTERVAL / 1000;
        break;
		
	case LVNS_ANIM_SOUND:
        LvnsStartSound(lvns, 1);
        break;
		
    case LVNS_ANIM_NONE:
        anim_data = NULL;
        return True;
		
    case LVNS_ANIM_WAIT:
        *wait_time = anim_data->time * INTERVAL / 1000;
        break;
    } 

	(*data)++;
    return False;
}    

void
LvnsAnimation(Lvns *lvns, LvnsAnimationData *data)
{
	int wait_time = 0;
    lvns->latitude   = 16;
	while (wait_time-- > 0 || !Animation(lvns, &data, &wait_time)) {
		LvnsFlip(lvns, False);
		if (lvns->skip)
			wait_time = 0;
	}
}

