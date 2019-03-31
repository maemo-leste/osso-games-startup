/**
    @file dbus.c

    DBUS functionality.

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

#include "dbus.h"
#include "state_save.h"

/* Osso Top cb. Deprecated? Yep. Still have it? Yep.*/
void
osso_top_callback(const gchar * arguments, gpointer data)
{
    g_assert(app_data);
    gtk_window_present(GTK_WINDOW(app_data->app_ui_data->main_view));
}

/* Send d-bus message */
osso_return_t
send_dbus_message(const gchar * service,
                  const gchar * object_path,
                  const gchar * iface,
                  const gchar * method, GArray * args, osso_rpc_t * retval)
{
    g_debug("Sending message %s to: %s\n", method, service);
    return osso_rpc_run(app_data->startup_app->osso,
                        service, object_path, iface, method, retval, 0
        );
}

/* Depending on the state of hw, do something */
void
hw_event_handler(osso_hw_state_t * state, gpointer data)
{
    if (state->shutdown_ind)
    {
        /* Rebooting */
        game_clean_up_and_quit(NULL, app_data->app_ui_data);
    }
    if (state->memory_low_ind)
    {
        /* Memory low */
        ui_show_save_nomem_dialog(app_data->app_ui_data);
        if (app_data->app_ui_data->low_memory == FALSE)
        {
            app_data->app_ui_data->low_memory = TRUE;
        }
    }
    else
    {
        if (app_data->app_ui_data->low_memory)
        {
            app_data->app_ui_data->low_memory = FALSE;
            ui_hide_save_nomem_dialog(app_data->app_ui_data);
        }
    }
    if (state->save_unsaved_data_ind)
    {
        /* Unsaved data should be saved */
    }
    if (state->system_inactivity_ind)
    {
        /* Minimum activity */
    }
}

/* Do initialization for OSSO, create osso context, set topping callback,
 * dbus-message handling callbaks, and hw-event callbacks. */

gboolean
init_osso()
{
    StartupApp *app = app_data->startup_app;
    gchar *service = NULL, *path = NULL, *iface = NULL;
    osso_return_t ret;
    
    /* Init osso */
    g_debug("Initializing osso");

    gchar *appl_name = g_strdup_printf("osso_%s_startup",
                                       startup_config_read_entry(app->config,
                                                                 "Name",
                                                                 NULL));
    app->osso =
        osso_initialize(appl_name,
                        startup_config_read_entry(app->config,
                                                  "Version", NULL), TRUE,
                        NULL);
    g_free(appl_name);

    if (app->osso == NULL)
    {
        g_critical("Osso initialization failed");
        return FALSE;
    }

    /* Set topping callback */
    ret = osso_application_set_top_cb(app->osso, startup_app_top_event_cb, app);

    if (ret != OSSO_OK)
    {
        g_warning("Could not set topping callback");
    }

    service = g_strdup_printf("%s.startup", app->service);
    path = g_strdup_printf("%s/startup", app->path);
    iface = g_strdup_printf("%s.startup", app->iface);
    ret = osso_rpc_set_cb_f(app->osso,
                            service,
                            path, iface, startup_app_rpc_event_cb, app);

    g_free(service);
    g_free(path);
    g_free(iface);

    if (ret != OSSO_OK)
    {
        g_warning("Could not set callback for receiving messages");
    }

    /* Set handling changes in HW states. */
    ret = osso_hw_set_event_cb(app->osso,
                               NULL, hw_event_handler, app_data);

    if (ret != OSSO_OK)
    {
        g_warning("Could not set callback for HW monitoring");
    }

    return TRUE;
}

/* Deinitialize osso specific data TODO: Check of return values from osso */
gboolean
deinit_osso()
{
    StartupApp *app = app_data->startup_app;	
    osso_application_unset_top_cb(app->osso,
                                  startup_app_top_event_cb,
                                  app);
    gchar *service =
        g_strdup_printf("%s.startup", app->service);
    gchar *path =
        g_strdup_printf("%s/startup", app->path);
    gchar *iface =
        g_strdup_printf("%s.startup", app->iface);

    /* Set handling d-bus messages from session bus */
    osso_rpc_unset_cb_f(app->osso,
                        service,
                        path,
                        iface,
                        startup_app_rpc_event_cb, app);

    g_free(service);
    g_free(path);
    g_free(iface);
    
    osso_hw_unset_event_cb(app->osso, NULL);

    /* Deinit osso */
    osso_deinitialize(app->osso);

    return TRUE;
}

/* Send async d-bus message */
osso_return_t
send_dbus_message_async(const gchar * service,
                        const gchar * object_path,
                        const gchar * iface,
                        const gchar * method,
			GArray * args,
			osso_rpc_async_f *result_cb,
			gpointer data)
{
    g_debug("Sending async message %s to: %s", method, service);
    app_data->app_ui_data->async_command_sending = TRUE;
    return osso_rpc_async_run(app_data->startup_app->osso,
                              service,
                              object_path,
                              iface,
                              method, result_cb, data, 0);
}

