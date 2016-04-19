/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: LvnsEffect.c,v 1.21 2001/08/18 02:53:09 tf Exp $
 */

/*
 * Lvns 描画エフェクト処理
 * 消去/描画時のエフェクトを処理する。
 */

#include <stdio.h>
#include <stdlib.h>
#include "LvnsEffect.h"

static Bool
set_waittime(Lvns *lvns, int time)
{
    long msec = lvns->getTimer(lvns, LVNSTIMER_EFFECT);
    if (msec < time) {
        return False;
    } else
        return True;
}

/*
 * マスクによる段階表示
 */
static int
FadeMask(Lvns *lvns, int *state)
{
    int i, j;

	/*
    if (!set_waittime(lvns, *state * 30))
	return False;
	*/

    for (i=0; i < HEIGHT; i+=16) {
        for (j=0; j < WIDTH; j+=16) {
            lvns->drawWindowMask(lvns, j, i, *state);
        }
    }   
    lvns->flushWindow(lvns);

    LvnsWait(lvns, 1);

    if (++*state == 16) {
        return True;                  /* 処理終了 */
    } else {
        return False;
    }
}

/*
 * マスク(矩形)による段階表示
 */
static int
FadeSquare(Lvns *lvns, int *state)
{
    int i, j;

    for (i=0; i < HEIGHT; i+=32) {
        for (j=0; j < WIDTH; j+=32) {
            lvns->drawWindowSquareMask(lvns, j, i, *state);
        }
    }
    lvns->flushWindow(lvns);

    if (++*state == 31) {
        return True;                  /* 処理終了 */
    } else {
        return False;
    }
}

/*
 * パレットホワイトアウト 
 * 画像 → 白
 */
static int
WhiteOut(Lvns *lvns, int *state)
{
    lvns->setPaletteScreen(lvns, *state);
    lvns->drawWindow(lvns);
    lvns->flushWindow(lvns);

    if (++*state > 16) {
        return True;                  /* 処理終了 */
    } else {
        return False;
    }
}

/*
 * パレットホワイトイン 
 * 白 → 画像 
 */
static int
WhiteIn(Lvns *lvns, int *state)
{
    lvns->setPaletteScreen(lvns, (16 - *state));
    lvns->drawWindow(lvns);
    lvns->flushWindow(lvns);

    if (++*state > 16) {
        return True;                  /* 処理終了 */
    } else {
        return False;
    }
}

/*
 * 上から下へワイプ
 */
static int
WipeTtoB(Lvns *lvns, int *state)
{
    int i;

    for (i=0;i<16;i++) {
        int y = (*state - 15 + i) * 16 + i;
        if (y >= 0 && y < HEIGHT) {
            lvns->drawWindowArea(lvns, 0, y, WIDTH, 1, 0, y);
        }
    }
    lvns->flushWindow(lvns);

    if (++*state == (HEIGHT / 16) + 16) {
        return True;                  /* 処理終了 */
    } else {
        return False;
    }
}

/*
 * 左から右へワイプ
 */
static int
WipeLtoR(Lvns *lvns, int *state)
{
    int i;

    for (i=0;i<16;i++) {
        int x = (*state - 15 + i) * 16 + i;
        if (x >= 0 && x < WIDTH) {
            lvns->drawWindowArea(lvns, x, 0, 1, HEIGHT, x, 0);
            lvns->flushWindowArea(lvns, x, 0, 1, HEIGHT);
        }
    }

    if (++*state == 40 + 16) {
        return True;                  /* 処理終了 */
    } else {
        return False;
    }
}

/*
 *マスクパターンによる左から右へのワイプ
*/
static int
WipeMaskLtoR(Lvns *lvns, int *state)
{
    int i, j;

    for (i = 0; i < 16; i++) {
        int x = (*state - i) * 16;
        if (x >= 0 && x < WIDTH) {
            for (j=0; j < HEIGHT; j+= 16) {
                lvns->drawWindowMask(lvns, x, j, i);
                lvns->flushWindowArea(lvns, x, j, 16, 16);
            }
        }
    }

    if (++*state == 40 + 16) {
        return True;                  /* 処理終了 */
    } else {
        return False;
    }
}

