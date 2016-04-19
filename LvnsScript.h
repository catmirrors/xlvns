#ifndef __LvnsScript_h_
#define __LvnsScript_h_

#include "Lvns.h"

typedef struct {
    enum {
        LVNS_SCRIPT_BG,                 /* 背景ロード       */
        LVNS_SCRIPT_DISP,               /* 画像表示(BGから) */
        LVNS_SCRIPT_DISP_VRAM,          /* 画像表示         */
        LVNS_SCRIPT_CLEAR,              /* 画像消去         */               
        LVNS_SCRIPT_MUSIC,              /* 音楽再生         */
        LVNS_SCRIPT_MUSIC_FADE,
        LVNS_SCRIPT_WAIT_MUSIC,         /* 音楽終了待ち     */
        LVNS_SCRIPT_ANIM,
        LVNS_SCRIPT_WAIT_CLICK,
        LVNS_SCRIPT_WAIT,               /* 指定時間待ち         */
        LVNS_SCRIPT_TIMER_INIT,         /* 待ち時間タイマ初期化 */
        LVNS_SCRIPT_TIMER_WAIT,         /* 指定時間経過まで待つ */
        LVNS_SCRIPT_FUNC,               /* 関数呼び出し         */
        LVNS_SCRIPT_LOOP_FUNC,        /* 関数呼び出し         */
        LVNS_SCRIPT_CLICK_JUMP,
		LVNS_SCRIPT_WHITEOUT,
		LVNS_SCRIPT_WHITEIN,
        LVNS_SCRIPT_EXIT,
        LVNS_SCRIPT_END
    } type;
    void *data0;
    void *data1;
    void *data2;
} LvnsScriptData;       /* OP/ED スクリプト処理用 */

typedef struct LvnsScript {
    LvnsScriptData *data;       /* スクリプトデータ       */
    int cur;                    /* 実行カーソル           */
    int state;                  /* ステート               */
} LvnsScript;

typedef int LvnsScriptFunc(struct Lvns *lvns, LvnsScript *scr, void *param1, void *param2);

/* LvnsScript.c OP/ED 用 スクリプト処理 */
void LvnsScriptRun(Lvns *lvns, LvnsScriptData *data);

#endif
