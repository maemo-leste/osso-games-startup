/**
    @file callbacks.c

    Provides callbacks for the user interface.

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

#include <gdk/gdkkeysyms.h>
#include <glib/gstdio.h>
#include <hildon/hildon-banner.h>
#include <hildon/hildon-helper.h>
#include <libgnomevfs/gnome-vfs.h>
#include <stdlib.h>
#include "callbacks.h"
#include "state_save.h"
#include "../appdata.h"

#define PAUSE_WITH_HW_KEY "/apps/osso/games_startup/pause_with_hw_key"
#define ENABLE_KEY_TIMEOUT 1000


static gboolean
enable_fullscreen(gpointer data)
{
    app_data->app_ui_data->timeouts = 0;
    app_data->app_ui_data->got_pause = FALSE;
    return FALSE;
}

void
undimm_everything(AppUIData * app)
{
    //gtk_widget_show(GTK_WIDGET(app->main_menu_bar));

    if (GTK_WIDGET_VISIBLE(app->play_button))
    {
        gtk_widget_set_sensitive(app->play_button, TRUE);
    }
    if (GTK_WIDGET_VISIBLE(app->continue_button))
    {
        gtk_widget_set_sensitive(app->continue_button, TRUE);
    }
    if (get_game_state() == gs_paused)
    {
        if (GTK_WIDGET_VISIBLE(app->restart_button))
        {
            gtk_widget_set_sensitive(app->restart_button, TRUE);
        }
    }
    if (app_data->plugin->info->plugin_cb)
    {
        app_data->plugin->info->plugin_cb(NULL, (gpointer) MA_GAME_PLAYING);
    }

    settings_set_bool(SETTINGS_CHESS_LOAD_IN_PROGRESS, FALSE);
}

void
dimm_everything(AppUIData * ad)
{
    gtk_widget_hide(GTK_WIDGET(ad->main_menu_bar));
    if (GTK_WIDGET_VISIBLE(ad->play_button))
    {
        gtk_widget_set_sensitive(ad->play_button, FALSE);
    }
    if (GTK_WIDGET_VISIBLE(ad->continue_button))
    {
        gtk_widget_set_sensitive(ad->continue_button, FALSE);
    }
    if (GTK_WIDGET_VISIBLE(ad->restart_button))
    {
        gtk_widget_set_sensitive(ad->restart_button, FALSE);
    }
    if (app_data->plugin->info->plugin_cb)
    {
        app_data->plugin->info->plugin_cb(NULL, (gpointer) MA_GAME_PLAYING_START);
    }

    settings_set_bool(SETTINGS_CHESS_LOAD_IN_PROGRESS, TRUE);
}

void
save_nomem_response(GtkDialog * dialog, gint arg1, gpointer user_data)
{
    AppUIData *app_ui_data = (AppUIData *) user_data;
    gtk_widget_destroy(app_ui_data->nomem_dialog);
    app_ui_data->nomem_dialog = NULL;
}

gboolean
dialog_key_press(GtkWidget * widget, GdkEventKey * event, gpointer data)
{
    (void) data;

    if (event->state & (GDK_CONTROL_MASK |
                        GDK_SHIFT_MASK |
                        GDK_MOD1_MASK |
                        GDK_MOD3_MASK | GDK_MOD4_MASK | GDK_MOD5_MASK))
    {
        return FALSE;
    }

    switch (event->keyval)
    {
        case GDK_Escape:
            gtk_dialog_response(GTK_DIALOG(widget), GTK_RESPONSE_CANCEL/*GAME_RESPONSE_CANCEL*/);
            return TRUE;
            break;
        default:
            break;
    }

    return FALSE;
}


gboolean
save_nomem_key_press(GtkWidget * widget, GdkEventKey * event, gpointer data)
{
    AppUIData *app_ui_data = (AppUIData *) data;

    if (event->state & (GDK_CONTROL_MASK |
                        GDK_SHIFT_MASK |
                        GDK_MOD1_MASK |
                        GDK_MOD3_MASK | GDK_MOD4_MASK | GDK_MOD5_MASK))
    {
        return FALSE;
    }

    switch (event->keyval)
    {
        case GDK_Escape:

            gtk_widget_destroy(GTK_WIDGET(widget));
            app_ui_data->nomem_dialog = NULL;
            return TRUE;
            break;
        default:
            break;
    }

    return FALSE;
}


