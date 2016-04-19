#ifndef __LvnsAnim_h_
#define __LvnsAnim_h_

#include "Lvns.h"

typedef struct {
    enum {
        LVNS_ANIM_NONE,
        LVNS_ANIM_IMAGE,
        LVNS_ANIM_IMAGE2,
        LVNS_ANIM_IMAGE_ADD,
        LVNS_ANIM_IMAGE_ADD2,
        LVNS_ANIM_DISP,
        LVNS_ANIM_SOUND,
        LVNS_ANIM_WAIT,
    } type;
    const char *name;
    int time;
    int x;
    int y;
    LvnsImage *image;
} LvnsAnimationData;    /* 単純アニメ処理用 */

void LvnsInitAnimation(Lvns *lvns, LvnsAnimationData *data);
void LvnsAnimation(Lvns *lvns, LvnsAnimationData *data);

#endif
