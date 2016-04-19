/*
 * FullScreenShell
 * $Id: FullScreenShell.h,v 1.1 2001/06/09 15:20:24 go Exp $
 */
#ifndef __FullScreenShell_h
#define __FullScreenShell_h

#include <X11/Shell.h>

typedef struct _FullScreenShellClassRec *FullScreenShellWidgetClass;
typedef struct _FullScreenShellRec *FullScreenShellWidget;
extern WidgetClass fullScreenShellWidgetClass;

/* resouces */
#define XtNscreenMode "screenMode"
#define XtCScreenMode "ScreenMode"

/* public functions */
Bool XtFullScreeShellWidgetEnableScreenMode(FullScreenShellWidget w);
Bool XtFullScreeShellWidgetScreenMode(FullScreenShellWidget w);
void XtFullScrenShellWidgetSetBestSize(FullScreenShellWidget w, Dimension width, Dimension height);
void XtFullScreenShellWidgetRestoreMode(FullScreenShellWidget fssw);

void XtFullScreenShellWidgetToScreenMode(FullScreenShellWidget fssw);
void XtFullScreenShellWidgetToWindowMode(FullScreenShellWidget fssw);
void XtFullScreenShellWidgetScreenWindow(FullScreenShellWidget fssw);

#endif