/*
 * 菱形での左から右へのワイプ
 */
static int
WipeSquareLtoR(Lvns *lvns, int *state)
{
    int i, j, x;

    for (i = 0; i < 32; i++) {
        x = (*state - i) * 32;
        if (x >= 0 && x < WIDTH) {
            for (j = 0; j < HEIGHT; j+= 32) {
                lvns->drawWindowSquareMask(lvns, x, j, i);
                lvns->flushWindowArea(lvns, x, j, 32, 32);
            }
        }
    }

    if (++*state == 20 + 31) {
        return True;                  /* 処理終了 */
    } else {
        return False;
    }
}

/*
 * 横スライド
 */
static int
SlideLtoR(Lvns *lvns, int *state)
{
    int i;

    for (i=0;i<40;i++) {
        int x = i*16 + *state;
        lvns->drawWindowArea(lvns, x, 0, 1, HEIGHT, x, 0);
    }
    lvns->flushWindow(lvns);

    LvnsWait(lvns, 1);

    if (++*state == 16) {
        return True;                  /* 処理終了 */
    } else {
        return False;
    }
}

/* 
 * 回転消去 (外側から)
 */
static Bool
GURUGURUClear(Lvns *lvns, int *state)
{
    static int lenx;
    static int leny;
    static int dir;
    static int len;
    static int x;
    static int y;
	
    int cnt = 0;
    for (cnt=0; cnt < 10 && leny >= 0; cnt++) {
		
		if (*state == 0) {
			dir = 0;
			len = 0;
			x   = 0;
			y   = 0;
			lenx = (WIDTH /16);
			leny = (HEIGHT/16)-1;
		}
		
		lvns->clearWindowArea(lvns, x*16, y*16, 16, 16);
		lvns->flushWindowArea(lvns, x*16, y*16, 16, 16);
		
		len++;
		switch (dir){
		case 0:
			if (len >= lenx) {
				lenx--;
				len = 0;
				dir = 1;
				y++;
			} else
				x++;
			break;
		case 1:
			if (len >= leny) {
				leny--;
				len = 0;
				dir = 2;
				x--;
			} else
				y++;
			break;
		case 2:
			if (len >= lenx) {
				lenx--;
				len = 0;
				dir = 3;
				y--;
			} else
				x--;
			break;
		case 3:
			if (len >= leny) {
				leny--;
				len = 0;
				dir = 0;
				x++;
			} else
				y--;
			break;
		}
		
		(*state)++;
		
    }

    if (leny < 0) {
        return True;                  /* 処理終了 */
    } else {
        return False;
    }
}

/*
 * 回転描画 (内側から)
 */
static Bool
GURUGURUDisp(Lvns *lvns, int *state)
{
    static int lenx;
    static int leny;
    static int dir;
    static int len;
    static int x;
    static int y;

#ifndef USE_MGL
#define BSIZE 16
#define FACTOR 32
#else
#define BSIZE 8
#define FACTOR 16
#endif

    int cnt = 0;
    for (cnt=0; cnt < 10 && leny >= 0; cnt++) {
		
		if (*state == 0) {
			dir = 0;
			len = 0;
			x   = 27;
			y   = (HEIGHT) / FACTOR;
			lenx = 16;
			leny = 1;
		}
		
		lvns->drawWindowArea(lvns, x*BSIZE, y*BSIZE, BSIZE, BSIZE, x*BSIZE, y*BSIZE);
		lvns->flushWindowArea(lvns, x*BSIZE, y*BSIZE, BSIZE, BSIZE);
		
		len++;
		switch (dir){
		case 0:
			x--;
			if (len == lenx) {
				lenx++;
				len = 0;
				dir = 1;
			}
			break;
		case 1:
			y++;
			if (len == leny) {
				leny++;
				len = 0;
				dir = 2;
			}
			break;
		case 2:
			x++;
			if (len == lenx) {
				lenx++;
				len = 0;
				dir = 3;
			}
			break;
		case 3:
			y--;
			if (len == leny) {
				leny++;
				len = 0;
				dir = 0;
			}
			break;
		}
		
		(*state)++;
    }
	
    if (y < 0) {
        return True;                  /* 処理終了 */
    } else {
        return False;
    }
}

