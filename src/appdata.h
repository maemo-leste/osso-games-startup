/**
    @file appdata.h

    Data structures for the whole application.

    Copyright (c) 2004, 2005 Nokia Corporation.
	
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the
    Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

/* OSSO */
#include <libosso.h>

/* GTK */
#include <gtk/gtk.h>

/* Hildon */
#include <hildon/hildon.h>

/* GConf */
#include <gconf/gconf.h>
#include <gconf/gconf-client.h>

#ifndef APPDATA_H
#define APPDATA_H

#define __(String) dgettext("ke-recv", String)

#include "startup_config.h"
#include "startup_app.h"
#include "ui/startup_plugin.h"

struct _StartupPluginPrivate {
  GModule *handle;
};

struct _StartupConfigPrivate 
{
  gboolean initialized;
  gchar *current_group;
  GHashTable *hash_group;
  GHashTable *current_hash_entry;
};

/*
  Application UI data

  This structure should contain ALL application UI related data, which
  otherwise would be impossible to pass with events to callbacks. It makes
  complicated intercommunication between widgets possible.

  So when you add a widget to ui, put a pointer to it inside this struct.
*/

typedef struct _AppUIData AppUIData;
struct _AppUIData {
  HildonProgram *app;
  HildonWindow *main_view;
  HildonAppMenu *main_menu_bar;
  GtkWidget *main_toolbar;
  GtkWidget *main_vbox;
  GtkWidget *game_vbox;
  GtkWidget *game_hbox;
  GtkWidget *game_image;
  GtkWidget *controls_hbox;
  GtkWidget *controls_state_label;
  GtkWidget *play_button;
  GtkWidget *continue_button;
  GtkWidget *restart_button;
  GtkWidget *quit_button;
  GtkWidget *game_restart_menu;
  GtkWidget *game_play_menu;
  GtkWidget *game_game_menu;
  GtkWidget *game_save_menu;
  GtkWidget *game_saveas_menu;
  
  /* Menu Items */
  GtkWidget *play_item;
  GtkWidget *open_item;

 /*No free memory dialog*/
  GtkWidget *nomem_dialog;
  
 /*No free memory dialog*/
  GtkWidget *active_dialog;
  
  guint timeout;
  gchar *mmc_uri;
  gboolean is_mmc;
  gchar *game_title;
  gboolean async_command_sending;
  gboolean low_memory;
  gboolean got_pause;
  gboolean window_front_needed;
  guint timeouts;
};

/*
  Application data structure. Pointer to this is passed eg. with UI
  event callbacks.
*/
typedef struct _AppData AppData;
struct _AppData {
  AppUIData *app_ui_data;
  GConfClient *gc_client;
  StartupApp *startup_app;
  StartupPlugin *plugin;
};

/* reference to the global appdata */
extern AppData *app_data;

#endif /* APPDATA_H */
