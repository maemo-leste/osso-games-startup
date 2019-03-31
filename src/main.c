/**
    @file main.c

    Application main.

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <signal.h>

#include "main.h"
#include "state_save.h"
#include "startup_state.h"
#include "ui/callbacks.h"

#include <hildon/hildon.h>

AppData *app_data = NULL;

static void
releaseAppData(AppData *appdata) {
    if (appdata!=NULL) {
        if (appdata->app_ui_data!=NULL) {
            g_free(appdata->app_ui_data);
            appdata->app_ui_data = NULL;
        }
        g_free(appdata);
    }
}

AppData *get_app_data(void)
{
    return app_data;
}

int
main(int argc, char **argv)
{
    if (argc < 2) {
      g_warning("No config file passed, exiting");
      return 1;
    }

    setlocale(LC_ALL, "");
    bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    textdomain(GETTEXT_PACKAGE);

    //gtk_init(&argc, &argv);
    hildon_gtk_init(&argc, &argv);

    app_data = g_new0(AppData, 1);
    app_data->app_ui_data = g_new0(AppUIData, 1);
    app_data->app_ui_data->active_dialog  = NULL;
    /* build plugin structures from config file */
    init_game_from_config(argv[1]);

    /* Init settings */
    if (!init_settings())
    {
        g_critical("Settings init failed");
        releaseAppData(app_data);
        return 1;
    }

    /* set up SIGTERM handler (for bgkill) */
    signal(SIGTERM, on_bgkill);
    
    /* Init osso */
    if (!init_osso())
    {
        g_critical("OSSO init failed");
        releaseAppData(app_data);
        return 1;
    }

    /* Create Hildon window */
    ui_create_main_window();

    /* fill the rest of plugin structures */
    Fill_Plugin_Settings();
    
    gtk_main();

    deinit_osso();
    deinit_settings();

    if (app_data->app_ui_data->timeout > 0)
    {
        g_source_remove(app_data->app_ui_data->timeout);
    }
    
    if (app_data->plugin)
    {
        g_free(app_data->startup_app->service);
        g_free(app_data->startup_app->path);
        g_free(app_data->startup_app->iface);
    }

    releaseAppData(app_data);

    return 0;
}