/*
 * 縦方向に縮みながら消去
 */
static int
VertCompositionClear(Lvns *lvns, int *state)
{
    int i, y, y2;

    y  = *state * 8;
    y2 = HEIGHT/2-y-1;

    /* 描画 */
    lvns->clearWindowArea(lvns, 0, 0, WIDTH, y);
#ifndef USE_MGL
    lvns->clearWindowArea(lvns, 0, HEIGHT-1-y, WIDTH, y);
#else
    lvns->clearWindowArea(lvns, 0, HEIGHT-y, WIDTH, y);
#endif
    for (i=0; i<y2;i++) {
        int pos = i * (HEIGHT/2) / y2;
        lvns->drawWindowArea(lvns, 0, pos, WIDTH, 1, 0, y+i+1);
	lvns->drawWindowArea(lvns, 0, HEIGHT-1-pos, WIDTH, 1, 
                               0,  HEIGHT-1-y-i-1);
    }
    lvns->flushWindow(lvns);

    if (++*state > HEIGHT/2/8) {
        return True;                  /* 処理終了 */
    } else {
        return False;
    }
}

/*
 * 縦方向に延びながら描画
 */
static int
VertCompositionDisp(Lvns *lvns, int *state)
{
    int i, y;

    y  = *state * 8 + 1;

    /* 描画 */
    for (i=0; i<y;i++) {
        int pos = i * (HEIGHT/2) / y;
        lvns->drawWindowArea(lvns, 0, pos, WIDTH, 1, 0, HEIGHT/2 - y + i + 1);
        lvns->drawWindowArea(lvns, 0, HEIGHT-1-pos, 
                               WIDTH, 1, 0, HEIGHT/2 + y - i - 1);
    }
    lvns->flushWindow(lvns);

    if (++*state > HEIGHT/2/8) {
        return True;                  /* 処理終了 */
    } else {
        return False;
    }
}

/* 
 * 左にスクロールして消去
 */
static int
LeftScrollClear(Lvns *lvns, int *state)
{
    int x;

    x  = *state * 8;

    /* 描画 */
    lvns->clearWindowArea(lvns, (WIDTH-x), 0, 8, HEIGHT);
    lvns->drawWindowArea(lvns, x, 0, WIDTH-x, HEIGHT, x, 0);
    lvns->flushWindow(lvns);

    if (++*state > WIDTH/8) {
        return True;                  /* 処理終了 */
    } else {
        return False;
    }
}

/* 
 * 左からスクロールイン
 */
static int
LeftScrollDisp(Lvns *lvns, int *state)
{
    int x;

    x  = *state * 8;

    /* 描画 */
    lvns->drawWindowArea(lvns, WIDTH-x, 0, x, HEIGHT, WIDTH-x, 0);
    lvns->flushWindow(lvns);

    if (++*state > WIDTH/8) {
        return True;                  /* 処理終了 */
    } else {
        return False;
    }
}

/* 
 * 上にスクロールして消去
 */
static int
TopScrollClear(Lvns *lvns, int *state)
{
    int y;

    y  = *state * 8;

    /* 描画 */
    lvns->clearWindowArea(lvns, 0, (HEIGHT-y), WIDTH, y);
    lvns->drawWindowArea(lvns, 0, y, WIDTH, HEIGHT-y, 0, y);
    lvns->flushWindow(lvns);

    if (++*state > WIDTH/8) {
        return True;                  /* 処理終了 */
    } else {
        return False;
    }
}

/* 
 * 上からスクロールイン
 */
static int
TopScrollDisp(Lvns *lvns, int *state)
{
    int y;

    y  = *state * 8;

    /* 描画 */
    lvns->clearWindowArea(lvns, 0, (HEIGHT-y), WIDTH, y);
    lvns->drawWindowArea(lvns, 0, HEIGHT-y, WIDTH, y, 0, HEIGHT-y);
    lvns->flushWindow(lvns);

    if (++*state > HEIGHT/8) {
        return True;                  /* 処理終了 */
    } else {
        return False;
    }
}

#ifndef USE_MGL
#define TSIZE 16
#else
#define TSIZE 8
#endif

