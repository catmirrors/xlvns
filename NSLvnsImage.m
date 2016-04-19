#import "NSLvnsImage.h"

@implementation NSLvnsImage

#define STORE24(p,d)  memcpy(p,d,3)

static int cmap_m[17][256]; /* Multiple計算用テーブル(黒とのαブレンド) */
static int cmap_s[17][256]; /* Screen  計算用テーブル(白とのαブレンド) */
static int cmap_a[17][512]; /* αブレンド計算用テーブル                 */

/**
 * 初期化処理
 */
+ (void) Init
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

- (id) init : (NSSize) size
{
	width  = size.width;
	height = size.height;
	bpr    = width * 3;
	data = malloc(sizeof(*data) * bpr * height);
	pixel_num = 0;
	return self;
}

- (void) clear : (int) color
{
	[self clearArea : 0 : 0 : width : height : color];
}

- (void) clearArea : (int)x : (int)y : (int)w : (int)h: (int) color
{
    int i, j;
	u_char *pixel = pixels[color];

	u_char *q = data + y * bpr + x * 3;
	for (i=0; i<h; i++) {
		u_char *s = q;
		for (j=0; j<w; j++, s+=3) {
			STORE24(s, pixel);
		}
		q += bpr;
	}
}

- (void) PutPixel : (int) x : (int) y : (u_char *) pixel
{
	u_char *q = data + y * bpr + x * 3;
	STORE24(q, pixel);
}

/*
 * 24×24 のビットマップパターンを pixel の色で描画する
 */ 
- (void) putPattern24 : (int) x : (int) y : (int) color : (u_char *) pat
{
	u_char *pixel   = pixels[color];

    int i, j, x2;
    for (i=0; i<24; i++, y++) {
        x2 = x;
        for (j=0; j<3; j++) {
            u_char c = *pat++;
            if ((c & 0x80))
                [self PutPixel : x2 : y : pixel];
            x2++;
            if ((c & 0x40))
                [self PutPixel : x2 : y : pixel];
            x2++;
            if ((c & 0x20))
                [self PutPixel : x2 : y : pixel];
            x2++;
            if ((c & 0x10))
                [self PutPixel : x2 : y : pixel];
            x2++;
            if ((c & 0x08))
                [self PutPixel : x2 : y : pixel];
            x2++;
            if ((c & 0x04))
                [self PutPixel : x2 : y : pixel];
            x2++;
            if ((c & 0x02))
                [self PutPixel : x2 : y : pixel];
            x2++;
            if ((c & 0x01))
                [self PutPixel : x2 : y : pixel];
            x2++;
        }
    }
}

/*
 * 24×24 のビットマップパターンを pixel の色で描画する
 * ToHeart/Kizuato
 */ 
- (void) putPattern24_2 : (int) x : (int) y : (int) color : (u_char *) pat
{
	u_char *pixel   = pixels[color];

    int i, j, x2, y2;
    for (j=0; j<3; j++, x+=8) {
        y2 = y;
        for (i=0; i<24; i++, y2++) {
            u_char c = *pat++;
            x2 = x;
            if ((c & 0x80))
                [self PutPixel : x2 : y2 : pixel];
            x2++;
            if ((c & 0x40))
                [self PutPixel : x2 : y2 : pixel];
            x2++;
            if ((c & 0x20))
                [self PutPixel : x2 : y2 : pixel];
            x2++;
            if ((c & 0x10))
                [self PutPixel : x2 : y2 : pixel];
            x2++;
            if ((c & 0x08))
                [self PutPixel : x2 : y2 : pixel];
            x2++;
            if ((c & 0x04))
                [self PutPixel : x2 : y2 : pixel];
            x2++;
            if ((c & 0x02))
                [self PutPixel : x2 : y2 : pixel];
            x2++;
            if ((c & 0x01))
                [self PutPixel : x2 : y2 : pixel];
            x2++;
        }
    }
}

- (void) setPalette : (int) index r:(int) r  g:(int) g  b:(int) b
{
	pixels[index][0] = r;
	pixels[index][1] = g;
	pixels[index][2] = b;
}

- (void) setPal : (LvnsImage *) image
{
    int i;
    for (i=0;i<image->palette_num;i++) {
		[self setPalette : i
			  r: image->palette[i][0]
			  g: image->palette[i][1]
			  b: image->palette[i][2]];
    }       
}

