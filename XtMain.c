/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: XtMain.c,v 1.9 2001/08/05 10:49:56 go Exp $
 *
 */

#ifndef XLVNSPACKDIR
#define XLVNSPACKDIR "/usr/local/share/xlvns"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <setjmp.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeLine.h>
#include <X11/Xaw/SmeBSB.h>

#ifdef XVIDMODE
#include "FullScreenShell.h"
#endif

#include "LvnsCore.h"

#define APPLICATIONCLASS "XLvns"

/* WM Protocol DELETE 用 */
Atom            wm_protocols[2];

/* options */
static XrmOptionDescRec options[] = {
#ifdef DEBUG
    {"-scn", "*scenarioNumber", XrmoptionSepArg, NULL},
#endif
    {"-demo", "*demoMode", XrmoptionNoArg, (XtPointer)"True"},
    {"-seen", "*seenMode", XrmoptionNoArg, (XtPointer)"True"},
    {"-fastclick", "*keyClickFast", XrmoptionNoArg, (XtPointer)"True"},
#ifdef XVIDMODE
    {"-screen", "*screenMode", XrmoptionNoArg, (XtPointer)"True"},
#endif
#ifdef MITSHM
    {"-noshared", "*noShared", XrmoptionNoArg, (XtPointer)"True"},
#endif
};

static XtAppContext app;
static void signal_handler(int sig) {
	XtAppSetExitFlag(app);
}

/* 
 * WM からの終了受理
 */
void
Delete(Widget w, XEvent * e, String * p, Cardinal * n)
{
    if (e->xclient.data.l[0] == wm_protocols[0] ||
		e->xclient.data.l[0] == wm_protocols[1]) {
		XtAppSetExitFlag(XtWidgetToApplicationContext(w));
    }
}

static void CreateMenu(Widget w, XEvent *e, String *p, Cardinal *n);

/* actions */
static XtActionsRec actions[] = {
    {"delete", Delete},
    {"menu", CreateMenu},
};

/* TOP Widget 用 translations */
String          translations =
"<Message>WM_PROTOCOLS: delete()";

/* LvnsCore 用 translations */
String          translations_core =
   "<Btn2Down>: menu()";

#ifdef XVIDMODE
/*
 * メニューから呼び出し用
 */

static void
ScreenWindowCallback(Widget w, XtPointer dat, XtPointer call)
{
	XtFullScreenShellWidgetScreenWindow((FullScreenShellWidget)dat);
}
#endif

static void
ClearGrab(Widget w, XtPointer dat, XtPointer call)
{
    XtUngrabPointer((Widget)dat, CurrentTime);
}

/* ------------------------------------------------------------------- 
 * システムメニュー
 */

/*
 * メニュー破棄用コールバック
 */
static void
destroyMenu(Widget w, XtPointer dat, XtPointer call)
{
    XtDestroyWidget((Widget)dat);
}

/*
 * プログラム終了用コールバック
 */
static void
endProgram(Widget w, XtPointer dat, XtPointer call)
{
    XtAppSetExitFlag(XtWidgetToApplicationContext(w));
}

/*
 * バージョン情報画面用コールバック
 */
static void
aboutProgram(Widget w, XtPointer dat, XtPointer call)
{
}

/*
 * コマンド起動用コールバック
 */
static void
execCommand(Widget w, XtPointer dat, XtPointer call)
{
    LvnsCoreExecCommand((LvnsCoreWidget)dat, XtName(w));
}

/*
 * メニュー作成
 *
 * Widget に現在利用可能なコマンドを問い合わせて
 * 動的にメニューを再構成する
 */
