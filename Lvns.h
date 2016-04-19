/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: Lvns.h,v 1.25 2001/08/17 16:55:51 tf Exp $
 *
 */

#ifndef __Lvns_h
#define __Lvns_h

#include <sys/types.h>
#include <setjmp.h>

#ifdef DEBUG
#define dprintf(a) fprintf a
#else
#define dprintf(a)
#endif

#ifndef Bool
#define Bool int
#endif

#ifndef True
#define True 1
#endif

#ifndef False
#define False 0
#endif

#include "leafpack.h"   /* Leaf Pack File   */
#include "Sound.h"
#include "Music.h"
#ifndef USE_MGL
#include "lvnsimage.h"  /* Leaf Image Data  */
#else
#include "mgl2.h"
#include "mglcol.h"
#include "mgImage.h"
#ifdef FONT_PLUS
#include "mgFont.h"
#endif /* FONT_PLUS */
#endif

#ifndef USE_MGL
#define VERSION "LEAF Visual Novel System for X\n"\
                " ==== XLVNS 1-2-3 Ver 3.0beta ====\n"\
                "(c) Copyright 1999,2000 Go Watanabe\n"\
                "Original LVNS (c)LEAF/AQUAPLUS\n"
#else
#define VERSION "LEAF Visual Novel System for MGL2\n"\
                " ==== mglvns 1-2-3 Ver 1.0beta ====\n"\
                "(c) Copyright 1999-2001 Go Watanabe\n"\
                "(c) Copyright 2001 TF\n"\
                "Original LVNS (c)LEAF/AQUAPLUS\n"
#endif

#define INTERVAL 60
#define NOCHARACTER 255

/* EUC コードからパックした JIS コードへ… */
#define EucToJisPack(code)  ((((code>>8)&0x7f)-33)*94 + ((code&0x7f)-33))

#define LVNS     lvns->system_state
#define COLORS  (lvns->system_state->colors)
#define WIDTH	(lvns->system_state->width)
#define HEIGHT	(lvns->system_state->height)
#ifndef USE_MGL
#define WIDTH2       640
#define CHARSIZE     24
#define CHARDRAWSIZE 26
#define EDYOFF       30
#define XPOS(x,y)    ((x) * 24 + lvns->tvram[lvns->current_tvram].row[y].offset)
#define YPOS(y)      ((y) * 32 + 8)
#define R_XPOS(x, y) (((x) - lvns->tvram[lvns->current_tvram].row[y].offset)/24)
#define R_YPOS(y)    (((y) - 8) / 32)
#else
#define WIDTH2       320
#define CHARSIZE     12
#define CHARDRAWSIZE 14
#define EDYOFF       15
#define XPOS(x,y)    ((x) * 12 + lvns->tvram[lvns->current_tvram].row[y].offset)
#define YPOS(y)      ((y) * 14 + 4)
#define R_XPOS(x, y) (((x) - lvns->tvram[lvns->current_tvram].row[y].offset)/12)
#define R_YPOS(y)    (((y) - 4) / 14)
#endif

#ifdef USE_MGL
#define PATH_LEN 200

#ifndef MGLVNS_APPDIR
#define MGLVNS_APPDIR "/usr/local/lib/mglvns";
#endif

#define MGL_XOFFSET 80
#define MGL_YOFFSET 20

/* 一部の関数・構造体は MGL2 版で差し替え */
#define LvnsImage mgImage
#define lvnsimage_new mglimage_new
#define lvnsimage_delete mglimage_delete
#define lvnsimage_load_lfg mglimage_load_lfg
#define lvnsimage_load_lf2 mglimage_load_lfg
#define lvnsSinEffect2 lvnsSinEffect
#define lvnsimage_add(s,d,x,y) mglimage_add(s,d,(x)/2,(y)/2)
#define lvnsimage_add2(s,d,x,y) mglimage_add(s,d,(x)/2,(y)/2)
#define lvnsimage_add3(s,d,x,y,p,t,i) mglimage_add3(s,d,(x)/2,(y)/2,p,t,i)
#define lvnsimage_add4(s,d,x,y,p) mglimage_add(s,d,(x)/2,(y)/2)
#define lvnsimage_circle(i,x,y,r,s,p) mglimage_circle(i,x,y,(r)/2,s,p)
/*
 * MGL2 版の場合は X 版と違い，色を引数にこの関数を呼び出すこと．
 */
