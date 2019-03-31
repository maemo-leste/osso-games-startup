/**
    @file startup_gamemanager.h

    Prototypes for GameManager.

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

#ifndef STARTUP_GAMEMANAGER_H
#define STARTUP_GAMEMANAGER_H

#include <glib.h>
#include <string.h>
#include "startup_state.h"
#include "startup_communication.h"
#include "ui/interface.h"
#include "dbus.h"
#include "settings.h"


/* Game save result
 */
enum {
  GAME_SAVE_NONE=1,
  GAME_SAVE_SAVING,
  GAME_SAVE_OK,
  GAME_SAVE_ERR,
  GAME_SAVE_CANCEL
};

/* Game load result
 */
enum {
  GAME_LOAD_FILE_UNSUPPORTED=1,
  GAME_LOAD_FILE_NOT_FOUND
};

/* Init plugin structures from config file. */
void
init_game_from_config(const gchar *config_file);

/** Fill the plugin structure. Call it after ui is created and libosso is
 * initialized! */
void Fill_Plugin_Settings(void);

/**
   Sets the game state.
   @param state The state of the game.
*/
void set_gamestate(GameState state);

/**
 Called when state changed
 @param state New state
 */
void game_state_changed(GameState state);

/**
 Get game state
 @returns Game state
 */
GameState get_game_state(void);

/**
 Returns the service of current game
 @returns Service as string
 */
const gchar *game_get_service(void);

/**
 Returns the interface of current game
 @returns Interface as string
 */
const gchar *game_get_iface(void);

/**
 Returns the object path of current game
 @returns Object path as string
 */
const gchar *game_get_object_path(void);

void game_mime_open(gchar *filename);

/* Games tate global */

extern GameState game_state;

#endif
