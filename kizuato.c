/*
 * leaf Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: kizuato.c,v 1.52 2001/08/11 19:59:36 tf Exp $
 *
 */

/*
 * 痕 シナリオ処理エンジン
 */

#include <stdio.h>
#include "kizuato.h"

/* うねうね sin_effect.h */
extern LvnsBackEffectInfo lvnsSinEffect2;

/* 特殊エフェクト「とびちる血」 */
static LvnsAnimationData kizuato01[] = {
    { LVNS_ANIM_IMAGE, "BLDW0.LFG", 100, 0, 0 },
    { LVNS_ANIM_IMAGE, "BLDW1.LFG", 100, 0, 0 },
    { LVNS_ANIM_IMAGE, "BLDW2.LFG", 100, 0, 0 },
    { LVNS_ANIM_IMAGE, "BLDW3.LFG", 100, 0, 0 },
    { LVNS_ANIM_WAIT, NULL, 100 },
    { LVNS_ANIM_NONE, NULL, 0 }
};

/* 特殊エフェクト「鬼の爪」 */
static LvnsAnimationData kizuato02[] =  {
    { LVNS_ANIM_IMAGE, "CLAW01W.LFG", 100, 0, 0 },
    { LVNS_ANIM_IMAGE, "CLAW02W.LFG", 100, 0, 0 },
    { LVNS_ANIM_IMAGE, "CLAW03W.LFG", 100, 0, 0 },
    { LVNS_ANIM_IMAGE, "CLAW04W.LFG", 100, 0, 0 },
    { LVNS_ANIM_WAIT, NULL, 100 },
    { LVNS_ANIM_NONE, NULL, 0 }
};

/* 特殊エフェクト「吹き出る血」 */
static LvnsAnimationData kizuato03[] =  {
    { LVNS_ANIM_IMAGE, "CLAW05W.LFG", 100, 0, 0 },
    { LVNS_ANIM_IMAGE, "CLAW06W.LFG", 100, 0, 0 },
    { LVNS_ANIM_IMAGE, "CLAW07W.LFG", 100, 0, 0 },
    { LVNS_ANIM_IMAGE, "CLAW08W.LFG", 100, 0, 0 },
    { LVNS_ANIM_IMAGE, "CLAW09W.LFG", 100, 0, 0 },
    { LVNS_ANIM_IMAGE, "CLAW10W.LFG", 100, 0, 0 },
    { LVNS_ANIM_IMAGE, "CLAW11W.LFG", 100, 0, 0 },
    { LVNS_ANIM_WAIT, NULL, 100 },
    { LVNS_ANIM_NONE, NULL, 0 }
};

/**
 * テキスト用エフェクトパラメータ変換
 */
static LvnsEffectType
text_effect(int no)
{
    static LvnsEffectType kizuato_effect[] =  {
/**/    LVNS_EFFECT_FADE_PALETTE,
/**/    LVNS_EFFECT_GURUGURU,
        LVNS_EFFECT_FADE_MASK,
/**/    LVNS_EFFECT_FADE_SQUARE,
        LVNS_EFFECT_WIPE_SQUARE_LTOR,
/**/    LVNS_EFFECT_FADE_MASK,
        LVNS_EFFECT_WIPE_TTOB,
/**/        LVNS_EFFECT_WIPE_LTOR,
/**/        LVNS_EFFECT_WIPE_MASK_LTOR,
        LVNS_EFFECT_VERTCOMPOSITION,

/**/        LVNS_EFFECT_SLIDE_LTOR,
/**/        LVNS_EFFECT_NORMAL,
        LVNS_EFFECT_FADE_MASK,
        LVNS_EFFECT_CIRCLE_SHRINK,
        LVNS_EFFECT_NONE,
        LVNS_EFFECT_NONE,
        LVNS_EFFECT_NONE,
        LVNS_EFFECT_LEFT_SCROLL,
/**/        LVNS_EFFECT_FADE_PALETTE, /* 高速版? */
        LVNS_EFFECT_RAND_RASTER,
    };

    LvnsEffectType ret;
    ret = kizuato_effect[no];
    dprintf((stderr, "(%2d)%s\n", no, LvnsEffectName(ret)));
    return ret;
}

/*
 * bgm パラメータ変換
 */
static int
bgmmap(int no)
{
    switch (no) {
    default:
        return no+2;
    }
}

