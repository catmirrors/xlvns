/*
 * LEAF Visual Novel System For MGL2 - Font Plus Extension
 * (c) Copyright 2001 Hiroshi Kuwagata <kgt@topaz.ocn.ne.jp>
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: mgFont.c,v 1.4 2001/08/12 11:35:44 tf Exp $
 *
 */
/*
 * vi:ts=4:sw=4;
 */
#ifdef FONT_PLUS
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "mgFont.h"

LvnsFont*
lvnsfont_load( const u_char* fname)
{
	FILE* fp;
	LvnsFont* ret;
	struct stat st;


	if( stat( fname, &st)){
		perror( "Lvns - font load (stat)\n");
		return NULL;
	}

	if( ( fp = fopen( (char*)fname, "rb")) == NULL){
		perror( "Lvns - font load (fopen)\n");
		return NULL;
	}

	if( (ret = malloc( sizeof( LvnsFont))) == NULL){
		perror( "Lvns - font load (malloc LvnsFont)\n");
		return NULL;
	}

	if( ( ret->draw_buf = malloc( sizeof( int) * 12 * 12)) == NULL){
		perror( "Lvns - font load (malloc draw_buf)\n");
		free( ret);
		return NULL;
		
	}

	if( ( ret->font_data = malloc( st.st_size)) == NULL){
		perror( "Lvns - font load (malloc draw_buf)\n");
		free( ret->draw_buf);
		free( ret);
		return NULL;
		
	}

	//ret->width  = width;
	//ret->height = height;

	// fontデータ読み込み
	fread( ret->font_data, st.st_size, 1, fp);

	return ret;
}

LvnsFont*
lvnsfont_new( const u_char* dat)
{
	LvnsFont* ret;

	if( (ret = malloc( sizeof( LvnsFont))) == NULL){
		perror( "Lvns - font load (malloc LvnsFont)\n");
		return NULL;
	}

	if( ( ret->draw_buf = malloc( sizeof( int) * 12 * 12)) == NULL){
		perror( "Lvns - font load (malloc draw_buf)\n");
		free( ret);
		return NULL;
		
	}

	ret->font_data = dat;

	return ret;
}

void lvnsfont_discard( LvnsFont* fnt)
{
	free( fnt->draw_buf);
	free( fnt->font_data);
	free( fnt);
}

static int default_cmap[ 4] = {
	COLOR_TRANSPARENT, 0x00000004, 0x0000000c, 0x0000000f
};

void lvnsfont_draw( LvnsFont* fnt, int x, int y, int code, int* cmap)
{
	int	i;
	u_char *fptn;

	if( cmap == NULL){
		cmap = default_cmap;
	}

	fptn = fnt->font_data + ( 3 * 12 * code);

	for( i = 0; i < 12; i++, fptn += 3){
		fnt->draw_buf[ i * 12 + 0]  =  cmap[ (fptn[ 0] >> 6) & 0x03];
		fnt->draw_buf[ i * 12 + 1]  =  cmap[ (fptn[ 0] >> 4) & 0x03];
		fnt->draw_buf[ i * 12 + 2]  =  cmap[ (fptn[ 0] >> 2) & 0x03];
		fnt->draw_buf[ i * 12 + 3]  =  cmap[ (fptn[ 0] >> 0) & 0x03];

		fnt->draw_buf[ i * 12 + 4]  =  cmap[ (fptn[ 1] >> 6) & 0x03];
		fnt->draw_buf[ i * 12 + 5]  =  cmap[ (fptn[ 1] >> 4) & 0x03];
		fnt->draw_buf[ i * 12 + 6]  =  cmap[ (fptn[ 1] >> 2) & 0x03];
		fnt->draw_buf[ i * 12 + 7]  =  cmap[ (fptn[ 1] >> 0) & 0x03];

		fnt->draw_buf[ i * 12 + 8]  =  cmap[ (fptn[ 2] >> 6) & 0x03];
		fnt->draw_buf[ i * 12 + 9]  =  cmap[ (fptn[ 2] >> 4) & 0x03];
		fnt->draw_buf[ i * 12 + 10] =  cmap[ (fptn[ 2] >> 2) & 0x03];
		fnt->draw_buf[ i * 12 + 11] =  cmap[ (fptn[ 2] >> 0) & 0x03];
	}

	put_pixstream_rect( x, y, fnt->draw_buf, 12 * 12, DIR_NORTH, 12);
}
#endif /* FONT_PLUS */
