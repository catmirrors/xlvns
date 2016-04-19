/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: lvnsimage.c,v 1.38 2001/08/11 19:59:36 tf Exp $
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lvnsimage.h"

/**
 * LVNS 用イメージデータ生成
 */
LvnsImage *
lvnsimage_new(int width, int height, int xoffset, int yoffset)
{
    LvnsImage *image;
    if ((image = malloc(sizeof *image)) == NULL) {
        perror("lvnsimage_new");
        exit(1);
    }

    image->width  = width;
    image->height = height;

    image->rwidth  = width  - xoffset;
    image->rheight = height - yoffset;
    image->xoffset = xoffset;
    image->yoffset = yoffset;

    /* initialize imaga area and work area */
    if ((image->data = malloc(image->rwidth * image->rheight)) == NULL) {
        perror("lvnsimage_new");
        exit(1);
    }

    image->black = 0;
    lvnsimage_clear(image);
    return image;
}

/**
 * LVNS 用イメージデータの破棄
 */
void
lvnsimage_delete(LvnsImage *image)
{
    if (image) {
        free(image->data);
        free(image);
    }
}

/**
 * LVNS用イメージデータの消去 (黒で消去)
 */
void
lvnsimage_clear(LvnsImage *img)
{
    memset(img->data, img->black, img->rwidth * img->rheight);
    img->palette_num = 0;
    img->transparent = -1;
#if 0
    img->palette[0][0] = img->palette[0][1] = img->palette[0][2] = 0;
    img->palette_num = 1;
#endif
}


/**
 * LVNS用イメージデータの消去 (透明で消去)
 */
void
lvnsimage_clear_transparent(LvnsImage *img)
{
    if (img->transparent == -1)
        img->transparent = img->black;
    memset(img->data, img->transparent, img->rwidth * img->rheight);
#if 0
    img->palette[img->transparent][0] = 
    img->palette[img->transparent][1] = 
    img->palette[img->transparent][2] = 0;
#endif
}

/**
 * LVNS用イメージデータの特定領域を0番のピクセルで消去
 */
void
lvnsimage_clear_area(LvnsImage *img, int x, int y, int w, int h)
{
    int i;
    for (i=0; i<h; y++, i++)
        memset(img->data + img->rwidth * y + x, img->black, w);
}

/**
 * LVNS用イメージデータの特定領域を透明色のピクセルで消去
 */
void
lvnsimage_clear_area_transparent(LvnsImage *img, int x, int y, int w, int h)
{
    int i;
    for (i=0; i<h; y++, i++)
        memset(img->data + img->rwidth * y + x, img->transparent, w);
}


/**
 * LVNS用イメージデータを指定した色で消去
 */
void
lvnsimage_clear_color(LvnsImage *img, int r, int g, int b)
{
    memset(img->data, 0, img->rwidth * img->rheight);
    img->palette[0][0] = r;
    img->palette[0][1] = g;
    img->palette[0][2] = b;
    img->palette_num = 1;
}

/**
 * LVNS用イメージデータを指定した色で消去
 */
void
lvnsimage_clear_index(LvnsImage *img, int idx)
{
    memset(img->data, idx, img->rwidth * img->rheight);
}


/**
 * LVNS用イメージの特定領域を指定した色で塗り潰す
 */
void
lvnsimage_boxfill(LvnsImage *img, int idx,
                  int x, int y, int x2, int y2)
{
    int i;
    for (i=y; i<=y2; i++)
        memset(img->data + img->rwidth * i + x, idx, x2-x+1);
}

/**
 * パレット情報のみコピーする
 */
void
lvnsimage_copy_palette(LvnsImage *src, LvnsImage *dst)
{
    dst->transparent = src->transparent;
    dst->palette_num = src->palette_num;
    memcpy(dst->palette, src->palette, src->palette_num * 3);
}


/**
 * 単純な全体のコピーを行う。
 * はみでのチェックは行っていない
 */
void
lvnsimage_copy(LvnsImage *src, LvnsImage *dst)
{
    dst->transparent = src->transparent;
    dst->palette_num = src->palette_num;
    memcpy(dst->palette, src->palette, src->palette_num * 3);
    
    if (src->rwidth == dst->rwidth && src->rheight == dst->rheight) {
        memcpy(dst->data, src->data, src->rwidth * src->rheight);
    } else {
        int i,j;
        u_char *p = src->data;

        memset(dst->data, dst->black, dst->rwidth * dst->rheight);

        for (i=0; i<src->rheight; i++) {
            u_char *q = 
                dst->data + (i + src->yoffset) * dst->rwidth + src->xoffset;
            for (j=0; j<src->rwidth; j++, q++, p++) {
                *q = *p;
            }
        }    
    }
}