/*
 * フラグ
  0: 楓と会話した
  1: 自分が犯人ではない
  2: 千鶴さんを疑う
  3: 梓と事件現場にむかった
  4: 梓シナリオ警察に電話
  5: グラビアを買った
  6: 雫を買った
  7: 初音ちゃんをこわがらせない
  8: おまもりをわたした
  9: 未使用?
  
  a-13  エンディング後フラグ制御用 (内部で初期化される)

  14: 千鶴BADエンドを見た
  15: 千鶴エンディングを見た
  16: 梓 Happy を見た
  17: 楓 BAD を見た
  18: 楓 HAPPY を見た 
  19: 柳川エンドを見た
  1a: 初音エンドを見た

  73: 全エンディングを見たかどうか
*/
static int
flagmap(int no)
{
    if (no == 0x73)
        return 0x1b;
    else
        return no;
}

/**
 * シナリオ初期化
 */
void
KizuatoScenarioInit(Lvns *lvns)
{
    int i;
    KizuatoState *state = (KizuatoState *)lvns->system_state;

    /* セーブ状態状態初期化 */
    LvnsInitSavePoint(lvns, &lvns->savepoint);

    /* シナリオフラグ初期化 */
    for (i=0; i<10;i++) {
        state->flag_save[i] = 0;
    }
}

/**
 * しおり初期化 (全フラグ消去)
 */
void
KizuatoSioriInit(Lvns *lvns)
{
    int i;
    KizuatoState *state = (KizuatoState *)lvns->system_state;

    dprintf((stderr, "Siori Init\n"));
    
    /* シナリオデータ初期化 */
    KizuatoScenarioInit(lvns);

    /* 未見フラグ全消去 */
    for (i=0; i<KIZUATO_SCN_NO;i++) {
        state->seen_flag[i] = 0;
    }
    
    /* 残りの制御フラグの消去 */
    for (i=0; i<KIZUATO_FLAG_NO;i++) {
        state->flag_save[i] = 0;
    }

    /* エンディング状態消去 */
    for (i=0; i<sizeof state->ending;i++) {
	state->ending[i] = 0;
    }

    state->reverse_palette = 0;
    state->change_palette  = 0;
    state->change_palette2 = 0;
}

/*
 * 痕 コマンド処理
 */
