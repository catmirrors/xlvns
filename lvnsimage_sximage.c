/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: lvnsimage_sximage.c,v 1.5 2001/06/26 15:47:44 go Exp $
 *
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lvnsimage.h"
#include "sximage.h"

static int cmap_m[17][256]; /* Multiple計算用テーブル(黒とのαブレンド) */
static int cmap_s[17][256]; /* Screen  計算用テーブル(白とのαブレンド) */
static int cmap_a[17][512]; /* αブレンド計算用テーブル                 */

/*
 * α値は 0-16 の間
 */

void
lvnsimage_sximage_init(void)
{
    int i, j;
    for (i=0;i<17;i++) {
        /* multiple 計算用テーブル */
        for (j=0;j<256;j++) {
            cmap_m[i][j] = j * i/16;
        }
        /* screen 計算用テーブル */
        for (j=0;j<256;j++) {
            cmap_s[i][j] = j + (255-j)*i/16;
        }
        /* αブレンド計算用テーブル */
        for (j=-255;j<256;j++) {
            cmap_a[i][j+255] = j *i/16;
        }
    }
}

#define SXIMAGE_TPIXEL_ALPHABLEND(s,r,g,b,r2,g2,b2,p)\
(s->tcmap->red[r+cmap_a[p][r2-r+255]] | s->tcmap->green[g+cmap_a[p][g2-g+255]] | s->tcmap->blue[b+cmap_a[p][b2-b+255]])

/*
 * LvnsImage から SXImage に対してパレットの設定を行う
 */
void
lvnsimage_set_pal_to_sximage(LvnsImage *image, SuperXImage *sximage)
{
    int i;
    for (i=0;i<image->palette_num;i++) {
      super_ximage_set_palette(sximage, i, 
                               image->palette[i][0],
                               image->palette[i][1],
                               image->palette[i][2]);
    }       
}

/*
 * LvnsImage から SXImage に対してパレットの設定を行う (Multiple)
 */
void
lvnsimage_set_pal_to_sximage_multi(LvnsImage *image, SuperXImage *sximage, int par16)
{
    int i;

    if (par16 >= 16) {
        lvnsimage_set_pal_to_sximage(image, sximage);
        return;
    }

    for (i=0;i<image->palette_num;i++) {
      super_ximage_set_palette(sximage, i, 
                               cmap_m[par16][image->palette[i][0]],
                               cmap_m[par16][image->palette[i][1]],
                               cmap_m[par16][image->palette[i][2]]);
    }
}

/*
 * LvnsImage から SXImage に対してパレットの設定を行う (Screen)
 */
void
lvnsimage_set_pal_to_sximage_screen(LvnsImage *image, SuperXImage *sximage, int par16)
{
    int i;
    for (i=0;i<image->palette_num;i++) {
      super_ximage_set_palette(sximage, i, 
                               cmap_s[par16][image->palette[i][0]],
                               cmap_s[par16][image->palette[i][1]],
                               cmap_s[par16][image->palette[i][2]]);
    }
}

/* 
 * LvnsImage 全体をコピー
 */
