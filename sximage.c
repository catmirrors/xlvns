/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: sximage.c,v 1.24 2001/08/11 19:59:36 tf Exp $
 *
 */
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "sximage.h"

static struct {
    int id;
    TrueColorMap *tcmap;  
} tcmap[10];

static int tcmap_idx = 0;

SuperXImage *
super_ximage_new(Display * dpy, Window win, int width, int height, int maxcol, int noshared)
{
    int major, minor;
    SuperXImage *sximage;
    int    depth;
    Visual *visual;

    if ((sximage = malloc(sizeof *sximage)) == NULL) {
        perror("SuperXImage_new");
        exit(1);
    }

    sximage->display = dpy;
    sximage->window  = win;
    sximage->xoff = 0;
    sximage->yoff = 0;
    sximage->pixel_num = 0;

#ifdef MITSHM
    if (!noshared && (sximage->shared_ok = XShmQueryExtension(dpy))) {
	XShmQueryVersion(dpy, &major, &minor, &sximage->shared_pixmap_ok);
    } else {
	sximage->shared_ok = False;
        sximage->shared_pixmap_ok = False;
    }
#endif

    /* ウィンドウ関連属性情報の取得 */
    {
	XWindowAttributes attr;
	XGetWindowAttributes(dpy, win, &attr);
	depth = attr.depth;
        visual = attr.visual;
        
	sximage->colormap = attr.colormap;
	sximage->gc       = XCreateGC(dpy, win, 0, NULL);

	/* search visual infomation */
	{
	    int             matched;
	    XVisualInfo    *vinfolist;
            XVisualInfo    vinfo;

            vinfo.visualid = XVisualIDFromVisual(attr.visual);
	    vinfolist =
		XGetVisualInfo(dpy, VisualIDMask, &vinfo, &matched);
	    if (!matched) {
			fprintf(stderr, "can't get visual info.");
			exit(1);
	    }

#ifdef DEBUG 
	    /* Visual の情報を表示させる */
	    fprintf(stderr, "Visual ID: %02x\n", vinfolist[0].visualid);
	    {
		char *visual_class[] = { 
		    "StaticGray",
		    "GrayScale",
		    "StaticColor",
		    "PseudoColor",
		    "TrueColor",
		    "DirectColor"
		};
		fprintf(stderr, "VisualClass: %s\n", 
			visual_class[vinfolist[0].class]);
	    }
#endif
            sximage->visual_class = vinfolist[0].class;
	    XFree(vinfolist);

            /* 分離カラーマップの時は実際の色との擦り合わせを行う... */
            if (sximage->visual_class == TrueColor ||
                sximage->visual_class == DirectColor) {
                
                int i, j;
                /* fprintf(stderr, "TrueColor Mode\n"); */

                for (i=0;i<tcmap_idx;i++) {
                    if (vinfo.visualid == tcmap[i].id) {
                        sximage->tcmap = tcmap[i].tcmap;
                        goto end;
                    }
                }

                {
                    /* TrueColor 用のカラーマップを取得 */
                    TrueColorMap *map;
                    if ((map = malloc(sizeof *map * 17)) == NULL) {
                        perror("tcmap");
                        exit(1);
                    }
                    sximage->tcmap = tcmap[tcmap_idx++].tcmap = map;
                    /* RGB それぞれについてピクセル値をとる */
                    for (j=0;j<256;j++) {
                        int col;
                        XColor defs;
                        defs.flags = DoRed | DoGreen | DoBlue;
                        defs.red   = col = ((j << 8) | j);
                        defs.green = 0;
                        defs.blue  = 0;
                        XAllocColor(dpy, sximage->colormap, &defs);
                        map[0].red[j] = defs.pixel;
                        
                        defs.red   = 0;
                        defs.green = col;
                        defs.blue  = 0;
                        XAllocColor(dpy, sximage->colormap, &defs);
                        map[0].green[j] = defs.pixel;
                        
                        defs.red   = 0;
                        defs.green = 0;
                        defs.blue  = col;
                        XAllocColor(dpy, sximage->colormap, &defs);
                        map[0].blue[j] = defs.pixel;
                    }
                    XFreeColors(dpy, sximage->colormap, map[0].red, 256, 0);
                    XFreeColors(dpy, sximage->colormap, map[0].blue, 256, 0);
                    XFreeColors(dpy, sximage->colormap, map[0].green, 256, 0);
                }
            end:;
            } else if (sximage->visual_class == PseudoColor) {
                sximage->pixel_num = maxcol;
                if (!XAllocColorCells(dpy, sximage->colormap, False, 0, 0, 
                                    sximage->pixels, maxcol)) {
                    fprintf(stderr, "Can't Allocate Cells\n");
                    free(sximage);
                    return NULL;
                }
            } else {
                fprintf(stderr, "Not Supported visualclass\n");
                free(sximage);
                return NULL;
            }
        }
    }

#ifdef MITSHM
    if (sximage->shared_ok) {
        /* shared XImage */

        /* fprintf(stderr, "Shared Image\n");*/

        sximage->ximage = XShmCreateImage(dpy, visual, depth, ZPixmap, NULL,
                                          &sximage->shminfo, width, height);

        if ((sximage->shminfo.shmid = 
             shmget(IPC_PRIVATE,sximage->ximage->bytes_per_line * height,
                    IPC_CREAT | 0777)) < 0) {
            fprintf(stderr, "shmget: size:%x %s\n",
                    sximage->ximage->bytes_per_line * height,
                    strerror(errno));

            exit(1);
        }

        if ((sximage->shminfo.shmaddr = sximage->ximage->data = 
             shmat(sximage->shminfo.shmid, 0, 0)) == NULL) {
            perror("shmat");
            exit(1);
        }
        sximage->shminfo.readOnly = False;
        XShmAttach(dpy, &sximage->shminfo);

        /* shared pixmap */
        if (sximage->shared_pixmap_ok) {
            /* fprintf(stderr, "Shared Pixmap\n");*/

            sximage->pixmap = XShmCreatePixmap(dpy, win,
                                               sximage->ximage->data, 
                                               &sximage->shminfo,
                                               sximage->ximage->width,
                                               sximage->ximage->height, 
                                               sximage->ximage->depth);
        } else {
            sximage->pixmap = XCreatePixmap(dpy, win, 
                                            sximage->ximage->width,
                                            sximage->ximage->height,
                                            sximage->ximage->depth);
        }
    } else
#endif
    {
        /* normal ximage */
        sximage->ximage = XCreateImage(dpy, visual, depth, ZPixmap, 0, NULL,
                                       width, height, 8, 0);

        if((sximage->ximage->data = malloc(sximage->ximage->bytes_per_line *
                                           sximage->ximage->height)) == NULL) {
            perror("sximage/new");
            exit(1);
        }
        sximage->pixmap = XCreatePixmap(dpy, win, 
                                        sximage->ximage->width,
                                        sximage->ximage->height,
                                        sximage->ximage->depth);
    }
#if 0
    fprintf(stderr, "sximage_new: %x %p\n", 
            sximage->shminfo.shmid,
	    sximage->shminfo.shmaddr);
#endif
    return sximage;
}

