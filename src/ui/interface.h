/**
    @file interface.h

    Function prototypes and variable definitions for general user
    interface functionality.

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

#ifndef INTERFACE_H
#define INTERFACE_H

/* GTK */
#include <gtk/gtk.h>

/* Hildon */
#include <hildon/hildon-program.h>
#include <hildon/hildon-window.h>
#include <hildon/hildon-file-chooser-dialog.h>

/* Log */
#include <log-functions.h> 

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "startup_i18n.h"
#include "startup_gamemanager.h"
#include "startup_state.h"
#include "appdata.h"
#include "settings.h"

/* Include callback prototypes */
#include "callbacks.h"


#define GAME_IMAGE_HEIGHT 179
#define GAME_IMAGE_WIDTH -1
#define CTRL_BUTTON_WIDTH 200

#define MENU_RECENT_MENU_WIDTH 15

/* Dialog button */
#define GAME_RESPONSE_YES 1
#define GAME_RESPONSE_OK GAME_RESPONSE_YES
#define GAME_RESPONSE_NO 0
#define GAME_RESPONSE_CANCEL -1

/**
    Enum to define, when button switching is needed
*/
typedef enum {
    CHANGE_PLAY,    /** change to play button */
    CHANGE_CONTINUE /** change to continue button */
} ChangeButton;

/**
 High scores view column enumration
 */
enum
{
  HIGHSCORE_LEVEL_COLUMN,
  HIGHSCORE_TIME_COLUMN,
  HIGHSCORE_DATE_COLUMN
};

/* Menu callback data definitions */

/* menu_action identifiers */
#define MA_GAME_PLAY 1
#define MA_GAME_RESTART 2
#define MA_GAME_OPEN 3
#define MA_GAME_SAVE 4
#define MA_GAME_SAVE_AS 5
#define MA_GAME_HELP 6
#define MA_GAME_RECENT_1 7
#define MA_GAME_RECENT_2 8
#define MA_GAME_RECENT_3 9
#define MA_GAME_RECENT_4 10
#define MA_GAME_RECENT_5 11
#define MA_GAME_RECENT_6 12
#define MA_GAME_CLOSE 13
#define MA_GAME_HIGH_SCORES 14
#define MA_GAME_RESET 15
#define MA_GAME_CHECKSTATE 16
#define MA_GAME_SAVEMENU_REFERENCE 17
#define MA_GAME_BG_KILL     39


#define MR_GAME_HUMAN_WHITE 1
#define MR_GAME_HUMAN_BLACK 2
#define MR_GAME_OPPONENT_1 3
#define MR_GAME_OPPONENT_2 4
#define MR_GAME_OPPONENT_3 5
#define MR_GAME_OPPONENT_4 6
#define MR_GAME_OPPONENT_HUMAN 7
#define MR_GAME_BOARD_1 8
#define MR_GAME_BOARD_2 9
#define MR_GAME_BOARD_3 10
#define MR_GAME_BOARD_4 11
#define MR_GAME_BOARD_5 12
#define MR_GAME_BOARD_6 13
#define MR_GAME_BOARD_7 14
#define MR_GAME_BOARD_8 15
#define MR_GAME_LMARBLES_EASY 16
#define MR_GAME_LMARBLES_NORMAL 17
#define MR_GAME_LMARBLES_HARD 18
#define MR_GAME_LMARBLES_BRAINSTORM 19
/* menu_action identifiers for plugin*/
#define ME_GAME_OPEN     20
#define ME_GAME_SAVE     21
#define ME_GAME_SAVE_AS  22
#define MA_GAME_PLAYING_START 30
#define MA_GAME_PLAYING 31
#define MA_WAIT_FOR_SAVE 32
#define MA_SETTING_RECENT_ITEMS 33
#define MA_LOAD_MIME 34
#define MA_GAME_RESET_OLD_SETTINGS 35
#define MA_GAME_HOME_PRESSED 36
#define MA_GAME_ENDED 37
#define MA_GAME_RESTORE_LAST_SETTINGS 38

#define MT_GAME_SOUND 1
#define MT_GAME_LEGAL_MOVES 2

/**
   Switches the play/continue button
   
   @param app_data  AppData
   @param next_button   The button to change to   
*/
void
ui_change_play_continue_button(AppData * app_data,ChangeButton next_button);

/**
   Creates and shows the INDT specific menu
   
   @param ui The structure what holds the informations
*/
void ui_create_indt_menu (AppUIData *ui);

/**
  Create the main "window". Initializes the application UI.
  @return Pointer to AppUIData structure
*/
void ui_create_main_window(void);

/**
  Create the main HildonApp.

  @param app_data Application data
  @return Pointer to HildonApp
*/
HildonProgram *ui_create_main_app( AppData *app_data );

/**
  Update UI status.

  @param app_ui_data Application data
  @returns TRUE if success, FALSE otherwise
*/
gboolean ui_update_status( AppUIData *app_ui_data );

/**
  Create common view for games.
*/
void ui_create_games_common_view( AppUIData *app_ui_data);

/**
   Creates INDT specific user interface

   @return Pointer to Chess HildonAppView
 */
void ui_create_indt_view( AppUIData *app_ui_data );

/**
   Repaint the user interface

 */
void ui_repaint( AppData *app_data );

/**
   Clean up UI, used before exiting program.

   @param app_ui_data AppUIData pointer to application data structure
*/
void ui_clean_up( AppUIData *app_ui_data );

/**
 Shows restart game confirmation note.

 @param app_ui_data Pointer to AppUIData struct.
 @return TRUE on Ok, FALSE otherwise.
*/
gboolean ui_show_restart_game_dialog(AppUIData *app_ui_data);

/**
 Shows restet game progress confirmation note.

 @param app_ui_data Pointer to AppUIData struct.
 @return TRUE on Ok, FALSE otherwise.
*/
gboolean ui_show_reset_game_progress_dialog(AppUIData *app_ui_data);

/**
   Set callback for a dimmed menu item.

   @param app_ui_data AppUIData object
*/
void ui_set_dimmed_menu_item_callback(AppUIData *app_ui_data);

/**
 * Shows infornote sfil_ni_not_enough_memory (WID-NOT201)
 */
void ui_show_save_nomem_dialog(AppUIData *app_ui_data);

/**
 * Hides infornote sfil_ni_not_enough_memory (WID-NOT201)
 */
void ui_hide_save_nomem_dialog(AppUIData *app_ui_data);

#endif
