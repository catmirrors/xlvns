/*
 * LEAF Visual Novel System For MGL2
 * (c) Copyright 2001 TF <tf@denpa.org>
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: sdt.c,v 1.2 2001/08/05 15:42:37 tf Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pakconv.h"
#include "leafpack.h"
#include "kizuato_pal.h"

static struct {
  char name[16];
  int width;
  int height;
  int pal;
} sdt_list[] = {
  {"OP1W.SDT", 336, 400, 1}, /* ÀéÄá */
  {"OP2W.SDT", 448, 512, 1}, /*  °´  */
  {"OP3W.SDT", 448, 512, 2}, /*  Éö  */
  {"OP4W.SDT", 336, 400, 2}, /* ½é²» */
  {"OP2_KM_W.SDT", 1280, 400, 0}, /* ±Æ */
};

Image *
lvnsimage_load_sdt(const u_char *data, const char *name)
{
    int i, width, height;
    int si;
    size_t size;
    Image *ret;

    for (i = 0; i < 5; i++) {
      if (!strcmp(name, sdt_list[i].name)) {
	si = i;
	width = sdt_list[i].width;
	height = sdt_list[i].height;
	break;
      }
    }
    
    ret = image_new(width, height, 0, 0);

    for (i = 0; i < 16; i++) {
      ret->palette[i][0] = pal[sdt_list[si].pal][i][0];
      ret->palette[i][1] = pal[sdt_list[si].pal][i][1];
      ret->palette[i][2] = pal[sdt_list[si].pal][i][2];
    };

    size = data[0] | (data[1]<<8) | (data[2]<<16) | (data[3]<<24);
    leafpack_lzs2(data + 4, ret->data, size);

    /* ¾å²¼È¿Å¾ */
    {
        int i, j;
        for (i=0; i<height/2; i++) {
            u_char *p = ret->data + i * width;
            u_char *q = ret->data + (height - i - 1) * width;
            for (j=0; j<width; j++, p++, q++) {
                u_char c;
                c = *p;
                *p = *q;
                *q = c;
            }
        }
    }
    return ret;
}
