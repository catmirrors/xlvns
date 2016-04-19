/*
 * LEAF Visual Novel System For MGL2
 * (c) Copyright 2001 TF <tf@denpa.org>
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: mgImage.c,v 1.11 2001/08/12 11:35:44 tf Exp $
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Lvns.h"

#include "LvnsEffect.h"

static int DecodeLZ77(u_char *, u_char *, int, int);

#define RINGBUF_SIZE 0x1000    /* size of ring buffer */

u_char ring[RINGBUF_SIZE];

static int cmap_m[17][17]; /* Multiple計算用テーブル(黒とのαブレンド) */

static int cmap_s_s[17][17]; /* Screen  計算用テーブル(白とのαブレンド) */
static int cmap_s_b[17][17]; /* Screen  計算用テーブル(白とのαブレンド) */

//static int cmap_a[17][17]; /* αブレンド計算用テーブル                 */

static int pwidth, pheight; /* 物理画面の有効な領域のサイズ */
static int xoffset, yoffset; /* 物理画面の有効な領域のオフセット */

#undef WIDTH
#undef HEIGHT

mgImage *
mglimage_new(int w, int h, int x, int y)
{
    mgImage *img;

    img = (mgImage *) malloc(sizeof(mgImage));
    img->width = w;
    img->height = h;
    img->xoffset = x;
    img->yoffset = y;
    img->rwidth = w - x;
    img->rheight = h - y;
    img->is_put = False;

    img->use_palette = False;
    img->use_multiple = False;
    img->use_whiteout = False;

    img->hsb_data =
	(u_short *)calloc(img->rwidth * img->rheight, sizeof(u_short));

    img->hsb_pal_data = NULL;

    return img;
}

void
mglimage_delete(mgImage *img)
{
    if (!img)
      return;

    if (img->hsb_data)
      free(img->hsb_data);

    if (img->hsb_pal_data)
      free(img->hsb_pal_data);

    free(img);
}

/*
 * 画像の追加．はみ出しのチェックは行なわない．
 */
void
mglimage_add(mgImage *src, mgImage *dst, int x, int y)
{
    int i, j;
    u_short *p;

    x += src->xoffset;
    y += src->yoffset;

    dst->use_multiple = src->use_multiple;
    dst->use_whiteout = src->use_whiteout;

    p = src->hsb_data;
    for (i = 0; i < src->rheight; i++) {
	u_short *q = dst->hsb_data + (y + i) * dst->rwidth + x;
	for (j = 0; j < src->rwidth; j++, q++, p++) {
	  if (*p != MGL_TRANSPARENT) {
	    *q = *p;
	  }
	}
    }
}

/*
 * 画像の追加．はみ出しのチェックを行なう．
 */
void
mglimage_add2(mgImage *src, mgImage *dst, int x, int y)
{
    int i, j;
    int w, h, xoff, yoff;

    x += src->xoffset;
    y += src->yoffset;

    dst->use_multiple = src->use_multiple;
    dst->use_whiteout = src->use_whiteout;

    if (x < 0) {
      xoff = -x;
      w = src->rwidth + x;
      x = 0;
    } else {
      xoff = 0;
      w = src->rwidth;
    }

    if (y < 0) {
      yoff = -y;
      h = src->rheight + y;
      y = 0;
    } else {
      yoff = 0;
      h = src->rheight;
    }

    if (w + x > dst->rwidth) {
      w = dst->rwidth - x;
    }
    if (h + y > dst->rheight) {
      h = dst->rheight - y;
    }

    for (i = 0; i < h; i++) {
        u_short *p, *q;
	p = src->hsb_data + (yoff + i) * src->rwidth + xoff;
	q = dst->hsb_data + (y + i) * dst->rwidth + x;
	for (j = 0; j < w; j++, q++, p++) {
	  if (*p != MGL_TRANSPARENT) {
	    *q = *p;
	  }
	}
    }
}