/**
 * 単純な全体のコピーを行う
 * 絵がない部分は col の色で塗り潰す
 */
void
lvnsimage_copy2(LvnsImage *src, LvnsImage *dst, int col)
{
    dst->transparent = src->transparent;
    dst->palette_num = src->palette_num;
    memcpy(dst->palette, src->palette, src->palette_num * 3);
    
    if (src->rwidth == dst->rwidth && src->rheight == dst->rheight) {
        memcpy(dst->data, src->data, src->rwidth * src->rheight);
    } else {
        int i,j;
        u_char *p = src->data;

        /* 他の部分は消去... XXX */
        memset(dst->data, col, dst->rwidth * dst->rheight);

        for (i=0; i<src->rheight; i++) {
            u_char *q = 
                dst->data + (i + src->yoffset) * dst->rwidth + src->xoffset;
            for (j=0; j<src->rwidth; j++, q++, p++) {
                *q = *p;
            }
        }    
    }
}


/**
 * 指定した領域のコピーを行う。
 * src のオフセットのチェックは行っていない。
 */
void
lvnsimage_copy_area(LvnsImage *src, LvnsImage *dst, int x, int y, int w, int h, int x2, int y2)
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

    for (i=0; i<h; i++) {
        u_char *p = src->data + (i + y) * src->rwidth + x;
        u_char *q = dst->data + (i + y2) * dst->rwidth + x2;
        for (j=0; j<w; j++, q++, p++) {
            *q = *p;
        }
    }
}

void
lvnsimage_copy_area2(LvnsImage *src, LvnsImage *dst, int x, int y, int w, int h) 
{
    lvnsimage_copy_area(src, dst, x, y, w, h, x, y);
}


/**
 * パレットを混ぜこむ
 */
void
lvnsimage_add_pal(LvnsImage *dst, u_char pal[][3], int transparent, int *index, int num)
{
    int i, j;
    for (i=0;i<num;i++) {
        if (i != transparent) {
            if (dst->palette_num >= 256) {
                fprintf(stderr, "WARN: over palette!! %d/%d\n",
                        i, num);
                return;
            }
            for (j=0;j<dst->palette_num;j++) {
                if (dst->transparent != j &&
                    pal[i][0] == dst->palette[j][0] &&
                    pal[i][1] == dst->palette[j][1] &&
                    pal[i][2] == dst->palette[j][2]) {
                    index[i] = j;
                    goto end;
                }
            }
            dst->palette[dst->palette_num][0] = pal[i][0];
            dst->palette[dst->palette_num][1] = pal[i][1];
            dst->palette[dst->palette_num][2] = pal[i][2];
            index[i] = dst->palette_num++;
        end:;
        }
    }
}


/* 
 * 強制合成。パレットもまぜて、index 変換を行う。
 * 画像のはみでのチェックは行っていないため、はみでると破綻する
 */
void
lvnsimage_add(LvnsImage *src, LvnsImage *dst, int x, int y)
{
    int index[256];
    int i,j;
    u_char *p;

    x += src->xoffset;
    y += src->yoffset;

    lvnsimage_add_pal(dst, src->palette, src->transparent, index, src->palette_num);

    p = src->data;
    for (i=0; i<src->rheight; i++) {
        u_char *q = dst->data + (y + i) * dst->rwidth + x;
        for (j=0; j<src->rwidth; j++, q++, p++) {
            if (*p != src->transparent) {
                *q = index[*p];
            }
        }    
    }
}

/* 
 * 強制合成。パレットは合成しない (dst 側のものをそのまま使う)
 * 画像のはみでのチェックは行っていないため、はみでると破綻する
 */
void
lvnsimage_add2(LvnsImage *src, LvnsImage *dst, int x, int y)
{
    int i,j;
    u_char *p;

    x += src->xoffset;
    y += src->yoffset;

    p = src->data;
    for (i=0; i<src->rheight; i++) {
        u_char *q = dst->data + (y + i) * dst->rwidth + x;
        for (j=0; j<src->rwidth; j++, q++, p++) {
            if (*p != src->transparent) {
                *q = *p;
            }
        }    
    }
}