#define c (*p)
static int
CommandParser(Lvns *lvns, const u_char **p, int history_mode) 
{
    KizuatoState *state = (KizuatoState *)lvns->system_state;

    switch (c[0]) {
    case 0x30:		/* 背景ロードのみ */
		dprintf((stderr, "[0x30: 処理無し背景ロード %d]\n", c[1]));
		if (!history_mode) {
			KizuatoLoadBG(lvns, c[1]);
		}
        c += 2;
        break;
       
    case 0x31:
        dprintf((stderr, "[0x31: 謎 エフェクト指定 (%x)?\n",c[1]));
        c += 2;
        break;
        
    case 0x34:
        dprintf((stderr, "[0x34: 画面消去?(%x)\n", c[1]));
		if (!history_mode) {
			LvnsClear(lvns, text_effect(c[1]));
		}
        c += 2;
        break;

    case 0x35:
        dprintf((stderr, "[0x31: 謎 エフェクト指定? (%x)\n", c[1]));
        c += 2;
        break;

    case 0x36:
		dprintf((stderr, "[0x36: 処理無しVisualロード %d]\n", c[1]));
		if (!history_mode) {
			KizuatoLoadVisual(lvns, c[1]);
		}
        c += 2;
        break;
        
    case 0x37:
        dprintf((stderr, "[0x37: 処理無しキャラクタロード %d]\n", c[2]));
		if (!history_mode) {
			KizuatoLoadCharacter(lvns, c[2], c[1]);
		}
        c += 3;
        break;
        
    case 0x38:
        dprintf((stderr, "[0x38: 直接表示処理 %d %d]\n", c[1], c[2]));
		if (!history_mode) {
			switch (c[1]) {
			case 0:
				KizuatoDisp(lvns, LVNS_EFFECT_NORMAL);
				break;
			case 1:
				KizuatoDisp(lvns, LVNS_EFFECT_NORMAL);
				break;
			case 2:
				KizuatoDisp(lvns, LVNS_EFFECT_FADE_MASK);
				break;
			}
		}
        c += 3;
        break;

    case 0x39:
        dprintf((stderr, "[0x39: 表示処理 %d]\n", c[1]));
		if (!history_mode) {
			KizuatoDisp(lvns, text_effect(c[1]));
		}
        c += 2;
        break;
        
    case 0x3a:
        dprintf((stderr, "[0x3a: パレット一時変更指定(即時反映)(%d)?\n",c[1]));
		if (!history_mode) {
			state->change_palette = c[1];
		}
        c += 2;
        break;
        
    case 0x3b:
        dprintf((stderr, "[0x3b: 表示時パレット変更指定(即時反映はしない)(%d)?\n",c[1]));
		if (!history_mode) {
			state->change_palette2 = c[1];
		}
        c += 2;
        break;
        
    case 0x3c:
        dprintf((stderr, "[0x3c: 色反転指定(%x)?\n",c[1]));
		if (!history_mode) {
			state->reverse_palette = c[1];
		}
        c += 2;
        break;
        
        /* ------------------------------------------------------ */
        
    case 0x60:
        dprintf((stderr, "[0x60: 謎 パレット反映?\n"));
        c++;
        break;
        
    case 0x61:
        dprintf((stderr, "[0x61: 謎 パレットを0に?\n"));
        c++;
        break;
            
    case 0x62:
        dprintf((stderr, "[0x62: 謎? エフェクト指定?(%x)\n",c[1]));
        c += 2;
        break;
            
    case 0x63:
        dprintf((stderr, "[0x63: 謎? エフェクト指定? (%x)\n",c[1]));
        c += 2;
        break;
        
    case 0x64:
        dprintf((stderr, "0x64:: 謎 エフェクトがらみ?\n"));
        c++;
        break;
        
    case 0x65:
        dprintf((stderr, "0x65:: 謎 エフェクトがらみ?\n"));
        c++;
        break;
        
    case 0x66:
        dprintf((stderr, "0x66:: 謎 エフェクトがらみ?\n"));
        c++;
        break;      
        
    case 0x67:
        dprintf((stderr, "[0x67: 謎? エフェクト指定?(%x)\n",c[1]));
        c += 2;
        break;

    case 0x68:
        dprintf((stderr, "[0x68: 謎? エフェクト指定? (%x)\n",c[1]));
        c += 2;
        break;

    case 0x69:
        dprintf((stderr, "0x69: フェードイン?\n"));
        c++;
        break;
        
    case 0x6a:
        dprintf((stderr, "0x6a: フェードアウト?\n"));
        c++;
        break;

    case 0x6b:
        dprintf((stderr, "[0x6b: 謎? エフェクト関連?\n"));
		if (!history_mode) {
			text_effect(c[1]);
		}
        c += 2;
        break;
        
    case 0x6c:
        dprintf((stderr, "[0x6c: 謎? エフェクト関連?\n"));
		if (!history_mode) {
			text_effect(c[1]);
		}
        c += 2;
        break;
        
        /* ------------------------------------------------- */
        
    case 0x80: /* BGM開始 */
        dprintf((stderr, "BGM 開始(%d)\n", c[1]));
		if (!history_mode) {
			LvnsStartMusicLoop(lvns, bgmmap(c[1]));
		}
        c += 2;
        break;
        
    case 0x81: /* BGM FADE */
        dprintf((stderr, "BGM フェード制御\n"));
		if (!history_mode) {
			LvnsFadeMusic(lvns);
		}
        c++;
        break;
        
    case 0x82: /* BGM 停止 */
        dprintf((stderr, "BGM停止\n"));
		if (!history_mode) {
			LvnsPauseMusic(lvns);
		}
        c++;
        break;
        
    case 0x84: /* 次シーンのBGM開始 */
        dprintf((stderr, "BGM 関連開始?(%d)\n", c[1]));
		if (!history_mode) {
			/* LvnsSetNextMusicLoop(lvns, bgmmap(c[1]));*/
		}
        c += 2;
        break;
        
    case 0x85: /* BGMフェード処理終了待ち */
        dprintf((stderr, "BGM フェード終了待ち\n"));
		if (!history_mode) {
			LvnsWaitMusicFade(lvns);
		}
        c++;
        break;

    case 0x87:
        dprintf((stderr, "BGM 開始(fadein)(%d)\n", c[1]));
		if (!history_mode) {
			LvnsStartMusicLoop2(lvns, bgmmap(c[1]));
		}
        c += 2;
        break;
       
        /* -------------------------------------------------- */

    case 0xa0:
        dprintf((stderr, "[A0 PCM読み込み(%02d)]\n", c[1]));
		if (!history_mode) {
			LvnsLoadSound(lvns, "KZ_VD%02d.P16", c[1]);
		}
        c += 2;
        break;

    case 0xa1:
        dprintf((stderr, "[A1 PCM完了]\n"));
		if (!history_mode) {
			LvnsStopSound(lvns);
		}
        c++;
        break;
                
    case 0xa2:
        dprintf((stderr, "[A2 PCM再生(%02x,%02d)]\n", c[1], c[2]));
		if (!history_mode) {
			LvnsStartSound(lvns, c[1]);
		}
        c += 3;
        break;

    case 0xa3:
        dprintf((stderr, "[A3 PCM停止待ち]\n"));
		if (!history_mode) {
			LvnsWaitSound(lvns);
		}
        c++;
        break;

    case 0xa6:
        dprintf((stderr, "[A6 PCM関連?]\n"));
        c++;
        break;
        
    case 0xaf:		/* メッセージ終了 */
        c++;
        return True;

    case 0xb0:		/* 改行 */
		LvnsNewLineText(lvns);
        c++;
        break;

    case 0xb2:		/* 入力待ち */
		if (!history_mode) {
			LvnsWaitKey(lvns);
		}
        c++;
        break;

    case 0xb3:              /* ページ更新まち */
		if (!history_mode) {
			LvnsWaitPage(lvns);
			lvns->savepoint_flag = True;
		}
        c++;
        break;
        
    case 0xb6:		/* 文字描画待ち時間指定 */
        dprintf((stderr, "文字描画速度指定(%d*10ms)\n", c[1]));
		if (!history_mode) {
			lvns->char_wait_time = c[1];
		}
        c += 2;
        break;

    case 0xb7:              /* 時間まち */
        dprintf((stderr, "時間待ち(%d*10ms)\n", c[1]));
		if (!history_mode) {
			if (!lvns->fast_text && !lvns->skip) {
				LvnsWait(lvns, c[1]);
			}
		}
        c += 2;
		break;

    case 0xb9:		/* 文字描画オフセット指定 */
        dprintf((stderr, "文字描画オフセット指定 %d\n", c[1]));
        LvnsSetTextOffset(lvns, c[1] * 8);
        c += 2;
        break;
        
    case 0xbb:		/* フラッシュエフェクト */
		dprintf((stderr, "フラッシュ(0xbb)\n"));
		if (!history_mode) {
			LvnsWhiteOut(lvns);
			LvnsWhiteIn(lvns);
		}
		c++;
        break;
        
    case 0xbc:		/* 画面振動エフェクト */
        dprintf((stderr, "「どかっ」画面振動(0xbc:%x)\n", c[1]));
		if (!history_mode) {
			LvnsClearCursor(lvns);
			LvnsVibrato(lvns);
		}
        c++;
        break;
        
    case 0xbd:		/* 通常背景ロード */
		dprintf((stderr, "背景:0xbd (%d) %d %d\n", c[1], c[2], c[3]));
		if (!history_mode) {
			LvnsUndispText(lvns);
			LvnsClear(lvns, text_effect(c[2]));
			KizuatoLoadBG(lvns, c[1]);
			KizuatoDisp(lvns, text_effect(c[3]));
		}
	    c += 4;
        break;
        
    case 0xbe:		/* 背景ロード? */
		dprintf((stderr, "背景2:0xbe %d %d %d\n", c[1], c[2], c[3]));
		if (!history_mode) {
			LvnsUndispText(lvns);
			LvnsClear(lvns, text_effect(c[2]));
			KizuatoLoadBG(lvns, c[1]);
			KizuatoDisp(lvns, text_effect(c[3]));
		}
	    c += 4;
        break;
        
    case 0xbf:		/* ビジュアルシーンロード */
		dprintf((stderr, "Visual:0xbf %d %d\n", c[2], c[3]));
		if (!history_mode) {
			LvnsUndispText(lvns);
			LvnsClear(lvns, text_effect(c[2]));
			KizuatoLoadVisual(lvns, c[1]);
			KizuatoDisp(lvns, text_effect(c[3]));
		}
	    c += 4;
        break;
        
    case 0xc0:		/* Hシーンビジュアルロード */
		dprintf((stderr, "Hシーン:0xc0 %d %d\n", c[2], c[3]));
		if (!history_mode) {
			LvnsUndispText(lvns);
			LvnsClear(lvns, text_effect(c[3]));
			KizuatoLoadHVisual(lvns, c[1]);
			KizuatoDisp(lvns, text_effect(c[3]));
		}
	    c += 4;
        break;
        
    case 0xc1:		/* キャラクタ変更 */
		dprintf((stderr, "キャラ変更 0xC1\n"));
		if (!history_mode) {
			LvnsUndispText(lvns);
			KizuatoClearCharacter(lvns, c[1]);
			KizuatoLoadCharacter(lvns, c[2], c[1]);
			KizuatoDisp(lvns, LVNS_EFFECT_FADE_MASK);
		}
		c += 3;
        break;
        
    case 0xc2:		/* キャラクタ表示 */
		dprintf((stderr, "キャラ表示 0xC2\n"));
		if (!history_mode) {
			LvnsUndispText(lvns);
			KizuatoLoadCharacter(lvns, c[2], c[1]);
			KizuatoDisp(lvns, LVNS_EFFECT_FADE_MASK);
		}
		c += 3;
        break;
        
    case 0xc3:	/* 全キャラクタを消してからキャラクタ表示 */
		dprintf((stderr, "全消去後キャラ表示 0xC3\n"));
		if (!history_mode) {
			LvnsUndispText(lvns);
			KizuatoClearCharacter(lvns, 3);
			KizuatoLoadCharacter(lvns, c[2], c[1]);      
			KizuatoDisp(lvns, LVNS_EFFECT_FADE_MASK);
		}
		c += 3;
        break;
        
    case 0xc4:	/* 背景付きキャラクタ表示 */
		dprintf((stderr, "背景つきキャラクタ表示 %d %d %d %d\n",
				 c[1], c[3],c[4],c[5]));
		if (!history_mode) {
			LvnsUndispText(lvns);
			LvnsClear(lvns, text_effect(c[4]));
			KizuatoLoadBG(lvns, c[3]);
			KizuatoLoadCharacter(lvns, c[2], c[1]);
			KizuatoDisp(lvns, text_effect(c[5]));
		}
		c += 6;
        break;
        
    case 0xc6: /* 3 枚表示 */
		dprintf((stderr, "3キャラ同時表示\n"));
		if (!history_mode) {
			LvnsUndispText(lvns);
			KizuatoClearCharacter(lvns, 3);
			KizuatoLoadCharacter(lvns, c[2], c[1]);
			KizuatoLoadCharacter(lvns, c[4], c[3]);
			KizuatoLoadCharacter(lvns, c[6], c[5]);
			KizuatoDisp(lvns, LVNS_EFFECT_FADE_MASK);
		}
		c += 7;
        break;

    case 0xc8:
        dprintf((stderr, "[0xc8: 謎 たぶん効果]\n"));
        c += 3;
        break;

    case 0xc9:
        dprintf((stderr, "[0xc9: 謎 たぶん効果]\n"));
        c++;
        break;

    case 0xca:
        dprintf((stderr, "[0xca: 鬼の爪\n"));
		if (!history_mode) {
			LvnsUndispText(lvns);
			LvnsAnimation(lvns, kizuato02);
		}
        c++;
        break;

    case 0xcb:
        dprintf((stderr, "[0xcb: 謎 たぶん効果]\n"));
        c++;
        break;

    case 0xcc: 
        dprintf((stderr, "[0xcc: うねうね ON %d %d]\n", c[1], c[2]));
		if (!history_mode) {
			if (c[1] == 1) {
				LvnsSetBackEffect(lvns, &lvnsSinEffect2);
			} else if (c[2] == 1) {
				LvnsSetBackEffect(lvns, NULL);
			}
		}
        c += 3;
        break;
        
    case 0xcd:
        dprintf((stderr, "[0xcd: ふきでる血]\n"));
		if (!history_mode) {
			LvnsAnimation(lvns, kizuato03);
		}
        c++;
        break;

    default:
        fprintf(stderr, "処理されなかったコマンドです(%02x)!!\n", c[0]);
		return True;
    }

    return False;
}
#undef c