/* 
 * 強制合成。ToHeart のカレンダー処理専用
 * 画像のはみ出しのチェックは行っていないため、はみでると破綻する
 */
void
mglimage_add3(mgImage *src, mgImage *dst, int x, int y, int pixoff, int transparent, int step)
{
  /* TODO */
    int i,j;
    int xlim[240];
    int col;

    x += src->xoffset;
    y += src->yoffset;

    if (pixoff == 26) { /* red */
      col = 0;
    } else if (pixoff == 52) { /* blue */
      col = 8;
    }

    if (step < 3) {
      if (transparent == 209) {/* 古い日付 */
	for (i=0; i<src->rheight; i++) {
	  u_short *p = src->hsb_data + i * src->rwidth;
	  u_short *q = dst->hsb_data + (y + i) * dst->rwidth + x;
	  for (j=0; j<src->rwidth; j++, q++, p++) {
	    if (*p != src->transparent && *p != COLOR_WHITE) {
	      if (pixoff) {
		*q = packMC(col, 10, 10);
	      } else {
		*q = *p;
	      }
	    }
	  }    
	}
      }
    }
    
    if (step == 3 || step == 4) {
      if (transparent == 209) {/* 古い日付 */
	
	for (i = 0; i < dst->rheight; i++) {
	  for (j = dst->rwidth - 10; j >= 0; j--) {
	    if (*(dst->hsb_data + i * dst->width + j) != COLOR_WHITE) break;  
	  }
	  xlim[i] = j;
	}
	
	for (i=0; i<src->rheight; i++) {
	  u_short *p = src->hsb_data + i * src->rwidth;
	  u_short *q = dst->hsb_data + (y + i) * dst->rwidth + x;
	  for (j=0; j<src->rwidth; j++, q++, p++) {
	    if (j + x <= xlim[y + i]) continue;
	    if (*p != MGL_TRANSPARENT && *p != COLOR_WHITE) {
	      if (pixoff) {
		*q = packMC(col, 10, 10);
	      } else {
		*q = *p;
	      }
	    }
	  }    
	}
      }
      
      if (transparent == 211) { /* 新しい日付 */
	
	for (i = 0; i < dst->rheight; i++) {
	  for (j = 10; j < dst->rwidth; j++) {
	    if (*(dst->hsb_data + i * dst->width + j) != COLOR_WHITE) break;  
	  }
	  xlim[i] = j;
	}
	
	for (i=0; i<src->rheight; i++) {
	  u_short *p = src->hsb_data + i * src->rwidth;
	  u_short *q = dst->hsb_data + (y + i) * dst->rwidth + x;
	  for (j=0; j<src->rwidth; j++, q++, p++) {
	    if (j + x >= xlim[y + i]) break;
	    if (*p != MGL_TRANSPARENT && *p != COLOR_WHITE) {
	      if (pixoff) {
		*q = packMC(col, 10, 10);
	      } else {
		*q = *p;
	      }
	    }
	  }  
	}
      }
    }
    
    if (step > 4) {
      
      if (transparent == 211) {
	
	for (i = 0; i < dst->rheight; i++) {
	  for (j = 60; j < dst->rwidth; j++) {
	    if (*(dst->hsb_data + i * dst->width + j) != COLOR_WHITE) break;  
	  }
	  xlim[i] = j;
	}
	
	for (i=0; i<src->rheight; i++) {
	  u_short *p = src->hsb_data + i * src->rwidth;
	  u_short *q = dst->hsb_data + (y + i) * dst->rwidth + x;
	  for (j=0; j<src->rwidth; j++, q++, p++) {
	    if (j + x >= xlim[y + i]) break;
	    if (*p != MGL_TRANSPARENT && *p != COLOR_WHITE) {
	      if (pixoff) {
		*q = packMC(col, 10, 10);
	      } else {
		*q = *p;
	      }
	    }
	  }    
	}
      }
    }
}

/*
 * 画像の OR 合成．はみ出しのチェックを行なう．
 */
