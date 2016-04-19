/*
 * LEAF Visual Novel System For MGL2
 * (c) Copyright 2001 TF <tf@denpa.org>
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: mgAction.c,v 1.7 2001/08/12 11:35:44 tf Exp $
 *
 */
#include <stdio.h>
#include <stdlib.h>

#include "Lvns.h"

static void
MglGetNormalEvent(Lvns *lvns, int timeout)
{
    int key;
    fd_set fds;

	FD_ZERO(&fds);
	FD_SET(0, &fds);
	key_select(1, &fds, timeout);
	key = get_key(0);
	
	switch (key) {
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	  LvnsInputNumber(lvns, key - '0');
	  break;
	case MK_UP:
	case 'k':
	  LvnsCursorUp(lvns);
	  break;
	case MK_DOWN:
	case 'j':
	  LvnsCursorDown(lvns);
	  break;
	case '\r':
	  LvnsSelect(lvns);
	  break;
	case 27: /* ESC key */
	  LvnsCancel(lvns);
	  break;
	case ' ':
	  LvnsImageMode(lvns);
	  break;
	case 's' | MGL_SKM_ALT:
	case 's' | MGL_SKM_CTRL:
	  LvnsSkipTillSelect(lvns);
	  break;
	case 'q' | MGL_SKM_ALT:
	case 'q' | MGL_SKM_CTRL:
	  LvnsJump(lvns, LVNS_JUMP_END);
 	case -1:
 	  return;
	}
}

static void
MglGetSelection(Lvns *lvns, int timeout)
{
  int key;
  fd_set fds;
  
  FD_ZERO(&fds);
  FD_SET(0, &fds);
  key_select(1, &fds, timeout);
  key = get_key(0);
  
  switch(key) {
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    LvnsInputNumber(lvns, key - '0');
    break;
  case MK_UP:
  case 'k':
    LvnsCursorUp(lvns);
    break;
  case MK_DOWN:
  case 'j':
    LvnsCursorDown(lvns);
    break;
  case '\n':
  case '\r':
    LvnsSelect(lvns);
    break;
  case 27: /* ESC key */
    LvnsCancel(lvns);
    break;
  }
}

void
MglGetEvent(Lvns *lvns, int timeout)
{
    int key;
    fd_set fds;

    FD_ZERO(&fds);
    FD_SET(0, &fds);
    key_select(1, &fds, timeout);
    key = get_key(0);
    
    switch (key) {
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      LvnsInputNumber(lvns, key - '0');
      break;
    case MK_UP:
    case 'k':
      LvnsCursorUp(lvns);
      break;
    case MK_DOWN:
    case 'j':
      LvnsCursorDown(lvns);
      break;
    case '\r':
      LvnsSelect(lvns);
      break;
    case 27: /* ESC key */
      LvnsCancel(lvns);
      break;
    case ' ':
      LvnsImageMode(lvns);
      break;
    case 's' | MGL_SKM_ALT:
    case 's' | MGL_SKM_CTRL:
      LvnsSkipTillSelect(lvns);
      break;
    case 'q' | MGL_SKM_ALT:
    case 'q' | MGL_SKM_CTRL:
      LvnsJump(lvns, LVNS_JUMP_END);
    case -1:
      return;
    }
}
