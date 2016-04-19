/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: leafpack.c,v 1.1 2001/07/25 14:36:49 tf Exp $
 *
 */

/*
 * $Id: leafpack.c,v 1.1 2001/07/25 14:36:49 tf Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "leafpack.h"

void *filemap(const char *path, size_t * size);
void fileunmap(void *addr, size_t size);


#ifdef MAIN
/*
 * ------------------------------------------------------------------ *
 * sample main 
 */
int
main(int argc, char **argv)
{
    LeafPack *lp;

    if (argc < 2) {
	fprintf(stderr, "usage: %s packfile\n", argv[0]);
	exit(1);
    }

    if ((lp = leafpack_new(argv[1])) == NULL) {
	exit(1);
    }

    leafpack_print_table(lp, TRUE);

    leafpack_delete(lp);

    return 0;
}
#endif



/*
 * ------------------------------------------------------------------ *
 * private functilns 
 */


/*
 * Calculate key using length of archive data.
 * To use this function, the archive must include at least 3 files.
 */
static void
guess_key(LeafPack *lp)
{
    /*
     * find the top of table 
     */
    u_char *p = lp->addr + lp->size - 24 * lp->file_num;

    /*
     * zero 
     */
    lp->key[0] = p[11];

    /*
     * 1st position, (maybe :-)) constant 
     */
    lp->key[1] = (p[12] - 0x0a) & 0xff;
    lp->key[2] = p[13];
    lp->key[3] = p[14];
    lp->key[4] = p[15];

    /*
     * 2nd position, from 1st next position 
     */
    lp->key[5] = (p[38] - p[22] + lp->key[0]) & 0xff;
    lp->key[6] = (p[39] - p[23] + lp->key[1]) & 0xff;

    /*
     * 3rd position, from 2nd next position 
     */
    lp->key[7] = (p[62] - p[46] + lp->key[2]) & 0xff;
    lp->key[8] = (p[63] - p[47] + lp->key[3]) & 0xff;

    /*
     * 1st next position, from 2nd position 
     */
    lp->key[9] = (p[20] - p[36] + lp->key[3]) & 0xff;
    lp->key[10] = (p[21] - p[37] + lp->key[4]) & 0xff;
}

/*
 * ファイル名の正規化
 */
static void
regularize_name(char *name)
{
    char buf[12];
    int i = 0;

    strcpy(buf, name);
    while (i < 8 && buf[i] != 0x20) {
	name[i] = buf[i];
	i++;
    }

    name[i++] = '.';

    /*
     * file extention 
     */
    name[i++] = buf[8];
    name[i++] = buf[9];
    name[i++] = buf[10];

    name[i] = '\0';
}

/*
 * ファイルテーブルの展開
 */
static void
extract_table(LeafPack *lp)
{
    /*
     * find the top of table 
     */
    u_char *p = lp->addr + lp->size - 24 * lp->file_num;

    int i, j;
    int k = 0;
    int b[4];

    for (i = 0; i < lp->file_num; i++) {
	/*
	 * get filename 
	 */
	for (j = 0; j < 12; j++) {
	    lp->files[i].name[j] = (*p++ - lp->key[k]) & 0xff;
	    k = (++k) % LP_KEY_LEN;
	}
	regularize_name(lp->files[i].name);

	/*
	 * a position in the archive file 
	 */
	for (j = 0; j < 4; j++) {
	    b[j] = (*p++ - lp->key[k]) & 0xff;
	    k = (++k) % LP_KEY_LEN;
	}
	lp->files[i].pos =
	    (b[3] << 24) | (b[2] << 16) | (b[1] << 8) | b[0];

	/*
	 * file length 
	 */
	for (j = 0; j < 4; j++) {
	    b[j] = (*p++ - lp->key[k]) & 0xff;
	    k = (++k) % LP_KEY_LEN;
	}
	lp->files[i].len =
	    (b[3] << 24) | (b[2] << 16) | (b[1] << 8) | b[0];

	/*
	 * the head of the next file 
	 */
	for (j = 0; j < 4; j++) {
	    b[j] = (*p++ - lp->key[k]) & 0xff;
	    k = (++k) % LP_KEY_LEN;
	}

	/*
	 * don't use 
	 */
    }

}