void
mglimage_or(mgImage *src, mgImage *dst, int x, int y)
{
    int i, j;
    int w, h, xoff, yoff;
    int h1, s1, b1, h2, s2, b2;

    x += src->xoffset;
    y += src->yoffset;

    dst->use_multiple = src->use_multiple;
    dst->use_whiteout = src->use_whiteout;

    if (x < 0) {
      xoff = -x;
      w = src->rwidth + x;
      x = 0;
    } else {
      xoff = 0;
      w = src->rwidth;
    }

    if (y < 0) {
      yoff = -y;
      h = src->rheight + y;
      y = 0;
    } else {
      yoff = 0;
      h = src->rheight;
    }

    if (w + x > dst->rwidth) {
      w = dst->rwidth - x;
    }
    if (h + y > dst->rheight) {
      h = dst->rheight - y;
    }

    for (i = 0; i < h; i++) {
        u_short *p, *q;
	p = src->hsb_data + (yoff + i) * src->rwidth + xoff;
	q = dst->hsb_data + (y + i) * dst->rwidth + x;
	for (j = 0; j < w; j++, q++, p++) {
	  if (*p == 0x0 || *p == MGL_TRANSPARENT) continue;
	  if (*q == 0x0 || *q == MGL_TRANSPARENT) {
	    *q = *p;
	  } else {
	    unpackMC(*p, h1, s1, b1);
	    unpackMC(*q, h2, s2, b2);
	    *q = packMC((h1 + h2) / 2, (s1 + s2) / 2, (b1 + b2) / 2);
	  }
	}
    }
}

void
lvnsimage_copy(mgImage *src, mgImage *dst)
{
  int i, j;

  dst->use_multiple = src->use_multiple;
  dst->use_whiteout = src->use_whiteout;

  if (src->rwidth == dst->rwidth && src->rheight == dst->rheight) {
    if (src->use_palette) {
      memcpy(dst->hsb_data, src->hsb_pal_data,
	     src->rwidth * src->rheight * sizeof(u_short));
      
    } else {
      memcpy(dst->hsb_data, src->hsb_data,
	     src->rwidth * src->rheight * sizeof(u_short));
    }
  } else {
    u_short *p;
    if (src->use_palette) {
      p = src->hsb_pal_data;
    } else {
      p = src->hsb_data;
    }
    memset(dst->hsb_data, COLOR_BLACK, dst->rwidth * dst->rheight);
    for (i = 0; i < src->rheight; i++) {
      u_short *q =
	dst->hsb_data + (i + src->yoffset) * dst->rwidth + src->xoffset;
      for (j = 0; j < src->rwidth; j++, q++, p++) {
	*q = *p;
      }
    }
  }
}

void
lvnsimage_clear_transparent(mgImage *img)
{
    memset(img->hsb_data, MGL_TRANSPARENT,
	   img->rwidth * img->rheight * sizeof(u_short));
}

void
lvnsimage_clear_area_transparent(mgImage *img, int x, int y, int w, int h)
{
    int i;
    for (i = 0; i < h; y++, i++)
	memset(img->hsb_data + img->rwidth * y + x, 0xff,
	       w * sizeof(u_short));
}

mgImage *
mglimage_load_lfg(u_char *data, mgImage *over)
{
    mgImage *img;
    u_char *bmp;
    u_char *p;
    u_short *q;
    int i, j, rwidth, rheight, xoffset, yoffset, width, height;
    int size, osize;

    size = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];
    osize = data[4] << 24 | data[5] << 16 | data[6] << 8 | data[7];

    bmp = (u_char *)calloc(osize, sizeof(u_char));
    DecodeLZ77(bmp, data + 8, size, osize);

    rwidth = bmp[0] << 8 | bmp[1];
    rheight = bmp[2] << 8 | bmp[3];
    xoffset = bmp[4] << 8 | bmp[5];
    yoffset = bmp[6] << 8 | bmp[7];
    width = rwidth + xoffset;
    height = rheight + yoffset;

    if (over) {
	/* XXX no check.... */
	img = over;
	img->width = width;
	img->height = height;
	img->xoffset = xoffset;
	img->yoffset = yoffset;
	img->rwidth = rwidth;
	img->rheight = rheight;
	if (img->hsb_pal_data) {
	  free (img->hsb_pal_data);
	  img->hsb_pal_data = NULL;
	}
	img->use_palette = False;
	img->use_multiple = False;
	img->use_whiteout = False;
    } else {
	img = mglimage_new(width, height, xoffset, yoffset);
    }


    p = bmp + 8;
    q = img->hsb_data;
    for (i = 0; i < rheight; i++) {
      for (j = 0; j < rwidth; j++) {
	*(q + i * rwidth + j) = (*p << 8) | *(p + 1);
	p += 2;
      }
    }

    free(bmp);

    return img;
}

