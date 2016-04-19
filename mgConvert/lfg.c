/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: lfg.c,v 1.2 2001/07/26 17:51:20 tf Exp $
 *
 */

/*
 * $Id: lfg.c,v 1.2 2001/07/26 17:51:20 tf Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pakconv.h"
#include "leafpack.h"

Image *
lvnsimage_load_lfg(const u_char *data, Image *over)
{
    Image *img;
    u_char *work;		/* 作業領域 */

    size_t size;
    int width, height, xoffset, yoffset;

    /*
     * check magic number 'LEAFCODE' 
     */
    if (strncmp(data, "LEAFCODE", 8)) {
	fprintf(stderr, "This file isn't LFG format.\n");
	return NULL;
    }

    /* geometry */
    xoffset = (data[33] << 8 | data[32]) * 8;
    yoffset = data[35] << 8 | data[34];
    width = ((data[37] << 8 | data[36]) + 1) * 8;
    height = ((data[39] << 8 | data[38]) + 1);

    if (over) {
	/* XXX no check.... */
	img = over;
	img->width = width;
	img->height = height;
	img->xoffset = xoffset;
	img->yoffset = yoffset;
	img->rwidth = width - xoffset;
	img->rheight = height - yoffset;
    } else {
	img = image_new(width, height, xoffset, yoffset);
    }

    img->palette_num = 16;
    /* read palette */
    {
	/*
	 * 4bit 単位 *   RG BR GB RG BR ... 
	 */

	int i;
	const u_char *p = data + 8;
	u_char *q = (u_char *) img->palette;	/* R G B R G B R G B ... */

	int upper, lower;

	for (i = 0; i < 24; i++) {
	    upper = *p & 0xf0;
	    upper |= upper >> 4;
	    lower = *p++ & 0xf;
	    lower |= lower << 4;
	    *q++ = upper;
	    *q++ = lower;
	}
    }

    /* transparent color */
    img->transparent = data[41];

    /* image size */
    size =
	data[44] | (data[45] << 8) | (data[46] << 16) | (data[47] << 24);
    if ((work = malloc(size * sizeof *work)) == NULL) {
	perror("lfg/extract_body:");
	exit(1);
    }

    /*
     * 展開 
     */
    leafpack_lzs(data + 48, work, size);


    /*
     * convert to indexed image 
     */
    {
	int i;
	u_char *p = work;
	u_char *q = img->data;

	/*
	 * direction flag 
	 */
	int direction = data[40];

	if (!direction) {	/*
				 * VERTICAL 
				 */
	    int x = 0, y = 0;
	    for (i = 0; i < size; i++, p++) {

		q[img->rwidth * y + x] = (*p & 0x80) >> 4 |
		    (*p & 0x20) >> 3 | (*p & 0x08) >> 2 | (*p & 0x02) >> 1;
		q[img->rwidth * y + x + 1] = (*p & 0x40) >> 3 |
		    (*p & 0x10) >> 2 | (*p & 0x04) >> 1 | (*p & 0x01);
		if (++y >= img->rheight) {
		    y = 0;
		    x += 2;
		}
	    }
	} else {		/*
				 * HORIZONTAL 
				 */
	    for (i = 0; i < size; i++, p++) {
		*q++ = (*p & 0x80) >> 4 |
		    (*p & 0x20) >> 3 | (*p & 0x08) >> 2 | (*p & 0x02) >> 1;
		*q++ = (*p & 0x40) >> 3 |
		    (*p & 0x10) >> 2 | (*p & 0x04) >> 1 | (*p & 0x01);
	    }
	}
    }

    free(work);

    return img;
}

void
lvnsimage_load_lfg_palette(Image *img, const u_char * data)
{
    /*
     * check magic number 'LEAFCODE' 
     */
    if (strncmp(data, "LEAFCODE", 8)) {
	fprintf(stderr, "This file isn't LFG format.\n");
	return;
    }

    img->palette_num = 16;
    /*
     * read palette 
     */
    {
	/*
	 * 4bit 単位 *   RG BR GB RG BR ... 
	 */

	int i;
	const u_char *p = data + 8;
	u_char *q = (u_char *) img->palette;	/*
						 * R G B R G B R G B
						 * ... 
						 */
	int upper, lower;

	for (i = 0; i < 24; i++) {
	    upper = *p & 0xf0;
	    upper |= upper >> 4;
	    lower = *p++ & 0xf;
	    lower |= lower << 4;
	    *q++ = upper;
	    *q++ = lower;
	}
    }

    /*
     * transparent color 
     */
    img->transparent = data[41];
}
