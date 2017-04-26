/*
Interface with EWMH-compatible window managers.

This file is part of alttab program.
alttab is Copyright (C) 2016-2017, by respective author (sa).
It is free software; you can redistribute it and/or modify it under
the terms of either:
a) the GNU General Public License as published by the Free Software
Foundation; either version 1, or (at your option) any later version, or
b) the "Artistic License".
*/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <X11/Xft/Xft.h>
#include "alttab.h"
#include "util.h"
extern Globals g;


// PUBLIC

//
// return the name of EWMH-compatible WM
// or NULL if not found
//
char* ewmh_getWmName (Display* dpy) {

Window* chld_win;
Window root;
char* r;
Atom utf8string;

chld_win = (Window*)NULL;
root = DefaultRootWindow (dpy);
if (! (chld_win = (Window *)get_x_property (dpy, root, XA_WINDOW, "_NET_SUPPORTING_WM_CHECK", NULL))) {
    if (! (chld_win = (Window *)get_x_property (dpy, root, XA_CARDINAL, "_WIN_SUPPORTING_WM_CHECK", NULL))) {
        return (char*)NULL;
    }
}

r = (char*)NULL;
utf8string = XInternAtom (dpy, "UTF8_STRING", False);
if (! (r = get_x_property (dpy, *chld_win, utf8string, "_NET_WM_NAME", NULL))) {
    (r = get_x_property (dpy, *chld_win, XA_STRING, "_NET_WM_NAME", NULL));
}

free (chld_win);
return r;
}

//
// initialize winlist/startNdx
// return 1 if ok
//
int ewmh_initWinlist (Display* dpy)
{
Window *client_list;
unsigned long client_list_size;
int i;
Window aw, root;
char *awp;
unsigned long sz;
char* title;

aw = (Window)0;
root = DefaultRootWindow (dpy);

if ((awp = get_x_property (dpy, root, XA_WINDOW, "_NET_ACTIVE_WINDOW", &sz))) {
    aw = *((Window*)awp);
    free(awp);
} else {
    if (g.debug>0) fprintf (stderr, "can't obtain _NET_ACTIVE_WINDOW\n");
    // not mandatory
}

if ((client_list = (Window *)get_x_property (dpy, root, XA_WINDOW, "_NET_CLIENT_LIST", &client_list_size)) == NULL) {
    if ((client_list = (Window *)get_x_property(dpy, root, XA_CARDINAL, "_WIN_CLIENT_LIST", &client_list_size)) == NULL) {
        fprintf (stderr, "can't get client list\n");
        return 0;
    }
}

for (i = 0; i < client_list_size / sizeof(Window); i++) {
    Window w = client_list[i];

    // build title
    char* wmn1 = get_x_property (dpy, w, XA_STRING, "WM_NAME", NULL);
    Atom utf8str = XInternAtom (dpy, "UTF8_STRING", False);
    char* wmn2 = get_x_property (dpy, w, utf8str, "_NET_WM_NAME", NULL);
    title = wmn2 ? strdup (wmn2) : 
        (wmn1 ? strdup (wmn1) : NULL);
    free(wmn1);
    free(wmn2);

    addWindowInfo (dpy, w, 0, 0, title);
    if (w == aw) { g.startNdx = i; }
}

if (g.debug>1) {fprintf(stderr, "ewmh active window: %lu index: %d name: %s\n", aw, g.startNdx, g.winlist[g.startNdx].name);}

return 1;
}

//
// focus window in EWMH WM
//
int ewmh_setFocus (Display* dpy, int winNdx)
{
Window root = DefaultRootWindow (dpy);
Window win = g.winlist[winNdx].id;
XEvent evt;
long rn_mask = SubstructureRedirectMask | SubstructureNotifyMask;

evt.xclient.window = win;
evt.xclient.type = ClientMessage;
evt.xclient.message_type = XInternAtom (dpy, "_NET_ACTIVE_WINDOW", False);
evt.xclient.serial = 0;
evt.xclient.send_event = True;
evt.xclient.format = 32;
if (!XSendEvent (dpy, root, False, rn_mask, &evt)) {
    fprintf (stderr, "ewmh_activate_window: can't send xevent\n");
}

XMapRaised (dpy, win);
return 1;
}