void
lvnsimage_clear(mgImage *img)
{
#if 1
    /* first, but it assumes COLOR_BLACK is zero. */
    memset(img->hsb_data, 0,
	   img->rwidth * img->rheight * sizeof(u_short));
#else
    { /* slow version */
      int i;
      for (i = 0; i < img->rwidth * img->rheight; i++) {
	img->hsb_data[i] = COLOR_BLACK;
      }
    }
#endif
    img->palette_num = 0;
    img->transparent = -1;
    if (img->hsb_pal_data) {
      free(img->hsb_pal_data);
      img->hsb_pal_data = NULL;
      img->use_palette = False;
    }
}

void
mglimage_clear_color(mgImage *img, int color)
{
    int i;
    for (i = 0; i < img->rwidth * img->rheight; i++) {
      img->hsb_data[i] = color;
    }
    img->palette_num = 0;
    img->transparent = -1;
    if (img->hsb_pal_data) {
      free(img->hsb_pal_data);
      img->hsb_pal_data = NULL;
      img->use_palette = False;
    }
}

static int
DecodeLZ77(u_char *dst, u_char *src, int size, int osize)
{
  int i, j, k;
  int c, m;
  int flag = 0;
  int upper, lower;
  int pos, len;
  int idx = 0;
  
  /* initialize ring buffer */
  for (i = 0; i < RINGBUF_SIZE; i++){
    ring[i] = 0;
  }

  /* extract data */
  for (c = 0, m = 0xfee, i = 0, k = 0; idx < osize;){
    /* flag bits, which indicates data or location */
    if (--c < 0) {
      flag = src[k++];
      c = 7;
    }
    
    if(flag & 0x80){
      /* data */
      ring[m++] = dst[idx++] = src[k++];
      m &= 0xfff;
    } else {
      /* copy from ring buffer */
      upper = src[k++];
      lower = src[k++];
      len = (upper & 0x0f) + 3;
      pos = (upper >> 4) + (lower << 4);

      for(j = 0; j < len; j++){
	ring[m++] = dst[idx++] =  ring[pos++];
	m &= 0xfff;
	pos &= 0xfff;
      }
    }
    flag = flag << 1;
    if (k >= size) {
      break;
    }
  }
  return idx;
}

/*
 * 内部関数。点をうつ。
 * はみでのチェックを行っている
 */
static void
mglimage_put_pixel(mgImage *image, int x, int y, u_short pixel)
{
    if (x >= 0 && x < image->rwidth &&
        y >= 0 && y < image->rheight) {
	put_pixel(xoffset + x, yoffset + y, pixel);
    }
}

/*
 * 内部関数
 * 指定したサイズの矩形で点を打つ
 */ 
static void 
mglimage_put_box_pixel(mgImage *image, int x, int y, int size, u_short pixel)
{
    int i, j;
    for (i=0;i<size;i++) {
        for (j=0;j<size;j++) {
            mglimage_put_pixel(image, x+i-size/2, y+j-size/2, pixel); 
        }
    }
}

/*
 * 指定したサイズの矩形の点で円を書く
 */
