/*
 * LEAF Visual Novel System For MGL2
 * (c) Copyright 2001 TF <tf@denpa.org>
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: pakconv.h,v 1.4 2001/08/05 15:42:37 tf Exp $
 */
#ifndef __INCLUDE_CONV_PAK
#define __INCLUDE_CONV_PAK

#include "leafpack.h"

#define PATH_LEN 256

#include <sys/types.h>

typedef struct {
    int width;
    int height;

    int xoffset;
    int yoffset;

    int rwidth;
    int rheight;

    u_char *data;

    u_short *hsb_data;

    int transparent;
    int black;

    u_char palette[256][3];
    int palette_num;
} Image;

typedef struct FileInfo {
    char name[13];
    off_t pos;
    off_t nextpos;
    size_t len;
} FileInfo;

typedef struct {
    int file_num;
    int current_pos;
    FileInfo *finfo;
    FILE *fp;
} PakWriter;

/*
 * image.c 
 */
Image *image_new(int, int, int, int);
void image_delete(Image *);
void lvnsimage_copy(Image *src, Image *dst);
void make_hsb_image(Image *);
u_char *pack_img(u_short *, int, int, int, int, int *);

/*
 * lfg.c 
 */
Image *lvnsimage_load_lfg(const u_char *, Image *);

/*
 * sdt.c 
 */
Image *lvnsimage_load_sdt(const u_char *, const char *);

/*
 * lf2.c 
 */
Image *lvnsimage_load_lf2(const u_char *, Image *);

/*
 * lzcompress.c 
 */
int lz77_compress(u_char *, int, u_char *);

/*
 * pakwriter.c 
 */
PakWriter *pakwriter_new(const char *, int);
void pakwriter_delete(PakWriter *);
void put_file(PakWriter *, u_char *, int, const char *);
void put_compressed_file(PakWriter *, u_char *, int, int, const char *);

#endif
