/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: audioinfo.h,v 1.2 2000/05/10 17:32:21 go Exp $
 *
 */

#ifndef __audioinfo_h_
#define __audioinfo_h_

typedef struct _AUDIOInfo AUDIOInfo;

AUDIOInfo *audioinfo_new(const char *devicename);
void audioinfo_delete(AUDIOInfo *audio);
void audioinfo_play(AUDIOInfo *ainfo, const char *data, size_t size);

#endif