void
mglimage_circle(mgImage *image, int xc, int yc, int r, int size, int pixel)
{
    int x, y, c;
    x = r; y = 0;c=0;
    while (x >= y) {
        if (c++ == 0) {
            mglimage_put_box_pixel(image, xc + x, yc + y, size, pixel);
            mglimage_put_box_pixel(image, xc + x, yc - y, size, pixel);
            mglimage_put_box_pixel(image, xc - x, yc + y, size, pixel);
            mglimage_put_box_pixel(image, xc - x, yc - y, size, pixel);
            mglimage_put_box_pixel(image, xc + y, yc + x, size, pixel);
            mglimage_put_box_pixel(image, xc + y, yc - x, size, pixel);
            mglimage_put_box_pixel(image, xc - y, yc + x, size, pixel);
            mglimage_put_box_pixel(image, xc - y, yc - x, size, pixel);
            if ((r -= (y++ << 1) - 1) < 0) 
                r += (x-- -1) << 1;
        } 
        c %= (size / 2);
    }
}

void
mglimage_set_multiple(mgImage *image, int *offscreen, int par16)
{
  image->use_multiple = True;
  image->par16_multiple = par16;
}

void
mglimage_set_whiteout(mgImage *image, int *offscreen, int par16)
{
  image->use_whiteout = True;
  image->par16_whiteout = par16;
}

void
mglimage_draw_area(mgImage *image, int *offscreen, int x, int y, int w, int h, int x2, int y2)
{
  int i, j;
  int *p;
  u_short *q;
  int hue, sat, bri;
  
  if (image->use_whiteout && image->use_multiple) {
    for (i = 0; i < h; i++) {
      p = offscreen + pwidth * (y2 + i) + x2;
      q = image->hsb_data + image->rwidth * (y + i) + x;
      for (j = 0; j < w; j++, p++, q++) {
	unpackMC(*q, hue, sat, bri);
	*p = packMC(hue, cmap_s_s[image->par16_whiteout][sat], cmap_s_b[image->par16_whiteout][cmap_m[image->par16_multiple][bri]]);
      }
    }
    image->use_whiteout = False;
    image->use_multiple = False;
  } else if (image->use_whiteout) {
    for (i = 0; i < h; i++) {
      p = offscreen + pwidth * (y2 + i) + x2;
      q = image->hsb_data + image->rwidth * (y + i) + x;
      for (j = 0; j < w; j++, p++, q++) {
	unpackMC(*q, hue, sat, bri);
	*p = packMC(hue, cmap_s_s[image->par16_whiteout][sat], cmap_s_b[image->par16_whiteout][bri]);
      }
    }
    image->use_whiteout = False;
  } else if (image->use_multiple) {
    for (i = 0; i < h; i++) {
      p = offscreen + pwidth * (y2 + i) + x2;
      q = image->hsb_data + image->rwidth * (y + i) + x;
      for (j = 0; j < w; j++, p++, q++) {
	unpackMC(*q, hue, sat, bri);
	*p = packMC(hue, sat, (cmap_m[image->par16_multiple][bri]));
      }
    }
    image->use_multiple = False;
  } else {
    for (i = 0; i < h; i++) {
      p = offscreen + pwidth * (y2 + i) + x2;
      q = image->hsb_data + image->rwidth * (y + i) + x;
      for (j = 0; j < w; j++, p++, q++) {
	*p = *q;
      }
    }
  }
}

void
mglimage_init(int width, int height)
{
  int i, j;

  pwidth = width;
  pheight = height;
  xoffset = 80;
  yoffset = (240 - height) / 2;

  for (i = 0; i < 17; i++) {
    for (j = 0; j < 17; j++) {
      cmap_m[i][j] = j * i / 16;

      cmap_s_s[i][j] = j  - j * i / 16;
      cmap_s_b[i][j] = j  + (15 - j) * i / 16;
    }
  }
}