/* 
 * 強制合成。ToHeart のカレンダー処理専用
 * 画像のはみでのチェックは行っていないため、はみでると破綻する
 * (dummy 引数は MGL 用の image_add3 との互換のためのダミーです by TF)
 */
void
lvnsimage_add3(LvnsImage *src, LvnsImage *dst, int x, int y, int pixoff, int transparent, int dummy)
{
    int i,j;
    u_char *p;

    x += src->xoffset;
    y += src->yoffset;

    p = src->data;
    for (i=0; i<src->rheight; i++) {
        u_char *q = dst->data + (y + i) * dst->rwidth + x;
        for (j=0; j<src->rwidth; j++, q++, p++) {
            if (*q == transparent && *p != src->transparent) {
                *q = *p + pixoff;
            }
        }    
    }
}

/* 
 * 強制合成。パレットはオフセット処理
 * 画像のはみでのチェックは行っていないため、はみでると破綻する
 */
void
lvnsimage_add4(LvnsImage *src, LvnsImage *dst, int x, int y, int pixoff)
{
    int i,j;
    u_char *p;

    x += src->xoffset;
    y += src->yoffset;

    p = src->data;
    for (i=0; i<src->rheight; i++) {
        u_char *q = dst->data + (y + i) * dst->rwidth + x;
        for (j=0; j<src->rwidth; j++, q++, p++) {
            if (*p != src->transparent) {
                *q = *p + pixoff;
            }
        }    
    }
}

/*
 * 16×16 の網目マスクパターンによる合成処理
 * @param mask 1-15の範囲
 * @param index 合成用 index 変換マップ
 */
static void 
lvnsimage_add_mask_base(LvnsImage *src, LvnsImage *dst, int x, int y, int mask, int idx_off)
{
    int i, j;
    static int xoff[] = { 3, 1, 1, 3,  2, 0, 0, 2,  3, 1, 1, 3, 2, 0, 0, 2 };
    static int yoff[] = { 0, 2, 0, 2,  1, 3, 1, 3,  1, 3, 1, 3, 0, 2, 0, 2 };
    for (i=yoff[mask]; i<src->rheight; i+=4) {
        u_char *p = src->data + i * src->rwidth + xoff[mask];
        u_char *q = dst->data + (i + y) * dst->rwidth + x + xoff[mask];
        for (j=xoff[mask]; j<src->rwidth; j+=4, q+=4, p+=4) {
            if (*p != src->transparent) {
                *q = idx_off + *p;
            }
        }    
    }
}

/*
 * 16×16の網目マスクパターンによる合成処理
 * パレットはまぜこむ
 */
void
lvnsimage_add_mask(LvnsImage *src, LvnsImage *dst, int x, int y, int mask, int idx_off)
{
    int i;
    x += src->xoffset;
    y += src->yoffset;
    for (i=0;i<mask;i++)
        lvnsimage_add_mask_base(src, dst, x, y, i, idx_off);
}

/*
 * 16×16の網目マスクパターンでコピーする
 * 指定した座標から mask で指定されたパターンで処理
 */
void 
lvnsimage_copy_mask_unit(LvnsImage *src, LvnsImage *dst, int x, int y, int mask)
{
    int i, j;
    static int xoff[] = { 3, 1, 1, 3,  2, 0, 0, 2,  3, 1, 1, 3, 2, 0, 0, 2 };
    static int yoff[] = { 0, 2, 0, 2,  1, 3, 1, 3,  1, 3, 1, 3, 0, 2, 0, 2 };

    y += yoff[mask];
    x += xoff[mask];
    for (i=0; i<16; i+=4) {
        u_char *p = src->data + (y+i) * src->rwidth + x;
        u_char *q = dst->data + (y+i) * dst->rwidth + x;
        for (j=0; j<16; j+=4, q+=4, p+=4) {
            *q = *p;
        }    
    }
}

/*
 * 16×16の矩形マスクパターンでコピーする。
 * 指定した座標から mask で指定されたパターンで処理
 */