static int
SlantTile(Lvns *lvns, int *state)
{
    int i, j, k;

    for (i=0;i<HEIGHT/TSIZE;i++) {
        for (j=0;j<4;j++) {
            int x = j * 10 + i;
            int num =  *state + (random() % 3) -1;
            for (k=0; k<num;k++,x++) {
                if (x >= 40)
                    x -= 40;
                lvns->drawWindowArea(lvns, x*TSIZE, i*TSIZE, TSIZE, TSIZE, x*TSIZE, i*TSIZE);
            }
        }
    }
    lvns->flushWindow(lvns);

    if (++*state > 12){
        return True;                  /* 処理終了 */
    } else {
        return False;
    }
}

/*
 * 画面を振動させる
 */
static int
Vibrato(Lvns *lvns, int *state)
{
#define VIB 16
    lvns->setDispOffset(lvns, 
                          (random() % (VIB*2+1)) - VIB, 
                          (random() % (VIB*2+1)) - VIB,
                          VIB);
    if (++*state > 15){
        lvns->setDispOffset(lvns, 0, 0, VIB);
        return True;                  /* 処理終了 */
    } else {
        return False;
    }
#undef VIB
}

/*
 * 円で消去
 */
static int
CircleClear(Lvns *lvns, int *state)
{
    int r;

    r  = 380 - *state*8;

    lvnsimage_circle(lvns->vram, WIDTH/2, HEIGHT/2, r, 10, 0);
    lvns->flushWindow(lvns);

    ++*state;
    if (r <= 0) {
        return True;                  /* 処理終了 */
    } else {
        return False;
    }
}

/*
 * ランダム行表示
 */
#ifndef USE_MGL
#define LINE 480
static u_short random_line[] = {
252,146,186,1,48,182,204,392,426,99,247,463,88,174,40,193,4,
110,63,15,443,272,135,453,395,322,466,393,238,190,202,28,124,
176,115,171,83,170,307,85,81,3,10,216,418,191,353,102,417,
246,104,325,245,54,80,345,42,235,460,223,427,217,366,280,27,
213,142,376,230,147,444,292,414,430,359,309,435,64,109,249,20,
369,407,459,75,375,329,274,58,362,368,131,348,60,346,228,71,
452,43,406,438,132,465,477,133,374,140,251,229,127,125,35,431,
471,303,253,47,55,259,273,69,236,158,404,59,157,255,233,349,
278,270,403,86,139,51,434,261,297,282,192,470,472,401,154,396,
267,293,221,237,112,421,361,130,241,144,181,240,198,286,167,446,
87,25,410,227,11,173,377,402,364,263,295,188,318,411,300,385,
429,180,320,38,254,33,21,94,101,199,24,408,189,17,256,116,
52,120,336,91,155,70,425,380,285,468,107,277,44,166,405,67,
351,279,464,6,45,331,90,337,205,394,82,448,343,328,420,72,
76,111,119,413,462,473,289,458,372,172,207,388,363,105,288,315,
324,317,106,41,0,330,123,232,432,95,476,305,19,299,185,467,
321,161,201,313,287,78,118,137,222,381,9,113,469,79,298,36,
122,23,92,97,258,18,447,342,103,314,312,340,387,56,262,143,
456,212,165,433,2,214,450,455,383,209,98,399,200,218,145,53,
266,225,424,296,178,128,428,141,409,391,151,177,203,422,93,445,
134,163,316,197,386,306,423,46,211,378,164,68,220,231,30,224,
206,84,347,183,327,257,159,148,117,457,150,275,244,439,308,39,
400,29,226,7,271,37,461,195,415,65,291,34,389,169,310,357,
49,13,333,243,367,437,219,332,370,338,281,356,31,294,283,373,
208,390,149,265,268,215,301,302,344,319,365,323,66,126,260,398,
239,451,339,379,382,57,284,61,5,334,74,114,234,210,276,358,
100,475,26,341,136,248,168,16,187,156,179,73,175,242,8,290,
22,371,121,14,152,436,153,449,108,454,269,474,194,62,416,419,
89,264,50,354,335,32,440,311,412,96,162,384,196,250,478,350,
479,442,77,184,355,138,12,441,352,129,360,304,397,326,160
};
#else
#define LINE 240
static u_short random_line[] = {
45, 227, 94, 233, 132, 205, 7, 185, 82, 13, 152, 2, 33, 143, 209, 86,
123, 6, 29, 9, 1, 74, 71, 10, 216, 89, 182, 234, 20, 68, 237, 127,
204, 188, 52, 21, 136, 50, 238, 108, 72, 186, 164, 160, 88, 170, 180,
67, 43, 115, 118, 48, 111, 27, 113, 23, 16, 203, 189, 149, 187, 224,
229, 116, 150, 30, 42, 99, 97, 0, 65, 61, 85, 173, 198, 134, 25, 217,
122, 121, 83, 161, 197, 47, 49, 101, 34, 15, 169, 4, 128, 200, 37,
114, 100, 154, 73, 131, 64, 207, 93, 31, 142, 98, 153, 177, 79, 26,
172, 19, 125, 212, 66, 230, 220, 107, 222, 106, 171, 87, 80, 36, 218,
232, 130, 193, 35, 40, 90, 17, 112, 24, 46, 210, 51, 91, 231, 178,
109, 199, 139, 159, 129, 102, 158, 117, 179, 56, 60, 196, 137, 151,
110, 157, 8, 144, 174, 201, 183, 126, 103, 11, 63, 69, 78, 213, 239,
22, 215, 104, 105, 95, 191, 163, 58, 120, 14, 5, 202, 168, 214, 138,
96, 59, 3, 39, 12, 166, 194, 165, 167, 44, 192, 155, 38, 140, 32, 135,
124, 57, 156, 147, 236, 76, 208, 41, 53, 235, 145, 219, 228, 184, 133,
176, 206, 190, 77, 226, 221, 70, 55, 146, 62, 223, 92, 54, 148, 175,
195, 81, 28, 141, 181, 211, 119, 75, 162, 225, 84, 18
};
#endif

