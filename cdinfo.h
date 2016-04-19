/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: cdinfo.h,v 1.4 2000/05/10 17:32:21 go Exp $
 *
 */

#ifndef __cdinfo_h_
#define __cdinfo_h_

typedef struct {
    int block;
    int len;
} CDTimeInfo;

typedef struct CdInfo {
    int fd;
    int track_no;
    CDTimeInfo *infos;
} CDInfo;


CDInfo *cdinfo_new(const char *devicename);
void cdinfo_delete(CDInfo *info);

int cdinfo_play(CDInfo *cdinfo, int no);
int cdinfo_stop(CDInfo *cdinfo);
int cdinfo_pause(CDInfo *cdio);
int cdinfo_set_volume(CDInfo *cdinfo, int vol);

int cdinfo_get_current_time(CDInfo *cdinfo, CDTimeInfo *current);

#endif
