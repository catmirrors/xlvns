/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: cdinfo.c,v 1.16 2000/10/30 11:48:31 go Exp $
 *
 */

/*
 * CD-DA 再生用のモジュール
 * 各 OS 用に同様のものを書き起こせば良いようにしたつもり…
 *
 * 情報保持用構造体
 * CDTimeInfo, CDInfo
 *
 * 初期化用 
 * CDInfo *cdinfo_new(const char *devicename);
 * 
 * 情報破棄用
 * void cdinfo_delete(CDInfo *info);
 *
 * 指定したトラックを演奏開始
 * int cdinfo_play(CDInfo *cdinfo, int no);
 *
 * 演奏中止
 * int cdinfo_stop(CDInfo *cdinfo);
 *
 * 演奏停止
 * int cdinfo_pause(CDInfo *cdio);
 *
 * CD 音量設定 (0-255)
 * int cdinfo_set_volume(CDInfo *cdinfo, int vol);
 *
 * 現在の演奏時刻の取得
 * int cdinfo_get_current_time(CDInfo *cdinfo, CDTimeInfo *current);
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include "cdinfo.h"

static u_int msf2lba (u_char m, u_char s, u_char f);
static int cdinfo_get_entry(CDInfo *info);

static u_int 
msf2lba (u_char m, u_char s, u_char f)
{
	return (((m * 60) + s) * 75 + f) - 150;
}

#ifdef BSD
#include <sys/cdio.h>

/*
 * トラック情報の取得
 */
static int
cdinfo_get_entry(CDInfo *info)
{
    struct ioc_toc_header toc_header;
    struct ioc_read_toc_entry toc;
    int no;

    /* 情報クリア */
    if (info->infos)
        free(info->infos);
    info->track_no = 0;

    if (ioctl(info->fd, CDIOREADTOCHEADER, &toc_header) < 0) {
        return -1;
    }

    /* ヘッダ情報取得 */
    no = toc_header.ending_track - toc_header.starting_track + 1;

    /* TOC 情報取得 */
    toc.address_format = CD_MSF_FORMAT;
    toc.starting_track = 0;

    if ((toc.data = malloc(toc.data_len = sizeof *toc.data * (no+1))) == NULL) {
        perror("cdinfo_get_entry");
        exit(1);
    }

    if (ioctl(info->fd, CDIOREADTOCENTRYS, (char *)&toc) < 0) {
        free(toc.data);
        return -1;
    }

    info->track_no = no;
    if ((info->infos = malloc(sizeof info->infos[0] * no)) == NULL) {
        perror("cdinfo_get_entry");     
        exit(1);
    }

    {
        /* 情報取得 */
        int i;
        for (i=0; i<no; i++) {
            info->infos[i].block = msf2lba(toc.data[i].addr.msf.minute,
                                           toc.data[i].addr.msf.second,
                                           toc.data[i].addr.msf.frame);
            info->infos[i].len   = msf2lba(toc.data[i+1].addr.msf.minute,
                                           toc.data[i+1].addr.msf.second,
                                           toc.data[i+1].addr.msf.frame) -
                                               info->infos[i].block;
/*
            fprintf(stderr, "block:%d len:%d\n",
                    info->infos[i].block,
                    info->infos[i].len);
*/
        }
    }

    /* 作業領域解放 */
    free(toc.data);

    return 0;
}

int
cdinfo_play(CDInfo *info, int track_no)
{
    struct ioc_play_blocks t;
    if (info) {
        if (track_no > info->track_no)
            return -1;
        t.blk = info->infos[track_no-1].block;
        t.len = info->infos[track_no-1].len;
        return ioctl(info->fd, CDIOCPLAYBLOCKS, &t);
    } else
        return 0;
}

int
cdinfo_stop(CDInfo *info)
{
    if (info) {
        return ioctl(info->fd, CDIOCSTOP);
    } else
        return 0;
}

int
cdinfo_pause(CDInfo *info)
{
    if (info) {
        return ioctl(info->fd, CDIOCPAUSE);
    } else
        return 0;
}

/*
 * 現在の演奏中の時刻を取得
 */
int
cdinfo_get_current_time(CDInfo *info, CDTimeInfo *current)
{
    if (info) {

        struct ioc_read_subchannel s;
        struct cd_sub_channel_info data;

        memset(&s, 0, sizeof s);
        s.data = &data;
        s.data_len = sizeof data.header + sizeof data.what.position;
        s.address_format = CD_MSF_FORMAT;
        s.data_format    = CD_CURRENT_POSITION;
    
        if (ioctl(info->fd, CDIOCREADSUBCHANNEL, (char *)&s) < 0) 
            return -1;

        if (data.header.audio_status != CD_AS_PLAY_IN_PROGRESS) 
            return -1;

        if (current)
            current->block = msf2lba(s.data->what.position.reladdr.msf.minute,
                                     s.data->what.position.reladdr.msf.second,
                                     s.data->what.position.reladdr.msf.frame);
        return 0;

    } else
        return -1;
}