/* The following function is from INDT's games-startup code. The copyrights
 * belongs to them. The INDT's games-startup was released under LGPL license. 
 * This function is modified under LGPL license */

/* Callback function when a game returns */
void
startup_ui_top_cb(StartupApp *app, gpointer data)
{
    AppUIData *ui = (AppUIData *) (data);
    gtk_window_present(GTK_WINDOW(ui->main_view));
}

/* The following function is from INDT's games-startup code. The copyrights
 * belongs to them. The INDT's games-startup was released under LGPL license. 
 * This function is modified under LGPL license */

/* RPC event handler for indt games */
gint
startup_app_rpc_event_cb(const gchar * interface,
                         const gchar * method,
                         GArray * args, gpointer data, osso_rpc_t * retval)
{
    AppUIData *app;
    GameState oldstate;
    osso_rpc_t val;
    g_debug("%s\n", __FUNCTION__);
    g_return_val_if_fail(data != NULL, OSSO_ERROR);
    g_return_val_if_fail(method != NULL, OSSO_ERROR);
    g_return_val_if_fail(retval != NULL, OSSO_ERROR);

    app = app_data->app_ui_data;

    retval->type = DBUS_TYPE_BOOLEAN;
    retval->value.b = TRUE;

    oldstate = get_game_state();
    /* Handle pause method */
    if (g_ascii_strcasecmp(method, GAME_PAUSE_METHOD) == 0)
    {
        int with_hw_key = settings_get_int(PAUSE_WITH_HW_KEY);
        g_debug("%s: Got pause message\n", __FUNCTION__);
        app->got_pause = TRUE;
        if ((with_hw_key == 286) || (with_hw_key == GDK_F5))
        {
            gtk_window_iconify(GTK_WINDOW(app->main_view));
            settings_set_int(PAUSE_WITH_HW_KEY, 0);
            g_debug("%s: ..with home key\n", __FUNCTION__);
        }
        else
        {
            gtk_window_present(GTK_WINDOW(app->main_view));
        }
        set_gamestate(gs_paused);
        if (GTK_WIDGET_VISIBLE(app->play_button))
        {
            g_debug("%s: setting focus to play button\n",__FUNCTION__);
            gtk_widget_grab_focus(app->play_button);
        }
        if (GTK_WIDGET_VISIBLE(app->continue_button))
        {
            g_debug("%s: setting focus continue button\n",__FUNCTION__);
            gtk_widget_grab_focus(app->continue_button);
        }
        if (app_data->plugin->info->plugin_cb)
        {
            app_data->plugin->info->plugin_cb(NULL, (gpointer) MA_GAME_CHECKSTATE);
        }
        undimm_everything(app);
    }
    else if (g_ascii_strcasecmp(method, GAME_CLOSE_METHOD) == 0 ||
             g_ascii_strcasecmp(method, GAME_CRASHED_METHOD) == 0 ||
             g_ascii_strcasecmp(method, GAME_END_METHOD) == 0)
    {
        g_debug("%s: Got end message\n", __FUNCTION__);
        int with_hw_key = settings_get_int(PAUSE_WITH_HW_KEY);
        if ((with_hw_key == 286) || (with_hw_key == GDK_F5))
        {
            gtk_window_iconify(GTK_WINDOW(app->main_view));
            settings_set_int(PAUSE_WITH_HW_KEY, 0);
            g_debug("%s: ..with home key\n", __FUNCTION__);
        }
        set_gamestate(gs_closed);
        if (app_data->plugin->info->plugin_cb)
        {
            app_data->plugin->info->plugin_cb(NULL, (gpointer) MA_GAME_CHECKSTATE);
            app_data->plugin->info->plugin_cb(NULL, (gpointer) MA_GAME_ENDED);
        }
        undimm_everything(app);
    }
    else /* Handle home method */
    if (g_ascii_strcasecmp(method, GAME_HOME_METHOD) == 0)
    {
        gtk_window_iconify(GTK_WINDOW(app->main_view));
        set_gamestate(gs_paused);
        if (GTK_WIDGET_VISIBLE(app->play_button))
        {
            gtk_widget_grab_focus(app->play_button);
        }
        if (GTK_WIDGET_VISIBLE(app->continue_button))
        {
            gtk_widget_grab_focus(app->continue_button);
        }
        if (app_data->plugin->info->plugin_cb)
        {
            app_data->plugin->info->plugin_cb(NULL, (gpointer) MA_GAME_CHECKSTATE);
        }
    }
    else /* Handle Load method */
	    if (g_ascii_strcasecmp(method, GAME_MIME_OPEN) == 0)
	    {
		    GtkWidget *widget = NULL;
		    osso_rpc_t *array = &g_array_index(args, osso_rpc_t, (guint) 0);

		    if(array) {
			    val.type = array->type;
			    val.value.s = g_strdup(array->value.s);
			    
			    if (app_data->plugin && app_data->plugin->info->get_ui_cb) {
				    widget = app_data->plugin->info->get_ui_cb();
			    }

			    /* Rama - Added for Bug#105609 */
			    if (widget) {
				    gtk_widget_destroy(widget);
			    }
			    if (app_data->app_ui_data->active_dialog) {
				    gtk_widget_destroy(app_data->app_ui_data->active_dialog);
			    }
			    g_debug("Loading game\n");
			    if ((val.type == DBUS_TYPE_STRING) && (val.value.s != NULL))
			    {
				    game_mime_open((gchar *) val.value.s);

				    /* success */
				    retval->type = DBUS_TYPE_BOOLEAN;
				    retval->value.b = TRUE;
				    return OSSO_OK;
			    }
			    else
			    {
				    retval->type = DBUS_TYPE_BOOLEAN;
				    retval->value.b = FALSE;
			    }
		    }
		    else {
			    return OSSO_ERROR;
		    }
	    }
	    else
	    {
		    retval->value.b = FALSE;
		    return OSSO_ERROR;
	    }

    if (app_data->startup_app->state_change_cb)
    {
	    app_data->startup_app->state_change_cb(app_data->startup_app, oldstate, get_game_state(),
			    app_data->startup_app->state_change_cb_data);
    }
    return OSSO_OK;
}