void
super_ximage_set_offset(SuperXImage *sximage, int xoff, int yoff)
{
    sximage->xoff = xoff;
    sximage->yoff = yoff;
}

void
super_ximage_set_pixmap(SuperXImage *sximage)
{
#ifdef MITSHM
    if (sximage->shared_ok) {
        if (!sximage->shared_pixmap_ok) {	/* not shared pixmap ... */
            XShmPutImage(sximage->display, sximage->pixmap, sximage->gc, 
                         sximage->ximage, 0, 0, 0, 0, 
                         sximage->ximage->width, 
                         sximage->ximage->height, False);
        } 
        /* else do nothing ... */
    } else 
#endif
    {
        XPutImage(sximage->display, sximage->pixmap, sximage->gc, 
                  sximage->ximage, 0, 0, 0, 0, 
                  sximage->ximage->width,
                  sximage->ximage->height);
    }
}

void
super_ximage_set_pixmap_area(SuperXImage *sximage, int x, int y, int w, int h)
{
#ifdef MITSHM
    if (sximage->shared_ok) {
        if (!sximage->shared_pixmap_ok) {	/* not shared pixmap ... */
            XShmPutImage(sximage->display, sximage->pixmap, sximage->gc, 
                         sximage->ximage, x, y, x, y, w, h, False);
        } 
        /* else do nothing ... */
    } else 
#endif
    {
        XPutImage(sximage->display, sximage->pixmap, sximage->gc, 
                  sximage->ximage, x, y, x, y, w, h);
    }
}