void
mglimage_copy_mask_unit_to_sximage(mgImage *src, int x, int y, int x2, int y2, int mask)
{
  int i, j;
  int *line;
  static int xoff[] = { 3, 1, 1, 3,  2, 0, 0, 2,  3, 1, 1, 3, 2, 0, 0, 2 };
  static int yoff[] = { 0, 2, 0, 2,  1, 3, 1, 3,  1, 3, 1, 3, 0, 2, 0, 2 };

  y += yoff[mask];
  x += xoff[mask];
  y2 += yoff[mask];
  x2 += xoff[mask];

  line = (int *)calloc(pwidth, sizeof(int));

  for (i=0; i< 16; i+=4) {
    u_short *p = src->hsb_data + (y+i) * src->rwidth + x;
    if (y + i >= src->rheight) {
      break;
    }
    get_pixstream(xoffset, yoffset + y2 + i, line, pwidth, DIR_NORTH, 0);
    for (j=0; j<16; j+=4, p+=4) {
      line[x2 + j] = *p;
    }
    put_pixstream_rect(xoffset, yoffset + y2 + i, line, pwidth, DIR_NORTH, pwidth);
  }
    free(line);
}

void
mglimage_copy_squaremask_unit_to_sximage(mgImage *src, int x, int y, int x2, int y2, int mask)
{
    int x0, y0;

    if (mask >= 16) {
        x0 = 15;
        y0 = mask - 15;
    } else {
        x0 = mask;
        y0 = 0;
    }

    /* slow version ... */
    while (x0 >= 0 && y0 < 16) {
        u_short *p  = src->hsb_data + (y + y0) * src->rwidth + x;
	if (y2 + y0 >= src->rheight) {
	  break;
	}
	put_pixel(xoffset + x2 + x0, yoffset + y2 + y0, p[x0]);
	put_pixel(xoffset + x2 + 31 - x0, yoffset + y2 + y0, p[31-x0]);
        if (y + 31 - y0 < src->rheight) {
	  u_short *p2 = src->hsb_data + (y + 31 -y0) * src->rwidth + x;
	  put_pixel(xoffset + x2 + x0, yoffset + y2 + 31-y0, p2[x0]);
	  put_pixel(xoffset + x2 + 31 - x0, yoffset + y2 + 31-y0, p2[31-x0]);
        }
        x0--;
        y0++;
    }
}

/**
 * 指定した領域のコピーを行う。
 * src のオフセットのチェックは行っていない。
 */
void
lvnsimage_copy_area(mgImage *src, mgImage *dst, int x, int y, int w, int h, int x2, int y2)
{
    int i,j;

    if (x < 0) {
       w += x;
       x = 0;
    }
    if (y < 0) {
       h += y;
       y = 0;
    }
    if (w <= 0 || h <= 0)
      return;

    if (x2 < 0) {
       w += x2;
       x2 = 0;
    }
    if (y2 < 0) {
       h += y2;
       y2 = 0;
    }
    if (w <= 0 || h <= 0)
      return;

    if (x + w > src->rwidth) w = src->rwidth - x;
    if (y + h > src->rheight) h = src->rheight - y;
    if (x2 + w > dst->rwidth) w = dst->rwidth - x2;
    if (y2 + h > dst->rheight) h = dst->rheight - y2;

  if (src->use_palette) {
    for (i=0; i<h; i++) {
      u_short *p = src->hsb_pal_data + (i + y) * src->rwidth + x;
      u_short *q = dst->hsb_data + (i + y2) * dst->rwidth + x2;
      for (j=0; j<w; j++, q++, p++) {
	*q = *p;
      }
    }
  } else {
    for (i=0; i<h; i++) {
      u_short *p = src->hsb_data + (i + y) * src->rwidth + x;
      u_short *q = dst->hsb_data + (i + y2) * dst->rwidth + x2;
      for (j=0; j<w; j++, q++, p++) {
	*q = *p;
      }
    }
  }
}

/**
 * LVNS用イメージデータの特定領域を0番のピクセルで消去
 */
