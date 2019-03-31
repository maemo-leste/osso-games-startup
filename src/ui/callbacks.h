/**
    @file callbacks.h

    Function definitions for user interface callbacks.

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

#ifndef CALLBACKS_H
#define CALLBACKS_H

/* GTK */
#include <gtk/gtk.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "interface.h"
#include "settings.h"
#include "startup_i18n.h"

#define STARTUP_UNTOP 50

/**
	Undimm everything, when it was dimmed before
	@param data Pointer to the AppUIData.
*/
void undimm_everything(AppUIData * app);

/**
	Dimm everything, because the memory is low
	@param data Pointer to the AppUIData.
*/
void dimm_everything(AppUIData * data);

/**
	Handle response events on the no-memory dialog.
	@param dialog Widget that received the event.
*/

void save_nomem_response(GtkDialog * dialog, gint arg1, gpointer user_data);


/**
	Handle key press events on the no-memory dialog.
	@param widget Widget that received the event.
	@param event The event that occurred.
	@param data Pointer to the AppUIData.
	@return TRUE if handled, FALSE otherwise.
*/
gboolean save_nomem_key_press(GtkWidget * widget, GdkEventKey * event,
                              gpointer data);

/**
	Handle key press events on dialog.
	@param widget Widget that received the event.
	@param event The event that occurred.
	@param data Pointer to the AppUIData.
	@return TRUE if handled, FALSE otherwise.
*/
gboolean dialog_key_press(GtkWidget * widget, GdkEventKey * event,
                          gpointer data);


/* Callback function when a game returns */
void
  startup_ui_top_cb(StartupApp * app, gpointer data);


/* The callback function to restart the game. It is connected to the reset
 * button */
void startup_ui_game_restart_cb(GtkWidget * widget, gpointer data);

/* D-bus event handler function for indt games */
gint startup_app_rpc_event_cb(const gchar * interface, const gchar * method,
                              GArray * args, gpointer data,
                              osso_rpc_t * retval);

/* top event cb for indt games */
void
  startup_app_top_event_cb(const gchar * arguments, gpointer data);


/** 
	   The callback function to close the application 
 */
void
  startup_ui_game_quit_cb(GtkWidget * widget, gpointer data);
/**
	 The callback function to start the INDT game. It is connected to the play button
*/
void startup_ui_game_play_cb(GtkWidget * widget, gpointer data);


/**
   The callback function to the state change
*/
void startup_ui_state_change_cb(StartupApp * app, guint oldstate,
                                guint newstate, gpointer data);

/**
   The callback function for the menu when a INDT game is running.
*/
void startup_ui_menu_action_cb(gpointer data, guint action,
                               GtkWidget * menu_item);

/**
   Callback top pop up our popupmenu

 */
gboolean show_popup(GtkWidget * widget, GdkEvent * event, GtkWidget * menu);

/**
  Sets save game dimmed or undimmed

  @param app_ui_data AppUIData structure
  @param dimmed TRUE to undim save options, FALSE to dim
 */
void menu_set_save_dimmed(AppUIData * app_ui_data, gboolean dimmed);


/**
  Callback for quitting game

  @param widget Caller widget
  @param data Extra data given by the caller
 */
gboolean game_quit_cb(GtkWidget * widget, gpointer data);

/**
  Callback for play game

  @param widget Caller widget
  @param data Extra data given by the caller
 */
void game_play_cb(GtkWidget * widget, gpointer data);

/**
  Callback for continue game

  @param widget Caller widget
  @param data Extra data given by the caller
 */
void game_continue_cb(GtkWidget * widget, gpointer data);

/**
  Callback for restart game

  @param widget Caller widget
  @param data Extra data given by the caller
 */
void game_restart_cb(GtkWidget * widget, gpointer data);

/**
  Set menu item Restart sensitivity.

  @param app_ui_data AppUIData structure
  @param data Extra data given by the caller
 */
void menu_set_restart_dimmed(AppUIData * app_ui_data, gboolean mode);

/**
 Handle key press events.

 @param widget Widget that received the event.
 @param event The event that occurred.
 @param data Pointer to the AppUIData.
 @return TRUE if handled, FALSE otherwise.
*/
gboolean key_press(GtkWidget * widget, GdkEventKey * event, gpointer data);

void top_changed(GObject * self, GParamSpec * property_param, gpointer data);

gboolean take_screen_shot(gpointer widget);

void game_get_screenshot(GtkWidget * widget, GdkEventKey * event, gpointer data);

void startup_close(GtkWidget * widget, gpointer data);


/**
	Handles insensitive menu press events.
	Shows given message in an infoprint.
	
	@param widget widget whose callback this is
	@param message message for infoprint
*/
void on_insensitive_menu_press(GtkWidget * widget, gchar * message);

/**
  Shows inforprint "Saved"/sfil_ib_saved (WID INF259)
 */
void show_infoprint(gchar * msg);

/**
    Handles delete event.
    
    @param widget Widget whose callback this is
    @return FALSE if user cancelled close; TRUE otherwise
*/
gboolean on_delete_event(GtkWidget * widget, gpointer data);

/**
    Clean up data and quit.
    
    @param widget Widget whose callback this is
*/
void game_clean_up_and_quit(GtkWidget * widget, gpointer data);

/** Handle background call signal. */
void on_bgkill(int signal);

#endif