static int
RandomRaster(Lvns *lvns, int *state)
{
    int y;

    y = random_line[*state];
    lvns->drawWindowArea(lvns, 0, y, WIDTH, 1, 0, y);
    lvns->flushWindowArea(lvns, 0, y, WIDTH, 1);

    if (++*state > LINE){
        return True;                  /* 処理終了 */
    } else {
        return False;
    }
}
#undef LINE

/*
 * パレットフェードアウト
 * to_latitude の値まで暗くする
 */
Bool
Darken(Lvns *lvns, int to_latitude, int diff)
{
    lvns->latitude -= diff;
    if (lvns->latitude < to_latitude)
		lvns->latitude = to_latitude;
	
    lvns->setPaletteMulti(lvns, lvns->latitude);
    lvns->drawWindow(lvns);
    lvns->flushWindow(lvns);
	
    if (lvns->latitude == to_latitude) {
        return True;                  /* 処理終了 */
    } else {
        return False;
    }
}

/*
 * パレットフェードイン 
 * to_latitude の値まで明るくする
 */
static Bool
Lighten(Lvns *lvns, int to_latitude, int diff)
{
    lvns->latitude += diff;
    if (lvns->latitude > to_latitude)
		lvns->latitude = to_latitude;

    lvns->setPaletteMulti(lvns, lvns->latitude);
    lvns->drawWindow(lvns);
    lvns->flushWindow(lvns);
	
    if (lvns->latitude == to_latitude) {
        return True;                  /* 処理終了 */
    } else {
        return False;
    }
}

/*
 * エフェクト処理実行
 */