void
lvnsimage_copy_to_sximage(LvnsImage *image, 
                          SuperXImage *sximage,
                          int x, int y)
{
    int i,j;
    u_char *p = image->data;

    x += image->xoffset;
    y += image->yoffset;

    /* XXX fast put pixel .... */
    if (sximage->ximage->bits_per_pixel == 24) {
        /* start position */
        u_char *q = sximage->ximage->data +
            y * sximage->ximage->bytes_per_line + x * 3;
        if (sximage->ximage->byte_order     == LSBFirst) {
            for (i=0; i<image->rheight; i++) {
                u_char *s = q;
                for (j=0; j<image->rwidth; j++, p++, s+=3) {
                    STORE24_LSB(s, sximage->pixels[*p]);
                }
                q += sximage->ximage->bytes_per_line;
            }
        } else {
            for (i=0; i<image->rheight; i++) {
                u_char *s = q;
                for (j=0; j<image->rwidth; j++, p++, s+=3) {
                    STORE24_HSB(s, sximage->pixels[*p]);
                }
                q += sximage->ximage->bytes_per_line;
            }
        }
    } else if (/*sximage->ximage->byte_order     == LSBFirst &&*/
               (sximage->ximage->bits_per_pixel == 16 ||
                sximage->ximage->bits_per_pixel == 15)) {

        /* start position */
        u_char *q = sximage->ximage->data +
            y * sximage->ximage->bytes_per_line + x * 2;

        for (i=0; i<image->rheight; i++) {
            u_char *s = q;
            for (j=0; j<image->rwidth; j++, p++, s+=2) {
                *(uint16_t*)s  = sximage->pixels[*p];
                /* STORE16_LSB(s, sximage->pixels[*p]); */
            }
            q += sximage->ximage->bytes_per_line;
        }
        
    } else if (/*sximage->ximage->byte_order     == LSBFirst &&*/
               sximage->ximage->bits_per_pixel == 32) {

        /* start position */
        u_char *q = sximage->ximage->data +
            y * sximage->ximage->bytes_per_line + x * 4;

        for (i=0; i<image->rheight; i++) {
            u_char *s = q;
            for (j=0; j<image->rwidth; j++, p++, s+=4) {
                *(uint32_t*)s  = sximage->pixels[*p];
                /* STORE32_LSB(s, sximage->pixels[*p]); */
            }
            q += sximage->ximage->bytes_per_line;
        }

    } else if (sximage->ximage->bits_per_pixel == 8) {
        /* start position */
        u_char *q = sximage->ximage->data +
            y * sximage->ximage->bytes_per_line + x;
        for (i=0; i<image->rheight; i++) {
            u_char *s = q;
            for (j=0; j<image->rwidth; j++, p++, s++) {
                *s  = sximage->pixels[*p];
            }
            q += sximage->ximage->bytes_per_line;
        }
    } else {
        /* put pixels */
        for (i=0; i<image->rheight; i++) {
            for (j=0; j<image->rwidth; j++) {
                XPutPixel(sximage->ximage, x + j, y + i, sximage->pixels[*p++]);
            }    
        }
    }
}

/**
 * LvnsImage から SXImage へ指定した領域のコピーを行う。
 * src のオフセットのチェックは行っていない
 */
void
lvnsimage_copy_area_to_sximage(LvnsImage *src, SuperXImage *dst, 
                               int x, int y, int w, int h, int x2, int y2)
{
    int i,j;

    if (x < 0) {
		w  += x;
		x2 -= x;
		x  = 0;
    }
    if (y < 0) {
		h  += y;
		y2 -= y;
		y  = 0;
    }
    if (w <= 0 || h <= 0)
      return;

    if (x2 < 0) {
		w += x2;
		x -= x2;
		x2 = 0;
    }
    if (y2 < 0) {
		h += y2;
		y -= y2;
		y2 = 0;
    }
    if (w <= 0 || h <= 0)
		return;

    if (x + w > src->rwidth)          w = src->rwidth - x;
    if (y + h > src->rheight)         h = src->rheight - y;
    if (x2 + w > dst->ximage->width)  w = dst->ximage->width - x2;
    if (y2 + h > dst->ximage->height) h = dst->ximage->height - y2;

    /* XXX fast put pixel .... */
    if (dst->ximage->bits_per_pixel == 24) {
        /* start position */
        u_char *p = src->data + y * src->rwidth + x;
        u_char *q = dst->ximage->data +
            y2 * dst->ximage->bytes_per_line + x2 * 3;
        if (dst->ximage->byte_order     == LSBFirst) {
            for (i=0; i<h; i++) {
                u_char *r = p;
                u_char *s = q;
                for (j=0; j<w; j++, r++, s+=3) {
                    STORE24_LSB(s, dst->pixels[*r]);
                }
                p += src->rwidth;
                q += dst->ximage->bytes_per_line;
            }
        } else {
            for (i=0; i<h; i++) {
                u_char *r = p;
                u_char *s = q;
                for (j=0; j<w; j++, r++, s+=3) {
                    STORE24_HSB(s, dst->pixels[*r]);
                }
                p += src->rwidth;
                q += dst->ximage->bytes_per_line;
            }
        }
    } else if (/*dst->ximage->byte_order     == LSBFirst &&*/
               (dst->ximage->bits_per_pixel == 16 ||
                dst->ximage->bits_per_pixel == 15)) {
        /* start position */
        u_char *p = src->data + y * src->rwidth + x;
        u_char *q = dst->ximage->data +
            y2 * dst->ximage->bytes_per_line + x2 * 2;

        for (i=0; i<h; i++) {
            u_char *r = p;
            u_char *s = q;
            for (j=0; j<w; j++, r++, s+=2) {
                *(uint16_t*)s  = dst->pixels[*r];
                /* STORE16_LSB(s, dst->pixels[*r]); */
            }
            p += src->rwidth;
            q += dst->ximage->bytes_per_line;
        }
        
    } else if (/*dst->ximage->byte_order     == LSBFirst &&*/
               dst->ximage->bits_per_pixel == 32) {

        /* start position */
        u_char *p = src->data + y * src->rwidth + x;
        u_char *q = dst->ximage->data +
            y2 * dst->ximage->bytes_per_line + x2 * 4;

        for (i=0; i<h; i++) {
            u_char *r = p;
            u_char *s = q;
            for (j=0; j<w; j++, r++, s+=4) {
                *(uint32_t*)s  = dst->pixels[*r];
                /* STORE32_LSB(s, dst->pixels[*r]); */
            }
            p += src->rwidth;
            q += dst->ximage->bytes_per_line;
        }
    } else if (dst->ximage->bits_per_pixel == 8) {

        /* start position */
        u_char *p = src->data + y * src->rwidth + x;
        u_char *q = dst->ximage->data +
            y2 * dst->ximage->bytes_per_line + x2;

        for (i=0; i<h; i++) {
            u_char *r = p;
            u_char *s = q;
            for (j=0; j<w; j++, r++, s++) {
                *s  = dst->pixels[*r];
            }
            p += src->rwidth;
            q += dst->ximage->bytes_per_line;
        }

    } else {
        /* put pixels */
        u_char *p = src->data + y * src->rwidth + x;
        for (i=0; i<h; i++) {
            u_char *r = p;
            for (j=0; j<w; j++, r++) {
                XPutPixel(dst->ximage, x2 + j, y2 + i, dst->pixels[*r]);
            }    
            p += src->rwidth;
        }
    }
} 

