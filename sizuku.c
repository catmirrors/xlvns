/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: sizuku.c,v 1.57 2001/08/05 11:46:02 go Exp $
 *
 */

/* 
 * 雫 シナリオ処理エンジン
 */
#include <stdio.h>
#include <ctype.h>
#include "sizuku.h"

/* うねうね sin_effect.h */
extern LvnsBackEffectInfo lvnsSinEffect;

/* 特殊エフェクト「涙の雫」用データ その1 */
static LvnsAnimationData sizuku01[] = {
    { LVNS_ANIM_IMAGE, "OP_S00.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S01.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S02.LFG", 10, 0, 160 },
    { LVNS_ANIM_SOUND },
    { LVNS_ANIM_IMAGE, "OP_S03.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S04.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S05.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S06.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S07.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S08.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S09.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S10.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S11.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S12.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S13.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S14.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S15.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S16.LFG", 10, 0, 160 },
    { LVNS_ANIM_WAIT, NULL, 200 },
    { LVNS_ANIM_IMAGE, "OP_S00.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S01.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S02.LFG", 10, 0, 160 },
    { LVNS_ANIM_SOUND },
    { LVNS_ANIM_IMAGE, "OP_S03.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S04.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S05.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S06.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S07.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S08.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S09.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S10.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S11.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S12.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S13.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S14.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S15.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S16.LFG", 10, 0, 160 },
    { LVNS_ANIM_WAIT, NULL, 200 },
    { LVNS_ANIM_NONE, NULL, 0 }
};