void
lvnsimage_copy_squaremask_unit(LvnsImage *src, LvnsImage *dst, int x, int y, int mask)
{
    int x0, y0;

    if (mask >= 16) {
        x0 = 15;
        y0 = mask - 15;
    } else {
        x0 = mask;
        y0 = 0;
    }

    while (x0 >= 0 && y0 < 16) {
        u_char *p  = src->data + (y + y0)     * src->rwidth + x;
        u_char *q  = dst->data + (y + y0)     * dst->rwidth + x;
        q[x0]     = p[x0];
        q[31-x0]  = p[31-x0];
        if (y + 31 - y0 < dst->rheight) {
            u_char *p2 = src->data + (y + 31 -y0) * src->rwidth + x;
            u_char *q2 = dst->data + (y + 31 -y0) * dst->rwidth + x;
            q2[x0]    = p2[x0];
            q2[31-x0] = p2[31-x0];
        }
        x0--;
        y0++;
    }
}

/*
 * パレットの外部からの設定
 * 個数指定あり
 */
void
lvnsimage_set_palette(LvnsImage *image, u_char paletteinfo[][3], int num) 
{
    image->palette_num = num;
    memcpy(image->palette, paletteinfo, 3 * num);
}

/*
 * パレットの外部からの上書き
 */
void
lvnsimage_set_palette2(LvnsImage *image, u_char paletteinfo[][3], int num) 
{
    memcpy(image->palette, paletteinfo, 3 * num);
}

/*
 * パレットの外部からの上書き
 */
void
lvnsimage_set_palette3(LvnsImage *image, u_char *paletteinfo, int pos, int num) 
{
    int i;
    for (i=0; i<num;i++) {
        image->palette[pos][2] = paletteinfo[2];
        image->palette[pos][1] = paletteinfo[1];
        image->palette[pos][0] = paletteinfo[0];
        paletteinfo+=3;
        pos++;
    }
}

/*
 * パレットの外部からの上書き
 */
void
lvnsimage_set_palette4(LvnsImage *image, u_char *paletteinfo, int pos, int num) 
{
    int i;
    for (i=0; i<num;i++) {
        image->palette[pos][2] = paletteinfo[0];
        image->palette[pos][1] = paletteinfo[1];
        image->palette[pos][0] = paletteinfo[2];
        paletteinfo+=3;
        pos++;
    }
}

/*
 * 内部関数。点をうつ。
 * はみでのチェックを行っている
 */
static void
lvnsimage_put_pixel(LvnsImage *image, int x, int y, int pixel)
{
    if (x >= 0 && x < image->rwidth &&
        y >= 0 && y < image->rheight) {
        *(image->data + y * image->rwidth + x) = pixel;
    }
}

/*
 * 内部関数
 * 指定したサイズの矩形で点を打つ
 */ 
static void 
lvnsimage_put_box_pixel(LvnsImage *image, int x, int y, int size, int pixel)
{
    int i, j;
    for (i=0;i<size;i++) {
        for (j=0;j<size;j++) {
            lvnsimage_put_pixel(image, x+i-size/2, y+j-size/2, pixel); 
        }
    }
}

/*
 * 指定したサイズの矩形の点で円を書く
 */
void
lvnsimage_circle(LvnsImage *image, int xc, int yc, int r, int size, int pixel)
{
    int x, y, c;
    x = r; y = 0;c=0;
    while (x >= y) {
        if (c++ == 0) {
            lvnsimage_put_box_pixel(image, xc + x, yc + y, size, pixel);
            lvnsimage_put_box_pixel(image, xc + x, yc - y, size, pixel);
            lvnsimage_put_box_pixel(image, xc - x, yc + y, size, pixel);
            lvnsimage_put_box_pixel(image, xc - x, yc - y, size, pixel);
            lvnsimage_put_box_pixel(image, xc + y, yc + x, size, pixel);
            lvnsimage_put_box_pixel(image, xc + y, yc - x, size, pixel);
            lvnsimage_put_box_pixel(image, xc - y, yc + x, size, pixel);
            lvnsimage_put_box_pixel(image, xc - y, yc - x, size, pixel);
            if ((r -= (y++ << 1) - 1) < 0) 
                r += (x-- -1) << 1;
        } 
        c %= (size / 2);
    }
}

/*
 * 24×24 のビットマップパターンを pixel の色で描画する
 * ToHeart用
 */ 
