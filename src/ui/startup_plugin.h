/*
 * Games Startup
 * Copyright (c) 2005 INdT.
 * @author Andre Moreira Magalhaes <andre.magalhaes@indt.org.br>
 *
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

#ifndef STARTUP_PLUGIN_H
#define STARTUP_PLUGIN_H

#include <gtk/gtk.h>
#include <glib.h>
#include <gmodule.h>

#include <hildon/hildon-program.h>
#include <hildon/hildon-window.h>

#include <startup_app.h>

G_BEGIN_DECLS

typedef struct _StartupPlugin        StartupPlugin;
typedef struct _StartupPluginInfo    StartupPluginInfo;
typedef struct _StartupPluginPrivate StartupPluginPrivate;
typedef struct _GameStartupInfo      GameStartupInfo;
typedef struct _StartupUI            StartupUI;

gboolean startup_init_plugin(StartupPlugin *plugin);

struct _StartupPluginInfo {
  
  GtkWidget *  (* load)         (void);
  void         (* unload)       (void);
  void         (* write_config) (void);
  GtkWidget ** (* load_menu)    (guint *); 
  void         (* update_menu)  (void);
  void         (* plugin_cb)    (GtkWidget *menu_item, gpointer cb_data); 
  GtkWidget *  (* get_ui_cb)    (void); 
};

struct _StartupUI {
  StartupApp *app;
  StartupPlugin *plugin;
 
  HildonProgram *hildon_app;
  HildonWindow *hildon_appview;
  
  GtkItemFactory *item_factory;
  GtkWidget *controls_state_label;
  GtkWidget *play_button;
  GtkWidget *restart_button;
};

struct _GameStartupInfo {
  StartupUI *ui;
  void (* startup_ui_menu_action_cb) (gpointer   data, 
                                      guint      action, 
                                      GtkWidget *menu_item);
  void (* startup_ui_state_change_cb)(StartupApp *app, 
                                      guint       oldstate,
                                      guint       newstate,
                                      gpointer    data);
};

struct _StartupPlugin {
  StartupPluginInfo *info;
  gpointer data;                /* plugin specific data */

  GameStartupInfo *gs;
  gboolean send_game_close;
  gboolean menu_open_save;

  StartupPluginPrivate *priv;
  const gchar *gettext_package; 
};

#define STARTUP_INIT_PLUGIN(plugininfo, gs, close, open_save) \
    G_MODULE_EXPORT gboolean startup_init_plugin(StartupPlugin *plugin) { \
        plugin->info = &(plugininfo); \
        plugin->gs = &(gs); \
        plugin->send_game_close = close; \
        plugin->menu_open_save = open_save; \
        if (!plugin->info || !plugin->gs) \
          return FALSE; \
        return TRUE; \
    }

G_END_DECLS

#endif