void
lvnsimage_clear_area(mgImage *img, int x, int y, int w, int h)
{
    int i;
    for (i=0; i<h; y++, i++)
        memset(img->hsb_data + img->rwidth * y + x, img->black, w * sizeof(u_short));
}

/**
 * LVNS用イメージの特定領域を指定した色で塗り潰す
 */
void
lvnsimage_boxfill(mgImage *img, int color, int x, int y, int x2, int y2)
{
  u_short *p;
  int i, j;

  for (i = 0; i <= y2 - y; i++) {
    p = img->hsb_data + img->rwidth * (y + i) + x;
    for (j = 0; j <= x2 - x; j++, p++) {
      *p = color;
    }
  }
}

void
mglimage_box(mgImage *img, int x, int y, int w, int h, int color)
{
  set_color(color);
  draw_rect(x + xoffset, y + yoffset, w, h);
}

void
lvnsimage_set_palette(mgImage *image, u_char paletteinfo[][3], int no)
{
  //dprintf((stderr, "lvnsimage_set_palette: %d\n", no));
}

void
mglimage_set_palette(mgImage *image, int pal[3][16])
{
  int i, h, s, b;
  u_short *p, *q;

  image->use_palette = True;
  memcpy(image->hsb_palette, pal, 3 * 16 * sizeof(int));

  image->hsb_pal_data = (u_short *)calloc(image->rwidth * image->rheight, sizeof(u_short));
  
  memset(image->hsb_pal_data, COLOR_BLACK, image->rwidth * image->rheight);
  p = image->hsb_data;
  q = image->hsb_pal_data;
  for (i = 0; i < image->rwidth * image->rheight; i++, p++, q++) {
    unpackMC(*p, h, s, b);
    *q = packMC(image->hsb_palette[0][h],
		image->hsb_palette[1][s],
		image->hsb_palette[2][b]);
  }
}

void
mglimage_set_pal_to_image(mgImage *image, int *offscreen)
{
  int i, j;
  u_short *p;
  int *q;  
  int bri_idx = 16;
  int hue, sat, bri;

  if (!image->use_palette) return;
  
  p = image->hsb_data;
  q = offscreen;

  for (i = 0; i < image->rheight; i++) {
    for (j = 0; j < image->rwidth; j++, p++, q++) {
      unpackMC(*p, hue, sat, bri);
      *q = packMC(hue, sat, (cmap_m[bri_idx][bri]));
    }
  }
}

void
mglimage_copy_area_win2(mgImage *img, int *offscreen,
			int x, int y, int w, int h, int x2, int y2)
{
  int i, j, *p;
  u_short *q;

  memset(offscreen, COLOR_BLACK,
	 img->rwidth * img->rheight * sizeof(int));
  for (i = 0; i < h; i++) {
    p = offscreen + pwidth * (y2 + i) + x2;
    q = img->hsb_data + img->rwidth * (y + i) + x;
    for (j = 0; j < w; j++) {
      *p++ = *q++;
    }
  }
  put_pixstream_rect(xoffset, yoffset, offscreen, pwidth * pheight, DIR_NORTH, pwidth);

  img->is_put = False;
}

void
mglimage_draw_string(mgImage *img, int x, int y, char *str, int color)
{
    struct screen *off;
    int len, i, j;
    
    len = strlen(str);
    off = create_memscreen(12 * len / 2, 12, NULL, STK_GENERIC_4COLOR, 0);

    /*
     * TODO: FONT_PLUS 拡張への対応
     */
    push_screen(off);
    set_color(COLOR_BLACK);
    clear_screen();
    set_color(COLOR_WHITE);
    draw_string(0, 0, str, DIR_NORTH);
    for (i = 0; i < 12; i++) {
      for (j = 0; j < 12 * len / 2; j++) {
	if (get_pixel(j, i, 0))
	  img->hsb_data[(y + i) * img->rwidth + x + j] = color;
      }
    }
    pop_screen();
    free_screen(off);
}

void
lvnsimage_copy_palette(mgImage *src, mgImage *dst)
{
  
}


