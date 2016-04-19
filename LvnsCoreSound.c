/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: LvnsCoreSound.c,v 1.29 2001/08/02 20:05:30 go Exp $
 *
 */

/*
 * LvnsCore サウンド処理回り
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/socket.h>

#include "LvnsCoreP.h"

/*
 * サウンドプロセス通信処理エンジン
 * サウンドの演奏が一回完了すると、子プロセスから通信が入るためここが呼ばれる
 */
static void
SoundControl(XtPointer cl, int *fd, XtInputId *id)
{
    char buf[100];
    size_t len;
    LvnsCoreWidget lcw = (LvnsCoreWidget) cl;

    if ((len = read(LCW.sound_ctl_fd, buf, 1)) < 0) {
        perror("SoundControl");
    } else if (len == 1) {
        if (buf[0] == 'E') {
            /* 特に処理は無し… */
    if (LCW.sound_ctl_id) {
        XtRemoveInput(LCW.sound_ctl_id);
    }
    LCW.sound_ctl_id = None;
        }
    }
}

/*
 * データ読み込み 
 */
static void
LoadSound(LvnsCoreWidget lcw, const char* name)
{
    char buf[100];
    if (LCW.sound_ctl_fd >= 0 && LCW.sound_ctl_id == None) {
        snprintf(buf, sizeof buf, "L%s\n", name);
        write(LCW.sound_ctl_fd, buf, strlen(buf));
    }
}

/*
 * 演奏開始
 */
static void
StartSound(LvnsCoreWidget lcw)
{
    /* 複数のサウンドは同時に鳴らせない… */
    if (LCW.sound_ctl_fd >= 0 && LCW.sound_ctl_id == None) {
        XtAppContext app = XtWidgetToApplicationContext((Widget)lcw);
        write(LCW.sound_ctl_fd, "S\n", 2);
        LCW.sound_ctl_id = XtAppAddInput(app, 
                                         LCW.sound_ctl_fd,
                                         (XtPointer)XtInputReadMask,
                                         SoundControl,
                                         lcw);
    }
}

/*
 * 演奏完了(データを破棄)
 */
static void
StopSound(LvnsCoreWidget lcw)
{
    dprintf((stderr, "stop sound\n"));
    if (LCW.sound_ctl_fd >= 0 && LCW.sound_ctl_id) {
        write(LCW.sound_ctl_fd, "E\n", 2);
    }
}

/*
 * 演奏状態の確認
 */
static int
GetSoundState(LvnsCoreWidget lcw)
{
    if (LCW.sound_ctl_fd >= 0 && LCW.sound_ctl_id) {
        return True;
    } else {
        return False;
    }
}       

/* 
 * 演奏エンジンの停止
 */
static void
CloseSound(LvnsCoreWidget lcw)
{
    /* サウンド用プロセス解放 */
    if (LCW.sound_ctl_id != None) {
        XtRemoveInput(LCW.sound_ctl_id);
    }
    if (LCW.sound_ctl_fd >= 0) {
        close(LCW.sound_ctl_fd);  /* 相手はこれをうけて終了する */
    }

    LCW.sound_ctl_id  = None;
    LCW.sound_ctl_fd  = -1;
}

/*
 * サウンド処理用子プロセス 
 */
#include "audioinfo.h"  /* SOUND-audio Control */
static void
OpenSound(LvnsCoreWidget lcw)
{
    Lvns *lvns = LCW.info;
    char *devicename;
    AUDIOInfo *ainfo;
    int fd[2];
    pid_t pid;

    devicename = getenv("AUDIODEVICE");
    if (!devicename)
        devicename = AUDIODEVICE;

    if ((ainfo = audioinfo_new(devicename)) == NULL) {
        return;
    }

    if (socketpair(AF_UNIX, SOCK_STREAM, AF_UNSPEC, fd) < 0) {
        perror("socketpair AF_UNIX/PF_UNSPEC");
        return;
    }

    if ((pid = fork()) < 0) {
        perror("fork");
        audioinfo_delete(ainfo);
        close(fd[0]);
        close(fd[1]);
        return;
    } else if (pid) {
        audioinfo_delete(ainfo);
        close(fd[1]);
        LCW.sound_ctl_fd = fd[0];
        return;
    }

    /* 演奏処理用子プロセス */
    dprintf((stderr, "starting sound process...\n"));
    close(fd[0]);

    {
        char buf[100];
        u_char *sound_data = NULL;
        size_t sound_len;
        FILE *fp;

        if ((fp = fdopen(fd[1], "rw")) == NULL) {
            perror("fdopen");
            exit(1);
        }
        
        while (fgets(buf, sizeof buf, fp)) {
            
            char *p;
            if ((p = strchr(buf, '\n'))) {
                *p = '\0';
            }

            switch (buf[0]) {
            case 'L':
                /* ロード */
                sound_data = LvnsLoadData(lvns, buf + 1, &sound_len);
                break;

            case 'S': 
                /* 演奏開始 */
                if (sound_data) {
                    audioinfo_play(ainfo, sound_data, sound_len);
                }
                /* 演奏完了を親に通知 */
                write(fd[1], "E", 1);
                break;

            case 'E':
                /* 演奏終了 */
                if (sound_data)
                    free(sound_data);
                sound_data = NULL;
            }
        }
    }
    exit(0);
}

void 
LvnsCoreInitSound(LvnsCoreWidget lcw)
{
	LCW.sound.depend   = lcw;
	LCW.sound.open     = OpenSound;
	LCW.sound.close    = CloseSound;
	LCW.sound.load     = LoadSound;
	LCW.sound.start    = StartSound;
	LCW.sound.stop     = StopSound;
	LCW.sound.getState = GetSoundState;

    LCW.sound_ctl_id  = None;
    LCW.sound_ctl_fd  = -1;
}