#define lvnsimage_clear_index mglimage_clear_color
#endif

struct Lvns;

#include "LvnsMenu.h"

typedef enum {
	LVNS_JUMP_NONE,
	LVNS_JUMP_RESTART,
	LVNS_JUMP_TITLE,
	LVNS_JUMP_END
} LvnsJumpParameter;

typedef enum {
	LVNSTIMER_MAIN,
	LVNSTIMER_EFFECT,
	LVNSTIMER_SCRIPT,
} LvnsTimerType;

typedef enum {
    LVNS_CURSOR_PAGE,
    LVNS_CURSOR_KEY,
}LvnsCursorType;

typedef void (*LvnsBackEffectSetFunc)(struct Lvns *lvns);
typedef void (*LvnsBackEffectFunc)(struct Lvns *lvns);
typedef struct {
    LvnsBackEffectSetFunc set;
    LvnsBackEffectFunc func;
} LvnsBackEffectInfo;

typedef struct {
	int scn;  /* シナリオ番号 */
	int blk;  /* ブロック番号 */
	int no;   /* テキスト番号 */
} LvnsHistoryData;        /* 回想モード用 */

typedef struct {
    char *command;
    int enable;
} LvnsCommandInfo;   /* 外部制御用 */

typedef struct {

    int width;                   /* 内部描画領域のサイズ */
    int height;             
    int colors;                  /* 必要な最大色数       */ 

    const char *leafpack_name;   /* データ用パックファイル名称   */
    const char *scnpack_name;    /* シナリオ用パックファイル名称 */
    const char *scn_name;        /* シナリオファイルベース名称 */
    const char *fonttable_name;  /* コードコンバートテーブル名称 */

	MenuData *main_menu;

	void (*start)(struct Lvns *lvns);    /* 初期化       */
	void (*jingle)(struct Lvns *lvns);   /* ジングル     */
	void (*opening)(struct Lvns *lvns);  /* オープニング */
	void (*title)(struct Lvns *lvns);    /* タイトル     */
	void (*main)(struct Lvns *lvns);     /* メイン       */

    void (*mergeCharacter)(struct Lvns *lvns);   /* キャラクタ合成 */
    void (*clearScreen)(struct Lvns *lvns);      /* 仮想 VRAM の消去 */
    void (*loadBG)(struct Lvns *lvns, int no);   /* 背景読み込み */

    LvnsCommandInfo* (*getCommandList)(struct Lvns *lvns);           /* 外部操作一覧 */
    void (*execCommand)(struct Lvns *lvns, const char *command);     /* 外部操作     */
    void (*drawChar)(struct Lvns *lvns, int x, int y, int, int);     /* 文字表示     */

	void (*dispHistory)(struct Lvns *lvns, int pos);

    int cursor_key;   /* キー入力待ちのカーソル */
    int cursor_page;  /* 改ページ待ちのカーソル */

} LvnsSystemState;