/*
 * ---------------------------------------------------------------------- *
 * public functions 
 */


/*
 * ファイルを開いてファイル一覧テーブルを取得する。
 */
LeafPack *
leafpack_new(const char *path)
{
    LeafPack *lp;
    u_char *addr;
    size_t size;

    if ((addr = filemap(path, &size)) == NULL)
	return NULL;

    /*
     * マジックコードのチェック 
     */
    if (strncmp(addr, "LEAFPACK", 8)) {
	fprintf(stderr, "leafpack_open/check magic\n");
	fileunmap(addr, size);
	return NULL;
    }

    /*
     * データ領域確保 
     */
    if ((lp = malloc(sizeof(LeafPack))) == NULL) {
	perror("leafpack_open");
	fileunmap(addr, size);
	return NULL;
    }

    lp->addr = addr;
    lp->size = size;
    lp->file_num = addr[8] | addr[9] << 8;

    /*
     * ファイル用データ領域確保 
     */
    if ((lp->files = calloc(sizeof(lp->files[0]), lp->file_num)) == NULL) {
	perror("leafpack_open");
	leafpack_delete(lp);
	return NULL;
    }

    /*
     * check type 
     */
    if (lp->file_num == 0x0248 || lp->file_num == 0x03e1) {
	lp->type = LPTYPE_TOHEART;
    } else if (lp->file_num == 0x01fb) {
	lp->type = LPTYPE_KIZUWIN;
    } else if (lp->file_num == 0x0193) {
	lp->type = LPTYPE_SIZUWIN;
    } else if (lp->file_num == 0x0072) {
	lp->type = LPTYPE_SAORIN;
    } else {
	lp->type = LPTYPE_UNKNOWN;
    }

    /*
     * KEY の自動取得 
     */
    guess_key(lp);

    /*
     * ファイルテーブルの取得 
     */
    extract_table(lp);

    return lp;
}

/*
 * Free allocated memories and close file.
 */
void
leafpack_delete(LeafPack *lp)
{
    if (lp) {
	/*
	 * ファイルテーブル解放 
	 */
	if (lp->files)
	    free(lp->files);
	/*
	 * マップ解放 
	 */
	fileunmap(lp->addr, lp->size);
	free(lp);
    }
}

/*
 * パックの種別を表示する
 */
void
leafpack_print_type(LeafPack *lp)
{
    printf("Archive file: ");

    switch (lp->type) {
    case LPTYPE_SIZUWIN:
	printf("SHIZUKU for Windows\n");
	break;
    case LPTYPE_KIZUWIN:
	printf("KIZUATO for Windows\n");
	break;
    case LPTYPE_TOHEART:
	printf("To Heart\n\n");
	break;
    case LPTYPE_SAORIN:
	printf("Saorin to Issho!! (maxxdata.pak)\n");
	break;
    default:
	printf("Unknown\n");
    }
}

/*
 * テーブルの内容を表示する
 */
void
leafpack_print_table(LeafPack *lp, int verbose)
{
    int i;

    if (verbose == TRUE) {
	leafpack_print_type(lp);
	printf("Key: ");
	for (i = 0; i < LP_KEY_LEN; i++) {
	    printf("%02x ", lp->key[i]);
	}
	printf("\n\n");
    }
    if (verbose == TRUE) {
	printf("Filename      Position  Length\n");
	printf("------------  --------  -------\n");
    } else {
	printf("Filename       Length\n");
	printf("------------   -------\n");
    }
    for (i = 0; i < lp->file_num; i++) {
	printf("%12s  ", lp->files[i].name);
	if (verbose == TRUE) {
	    printf("%08x ", (u_int) lp->files[i].pos);
	}
	printf("%8d  ", lp->files[i].len);
	printf("\n");
    }
    printf("%d files.\n", lp->file_num);
}