static void
CreateMenu(Widget widget, XEvent *e, String *p, Cardinal *n)
{
    Widget menu, w;
    Widget top = XtParent(widget);
    LvnsCoreWidget lcw = (LvnsCoreWidget)widget;
    LvnsCommandInfo *info;

    menu = XtVaCreatePopupShell("menu", simpleMenuWidgetClass, top, NULL);

    /* コマンド部分 */
    if ((info = LvnsCoreGetCommandList(lcw)) != NULL) {
        XtVaCreateManagedWidget("line", smeLineObjectClass, menu, NULL);
	while (info->command != NULL) {
	    w = XtVaCreateManagedWidget(info->command,
					smeBSBObjectClass, menu, 
					XtNsensitive, info->enable, NULL);
	    XtAddCallback(w, XtNcallback, execCommand, (XtPointer)lcw);
	    info++;
	}
    }

#ifdef XVIDMODE
    if (XtFullScreenShellWidgetEnableScreenMode(top)) {
        XtVaCreateManagedWidget("line", smeLineObjectClass, menu, NULL);
        if (XtFullScreenShellWidgetScreenMode(top)) {
            w = XtVaCreateManagedWidget("window", smeBSBObjectClass, menu, NULL);
            XtAddCallback(w, XtNcallback, ScreenWindowCallback, top);
        } else {
            w = XtVaCreateManagedWidget("screen", smeBSBObjectClass, menu, NULL);
            XtAddCallback(w, XtNcallback, ClearGrab, top);
            XtAddCallback(w, XtNcallback, ScreenWindowCallback, top);
        }
    }
#endif

#if 0
   /* ABOUT */
    XtVaCreateManagedWidget("line", smeLineObjectClass, menu, NULL);
    w = XtVaCreateManagedWidget("about", smeBSBObjectClass, menu, NULL);
    XtAddCallback(w, XtNcallback, aboutProgram, NULL);
#endif

    /* 終了 */
    XtVaCreateManagedWidget("line", smeLineObjectClass, menu, NULL);
    w = XtVaCreateManagedWidget("end", smeBSBObjectClass, menu, NULL);
    XtAddCallback(w, XtNcallback, endProgram, NULL);

    /* POPDOWN する時には抹消… */
#ifdef XVIDMODE
    if (!XtFullScreenShellWidgetScreenMode(top)) {
#endif
        XtAddCallback(menu, XtNpopdownCallback, ClearGrab, top);
#ifdef XVIDMODE
    }
#endif
    XtAddCallback(menu, XtNpopdownCallback, destroyMenu, (XtPointer)menu);

    /* メニューを移動 */
    {
        String param = "menu";
        XtCallActionProc(menu, "XawPositionSimpleMenu", e, &param, 1);
    }

    XtPopupSpringLoaded(menu);
#ifdef XVIDMODE
    if (!XtFullScreenShellWidgetScreenMode(top)) {
#endif
        XtGrabPointer(top,
                      True, 0,
                      GrabModeAsync, GrabModeAsync, 
                      XtWindow(top), None, CurrentTime);
#ifdef XVIDMODE
    }
#endif
}

typedef void Sigfunc(int);
Sigfunc *
setup_signal(int signo, Sigfunc *func)
{
	struct sigaction act, oact;
	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (sigaction(signo, &act, &oact) < 0)
		return SIG_ERR;
	return oact.sa_handler;
}

int
main(int argc, char **argv)
{
    Widget          top;
    LvnsCoreWidget  core;
	Lvns            *lvns;

    char savepath[FILENAME_MAX];
	char *datapath;

	srandom((unsigned)getpid());

    /* 保存用ディレクトリのチェック */
    {
        struct stat sb;
        snprintf(savepath, sizeof savepath, "%s/%s", getenv("HOME"), ".xlvns");
        if (stat(savepath, &sb) < 0) {
            perror(savepath);
            fprintf(stderr, "%s: making...\n", savepath);
            if (mkdir(savepath, 0700)  < 0) {
                perror(savepath);
                exit(1);
            }
        } else if (!(sb.st_mode & S_IFDIR)) {
            fprintf(stderr, "%s: not directory!\n", savepath);
            exit(1);
        }
    }

	if (!(datapath = getenv("XLVNSPACKDIR")))
		datapath = XLVNSPACKDIR;

    /* バージョン表示 */
    fprintf(stderr, VERSION); 

#ifdef XVIDMODE
    top = XtVaOpenApplication(&app, APPLICATIONCLASS,
							  options, XtNumber(options),
							  &argc, argv, NULL, fullScreenShellWidgetClass,
							  NULL);
#else
    top = XtVaOpenApplication(&app, APPLICATIONCLASS,
							  options, XtNumber(options),
							  &argc, argv, NULL, sessionShellWidgetClass,
							  NULL);
#endif
	setup_signal(SIGINT,  signal_handler);
	setup_signal(SIGTERM, signal_handler);

    XtAppAddActions(app, actions, XtNumber(actions));
    XtOverrideTranslations(top, XtParseTranslationTable(translations));

    /* making childs */
	core = (LvnsCoreWidget)
		XtVaCreateManagedWidget("lvnscore", lvnsCoreWidgetClass, top, NULL);

	
    XtOverrideTranslations((Widget)core, 
                           XtParseTranslationTable(translations_core));

	LvnsCoreSetLvns(core, lvns = LvnsNew());
	LvnsSetDataPath(lvns, datapath);
    LvnsSetSaveDataPath(lvns, savepath);

    /* realize */
    XtRealizeWidget(top);
    wm_protocols[0] = XInternAtom(XtDisplay(top), "WM_DELETE_WINDOW", False);
    wm_protocols[1] = XInternAtom(XtDisplay(top), "WM_SAVEYOURSELF", False);
    XSetWMProtocols(XtDisplay(top), XtWindow(top), wm_protocols, 2);

	LvnsMain(lvns);
 end:
    XtDestroyWidget(top);
    exit(0);
}