int
cdinfo_set_volume(CDInfo *info, int vol)
{
    if (info) {
        struct ioc_vol v;
        v.vol[0] = vol;
        v.vol[1] = vol;
        v.vol[2] = vol;
        v.vol[3] = vol;
        return ioctl(info->fd, CDIOCSETVOL, &v);
    } else
        return 0;
}

CDInfo *
cdinfo_new(const char *devicename)
{
    CDInfo *c;
    int fd;

    if ((fd = open(devicename, O_RDONLY, 0)) < 0) {
        return NULL;
    }

    if ((c = malloc(sizeof *c)) == NULL) {
        perror("cdinfo_new");
        exit(1);
    }

    c->fd       = fd;
    c->track_no = 0;
    c->infos    = NULL;

    if (cdinfo_get_entry(c)) {
        cdinfo_delete(c);
        return NULL;
    }

    return c;
}

#elif defined(__linux__)
#include <linux/cdrom.h>

static struct cdrom_msf lba2msf (u_int from, u_int to);

static struct cdrom_msf
lba2msf (u_int from, u_int to)
{
  struct cdrom_msf msf;

  msf.cdmsf_min0 = (from + 150) / (60 * 75);
  msf.cdmsf_sec0 = (from + 150) / 75 - msf.cdmsf_min0 * 60;
  msf.cdmsf_frame0 = (from + 150) - msf.cdmsf_sec0 * 75 - msf.cdmsf_min0 * 60 * 75;

  msf.cdmsf_min1 = (to + 150) / (60 * 75);
  msf.cdmsf_sec1 = (to + 150) / 75 - msf.cdmsf_min1 * 60;
  msf.cdmsf_frame1 = (to + 150) - msf.cdmsf_sec1 * 75 - msf.cdmsf_min1 * 60 * 75;

  return msf;
}

/*
 * トラック情報の取得
 */

static int
cdinfo_get_entry(CDInfo *info)
{
    struct cdrom_tochdr   toc_header;
    struct cdrom_tocentry toc;
    int no, endtrk, i;
    struct  cdrom_msf0 tocmsf[100];
    
    /* 情報クリア */
    if (info->infos)
        free(info->infos);
    info->track_no = 0;
    
    if (ioctl(info->fd, CDROMREADTOCHDR, &toc_header) < 0) {
        return -1;
    }

    /* ヘッダ情報取得 */
    no = endtrk = toc_header.cdth_trk1;

    /* TOC 情報取得 */
    toc.cdte_format = CDROM_MSF;
    for (i = 1; i <=endtrk; i++) {
        toc.cdte_track = i;
        if (ioctl(info->fd, CDROMREADTOCENTRY, &toc) < 0) {
            return -1;
        }
        tocmsf[i - 1].minute = toc.cdte_addr.msf.minute;
        tocmsf[i - 1].second = toc.cdte_addr.msf.second;
        tocmsf[i - 1].frame  = toc.cdte_addr.msf.frame;
    }
    toc.cdte_track = CDROM_LEADOUT;
    if (ioctl(info->fd, CDROMREADTOCENTRY, &toc) < 0) {
        return -1;
    }
    /* Linux PPC 対策 */

    tocmsf[endtrk].minute = toc.cdte_addr.msf.minute - (toc.cdte_addr.msf.second == 0 && toc.cdte_addr.msf.frame == 0 ? 1 : 0);
    tocmsf[endtrk].second = toc.cdte_addr.msf.second - toc.cdte_addr.msf.frame == 0 ? 1 : 0;
    tocmsf[endtrk].frame  = toc.cdte_addr.msf.frame > 0 ? toc.cdte_addr.msf.frame - 1 : 75 - toc.cdte_addr.msf.frame;
    
    
    if (ioctl(info->fd, CDROMREADTOCENTRY, (char *)&toc) < 0) {
        return -1;
    }

    info->track_no = no;
    if ((info->infos = malloc(sizeof info->infos[0] * no)) == NULL) {
        perror("cdinfo_get_entry");     
        exit(1);
    }

    {
        /* 情報取得 */
        int i;
        for (i=0; i<no; i++) {
            info->infos[i].block = msf2lba(tocmsf[i].minute,
                                           tocmsf[i].second,
                                           tocmsf[i].frame);
            info->infos[i].len   = msf2lba(tocmsf[i+1].minute,
                                           tocmsf[i+1].second,
                                           tocmsf[i+1].frame) -
                                               info->infos[i].block;
#if 0
            fprintf(stderr, "block:%d len:%d\n",
                    info->infos[i].block,
                    info->infos[i].len);
#endif

        }
    }

    return 0;
}

