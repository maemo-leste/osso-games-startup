/**
    @file settings.h

    Prototypes for Settings component.

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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <gconf/gconf.h>
#include <gconf/gconf-client.h>
#include <libosso.h>
#include <osso-log.h>
#include <log-functions.h>

#ifdef __cplusplus
# include "appdata.hh"
#else
# include "appdata.h"
#endif

#define SETTINGS_CHESS_SAVE_FILE     "/apps/osso/chess/save_file"

/* When a (saved or new) game is loading, applicatin won't let the user start
 * a new one with full-screen hw-key. This gconf key is set to TRUE when
 * loading starts and set back to FALSE when finished. */
#define SETTINGS_CHESS_LOAD_IN_PROGRESS "/apps/osso/chess/load_in_progress"

void
  startup_config_set_group(StartupConfig * sc, const gchar * group);

void
  startup_config_hash_free_hash_func(gpointer data);

void
  startup_config_hash_free_str_func(gpointer data);


/* keys */

/**
  Gets an entry from the hashtable in the StartupConfig structure
  
  @param sc The StartupConfig where the hashtable is
  @param entry The key entry
  @param default_value The default valu. It could be NULL
*/
G_CONST_RETURN gchar *startup_config_read_entry(StartupConfig * sc,
                                                const gchar * entry,
                                                const gchar * default_value);


/**
 Cleans up some of the StartupConfig structure's entrys.

 @param sc The StartupConfig structure to be clean.
 */
void startup_config_cleanup(StartupConfig * sc);

/**
 Cleans up the StartupConfig structure.

 @param sc The StartupConfig structure to be clean.
 */
void startup_config_destroy(StartupConfig * sc);

/**
 Parses the INDT config file, and fills the StartupConfig structure

 @param sc The StartupConfig structure to be filled.
 @returns TRUE on success, FALSE otherwise.
 */
gboolean startup_config_parse_file(StartupConfig * sc);


/**
 It creates a new StartupConfig stucture, and initialize that. The filename is 
 stored in the structure

 @param file The filename to be parsed.
 @returns A new StartupConfig structure.
 */
StartupConfig *startup_config_new(const gchar * file);

/**
 Set key value. Value type not specified.

 @param key Key name as string.
 @param value Key value to be set as GConfValue.
 @returns TRUE on success, FALSE otherwise.
 */
gboolean settings_set(const gchar * key, const GConfValue * value);

/**
 Set int type key value.

 @param key Key name as string.
 @param value Key value to be set as gint.
 @returns TRUE on success, FALSE otherwise.
 */
gboolean settings_set_int(const gchar * key, const gint val);

/**
 Set string type key value.

 @param key Key name as string
 @param value Key value to be set as string.
 @returns TRUE on success, FALSE otherwise.
 */
gboolean settings_set_string(const gchar * key, const gchar * val);

/**
 Set float type key value.

 @param key Key name as string.
 @param value Key value to be set as gdouble.
 @returns TRUE on success, FALSE otherwise.
 */
gboolean settings_set_float(const gchar * key, const gdouble val);

/**
 Set boolean type key value.

 @param key Key name as string.
 @param value Key value to be set as gboolean.
 @returns TRUE on success, FALSE otherwise.
 */
gboolean settings_set_bool(const gchar * key, const gboolean val);

/**
 Set stringlist type key value.

 @param key Key name as string.
 @param value Key value to be set as GSList of gchar *'s.
 @return TRUE on success, FALSE otherwise.
*/
gboolean settings_set_list_string(const gchar * key, GSList * val);

/**
 Get key value.

 @param key Key name as string.
 @returns Value as GConfValue.
 */
GConfValue *settings_get(const gchar * key);

/**
 Get int type key value.

 @param key Key name as string.
 @returns Value as gint.
 */
gint settings_get_int(const gchar * key);

/**
 Get float type key value.

 @param key Key name as string.
 @returns Value as gdouble.
 */
gdouble settings_get_float(const gchar * key);

/**
 Get string type key value.

 @param key Key name as string.
 @returns Value as string.
 */
gchar *settings_get_string(const gchar * key);

/**
 Get boolean type key value.

 @param key Key name as string.
 @returns Value as gboolean.
 */
gboolean settings_get_bool(const gchar * key);

/**
 Get stringlist type key value

 @param key Key name as string.
 @return Value as GSList.
 */
GSList *settings_get_list_string(const gchar * key);

/**
 Add a notify about changes on a key. Key may be folder as well.

 @param key Key name as string.
 @param func Function to handle the notify event.
 @returns Connection ID for removing the notify or zero on error.
 */
guint settings_notify_add(const gchar * key, GConfClientNotifyFunc func);

/**
 Removes the notify about changes on the key.

 @param id Connection ID to be removed.
 */
void settings_notify_remove(guint id);

/**
 Initialize GConf settings object.
 
 @returns TRUE on success, FALSE otherwise.
 */
gboolean init_settings(void);

/**
 Deinit GConf, free reserved memory.

 @param app_data Application data.
 @returns TRUE on success, FALSE otherwise.
 */
gboolean deinit_settings(void);

#endif
