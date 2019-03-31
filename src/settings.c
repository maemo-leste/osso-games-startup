/**
    @file settings.c

    Implementation of Settings component. Access to GConf and
    possibility to read and change key values.

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

#include "settings.h"
#include <string.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <glib/gstdio.h>

/* The following function is from INDT's games-startup code. The copyrights
 * belongs to them. The INDT's games-startup was released under LGPL license. 
 * This function is modified under LGPL license */

/* Get a AppUIData's INDT configure structure */
StartupConfig *
startup_app_get_config(StartupApp *app)
{
    g_return_val_if_fail(app != NULL, NULL);

    return app->config;
}

/* The following function is from INDT's games-startup code. The copyrights
 * belongs to them. The INDT's games-startup was released under LGPL license. */

/* Get an entry, defined with the entry key */
G_CONST_RETURN gchar *
startup_config_read_entry(StartupConfig * sc,
                          const gchar * entry, const gchar * default_value)
{
    gchar *group;
    gpointer *key, *value;

    g_return_val_if_fail(sc != NULL, NULL);
    g_return_val_if_fail(sc->priv->initialized != FALSE, NULL);
    g_return_val_if_fail(entry != NULL, NULL);

    if (!sc->priv->current_hash_entry)
    {                           // use default group
        if (sc->priv->current_group)
            group = sc->priv->current_group;
        else
            group = "";

        if (!g_hash_table_lookup_extended
            (sc->priv->hash_group, group, (gpointer) & key,
             (gpointer) & value))
        {
#ifdef DEBUG
            g_print
                ("%s (%d): invalid group '%s'. use startup_config_set_group\n",
                 __FUNCTION__, __LINE__, group);
#endif
            return NULL;
        }
        sc->priv->current_hash_entry = (GHashTable *) value;
    }

    if (g_hash_table_lookup_extended
        (sc->priv->current_hash_entry, entry, (gpointer) & key,
         (gpointer) & value))
    {
        return (gchar *) value;
    }

    return default_value;
}

/* The following function is from INDT's games-startup code. The copyrights
 * belongs to them. The INDT's games-startup was released under LGPL license. */

void
startup_config_set_group(StartupConfig * sc, const gchar * group)
{
    g_return_if_fail(sc != NULL);

    if (sc->priv->current_group)
        g_free(sc->priv->current_group);

    if (group)
        sc->priv->current_group = g_strdup(group);
    else
        sc->priv->current_group = NULL;
}

/* The following function is from INDT's games-startup code. The copyrights
 * belongs to them. The INDT's games-startup was released under LGPL license. */

/* Makes, and initialize a new StartupConfig structure. */
StartupConfig *
startup_config_new(const gchar * file)
{
    StartupConfig *sc;

    g_return_val_if_fail(file != NULL, NULL);

    sc = g_new0(StartupConfig, 1);
    sc->file = g_strdup(file);

    sc->priv = g_new0(StartupConfigPrivate, 1);
    sc->priv->initialized = FALSE;
    sc->priv->current_group = NULL;
    sc->priv->hash_group = NULL;
    sc->priv->current_hash_entry = NULL;

    return sc;
}

/* The following function is from INDT's games-startup code. The copyrights
 * belongs to them. The INDT's games-startup was released under LGPL license. */

/* Cleans up some of the StartupConfig structure's entry */
void
startup_config_cleanup(StartupConfig * sc)
{
    if (!sc->priv->initialized)
        return;

    sc->priv->initialized = FALSE;
    if (sc->priv->current_group)
    {
        g_free(sc->priv->current_group);
        sc->priv->current_group = NULL;
    }

    if (sc->priv->hash_group)
    {
        g_hash_table_destroy(sc->priv->hash_group);
        sc->priv->hash_group = NULL;
    }
    sc->priv->current_hash_entry = NULL;
}

/* The following function is from INDT's games-startup code. The copyrights
 * belongs to them. The INDT's games-startup was released under LGPL license. */

/* Fully destroys the StartupConfig structure */
void
startup_config_destroy(StartupConfig * sc)
{
    g_return_if_fail(sc != NULL);

    g_free(sc->file);
    sc->file = NULL;

    startup_config_cleanup(sc);
    g_free(sc->priv);
}


