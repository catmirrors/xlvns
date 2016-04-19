/*
 * LEAF Visual Novel System For MGL2
 * (c) Copyright 2001 TF <tf@denpa.org>
 * All rights reserverd.
 *
 * ORIGINAL LVNS (c) Copyright 1996-1999 LEAF/AQUAPLUS Inc.
 *
 * $Id: mgMain.c,v 1.6 2001/08/12 11:35:44 tf Exp $
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "Lvns.h"

static void
usage_exit()
{
  fprintf(stderr, "usage: mglvns [-v <LVNS version>] [-f <FONT>] [-d] [-h]\n");
  fprintf(stderr, "    -v <LVNS version>: Specify the version of LVNS\n");
  fprintf(stderr, "    -f <FONT>: Specify the font.\n");
  fprintf(stderr, "    -d: Run in the demo mode.\n");
  fprintf(stderr, "    -h: Show this help and quit\n");
  exit(1);
}

static void
parse_arg(Lvns *lvns, int argc, char *argv[])
{
    int c;

    lvns->version = 1;      /* the default is 'sizuku' */

#ifdef FONT_PLUS
    lvns->font_file = "PAK FILE"; /* the default font file */
#endif /* FONT_PLUS */

#ifdef FONT_PLUS
    while ((c = getopt(argc, argv, "v:dhn:f:")) > 0) {
#else /* FONT_PLUS */
    while ((c = getopt(argc, argv, "v:dhn:")) > 0) {
#endif /* FONT_PLUS */
      switch (c) {
      case 'v':
	fprintf(stderr, "LVNS version: %s\n", optarg);
	lvns->version = atoi(optarg);
	if (lvns->version < 1 || lvns->version > 3) {
	  fprintf(stderr, "Invalid LVNS version.\n");
	  exit(1);
	}
	break;

#ifdef FONT_PLUS
      case 'f':
        if( strcmp( optarg, "system") == 0){
	    lvns->font_file = NULL;
        } else {
	    lvns->font_file = optarg;
        }
	break;
#endif /* FONT_PLUS */
        
      case 'd':
	fprintf(stderr, "Run in the demo mode.\n");
	lvns->demo_mode = True;
	break;

      case 'n':
        if(strcmp(optarg, "b") == 0){
	  fprintf(stderr, "Disable the effect on the background screen.\n");
	  lvns->enable_effect_back = False;
	}
        if(strcmp(optarg, "e") == 0){
	  fprintf(stderr, "Disable the effect.\n");
	  lvns->enable_effect = False;
	}
	break;

      case 'h':
	usage_exit();
	break;
      default:
	usage_exit();
      }
    }
}

/*
 * check and preare directories.
 */
static void
init_path(Lvns *lvns)
{
    char dir[PATH_LEN];
    char *d;
    struct stat sb;

    if (lvns->data_path) free(lvns->data_path);

    /*
     * user's home directory 
     */
    snprintf(dir, sizeof(dir), "%s/%s", getenv("HOME"), ".mglvns");
    if (stat(dir, &sb) < 0) {
	perror(dir);
	dprintf((stderr, "%s: made user directory.\n", dir));
	if (mkdir(dir, 0700) < 0) {
	    perror(dir);
	    exit(1);
	}
    } else if (!(sb.st_mode & S_IFDIR)) {
	dprintf((stderr, "%s: not directory.\n", dir));
	exit(1);
    }

    lvns->savedata_path = strdup(dir);

    /*
     * where leafpack archive is 
     */
    d = getenv("MGLVNS_APPDIR");
    if (!d) {
	d = MGLVNS_APPDIR;
    }

    lvns->data_path = strdup(d);

    dprintf((stderr, "user dir: %s\n", lvns->savedata_path));
    dprintf((stderr, "appdir: %s\n", lvns->data_path));
}

static Lvns *
initialize(int argc, char *argv[])
{
    Lvns *lvns;

    fprintf(stderr, VERSION);

    if (!open_graph()) {
	fprintf(stderr, "Can't open display.\n");
	exit(1);
    }
    set_color(COLOR_BLACK);
    mgl_set_keymode(MGL_SK_EXTRANSLATED);
    clear_screen();

    lvns = LvnsNew();
    parse_arg(lvns, argc, argv);
    init_path(lvns);
    mgLvnsInit(lvns);

    return lvns;
}

void
mgLvnsQuit(Lvns *lvns)
{
    LvnsJump(lvns, LVNS_JUMP_END);

    exit(1);
}

int
main(int argc, char *argv[])
{
    Lvns *lvns;

    lvns = initialize(argc, argv);

    LvnsMain(lvns);

    return 0;
}