int
cdinfo_play(CDInfo *info, int track_no)
{
    struct cdrom_blk t;
    struct cdrom_msf msf;

    if (info) {
        if (track_no > info->track_no)
            return -1;
        t.from = info->infos[track_no-1].block;
        t.len = info->infos[track_no-1].len;
	msf = lba2msf(t.from, t.from + t.len);
        return ioctl(info->fd, CDROMPLAYMSF, &msf);
    } else
        return 0;
}

int
cdinfo_stop(CDInfo *info)
{
    if (info) {
        return ioctl(info->fd, CDROMSTOP);
    } else
        return 0;
}

int
cdinfo_pause(CDInfo *info)
{
    if (info) {
        return ioctl(info->fd, CDROMPAUSE);
    } else
        return 0;
}

/*
 * 現在の演奏中の時刻を取得
 */
int
cdinfo_get_current_time(CDInfo *info, CDTimeInfo *current)
{
    if (info) {

        struct cdrom_subchnl s;

        s.cdsc_format = CDROM_MSF;
        if (ioctl(info->fd, CDROMSUBCHNL, &s) < 0) {
            return -1;
        }
        if (s.cdsc_audiostatus != CDROM_AUDIO_PLAY) {
                return -1;
        }

        if (current)
            current->block = msf2lba(s.cdsc_reladdr.msf.minute,
                                     s.cdsc_reladdr.msf.second,
                                     s.cdsc_reladdr.msf.frame);
        return 0;

    } else
        return -1;
}

int
cdinfo_set_volume(CDInfo *info, int vol)
{
    if (info) {
        struct cdrom_volctrl v;
        v.channel0 = vol;
        v.channel1 = vol;
        v.channel2 = vol;
        v.channel3 = vol;
        return ioctl(info->fd, CDROMVOLCTRL, &v);
    } else
        return 0;
}

CDInfo *
cdinfo_new(const char *devicename)
{
    CDInfo *c;
    int fd;

    if ((fd = open(devicename, O_RDONLY, 0)) < 0) {
        return NULL;
    }

    if ((c = malloc(sizeof *c)) == NULL) {
        perror("cdinfo_new");
        exit(1);
    }

    c->fd       = fd;
    c->track_no = 0;
    c->infos    = NULL;

    if (cdinfo_get_entry(c)) { 
        cdinfo_delete(c);
        return NULL;
    }
        
    return c;
}

#elif defined(sun) && defined(SVR4)   /* Solaris */
#include <sys/cdio.h>

/*
 * トラック情報の取得
 */
static struct cdrom_msf lba2msf (u_int from, u_int to);

static struct cdrom_msf
lba2msf (u_int from, u_int to)
{
  struct cdrom_msf msf;

  msf.cdmsf_min0 = (from + 150) / (60 * 75);
  msf.cdmsf_sec0 = (from + 150) / 75 - msf.cdmsf_min0 * 60;
  msf.cdmsf_frame0 = (from + 150) - msf.cdmsf_sec0 * 75 - msf.cdmsf_min0 * 60 * 75;

  msf.cdmsf_min1 = (to + 150) / (60 * 75);
  msf.cdmsf_sec1 = (to + 150) / 75 - msf.cdmsf_min1 * 60;
  msf.cdmsf_frame1 = (to + 150) - msf.cdmsf_sec1 * 75 - msf.cdmsf_min1 * 60 * 75;

  return msf;
}