int
leafpack_find(LeafPack *lp, const char *name)
{
    int i = 0;
    while (i < lp->file_num) {
	if (!strcasecmp(name, lp->files[i].name)) {
	    return i;
	}
	i++;
    }
    return -1;
}

u_char *
leafpack_extract(LeafPack *lp, int index, size_t * sizeret)
{
    int i;
    u_char *ret;
    u_char *p, *q;
    size_t size;

    /*
     * 領域確保 
     */
    if ((ret = malloc(lp->files[index].len)) == NULL) {
	perror("leafpack_extract");
	return NULL;
    }

    size = lp->files[index].len;	/*
					 * サイズ 
					 */
    if (sizeret)
	*sizeret = size;

    p = lp->addr + lp->files[index].pos;	/*
						 * 転送元 
						 */
    q = ret;			/*
				 * 転送先 
				 */

    /*
     * キーの解除 & copy 
     */
    for (i = 0; i < size; i++) {
	int a = *p++;
	a = (a - lp->key[i % LP_KEY_LEN]) & 0xff;
	*q++ = a;
    }

    return ret;
}

void
leafpack_lzs(const u_char * loadBuf, u_char * saveBuf, size_t size)
{
    u_char ring[0x1000];
    int i, j;
    int c, m;
    int flag;
    int pos, len;

    /*
     * initialize ring buffer 
     */
    memset(ring, 0, sizeof ring);

    /*
     * extract data 
     */
    for (i = 0, c = 0, m = 0xfee; i < size;) {

	/*
	 * flag bits, which indicates data or location 
	 */
	if (--c < 0) {
	    flag = *loadBuf++;
	    c = 7;
	}
	if (flag & 0x80) {
	    /*
	     * data 
	     */
	    saveBuf[i++] = ring[m++] = *loadBuf++;
	    m &= 0xfff;
	} else {
	    /*
	     * copy from ring buffer 
	     */

	    int data = loadBuf[0] + (loadBuf[1] << 8);
	    loadBuf += 2;

	    len = (data & 0x0f) + 3;
	    pos = data >> 4;

	    for (j = 0; j < len; j++) {
		saveBuf[i++] = ring[m++] = ring[pos++];
		m &= 0xfff;
		pos &= 0xfff;
	    }
	}
	flag = flag << 1;
    }
}

#if 0
void
leafpack_lzs2(const u_char * loadBuf, u_char * saveBuf, size_t size)
{
    u_char ring[0x1011];
    int i, j;
    int c, m;
    int flag;
    int pos, len;

    /*
     * initialize ring buffer 
     */
    memset(ring, 0, sizeof ring);

    /*
     * extract data 
     */
    for (i = 0, c = 0, m = 0xfee; i < size;) {

	/*
	 * flag bits, which indicates data or location 
	 */
	if (--c < 0) {
	    flag = ~(*loadBuf++);
	    c = 7;
	}
	if (flag & 0x80) {
	    /*
	     * data 
	     */
	    saveBuf[i++] = ring[m++] = ~(*loadBuf++);
	    m &= 0xfff;
	} else {
	    /*
	     * copy from ring buffer 
	     */

	    int data = ~(loadBuf[0] + (loadBuf[1] << 8));
	    loadBuf += 2;

	    len = (data & 0x0f) + 3;
	    pos = data >> 4;

	    for (j = 0; j < len; j++) {
		saveBuf[i++] = ring[m++] = ring[pos++];
		m &= 0xfff;
		pos &= 0xfff;
	    }
	}
	flag = flag << 1;
    }
}
#endif

