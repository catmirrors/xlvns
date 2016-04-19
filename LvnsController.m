#import "LvnsController.h"

#define TICKSPERSEC 1.0

@implementation LvnsController

// NSView メソッド群

/* 再描画 */
- (void) drawRect : (NSRect) arBounds
{
	[image draw : arBounds];
	[[NSGraphicsContext currentContext] flushGraphics];
}

// イベント処理

- (void)mouseMoved:(NSEvent *)theEvent
{
}

- (void)mouseDown:(NSEvent *)theEvent
{
	[self Select : self];
}

- (void)rightMouseDown:(NSEvent *)theEvent
{
	[self Cancel : self];
} 

- (void)keyDown:(NSEvent *)theEvent
{
}

// actions 

- (IBAction) Select : (id) sender
{
	LvnsSelect(lvns);
}

- (IBAction) Cancel : (id) sender
{
	LvnsCancel(lvns);
}

- (IBAction) ImageMode : (id) sender
{
}

- (IBAction) SkipTillSelect : (id) sender
{
}

- (IBAction) SkipTillSelectForce : (id) sender
{
}

- (IBAction) CursorUp : (id) sender
{
	LvnsCursorUp(lvns);
}

- (IBAction) CursorDown : (id) sender
{
	LvnsCursorDown(lvns);
}

// privates

// Lvns System Depend Functions

/** 
 * 全画面をリフレッシュして実際の画面に反映させる
 */
static void 
FlushWindow(Lvns *lvns)
{
	LvnsController *self = lvns->system_depend;
	[self setNeedsDisplay : true];
	//[self lockFocus];
	//[self->image draw : NSMakeRect(0,0,WIDTH,HEIGHT)];
	//[self unlockFocus];
}

/**
 * 画面を部分的にリフレッシュして実際の画面に反映させる
 */
static void
FlushWindowArea(Lvns *lvns, int x, int y, int w, int h)
{
	LvnsController *self = lvns->system_depend;
	[self setNeedsDisplayInRect : NSMakeRect(x,y,w,h)];
	//[self lockFocus];
	//[self->image draw : NSMakeRect(x,HEIGHT-y-1,w,h)];
	//[self unlockFocus];
}

/**
 * 全体を描画(フラッシュ無し) 
 */
static void
DrawWindow(Lvns *lvns) 
{
	LvnsController *self = lvns->system_depend;
	[self->image copy : lvns->vram : 0 : 0];
}

/**
 * 部分描画(フラッシュ無し) 
 */
static void
DrawWindowArea(Lvns *lvns, int x, int y, int w, int h, int x2, int y2)
{
	LvnsController *self = lvns->system_depend;
	[self->image copyArea : lvns->vram : x : y : w : h : x2 : y2];
}

/**
 * 全画面画面消去(フラッシュ無し)
 */
static void
ClearWindow(Lvns *lvns)
{
	LvnsController *self = lvns->system_depend;
	[self->image clearArea : 0 : 0 : WIDTH : HEIGHT : lvns->vram->black];
}

/**
 * 部分画面消去(フラッシュ無し)
 */
static void
ClearWindowArea(Lvns *lvns, int x, int y, int w, int h)
{
	LvnsController *self = lvns->system_depend;
	[self->image clearArea : x : y : w : h : lvns->vram->black];
}

/**
 * 特定のインデックスにカラーを割り当てる
 */
static void
SetPaletteIndex(Lvns *lvns, int index, int r, int g, int b)
{
	LvnsController *self = lvns->system_depend;
	[self->image setPalette : index r:r g:g b:b];
}

/**
 * パレット設定(通常)
 * 全部を設定する…
 */
static void
MySetPalette(Lvns *lvns)
{
	LvnsController *self = lvns->system_depend;
	[self->image setPal : lvns->vram];
}

/**
 * パレット設定(multiple)
 */
static void
SetPaletteMulti(Lvns *lvns, int par16)
{
	LvnsController *self = lvns->system_depend;
	[self->image setPalMulti : lvns->vram : par16];
}

/**
 * パレット設定(screen)
 */
static void
SetPaletteScreen(Lvns *lvns, int par16)
{
	LvnsController *self = lvns->system_depend;
	[self->image setPalScreen : lvns->vram : par16];
}

/* 
 * マスクパターンでウインドウに対して描画を行う(フラッシュ無し)
 */
static void
DrawWindowMask(Lvns *lvns, int x, int y, int state)
{
	LvnsController *self = lvns->system_depend;
	[self->image copyMaskUnit : lvns->vram : x : y : x : y : state ];
}

/* 
 * 矩形パターンでウィンドウに対して描画を行う(フラッシュ無し)
 */
static void
DrawWindowSquareMask(Lvns *lvns, int x, int y, int state)
{
	LvnsController *self = lvns->system_depend;
	[self->image copySquareMaskUnit : lvns->vram : x : y : x : y : state];
}

/*
 * 表示されている画面のオフセットをずらす(振動用)
 */
static void
SetDispOffset(Lvns *lvns, int xoff, int yoff, int maxoff)
{
}