/* The following function is from INDT's games-startup code. The copyrights
 * belongs to them. The INDT's games-startup was released under LGPL license. 
 * This function is modified under LGPL license */

/* top event cb for indt games */
void
startup_app_top_event_cb(const gchar * arguments, gpointer data)
{
    StartupApp *app = (StartupApp *) data;
    g_return_if_fail(app != NULL);
    
    if (app->top_cb)
        app->top_cb(app, app->top_cb_data);
}

/* The following function is from INDT's games-startup code. The copyrights
 * belongs to them. The INDT's games-startup was released under LGPL license. 
 * This function is modified under LGPL license */

/* The callback function to close the application */
void
startup_ui_game_quit_cb(GtkWidget * widget, gpointer data)
{
    g_debug("%s", __FUNCTION__);

    AppUIData *ui = app_data->app_ui_data;

    if (ui->timeouts)
    {
        g_source_remove(ui->timeouts);
    }
    if (ui->async_command_sending == TRUE)
    {
        set_game_state_async(GAME_CLOSE);
    }

    if (app_data->plugin && app_data->plugin->info->write_config) {
        app_data->plugin->info->write_config();
    }

    if (app_data->plugin && app_data->plugin->send_game_close)
    {
        if (get_game_state() == gs_running || get_game_state() == gs_paused)
        {
            set_game_state_async(GAME_CLOSE);
        }
    }
    if (app_data->plugin && app_data->plugin->info->unload) {
        app_data->plugin->info->unload();
    }
    /* if exiting cleanly, clear state */
    clear_state();
}

/* The following function is from INDT's games-startup code. The copyrights
 * belongs to them. The INDT's games-startup was released under LGPL license. 
 * This function is modified under LGPL license */

/* The callback function to restart the game. It is connected to the reset
 * button */
