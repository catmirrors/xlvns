/*
 * LEAF Visual Novel System For MGL2
 * (c) Copyright 2001 TF <tf@denpa.org>
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: mgLvns.c,v 1.6 2001/08/17 16:55:52 tf Exp $
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "Lvns.h"

static void read_convert_table(Lvns *);
static void open_leafpack(Lvns *);

void
mgLvnsInit(Lvns *lvns)
{

    srandom((unsigned)getpid());

    switch (lvns->version) {
    case 1:
	dprintf((stderr, "LVNS1: Shizuku\n"));
	SizukuInitialize(lvns);
	break;
    case 2:
	dprintf((stderr, "LVNS2: Kizuato\n"));
	KizuatoInitialize(lvns);
	break;
    case 3:
	dprintf((stderr, "LVNS3: To Heart\n"));
	ToHeartInitialize(lvns);
	break;
    default:
	fprintf(stderr, "Unknown version of LVNS: %d.\n", lvns->version);
	exit(1);
    }

    /*
     * オフスクリーン生成
     */
    lvns->background = mglimage_new(WIDTH, HEIGHT, 0, 0);
    lvns->vram = mglimage_new(WIDTH, HEIGHT, 0, 0);

    /*
     * set system depended functions
    */
    mgLvnsCoreSetLvns(lvns);
    mglimage_init(LVNS->width, LVNS->height);

    read_convert_table(lvns);
    open_leafpack(lvns);

#ifdef FONT_PLUS
    {
        char* buf;
        char fname[ 128];

        if( lvns->font_file == NULL){
            /* フォントファイルが指定されていない場合 */
		;;
	} else if( strcmp( lvns->font_file , "PAK FILE") == 0){
            /* PAKファイルから読み込む場合 */
            if( (buf = LvnsLoadData(lvns, "KNJ_ALL.KNJ", NULL)) != NULL){
                /* 読めた場合 */
                if( ( lvns->font = lvnsfont_new( buf)) == NULL){
                    perror( "Lvns fontload. Use system font.");
	        }
            } else {
                /* 読めなかった場合 */
                strcpy( fname, lvns->data_path);
                strcat( fname, "/");
                strcat( fname, "KNJ_ALL.KNJ");

                if( ( lvns->font = lvnsfont_load( fname)) == NULL){
                   perror( "Lvns fontload. Use system font.");
	        }
	    }
        } else {
            /* フォントファイルが指定されている場合 */
            if( ( lvns->font = lvnsfont_load( lvns->font_file)) == NULL){
                perror( "Lvns fontload. Use system font.");
            }
        }
    }
#endif FONT_PLUS
}

void
mgLvnsDelete(Lvns * lvns)
{
    leafpack_delete(lvns->leafpack);
    if (lvns->scnpack != lvns->leafpack) {
	leafpack_delete(lvns->scnpack);
    }

    mglimage_delete(lvns->background);
    mglimage_delete(lvns->vram);
}

static void
read_convert_table(Lvns *lvns)
{
    FILE           *fp;
    char            path[PATH_LEN];
    size_t          size;
    int             i;

    snprintf(path, sizeof(path), "%s/%s", lvns->data_path,
	     LVNS->fonttable_name);
    dprintf((stderr, "font table: %s\n", LVNS->fonttable_name));
    if ((fp = fopen(path, "r")) == NULL) {
	perror(path);
	exit(1);
    }
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    rewind(fp);

    /*
     * Read leaf->euc table 
     */
    lvns->leaf_to_euc = malloc(size);
    if (fread(lvns->leaf_to_euc, 1, size, fp) != size) {
	perror(LVNS->fonttable_name);
	free(lvns->leaf_to_euc);
	exit(1);
    }
    fclose(fp);

    /*
     * create jis->leaf table 
     */
    size /= 2;			/* # of chars. */
    lvns->jis_to_leaf = (u_short *)
	malloc(sizeof(u_short) * 94 * 94);

    for (i = 0; i < 94 * 94; i++) {
	lvns->jis_to_leaf[i] = 0;
    }

    for (i = 0; i < size; i++) {
	int             code =
	    ((lvns->leaf_to_euc[i * 2] & 0x7f) - 33) * 94 +
	    (lvns->leaf_to_euc[i * 2 + 1] & 0x7f) - 33;
	lvns->jis_to_leaf[code] = i;
    }
}

static void
open_leafpack(Lvns *lvns)
{
    char path[PATH_LEN];

    /*
     * open CG and sound package 
     */
    snprintf(path, sizeof(path), "%s/%s", lvns->data_path,
	     LVNS->leafpack_name);
    if ((lvns->leafpack = leafpack_new(path)) == NULL) {
	fprintf(stderr, "Can't open package %s.\n", LVNS->leafpack_name);
	exit(1);
    }

    /*
     * open scenario package 
     */
    if ((LVNS->scnpack_name)) {
	snprintf(path, sizeof(path), "%s/%s", lvns->data_path,
		 LVNS->scnpack_name);
	if ((lvns->scnpack = leafpack_new(path)) == NULL) {
	    fprintf(stderr, "Can't open package %s.\n", LVNS->scnpack_name);
	    exit(1);
	}
    } else {
	lvns->scnpack = lvns->leafpack;
    }
}
