/*
 * LEAF Visual Novel System For MGL2
 * (c) Copyright 2001 TF <tf@denpa.org>
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: image.c,v 1.4 2001/08/16 20:04:19 tf Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hsb.h"
#include "pakconv.h"

Image *
image_new(int w, int h, int x, int y)
{
    Image *img;

    img = (Image *)malloc(sizeof(Image));
    img->width = w;
    img->height = h;
    img->xoffset = x;
    img->yoffset = y;
    img->rwidth = w - x;
    img->rheight = h - y;

    img->data = (u_char *)calloc(w * h, sizeof(u_char));

    img->hsb_data = NULL;

    return img;
}

void
image_delete(Image *img)
{
    if (!img)
	return;

    if (img->data) {
	free(img->data);
    }
    if (img->hsb_data) {
	free(img->hsb_data);
    }
    free(img);
}

typedef struct {
    u_short r;
    u_short g;
    u_short b;
    u_short t;
} RGB;

typedef struct {
    int h;
    int s;
    int b;
    int t;
} HSB;

/*
 * RGB→HSB変換メソッド
 *
 *  Hue: 0〜359 (1/2rad, 0 赤, 120 緑, 240 青)
 *  Sat: 0〜255 (0 白方向極, 255彩色極)
 *  Bri: 0〜255 (0 最暗, 255 最明)
 */
#if 0
void RGBtoHSB( HSB* hsb, int r, int g, int b)
{
	int	max, min, diff, hue;

	max = ( r > g)? r: g;
	if( b > max) {
		max = b;
	}

	min = ( r < g)? r: g;
	if( b < min) {
		min = b;
	}

	diff = max - min;

	// 明度はRGB三要素の最高輝度要素で代用
	hsb->b = max;

	// 彩度算出(但し、黒の時は0で割る事になるので特例処理を行う)
	if( max == 0){
		hsb->s = 0;
	} else {
		hsb->s = (255 * diff)/max;
	}

	// 色相算出(彩度0の場合でも算出)
	if( diff != 0){
		if( r == max){
			hue = (( 60 * (g - b))/diff);
		} else if( g == max) {
			hue = 120 + (( 60 * (b - r))/diff);
		} else {
			hue = 240 + (( 60 * (r - g))/diff);
		}
	} else {
		hue = 0;
	}

	hsb->h = hue;
	hsb->t = 0;
}
#else
void RGBtoHSB( HSB* hsb, int r, int g, int b)
{
	double fr, fg, fb;
	double max, min, diff;
	double hue, sat, bri;

	fr = ((double)r)/255.0;
	fg = ((double)g)/255.0;
	fb = ((double)b)/255.0;

	max = ( fr > fg)? fr: fg;
	if( fb > max) {
		max = fb;
	}

	min = ( fr < fg)? fr: fg;
	if( fb < min) {
		min = fb;
	}

	diff = max - min;

	// 明度はRGB三要素の最高輝度要素で代用
	bri = max;

	// 彩度算出(但し、黒の時は0で割る事になるので特例処理を行う)
	if( max == 0){
		sat = 0.0;
	} else {
		sat = diff/max;
	}

	// 色相算出(彩度0の場合でも算出)
	if( diff != 0.0){
		if( fr == max){
			hue = ((fg - fb)/ diff);
		} else if( fg == max) {
			hue = 2.0 + ((fb - fr) / diff);
		} else {
			hue = 4.0 + ((fr - fg) / diff);
		}
	} else {
		hue = 0.0;
	}
	hue /= 6.0;
	if( hue < 0.0){
		hue += 1.0;
	}

	hsb->h = (int)(hue * 360.0);
	hsb->s = (int)(sat * 255.0);
	hsb->b = (int)(bri * 255.0);
	hsb->t = 0;
}
#endif

#undef HUE_JN_FILTER
#define HUE_FS_FILTER
#undef SAT_JN_FILTER
#define SAT_FS_FILTER
#undef BRI_JN_FILTER
#define BRI_FS_FILTER