void
startup_ui_game_restart_cb(GtkWidget * widget, gpointer data)
{
    AppUIData *ui = app_data->app_ui_data;
    
    if (ui->low_memory)
    {
        show_infoprint(__("memr_ib_operation_disabled"));
        return;
    }
    if (get_game_state() == gs_paused)
    {
        gchar *restart_needed = NULL;
        restart_needed =
            (gchar *) startup_config_read_entry(app_data->startup_app->config,
                                                "ShowRestartDialog", NULL);
        if (restart_needed)
        {
            if (g_ascii_strncasecmp(restart_needed, "FALSE", 6) == 0)
            {
                if (app_data->plugin->info->plugin_cb)
                {
                    app_data->plugin->info->plugin_cb(
                        NULL, (gpointer) MA_GAME_RESTORE_LAST_SETTINGS);
                }
                if (app_data->plugin && app_data->plugin->info->write_config)
                    app_data->plugin->info->write_config();

                g_set_application_name(ui->game_title);
                if (app_data->plugin->info->plugin_cb)
                {
                    app_data->plugin->info->plugin_cb(widget,
                                                (gpointer)
                                                MA_GAME_PLAYING_START);
                }
                set_game_state_async(GAME_RESTART);
                set_gamestate(gs_running);
                return;
            }
        }
        if (ui_show_restart_game_dialog(ui) == GTK_RESPONSE_OK/*GAME_RESPONSE_OK*/)
        {
            if (app_data->plugin->info->plugin_cb)
            {
               app_data->plugin->info->plugin_cb(
                    NULL, (gpointer) MA_GAME_RESTORE_LAST_SETTINGS);
            }
            if (app_data->plugin && app_data->plugin->info->write_config)
                app_data->plugin->info->write_config();

            g_set_application_name(ui->game_title);
            if (app_data->plugin->info->plugin_cb)
            {
                app_data->plugin->info->plugin_cb(widget,
                                            (gpointer) MA_GAME_PLAYING_START);
            }
            set_game_state_async(GAME_RESTART);
            set_gamestate(gs_running);
        }
    }
}

/* The following function is from INDT's games-startup code. The copyrights
 * belongs to them. The INDT's games-startup was released under LGPL license. 
 * This function is modified under LGPL license */

/* The callback function to start the INDT game. It is connected to the play
 * button */
void
startup_ui_game_play_cb(GtkWidget * widget, gpointer data)
{
    AppUIData *ui = app_data->app_ui_data;
    if (ui->low_memory)
    {
        show_infoprint(__("memr_ib_operation_disabled"));
        return;
    }

    /* remove stale process guard event source */
    g_source_remove(app_data->startup_app->stale_process_guard);
	
	//First call the plugin with MA_GAME_RESET_OLD_SETTINGS to display the message
	//"New settings will come into effect" if needed, and after that write the configuration
    if (get_game_state() == gs_paused)
    {
        if (app_data->plugin->info->plugin_cb)
        {
            app_data->plugin->info->plugin_cb(widget,
                                        (gpointer)
                                        MA_GAME_RESET_OLD_SETTINGS);
        }
    }
    if (app_data->plugin && app_data->plugin->info->write_config)
    {
        app_data->plugin->info->write_config();
    }

    if (app_data->plugin->info->plugin_cb)
    {
        app_data->plugin->info->plugin_cb(widget, (gpointer) MA_GAME_PLAYING_START);
    }
    if (get_game_state() == gs_paused)
    {
        set_game_state_async(GAME_CONTINUE);
    }
    else
    {
        set_game_state_async(GAME_RUN);
    }
    set_gamestate(gs_running);
}

