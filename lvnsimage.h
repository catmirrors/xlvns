/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: lvnsimage.h,v 1.32 2001/08/11 19:59:36 tf Exp $
 */

/*
 * LVNSImage
 * 8bit indexed colored image
 */

#ifndef __lvnsimage_h_
#define __lvnsimage_h_

#include <sys/types.h>

typedef struct LvnsImage {

    int width;        /* 画像の幅   */
    int height;       /* 画像の高さ */

    int xoffset;      /* 画像のオフセット (x) */
    int yoffset;      /* 画像のオフセット (y) */

    int rwidth;       /* 実サイズ */
    int rheight;

    u_char *data;     /* 画像データ     */
 
    int transparent;  /* 透明色 index 値: add の場合に有効   */
    int black;        /* 「黒」の index 値: 消去の場合に有効 */

    u_char palette [256][3]; /* パレットデータ */
    int palette_num;  /* パレット個数   */

} LvnsImage;

LvnsImage *lvnsimage_new(int width, int height, int xoffset, int yoffset);
void lvnsimage_delete(LvnsImage *image);

LvnsImage *lvnsimage_load_lfg(const u_char *data, LvnsImage *over);
void lvnsimage_load_lfg_palette(LvnsImage *img, const u_char *data);
LvnsImage *lvnsimage_load_lf2(const u_char *data, LvnsImage *over);

void lvnsimage_add_pal(LvnsImage *dst, u_char pal[][3], int transparent, int *index, int num);

void lvnsimage_clear(LvnsImage *img);
void lvnsimage_clear_transparent(LvnsImage *img);
void lvnsimage_clear_area(LvnsImage *img, int x, int y, int w, int h);
void lvnsimage_clear_area_transparent(LvnsImage *img, int x, int y, int w, int h);
void lvnsimage_clear_color(LvnsImage *img, int r, int g, int b);

void lvnsimage_clear_index(LvnsImage *img, int idx);
void lvnsimage_boxfill(LvnsImage *img, int idx, int x, int y, int x2, int y2);

void lvnsimage_copy_palette(LvnsImage *src, LvnsImage *dst);
void lvnsimage_copy(LvnsImage *src, LvnsImage *dst);
void lvnsimage_copy2(LvnsImage *src, LvnsImage *dst, int col);
void lvnsimage_copy_area(LvnsImage *src, LvnsImage *dst, int x, int y,  
                         int w, int h, int x2, int y2);
void lvnsimage_copy_area2(LvnsImage *src, LvnsImage *dst, int x, int y, 
                          int w, int h);

void lvnsimage_add(LvnsImage *src, LvnsImage *dst, int x, int y);
void lvnsimage_add2(LvnsImage *src, LvnsImage *dst, int x, int y);
void lvnsimage_add3(LvnsImage *src, LvnsImage *dst, int x, int y, int pixoff, int transparent, int dummy);
void lvnsimage_add_mask(LvnsImage *src, LvnsImage *dst, int x, int y, int idx_off, int mask);

void lvnsimage_copy_mask_unit(LvnsImage *src, LvnsImage *dst, int x, int y, int mask);
void lvnsimage_copy_squaremask_unit(LvnsImage *src, LvnsImage *dst, int x, int y, int mask);

void lvnsimage_set_palette(LvnsImage *image, u_char paletteinfo[][3], int no);
void lvnsimage_set_palette2(LvnsImage *image, u_char paletteinfo[][3], int no);
void lvnsimage_set_palette3(LvnsImage *image, u_char *paletteinfo, int pos, int num);
void lvnsimage_set_palette4(LvnsImage *image, u_char *paletteinfo, int pos, int num);

void lvnsimage_circle(LvnsImage *image, int xc, int yc, int r, int size, int pixel);

void lvnsimage_put_pattern24(LvnsImage *image, int x, int y, int pixel, char *data);
void lvnsimage_put_pattern24_2(LvnsImage *image, int x, int y, int pixel, char *data);

void lvnsimage_copy_scale(LvnsImage *src, int sx, int sy, int sw, int sh, 
                          LvnsImage *dst, int dx, int dy, int dw, int dh);

void lvnsimage_box(LvnsImage *image, int x, int y, int w, int h, int idx);

#endif