/* The following function is from INDT's games-startup code. The copyrights
 * belongs to them. The INDT's games-startup was released under LGPL license. */

/* It sets up the INDT config files */
gboolean
startup_app_set_config_file(StartupApp *app, const gchar *file)
{
    g_return_val_if_fail(app != NULL, FALSE);
    g_return_val_if_fail(file != NULL, FALSE);

    if (app->config)
        startup_config_destroy(app->config);

    app->config = startup_config_new(file);

    if (!startup_config_parse_file(app->config))
        return FALSE;

    startup_config_set_group(app->config, "Startup Entry");
    return TRUE;
}

/* The following function is from INDT's games-startup code. The copyrights
 * belongs to them. The INDT's games-startup was released under LGPL license. */

void
startup_config_hash_free_hash_func(gpointer data)
{
    g_hash_table_destroy((GHashTable *) data);
}

/* The following function is from INDT's games-startup code. The copyrights
 * belongs to them. The INDT's games-startup was released under LGPL license. 
 */

void
startup_config_hash_free_str_func(gpointer data)
{
    g_free(data);
}

/* The following function is from INDT's games-startup code. The copyrights
 * belongs to them. The INDT's games-startup was released under LGPL license. */

/* Parse the INDT configfile and fills the Configstructure */
gboolean
startup_config_parse_file(StartupConfig * sc)
{
    FILE *file;
    gchar line[256];
    gchar *group, *key, *value;
#ifdef DEBUG
    guint current_line = 0;
#endif

    g_return_val_if_fail(sc != NULL, FALSE);

    if (sc->priv->initialized)
        startup_config_cleanup(sc);

    file = fopen(sc->file, "rb");
    if (!file)
    {
        // error - file not found or permission denied
        return FALSE;
    }

    sc->priv->hash_group = g_hash_table_new_full(g_str_hash,
                                                 g_str_equal,
                                                 startup_config_hash_free_str_func,
                                                 startup_config_hash_free_hash_func);

    while (!feof(file))
    {
#ifdef DEBUG
        ++current_line;
#endif

        fgets(line, 256, file);
        g_strchug(line);
        if (line[0] == '\0' || line[0] == '#')
        {
            continue;
        }

        g_strchomp(line);

        // group found
        if (line[0] == '[')
        {
            if (line[strlen(line) - 1] != ']')
            {
#ifdef DEBUG
                g_print("invalid group '%s' at line '%d'\n", line,
                        current_line);
#endif

                startup_config_cleanup(sc);
                fclose(file);
                return FALSE;
            }

            group = g_new0(gchar, strlen(line) - 1);
            g_strlcpy(group, line + 1, strlen(line) - 1);

            sc->priv->current_hash_entry = g_hash_table_new_full(g_str_hash,
                                                                 g_str_equal,
                                                                 startup_config_hash_free_str_func,
                                                                 startup_config_hash_free_str_func);

            g_hash_table_insert(sc->priv->hash_group,
                                group, sc->priv->current_hash_entry);
            continue;
        }

        value = strchr(line, '=');
        if (!value)
        {
#ifdef DEBUG
            g_print("invalid entry '%s' at line '%d'\n", line, current_line);
#endif

            startup_config_cleanup(sc);
            fclose(file);
            return FALSE;
        }

        if (sc->priv->current_hash_entry == NULL)
        {
            // no group
            sc->priv->current_hash_entry = g_hash_table_new_full(g_str_hash,
                                                                 g_str_equal,
                                                                 startup_config_hash_free_str_func,
                                                                 startup_config_hash_free_str_func);
            g_hash_table_insert(sc->priv->hash_group,
                                g_strdup(""), sc->priv->current_hash_entry);
        }

        key = g_new0(gchar, value - line + 1);
        g_strlcpy(key, line, value - line + 1);

        // TODO test duplicate entry 

        g_hash_table_insert(sc->priv->current_hash_entry,
                            key, g_strdup(value + 1));
    }

    sc->priv->current_hash_entry = NULL;
    sc->priv->initialized = TRUE;

    fclose(file);
    return TRUE;
}