/**
 * LvnsImage から SXImage へ指定した領域のコピーを行う。
 * src のオフセットのチェックは行っていない
 * 同じ場所を指定する版
 */
void
lvnsimage_copy_area_to_sximage2(LvnsImage *src, SuperXImage *dst, 
                               int x, int y, int w, int h)
{
    lvnsimage_copy_area_to_sximage(src, dst, x, y, w, h, x, y);
}

/*
 * 16×16の網目マスクパターンでコピーする
 * 指定した座標から mask で指定されたパターンで処理
 */
void 
lvnsimage_copy_mask_unit_to_sximage(LvnsImage *src, SuperXImage *dst, int x, int y, int x2, int y2, int mask)
{
    int i, j;
    static int xoff[] = { 3, 1, 1, 3,  2, 0, 0, 2,  3, 1, 1, 3, 2, 0, 0, 2 };
    static int yoff[] = { 0, 2, 0, 2,  1, 3, 1, 3,  1, 3, 1, 3, 0, 2, 0, 2 };

    y += yoff[mask];
    x += xoff[mask];
	y2 += yoff[mask];
	x2 += xoff[mask];

    if (dst->ximage->bits_per_pixel == 24) {
        u_char *q = dst->ximage->data +
            y2 * dst->ximage->bytes_per_line + x2 * 3;
        u_char *p = src->data + y * src->rwidth + x;
        if (dst->ximage->byte_order     == LSBFirst) {
            for (i=0; i<16; i+=4) {
                u_char *s = q;
                u_char *r = p;
                for (j=0; j<16; j+=4, s+=12, r+=4) {
                    STORE24_LSB(s, dst->pixels[*r]);
                }
                q += dst->ximage->bytes_per_line * 4;
                p += src->rwidth * 4;
            }
        } else {
            for (i=0; i<16; i+=4) {
                u_char *s = q;
                u_char *r = p;
                for (j=0; j<16; j+=4, s+=12, r+=4) {
                    STORE24_HSB(s, dst->pixels[*r]);
                }
                q += dst->ximage->bytes_per_line * 4;
                p += src->rwidth * 4;
            }
        }
    } else if (/*dst->ximage->byte_order     == LSBFirst &&*/
               (dst->ximage->bits_per_pixel == 16 ||
                dst->ximage->bits_per_pixel == 15)) {
        /* start position */
        u_char *q = dst->ximage->data +
            y2 * dst->ximage->bytes_per_line + x2 * 2;
        u_char *p = src->data + y * src->rwidth + x;
        for (i=0; i<16; i+=4) {
            u_char *s = q;
            u_char *r = p;
            for (j=0; j<16; j+=4, s+=8, r+=4) {
                *(uint16_t*)s  = dst->pixels[*r];
                /* STORE16_LSB(s, dst->pixels[*r]); */
            }
            q += dst->ximage->bytes_per_line * 4;
            p += src->rwidth * 4;
        }
    } else if (/*dst->ximage->byte_order     == LSBFirst &&*/
               dst->ximage->bits_per_pixel == 32) {
        /* start position */
        u_char *q = dst->ximage->data +
            y2 * dst->ximage->bytes_per_line + x2 * 4;
        u_char *p = src->data + y * src->rwidth + x;
        for (i=0; i<16; i+=4) {
            u_char *s = q;
            u_char *r = p;
            for (j=0; j<16; j+=4, s+=16, r+=4) {
                *(uint32_t*)s  = dst->pixels[*r];
                /* STORE32_LSB(s, dst->pixels[*r]); */
            }
            q += dst->ximage->bytes_per_line * 4;
            p += src->rwidth * 4;
        }
    } else if (dst->ximage->bits_per_pixel == 8) {
        /* start position */
        u_char *q = dst->ximage->data +
            y2 * dst->ximage->bytes_per_line + x2;
        u_char *p = src->data + y * src->rwidth + x;
        for (i=0; i<16; i+=4) {
            u_char *s = q;
            u_char *r = p;
            for (j=0; j<16; j+=4, s+=4, r+=4) {
                *s  = dst->pixels[*r];
            }
            q += dst->ximage->bytes_per_line * 4;
            p += src->rwidth * 4;
        }
    } else {
        /* slow version ... */
        for (i=0; i<16; i+=4) {
            u_char *p = src->data + (y+i) * src->rwidth + x;
            for (j=0; j<16; j+=4, p+=4) {
                XPutPixel(dst->ximage, x2 + j, y2 + i, dst->pixels[*p]);
            }    
        }
    }
}

