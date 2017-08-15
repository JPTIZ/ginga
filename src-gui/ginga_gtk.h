/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Ginga is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef GINGA_GTK_H
#define GINGA_GTK_H

#include "ginga.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <stdlib.h>
#include <string.h>

#define BUTTON_SIZE 40

typedef struct
{
  guint8 aspectRatio = 0; /* 0=(4:3) 1=(16:9) 2=(16:10) */
  guint16 resolutionWidth = 850;
  guint16 resolutionHeight = 500;
  guint8 frameRate = 0; /* 0=30 1=60 2=Free  */
  gchar *lastFileName = NULL;
} PresentationAttributes;

extern GList *historic_list;

extern Ginga *GINGA; /* Ginga Scheduler */
extern GtkWidget *mainWindow;
extern GtkWidget *tvcontrolWindow;
extern GtkWidget *fullscreenWindow;
extern GtkWidget *settingsWindow;
extern GtkWidget *aboutWindow;

extern GtkWidget *debugView;
extern GtkWidget *sideView;
extern GtkWidget *infoBar;
extern GtkWidget *gingaView;
extern gchar *executableFolder;
extern gboolean isFullScreenMode;
extern gboolean isDebugMode;
extern gboolean inBigPictureMode;
extern gboolean tvcontrolAsSidebar;
extern gboolean isCrtlModifierActive;
extern PresentationAttributes presentationAttributes;

/* View/MainWindow */
void create_main_window (void);
void destroy_main_window (void);
void enable_disable_debug (void);
void select_ncl_file_callback (GtkWidget *widget, gpointer data);
void play_pause_button_callback (void);
void stop_button_callback (void);
void keyboard_callback (GtkWidget *widget, GdkEventKey *e, gpointer type);

/* View/TvControlWindow */
void create_tvcontrol_window (void);
void destroy_tvcontrol_window (void);
GtkWidget *create_tvremote_buttons (guint, guint);

/* View/FullscreenWindow */
void create_fullscreen_window (void);
void destroy_fullscreen_window (void);
void set_fullscreen_mode (void);
void set_unfullscreen_mode (void);

/* View/BigPictureWindow */
void create_bigpicture_window (void);
void destroy_bigpicture_window (void);
void carrousel_rotate (gint);

/* View/AboutWindow */
void create_about_window (void);
void destroy_about_window (void);

/* View/Draw */
#if GTK_CHECK_VERSION(3, 8, 0)
gboolean update_draw_callback (GtkWidget *widget,
                               GdkFrameClock *frame_clock,
                               G_GNUC_UNUSED gpointer data);
#else
gboolean update_draw_callback (GtkWidget *widget);
#endif
void draw_callback (GtkWidget *widget, cairo_t *cr, gpointer data);

/* Model/SaveLoadSettings */
void save_settings (void);
void load_settings (void);

/* Model/CheckUpdates */
void check_updates (void);

#endif /* GINGA_GTK_H */