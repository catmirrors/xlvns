/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: Lvns.c,v 1.30 2001/08/12 11:35:43 tf Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Lvns.h"
#include "LvnsEffect.h"

/**
 * LVNS 生成
 */
Lvns *
LvnsNew(void)
{
	Lvns *lvns;
    dprintf((stderr, "system initialize...\n"));

	if ((lvns = malloc(sizeof *lvns)) == NULL)
		return NULL;

    lvns->data_path     = strdup(".");
    lvns->savedata_path = NULL;

    /* ---------------------------------------------------------- */
    /* シナリオパーサ初期化 */

    lvns->start_scn_num = -1;
    lvns->scn_data = NULL;
    lvns->scn_text = NULL;
    lvns->scn_current = 0;

    lvns->image_mode = False;
	
    lvns->fast_text     = False;
    lvns->skip          = False;
	
    lvns->char_wait_time  = 1; /* tick */
	
    lvns->force_skip      = False;

	lvns->fast_when_seen  = True;
    lvns->key_click_fast  = True;
    lvns->enable_effect = True;
    lvns->enable_effect_back = True;

    lvns->seen_mode = False;
    lvns->demo_mode = False;

    /* ------------------------------------------------------ */
    /* テキストレイヤ処理系 */

    lvns->current_tvram = 1;
    LvnsClearText(lvns);
    lvns->current_tvram = 0;
    LvnsClearText(lvns);
	
    lvns->cursor_state = 0;
    lvns->text_cursor_state = 0;
    lvns->text_attr   = 0;

    /* -------------------------------------------------------- */
    /* 画像描画系初期化 */
    lvns->latitude  = 16;
    lvns->latitude_dark = 11;

    lvns->sepia_mode_next = False;
    lvns->sepia_mode = False;

    lvns->effect_back       = NULL;
    lvns->effect_back_next  = NULL;
    lvns->effect_back_state = 0;

	lvns->disp_func = NULL;

    /* -------------------------------------------------------- */
    /* 音楽再生系初期化 */
    lvns->current_music        = 0;
    lvns->next_music           = 0;
    lvns->loop_music           = False;
    lvns->current_music_volume = 255;
    lvns->music_fade_mode      = False;
    lvns->music_fade_flag      = 0;
    lvns->music                = NULL;

    /* --------------------------------------------------------- */
    /* SE 関連初期化 */
    lvns->sound_loop  = False;
    lvns->sound_count = 0;
	lvns->sound       = NULL;

    /* --------------------------------------------------------- */
    /* ヒストリ関連初期化 */
	lvns->history      = NULL;
	lvns->history_pos  = 0;
	lvns->history_size = 0;

	return lvns;
}

/**
 * LVNS 初期化
 * @param type   ゲーム種別
 * @param depend システム依存情報
 */
