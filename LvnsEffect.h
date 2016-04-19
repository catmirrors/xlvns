#ifndef __LvnsEffect_h_
#define __LvnsEffect_h_

#include "Lvns.h"

typedef enum {
    LVNS_EFFECT_NONE,
	LVNS_EFFECT_NO_EFFECT,         /* なにもしない */
    LVNS_EFFECT_NORMAL,            /* 単純表示 */
    LVNS_EFFECT_FADE_MASK,         /* マスクフェード */
    LVNS_EFFECT_WIPE_TTOB,         /* 上からワイプ */
    LVNS_EFFECT_WIPE_LTOR,         /* 左からワイプ */
    LVNS_EFFECT_FADE_PALETTE,      /* Palette フェード */
    LVNS_EFFECT_WIPE_MASK_LTOR,    /* 左からワイプ(マスク) */
    LVNS_EFFECT_FADE_SQUARE,       /* ひし形◆フェード */
    LVNS_EFFECT_WIPE_SQUARE_LTOR,  /* ひし形◆左ワイプ */
    LVNS_EFFECT_SLIDE_LTOR,        /* 左から縦スライド */
    LVNS_EFFECT_SLANTTILE,         /* 斜ずれ横モザイクスライド */
    LVNS_EFFECT_GURUGURU,          /* ぐるぐる */
    LVNS_EFFECT_VERTCOMPOSITION,   /* 縦方向ではさみこみ */
    LVNS_EFFECT_CIRCLE_SHRINK,     /* 中心方向にちじむ */
    LVNS_EFFECT_LEFT_SCROLL,       /* 左にスクロール */
    LVNS_EFFECT_TOP_SCROLL,        /* 上にスクロール */
    LVNS_EFFECT_RAND_RASTER,       /* ランダム行表示 */
    LVNS_EFFECT_BLOOD,             /* 血がだらだら   */
} LvnsEffectType;                  /* エフェクト種別 */

/* LvnsEffect.c */
void LvnsClearLow(Lvns *lvns, LvnsEffectType effect_clear);
void LvnsDispLow(Lvns *lvns, LvnsEffectType effect_disp);
const char* LvnsEffectName(LvnsEffectType type);

void LvnsClear(Lvns *lvns, LvnsEffectType effect_clear);
void LvnsDisp(Lvns *lvns, LvnsEffectType effect_disp);

void LvnsLighten(Lvns *lvns);
void LvnsDarken(Lvns *lvns);
void LvnsVibrato(Lvns *lvns);
void LvnsWhiteOut(Lvns *lvns);
void LvnsWhiteIn(Lvns *lvns);


#endif
