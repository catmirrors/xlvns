/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: kizuato_ed.c,v 1.17 2001/08/02 20:05:30 go Exp $
 *
 */

/*
 * º¯ ¥¨¥ó¥Ç¥£¥ó¥°
 */

#include <stdio.h>
#include "kizuato.h"

#define EDCOL 4
#define EDSHA 0

static int
tputs(Lvns *lvns, LvnsScript *scr, void *param1, void *param2)
{
    char *str = param1;
    int x = (WIDTH2 - strlen(str) * CHARSIZE/2)/2;
    int y = (int)param2 * EDYOFF;
    KizuatoPutsVRAM(lvns, x+1, y+1, (u_char*)param1, EDSHA);
    KizuatoPutsVRAM(lvns, x+2, y+2, (u_char*)param1, EDSHA);
    KizuatoPutsVRAM(lvns, x  , y  , (u_char*)param1, EDCOL);
    return True;
}

static int
DispMoji(Lvns *lvns, LvnsScript *scr, 
         void *param1, void *param2)
{
    lvns->drawWindow(lvns);
    lvns->flushWindow(lvns);
    return True;
}

/* º¯ ¥¨¥ó¥Ç¥£¥ó¥°¥¹¥¯¥ê¥×¥È */
static LvnsScriptData eddata[] = {
    { LVNS_SCRIPT_CLEAR, (void*)LVNS_EFFECT_FADE_PALETTE },

    { LVNS_SCRIPT_BG, (void*)1 },
    { LVNS_SCRIPT_DISP, (void*)LVNS_EFFECT_FADE_PALETTE },

    { LVNS_SCRIPT_WAIT, (void*)1000 },
    { LVNS_SCRIPT_FUNC, tputs, "´ë²è", (void*)5 },
    { LVNS_SCRIPT_FUNC, tputs, "¹â¶¶¡¡Î¶Ìé", (void*)7 },
    { LVNS_SCRIPT_FUNC, DispMoji },

    { LVNS_SCRIPT_BG, (void*)6 },
    { LVNS_SCRIPT_WAIT, (void*)6000 },
    { LVNS_SCRIPT_CLEAR, (void*)LVNS_EFFECT_FADE_PALETTE },
    { LVNS_SCRIPT_DISP, (void*)LVNS_EFFECT_FADE_PALETTE },

    { LVNS_SCRIPT_WAIT, (void*)1000 },
    { LVNS_SCRIPT_FUNC, tputs, "µÓËÜ", (void*)5 },
    { LVNS_SCRIPT_FUNC, tputs, "¹â¶¶¡¡Î¶Ìé", (void*)7 },
    { LVNS_SCRIPT_FUNC, DispMoji },

    { LVNS_SCRIPT_BG, (void*)7 },
    { LVNS_SCRIPT_WAIT, (void*)6000 },
    { LVNS_SCRIPT_CLEAR, (void*)LVNS_EFFECT_FADE_PALETTE },
    { LVNS_SCRIPT_DISP, (void*)LVNS_EFFECT_FADE_PALETTE },

    { LVNS_SCRIPT_WAIT, (void*)1000 },
    { LVNS_SCRIPT_FUNC, tputs, "¥­¥ã¥é¥¯¥¿¡¼¸¶°Æ", (void*)5 },
    { LVNS_SCRIPT_FUNC, tputs, "¹âÉ§¡¡Î¶ºÈ", (void*)7 },
    { LVNS_SCRIPT_FUNC, DispMoji },

    { LVNS_SCRIPT_BG,(void*) 8 },
    { LVNS_SCRIPT_WAIT, (void*)6000 },
    { LVNS_SCRIPT_CLEAR,(void*) LVNS_EFFECT_FADE_PALETTE },
    { LVNS_SCRIPT_DISP, (void*)LVNS_EFFECT_FADE_PALETTE },

    { LVNS_SCRIPT_WAIT, (void*)1000 },
    { LVNS_SCRIPT_FUNC, tputs, "¸¶²è", (void*)5 },
    { LVNS_SCRIPT_FUNC, tputs, "¿åÌµ·î¡¡Å°", (void*) 7 },
    { LVNS_SCRIPT_FUNC, DispMoji },

    { LVNS_SCRIPT_BG, (void*)9 },
    { LVNS_SCRIPT_WAIT, (void*)6000 },
    { LVNS_SCRIPT_CLEAR, (void*)LVNS_EFFECT_FADE_PALETTE },
    { LVNS_SCRIPT_DISP, (void*)LVNS_EFFECT_FADE_PALETTE },

    { LVNS_SCRIPT_WAIT, (void*)1000 },
    { LVNS_SCRIPT_FUNC, tputs, "¥×¥í¥°¥é¥à", (void*)5 },
    { LVNS_SCRIPT_FUNC, tputs, "£È£Á£Ê£É£Í£Å¡¡£Î£É£Î£Ï£Í£Á£Å", (void*)7 },
    { LVNS_SCRIPT_FUNC, DispMoji },

    { LVNS_SCRIPT_BG, (void*)12 },
    { LVNS_SCRIPT_WAIT, (void*)6000 },
    { LVNS_SCRIPT_CLEAR, (void*)LVNS_EFFECT_FADE_PALETTE },
    { LVNS_SCRIPT_DISP, (void*)LVNS_EFFECT_FADE_PALETTE },

    { LVNS_SCRIPT_WAIT, (void*)1000 },
    { LVNS_SCRIPT_FUNC, tputs, "¥Ó¥¸¥å¥¢¥ë¥°¥é¥Õ¥£¥Ã¥¯¥¹", (void*)4 },
    { LVNS_SCRIPT_FUNC, tputs, "Ä»Ìî¡¡Àµ¿®", (void*)6 },
    { LVNS_SCRIPT_FUNC, tputs, "¥é¡¼¡¦£Ù£Ï£Õ", (void*)7 },
    { LVNS_SCRIPT_FUNC, tputs, "À¸ÇÈÌ´", (void*)8 },
    { LVNS_SCRIPT_FUNC, tputs, "£Ä£Ï£Ú£Á", (void*)9 },
    { LVNS_SCRIPT_FUNC, tputs, "£Ë£Å£Î£Ë£Å£Î", (void*)10 },
    { LVNS_SCRIPT_FUNC, DispMoji },

    { LVNS_SCRIPT_BG, (void*)15 },
    { LVNS_SCRIPT_WAIT, (void*)6000 },
    { LVNS_SCRIPT_CLEAR, (void*)LVNS_EFFECT_FADE_PALETTE },
    { LVNS_SCRIPT_DISP, (void*)LVNS_EFFECT_FADE_PALETTE },

    { LVNS_SCRIPT_WAIT, (void*)1000 },
    { LVNS_SCRIPT_FUNC, tputs, "¥­¥ã¥é¥¯¥¿¡¼¥°¥é¥Õ¥£¥Ã¥¯¥¹", (void*)5 },
    { LVNS_SCRIPT_FUNC, tputs, "Ä»Ìî¡¡Àµ¿®", (void*)6 },
    { LVNS_SCRIPT_FUNC, tputs, "¥é¡¼¡¦£Ù£Ï£Õ", (void*)7 },
    { LVNS_SCRIPT_FUNC, tputs, "À¸ÇÈÌ´", (void*)8 },
    { LVNS_SCRIPT_FUNC, tputs, "£Ë£Å£Î£Ë£Å£Î", (void*)9 },
    { LVNS_SCRIPT_FUNC, tputs, "£Ä£Ï£Ú£Á", (void*)10 }, 
    { LVNS_SCRIPT_FUNC, DispMoji },

    { LVNS_SCRIPT_BG, (void*)16 },
    { LVNS_SCRIPT_WAIT, (void*)6000 },
    { LVNS_SCRIPT_CLEAR, (void*)LVNS_EFFECT_FADE_PALETTE },
    { LVNS_SCRIPT_DISP, (void*)LVNS_EFFECT_FADE_PALETTE },

    { LVNS_SCRIPT_WAIT, (void*)1000 },
    { LVNS_SCRIPT_FUNC, tputs, "ÇØ·Ê¥°¥é¥Õ¥£¥Ã¥¯¥¹", (void*)5 },
    { LVNS_SCRIPT_FUNC, tputs, "Ä»Ìî¡¡Àµ¿®", (void*)7 },
    { LVNS_SCRIPT_FUNC, DispMoji },

    { LVNS_SCRIPT_BG, (void*)17 },
    { LVNS_SCRIPT_WAIT, (void*)6000 },
    { LVNS_SCRIPT_CLEAR, (void*)LVNS_EFFECT_FADE_PALETTE },
    { LVNS_SCRIPT_DISP, (void*)LVNS_EFFECT_FADE_PALETTE },

    { LVNS_SCRIPT_WAIT, (void*)1000 },
    { LVNS_SCRIPT_FUNC, tputs, "²»³Ú", (void*)4 },
    { LVNS_SCRIPT_FUNC, tputs, "²¼Àî¡¡Ä¾ºÈ", (void*)6 },
    { LVNS_SCRIPT_FUNC, tputs, "ÀÐÀî¡¡¿¿Ìé", (void*)7 },
    { LVNS_SCRIPT_FUNC, tputs, "Ãæ¾å¡¡ÏÂ±Ñ", (void*)8 },
    { LVNS_SCRIPT_FUNC, tputs, "¹â¶¶¡¡Î¶Ìé", (void*)9 },
    { LVNS_SCRIPT_FUNC, DispMoji },

    { LVNS_SCRIPT_BG, (void*)19 },
    { LVNS_SCRIPT_WAIT, (void*)6000 },
    { LVNS_SCRIPT_CLEAR, (void*)LVNS_EFFECT_FADE_PALETTE },
    { LVNS_SCRIPT_DISP,(void*) LVNS_EFFECT_FADE_PALETTE },

    { LVNS_SCRIPT_WAIT, (void*)1000 },
    { LVNS_SCRIPT_FUNC, tputs, "¥·¡¼¥ó¹½À®", (void*)5 },
    { LVNS_SCRIPT_FUNC, tputs, "ÍÕ·î¡¡Í¥°ì", (void*)7 },
    { LVNS_SCRIPT_FUNC, tputs, "¿åÌµ·î¡¡Å°", (void*)8 },
    { LVNS_SCRIPT_FUNC, tputs, "Ãæ¾å¡¡ÏÂ±Ñ", (void*)9 },
    { LVNS_SCRIPT_FUNC, tputs, "À¸ÇÈÌ´", (void*)10 },
    { LVNS_SCRIPT_FUNC, DispMoji },

    { LVNS_SCRIPT_BG, (void*)24 },
    { LVNS_SCRIPT_WAIT, (void*)6000 },
    { LVNS_SCRIPT_CLEAR, (void*)LVNS_EFFECT_FADE_PALETTE },
    { LVNS_SCRIPT_DISP, (void*)LVNS_EFFECT_FADE_PALETTE },

    { LVNS_SCRIPT_WAIT, (void*)1000 }, 
    { LVNS_SCRIPT_FUNC, tputs, "¥ª¥ê¥¸¥Ê¥ë¥Õ¥©¥ó¥È", (void*)5 },
    { LVNS_SCRIPT_FUNC, tputs, "°ì¡¡°ì", (void*)7 },
    { LVNS_SCRIPT_FUNC, tputs, "¿åÌµ·î¡¡Å°", (void*)8 },
    { LVNS_SCRIPT_FUNC, tputs, "£È£Á£Í£Í£Å£Ò", (void*)9 },
    { LVNS_SCRIPT_FUNC, DispMoji },

    { LVNS_SCRIPT_BG, (void*)27 },
    { LVNS_SCRIPT_WAIT, (void*)6000 },
    { LVNS_SCRIPT_CLEAR, (void*)LVNS_EFFECT_FADE_PALETTE },
    { LVNS_SCRIPT_DISP,(void*) LVNS_EFFECT_FADE_PALETTE },

    { LVNS_SCRIPT_WAIT, (void*)1000 },
    { LVNS_SCRIPT_FUNC, tputs, "¥·¥ç¡¼¥È¥·¥Ê¥ê¥ªµÓËÜ", (void*)5 },
    { LVNS_SCRIPT_FUNC, tputs, "ÀÄ¡¡»ç", (void*)7 },
    { LVNS_SCRIPT_FUNC, DispMoji },

    { LVNS_SCRIPT_BG, (void*)45 },
    { LVNS_SCRIPT_WAIT, (void*)6000 },
    { LVNS_SCRIPT_CLEAR, (void*)LVNS_EFFECT_FADE_PALETTE },
    { LVNS_SCRIPT_DISP, (void*)LVNS_EFFECT_FADE_PALETTE },

    { LVNS_SCRIPT_WAIT, (void*)1000 },
    { LVNS_SCRIPT_FUNC, tputs, "£Ô£Å£Ó£Ô¡¡£Ð£Ì£Á£Ù", (void*)5 },
    { LVNS_SCRIPT_FUNC, tputs, "£Á£Ì£Ì¡¡£Ì£Å£Á£Æ¡¡£Ó£Ô£Á£Æ£Æ", (void*)7 },
    { LVNS_SCRIPT_FUNC, DispMoji },

    { LVNS_SCRIPT_BG, (void*)11 },
    { LVNS_SCRIPT_WAIT, (void*)6000 },
    { LVNS_SCRIPT_CLEAR,(void*)LVNS_EFFECT_FADE_PALETTE },
    { LVNS_SCRIPT_DISP, (void*)LVNS_EFFECT_FADE_PALETTE },

    { LVNS_SCRIPT_WAIT, (void*)1000 },
    { LVNS_SCRIPT_FUNC, tputs, "³«È¯", (void*)5 },
    { LVNS_SCRIPT_FUNC, tputs, "£±£¹£¹£¶¡¡£Ì£Å£Á£Æ", (void*)7 },
    { LVNS_SCRIPT_FUNC, DispMoji },
    { LVNS_SCRIPT_WAIT, (void*)2000 },

    { LVNS_SCRIPT_WAIT_CLICK },
    { LVNS_SCRIPT_CLICK_JUMP },
    { LVNS_SCRIPT_CLEAR, (void*)LVNS_EFFECT_FADE_PALETTE },
    { LVNS_SCRIPT_END }
};

void
KizuatoEnding(Lvns *lvns)
{
	LvnsScriptRun(lvns, eddata);
}
