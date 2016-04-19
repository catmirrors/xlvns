/*
 * LEAF Visual Novel System For X
 * (c) Copyright 1999,2000 Go Watanabe mailto:go@denpa.org
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: LvnsCore.h,v 1.8 2001/06/10 09:07:03 go Exp $
 *
 */

#ifndef __LvnsCore_h
#define __LvnsCore_h

#include <X11/Core.h>
#include "Lvns.h"

typedef struct _LvnsCoreClassRec *LvnsCoreWidgetClass;
typedef struct _LvnsCoreRec *LvnsCoreWidget;

extern WidgetClass lvnsCoreWidgetClass;

/* functions */
int LvnsCoreGetBestWidth(LvnsCoreWidget lcw);
int LvnsCoreGetBestHeight(LvnsCoreWidget lcw);
LvnsCommandInfo *LvnsCoreGetCommandList(LvnsCoreWidget lcw);
void LvnsCoreExecCommand(LvnsCoreWidget lcw, const char *command);
void LvnsCoreSetLvns(LvnsCoreWidget lcw, Lvns *lvns);

#ifndef XtRQuark
#define XtRQuark "Quark"
#endif

#endif