typedef struct Lvns {

	enum {
		LVNS_JINGLE,
		LVNS_OP,
		LVNS_MAIN,
		LVNS_WAIT_MENU,
		LVNS_WAIT_KEY,
		LVNS_WAIT_PAGE,
		LVNS_WAIT_SELECT,
	} inside_state;

    /* ------------------------------------------------------------ */
	/* データパラメータ */

    LvnsSystemState *system_state;     /* システムステート */

    char *data_path;        /* データ検索基本パス */
    char *savedata_path;    /* データ保存位置 */
 
    LeafPack *leafpack;     /* データ用パックファイル */
    LeafPack *scnpack;      /* シナリオ用パックファイル */
    u_char *leaf_font_data; /* Leaf FONT のデータ   */

    u_char *leaf_to_euc;    /* コードコンバートテーブル Leaf -> EUC  */
    u_short *jis_to_leaf;   /* コードコンバートテーブル JIS Pack -> Leaf */

    /* ------------------------------------------------------ */
    /* リソース */

    Bool force_skip;      /* 見た文書でもすっとばす           */
    Bool key_click_fast;  /* キークリックで文書を高速表示許可 */
	Bool fast_when_seen;  /* 既読文書は早く表示する           */
    Bool enable_effect;       /* 効果を使う */
    Bool enable_effect_back;  /* 背景効果を使う */

    Bool seen_mode;       /* 全文既読とみなすモード(for DEBUG) */
    Bool demo_mode;       /* オートデモモード */

    /* ------------------------------------------------------ */
    /* システムパラメータ */

    Bool image_dark;    /* 現在の暗さ                      */

    int  char_wait_time;         /* 文字表示待ち時間     */
    
    Bool fast_text;    /* テキストすっ飛ばしフラグ       */
    Bool fast_disp;    /* 表示エフェクトすっとばしフラグ */

	jmp_buf jump;

    /* ------------------------------------------------------ */
    /* 現在の状態 データの保存用 */
    enum bgtype {
        LVNS_VISUAL,
        LVNS_HCG,
        LVNS_BACKGROUND
    } bg_type;
    int bg_no;

	int character_no[3];

    Bool savepoint_flag;
    struct SavePoint{
        enum bgtype bg_type;
        int bg_no;
		int character_no[3];
        int scn;         /* シナリオ番号保存用 */
        int blk;         /* シナリオブロック保存用       */
        long scn_offset; /* シナリオデータ保存用カーソル */
        int current_music;
    } savepoint;
    struct SavePoint selectpoint;

    /* ---------------------------------------------------------- */
    /* 操作状態 */

	/* 外部制御状態 */
    Bool select;
	Bool cancel;
	Bool skip;
	Bool cursor_up;
	Bool cursor_down;
	Bool motion;
	int  motion_x;
	int  motion_y;
	int  number;

	Bool image_mode;
	
    /* ---------------------------------------------------------- */
    /* シナリオパーサ */

    int start_scn_num;    /* 起動時シナリオ番号(for DEBUG) */

    u_char *scn_data;     /* シナリオデータ             */
    u_char *scn_cur_head; /* シナリオデータ先頭         */
    u_char *scn_cur;      /* シナリオデータカーソル     */
    u_char *scn_text;     /* シナリオテキスト           */

    size_t  scn_length;
	Bool    seen;         /* 既読シナリオフラグ         */

    int scn_current;                       /* 現在のシナリオ番号     */
    int blk_current;                       /* 現在のシナリオブロック */


    /* ------------------------------------------------------ */
    /* テキストレイヤ処理系 */

#define TEXT_WIDTH  26
#define TEXT_HEIGHT 16

    struct {
        /* テキストレイヤ用仮想VRAM */
		struct TextVramLine {
			int offset;
			struct TextVram {
				int code;       /* 文字のコード       */
				int attribute;  /* 文字の属性         */
			} column[TEXT_WIDTH];
		} row[TEXT_HEIGHT];
        int cur_x;
        int cur_y;
        int o_cur_x;
        int o_cur_y;
    } tvram[2];
    int current_tvram;

    /* カーソルの状態 */
    int cursor_state;
    
    /* テキストカーソルの状態 */
    int text_cursor_state;     

	/* 表示属性 */
    int text_attr;

	/* 文字が書き込まれているか? */
	Bool text_written;

	/* テキスト表示モードかどうか */
	Bool text_mode;

    /* ------------------------------------------------------- */
    /* 画像描画系 */

    LvnsImage *background; /* 背景データ保持用        */
    LvnsImage *vram;       /* 背景合成作業用仮想 VRAM */

    int latitude;          /* 現在の画像の明るさ 0-255 */
    int latitude_dark;     /* 画面が暗い時の輝度の指定 */

    int sepia_mode_next;   /* 次の画面切り替えでセピア調表示 */
    int sepia_mode;        /* セピア調表示 */

    LvnsBackEffectInfo *effect_back;
    LvnsBackEffectInfo *effect_back_next;
    int effect_back_state;                 /* 背景エフェクト用ステート */
#ifdef FONT_PLUS
    u_char*          font_file;
    LvnsFont	     *font;
#endif /* FONT_PLUS */

	void (*disp_func)(struct Lvns *lvns, void *data); /* 追加表示関数 */
	void *disp_func_param;

    /* -------------------------------------------------------- */
    /* 音楽再生系 */
	Music *music;

    /* ループ処理用 */
    int current_music;                            /* 現在演奏中の音楽     */
    Bool loop_music;                              /* ループかどうか       */
    int next_music;                               /* 次のシーンからの音楽 */
    Bool loop_next_music;                         /* ループかどうか       */

	/* ボリューム処理用 */
    int current_music_volume;                     /* 現在のボリューム設定 */
    int music_fade_mode;                          /* フェードモード       */
    int music_fade_flag;                          /* フェードの方向       */

    /* -------------------------------------------------------- */
    /* サウンド再生系 */
	Sound *sound;

    /* ループ処理用 */
    int sound_loop;
    int sound_count;
    int interval_count;

    /* -------------------------------------------------------- */
	/* ヒストリ */
	LvnsHistoryData *history;
	int history_size;
	int history_pos;

#ifdef USE_MGL
    /* -------------------------------------------------------- */
    /* mglvns のみで使う状態値 */
    int version;
#endif

    /* -------------------------------------------------------- */
	/* システム依存部 */

    void *system_depend;                /* 動作システム依存情報 */

	void (*flushWindow)(struct Lvns *lvns);
	void (*flushWindowArea)(struct Lvns *lvns, int x, int y, int w, int h);
	void (*drawWindow)(struct Lvns *lvns);
	void (*drawWindowArea)(struct Lvns *lvns, int x, int y, int w, int h, int x2, int y2);
	void (*clearWindow)(struct Lvns *lvns);
	void (*clearWindowArea)(struct Lvns *lvns, int x, int y, int w, int h);
	void (*setPaletteIndex)(struct Lvns *lvns, int idx, int r, int g, int b);
	void (*setPalette)(struct Lvns *lvns);
	void (*setPaletteMulti)(struct Lvns *lvns, int par16);
	void (*setPaletteScreen)(struct Lvns *lvns, int par16);
	void (*drawWindowMask)(struct Lvns *lvns, int x, int y, int state);
	void (*drawWindowSquareMask)(struct Lvns *lvns, int x, int y, int state);
	void (*setDispOffset)(struct Lvns *lvns, int xoff, int yoff, int maxoff);
	void (*putPattern)(struct Lvns *lvns, int x, int y, int index, u_char *data);
	void (*putPattern2)(struct Lvns *lvns, int x, int y, int index, u_char *data);
	void (*resetTimer)(struct Lvns *lvns, int no);
	long (*getTimer)(struct Lvns *lvns, int no);
	void (*drawBox)(struct Lvns *lvns, int x, int y, int w, int h, int idx);
	time_t (*getFileTime)(struct Lvns *lvns, const char *path);
	void (*flip)(struct Lvns *lvns);

    /* -------------------------------------------------------- */


} Lvns;