/* Set key value */
gboolean
settings_set(const gchar * key, const GConfValue * value)
{
    GError *err = NULL;

    g_assert(app_data->gc_client);
    g_assert(key);

    /* Set key value */
    g_debug("Setting key %s value\n", key);
    gconf_client_set(app_data->gc_client, key, value, &err);

    /* Check errors */
    if (err != NULL)
    {
        g_warning("Settings set failed: %s", err->message);
        g_error_free(err);
        return FALSE;
    }

    return TRUE;
}

/* Get key value */
GConfValue *
settings_get(const gchar * key)
{
    g_debug("Getting key %s value\n", key);
    return gconf_client_get(app_data->gc_client, key, NULL);
}

/* Set int type value */
gboolean
settings_set_int(const gchar * key, const gint val)
{
    g_debug("Setting key %s value to %i\n", key, val);
    return gconf_client_set_int(app_data->gc_client, key, val, NULL);
}

/* Set string type value */
gboolean
settings_set_string(const gchar * key, const gchar * val)
{
    g_debug("Setting key %s value to %s\n", key, val);
    return gconf_client_set_string(app_data->gc_client, key, val, NULL);
}

/* Set float type value */
gboolean
settings_set_float(const gchar * key, const gdouble val)
{
    g_debug("Setting key %s value to %f\n", key, val);
    return gconf_client_set_float(app_data->gc_client, key, val, NULL);
}

/* Set boolean type value */
gboolean
settings_set_bool(const gchar * key, const gboolean val)
{
    g_debug("Setting key %s value to %i\n", key, val);
    return gconf_client_set_bool(app_data->gc_client, key, val, NULL);
}

gboolean
settings_set_list_string(const gchar * key, GSList * val)
{
    g_debug("Setting key %s value to list of chars\n", key);
    return gconf_client_set_list(app_data->gc_client, key, GCONF_VALUE_STRING, val,
                                 NULL);
}

/* Get int type key value */
gint
settings_get_int(const gchar * key)
{
    g_debug("Getting key %s value as int\n", key);
    return gconf_client_get_int(app_data->gc_client, key, NULL);
}

/* Get float type key value */
gdouble
settings_get_float(const gchar * key)
{
    g_debug("Getting key %s value as float\n", key);
    return gconf_client_get_float(app_data->gc_client, key, NULL);
}

/* Get string type key value */
gchar *
settings_get_string(const gchar * key)
{
    g_debug("Getting key %s value as string\n", key);
    return gconf_client_get_string(app_data->gc_client, key, NULL);
}

/* Get boolean type key value */
gboolean
settings_get_bool(const gchar * key)
{
    g_debug("Getting key %s value as boolean\n", key);
    return gconf_client_get_bool(app_data->gc_client, key, NULL);
}

/* Get stringlist type key value */
GSList *
settings_get_list_string(const gchar * key)
{
    g_debug("Getting key %s value as stringlist\n", key);
    return gconf_client_get_list(app_data->gc_client, key, GCONF_VALUE_STRING, NULL);
}

/* Notify for changes in key/folder */
guint
settings_notify_add(const gchar * key, GConfClientNotifyFunc func)
{
    GError *err;
    /* Add a notify */
    g_debug("Adding notify to key %s\n", key);
    guint id = gconf_client_notify_add(app_data->gc_client, key, func,
                                       NULL, NULL, &err);

    /* Check error */
    if (err != NULL)
    {
        g_warning("Settings notify addition failed: %s",
                 err->message);
        g_error_free(err);
        return 0;
    }

    return id;
}

/* Remove notify about changes */
void
settings_notify_remove(guint id)
{
    gconf_client_notify_remove(app_data->gc_client, id);
}

/* Init the gconf settings object */
gboolean
init_settings()
{
    g_assert(app_data);

    /* Init the client */
    g_debug("Initializing GConf\n");
    app_data->gc_client = gconf_client_get_default();
    app_data->gc_client = app_data->gc_client;

    if (app_data->gc_client == NULL)
    {
        return FALSE;
    }

    return TRUE;
}

/* Deinit gconf settings object */
gboolean
deinit_settings(void)
{
    if (app_data->gc_client == NULL)
    {
        return FALSE;
    }

    g_debug("Deinitializing GConf\n");
    g_object_unref(app_data->gc_client);
    app_data->gc_client = NULL;

    return TRUE;
}