static void
set_save_menu_state(gboolean state)
{
    g_debug("%s\n", __FUNCTION__);
    AppUIData *ui = app_data->app_ui_data;
    if (state == TRUE)
    {                           /* Check, if the file is read-only, or not */
        gchar *file = NULL;
        settings_get_string(SETTINGS_CHESS_SAVE_FILE);
        gchar *directory_uri = NULL, *last_slash = NULL;
        gboolean dir_ok = TRUE;
        GnomeVFSResult vfs_res = GNOME_VFS_OK;

        file = settings_get_string(SETTINGS_CHESS_SAVE_FILE);
        if (file != NULL) {
            GnomeVFSFileInfo *file_info = gnome_vfs_file_info_new();
            GnomeVFSFileInfo *dir_info = gnome_vfs_file_info_new();
            if (file_info != NULL && dir_info != NULL) {
        
                g_debug("%s  44\n", __FUNCTION__);
                g_debug("Fname: %s\n", file);
                
                vfs_res = file ? gnome_vfs_get_file_info(file, file_info, GNOME_VFS_FILE_INFO_FOLLOW_LINKS) : GNOME_VFS_ERROR_GENERIC;
                if (vfs_res == GNOME_VFS_OK) {
                    if ((last_slash = g_strrstr(file, "/")) != NULL) {
                        directory_uri = g_strndup(file, last_slash - file);
                    }
                    vfs_res = directory_uri ? gnome_vfs_get_file_info(directory_uri, dir_info, GNOME_VFS_FILE_INFO_FOLLOW_LINKS) : GNOME_VFS_ERROR_GENERIC;
                    
                    if (directory_uri && (g_strncasecmp(directory_uri, "obex", 4) != 0)) {
                        dir_ok = (dir_info->type == GNOME_VFS_FILE_TYPE_DIRECTORY) && (dir_info->permissions & GNOME_VFS_PERM_USER_WRITE);
                    } else {
                        dir_ok = TRUE;
                    }
                    if (((strlen(file) <= 1) || ((vfs_res == GNOME_VFS_OK) &&
                                                (file_info->type == GNOME_VFS_FILE_TYPE_REGULAR) &&
                                                (file_info-> permissions & GNOME_VFS_PERM_USER_WRITE) && 
                                                dir_ok))
			&& state) {
                        gtk_widget_show(ui->game_save_menu);
                    } else {
                        gtk_widget_hide(ui->game_save_menu);
                    }
                    if (file) {
                        g_free(file);
                        file = NULL;
                    }
                    if (directory_uri)
                    {
                        g_free(directory_uri);
                        directory_uri = NULL;
                    }
                }
                gnome_vfs_file_info_unref(dir_info);
                gnome_vfs_file_info_unref(file_info);
            }
        }
    } else {
        gtk_widget_hide(ui->game_save_menu);
    }
}

/* The following function is from INDT's games-startup code. The copyrights
 * belongs to them. The INDT's games-startup was released under LGPL license. 
 * This function is modified under LGPL license */

void
startup_ui_state_change_cb(StartupApp * app,
                           guint oldstate, guint newstate, gpointer data)
{
    g_debug("%s\n", __FUNCTION__);
    AppUIData *ui = app_data->app_ui_data;
    g_assert(ui);
    switch (newstate)
    {
        case gs_unloaded:
            game_state_changed(gs_unloaded);
            break;

        case gs_paused:
            ui->timeouts =
                g_timeout_add(ENABLE_KEY_TIMEOUT, enable_fullscreen, app_data);
            game_state_changed(gs_paused);
            if (app_data->plugin)
                if (app_data->plugin->info)
                    if (app_data->plugin->info->plugin_cb)
                        app_data->plugin->info->plugin_cb(NULL,
                                                    (gpointer)
                                                    MA_GAME_PLAYING);

            gtk_widget_set_sensitive(ui->game_restart_menu, TRUE);
            if (ui->game_save_menu && ui->game_saveas_menu)
            {
                set_save_menu_state(TRUE);
                gtk_widget_show(ui->game_saveas_menu);
            }
            if (app_data->plugin && app_data->plugin->info->update_menu)
                app_data->plugin->info->update_menu();
            break;

        case gs_closed:
            game_state_changed(gs_closed);
            if (app_data->plugin && app_data->plugin->info->update_menu)
                app_data->plugin->info->update_menu();
            break;

        default:
            break;
    }
    set_gamestate(newstate);
}

/* The following function is from INDT's games-startup code. The copyrights
 * belongs to them. The INDT's games-startup was released under LGPL license. 
 * This function is modified under LGPL license */