void
make_hsb_image(Image *img)
{
    int i, j;
    u_char *p;
    u_short *q;
    u_char a00, a10, a01, a11;
    u_char r, g, b;

    if (img->hsb_data) {
	free(img->hsb_data);
    }
    img->hsb_data =
	(u_short *)calloc(img->rwidth * img->rheight / 4,
			   sizeof(u_short));

    q = img->hsb_data;

    if( img->rheight != 480 || img->rwidth != 640){
        for (i = 0; i < img->rheight / 2; i++) {
            p = img->data + img->rwidth * i * 2;
            for (j = 0; j < img->rwidth / 2; j++, p += 2, q++) {
                a00 = *p;
                a10 = *(p + 1);
                a01 = *(p + img->rwidth);
                a11 = *(p + img->rwidth + 1);
                if ((a00 == img->transparent && a10 == img->transparent) ||
                    (a01 == img->transparent && a11 == img->transparent) ||
                    (a00 == img->transparent && a01 == img->transparent) ||
                    (a10 == img->transparent && a11 == img->transparent) ||
                    (a00 == img->transparent && a11 == img->transparent) ||
                    (a10 == img->transparent && a01 == img->transparent)) {
                    *q = 0xffff;
                    continue;
                }
                /*
                 * shift 6 bits:
                 *   -get average of 4 pixels (>> 2)
                 *   -convert 24bpp to 12bpp (>> 4)
                 */
                r = (img->palette[a00][0] +
                     img->palette[a10][0] +
                     img->palette[a01][0] + img->palette[a11][0]) >> 6;

                g = (img->palette[a00][1] +
                     img->palette[a10][1] +
                     img->palette[a01][1] + img->palette[a11][1]) >> 6;

                b = (img->palette[a00][2] +
                     img->palette[a10][2] +
                     img->palette[a01][2] + img->palette[a11][2]) >> 6;
                *q = rgb_to_hsb[packRGB(r, g, b)];
            }
        }
    } else {
        HSB* hsb = ( HSB*)malloc( sizeof( HSB) * 324 * 242);
        HSB* tmp;
        short x;

        /*
         * Phase 1, convertion  RGB to HSB
         */
        for (i = 0, tmp = hsb + 2; i < img->rheight / 2; tmp += 324, i++) {
            p = img->data + img->rwidth * i * 2;
            for (j = 0; j < img->rwidth / 2; j++, p += 2) {
                a00 = *p;
                a10 = *(p + 1);
                a01 = *(p + img->rwidth);
                a11 = *(p + img->rwidth + 1);
                if ((a00 == img->transparent && a10 == img->transparent) ||
                    (a01 == img->transparent && a11 == img->transparent) ||
                    (a00 == img->transparent && a01 == img->transparent) ||
                    (a10 == img->transparent && a11 == img->transparent) ||
                    (a00 == img->transparent && a11 == img->transparent) ||
                    (a10 == img->transparent && a01 == img->transparent)) {
                    tmp[ j].t = !0;
                    continue;
                }
                /*
                 * shift 6 bits:
                 *   -get average of 4 pixels (>> 2)
                 *   -convert 24bpp to 12bpp (>> 4)
                 */
                r = (img->palette[a00][0] +
                     img->palette[a10][0] +
                     img->palette[a01][0] + img->palette[a11][0]) >> 2;

                g = (img->palette[a00][1] +
                     img->palette[a10][1] +
                     img->palette[a01][1] + img->palette[a11][1]) >> 2;

                b = (img->palette[a00][2] +
                     img->palette[a10][2] +
                     img->palette[a01][2] + img->palette[a11][2]) >> 2;

                RGBtoHSB( tmp + j, r, g, b);
            }
        }

        /*
         * Phase 2, make dithered HSB data.
         */
        for (i = 0, tmp = hsb + 2; i < img->rheight / 2; tmp += 324, i++) {
            for (j = 0; j < img->rwidth / 2; j++, p += 2) {
                int hdiff, sdiff, bdiff;
                HSB *c = (tmp + j);

                static int thold[ 16] = {
                      -1,  16,  32,  48,  64,  80,  96, 112,
                     128, 144, 160, 176, 192, 208, 224, 240
                };

                if( c->t){
                    continue;
                }

                // Hueの変換
                if( c->h < 0){
                    c->h += 360;
                }
                if( c->h >= 360){
                    c->h -= 360;
                }
                if( 0 <= c->h && 15 >= c->h){
                    hdiff = 0 - c->h;
                    c->h = 0;
                } else if( 15 < c->h && 45 >= c->h){
                    hdiff = 30 - c->h;
                    c->h = 1;
                } else if( 45 < c->h && 75 >= c->h){
                    hdiff = 60 - c->h;
                    c->h = 2;
                } else if( 75 < c->h && 105 >= c->h){
                    hdiff = 90 - c->h;
                    c->h = 3;
                } else if( 105 < c->h && 135 >= c->h){
                    hdiff = 120 - c->h;
                    c->h = 4;
                } else if( 135 < c->h && 165 >= c->h){
                    hdiff = 150 - c->h;
                    c->h = 5;
                } else if( 165 < c->h && 195 >= c->h){
                    hdiff = 180 - c->h;
                    c->h = 6;
                } else if( 195 < c->h && 225 >= c->h){
                    hdiff = 210 - c->h;
                    c->h = 7;
                } else if( 225 < c->h && 255 >= c->h){
                    hdiff = 240 - c->h;
                    c->h = 8;
                } else if( 255 < c->h && 285 >= c->h){
                    hdiff = 270 - c->h;
                    c->h = 9;
                } else if( 285 < c->h && 315 >= c->h){
                    hdiff = 300 - c->h;
                    c->h = 10;
                } else if( 315 < c->h && 345 >= c->h){
                    hdiff = 330 - c->h;
                    c->h = 11;
                } else if( 345 < c->h && 360 >= c->h){
                    hdiff = 360 - c->h;
                    c->h = 0;
                }

                // 色相のディザは特にヤスリ状の模様が目立つので
                // 誤差量を意図的に減らし、模様の現出度合の低下を
                // 狙う(大当たり)
                hdiff /= 2;

		// 彩度の変換
                if( c->s < 0){
                   c->s = 0;
                }

                if( c->s > 255){
                   c->s = 255;
                }
                for( x = 15; x >= 0; x--){
                    if(  c->s > thold[ x]){
                        sdiff = thold[ x] + 16 - c->s;
                        c->s = x;
                        break;
                    }
                }

		// 明度の変換
                if( c->b < 0){
                    c->b = 0;
                }

                if( c->b > 255){
                    c->b = 255;
                }
                for( x = 15; x >= 0; x--){
                    if(  c->b > thold[ x]){
                        bdiff = thold[ x] + 16 - c->b;
                        c->b = x;
                        break;
                    }
                }

		// 周辺画素への各要素の誤差の拡散
#ifdef HUE_JN_FILTER
                (c + 1)->h -= (( hdiff * 7) / 48);
                (c + 2)->h -= (( hdiff * 5) / 48);
#endif /* HUE_JN_FILTER */
#ifdef HUE_FS_FILTER
                (c + 1)->h -= (( hdiff * 7) / 16);
#endif /* HUE_FS_FILTER */

#ifdef SAT_JN_FILTER
                (c + 1)->s -= (( sdiff * 7) / 48);
                (c + 2)->s -= (( sdiff * 5) / 48);
#endif /* SAT_JN_FILTER */
#ifdef SAT_FS_FILTER
                (c + 1)->s -= (( sdiff * 7) / 16);
#endif /* SAT_FS_FILTER */

#ifdef BRI_JN_FILTER
                (c + 1)->b -= (( bdiff * 7) / 48);
                (c + 2)->b -= (( bdiff * 5) / 48);
#endif /* BRI_JN_FILTER */
#ifdef BRI_FS_FILTER
                (c + 1)->b -= (( bdiff * 7) / 16);
#endif /* BRI_FS_FILTER */

                c += (320 + 4);
#ifdef HUE_JN_FILTER
                (c - 2)->h -= (( hdiff * 3) / 48);
                (c - 1)->h -= (( hdiff * 5) / 48);
                (c + 0)->h -= (( hdiff * 7) / 48);
                (c + 1)->h -= (( hdiff * 5) / 48);
                (c + 2)->h -= (( hdiff * 3) / 48);
#endif /* HUE_JN_FILTER */
#ifdef HUE_FS_FILTER
                (c - 1)->h -= (( hdiff * 3) / 16);
                (c + 0)->h -= (( hdiff * 5) / 16);
                (c + 1)->h -= (( hdiff * 1) / 16);
#endif /* HUE_FS_FILTER */

#ifdef SAT_JN_FILTER
                (c - 2)->s -= (( sdiff * 3) / 48);
                (c - 1)->s -= (( sdiff * 5) / 48);
                (c + 0)->s -= (( sdiff * 7) / 48);
                (c + 1)->s -= (( sdiff * 5) / 48);
                (c + 2)->s -= (( sdiff * 3) / 48);
#endif /* SAT_JN_FILTER */
#ifdef SAT_FS_FILTER
                (c - 1)->s -= (( sdiff * 3) / 16);
                (c + 0)->s -= (( sdiff * 5) / 16);
                (c + 1)->s -= (( sdiff * 1) / 16);
#endif /* SAT_FS_FILTER */

#ifdef BRI_JN_FILTER
                (c - 2)->b -= (( bdiff * 3) / 48);
                (c - 1)->b -= (( bdiff * 5) / 48);
                (c + 0)->b -= (( bdiff * 7) / 48);
                (c + 1)->b -= (( bdiff * 5) / 48);
                (c + 2)->b -= (( bdiff * 3) / 48);
#endif /* BRI_JN_FILTER */
#ifdef BRI_FS_FILTER
                (c - 1)->b -= (( bdiff * 3) / 16);
                (c + 0)->b -= (( bdiff * 5) / 16);
                (c + 1)->b -= (( bdiff * 1) / 16);
#endif /* BRI_FS_FILTER */

#if defined( HUE_JN_FILTER) || defined( SAT_JN_FILTER) || defined(BRI_JN_FILTER)
                c += (320 + 4);
#endif
#ifdef HUE_JN_FILTER
                (c - 2)->h -= (( hdiff * 1) / 48);
                (c - 1)->h -= (( hdiff * 3) / 48);
                (c + 0)->h -= (( hdiff * 5) / 48);
                (c + 1)->h -= (( hdiff * 3) / 48);
                (c + 2)->h -= (( hdiff * 1) / 48);
#endif /* HUE_JN_FILTER */

#ifdef SAT_JN_FILTER
                (c - 2)->s -= (( sdiff * 1) / 48);
                (c - 1)->s -= (( sdiff * 3) / 48);
                (c + 0)->s -= (( sdiff * 5) / 48);
                (c + 1)->s -= (( sdiff * 3) / 48);
                (c + 2)->s -= (( sdiff * 1) / 48);
#endif /* SAT_JN_FILTER */

#ifdef BRI_JN_FILTER
                (c - 2)->b -= (( bdiff * 1) / 48);
                (c - 1)->b -= (( bdiff * 3) / 48);
                (c + 0)->b -= (( bdiff * 5) / 48);
                (c + 1)->b -= (( bdiff * 3) / 48);
                (c + 2)->b -= (( bdiff * 1) / 48);
#endif /* BRI_JN_FILTER */
            }
        }

	    /*
         * Phase 3, packing MGL HSB format.
         */
        for (i = 0, tmp = hsb + 2; i < img->rheight / 2; tmp += 324, i++) {
            for (j = 0; j < img->rwidth / 2; j++, p += 2, q++) {
                if( tmp[j].t){
                    *q = 0xffff;
                } else {
                    *q = tmp[j].h << 8 | tmp[j].s << 4 | tmp[j].b;
                }
            }
        }
        free( hsb);
    }
}

u_char *
pack_img(u_short * data, int width, int height, int xoffset, int yoffset,
	 int *size)
{
    int i, j;
    u_char *dst, *p;

    *size = width * height * 2 + 8;
    p = dst = (u_char *) calloc(*size, sizeof(u_char));
    if (dst == NULL) {
	perror("pack_img");
	exit(1);
    }

    *p++ = (width >> 8) & 0xff;
    *p++ = width & 0xff;

    *p++ = (height >> 8) & 0xff;
    *p++ = height & 0xff;

    *p++ = (xoffset >> 8) & 0xff;
    *p++ = xoffset & 0xff;

    *p++ = (yoffset >> 8) & 0xff;
    *p++ = yoffset & 0xff;

    for (i = 0; i < height; i++) {
	for (j = 0; j < width; j++, data++) {
	    *p++ = (*data >> 8) & 0xff;
	    *p++ = *data & 0xff;
	}
    }

    return dst;
}