void
LvnsInitialize(Lvns *lvns, int type, void *depend)
{
	lvns->system_depend = depend;

	/* モジュール別処理 */

    switch (type) {
    case 0:
        SizukuInitialize(lvns);
        break;
    case 1:
        KizuatoInitialize(lvns);
        break;
    case 2:
        ToHeartInitialize(lvns);
        break;
    }

	/* 内部画面バッファ生成 */
    lvns->background = lvnsimage_new(WIDTH, HEIGHT, 0, 0);
    lvns->vram       = lvnsimage_new(WIDTH, HEIGHT, 0, 0);
    LVNS->clearScreen(lvns);

    dprintf((stderr, "data loading...\n"));
    {
        /* データ読み出し用パス */
        char path[1024];
    
        /* コード変換テーブルの読み込み */
        {
            FILE *fp;
            size_t size;
            int i;
        
            if ((fp = fopen(LVNS->fonttable_name, "r")) == NULL) {
                snprintf(path, sizeof path, "%s/%s", lvns->data_path, LVNS->fonttable_name);
                if ((fp = fopen(path, "r")) == NULL) {
                    perror(path);
                    exit(1);
                }
            }
            fseek(fp, 0, SEEK_END);
            size = ftell(fp);
            rewind(fp);
        
            /* 変換用テーブルよみこみ */
            lvns->leaf_to_euc = malloc(size);
            if (fread(lvns->leaf_to_euc, 1, size, fp) != size) {
                perror(LVNS->fonttable_name);
                free(lvns->leaf_to_euc);
                exit(1);
            }
            fclose(fp);
        
            /* 逆変換用テーブル作成 */
            size /= 2;  /* 文字数 */
            lvns->jis_to_leaf = (u_short *)
                malloc(sizeof(u_short) * 94 * 94);

            /* 初期化 */
            for (i=0; i < 94 * 94; i++) {
                lvns->jis_to_leaf[i] = 0;
            }
        
            /* マッピング */
            for (i=0; i < size; i++) {
                int code = ((lvns->leaf_to_euc[i*2  ] & 0x7f) - 33) * 94 +
                    (lvns->leaf_to_euc[i*2+1] & 0x7f) - 33;
                lvns->jis_to_leaf[code] = i;
            }
        }

        /* データパッケージオープン */
        snprintf(path, sizeof path, "%s/%s", lvns->data_path, LVNS->leafpack_name);
        if ((lvns->leafpack = leafpack_new(path)) == NULL) {
            if ((lvns->leafpack = leafpack_new(LVNS->leafpack_name)) == NULL) {
                fprintf(stderr, "Can't open package %s.\n", LVNS->leafpack_name);
                exit(1);
            }
        }

        /* シナリオパッケージオープン */
        if ((LVNS->scnpack_name)) {
            snprintf(path, sizeof path, "%s/%s", lvns->data_path, LVNS->scnpack_name);
            if ((lvns->scnpack = leafpack_new(path)) == NULL) {
                if ((lvns->scnpack = leafpack_new(LVNS->scnpack_name)) == NULL) {
                    fprintf(stderr, "Can't open package %s.\n", 
                            LVNS->scnpack_name);
                    exit(1);
                }
            }
        } else {
            lvns->scnpack = lvns->leafpack;
        }

        /* フォントデータ読み込み */
        if (!(lvns->leaf_font_data=LvnsLoadData(lvns, "KNJ_ALL.KNJ", NULL))){
            fprintf(stderr, "Can't open KNJ_ALL.KNJ.\n");
            exit(1);
        }
    }
}

/**
 * LVNS 破棄
 */
void
LvnsDestroy(Lvns *lvns)
{
	if (lvns) {
		/* シナリオデータ解放 */
		free(lvns->scn_data);
		free(lvns->scn_text);
		
		/* データパッケージ解放 */
		leafpack_delete(lvns->leafpack);
		
		/* シナリオパッケージ解放 */
		if (LVNS->scnpack_name)
			leafpack_delete(lvns->scnpack);
		
		/* フォントデータ解放 */
		free(lvns->leaf_font_data);

		/* セーブデータパス情報開放 */
		if (lvns->savedata_path) {
			free(lvns->savedata_path);
		}

		/* データパス情報開放 */
		if (lvns->data_path) {
			free(lvns->data_path);
		}

		/* ヒストリ情報廃棄 */
		if (lvns->history) {
			free(lvns->history);
		}

		/* 自分自身を破棄 */
		free(lvns);
	}
}

/**
 * 内部関数インターバル実行処理
 */
static void
Interval(Lvns *lvns, Bool redisp_ok)
{
	/* BGM の処理 */
	if (lvns->current_music) {
		/* フェード処理 */
		if (lvns->music_fade_mode) {
			int vol, fade;
			if (lvns->music_fade_flag) {
				fade =   256 * 2 * 10 / INTERVAL;  /* 0.5秒以内 */
			} else {
				fade = - 256 * 2 * 10 / INTERVAL;  /* 0.5秒以内 */
			}
			
			vol = lvns->current_music_volume + fade;
			
			if (vol <= 0) {
				vol = 0;
			}
			if (vol >= 255) {
				vol = 255;
			}
			
			if (lvns->music_fade_flag) {
				if (vol == 255) {
					lvns->music_fade_mode = False;
					lvns->music_fade_flag = False;
				}
			}  else {
				if (vol == 0) {
					lvns->music_fade_mode = False;
					lvns->music_fade_flag = True;
				}
			}
			
			LvnsSetMusicVolume(lvns, vol);
			if (vol == 0)
				LvnsPauseMusic(lvns);
		}

		if (lvns->loop_music)
			LvnsLoopMusic(lvns);
	}
	
	if (lvns->sound_loop || lvns->sound_count) 
		LvnsLoopSound(lvns);

	/* 背景画像処理 */
	if (lvns->effect_back && lvns->enable_effect_back) {
		LvnsBackEffectSetState(lvns);
#ifdef USE_MGL
		MglGetEvent(lvns, 1);
#endif
		if (redisp_ok) {
			LvnsBackEffect(lvns);
			LvnsDispWindow(lvns);
		} else {
			LvnsBackEffect(lvns);
		}
	}
}

