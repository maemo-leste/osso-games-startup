/*
 * Games Startup
 * Copyright (c) 2005 INdT.
 * @author Andre Moreira Magalhaes <andre.magalhaes@indt.org.br>
 *
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

#ifndef STARTUP_APP_H
#define STARTUP_APP_H

#include <glib.h>
#include <startup_config.h>
#include <libosso.h>

G_BEGIN_DECLS

/* TYPE MACROS */
#define STARTUP_APP(obj) ((StartupApp *) (obj))

typedef struct _StartupApp StartupApp;

typedef void   (startup_app_top_cb)          (StartupApp *app,
                                              gpointer    data);
typedef void   (startup_app_state_change_cb) (StartupApp *app,
                                              guint       oldstate,
                                              guint       newstate,
                                              gpointer    data);

struct _StartupApp {
  StartupConfig *config;
  osso_context_t *osso;

  startup_app_top_cb *top_cb;
  gpointer top_cb_data;
  startup_app_state_change_cb *state_change_cb;
  gpointer state_change_cb_data;
  guint stale_process_guard; /* timeout event source, see check_and_kill_game */

  gchar *service;
  gchar *path;
  gchar *iface;
};


/* Game messages
 */
enum {
  GAME_RUN = 0,
  GAME_CONTINUE,
  GAME_RESTART,
  GAME_CLOSE,
  GAME_SAVE,
  GAME_LOAD,
};

/* Game state
 */
enum {
  GAME_UNLOADED = 0,
  GAME_RUNNING,
  GAME_PAUSED,
  GAME_CLOSED,
};

gboolean       startup_app_set_config_file (StartupApp  *app,
                                            const gchar *file);
StartupConfig *startup_app_get_config      (StartupApp  *app);

void           startup_app_set_top_cb      (StartupApp  *app,
                                            startup_app_top_cb *top_cb,
                                            gpointer     data);
void           startup_app_set_state_change_cb
                                           (StartupApp  *app,
                                            startup_app_state_change_cb *state_change_cb,
                                            gpointer     data);

G_END_DECLS

#endif