void
super_ximage_put_pixmap(SuperXImage *sximage, Pixmap pixmap)
{
#ifdef MITSHM
    if (sximage->shared_ok) {
        XShmPutImage(sximage->display, pixmap, sximage->gc, 
                     sximage->ximage, 0, 0, 0, 0, 
                     sximage->ximage->width, 
                     sximage->ximage->height, False);
    } else 
#endif
    {
        XPutImage(sximage->display, pixmap, sximage->gc, 
                  sximage->ximage, 0, 0, 0, 0, 
                  sximage->ximage->width,
                  sximage->ximage->height);
    }
}

void
super_ximage_delete(SuperXImage * sximage)
{
#if 0
    fprintf(stderr, "ximage_delete: %x %p\n", 
            sximage->shminfo.shmid,
            sximage->shminfo.shmaddr);
#endif
	
	XFreeGC(sximage->display, sximage->gc);

    /* free color cells */
    if (sximage->pixel_num > 0) {
        XFreeColors(sximage->display, sximage->colormap,
                    sximage->pixels, sximage->pixel_num, 0);
        sximage->pixel_num = 0;
    }

    /* free pixmap */
    XFreePixmap(sximage->display, sximage->pixmap);

#ifdef MITSHM
    if (sximage->shared_ok) {
        /* detach shared memory */
        XShmDetach(sximage->display, &sximage->shminfo);
        sximage->ximage->data = NULL;
        /* delete shared memory */
        shmdt(sximage->shminfo.shmaddr);
        shmctl(sximage->shminfo.shmid, IPC_RMID, NULL);
    }
#endif

    /* destroy image */
    XDestroyImage(sximage->ximage);

    free(sximage);
}

void
super_ximage_clear(SuperXImage *sximage, long pixel)
{
    super_ximage_clear_area(sximage, 0, 0, 
                            sximage->ximage->width,
                            sximage->ximage->height,
                            pixel);
}

/*
 * 24×24 のビットマップパターンを pixel の色で描画する
 */ 
void
super_ximage_put_pattern24(SuperXImage *sximage, int x, int y, long pixel, char *data)
{
    /* oh! slow version ... */
    int i, j, x2;
    for (i=0; i<24; i++, y++) {
        x2 = x;
        for (j=0; j<3; j++) {
            u_char c = *data++;
            if ((c & 0x80))
                XPutPixel(sximage->ximage, x2, y, pixel);
            x2++;
            if ((c & 0x40))
                XPutPixel(sximage->ximage, x2, y, pixel);
            x2++;
            if ((c & 0x20))
                XPutPixel(sximage->ximage, x2, y, pixel);
            x2++;
            if ((c & 0x10))
                XPutPixel(sximage->ximage, x2, y, pixel);
            x2++;
            if ((c & 0x08))
                XPutPixel(sximage->ximage, x2, y, pixel);
            x2++;
            if ((c & 0x04))
                XPutPixel(sximage->ximage, x2, y, pixel);
            x2++;
            if ((c & 0x02))
                XPutPixel(sximage->ximage, x2, y, pixel);
            x2++;
            if ((c & 0x01))
                XPutPixel(sximage->ximage, x2, y, pixel);
            x2++;
        }
    }
}

/*
 * 24×24 のビットマップパターンを pixel の色で描画する
 */ 