void
lvnsimage_copy_scale(mgImage *src, int sx, int sy, int sw, int sh, 
                     mgImage *dst, int dx, int dy, int dw, int dh)
{
    int i, j;
    int xpos[320]; /* XXX */
    int ypos[240]; /* XXX */
    int x, y, xoff, yoff;
    int w, h;

    if (dx < 0) {
        x = 0;
        xoff = -dx;
    } else {
        x = dx;
        xoff = 0;
    }
        
    if (dy < 0) {
        y = 0;
        yoff = -dy;
    } else {
        y = dy;
        yoff = 0;
    }

    w = dw; h = dh;
    if (dx + dw > dst->rwidth)
        w = dst->rwidth - x;
    if (dy + dh > dst->rheight)
        h = dst->rheight - y;

    /* 対応座標算出 */
    for (i=0; i<w; i++)
        xpos[i] = sx + (xoff + i) * sw / dw;
    for (i=0; i<h; i++)
        ypos[i] = sy + (yoff + i) * sh / dh;

    /* こぴー */
    {
        u_short *q = dst->hsb_data + y * dst->rwidth + x;
        for (i=0; i<h; i++) {
            u_short *r = src->hsb_data + ypos[i] * src->rwidth;
            u_short *s = q;
            for (j=0; j<w; j++) {
                *s++ = r[xpos[j]];
            }
            q += dst->rwidth;
        }
    }
}

#if 0
void
lvnsimage_clear_index(mgImage *img, int idx)
{
  mglimage_clear_color(img, idx);
}
#endif

void
lvnsimage_set_palette3(mgImage *image, u_char *paletteinfo, int pos, int num) 
{
  //fprintf(stderr, "TODO: lvnsimage_set_palette3\n");
}

void
lvnsimage_set_palette4(mgImage *image, u_char *paletteinfo, int pos, int num) 
{
  //fprintf(stderr, "TODO: lvnsimage_set_palette4\n");
}

void
lvnsimage_put_pattern24(mgImage *image, int x, int y, int pixel, char *data)
{
  //fprintf(stderr, "TODO: lvnsimage_put_pattern24\n");
}

static void
lvnsimage_add_mask_base(mgImage *src, mgImage *dst, int x, int y, int mask, int dummy)
{
  int i, j;
  static int xoff[] = { 3, 1, 1, 3,  2, 0, 0, 2,  3, 1, 1, 3, 2, 0, 0, 2 };
  static int yoff[] = { 0, 2, 0, 2,  1, 3, 1, 3,  1, 3, 1, 3, 0, 2, 0, 2 };

  for (i=yoff[mask]; i<src->rheight; i+=4) {
    u_short *p = src->hsb_data + i * src->rwidth + xoff[mask];
    u_short *q = dst->hsb_data + (i + y) * dst->rwidth + x + xoff[mask];
    for (j=xoff[mask]; j<src->rwidth; j+=4, q+=4, p+=4) {
      if (*p != MGL_TRANSPARENT) {
	*q = *p;
      }
    }    
  }
}

void
lvnsimage_add_mask(mgImage *src, mgImage *dst, int x, int y, int mask, int dummy)
{
    int i;
    x += src->xoffset;
    y += src->yoffset;
    for (i=0;i<mask;i++)
        lvnsimage_add_mask_base(src, dst, x, y, i, dummy);
}

/*
 * put_pixstream_rect のラッパー
 */
void
mglimage_put_pixstream_rect(int x, int y, int *stream, int len, int op, int w)
{
  put_pixstream_rect(xoffset + x, yoffset + y, stream, len, op, w);
}

void
lvnsimage_set_sepia(mgImage *img)
{
  int i;
  u_short *p, h, s, b;
  dprintf((stderr, "TODO: lvnsimage_set_sepia\n"));

  p = img->hsb_data;
  for (i = 0; i < img->rwidth * img->rheight; i++, p++) {
    unpackMC(*p, h, s, b);
    *p = packMC(1, s, b);
  }
}