/**
 * 画面フリップ待ち
 */
void
LvnsFlip(Lvns *lvns, Bool redisp_ok)
{
	Interval(lvns, redisp_ok);
	lvns->flip(lvns);
}

/*
 * 処理メインエンジン
 */
void
LvnsMain(Lvns *lvns)
{
	/* BGM系初期化 */
	MusicOpen(lvns->music);
	
	/* SE系初期化 */
	SoundOpen(lvns->sound);

	/* 各種初期化 */
	LVNS->start(lvns);

	/* 大域ジャンプ用 */
	switch ((LvnsJumpParameter)setjmp(lvns->jump)) {
	case LVNS_JUMP_NONE: /* 初回 */
		break;
	case LVNS_JUMP_RESTART:
		goto main_start;
	case LVNS_JUMP_TITLE:
		goto title;
	case LVNS_JUMP_END:
		goto end;
	}

	if (lvns->start_scn_num >= 0) {
		/* デバッグ用シナリオ指定起動 */
		LvnsInitSavePoint(lvns, &lvns->savepoint);
		lvns->savepoint.scn        = lvns->start_scn_num;
		lvns->savepoint.blk        = 1;
		lvns->savepoint.scn_offset = 0;
	} else {
		/* 起動時ジングル & オープニング */
		LVNS->jingle(lvns);
		LVNS->opening(lvns);
	}

 main_start:
	while (1) {
		lvns->history_pos = 0;
		lvns->latitude    = 16;
		LVNS->main(lvns);

		/* BGM停止 */
		LvnsStopMusic(lvns);
		LvnsStopSound(lvns);

		/* 画面消去 */
		lvns->current_tvram = 0;
		LvnsClearText(lvns);
		LvnsClear(lvns, LVNS_EFFECT_WIPE_TTOB);

	title:
		lvns->latitude    = 16;
		LVNS->title(lvns);
	}

 end:

	/* サウンド系終了 */
	SoundClose(lvns->sound);
	
	/* BGM系終了 */
	MusicClose(lvns->music);
}

/**
 * 時間待ち
 * 指定した回数のフリップ分待つ
 */
void
LvnsWait(Lvns *lvns, int flip_cnt)
{
	while (flip_cnt-- > 0) {
		LvnsFlip(lvns, True);
	}
}

/**
 * 大域ジャンプ処理
 */
void
LvnsJump(Lvns *lvns, LvnsJumpParameter param)
{
	LvnsSetDispFunc(lvns, NULL, NULL);
	longjmp(lvns->jump, (int)param);
	fprintf(stderr, "warning: bad jump!: %d\n", param);
	exit(1);
}

/**
 * ファイル保存パス設定
 */
void
LvnsSetSaveDataPath(Lvns *lvns, const char *path)
{
    if (lvns->savedata_path) {
        free(lvns->savedata_path);
    }
    lvns->savedata_path = strdup(path);
}

void
LvnsSetDataPath(Lvns *lvns, const char *path)
{
	if (lvns->data_path) {
		free(lvns->data_path);
	} 
	lvns->data_path = strdup(path);
}

void
LvnsSetDispFunc(Lvns *lvns, void (*func)(Lvns *lvns, void *data), void *data)
{
	lvns->disp_func = func;
	lvns->disp_func_param = data;
}