static Bool
DispEffect(Lvns *lvns, LvnsEffectType effect, int *state)
{
    if (!lvns->enable_effect) {
        lvns->drawWindow(lvns);
        lvns->flushWindow(lvns);
        return True;
    }

    switch (effect) {
	case LVNS_EFFECT_NO_EFFECT:
		return True;

    case LVNS_EFFECT_NORMAL:
        lvns->drawWindow(lvns);
        lvns->flushWindow(lvns);
        return True;

    case LVNS_EFFECT_FADE_MASK:
        return FadeMask(lvns, state);
		
    case LVNS_EFFECT_WIPE_TTOB:
        return WipeTtoB(lvns, state);

    case LVNS_EFFECT_WIPE_LTOR:
        return WipeLtoR(lvns, state);

    case LVNS_EFFECT_FADE_PALETTE:
		return Lighten(lvns, 16, 1);

    case LVNS_EFFECT_WIPE_MASK_LTOR:
        return WipeMaskLtoR(lvns, state);

    case LVNS_EFFECT_FADE_SQUARE:
        return FadeSquare(lvns, state);

    case LVNS_EFFECT_WIPE_SQUARE_LTOR:
        return WipeSquareLtoR(lvns, state);

    case LVNS_EFFECT_SLIDE_LTOR:
        return SlideLtoR(lvns, state);

    case LVNS_EFFECT_SLANTTILE:
        return SlantTile(lvns, state);

    case LVNS_EFFECT_VERTCOMPOSITION:
		return VertCompositionDisp(lvns, state);

    case LVNS_EFFECT_GURUGURU:
		return GURUGURUDisp(lvns, state);
		
    case LVNS_EFFECT_LEFT_SCROLL:
		return LeftScrollDisp(lvns, state);
		
    case LVNS_EFFECT_TOP_SCROLL:
		return TopScrollDisp(lvns, state);
		
    case LVNS_EFFECT_RAND_RASTER:
        return RandomRaster(lvns, state);
		
    default:
		dprintf((stderr, "未実装のエフェクトです\n"));
        return True;
    }
}

/*
 * エフェクト処理実行
 */
static Bool
ClearEffect(Lvns *lvns, LvnsEffectType effect, int *state)
{
    if (!lvns->enable_effect) {
        lvns->drawWindow(lvns);
        lvns->flushWindow(lvns);
        return True;
    }

    switch (effect) {
	case LVNS_EFFECT_NO_EFFECT:
		return True;

    case LVNS_EFFECT_NORMAL: 
    	lvns->drawWindow(lvns);
    	lvns->flushWindow(lvns);
        return True;
		
    case LVNS_EFFECT_FADE_MASK:
        return FadeMask(lvns, state);
		
    case LVNS_EFFECT_WIPE_TTOB:
        return WipeTtoB(lvns, state);
		
    case LVNS_EFFECT_WIPE_LTOR:
        return WipeLtoR(lvns, state);
		
    case LVNS_EFFECT_FADE_PALETTE:
		{
			int ret = Darken(lvns, 0, 1);
			if (ret) {
        		lvnsimage_clear(lvns->vram);
    			lvns->drawWindow(lvns);
    			lvns->flushWindow(lvns);
			}
			return ret;
		}
		
    case LVNS_EFFECT_WIPE_MASK_LTOR:
        return WipeMaskLtoR(lvns, state);
		
    case LVNS_EFFECT_FADE_SQUARE:
        return FadeSquare(lvns, state);

    case LVNS_EFFECT_WIPE_SQUARE_LTOR:
        return WipeSquareLtoR(lvns, state);

    case LVNS_EFFECT_SLIDE_LTOR:
        return SlideLtoR(lvns, state);

    case LVNS_EFFECT_SLANTTILE:
        return SlantTile(lvns, state);

    case LVNS_EFFECT_VERTCOMPOSITION:
		return VertCompositionClear(lvns, state);
		
    case LVNS_EFFECT_GURUGURU:
		return GURUGURUClear(lvns, state);
		
    case LVNS_EFFECT_CIRCLE_SHRINK:
        return CircleClear(lvns, state);
		
    case LVNS_EFFECT_LEFT_SCROLL:
		return LeftScrollClear(lvns, state);
		
    case LVNS_EFFECT_TOP_SCROLL:
		return TopScrollClear(lvns, state);
		
    case LVNS_EFFECT_RAND_RASTER:
        return RandomRaster(lvns, state);
		
    default:
		dprintf((stderr, "未実装のエフェクトです\n"));
        return True;
    }
}