void
super_ximage_put_pattern24_2(SuperXImage *sximage, int x, int y, long pixel, char *data)
{
    /* oh! slow version ... */
    int i, j, x2, y2;
    for (j=0; j<3; j++, x+=8) {
        y2 = y;
        for (i=0; i<24; i++, y2++) {
            u_char c = *data++;
            x2 = x;
            if ((c & 0x80))
                XPutPixel(sximage->ximage, x2, y2, pixel);
            x2++;
            if ((c & 0x40))
                XPutPixel(sximage->ximage, x2, y2, pixel);
            x2++;
            if ((c & 0x20))
                XPutPixel(sximage->ximage, x2, y2, pixel);
            x2++;
            if ((c & 0x10))
                XPutPixel(sximage->ximage, x2, y2, pixel);
            x2++;
            if ((c & 0x08))
                XPutPixel(sximage->ximage, x2, y2, pixel);
            x2++;
            if ((c & 0x04))
                XPutPixel(sximage->ximage, x2, y2, pixel);
            x2++;
            if ((c & 0x02))
                XPutPixel(sximage->ximage, x2, y2, pixel);
            x2++;
            if ((c & 0x01))
                XPutPixel(sximage->ximage, x2, y2, pixel);
            x2++;
        }
    }
}

#define STORE16(p,d)  (p[0]= (d) & 0xff, p[1]= ((d) >> 8) & 0xff)
#define STORE24(p,d)  (p[0]= (d) & 0xff, p[1]= ((d) >> 8) & 0xff, p[2]= ((d) >> 16) & 0xff)
#define STORE32(p,d)  (p[0]= (d) & 0xff, p[1]= ((d) >> 8) & 0xff, p[2]= ((d) >> 16) & 0xff, p[3]= ((d) >> 24) & 0xf)

void
super_ximage_copy_area(SuperXImage *src, SuperXImage *dst, int x, int y,
                       int w, int h, int x2, int y2) {
    int i, j;
    if (src->ximage->bits_per_pixel == 32) {
        /* start position */
        u_char *p = src->ximage->data +
            y * src->ximage->bytes_per_line + x * 4;
        u_char *q = dst->ximage->data +
            y2 * dst->ximage->bytes_per_line + x2 * 4;
        for (i=0;i<h;i++) {
            memcpy(q, p, w * 4);
            p += src->ximage->bytes_per_line;
            q += dst->ximage->bytes_per_line;
        }   
    } else if (src->ximage->bits_per_pixel == 24) {
        /* start position */
        u_char *p = src->ximage->data +
            y * src->ximage->bytes_per_line + x * 3;
        u_char *q = dst->ximage->data +
            y2 * dst->ximage->bytes_per_line + x2 * 3;
        for (i=0;i<h;i++) {
            memcpy(q, p, w * 3);
            p += src->ximage->bytes_per_line;
            q += dst->ximage->bytes_per_line;
        }   
    } else if (src->ximage->bits_per_pixel == 16) {
        /* start position */
        u_char *p = src->ximage->data +
            y * src->ximage->bytes_per_line + x * 2;
        u_char *q = dst->ximage->data +
            y2 * dst->ximage->bytes_per_line + x2 * 2;
        for (i=0;i<h;i++) {
            memcpy(q, p, w * 2);
            p += src->ximage->bytes_per_line;
            q += dst->ximage->bytes_per_line;
        }   
    } else if (src->ximage->bits_per_pixel == 8) {
        /* start position */
        u_char *p = src->ximage->data +
            y * src->ximage->bytes_per_line + x;
        u_char *q = dst->ximage->data +
            y2 * dst->ximage->bytes_per_line + x2;
        for (i=0;i<h;i++) {
            memcpy(q, p, w);
            p += src->ximage->bytes_per_line;
            q += dst->ximage->bytes_per_line;
        }   
    } else {
        for (i=0;i<h;i++) {
            for (j=0;j<w;j++) {
                XPutPixel(dst->ximage, x2+j, y2+i, 
                          XGetPixel(src->ximage, x+j, y+i));
            }       
        }                   
    }
}

