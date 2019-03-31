/**
    @file startup_communication.h

    Prototypes for Communication.

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

#ifndef STARTUP_COMMUNICATION_H
#define STARTUP_COMMUNICATION_H

#include <glib.h>
#include <libosso.h>
#include <log-functions.h> 
#include <hildon/hildon-banner.h>

#include "dbus.h"
#include "startup_gamemanager.h"
#include "startup_state.h"

#define GAME_RUN_METHOD      "game_run"
#define GAME_CONTINUE_METHOD "game_continue"
#define GAME_RESTART_METHOD  "game_restart"
#define GAME_CLOSE_METHOD    "game_close"
#define GAME_PAUSE_METHOD    "game_pause"
#define GAME_HOME_METHOD    "game_home"
#define GAME_END_METHOD      "game_end"
#define GAME_SAVE_METHOD     "game_save"
#define GAME_LOAD_METHOD     "game_load"
#define GAME_CRASHED_METHOD  "game_crashed"
#define GAME_MIME_OPEN 	     "mime_open"

/**
 Handles incoming D-BUS message from Game Screen

 @param method The method that was called.
 @param arguments A GArray of osso_rpc_t structures. 
 @param data An application specific pointer.
 @param retval The return value of the method.
 @returns osso_rpc_type_t value
 */
gint handle_game_message(const gchar *method, GArray *arguments,
  gpointer data, osso_rpc_t *retval);

/**
 Send D-BUS message to Game Screen to set its state.

 @param state State to be set
 @returns TRUE on success, FALSE otherwise
 */
gboolean set_game_state(const guint state, osso_rpc_t *retval);

/**
 Asyncronous rpc return handler.
 
 @param interface The interface of the called method.
 @param method The method that was called.
 @param retval.The value that was returned.
 @param data An application specific pointer.
 */
void set_game_state_async_dbus_result(const gchar * interface,
                               const gchar * method,
                               osso_rpc_t * retval,
			       gpointer data);

/**
 Send D-BUS message to Game Screen to set its state.

 @param state State to be set
 @returns TRUE on success, FALSE otherwise
 */
gboolean set_game_state_async(const guint state);

#endif
