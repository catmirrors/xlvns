#ifndef __Music_h
#define __Music_h

typedef struct {
	void *depend;
    void (*open)(void *dep);                 /* BGM初期化       */
    void (*close)(void *dep);                /* BGM終了         */
    void (*start)(void *dep, int no);        /* 演奏開始        */
    void (*stop)(void *dep);                 /* 演奏終了        */
    void (*pause)(void *dep);                /* 演奏一時停止    */
    void (*setVolume)(void *dep, int no);    /* ボリューム指定  */
    int  (*getState)(void *dep);             /* 演奏中チェック  */
} Music;

#define MusicOpen(m)        if ((m) && (m)->open)  (m)->open((m)->depend)
#define MusicClose(m)       if ((m) && (m)->close) (m)->close((m)->depend)
#define MusicStart(m,n)     if ((m) && (m)->start) (m)->start((m)->depend,n)
#define MusicStop(m)        if ((m) && (m)->pause) (m)->stop((m)->depend)
#define MusicPause(m)       if ((m) && (m)->pause) (m)->pause((m)->depend)
#define MusicSetVolume(m,v) if ((m) && (m)->setVolume) (m)->setVolume((m)->depend,v)
#define MusicGetState(m) (((m) && (m)->getState) ? (m)->getState((m)->depend):1)

#endif

