/*
 * LEAF Visual Novel System For MGL2
 * (c) Copyright 2001 TF <tf@denpa.org>
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: pakwriter.c,v 1.1 2001/07/25 14:36:49 tf Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pakconv.h"

static void free_pakwriter(PakWriter *);
static void write_header(PakWriter *);

#define PUTSHORT(d) putc(((d) >> 0) & 0xff, pw->fp); putc(((d) >> 8) & 0xff, pw->fp)
#define PUTKEYED(d) fputc(((d) + key[kidx]) & 0xff, pw->fp); kidx = (kidx + 1) % KEY_LEN

#define KEY_LEN 11

int key[KEY_LEN] = {
    0x71, 0x48, 0x6a, 0x55, 0x9f, 0x13, 0x58, 0xf7,
    0xd1, 0x7c, 0x3e
};

PakWriter *
pakwriter_new(const char *filename, int num)
{
    PakWriter *pw;
    char magic[] = "LEAFPACK";

    pw = (PakWriter *) malloc(sizeof(PakWriter));
    if (pw == NULL) {
	perror("pw");
	exit(1);
    }
    pw->fp = fopen(filename, "wb");
    if (pw->fp == NULL) {
	perror("pw->fp");
	exit(1);
    }
    pw->finfo = (FileInfo *) calloc(num, sizeof(FileInfo));
    if (pw->finfo == NULL) {
	perror("pw->finfo");
	exit(1);
    }

    pw->file_num = 0;
    pw->current_pos = 0;

    fwrite(magic, 1, 8, pw->fp);
    pw->current_pos += 8;

    PUTSHORT(num);
    pw->current_pos += 2;

    return pw;
}

void
pakwriter_delete(PakWriter *pw)
{
    write_header(pw);

    free_pakwriter(pw);
}

static void
free_pakwriter(PakWriter *pw)
{
    fclose(pw->fp);

    if (pw) {
	if (pw->finfo)
	    free(pw->finfo);
	free(pw);
    }
}

static void
write_header(PakWriter *pw)
{
    int i, j, k, kidx = 0;

    fprintf(stderr, "writing header...");

    for (i = 0; i < pw->file_num; i++) {
	j = 0;
	while (pw->finfo[i].name[j] != '.') {
	    PUTKEYED(pw->finfo[i].name[j]);
	    j++;
	}
	k = j;
	for (; j < 8; j++) {
	    PUTKEYED(0x20);
	}
	j = k + 1;		/*
				 * skip '.' 
				 */
	for (k = 0; k < 3; j++, k++) {
	    PUTKEYED(pw->finfo[i].name[j]);
	}
	PUTKEYED(0x0);

	PUTKEYED(pw->finfo[i].pos & 0xff);
	PUTKEYED((pw->finfo[i].pos >> 8) & 0xff);
	PUTKEYED((pw->finfo[i].pos >> 16) & 0xff);
	PUTKEYED((pw->finfo[i].pos >> 24) & 0xff);

	PUTKEYED(pw->finfo[i].len & 0xff);
	PUTKEYED((pw->finfo[i].len >> 8) & 0xff);
	PUTKEYED((pw->finfo[i].len >> 16) & 0xff);
	PUTKEYED((pw->finfo[i].len >> 24) & 0xff);

	PUTKEYED(pw->finfo[i].nextpos & 0xff);
	PUTKEYED((pw->finfo[i].nextpos >> 8) & 0xff);
	PUTKEYED((pw->finfo[i].nextpos >> 16) & 0xff);
	PUTKEYED((pw->finfo[i].nextpos >> 24) & 0xff);

#ifdef DEBUG
	fprintf(stderr, "%s: %06d\t%06d\t%06d\n",
		pw->finfo[i].name,
		pw->finfo[i].pos, pw->finfo[i].len, pw->finfo[i].nextpos);
    }
#endif
    fprintf(stderr, "done [%d files].\n", pw->file_num);
}

static void
add_fileinfo(PakWriter *pw, int len, const char *name)
{
    pw->finfo[pw->file_num].pos = pw->current_pos;
    pw->finfo[pw->file_num].nextpos = pw->current_pos + len;
    pw->finfo[pw->file_num].len = len;
    strncpy(pw->finfo[pw->file_num].name, name, 13);

    pw->file_num++;
    pw->current_pos += len;
}

void
put_file(PakWriter *pw, u_char * data, int size, const char *name)
{
    int i;
    int kidx = 0;

    for (i = 0; i < size; i++) {
	PUTKEYED(data[i]);
    }
    add_fileinfo(pw, size, name);
}

void
put_compressed_file(PakWriter *pw, u_char * data, int osize, int size,
		    const char *name)
{
    int i;
    int kidx = 0;

    PUTKEYED((size >> 24) & 0xff);
    PUTKEYED((size >> 16) & 0xff);
    PUTKEYED((size >> 8) & 0xff);
    PUTKEYED(size & 0xff);

    PUTKEYED((osize >> 24) & 0xff);
    PUTKEYED((osize >> 16) & 0xff);
    PUTKEYED((osize >> 8) & 0xff);
    PUTKEYED(osize & 0xff);

    for (i = 0; i < size; i++) {
	PUTKEYED(data[i]);
    }
    add_fileinfo(pw, size + 8, name);
}