void
LvnsClearLow(Lvns *lvns, LvnsEffectType effect) 
{
	int state = 0;

    switch (effect) {
    case LVNS_EFFECT_FADE_PALETTE:
    case LVNS_EFFECT_VERTCOMPOSITION:
    case LVNS_EFFECT_LEFT_SCROLL:
    case LVNS_EFFECT_TOP_SCROLL:
		/* 現在の描画内容を利用 */
		break;
    default:
		/* 「黒」による再描画として処理 */
        lvnsimage_clear(lvns->vram);
        break;
    }

	if (effect != LVNS_EFFECT_FADE_PALETTE) {
		LvnsSetBackEffect(lvns, NULL);
	}

	while (!ClearEffect(lvns, effect, &state))
		LvnsFlip(lvns, False);
	LvnsFlip(lvns, False);


    switch (effect) {
    case LVNS_EFFECT_FADE_PALETTE:
    case LVNS_EFFECT_VERTCOMPOSITION:
    case LVNS_EFFECT_LEFT_SCROLL:
    case LVNS_EFFECT_TOP_SCROLL:
		lvnsimage_clear(lvns->vram);
		break;
    default:
        break;
    }
}

void
LvnsDispLow(Lvns *lvns, LvnsEffectType effect) 
{
	int state = 0;

	// エフェクトの初期化
    switch (effect) {
    case LVNS_EFFECT_FADE_PALETTE:  /* 独自パレット設定 */
		lvns->latitude = 0;
		break;
    default:
#if 0
      /* ここで NULL を設定すると LvnsSetNextBackEffect が動かない */
      		LvnsSetBackEffect(lvns, NULL);
#endif
        lvns->setPaletteMulti(lvns, lvns->latitude);
        break;
    }
    lvns->resetTimer(lvns, LVNSTIMER_EFFECT);

	while (!DispEffect(lvns, effect, &state)) 
		LvnsFlip(lvns, False);
	LvnsFlip(lvns, False);
}

void
LvnsLighten(Lvns *lvns)
{
	if (!lvns->enable_effect) {
	  	lvns->latitude = 16;
		lvns->drawWindow(lvns);
		lvns->flushWindow(lvns);
		return;
	}

	while (!Lighten(lvns, 16, 1))
		LvnsFlip(lvns, False);
	LvnsFlip(lvns, False);
}

void
LvnsDarken(Lvns *lvns)
{
	if (!lvns->enable_effect) {
	  	lvns->latitude = lvns->latitude_dark;
		lvns->drawWindow(lvns);
		lvns->flushWindow(lvns);
		return;
	}

	while (!Darken(lvns, lvns->latitude_dark, 1))
		LvnsFlip(lvns, False);
	LvnsFlip(lvns, False);
}

void
LvnsVibrato(Lvns *lvns) 
{
	int state = 0;

	if (!lvns->enable_effect) {
	  	state = 16;
	}

	for (;;) {
		if (Vibrato(lvns, &state)) {
			break;
		} else
			LvnsFlip(lvns, False);
	}
	LvnsFlip(lvns, False);
}

void
LvnsWhiteOut(Lvns *lvns) 
{
	int state = 0;

	if (!lvns->enable_effect) {
	  	state = 16;
	}

	for (;;) {
		if (WhiteOut(lvns, &state)) {
			break;
		} else
			LvnsFlip(lvns, False);
	}
	LvnsFlip(lvns, False);
}

void
LvnsWhiteIn(Lvns *lvns)
{
	int state = 0;

	if (!lvns->enable_effect) {
	  	state = 16;
	}

	for (;;) {
		if (WhiteIn(lvns, &state)) {
			break;
		} else
			LvnsFlip(lvns, False);
	}
	LvnsFlip(lvns, False);
}	

const char*
LvnsEffectName(LvnsEffectType type)
{
    static const char *effect_type[] = {
        "未知",
        "何もしない",
        "通常表示",
        "マスクフェード",
        "上からワイプ",
        "左からワイプ",
        "Palette フェード",
        "Palette ホワイトアウト",
        "左からワイプ(マスク)",
        "ひし形◆フェード",
        "ひし形◆左ワイプ",
        "左から縦スライド",
        "斜ずれ横モザイクスライド",
        "ぐるぐる",
        "縦方向ではさみこみ",
        "中心方向にちじむ",
        "左スクロール",
        "上スクロール",
        "ランダム行表示",
        "血がだらだら"
    };
    return effect_type[type];
}