void
super_ximage_clear_area(SuperXImage *sximage, 
                        int x, int y, int w, int h, long pixel)
{
    int i, j;
    if (x < 0) {
      w += x;
      x = 0;
    }
    if (y < 0) {
      h += y;
      y = 0;
    }
    if (w < 0) {
      w = 0;
    }
    if (h < 0) {
      h = 0;
    }
    if (sximage->ximage->bits_per_pixel == 24) {
        u_char *q = sximage->ximage->data +
            y * sximage->ximage->bytes_per_line + x * 3;
        if (sximage->ximage->byte_order     == LSBFirst) {
            for (i=0; i<h; i++) {
                u_char *s = q;
                for (j=0; j<w; j++, s+=3) {
                    STORE24_LSB(s, pixel);
                }
                q += sximage->ximage->bytes_per_line;
            }
        } else {
            for (i=0; i<h; i++) {
                u_char *s = q;
                for (j=0; j<w; j++, s+=3) {
                    STORE24_HSB(s, pixel);
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
        for (i=0; i<h; i++) {
            u_char *s = q;
            for (j=0; j<w; j++, s+=2) {
                *(uint16_t*)s  = pixel;
                /* STORE16_LSB(s, pixel); */
            }
            q += sximage->ximage->bytes_per_line;
        }
    } else if (/*sximage->ximage->byte_order     == LSBFirst &&*/
               sximage->ximage->bits_per_pixel == 32) {
        /* start position */
        u_char *q = sximage->ximage->data +
            y * sximage->ximage->bytes_per_line + x * 4;
        for (i=0; i<h; i++) {
            u_char *s = q;
            for (j=0; j<w; j++, s+=4) {
                *(uint32_t*)s  = pixel;
                /* STORE32_LSB(s, pixel); */
            }
            q += sximage->ximage->bytes_per_line;
        }
    } else if (sximage->ximage->bits_per_pixel == 8) {
        u_char *q = sximage->ximage->data +
            y * sximage->ximage->bytes_per_line + x;
        for (i=0; i<h; i++) {
            u_char *s = q;
            for (j=0; j<w; j++, s++) {
                *s  = pixel;
            }
            q += sximage->ximage->bytes_per_line;
        }
    } else {
        for (i=0;i<h;i++) {
            for (j=0;j<w;j++) {
                XPutPixel(sximage->ximage, x+j, y+i, pixel);
            }
        }
    }
}

void
super_ximage_copy_area_win(SuperXImage *sximage, 
                           int x, int y, int w, int h) {
    XCopyArea(sximage->display, sximage->pixmap,
              sximage->window,
              sximage->gc, x, y, w, h, 
              sximage->xoff + x, 
              sximage->yoff + y);
}

void
super_ximage_copy_area_win2(SuperXImage *sximage, 
                           int x, int y, int w, int h, int x2, int y2) {
    XCopyArea(sximage->display, sximage->pixmap,
              sximage->window,
              sximage->gc, x, y, w, h, 
              sximage->xoff + x2, 
              sximage->yoff + y2);
}

void
super_ximage_copy_area_win_region(SuperXImage *sximage, Region region)
{
	if (region != None) {
		XSetRegion(sximage->display, sximage->gc, region);
	}
    XCopyArea(sximage->display, sximage->pixmap, sximage->window,
              sximage->gc, 0, 0, 
              sximage->ximage->width,
              sximage->ximage->height,
              sximage->xoff, 
              sximage->yoff);
	if (region != None) {
		XSetClipMask(sximage->display, sximage->gc, None);
	}
}

void
super_ximage_clear_area_win(SuperXImage *sximage, int x, int y, 
			    int w, int h, long pixel)
{
    XSetForeground(sximage->display, sximage->gc, pixel);
    XFillRectangle(sximage->display, sximage->window, sximage->gc,
		   sximage->xoff + x, sximage->yoff + y, w, h);
}

void
super_ximage_sync(SuperXImage *sximage)
{
    XSync(sximage->display, False);
}

void
super_ximage_set_palette(SuperXImage *sximage, int index, int r, int g, int b)
{
    switch (sximage->visual_class) {
    case PseudoColor:
	{
	    XColor defs;
	    defs.red   = r << 8 | r;
	    defs.green = g << 8 | g;
	    defs.blue  = b << 8 | b;
	    defs.pixel = sximage->pixels[index];
	    defs.flags = DoRed | DoGreen | DoBlue;
	    XStoreColor(sximage->display, sximage->colormap, &defs);
	}
	break;
    case TrueColor:
    case DirectColor:
        sximage->pixels[index] =  SXIMAGE_TPIXEL(sximage, r, g, b);
    default:;
    }
}