void
lvnsimage_put_pattern24(LvnsImage *image, int x, int y, int pixel, char *data)
{
    /* oh! slow version ... */
    int i, j, x2;
    for (i=0; i<24; i++, y++) {
        x2 = x;
        for (j=0; j<3; j++) {
            u_char c = *data++;
            if ((c & 0x80))
                lvnsimage_put_pixel(image, x2, y, pixel);
            x2++;
            if ((c & 0x40))
                lvnsimage_put_pixel(image, x2, y, pixel);
            x2++;
            if ((c & 0x20))
                lvnsimage_put_pixel(image, x2, y, pixel);
            x2++;
            if ((c & 0x10))
                lvnsimage_put_pixel(image, x2, y, pixel);
            x2++;
            if ((c & 0x08))
                lvnsimage_put_pixel(image, x2, y, pixel);
            x2++;
            if ((c & 0x04))
                lvnsimage_put_pixel(image, x2, y, pixel);
            x2++;
            if ((c & 0x02))
                lvnsimage_put_pixel(image, x2, y, pixel);
            x2++;
            if ((c & 0x01))
                lvnsimage_put_pixel(image, x2, y, pixel);
            x2++;
        }
    }
}

/*
 * 24×24 のビットマップパターンを pixel の色で描画する
 * 雫/痕用
 */ 
void
lvnsimage_put_pattern24_2(LvnsImage *image, int x, int y, int pixel, char *data)
{
    /* oh! slow version ... */
    int i, j, x2, y2;
    for (j=0; j<3; j++, x+=8) {
        y2 = y;
        for (i=0; i<24; i++, y2++) {
            u_char c = *data++;
            x2 = x;
            if ((c & 0x80))
                lvnsimage_put_pixel(image, x2, y2, pixel);
            x2++;
            if ((c & 0x40))
                lvnsimage_put_pixel(image, x2, y2, pixel);
            x2++;
            if ((c & 0x20))
                lvnsimage_put_pixel(image, x2, y2, pixel);
            x2++;
            if ((c & 0x10))
                lvnsimage_put_pixel(image, x2, y2, pixel);
            x2++;
            if ((c & 0x08))
                lvnsimage_put_pixel(image, x2, y2, pixel);
            x2++;
            if ((c & 0x04))
                lvnsimage_put_pixel(image, x2, y2, pixel);
            x2++;
            if ((c & 0x02))
                lvnsimage_put_pixel(image, x2, y2, pixel);
            x2++;
            if ((c & 0x01))
                lvnsimage_put_pixel(image, x2, y2, pixel);
            x2++;
        }
    }
}

/*
 * 拡大/縮小コピー 
 * ソース側の範囲チェックをしていないので注意
 */
void
lvnsimage_copy_scale(LvnsImage *src, int sx, int sy, int sw, int sh, 
                     LvnsImage *dst, int dx, int dy, int dw, int dh)
{
    int i, j;
    int xpos[640]; /* XXX */
    int ypos[480]; /* XXX */
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

#if 0    
    fprintf(stderr, "scale: %d %d %d %d\n", x, y, w, h);
#endif

    /* 対応座標算出 */
    for (i=0; i<w; i++)
        xpos[i] = sx + (xoff + i) * sw / dw;
    for (i=0; i<h; i++)
        ypos[i] = sy + (yoff + i) * sh / dh;

    /* こぴー */
    {
        u_char *q = dst->data + y * dst->rwidth + x;
        for (i=0; i<h; i++) {
            u_char *r = src->data + ypos[i] * src->rwidth;
            u_char *s = q;
            for (j=0; j<w; j++) {
                *s++ = r[xpos[j]];
            }
            q += dst->rwidth;
        }
    }

}

static void
lvnsimage_vline(LvnsImage *image, int x, int y, int h, int idx)
{
    int i;
    for (i=0; i<h; i++) {
        u_char *p = image->data + (y+i) * image->rwidth + x;
        *p = idx;
    }
}

static void
lvnsimage_hline(LvnsImage *image, int x, int y, int w, int idx)
{
    int i;
    u_char *p = image->data + y * image->rwidth + x;
    for (i=0; i<w; i++) {
        *p++ = idx;
    }
}

void
lvnsimage_box(LvnsImage *image, int x, int y, int w, int h, int idx)
{
    lvnsimage_vline(image, x, y, h, idx);
    lvnsimage_vline(image, x+w-1, y, h, idx);
    lvnsimage_hline(image, x, y, w, idx);
    lvnsimage_hline(image, x, y+h-1, w, idx);
}

lvnsimage_set_sepia(LvnsImage *img)
{
    int i;
    for (i = 0; i < img->palette_num; i++) {
        img->palette[i][1] = img->palette[i][1] >> 1;
        img->palette[i][2] = img->palette[i][2] >> 3;
    }
}