/* callback for the menu */
void
startup_ui_menu_action_cb(gpointer data, guint action, GtkWidget * menu_item)
{
    AppUIData *ui = app_data->app_ui_data;

    switch (action)
    {
        case MA_GAME_PLAY:
            startup_ui_game_play_cb(menu_item, ui);
            break;

        case MA_GAME_RESTART:
            startup_ui_game_restart_cb(menu_item, ui);
            break;

        case MA_GAME_CLOSE:
            game_quit_cb(menu_item, ui);
            break;
    }
}

void
menu_set_save_dimmed(AppUIData * app_ui_data, gboolean dimmed)
{
    if (app_data->plugin && app_data->plugin->menu_open_save)
    {
      if (dimmed) {
        gtk_widget_show(app_ui_data->game_save_menu);
        gtk_widget_show(app_ui_data->game_saveas_menu);
      } else {
        gtk_widget_hide(app_ui_data->game_save_menu);
        gtk_widget_hide(app_ui_data->game_saveas_menu);
      }	
    }
}

gboolean
game_quit_cb(GtkWidget * widget, gpointer data)
{
    /* Don't let the user close startup while loading in progress. It will
     * restart on game-pause anyway... */
    if (settings_get_bool(SETTINGS_CHESS_LOAD_IN_PROGRESS))
    {
        return TRUE;
    }
    if (app_data->plugin)
    {
        startup_ui_game_quit_cb(NULL, NULL);
    }
    gtk_widget_hide(GTK_WIDGET(app_data->app_ui_data->main_view));
    gtk_main_iteration();
    gtk_main_quit();
    return TRUE;
}

static gboolean
check_and_kill_game(gpointer data)
{
        gchar *cmd = data;
        
        if (get_game_state() == gs_running)
        {
                /* maemoblocks or marbles is running in the background
                 * we kill it, the sigterm handler will do state saving
                 * in the killed process */
                gchar *killcmd;
                g_debug("%s still running, killing it", cmd);
                killcmd = g_strdup_printf("/usr/bin/killall %s", cmd);
                system(killcmd);
                g_free(killcmd);
        }

        return FALSE;
}

void
top_changed(GObject * self, GParamSpec * property_param, gpointer data)
{
    HildonProgram *program = HILDON_PROGRAM(self);
    if (hildon_program_get_is_topmost(program))
    {
        g_debug("%s\n", __FUNCTION__);
        hildon_program_set_can_hibernate(app_data->app_ui_data->app,
                                         FALSE);

            /* handle situation when the game process is effectivily 
             * running. It can occur when Others/Home menu is opened on the top
             * of the game window and application was chosen
             *
             * Schedule a function which checks if maemoblocks still runs
             * and kills it.
             */
            if (strstr(app_data->startup_app->service, "maemoblocks"))
            {
                    app_data->startup_app->stale_process_guard = 
                            g_timeout_add(500, check_and_kill_game, 
                                            "maemoblocks");
            }
            else if (strstr(app_data->startup_app->service, "osso_lmarbles"))
            {
                app_data->startup_app->stale_process_guard = 
                    g_timeout_add(500, check_and_kill_game, 
                            "osso_lmarbles");
            }

    }
    else
    {
        save_state();
        hildon_program_set_can_hibernate(app_data->app_ui_data->app, TRUE);
    }

    if (GTK_WIDGET_VISIBLE(app_data->app_ui_data->play_button))
    {
        gtk_widget_grab_focus(app_data->app_ui_data->play_button);
    }
    if (GTK_WIDGET_VISIBLE(app_data->app_ui_data->continue_button))
    {
        gtk_widget_grab_focus(app_data->app_ui_data->continue_button);
    }
}

gboolean take_screen_shot(gpointer widget)
{
	GtkWidget *window = (GtkWidget *)widget;
        gchar *filename = NULL;
        gchar *service = g_strconcat(game_get_service(), ".pvr", NULL);

        filename = g_build_filename((const gchar*)"/home/user/.cache/launch/", service,
		                                           NULL);
	if(FALSE == g_file_test(filename, G_FILE_TEST_EXISTS)) {
		hildon_gtk_window_take_screenshot(GTK_WINDOW(window), TRUE);
	}

	return FALSE;	

}

void
game_get_screenshot(GtkWidget * widget, GdkEventKey * event, gpointer data)
{
	g_idle_add((GSourceFunc)(take_screen_shot), (gpointer)(widget));
}