void
leafpack_lzs2(const u_char * pLoadBuff, u_char * pSaveBuff,
	      size_t DataSize)
{
    u_long Index, LIndex, Len, FlagCount;
    u_short LFlag;
    u_char TextBuff[0x1011], Flag;

    /*
     * テキストバッファのクリア 
     */
    memset(TextBuff, 0, sizeof(TextBuff));

    /*
     * テキストバッファの書き込み位置の設定 
     */
    Index = 0xfee;

    FlagCount = 0;
    while (DataSize > 0) {
	if (FlagCount-- > 0) {
	    Flag <<= 1;
	} else {
	    /*
	     * フラグの取得 
	     */
	    Flag = ~(*(pLoadBuff++));
	    FlagCount = 7;
	}

	if (Flag & 0x80) {
	    TextBuff[Index++] = *pSaveBuff++ = ~(*pLoadBuff++);
	    Index &= 0x0fff;
	    DataSize--;
	} else {
	    /*
	     * 以前に出現した場所と長さの情報の取得(2 byte) 
	     */
	    LFlag = ~(*(pLoadBuff) + (*(pLoadBuff + 1) << 8));
	    pLoadBuff += 2;

	    /*
	     * 長さ 
	     */
	    Len = (LFlag & 0xf) + 3;
	    DataSize -= Len;

	    /*
	     * 以前出現した位置
	     */
	    LIndex = LFlag >> 4;

	    /*
	     * 以前出現した位置からコピー
	     */
	    while (Len-- > 0) {
		TextBuff[Index++] = *(pSaveBuff++) = TextBuff[LIndex++];
		LIndex &= 0x0fff;
		Index &= 0x0fff;
	    }
	}
    }
}


void
leafpack_lzs3(const u_char * pLoadBuff, u_char * pSaveBuff,
	      size_t DataSize)
{
    u_long Index, LIndex, Len, FlagCount;
    u_short LFlag;
    u_char TextBuff[0x1011], Flag;

    pSaveBuff += DataSize - 1;

    /*
     * テキストバッファのクリア 
     */
    memset(TextBuff, 0, sizeof(TextBuff));

    /*
     * テキストバッファの書き込み位置の設定 
     */
    Index = 0xfee;

    FlagCount = 0;
    while (DataSize > 0) {
	if (FlagCount-- > 0) {
	    Flag <<= 1;
	} else {
	    /*
	     * フラグの取得 
	     */
	    Flag = ~(*(pLoadBuff++));
	    FlagCount = 7;
	}

	if (Flag & 0x80) {
	    TextBuff[Index++] = *pSaveBuff-- = ~(*pLoadBuff++);
	    Index &= 0x0fff;
	    DataSize--;
	} else {
	    /*
	     * 以前に出現した場所と長さの情報の取得(2 byte) 
	     */
	    LFlag = ~(*(pLoadBuff) + (*(pLoadBuff + 1) << 8));
	    pLoadBuff += 2;

	    /*
	     * 長さ 
	     */
	    Len = (LFlag & 0xf) + 3;
	    DataSize -= Len;

	    /*
	     * 以前出現した位置
	     */
	    LIndex = LFlag >> 4;

	    /*
	     * 以前出現した位置からコピー
	     */
	    while (Len-- > 0) {
		TextBuff[Index++] = *(pSaveBuff--) = TextBuff[LIndex++];
		LIndex &= 0x0fff;
		Index &= 0x0fff;
	    }
	}
    }
}

void *
filemap(const char *path, size_t * size)
{
    int fd;
    struct stat sb;
    u_char *addr;

    if ((fd = open(path, 0, O_RDONLY)) < 0) {
	perror(path);
	return NULL;
    }

    if (fstat(fd, &sb) < 0 ||
	(addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0))
	== MAP_FAILED) {
	perror("mmap");
	close(fd);
	return NULL;
    }
    close(fd);			/*
				 * マップしたので不要 
				 */

    if (size)
	*size = sb.st_size;

    return addr;
}

void
fileunmap(void *addr, size_t size)
{
    munmap(addr, size);
}
