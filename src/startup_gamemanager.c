/**
    @file startup_gamemanager.c

    Implements GameManager module.

    Copyright (c) 2004, 2005 Nokia Corporation.
    Parts of this file (c) 2004 INDT
	
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

#include <libgnomevfs/gnome-vfs.h>
#include <glib/gstdio.h>
#include "startup_gamemanager.h"

/* State information */
GameState game_state = gs_unloaded;

/* The following function is from INDT's games-startup code. The copyrights 
belongs to them. The INDT's games-startup was released under LGPL license*/

void 
startup_app_set_state_change_cb (StartupApp *app,
                                 startup_app_state_change_cb *state_change_cb,
                                 gpointer data)
{
  g_return_if_fail (app != NULL);
  app->state_change_cb = state_change_cb;
  app->state_change_cb_data = data;
}

/* The following function is from INDT's games-startup code. The copyrights 
belongs to them. The INDT's games-startup was released under LGPL license*/

void
startup_app_set_top_cb (StartupApp *app,
                        startup_app_top_cb *top_cb,
                        gpointer data)
{
  g_return_if_fail (app != NULL);
  app->top_cb = top_cb;
  app->top_cb_data = data;
}

/* The following function is from INDT's games-startup code. The copyrights 
belongs to them. The INDT's games-startup was released under LGPL license*/

/* Startup the plugin, and initialize the structure */
static StartupPlugin *
startup_plugin_load (const gchar *path)
{
  StartupPlugin *plugin;
  GModule *handle;
  gboolean (*startup_init_plugin) ();

  g_return_val_if_fail (path != NULL, NULL);

  handle = g_module_open (path, 0);
  g_assert (handle);

  if (!g_module_symbol (handle,
        "startup_init_plugin",
        (gpointer) &startup_init_plugin)) {
    g_module_close (handle);
    g_warning("Symbol can't be found");
    return NULL;
  }
  
  plugin = g_new0 (StartupPlugin, 1);
  g_assert (plugin);

  plugin->priv = g_new0 (StartupPluginPrivate, 1);
  g_assert (plugin->priv);
  plugin->priv->handle = handle;

  if (startup_init_plugin (plugin)) 
    return plugin;

  g_module_close (plugin->priv->handle);
  g_free (plugin->priv);
  g_free (plugin);
    
  return NULL;
}

/* The following function is from INDT's games-startup code. The copyrights 
belongs to them. The INDT's games-startup was released under LGPL license. 
This function is modified under LGPL license*/

/* Initialize the INDT specific settings */
static void
Initialize_INDT_settings(StartupApp *app)
{ 
  const gchar *plugin_path;

  startup_app_set_top_cb(app, startup_ui_top_cb, app);
  startup_app_set_state_change_cb(app, startup_ui_state_change_cb, app);
  
  plugin_path = startup_config_read_entry(startup_app_get_config(app), "PluginPath", NULL);

  if (plugin_path) {
    app_data->plugin = startup_plugin_load(plugin_path);
    app_data->plugin->gs->ui = g_new0(StartupUI, 1);
    app_data->plugin->gs->ui->app = app;
    app_data->plugin->gs->startup_ui_menu_action_cb = startup_ui_menu_action_cb;
    app_data->plugin->gs->startup_ui_state_change_cb = startup_ui_state_change_cb;
  } else 
    app_data->plugin = NULL;
}

void
init_game_from_config(const gchar *config_file)
{
  StartupApp *app = g_new0(StartupApp, 1);

  /* parse config */
  startup_app_set_config_file(app, config_file);
  
  /* set up StartupApp */
  app->service = (gchar *) startup_config_read_entry(app->config,
                                            "ServiceName", NULL);
  app->path = (gchar *) startup_config_read_entry(app->config,
                                            "PathName", NULL);
  app->iface = (gchar *) startup_config_read_entry(app->config,
                                            "InterfaceName", NULL);

  /* load the plugin, it fills app_data->plugin and plugin->gs */
  Initialize_INDT_settings(app);
  
  app_data->startup_app = app;
}

/* Fill the rest of plugin structures, after the ui
 * is constructed, and libosso is initialized.*/
void
Fill_Plugin_Settings(void)
{
  StartupUI *ui = app_data->plugin->gs->ui;
  
  ui->plugin = app_data->plugin;
  ui->hildon_app = app_data->app_ui_data->app;
  ui->hildon_appview = app_data->app_ui_data->main_view;
  ui->controls_state_label = app_data->app_ui_data->controls_state_label;
  ui->play_button = app_data->app_ui_data->play_button;
  ui->restart_button = app_data->app_ui_data->restart_button;
}

/* Game state changes, set state and update UI */
void game_state_changed(GameState state)
{
  game_state = state;
  ui_repaint(app_data);
  gtk_window_present(GTK_WINDOW(app_data->app_ui_data->main_view));
}

/* Set the game state */
void set_gamestate(GameState state)
{
  game_state = state;
}

/* Return game state */
GameState get_game_state(void)
{
  return game_state;
}

/* Return selected game object path */
const gchar *game_get_object_path(void)
{
  return app_data->startup_app->path;	
}

/* Return selected game object path */
const gchar *game_get_service(void)
{
  return app_data->startup_app->service;
}

/* Return selected game object path */
const gchar *game_get_iface(void)
{
  return app_data->startup_app->iface;
}

void game_mime_open(gchar *filename) 
{
    AppUIData *app_ui_data = app_data->app_ui_data;
    settings_set_bool(SETTINGS_CHESS_LOAD_IN_PROGRESS, TRUE);
	
    /* If loading game is not possible, just display some error message and
     * return */
    if (!app_data->plugin->menu_open_save)
    {
        gchar *filen = gnome_vfs_format_uri_for_display(filename);
        gchar *basename = g_path_get_basename(filen);
	gchar *temp = g_strrstr(basename, ".");
        if (temp) *temp = '\0';
        gchar *msg = g_strdup_printf(_("game_ni_unsupported_file%s"), basename);
	g_free(filen);
	g_free(basename);

        show_infoprint(msg);
        g_free(msg);
        settings_set_bool(SETTINGS_CHESS_LOAD_IN_PROGRESS, FALSE);
        return;
    }

    /* The loading takes too much time. Make insensitive all during the load */
    app_data->plugin->info->plugin_cb(NULL, (gpointer)MA_GAME_PLAYING_START);
    gtk_widget_set_sensitive(app_ui_data->play_button,FALSE);
    gtk_widget_hide(GTK_WIDGET(app_ui_data->main_menu_bar));

    while (gtk_events_pending())
        gtk_main_iteration();
    
    app_data->plugin->info->plugin_cb(NULL,(gpointer)MA_WAIT_FOR_SAVE);
    if ((get_game_state() != gs_running)) {
        /* Show startup screen */
        gtk_window_present(GTK_WINDOW(app_ui_data->main_view));

	while (gtk_events_pending())
            gtk_main_iteration();
    }
	
    if (get_game_state() == gs_running) {
        return;
    }
    
    settings_set_string(SETTINGS_CHESS_SAVE_FILE, filename);
    app_data->plugin->info->plugin_cb(NULL, (gpointer)MA_SETTING_RECENT_ITEMS);
    game_state = gs_running;
    app_data->plugin->info->plugin_cb(NULL, (gpointer)MA_LOAD_MIME);
    app_data->plugin->info->plugin_cb(NULL, (gpointer)MA_GAME_PLAYING);

    gtk_widget_show(GTK_WIDGET(app_ui_data->main_menu_bar));
}