/* LvnsDisp.c 表示系 */
void LvnsDrawTextLayerAll(Lvns *lvns);
void LvnsDrawTextLayer(Lvns *lvns);
void LvnsClearCursor(Lvns *lvns);
void LvnsDrawCursor(Lvns *lvns, LvnsCursorType cursor_type);
void LvnsClearTextCursor(Lvns *lvns);
void LvnsDrawTextCursor(Lvns *lvns);
void LvnsDispWindow(Lvns *lvns);
void LvnsDispText(Lvns *lvns);
void LvnsTextOff(Lvns *lvns);
void LvnsTextOn(Lvns *lvns);

/* LvnsText.c テキスト処理系 */
void LvnsLocate(Lvns *lvns, int x, int y);
void LvnsPutChar(Lvns *lvns, int c, int attr);
void LvnsPutCharNormal(Lvns *lvns, int c, int attr);
void LvnsPuts(Lvns *lvns, const u_char *str, int attr);
void LvnsClearText(Lvns *lvns);
void LvnsSetTextOffset(Lvns *lvns, int offset);
void LvnsNewLineText(Lvns *lvns);
void LvnsDrawString(Lvns *lvns, int x, int y, const u_char *str, int attr);
void LvnsDrawStringCenter(Lvns *lvns, int y, const u_char *str, int attr);

/* LvnsMusic.c 音楽制御 */
void LvnsSetMusicVolume(Lvns *lvns, int no);
void LvnsStartMusic(Lvns *lvns, int no);
void LvnsStopMusic(Lvns *lvns);
void LvnsPauseMusic(Lvns *lvns);
void LvnsStartMusicLoop(Lvns *lvns, int no);
void LvnsStartMusicLoop2(Lvns *lvns, int no);
void LvnsSetNextMusic(Lvns *lvns, int no);
void LvnsSetNextMusicLoop(Lvns *lvns, int no);
void LvnsFadeMusic(Lvns *lvns);
void LvnsStartNextMusic(Lvns *lvns);
void LvnsLoopMusic(Lvns *lvns);
void LvnsWaitMusic(Lvns *lvns);
void LvnsWaitMusicFade(Lvns *lvns);