static int
cdinfo_get_entry(CDInfo *info)
{
    struct cdrom_tochdr   toc_header;
    struct cdrom_tocentry toc;
    int no, endtrk, i;
    struct  cdrom_msf tocmsf[100];
    
    /* 情報クリア */
    if (info->infos)
        free(info->infos);
    info->track_no = 0;
    
    if (ioctl(info->fd, CDROMREADTOCHDR, &toc_header) < 0) {
        return -1;
    }

    /* ヘッダ情報取得 */
    no = endtrk = toc_header.cdth_trk1;

    /* TOC 情報取得 */
    toc.cdte_format = CDROM_MSF;
    for (i = 1; i <=endtrk; i++) {
        toc.cdte_track = i;
        if (ioctl(info->fd, CDROMREADTOCENTRY, &toc) < 0) {
            return -1;
        }
        tocmsf[i - 1].cdmsf_min0    = toc.cdte_addr.msf.minute;
        tocmsf[i - 1].cdmsf_sec0    = toc.cdte_addr.msf.second;
        tocmsf[i - 1].cdmsf_frame0  = toc.cdte_addr.msf.frame;
    }
    toc.cdte_track = CDROM_LEADOUT;
    if (ioctl(info->fd, CDROMREADTOCENTRY, &toc) < 0) {
        return -1;
    }
    
    if (ioctl(info->fd, CDROMREADTOCENTRY, (char *)&toc) < 0) {
        return -1;
    }

    info->track_no = no;
    if ((info->infos = malloc(sizeof info->infos[0] * no)) == NULL) {
        perror("cdinfo_get_entry");     
        exit(1);
    }

    {
        /* 情報取得 */
        int i;
        for (i=0; i<no; i++) {
            info->infos[i].block = msf2lba(tocmsf[i].cdmsf_min0,
                                           tocmsf[i].cdmsf_sec0,
                                           tocmsf[i].cdmsf_frame0);
            info->infos[i].len   = msf2lba(tocmsf[i+1].cdmsf_min0,
                                           tocmsf[i+1].cdmsf_sec0,
                                           tocmsf[i+1].cdmsf_frame0) -
                                               info->infos[i].block;
#if 0
            fprintf(stderr, "block:%d len:%d\n",
                    info->infos[i].block,
                    info->infos[i].len);
#endif

        }
    }

    return 0;
}

int
cdinfo_play(CDInfo *info, int track_no)
{
    int start, len;
    struct cdrom_msf msf;

    if (info) {
        if (track_no > info->track_no)
            return -1;
        start = info->infos[track_no-1].block;
        len = info->infos[track_no-1].len;
	msf = lba2msf(start, start + len);
        return ioctl(info->fd, CDROMPLAYMSF, &msf);
    } else
        return 0;
}

int
cdinfo_stop(CDInfo *info)
{
    if (info) {
        return ioctl(info->fd, CDROMSTOP);
    } else
        return 0;
}

int
cdinfo_pause(CDInfo *info)
{
    if (info) {
        return ioctl(info->fd, CDROMPAUSE);
    } else
        return 0;
}

/*
 * 現在の演奏中の時刻を取得
 */
int
cdinfo_get_current_time(CDInfo *info, CDTimeInfo *current)
{
    if (info) {

        struct cdrom_subchnl s;

        s.cdsc_format = CDROM_MSF;
        if (ioctl(info->fd, CDROMSUBCHNL, &s) < 0) {
            return -1;
        }
        if (s.cdsc_audiostatus != CDROM_AUDIO_PLAY) {
                return -1;
        }

        if (current)
            current->block = msf2lba(s.cdsc_reladdr.msf.minute,
                                     s.cdsc_reladdr.msf.second,
                                     s.cdsc_reladdr.msf.frame);
        return 0;

    } else
        return -1;
}

int
cdinfo_set_volume(CDInfo *info, int vol)
{
    if (info) {
        struct cdrom_volctrl v;
        v.channel0 = vol;
        v.channel1 = vol;
	/*
        v.channel2 = vol;
        v.channel3 = vol;
	*/
        return ioctl(info->fd, CDROMVOLCTRL, &v);
    } else
        return 0;
}

CDInfo *
cdinfo_new(const char *devicename)
{
    CDInfo *c;
    int fd;

    if ((fd = open(devicename, O_RDONLY, 0)) < 0) {
        return NULL;
    }

    if ((c = malloc(sizeof *c)) == NULL) {
        perror("cdinfo_new");
        exit(1);
    }

    c->fd       = fd;
    c->track_no = 0;
    c->infos    = NULL;

    if (cdinfo_get_entry(c)) { 
        cdinfo_delete(c);
        return NULL;
    }
        
    return c;
}

#else

int cdinfo_play(CDInfo *cdinfo, int no) {return -1;}
int cdinfo_stop(CDInfo *cdinfo) {return -1;}
int cdinfo_pause(CDInfo *cdio) {return -1;}
int cdinfo_set_volume(CDInfo *cdinfo, int vol) {return -1;}
int cdinfo_get_current_time(CDInfo *cdinfo, CDTimeInfo *current){ return -1;}
CDInfo *cdinfo_new(const char *devicename) { return NULL; }

#endif

void
cdinfo_delete(CDInfo *info)
{
    if (info) {
        cdinfo_stop(info);
        close(info->fd);
        if (info->infos)
            free(info->infos);
        free(info);
    }
}
