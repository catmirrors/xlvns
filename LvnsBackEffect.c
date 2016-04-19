/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: LvnsBackEffect.c,v 1.4 2001/07/25 14:36:48 tf Exp $
 */

/*
 * Lvns 背景特殊エフェクト処理
 * 将来的にはモジュール化
 *
 * 定期的に背景で自動的におこなわれるエフェクトを処理する。
 *
 * 仮想 VRAM(lvns->vram)に背景(lvns->background)をコピーする
 * 際にエフェクトを加えることができる。
 * 背景処理後のキャラクタの合成は lvns->mergeCharacter
 * (lvns)が利用できる
 */

#include <stdio.h>
#include <stdlib.h>
#include "Lvns.h"

/*
 * エフェクト処理実行(状態設定)
 */
void
LvnsBackEffectSetState(Lvns *lvns)
{
    if (lvns->effect_back) 
        lvns->effect_back->set(lvns);
}

/*
 * エフェクト処理実行(描画準備)
 */
void
LvnsBackEffect(Lvns *lvns)
{
    if (lvns->effect_back) {
        lvns->effect_back->func(lvns);
    } else {
		lvnsimage_copy(lvns->background, lvns->vram);
        LVNS->mergeCharacter(lvns);
    }
}

/*
 * 背景エフェクトの種類の指定
 */
void
LvnsSetBackEffect(Lvns *lvns, LvnsBackEffectInfo *info)
{
    lvns->effect_back = info;
    lvns->effect_back_state = 0;
}

/*
 * 次の表示タイミングからの背景エフェクトの種類の指定
 */
void
LvnsSetNextBackEffect(Lvns *lvns, LvnsBackEffectInfo *info)
{
    lvns->effect_back_next = info;
}
