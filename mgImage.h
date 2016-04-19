/*
 * LEAF Visual Novel System For MGL2
 * (c) Copyright 2001 TF <tf@denpa.org>
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: mgImage.h,v 1.4 2001/08/05 15:43:47 tf Exp $
 *
 */
#ifndef __INCLUDE_IMAGE_H
#define __INCLUDE_IMAGE_H

#include "mgl2.h"
#include "mglcol.h"

#define MGL_TRANSPARENT 0xffff

typedef struct {
  int width;
  int height;
  
  int xoffset;
  int yoffset;
  
  int rwidth;
  int rheight;
  
  u_char *data;

  u_short *hsb_data;
  u_short *hsb_pal_data;

  int hsb_palette[3][16];
  int use_palette;

  int use_multiple;
  int par16_multiple;
  int use_whiteout;
  int par16_whiteout;

  int is_put;

  int transparent;
  int black;
  
  u_char palette [256][3];
  int palette_num;
} mgImage;

/* mgImage.c */
void mglimage_init(int, int);
mgImage *mglimage_new(int, int, int, int);
void mglimage_delete(mgImage *);
void mglimage_add(mgImage *, mgImage *, int, int);
void mglimage_add2(mgImage *, mgImage *, int, int);
void mglimage_or(mgImage *, mgImage *, int, int);
void mglimage_copy_mask_unit_to_sximage(mgImage *, int, int, int, int, int);
void mglimage_copy_area_win2(mgImage *, int *, int, int, int, int, int, int);
void mglimage_copy_squaremask_unit_to_sximage(mgImage *, int, int, int, int, int);
void mglimage_clear_color(mgImage *, int);
void mglimage_draw_area(mgImage *, int *, int, int, int, int, int, int);
void mglimage_draw_string(mgImage *, int, int, char *, int);
void mglimage_set_palette(mgImage *, int[][16]);
mgImage *mglimage_load_lfg(u_char *, mgImage *);
void mglimage_set_multiple(mgImage *, int *, int);
void mglimage_set_pal_to_image(mgImage *, int *);
void mglimage_set_whiteout(mgImage *, int *, int);
void mglimage_put_pixstream_rect(int, int, int *, int, int, int);
void mglimage_box(mgImage *, int, int, int, int, int);

void lvnsimage_copy(mgImage *, mgImage *);
void lvnsimage_copy_area(mgImage *, mgImage *, int, int, int, int, int, int);
void lvnsimage_clear(mgImage *);
void lvnsimage_clear_transparent(mgImage *);
void lvnsimage_clear_area(mgImage *, int, int, int, int);
void lvnsimage_clear_area_transparent(mgImage *, int, int, int, int);
void lvnsimage_circle(mgImage *, int, int, int, int, int);
void lvnsimage_set_palette(mgImage *, u_char [][3], int);
void lvnsimage_boxfill(mgImage *, int, int, int, int, int);

#endif