void
game_play_cb(GtkWidget * widget, gpointer data)
{
    if (app_data->app_ui_data->low_memory)
    {
        show_infoprint(__("memr_ib_operation_disabled"));
        return;
    }
    startup_ui_game_play_cb(widget, data);
}

void
game_continue_cb(GtkWidget * widget, gpointer data)
{
    g_debug("%s\n", __FUNCTION__);
    if (app_data->app_ui_data->low_memory)
    {
        show_infoprint(__("memr_ib_operation_disabled"));
        return;
    }

    startup_ui_game_play_cb(NULL, NULL);
}

void
game_restart_cb(GtkWidget * widget, gpointer data)
{
    if (app_data->app_ui_data->low_memory)
    {
        show_infoprint(__("memr_ib_operation_disabled"));
        return;
    }
    startup_ui_game_restart_cb(widget, data);
}

/* Set restart option state in menu */
void
menu_set_restart_dimmed(AppUIData * app_ui_data, gboolean mode)
{
    gtk_widget_set_sensitive(app_ui_data->game_restart_menu, mode);
}


gboolean
key_press(GtkWidget * widget, GdkEventKey * event, gpointer data)
{
    AppUIData *app_ui_data = (AppUIData *) data;
    GameState gs;
    if (event->state & (GDK_CONTROL_MASK |
                        GDK_SHIFT_MASK |
                        GDK_MOD1_MASK |
                        GDK_MOD3_MASK | GDK_MOD4_MASK | GDK_MOD5_MASK))
    {
        return FALSE;
    }

    switch (event->keyval)
    {
        case GDK_F6:
            if (settings_get_bool(SETTINGS_CHESS_LOAD_IN_PROGRESS))
            {
                return TRUE;
            }
            gs = get_game_state();
            if (!app_ui_data->got_pause)
            {
                if ((gs == gs_unloaded) || (gs == gs_closed)
                    || (gs == gs_paused))
                {
                    startup_ui_game_play_cb(NULL, NULL);
                }
                return TRUE;
            }
            break;
        case GDK_Escape:
            return FALSE;
            break;
        case GDK_F5:
            if (app_data->plugin->info->plugin_cb)
            {
                app_data->plugin->info->plugin_cb(NULL,
                                                     (gpointer)
                                                     MA_GAME_HOME_PRESSED);
            }
            return FALSE;
            break;

    }
    return FALSE;
}

void
show_infoprint(gchar * msg)
{
    if (app_data != NULL)
    {
        hildon_banner_show_information(GTK_WIDGET(app_data->app_ui_data->main_view),
                                       NULL, msg);
    }
}

/* Handles insensitive menu press events, display infoprint */
void
on_insensitive_menu_press(GtkWidget * widget, gchar * message)
{
    hildon_banner_show_information(GTK_WIDGET(app_data->app_ui_data->main_view), NULL,
                                   message);
}

/* Created */
gboolean
on_delete_event(GtkWidget * widget, gpointer data)
{
    g_assert(data);
    /* Rama #94754 - Dont handle delete event if game 
     * has started */
    if (gs_running == get_game_state())
    {
	    return TRUE;
    }

    game_clean_up_and_quit(widget, data);

    return TRUE;
}

void
game_clean_up_and_quit(GtkWidget * widget, gpointer data)
{
    game_quit_cb(widget, data);
}

/* Handles SIGTERM for background killing */
void
on_bgkill(int signal)
{
    /* if user is playing (or paused), remember that
     * we have to continue next time */

    GameState gs = get_game_state();

    if(app_data->app_ui_data->active_dialog != NULL)
    {
      gtk_widget_destroy(app_data->app_ui_data->active_dialog);
      app_data->app_ui_data->active_dialog = NULL;
    }	    

    app_data->plugin->info->plugin_cb(NULL, (gpointer) MA_GAME_BG_KILL);	
    
    if (gs == gs_running || gs == gs_paused) {
	save_state();
    }

    /* quit gracefully */
    
    gtk_widget_hide(GTK_WIDGET(app_data->app_ui_data->main_view));
    gtk_main_quit();
}

