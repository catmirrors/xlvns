/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: lf2.c,v 1.1 2001/07/28 14:57:56 tf Exp $
 *
 */

/*
 * $Id: lf2.c,v 1.1 2001/07/28 14:57:56 tf Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pakconv.h"
#include "leafpack.h"

Image *
lvnsimage_load_lf2(const u_char *data, Image *over)
{
    Image *img;
    int width, height, xoffset, yoffset;
    size_t size;

    /* check magic number 'LEAFCODE' */
    if (strncmp(data, "LEAF256\0", 8)){
	fprintf(stderr, "This file isn't LF2 format.\n");
	return NULL;
    }

    xoffset = (data[9] << 8) | data[8];
    yoffset = (data[11] << 8) | data[10];
    width   = ((data[13] << 8) | data[12]) + xoffset;
    height  = ((data[15] << 8) | data[14]) + yoffset;

    if (over) {
        img = over;
        img->width = width;
        img->height = height;
        img->xoffset = xoffset;
        img->yoffset = yoffset;
        img->rwidth  = width  - xoffset;
        img->rheight = height - yoffset;
    } else {
	img = image_new(width, height, xoffset, yoffset);
    }
    size = img->rwidth * img->rheight;

    img->transparent = data[0x12];
    img->palette_num = data[0x16];
    /* read palette */
    {
        int i;
        const u_char *p = data + 0x18;
        for (i=0; i<img->palette_num;i++) {
            img->palette[i][2] = *p++;
            img->palette[i][1] = *p++;
            img->palette[i][0] = *p++;
        }
    }

{
  int ring[0x1000];
  int i, j;
  int c, m;
  int flag;
  int upper, lower;
  int pos, len;
  int d;
  int idx;
  int p;

  p = 0x18 + img->palette_num * 3;

  /* initialize ring buffer */
  for (i = 0; i < 0xfff; i++){
    ring[i] = 0;
  }
  
  /* extract data */
  for (i = 0, c = 0, m = 0xfee; i < size;){
    /* flag bits, which indicates data or location */
    if (--c < 0) {
      flag = data[p++];
      flag ^= 0xff;
      c = 7;
    }
    
    if(flag & 0x80){
      /* data */
      d = data[p++];
      d ^= 0xff;
      ring[m++] = d;
      idx = (i % img->rwidth) + img->rwidth * (img->rheight - i / img->rwidth - 1);
      img->data[idx] = d;
      i++;
      m &= 0xfff;
    } else {
      /* copy from ring buffer */
      upper = data[p++];
      upper ^= 0xff;

      lower = data[p++];
      lower ^= 0xff;
      
      len = (upper & 0x0f) + 3;
      pos = (upper >> 4) + (lower << 4);

      for(j = 0; j < len && i < size; j++){
          idx = (i % img->rwidth) + img->rwidth * (img->rheight - i / img->rwidth - 1);
          ring[m++] = img->data[idx] =  ring[pos++];
	i++;
	m &= 0xfff;
	pos &= 0xfff;
      }
    }
    flag = flag << 1;
  }

}

    return img;
}