static void
KizuatoDispText(Lvns *lvns, const u_char *c, Bool history_mode)
{
	while (1) {
		int code = c[0];
		if (code >= 0 && code <= 0x20) {
			int FontCode = (code << 8) + c[1];
			LvnsDispText(lvns);
			c += 2;
			LvnsPutChar(lvns, FontCode, lvns->text_attr);
		} else  {
			if (CommandParser(lvns, &c, history_mode))
				return;
		}
    } /* While(1) */
}

/*
 * テキストシナリオパーサ
 * 表示が終了したら True を返す
 */
static void
TextParser(Lvns *lvns, int no, Bool add_history)
{
	KizuatoSetTextScenarioState(lvns, no);
	KizuatoDispText(lvns, LvnsGetScenarioText(lvns, no),  False);
	if (add_history)
		LvnsAddHistory(lvns, no);
}


/* ------------------------------------------------------------ */
/** シナリオエンジン開始用 */

static void
KizuatoStartScenario(Lvns *lvns)
{
    KizuatoState *state = (KizuatoState*)lvns->system_state;

    /* シナリオ状態復活 */
    LvnsLoadScenario(lvns, lvns->savepoint.scn, lvns->savepoint.blk);
    lvns->scn_cur = lvns->scn_cur_head + lvns->savepoint.scn_offset;

	LvnsClearText(lvns);

    /* 画面復活 */
    if (lvns->savepoint.bg_no) {
		int i;
        switch (lvns->savepoint.bg_type) {
        case LVNS_VISUAL:
	    KizuatoLoadVisual(lvns, lvns->savepoint.bg_no);
            break;
        case LVNS_HCG:
            KizuatoLoadHVisual(lvns, lvns->savepoint.bg_no);
            break;
        default:
            KizuatoLoadBG(lvns, lvns->savepoint.bg_no);
            break;
        }
        KizuatoClearCharacter(lvns, 3);
		for (i=0;i<3;i++) {
			if (lvns->savepoint.character_no[i] != NOCHARACTER)
				KizuatoLoadCharacter(lvns, lvns->savepoint.character_no[i], i);
		}
		LvnsDisp(lvns, LVNS_EFFECT_WIPE_TTOB);
    } else {
        KizuatoLoadBG(lvns, 0);       
        KizuatoClearCharacter(lvns, 3);
    }

    /* BGM 復活 */
    if (lvns->savepoint.current_music)
        LvnsSetNextMusicLoop(lvns, lvns->savepoint.current_music);

    /* フラグ復活 */
    memcpy(state->flag, state->flag_save, sizeof state->flag);

    /* 「選択肢に戻る」初期化 */
    lvns->selectpoint = lvns->savepoint;
    memcpy(state->flag_select, state->flag_save, sizeof state->flag);

}

