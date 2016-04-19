#ifndef __Sound_h
#define __Sound_h

typedef struct {
	void *depend;
    void (*open)(void *dep);                    /* SOUND初期化         */
    void (*close)(void *dep);                   /* SOUND終了           */
    void (*load)(void *dep, const char *name);  /* データのロード      */
    void (*start)(void *dep);                   /* 再生開始            */
    void (*stop)(void *dep);                    /* 停止                */
    int  (*getState)(void *dep);                /* 演奏中チェック      */
} Sound;

#define SoundOpen(m)     if ((m) && (m)->open)  (m)->open((m)->depend)
#define SoundClose(m)    if ((m) && (m)->close) (m)->close((m)->depend)
#define SoundLoad(m,n)   if ((m) && (m)->load)  (m)->load((m)->depend,n)
#define SoundStart(m)    if ((m) && (m)->start) (m)->start((m)->depend)
#define SoundStop(m)     if ((m) && (m)->stop)  (m)->stop((m)->depend)
#define SoundGetState(m) (((m) && (m)->getState) ? (m)->getState((m)->depend):1)

#endif