/* 
 * 文字パターン表示 (雫/痕 用)
 */
static void
PutPattern(Lvns *lvns, int x, int y, int index, u_char *data)
{
	LvnsController *self = lvns->system_depend;
	[self->image putPattern24_2 : x : y : index : data];
}

/* 
 * 文字パターン表示 (ToHeart用)
 */ 
static void 
PutPattern2(Lvns *lvns, int x, int y, int index, u_char *data)
{
	LvnsController *self = lvns->system_depend;
	[self->image putPattern24 : x : y : index : data];
}

/* 
 * タイマをリセットする
 * @param no タイマ番号
 */
static void
ResetTimer(Lvns *lvns, int no)
{
	LvnsController *self = lvns->system_depend;
    gettimeofday(&self->timer[no], NULL);
}

/*
 * タイマ値を取得する
 * @param no タイマ番号
 * @return タイマ値 (msec単位)
 */	// シンクロ
	// シンクロ

static long 
GetTimer(Lvns *lvns, int no)
{
	LvnsController *self = lvns->system_depend;
	struct timeval current;
    gettimeofday(&current, NULL);
    return (current.tv_sec - self->timer[no].tv_sec) * 1000 +
        (current.tv_usec - self->timer[no].tv_usec) / 1000;
}

/**
 * 矩形を描画する
 * @param x,y,w,h 位置とサイズ
 * @param idx 色のインデックス値
 */
static void
DrawBox(Lvns *lvns, int x, int y, int w, int h, int idx)
{
}

#include <sys/types.h>
#include <sys/stat.h>

/** 
 * 指定したファイルの更新日付を取得する
 */
static time_t
GetFileTime(Lvns *lvns, const char *path)
{
    struct stat sb;
    if (stat(path, &sb) < 0) {
        return 0;
    }
    return sb.st_mtime;
}

- (void) flip
{
	int wait, sa;

	wait = wait_count / 60;
	if (wait > 0) {
		u_long tw;
		sa = [NSDate timeIntervalSinceReferenceDate] - wait_clock;
		if (sa > 0) wait -= sa;
		if (wait > 0) {
			NSDate *date = [[NSDate alloc] initWithTimeIntervalSinceNow: wait];
			[NSThread sleepUntilDate : date];
			[date release];
		}
		tw = [NSDate timeIntervalSinceReferenceDate];
		sa = tw - wait_clock;
		if (sa > 0) 
			wait_count -= sa * 60;
		else 
			wait_count = 0;
		wait_clock = tw;
	}
	wait_count += TICKSPERSEC;
	if (wait_count < 0) {
		wait_count = 0;
	}
}

/*
 * 描画タイミング待ち処理
 */
static void
Flip(Lvns *lvns)
{
	LvnsController *self = lvns->system_depend;
	[self flip];
}

/**
 * メインエンジン 
 */
- (void) runMain : (id) param
{
    LvnsMain(lvns);
}

extern int system_type;

/**
 * 初期化処理
 */
- (void) applicationDidFinishLaunching:(NSNotification *)aNotification
{
    char *home = getenv("HOME");
    lvns = LvnsNew();

	if (home) {
		LvnsSetSaveDataPath(lvns, home);
	}
	//LvnsSetDataPath(lvns, );

	/* リソースの反映 */
	lvns->demo_mode        = False;
	lvns->seen_mode        = False;
	lvns->latitude_dark    = 11;
	lvns->key_click_fast   = True;

	/* システム依存関数群の設定 */
	lvns->flushWindow          = FlushWindow;
	lvns->flushWindowArea      = FlushWindowArea;
	lvns->drawWindow           = DrawWindow;
	lvns->drawWindowArea       = DrawWindowArea;
	lvns->clearWindow          = ClearWindow;
	lvns->clearWindowArea      = ClearWindowArea;
	lvns->setPaletteIndex      = SetPaletteIndex;
	lvns->setPalette           = MySetPalette;
	lvns->setPaletteMulti      = SetPaletteMulti;
	lvns->setPaletteScreen     = SetPaletteScreen;
	lvns->drawWindowMask       = DrawWindowMask;
	lvns->drawWindowSquareMask = DrawWindowSquareMask;
	lvns->setDispOffset        = SetDispOffset;
	lvns->putPattern           = PutPattern;
	lvns->putPattern2          = PutPattern2;
	lvns->resetTimer           = ResetTimer;
	lvns->getTimer             = GetTimer;
	lvns->drawBox              = DrawBox;
	lvns->getFileTime          = GetFileTime;
	lvns->flip                 = Flip;

	LvnsInitialize(lvns, system_type, self);

	//サイズ決定
	{
		NSSize size = { WIDTH, HEIGHT };
		[mainWindow setContentSize : size];
		[mainWindow setContentView : self];
		image = [[NSLvnsImage alloc] init : size];
	}

	wait_clock = [NSDate timeIntervalSinceReferenceDate];
	wait_count = TICKSPERSEC;

	// スレッドを分けて本体を動かす
	[NSThread detachNewThreadSelector: @selector(runMain:) toTarget:self withObject:self];
}

@end