/* LvnsSound.c 効果音制御 */
void LvnsLoadSound(Lvns *lvns, const char* basename, int no);
void LvnsLoadSound2(Lvns *lvns, const char* name);
void LvnsStartSound(Lvns *lvns, int count);
void LvnsStopSound(Lvns *lvns);
void LvnsWaitSound(Lvns *lvns);
void LvnsLoopSound(Lvns *lvns);

/* LvnsBackEffect.c 背景エフェクト制御 */
void LvnsSetBackEffect(Lvns *lvns, LvnsBackEffectInfo *info);
void LvnsSetNextBackEffect(Lvns *lvns, LvnsBackEffectInfo *info);
void LvnsBackEffectSetState(Lvns *lvns);
void LvnsBackEffect(Lvns *lvns);

/* LvnsFile.c ファイル読み込み */
u_char *LvnsLoadData(Lvns *lvns, const char *name, size_t *size);
LvnsImage *LvnsLoadImage(Lvns *lvns, const char *name, LvnsImage *over);
void LvnsLoadScenario(Lvns *lvns, int scn, int blk);
void LvnsLoadScenarioBlock(Lvns *lvns, int blk);
u_char *LvnsGetScenarioText(Lvns *lvns, int no);
void LvnsLoadBackground(Lvns *lvns, const char *basename, int no);

/* LvnsEtc.c */
void LvnsInitSavePoint(Lvns *lvns, struct SavePoint *sp);
void LvnsSetSavePoint(Lvns *lvns, struct SavePoint *sp);

/* LvnsControl.c */
void LvnsMotion(Lvns *lvns, int x, int y);
void LvnsSelect(Lvns *lvns);
void LvnsCancel(Lvns *lvns);
void LvnsSetImageMode(Lvns *lvns);
void LvnsSkipTillSelect(Lvns *lvns);
void LvnsCursorUp(Lvns *lvns);
void LvnsCursorDown(Lvns *lvns);
void LvnsInputNumber(Lvns *lvns, int no);
void LvnsImageMode(Lvns *lvns);
void LvnsWaitKey(Lvns *lvns);
void LvnsWaitPage(Lvns *lvns);
int  LvnsWaitSelect(Lvns *lvns, int select_num);
void LvnsWaitClick(Lvns *lvns);

/* Lvns.c */
Lvns* LvnsNew(void);
void  LvnsInitialize(Lvns *lvns, int type, void *system_depend);
void  LvnsDestroy(Lvns *lvns);
void  LvnsFlip(Lvns *lvns, Bool disp_ok);
void  LvnsMain(Lvns *lvns);
void  LvnsWait(Lvns *lvns, int flip_cnt);
void  LvnsJump(Lvns *lvns, LvnsJumpParameter param);
void  LvnsSetDataPath(Lvns *lvns, const char *path);
void  LvnsSetSaveDataPath(Lvns *lvns, const char *path);
void  LvnsSetDispFunc(Lvns *lvns, void (*func)(Lvns *lvns, void *data), void *data);

/* LvnsMenu.c */
void LvnsDrawMenu(Lvns *lvns);

/* LvnsHistory.c */
void LvnsAddHistory(Lvns *lvns, int no);
void LvnsHistoryMode(Lvns *lvns);

#ifdef USE_MGL
/* mgMain.c */
void mgLvnsQuit(Lvns *);

/* mgLvns.c */
void mgLvnsInit(Lvns *);
void mgLvnsDelete(Lvns *);
void mgLvnsStart(Lvns *);

/* mgLvnsCore.c */
void mgLvnsCoreSetLvns(Lvns *);

/* mgAction.c */
void MglGetEvent(Lvns *, int);

/* sizuku.c */
void SizukuInitialize(Lvns *);

/* kizuato.c */
void KizuatoInitialize(Lvns *);

/* toheart.c */
void ToHeartInitialize(Lvns *);
#endif

#endif
