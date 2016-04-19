/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: leafpack.h,v 1.1 2001/07/25 14:36:49 tf Exp $
 *
 */

#ifndef ___LEAFPAK_H
#define ___LEAFPAK_H

#define LP_KEY_LEN 11

#include <sys/types.h>

typedef enum {
    LPTYPE_SIZUWIN,
    LPTYPE_KIZUWIN,
    LPTYPE_TOHEART,
    LPTYPE_SAORIN,
    LPTYPE_UNKNOWN
} LeafPackType;

/*
 * パック情報 
 */
typedef struct {

    LeafPackType type;		/*
				 * パックの種別         
				 */

    u_char *addr;		/*
				 * 先頭アドレス         
				 */
    size_t size;		/*
				 * サイズ               
				 */
    int file_num;		/*
				 * パック中のファイル数 
				 */
    int key[LP_KEY_LEN];	/*
				 * 展開用キー           
				 */

    /*
     * ファイル情報 
     */
    struct LeafFileInfo {
	char name[13];		/*
				 * ファイル名                     
				 */
	off_t pos;		/*
				 * ファイルの先頭からのオフセット 
				 */
	size_t len;		/*
				 * ファイルのサイズ               
				 */
    } *files;

} LeafPack;

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

extern LeafPack *leafpack_new(const char *packname);
extern void leafpack_delete(LeafPack *);

extern void leafpack_print_type(LeafPack *);
extern void leafpack_print_table(LeafPack *, int verbose);

extern int leafpack_find(LeafPack *p, const char *name);
extern u_char *leafpack_extract(LeafPack *p, int index, size_t * sizeret);

void leafpack_lzs(const u_char * loadBuf, u_char * saveBuf, size_t size);
void leafpack_lzs2(const u_char * loadBuf, u_char * saveBuf, size_t size);
void leafpack_lzs3(const u_char * loadBuf, u_char * saveBuf, size_t size);

#endif