- (void) setPalMulti : (LvnsImage *) image : (int)par16
{
    int i;

    if (par16 >= 16) {
        [self setPal : image];
        return;
    }

    for (i=0;i<image->palette_num;i++) {
		[self setPalette : i
			  r: cmap_m[par16][image->palette[i][0]]
			  g: cmap_m[par16][image->palette[i][1]]
			  b: cmap_m[par16][image->palette[i][2]]];
    }
}

- (void) setPalScreen : (LvnsImage *) image : (int)par16
{
    int i;
    for (i=0;i<image->palette_num;i++) {
		[self setPalette : i
			  r: cmap_s[par16][image->palette[i][0]]
			  g: cmap_s[par16][image->palette[i][1]]
			  b: cmap_s[par16][image->palette[i][2]]];
    }
}

- (void) copy : (LvnsImage *) image : (int) x : (int) y
{
    int i,j;
    u_char *p = image->data, *q;

    x += image->xoffset;
    y += image->yoffset;

	/* start position */
	q = data + y * bpr + x * 3;
	for (i=0; i<image->rheight; i++) {
		u_char *s = q;
		for (j=0; j<image->rwidth; j++, p++, s+=3) {
			STORE24(s, pixels[*p]);
		}
		q += bpr;
	}
}

- (void) copyArea : (LvnsImage *) src : (int) x : (int) y 
				  : (int) w : (int) h : (int) x2 : (int) y2
{
    int i,j;
	u_char *p, *q;

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

    if (x + w > src->rwidth)  w = src->rwidth - x;
    if (y + h > src->rheight) h = src->rheight - y;
    if (x2 + w > width)  w = width - x2;
    if (y2 + h > height) h = height - y2;

	/* start position */
	p = src->data + y * src->rwidth + x;
	q = data + y2 * bpr + x2 * 3;
	for (i=0; i<h; i++) {
		u_char *r = p;
		u_char *s = q;
		for (j=0; j<w; j++, r++, s+=3) {
			STORE24(s, pixels[*r]);
		}
		p += src->rwidth;
		q += bpr;
	}
}

- (void) copyMaskUnit : (LvnsImage *) src : (int) x : (int) y
					  : (int) x2 : (int) y2 : (int) mask
{
    int i, j;
    static int xoff[] = { 3, 1, 1, 3,  2, 0, 0, 2,  3, 1, 1, 3, 2, 0, 0, 2 };
    static int yoff[] = { 0, 2, 0, 2,  1, 3, 1, 3,  1, 3, 1, 3, 0, 2, 0, 2 };
	u_char *p, *q;

    y += yoff[mask];
    x += xoff[mask];
	y2 += yoff[mask];
	x2 += xoff[mask];

	q = data + y2 * bpr + x2 * 3;
	p = src->data + y * src->rwidth + x;
	for (i=0; i<16; i+=4) {
		u_char *s = q;
		u_char *r = p;
		for (j=0; j<16; j+=4, s+=12, r+=4) {
			STORE24(s, pixels[*r]);
		}
		q += bpr * 4;
		p += src->rwidth * 4;
	}
}

- (void) copySquareMaskUnit : (LvnsImage *) src : (int) x : (int) y
							: (int) x2 : (int) y2 : (int) mask
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
        [self PutPixel : x2 + x0    : y2 + y0 : pixels[p[x0]]];
        [self PutPixel : x2 + 31-x0 : y2 + y0 : pixels[p[31-x0]]];
        if (y + 31 - y0 < height) {
            u_char *p2 = src->data + (y + 31 -y0) * src->rwidth + x;
            [self PutPixel : x2 + x0    : y2 + 31-y0 : pixels[p2[x0]]];
            [self PutPixel : x2 + 31-x0 : y2 + 31-y0 : pixels[p2[31-x0]]];
        }
        x0--;
        y0++;
    }
}

- (void) draw : (NSRect) bounds
{
	unsigned char *dat2 = data + 
		(int)((height - (bounds.origin.y + bounds.size.height)) * bpr + bounds.origin.x);
	NSDrawBitmap(bounds, bounds.size.width, bounds.size.height, 
				 8, 3, 24, bpr, false, false, 
				 NSCalibratedRGBColorSpace, &dat2);
}

@end