/* 特殊エフェクト「涙の雫」用データ その2 */
static LvnsAnimationData sizuku02[] = {
    { LVNS_ANIM_IMAGE, "OP_S00.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S01.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S02.LFG", 10, 0, 160 },
    { LVNS_ANIM_SOUND },
    { LVNS_ANIM_IMAGE, "OP_S03.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S04.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S05.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S06.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S07.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S08.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S09.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S10.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S11.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S12.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S13.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S14.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S15.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "OP_S16.LFG", 10, 0, 160 },
    { LVNS_ANIM_IMAGE, "MAX_S37.LFG", 10, 0, 0 },
    { LVNS_ANIM_NONE, NULL, 0 }
};

/* 雫 エフェクト対応表 */
static LvnsEffectType sizuku_effect[] = {
    LVNS_EFFECT_FADE_PALETTE,
    LVNS_EFFECT_GURUGURU,
    LVNS_EFFECT_SLANTTILE,
    LVNS_EFFECT_FADE_SQUARE,
    LVNS_EFFECT_WIPE_SQUARE_LTOR,
    LVNS_EFFECT_FADE_MASK,
    LVNS_EFFECT_WIPE_TTOB,
    LVNS_EFFECT_WIPE_LTOR, 
    LVNS_EFFECT_WIPE_MASK_LTOR,
    LVNS_EFFECT_VERTCOMPOSITION,
    
    LVNS_EFFECT_SLIDE_LTOR,
    LVNS_EFFECT_NORMAL,
    LVNS_EFFECT_FADE_MASK
};

/*
 * テキスト用エフェクトパラメータ変換
 */
static LvnsEffectType
text_effect(int c1, int c2)
{
    int no = ((c1 - '0') * 10) + (c2 - '0');
    LvnsEffectType ret;

    if (no == 99) {
        ret =LVNS_EFFECT_NORMAL;
    } else  {
        ret = sizuku_effect[no];
    }

    // dprintf((stderr,  "(%02d)%s\n", no, LvnsEffectName(ret)));
    return ret;
}

/*
 * bgm パラメータ変換
 */
static int
bgmmap(int no)
{
    if (no == 14) 
        return 2;
    else if (no < 16)
        return no+2;
    else 
        return no+1;
}

/* 
 * フラグパラメータ変換
 */
static int
flagmap(int no)
{
    switch (no) {
    case 0:
        return 0; /* エンディング状態記録(非初期化対象) */
    case 1:
        return 1; /* 雑フラグ */
    case 0x40: /* 瑠璃子と約束 */
    case 0x41: /* 怪しい人影 */
    case 0x42: /* 部活動で変わったこと */
    case 0x43: /* 体育館で… */
    case 0x44: /* 瑠璃子屋上「届かなかった」 */
    case 0x45: /* 瑠璃子 TRUE  (非初期化対象) */
    case 0x46: /* 瑠璃子 HAPPY (非初期化対象) */
    case 0x47: /* 佐織ルートON */
    case 0x48: /* 佐織 Hシーン */  
    case 0x49: /* おそらく未使用 */
    case 0x4a: /* 瑞穂移動場所制御用 */
    case 0x4b: /* オルゴールぜんまい */
        return 2 + no - 0x40;
    default:
        dprintf((stderr, "bad flag no:%d\n", no));
        exit(1);
    }
}

/**
 * シナリオ初期化 (クリア状態フラグと未見情報以外の初期化)
 */
void
SizukuScenarioInit(Lvns *lvns)
{
    SizukuState *state = (SizukuState*)lvns->system_state;

    /* セーブ状態状態初期化 */
    LvnsInitSavePoint(lvns, &lvns->savepoint);

    /* シナリオフラグ初期化 */
    state->flag_save[2] = 0;
    state->flag_save[3] = 0;
    state->flag_save[4] = 0;
    state->flag_save[5] = 0;
    state->flag_save[6] = 0;
	
    state->flag_save[9]  = 0;
    state->flag_save[10] = 0;
    state->flag_save[11] = 0;
    state->flag_save[12] = 0;
    state->flag_save[13] = 0;
}

/**
 * しおり初期化 (全フラグ消去)
 */
void
SizukuSioriInit(Lvns *lvns)
{
    SizukuState *state = (SizukuState *)lvns->system_state;
    int i;

    dprintf((stderr, "Siori Init\n"));

    /* シナリオデータ初期化 */
    SizukuScenarioInit(lvns);

    /* 未見フラグ全消去 */
    for (i=0; i<SIZUKU_SCN_NO;i++) {
		state->seen_flag[i] = 0;
    }

    /* 残りの制御フラグの消去 */
    state->flag_save[0] = 0;
    state->flag_save[7] = 0;
    state->flag_save[8] = 0;
}

#define HexToDig(c) ((tolower(c) >= 'a')? (tolower(c) -'a' + 10) : (c - '0'))
#define HexToDig2(c,c2) (HexToDig(c)*16 + HexToDig(c2))
#define Dig(c1,c2) (c1 - '0') * 10 + (c2 - '0')

static void
SizukuDispText(Lvns *lvns, const u_char *c, Bool history_mode)
{
    while(1) {
		if (c[0] & 0x80) {
			int FontCode = ((c[0] & 0x7f) << 8) + c[1];
			LvnsDispText(lvns);
			if (lvns->tvram[lvns->current_tvram].cur_x > 24) {
				LvnsNewLineText(lvns);
			}
			LvnsPutChar(lvns, FontCode, lvns->text_attr);
			c += 2;
		} else switch (c[0]) {

		case '$': /* '$' メッセージ終了 */
			c++;
			return;
			
		case 'r': /* 改行 */
			LvnsNewLineText(lvns);
			c++;
			break;
			
		case 'p': /* ページ更新待ち */
			if (!history_mode) {
				LvnsWaitPage(lvns);
				lvns->savepoint_flag = True;
			}
			c++;
			break;
			
		case 'k':
		case 'K': /* キー入力待ち */
			if (!history_mode) {
				if (!lvns->fast_text)
					LvnsWaitKey(lvns);
			}
			c++;
			break;
			
		case '0': /* スクリプトバグで '0' が余分についているところがある */
			dprintf((stderr, "[「0」無視!]\n"));
			c++;
			break;
			
		case 'C': /* キャラクタ交換 */
			dprintf((stderr, "[キャラクタ交換(%c, MAX_C%c%c)]", c[1], c[2], c[3]));
			if (!history_mode) {
				LvnsUndispText(lvns);
				SizukuClearCharacter(lvns, c[1]);
				SizukuLoadCharacter(lvns, HexToDig2(c[2], c[3]), c[1]);
				LvnsDisp(lvns, LVNS_EFFECT_FADE_MASK);
			}
			c += 4;
			break;
			
		case 'B': /* 背景表示 */
			dprintf((stderr, "[背景ロード(%c%c, %d, %d)]\n", c[1], c[2],
					 Dig(c[3],c[4]), Dig(c[5],c[6])));
			if (!history_mode) {
				LvnsUndispText(lvns);
				LvnsClear(lvns, text_effect(c[3],c[4]));
				SizukuLoadBG(lvns, Dig(c[1],c[2]));
				LvnsDisp(lvns, text_effect(c[5],c[6]));
			}
			c += 7;
			break;
			
		case 'S': /* 背景つきキャラクタ表示 */
			dprintf((stderr, "[背景付きキャラ表示(%c, MAX_C%c%c, MAX_S%c%c, %d, %d)]",
					 c[1], c[2],c[3],c[4],c[5], Dig(c[6],c[7]), Dig(c[8], c[9])));
			if (!history_mode) {
				LvnsUndispText(lvns);
				LvnsClear(lvns, text_effect(c[6],c[7]));
				SizukuLoadBG(lvns, Dig(c[4], c[5]));
				SizukuLoadCharacter(lvns, HexToDig2(c[2], c[3]), c[1]);
				LvnsDisp(lvns,text_effect(c[8],c[9]));
			}
			c += 10;
			break;
			
		case 'D': /* キャラクタ全消去後表示 */
			dprintf((stderr, "[キャラ全消去後表示(%c, MAX_C%c%c)]", c[1], c[2], c[3]));
			if (!history_mode) {
				LvnsUndispText(lvns);
				SizukuClearCharacter(lvns, 'a');
				SizukuLoadCharacter(lvns, HexToDig2(c[2], c[3]), c[1]);
				LvnsDisp(lvns, LVNS_EFFECT_FADE_MASK);
			}
			c += 4;
			break;
			
		case 'A':
		case 'a':
			dprintf((stderr, "[キャラ3人表示(%c, %c%c, %c, %c%c, %c, %c%c)]",
					 c[1],c[2],c[3],  c[4],c[5],c[6], c[7],c[8],c[9]));
			if (!history_mode) {
				LvnsUndispText(lvns);
 				SizukuClearCharacter(lvns, 'a');
				SizukuLoadCharacter(lvns, HexToDig2(c[2], c[3]), c[1]);
				SizukuLoadCharacter(lvns, HexToDig2(c[5], c[6]), c[4]);
				SizukuLoadCharacter(lvns, HexToDig2(c[8], c[9]), c[7]);
				LvnsDisp(lvns, LVNS_EFFECT_FADE_MASK);
			}
			c += 10;
			break;
			
		case 'Q': /* 画面を揺らす */
			dprintf((stderr, "[画面を揺らす(%02x)]", c[0]));
			if (!history_mode) {
				LvnsClearCursor(lvns);
				LvnsVibrato(lvns);
			}
			c++;
			break;
			
		case 'E': /* 背景表示2 ? */
			dprintf((stderr, "[背景ロード(2)?(MAX_S%c%c.LFG, %d, %d)]",
					 c[1], c[2], Dig(c[3], c[4]), Dig(c[5], c[6])));
			if (!history_mode) {
				LvnsUndispText(lvns);
				LvnsClear(lvns, text_effect(c[3],c[4]));
				SizukuLoadBG(lvns, Dig(c[1],c[2]));
				LvnsDisp(lvns, text_effect(c[5],c[6]));
			}
			c += 7;
			break;
			
		case 'F': /* フラッシュ */
			dprintf((stderr, "[フラッシュ(%c)]", c[0]));
			if (!history_mode) {
				LvnsWhiteOut(lvns);
				LvnsWhiteIn(lvns);
			}
			c++;
			break;
			
		case 'V': /* ビジュアルシーン表示 */
			dprintf((stderr, "[ビジュアル(VIS%c%c, %d, %d)]",
					 c[1], c[2], Dig(c[3], c[4]), Dig(c[5], c[6])));
			if (!history_mode) {
				LvnsUndispText(lvns);
				LvnsClear(lvns, text_effect(c[3],c[4]));
				SizukuLoadVisual(lvns, Dig(c[1],c[2]));
				LvnsDisp(lvns, text_effect(c[5],c[6]));
			}
			c += 7;
			break;
			
		case 'H': /* Hシーン表示 */
			dprintf((stderr, "[Hシーン(HVS%c%c, %d, %d)]",
					 c[1], c[2], Dig(c[3], c[4]), Dig(c[5], c[6])));
			if (!history_mode) {
				LvnsUndispText(lvns);
				LvnsClear(lvns,text_effect(c[3],c[4]));
				SizukuLoadHVisual(lvns, Dig(c[1],c[2]));
				LvnsDisp(lvns, text_effect(c[5],c[6]));
			}
			c += 7;
			break;
			
		case 'M': /* BGM 関連 */
			{
				int c1 = c[1];
				c += 2;
				if (c1 == 'f') {
					dprintf((stderr, "[BGM フェードアウト]\n"));
					if (!history_mode) {
						LvnsFadeMusic(lvns);
					}
				} else if (c1 == 'n') {
					int c2 = *c++;
					int c3 = *c++;
					dprintf((stderr, "[BGM 再生(next)(M_%c%c)\n", c2, c3));
					if (!history_mode) {
						LvnsSetNextMusicLoop(lvns, bgmmap(Dig(c2, c3)));
					}
				} else if (c1 == 'w') {
					dprintf((stderr, "[BGM FADE WAIT]\n"));
					if (!history_mode) {
						LvnsWaitMusicFade(lvns);
					}
				} else if (c1 >= '0' && c1 <= '2') {
					int c2 = *c++;
					dprintf((stderr, "[BGM 再生(M_%c%c)]\n", c1, c2));
					if (!history_mode) {
						LvnsStartMusicLoop(lvns, bgmmap(Dig(c1, c2)));
					}
				} else if (c1 == 's') {
					dprintf((stderr, "[BGM 停止]\n"));
					if (!history_mode) {
						LvnsPauseMusic(lvns);
					}
				} else {
					dprintf((stderr, "[cmd:4d][cmd:%x]\n", c1));
				}
			}
			break;
			
		case 'P': /* PCM 関連 */
			{
				int c1 = c[1];
				c += 2;
				if ( c1 == 'l' ) {
					int c2 = *c++;
					int c3 = *c++;
					dprintf((stderr, "[PCMロード(%c%c)]\n", c2, c3));
					if (!history_mode) {
						LvnsLoadSound(lvns, "SZ_VD%02d.P16", Dig(c2,c3));
					}
				} else if (c1 >= '0' && c1 <= '9') {
					int c2 = *c++;
					int c3 = *c++;
					int c4 = *c++;
					dprintf((stderr, "[PCM再生指定(%c%c, %c%c)\n]", 
							 c1, c2, c3, c4));
					if (!history_mode) {
						LvnsStartSound(lvns, 1);
					}
				} else if (c1 == 'f') {
					dprintf((stderr, "[PCMフェードアウト?]\n"));
				} else if (c1 == 'w') {
					dprintf((stderr, "[PCM wait?]\n"));
					if (!history_mode) {
						LvnsWaitSound(lvns);
					}
				} else if (c1 == 's') {
					dprintf((stderr, "[PCM停止]\n"));	
					if (!history_mode) {
						LvnsStopSound(lvns);
					}
				} else {
					dprintf((stderr, "[cmd:50][cmd:%x]\n", c1));
				}
				break;
			}
			
		case 'X': /* 'X' 表示オフセット指定 */
			dprintf((stderr, "[表示オフセット指定(%x)]\n", c[1]));
			if (!history_mode) {
				LvnsSetTextOffset(lvns, c[1]);
			}
			c += 2;
			break;
			
		case 's': /* 's' 表示速度指定 */
			dprintf((stderr, "[表示速度指定?(%x)]\n", c[1]));
			if (!history_mode) {
				lvns->char_wait_time = c[1];
			}
			c += 2;
			break;
			
		default:
			dprintf((stderr, "unknown!!"));
			dprintf((stderr, "[cmd:%x]", c[0]));
			return ;
		}
    } /* while(1) */
}

/*
 * テキストシナリオ処理
 */
static void
TextParser(Lvns *lvns, int no, Bool add_history)
{
	SizukuSetTextScenarioState(lvns, no);
	SizukuDispText(lvns, LvnsGetScenarioText(lvns, no),  False);
	if (add_history)
		LvnsAddHistory(lvns, no);
}

/* ------------------------------------------------------------ */
/** シナリオエンジン開始用 */
static void
SizukuStartScenario(Lvns *lvns)
{
    SizukuState *state = (SizukuState*)lvns->system_state;

    /* シナリオ状態復活 */
    LvnsLoadScenario(lvns, lvns->savepoint.scn, lvns->savepoint.blk);
    lvns->scn_cur = lvns->scn_cur_head + lvns->savepoint.scn_offset;

	LvnsClearText(lvns);

    /* 画面状態の復活 */
    if (lvns->savepoint.bg_no) {
        switch (lvns->savepoint.bg_type) {
        case LVNS_VISUAL:
			SizukuLoadVisual(lvns, lvns->savepoint.bg_no);
            break;
        case LVNS_HCG:
            SizukuLoadHVisual(lvns, lvns->savepoint.bg_no);
            break;
        default:
            SizukuLoadBG(lvns, lvns->savepoint.bg_no);
            break;
        }
        SizukuClearCharacter(lvns, 'a');
        if (lvns->savepoint.character_no[0] != NOCHARACTER)
            SizukuLoadCharacter(lvns, lvns->savepoint.character_no[0], 'l');
        if (lvns->savepoint.character_no[1] != NOCHARACTER)
            SizukuLoadCharacter(lvns, lvns->savepoint.character_no[1], 'r');
        if (lvns->savepoint.character_no[2] != NOCHARACTER)
            SizukuLoadCharacter(lvns, lvns->savepoint.character_no[2], 'c');
		LvnsDisp(lvns, LVNS_EFFECT_WIPE_TTOB);
    } else {
        SizukuLoadBG(lvns, 0);       
        SizukuClearCharacter(lvns, 'a');
    }


    /* BGM復活 */
    if (lvns->savepoint.current_music)
        LvnsSetNextMusicLoop(lvns, lvns->savepoint.current_music);

    /* フラグ復活 */
    memcpy(state->flag, state->flag_save, sizeof state->flag);

    /* 「選択肢に戻る」初期化 */
    lvns->selectpoint = lvns->savepoint;
    memcpy(state->flag_select, state->flag_save, sizeof state->flag);
}


/*
 * 雫シナリオパーサ本体
 */
#define c lvns->scn_cur
void
SizukuMain(Lvns *lvns)
{
    SizukuState *state = (SizukuState *)lvns->system_state;
	SizukuStartScenario(lvns);

	/* メインループ */
    while(1) {

		lvns->inside_state = LVNS_MAIN;

		/* セーブポイント設定 */
		if (lvns->savepoint_flag) {
			LvnsSetSavePoint(lvns, &lvns->savepoint);
			memcpy(state->flag_save, state->flag, sizeof state->flag);
			lvns->savepoint_flag = False;
		}

        switch (c[0]) {
        case 0x00:		/* ブロック終了 */
            dprintf((stderr, "[END]\n"));
            c++;
			return;

        case 0x01: /* 特殊効果 */
            switch (c[1]) {
            case 0x01:
                dprintf((stderr, "[ぐにゃり→暗]-[メッセージ:%d]\n", c[2]));
				LvnsSetBackEffect(lvns, &lvnsSinEffect);
				TextParser(lvns, c[2], True);
				LvnsSetBackEffect(lvns, NULL);   
				LvnsClear(lvns, LVNS_EFFECT_FADE_PALETTE);
				c += 3;
                break;
            case 0x02:
                dprintf((stderr, "[暗→ぐにゃり]-[メッセージ:%d]\n", c[2]));
				LvnsDisp(lvns, LVNS_EFFECT_FADE_PALETTE);
				LvnsSetBackEffect(lvns, &lvnsSinEffect);
				TextParser(lvns, c[2], True);
				LvnsSetBackEffect(lvns, NULL);   
				c += 3;
                break;
            case 0x03:
                dprintf((stderr, "[涙の雫:%02x]\n", c[2]));
                LvnsAnimation(lvns, (c[2] == 0)?sizuku01:sizuku02);
                c += 3;
                break;
            case 0x04:
                dprintf((stderr, "[ぐにゃり2(異次元)]-[メッセージ:%d]\n", c[2]));
				TextParser(lvns, c[2], True);
                c += 3;
                break;
            default:
                dprintf((stderr, "異常な0x01コマンドです(%02x,%02x)\n", c[1], c[2]));
				return;
                break;
            }
            break;

        case 0x03: /* 謎 */
            dprintf((stderr, "[謎:%02x(%02x)]\n", c[0], c[1]));
            c += 2;
            break;

        case 0x04: /* ジャンプ命令 */
            dprintf((stderr, "[ジャンプ SCN%03d.DAT - Block %d]\n", c[1], c[2]));
            LvnsLoadScenario(lvns, c[1], c[2]);
			break;
			
        case 0x05: /* 選択肢 */
			dprintf((stderr, "[選択肢(%d)]-[メッセージ:%d]\n", c[2], c[1]));
            {
                int i;
				TextParser(lvns, c[1], True);
                for (i = 0; i < c[2]; i++) {
                    dprintf((stderr,
							 "\t[選択肢 %d]-[メッセージ:%d]-[オフセット:%02x]\n"
                             , i, c[3+i*2], c[4+i*2]));
					lvns->text_attr = i+1;
					TextParser(lvns, c[3+i*2], False);
                }
				lvns->text_attr = 0;
				i = LvnsWaitSelect(lvns, c[2]);
				LvnsAddHistory(lvns, c[3+i*2]);

				dprintf((stderr, "選択分岐: %d (+%02x)\n", i, c[4 + i*2]));
				c = c + 3 + c[2]*2 + c[4 + i*2];
            }
            break;

        case 0x06:
            dprintf((stderr, "[謎:%02x()]\n", c[0]));
            c++;
            break;
			
        case 0x07:
            dprintf((stderr, "[前の選択肢に戻るマーク位置]\n"));
            LvnsSetSavePoint(lvns, &lvns->selectpoint);
            memcpy(state->flag_select, state->flag, sizeof state->flag);
            c++;
            break;		      
			
        case 0x0a: /* 背景ロード */
            dprintf((stderr, "[背景ロードのみ: MAX_S%02d.LFG]\n", c[1]));
			SizukuLoadBG(lvns, c[1]);
            c += 2;
			break;
			
        case 0x14: /* 画面消去 */
            dprintf((stderr, "[画面クリア? (%02d)]\n", c[1]));
			LvnsUndispText(lvns);
            LvnsClear(lvns, sizuku_effect[c[1]]);
            c += 2;
			break;

        case 0x16: /* Hロード */
            dprintf((stderr, "[Hシーンロード : MAX_H%02d.LFG]\n", c[1]));
			LvnsUndispText(lvns);
			SizukuLoadHVisual(lvns, c[1]);
			LvnsDisp(lvns, LVNS_EFFECT_NORMAL);
            c += 2;
			break;

        case 0x22: /* キャラクタロード */
			dprintf((stderr, "[キャラクタロード : MAX_C%02x.LFG : %02x]\n", c[1], c[2]));
			LvnsUndispText(lvns);
			SizukuLoadCharacter(lvns, c[1], c[2]);
			LvnsDisp(lvns, LVNS_EFFECT_FADE_MASK);
			c += 3;
			break;
			
        case 0x24: /* キャラクタロードその2? */
			dprintf((stderr, "[キャラクタロード(2? center?) : MAX_C%02x.LFG]\n", c[1]));
			LvnsUndispText(lvns);
			SizukuLoadCharacter(lvns, c[1], 'c');
			LvnsDisp(lvns, LVNS_EFFECT_FADE_MASK);
			c += 3;
			break;
			
        case 0x28: /* 選択肢の前に存在するデータ */
            dprintf((stderr,  "[選択肢の前]\n"));
            c++;
            break;

        case 0x38:      
            dprintf((stderr, "[表示処理:%02x(%02x)]\n", c[0], c[1]));
            LvnsDisp(lvns, sizuku_effect[c[1]]);
            c += 2;
            break;
			
        case 0x3d:	/* if文 */
            dprintf((stderr, "[if flg:%02x == 0x%02x pc += %02x]\n", c[1], c[2], c[3]));
            if (state->flag[flagmap(c[1])] == c[2])
                c += c[3];
            c += 4;
            break;

        case 0x3e:	/* if文(否定) */
            dprintf((stderr, "[if flg:%02x != 0x%02x pc += %02x]\n", c[1], c[2], c[3]));
            if (state->flag[flagmap(c[1])] != c[2])
                c += c[3];
            c += 4;
            break;

        case 0x47:     /* フラグの値設定 */
            dprintf((stderr, "[flg:%02x = 0x%02x]\n", c[1], c[2]));
            state->flag[flagmap(c[1])] = c[2];
            c += 3;
            break;
			
        case 0x48:	/* フラグ加算 */
            dprintf((stderr, "[flg:%02x += 0x%02x\n", c[1], c[2]));
            state->flag[flagmap(c[1])] += c[2];
            c += 3;
            break;
			
        case 0x54:      /* テキストメッセージ */
            dprintf((stderr, "[メッセージ:%d]\n", c[1]));
			TextParser(lvns, c[1], True);
            c += 2;
            break;

        case 0x5a:
            dprintf((stderr, "[謎:%02x()]\n", c[0]));
            c ++;
            break;

        case 0x5c:
            dprintf((stderr, "[謎:%02x(%02x)]\n", c[0], c[1]));
            c += 2;
            break;

        case 0x61:
            dprintf((stderr, "[謎:%02x(%02x %02x)]\n", c[0], c[1], c[2]));
            c += 3;
            break;

        case 0x62:
            dprintf((stderr, "[謎:%02x(%02x)]\n", c[0], c[1]));
            c += 2;
            break;

        case 0x60:
        case 0x63:
        case 0x64:
        case 0x65:
        case 0x66:
            dprintf((stderr, "[謎:%02x()]\n", c[0]));
            c++;
            break;

        case 0x6e: /* BGM 再生 */
            dprintf((stderr, "[BGM再生(CMD):(%02x)]\n", c[1]));
            LvnsStartMusicLoop(lvns, bgmmap(c[1]));
            c += 2;
            break;

        case 0x6f:
        case 0x73:
            dprintf((stderr, "[謎:%02x()]\n", c[0]));
            c++;
            break;

        case 0x7e:
            dprintf((stderr, "[エンディング番号設定: %02x]\n", c[1]));
/*
 0 12 卒業式
 1 12 瑞穂 BAD
 2 12 破壊
 3 12 トースター
 4 11 佐織 HAPPY
 5 12 佐織 BAD
 6 11 瑞穂 HAPPY
 7 12 瑞穂 BAD
 8 10 True
 9 11 瑠璃子 Happy
 a 01 大田さん
 b 14 異次元
 c 12 異次元 BAD
*/
            if (state->flag[flagmap(0x46)] == 1) { 
                /* 瑠璃子 True END を見ている */
                state->flag[0] = 3;
            } else {
                if (state->flag[0] == 0)
                    state->flag[0] = 2;
                else
                    state->flag[0] = 1;
            }       
            c += 2;
            break;

        case 0x7c:
            dprintf((stderr, "[エンディング関係 謎:%02x()]\n", c[0]));
            c++;
            break;

        case 0x7d:
            dprintf((stderr, "[エンディングBGM設定 & 起動:%02x(%d)]\n", c[0], c[1]));
            /* エンディング演奏開始 */
            LvnsStartMusic(lvns, bgmmap(c[1]));
            
			/* エンディング実行 */
			SizukuEnding(lvns);

            /* 最終的なフラグを反映 */
            memcpy(state->flag_save, state->flag, sizeof state->flag);

            /* エンディング到達につき強制シナリオデータ初期化 */
            SizukuScenarioInit(lvns);
            SizukuSave(lvns);

            c += 2;
			break;

        case 0xff:
			dprintf((stderr, "本来アクセスし得ない場所にアクセスした??\n"));
			return;
			
        default:
            dprintf((stderr, "キャッチされなかったコマンドです(%02x)\n", c[0]));
			return;
        }
	
    } /* while(1) */
}
#undef c

/**
 * 開始直前の初期化
 */
void
SizukuStart(Lvns *lvns)
{
    lvns->setPaletteIndex(lvns, SIZUKU_COL_WHITE, 255, 255, 255);
    lvns->setPaletteIndex(lvns, SIZUKU_COL_GRAY,  127, 127, 127);
    lvns->vram->black       = SIZUKU_COL_BLACK;
    lvns->background->black = SIZUKU_COL_BLACK;
    LvnsInitAnimation(lvns, sizuku01);
    LvnsInitAnimation(lvns, sizuku02);
}

/**
 * 履歴の表示
 */
void
SizukuDispHistory(Lvns *lvns, int pos)
{
	LvnsClearText(lvns);         /* 消去 */
	LvnsDispWindow(lvns);
	if (pos >= 0 && pos < lvns->history_pos) {
		LvnsLoadScenario(lvns, lvns->history[pos].scn, lvns->history[pos].blk);
		SizukuDispText(lvns, LvnsGetScenarioText(lvns, lvns->history[pos].no), True);
	}

	// カーソル表示
#ifndef USE_MGL
#define CUR_X 25
#else
#define CUR_X 24
#endif
	LvnsLocate(lvns, CUR_X, 0);
	LvnsPuts(lvns, "↑", 1);
	LvnsLocate(lvns, CUR_X, 11);
	LvnsPuts(lvns, "↓", 2);
}

