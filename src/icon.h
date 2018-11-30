/*
icon.c definitions.

Copyright 2017-2018 Alexander Kulak.
This file is part of alttab program.

alttab is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

alttab is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with alttab.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ICON_H
#define ICON_H

#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include <uthash.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <err.h>
#include <fts.h>
#include <stdio.h>
#include <ctype.h>

#define MAXICONDIRS     64
#define MAXAPPLEN       64
#define MAXICONPATHLEN  1024

typedef struct {
    char app[MAXAPPLEN];        // application name; uthash key
    char src_path[MAXICONPATHLEN];  // \0 -if not initialized or loaded from X window properties
    unsigned int src_w, src_h;  // width/height of source (not resized) icon
    Pixmap drawable;            // resized (ready to use)
    Pixmap mask;
    bool drawable_allocated;    // we must free drawable (but not mask), because we created it
    UT_hash_handle hh;
} icon_t;

icon_t *initIcon();
void deleteIcon(icon_t * ic);
int initIconHash(icon_t ** ihash);
int updateIconsFromFile(icon_t ** ihash);   // load all icons into hash (no pixmaps, just path and dimension)
int inspectIconFile(FTSENT * pe);   // check if file pe has better icon than we have in g.ic
int loadIconContent(icon_t * ic);   // update drawable
icon_t *lookupIcon(char *app);  // search app icon in hash
bool iconMatchBetter(int new_w, int new_h, int old_w, int old_h);
void deleteIconHash(icon_t **ihash);

#endif
