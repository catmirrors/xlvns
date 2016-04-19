/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: audioinfo.c,v 1.6 2000/08/09 16:52:04 go Exp $
 *
 */

/*
 * サウンド再生用のモジュール
 * 各 OS 用に同様のものを書き起こせば良いようにしたつもり…
 * _AUDIOInfo は必要に応じて変更して下さい
 *
 * 情報保持用構造体
 * struct _AUDIOInfo
 *
 * 初期化用
 * AUDIOInfo *audioinfo_new(const char *audiodevice);  
 *
 * 情報破棄用
 * void audioinfo_delete(AUDIOInfo *ainfo)
 *
 * データ (11KHz, 16bit, Linear, Stereo, Little Endian) のべた再生
 * void audioinfo_play(AUDIOInfo *ainfo, const char *data, size_t size);
 *
 * プロセスは LvnsCoreSound のほうで分けてあるのでそのまま出力するだけで
 * かまいません
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/param.h>

#include "audioinfo.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#ifdef ESDAUDIO

#include <esd.h>

struct _AUDIOInfo {
    int fd;
};

AUDIOInfo *
audioinfo_new(const char *audiodevice)
{
    AUDIOInfo *ainfo;
    int fd;

    if ((ainfo = malloc(sizeof *ainfo)) == NULL) {
        perror("audioinfo_new");
        exit(1);
    }

    if ((fd = esd_play_stream_fallback(ESD_BITS16 | ESD_STEREO | ESD_STREAM | ESD_PLAY, 11025, audiodevice, "xlvns")) < 0) {
        fprintf(stderr, "ESD: can't connect to %s.\n", audiodevice);
        free(ainfo);
        return NULL;
    }
    ainfo->fd = fd;
    return ainfo;
}

void
audioinfo_delete(AUDIOInfo *ainfo)
{
    if (ainfo && ainfo->fd >= 0) {
        close(ainfo->fd);
        free(ainfo);
    }
}

void
audioinfo_play(AUDIOInfo *ainfo, const char *data, size_t size)
{
  if (ainfo->fd >= 0) {
    write(ainfo->fd, data, size);
    sleep(1); /* XXX */
  }
}

#else 

/*
 * OpenBSD/Solaris 用 (/dev/audio インターフェース仕様)
 */
#if defined(__OpenBSD__) || (defined(sun) && defined(SVR4))
#include <sys/audioio.h>

struct _AUDIOInfo {
    int fd;
};

AUDIOInfo *
audioinfo_new(const char *audiodevice)
{
    AUDIOInfo *ainfo;
    struct audio_info info;
    int fd;

    if ((ainfo = malloc(sizeof *ainfo)) == NULL) {
        perror("audioinfo_new");
        exit(1);
    }

    if ((fd = open(audiodevice, O_WRONLY, 0)) < 0) {
        perror(audiodevice);
        return NULL;
    }

    AUDIO_INITINFO(&info);

#if defined (__OpenBSD__)
    info.play.sample_rate = 11000;
    info.play.encoding    = AUDIO_ENCODING_SLINEAR_LE;
#else /* Solaris */
    info.play.sample_rate = 11025;
    info.play.encoding    = AUDIO_ENCODING_LINEAR;
#endif
    info.play.precision   = 16;
    info.play.channels    = 2;
    if (ioctl(fd, AUDIO_SETINFO, &info) < 0) {
        perror(audiodevice);
        close(fd);
        return NULL;
    }

    ainfo->fd = fd;
    return ainfo;
}

void
audioinfo_delete(AUDIOInfo *ainfo)
{
    if (ainfo && ainfo->fd >= 0) {
        close(ainfo->fd);
        free(ainfo);
    }
}

#if !defined(__OpenBSD__) 
#include <sys/byteorder.h>
#endif

void
audioinfo_play(AUDIOInfo *ainfo, const char *data, size_t size)
{
    size_t ret;
#if !defined(__OpenBSD__) && defined(_BIG_ENDIAN)
    /* エンディアン変換する */
    int i;
    u_char *data2;
    if ((data2 = malloc(size)) == NULL) 
	return;
    for (i=0;i<size;i++) {
	data2[i]   = data[i+1];
	data2[i+1] = data[i];
    }
    do {
        if ((ret = write(ainfo->fd, data2, size)) < 0) {
            perror("audioinfo_play");
            return;
        }
        size -= ret;
        data += ret;
    } while (size > 3);
    free(data2);
#else 
    do {
        if ((ret = write(ainfo->fd, data, size)) < 0) {
            perror("audioinfo_play");
            return;
        }
        size -= ret;
        data += ret;
    } while (size > 3);
#endif
    /* 演奏完了待ち */
    ioctl(ainfo->fd, AUDIO_DRAIN, NULL);
}

/*
 * FreeBSD/NetBSD/Linux 用コード (OSS互換仕様)
 * OpenBSD もこれでもいけるはず…
 */
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__linux__)

#if defined(__FreeBSD__)
#include <machine/soundcard.h>
#elif defined(__NetBSD__)
#include <soundcard.h>
#elif defined(__linux__)
#include <sys/soundcard.h>
#endif

struct _AUDIOInfo {
    int fd;
};

AUDIOInfo *
audioinfo_new(const char *audiodevice)
{
    AUDIOInfo *ainfo;
    int fd, t;

    if ((ainfo = malloc(sizeof *ainfo)) == NULL) {
        perror("audioinfo_new");
        exit(1);
    }

    if ((fd = open(audiodevice, O_WRONLY, 0)) < 0) {
        perror(audiodevice);
        return NULL;
    }
    
    t = 16;
    if (ioctl(fd, SNDCTL_DSP_SAMPLESIZE, &t) < 0) {
        perror(audiodevice);
        close(fd);
        return NULL;
    }
    
    t = 1;
    if (ioctl(fd, SNDCTL_DSP_STEREO, &t) < 0) {
        perror(audiodevice);
        close(fd);
        return NULL;
    }
    
    t = 11000;
    if (ioctl(fd, SNDCTL_DSP_SPEED, &t) < 0) {
        perror(audiodevice);
        close(fd);
        return NULL;
    }
    
    ainfo->fd = fd;
    return ainfo;
}

void
audioinfo_delete(AUDIOInfo *ainfo)
{
    if (ainfo && ainfo->fd >= 0) {
        close(ainfo->fd);
        free(ainfo);
    }
}

void
audioinfo_play(AUDIOInfo *ainfo, const char *data, size_t size)
{
    size_t ret;
    do {
        if ((ret = write(ainfo->fd, data, size)) < 0) {
            perror("audioinfo_play");
            return;
        }
        size -= ret;
        data += ret;
    } while (size > 3);
    /* 演奏完了待ち */
    ioctl(ainfo->fd, SNDCTL_DSP_SYNC, NULL);
}

#else

/*
 * 何もしないダミー関数
 */
struct _AUDIOInfo {};
AUDIOInfo *audioinfo_new(const char *audiodevice) { return NULL; }
void audioinfo_play(AUDIOInfo *ainfo, const char *data, size_t size) {};
void audioinfo_delete(AUDIOInfo *ainfo) {};

#endif

#endif