/*
 * 16×16の矩形マスクパターンでコピーする。
 * 指定した座標から mask で指定されたパターンで処理
 */
void
lvnsimage_copy_squaremask_unit_to_sximage(LvnsImage *src, SuperXImage *dst, int x, int y, int x2, int y2, int mask)
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
        u_char *p  = src->data + (y + y0)     * src->rwidth + x;
        XPutPixel(dst->ximage, x2 + x0,    y2 + y0,    dst->pixels[p[x0]]);
        XPutPixel(dst->ximage, x2 + 31-x0, y2 + y0,    dst->pixels[p[31-x0]]);
        if (y + 31 - y0 < dst->ximage->height) {
            u_char *p2 = src->data + (y + 31 -y0) * src->rwidth + x;
            XPutPixel(dst->ximage, x2 + x0,    y2 + 31-y0, dst->pixels[p2[x0]]);
            XPutPixel(dst->ximage, x2 + 31-x0, y2 + 31-y0, dst->pixels[p2[31-x0]]);
        }
        x0--;
        y0++;
    }
}

static void
sximage_vline(SuperXImage *sximage, int x, int y, int h, int idx)
{
    int i;
    for (i=0; i<h; i++) {
        XPutPixel(sximage->ximage, x, y+i, sximage->pixels[idx]);
    }
}

static void
sximage_hline(SuperXImage *sximage, int x, int y, int w, int idx)
{
    int i;
    for (i=0; i<w; i++) {
        XPutPixel(sximage->ximage, x+i, y, sximage->pixels[idx]);
    }
}

void
sximage_box(SuperXImage *image, int x, int y, int w, int h, int idx)
{
    sximage_vline(image, x, y, h, idx);
    sximage_vline(image, x+w-1, y, h, idx);
    sximage_hline(image, x, y, w, idx);
    sximage_hline(image, x, y+h-1, w, idx);
}
