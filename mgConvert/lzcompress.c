/*
 * LEAF Visual Novel System For MGL2
 * (c) Copyright 2001 TF <tf@denpa.org>
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: lzcompress.c,v 1.2 2001/08/11 12:30:07 tf Exp $
 */
#include <stdio.h>

#define RINGBUF_SIZE 0x1000	/*
				 * size of ring buffer 
				 */
#define F 18			/*
				 * longest match length 
				 */
#define NIL RINGBUF_SIZE	/*
				 * end of tree 
				 */

static void init_tree(void);
static void insert_node(int);
static void delete_node(int);

/*
 * for encoding
 */
unsigned long outcount = 0;	/* counter for output size */
unsigned char text[RINGBUF_SIZE + F - 1];	/* text buffer */
int dad[RINGBUF_SIZE + 1];
int lson[RINGBUF_SIZE + 1];
int rson[RINGBUF_SIZE + 257];	/* tree */
int matchpos;			/* the position of longest match pattern */
int matchlen;			/* the length of longest match pattern */

int
lz77_compress(unsigned char *img, int org_size, unsigned char *dst)
{
    int i, c, len, r, s, lastmatchlen, codeptr;
    unsigned char code[17], mask;
    unsigned long int incount = 0;
    int img_ptr = 0;
    int dst_ptr = 0;

    /*
     * initialize a tree 
     */
    init_tree();

    code[0] = 0;
    codeptr = 1;
    mask = 0x80;
    s = 0;
    r = RINGBUF_SIZE - F;

    /*
     * initialize a ring buffer 
     */
    for (i = s; i < r; i++) {
	text[i] = 0x0;
    }
    for (len = 0; len < F; len++) {
	c = img[img_ptr++];
	if (img_ptr > org_size)
	    break;
	text[r + len] = c;
    }

    incount = len;
    if (incount == 0) {
	return 0;
    }
    for (i = 1; i <= F; i++)
	insert_node(r - i);
    insert_node(r);

    do {
	if (matchlen > len) {
	    matchlen = len;
	}
	if (matchlen < 3) {
	    matchlen = 1;
	    code[0] |= mask;
	    code[codeptr++] = text[r];
	} else {
	    code[codeptr++] =
		(unsigned char) (((matchpos & 0x0f) << 4) |
				 (matchlen - 3));
	    code[codeptr++] = (unsigned char) (matchpos >> 4);
	}
	if ((mask >>= 1) == 0) {
	    for (i = 0; i < codeptr; i++) {
		dst[dst_ptr++] = code[i];
	    }
	    outcount += codeptr;
	    code[0] = 0;
	    codeptr = 1;
	    mask = 0x80;
	}
	lastmatchlen = matchlen;
	for (i = 0; i < lastmatchlen; i++) {
	    if (img_ptr == org_size) {
		break;
	    }
	    c = img[img_ptr++];
	    delete_node(s);
	    text[s] = c;
	    if (s < F - 1) {
		text[s + RINGBUF_SIZE] = c;
	    }
	    s = (s + 1) & (RINGBUF_SIZE - 1);
	    r = (r + 1) & (RINGBUF_SIZE - 1);
	    insert_node(r);
	}
	while (i++ < lastmatchlen) {
	    delete_node(s);
	    s = (s + 1) & (RINGBUF_SIZE - 1);
	    r = (r + 1) & (RINGBUF_SIZE - 1);
	    if (--len)
		insert_node(r);
	}
    } while (len > 0);

    if (codeptr > 1) {
	for (i = 0; i < codeptr; i++) {
	    dst[dst_ptr++] = code[i];
	}
	outcount += codeptr;
    }

    return dst_ptr;
}

/*
 * initialize a tree
 */
static void
init_tree(void)
{
    int i;

    for (i = RINGBUF_SIZE + 1; i <= RINGBUF_SIZE + 256; i++) {
	rson[i] = NIL;
    }
    for (i = 0; i < RINGBUF_SIZE; i++) {
	dad[i] = NIL;
    }
}

/*
 * insert a node r into a tree
 */
static void
insert_node(int r)
{
    int i, p, cmp;
    unsigned char *key;

    cmp = 1;
    key = &text[r];
    p = RINGBUF_SIZE + 1 + key[0];
    rson[r] = lson[r] = NIL;
    matchlen = 0;

    for (;;) {
	if (cmp >= 0) {
	    if (rson[p] != NIL)
		p = rson[p];
	    else {
		rson[p] = r;
		dad[r] = p;
		return;
	    }
	} else {
	    if (lson[p] != NIL)
		p = lson[p];
	    else {
		lson[p] = r;
		dad[r] = p;
		return;
	    }
	}
	for (i = 1; i < F; i++)
	    if ((cmp = key[i] - text[p + i]) != 0)
		break;
	if (i > matchlen) {
	    matchpos = p;
	    if ((matchlen = i) >= F)
		break;
	}
    }
    dad[r] = dad[p];
    lson[r] = lson[p];
    rson[r] = rson[p];
    dad[lson[p]] = r;
    dad[rson[p]] = r;
    if (rson[dad[p]] == p) {
	rson[dad[p]] = r;
    } else {
	lson[dad[p]] = r;
    }
    /*
     * detach p 
     */
    dad[p] = NIL;
}

/*
 * delete a node p from tree
 */
static void
delete_node(int p)
{
    int q;

    if (dad[p] == NIL) {	/*
				 * p isn't found 
				 */
	return;
    }

    if (rson[p] == NIL) {
	q = lson[p];
    } else if (lson[p] == NIL) {
	q = rson[p];
    } else {
	q = lson[p];
	if (rson[q] != NIL) {
	    do {
		q = rson[q];
	    } while (rson[q] != NIL);
	    rson[dad[q]] = lson[q];
	    dad[lson[q]] = dad[q];
	    lson[q] = lson[p];
	    dad[lson[p]] = q;
	}
	rson[q] = rson[p];
	dad[rson[p]] = q;
    }
    dad[q] = dad[p];
    if (rson[dad[p]] == p) {
	rson[dad[p]] = q;
    } else {
	lson[dad[p]] = q;
    }
    dad[p] = NIL;
}