/* 
 * 痕シナリオパーサ 
 */
#define c lvns->scn_cur
void 
KizuatoMain(Lvns *lvns)
{
    KizuatoState *state = (KizuatoState *)lvns->system_state;
	KizuatoStartScenario(lvns);

	while(1) {

		lvns->inside_state = LVNS_MAIN;

		/* セーブポイント設定 */
		if (lvns->savepoint_flag) {
			LvnsSetSavePoint(lvns, &lvns->savepoint);
			memcpy(state->flag_save, state->flag, sizeof state->flag);
			lvns->savepoint_flag = False;
		}

		/* メニュー起動 */


        switch (c[0]) {
        case 0x20: /* 終了 */
            dprintf((stderr, "[END]\n"));
            c ++;
			return;

        case 0x24: /* ジャンプ命令 */
            dprintf((stderr, "[ジャンプ SCN%03d.DAT - Block %d]\n", c[1], c[2]));
            LvnsLoadScenario(lvns, c[1], c[2]);
			break;

        case 0x25: /* 選択肢 */
			dprintf((stderr, "[選択肢(%d)]-[メッセージ:%d]\n", c[2], c[1]));
            {
                int i;
				TextParser(lvns, c[1], True);
                for (i = 0; i < c[2]; i++) {
                    dprintf((stderr,
                             "[選択肢 %d]-[メッセージ:%d]-[オフセット:%02x]\n",
                             i, c[3 + i*2], c[4 + i*2]));
					lvns->text_attr = i+1;
					TextParser(lvns, c[3+i*2], False);
                }
				lvns->text_attr = 0;
				i = LvnsWaitSelect(lvns, c[2]);
				LvnsAddHistory(lvns, c[3+i*2]);
				
				dprintf((stderr, "選択分岐: %d (+%02x)\n", i, c[4 + i*2]));

				c = c + 3 + c[2] * 2 + c[4 + i*2];
            }
            break;

        case 0x27:
            dprintf((stderr, "[前の選択肢に戻るマーク位置]\n"));
			/* 状態を保存… */
            LvnsSetSavePoint(lvns, &lvns->selectpoint);
            memcpy(state->flag_select, state->flag, sizeof state->flag);
            c ++;
            break;

        case 0x41: /* if 文 */
            dprintf((stderr, "[if flg:%02x == 0x%02x pc += %02x]\n", 
                     c[1], c[2], c[3]));
            if (state->flag[flagmap(c[1])] == c[2])
                lvns->scn_cur += c[3];
            c += 4;
            break;

        case 0x42: /* if 文 (否定) */
            dprintf((stderr, "[if flg:%02x != 0x%02x pc += %02x]\n", 
                     c[1], c[2], c[3]));
            if (state->flag[flagmap(c[1])] != c[2])
                lvns->scn_cur += c[3];
            c += 4;
            break;

        case 0x4b: /* フラグセット */
            dprintf((stderr, "[flg:%02x = 0x%02x]\n", c[1], c[2]));
            state->flag[flagmap(c[1])] = c[2];
            c += 3;
            break;

        case 0x51: /* メッセージ出力 */
            dprintf((stderr, "[メッセージ:%d]\n", c[1])); 
			TextParser(lvns, c[1], True);
            c += 2;
            break;

        case 0x52:		/* 選択肢に存在する。無くても大丈夫か? */
            dprintf((stderr, "[謎 0x%02x]\n", c[0]));
            c ++;
            break;

        case 0x94:		/* 不明。多分引数0 */
            dprintf((stderr, "[エンディング関連 0x%02x]\n", c[0]));
            c ++;
            break;

        case 0x95: /* エンディングBGM 選択 */
            dprintf((stderr, "[エンディング BGM 開始 0x%02x] %d\n", c[0], c[1]));
            /* エンディングテーマ演奏開始 */
            LvnsStartMusic(lvns, bgmmap(c[1]));

            /* エンディング起動 */
            KizuatoEnding(lvns);

            /* 最終的なフラグを反映… */
            memcpy(state->flag_save, state->flag, sizeof state->flag);

            /* エンディング到達につきシナリオデータ初期化 */
            KizuatoScenarioInit(lvns);
            KizuatoSave(lvns);

            c += 2;
            break;

        case 0x96: /* エンディング番号指定 */
            dprintf((stderr, "[エンディング番号指定0x%02x] %d\n", c[0], c[1]));
/*
   0  25 千鶴 True 
   8  24 楓 Happy
   10 23 柳川 END
   15 23 ガチャピンエンド
   16 15 食卓
*/
            state->ending[c[1]] = 1;
            {
                int i;
                for (i=0; i<sizeof state->ending;i++) {
                    if (state->ending[i])
                        break;
                }
                if (i == sizeof state->ending) {
                    state->flag[flagmap(0x73)] = 1;
                }
            }
            c += 2;
            break;

        default:
            if (CommandParser(lvns, &lvns->scn_cur, False))
				return;
        }
    } /* while(1) */
}
#undef c

/**
 * 開始直前の初期化
 */
void
KizuatoStart(Lvns *lvns)
{
    lvns->setPaletteIndex(lvns, KIZUATO_COL_WHITE, 255, 255, 255);
    lvns->setPaletteIndex(lvns, KIZUATO_COL_GRAY,  127, 127, 127);
    lvns->vram->black       = KIZUATO_COL_BLACK;
    lvns->background->black = KIZUATO_COL_BLACK;
    LvnsInitAnimation(lvns, kizuato01);
    LvnsInitAnimation(lvns, kizuato02);
    LvnsInitAnimation(lvns, kizuato03);
}

void
KizuatoDispHistory(Lvns *lvns, int pos)
{
	LvnsClearText(lvns);         /* 消去 */
	LvnsDispWindow(lvns);
	if (pos >= 0 && pos < lvns->history_pos) {
		LvnsLoadScenario(lvns, lvns->history[pos].scn, lvns->history[pos].blk);
		KizuatoDispText(lvns, LvnsGetScenarioText(lvns, lvns->history[pos].no), True);
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

