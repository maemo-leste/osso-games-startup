/**
    @file startup_communication.c

    Implements Communication module.

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

#include "startup_communication.h"

gint handle_game_message(const gchar *method, GArray *arguments,
  gpointer data, osso_rpc_t *retval)
{
  g_assert(method);
  g_debug("%s\n", __FUNCTION__);
  osso_rpc_t val = g_array_index(arguments, osso_rpc_t, (guint)0);

  g_debug("Got DBUS method: %s\n",method);
  
  /* Handle pause method */
  if (g_ascii_strcasecmp(method,GAME_PAUSE_METHOD)==0) {
    g_assert(retval);
    
    app_data->app_ui_data->window_front_needed=TRUE;
    /* Update state & UI */
    game_state_changed(gs_paused);
    
    retval->type=DBUS_TYPE_BOOLEAN;
    retval->value.b=TRUE;
    return OSSO_OK;
  }
  /* Handle home method */
  if (g_ascii_strcasecmp(method,GAME_HOME_METHOD)==0) {
    g_assert(retval);
	  gtk_window_iconify(GTK_WINDOW(app_data->app_ui_data->main_view));

    /* Update state & UI */
    game_state_changed(gs_paused);
        
    retval->type=DBUS_TYPE_BOOLEAN;
    retval->value.b=TRUE;
    return OSSO_OK;
  }
  /* Handle end method */
  else if (g_ascii_strcasecmp(method,GAME_END_METHOD)==0) {
    g_assert(retval);

    /* Update state & UI */
    game_state_changed(gs_closed);

    retval->type=DBUS_TYPE_BOOLEAN;
    retval->value.b=TRUE;
    return OSSO_OK;
  }
  /* Handle mime open message */
  else if (g_ascii_strcasecmp(method, GAME_MIME_OPEN) == 0) {
    if ((val.type == DBUS_TYPE_STRING)
        && (val.value.s != NULL)) {
      game_mime_open((gchar *)val.value.s);
              
      /* success*/
      retval->type=DBUS_TYPE_BOOLEAN;
      retval->value.b=TRUE;
      return OSSO_OK;
    } else {
      retval->type=DBUS_TYPE_BOOLEAN;
      retval->value.b=FALSE;
    }
    
    return OSSO_OK;    
  }
  else {
    g_warning("Unknown DBUS method: %s\n", method);

    retval->type=DBUS_TYPE_BOOLEAN;
    retval->value.b=FALSE;
  }

  return OSSO_ERROR;
}

gboolean set_game_state(const guint state, osso_rpc_t *retval)
{
  osso_return_t ret;
  const gchar *method, *service, *path, *iface;

  g_debug("Setting state: %d\n",state);
  g_debug("State: %d\n",get_game_state());

  /* Select method */
  switch(state) {
    case GAME_RUN:
      /* Start new game */
      method = GAME_RUN_METHOD;
      break;
    case GAME_CONTINUE:
      /* Resume game */
      method = GAME_CONTINUE_METHOD;
      break;
    case GAME_RESTART:
      /* Restart ongoing game */
      method = GAME_RESTART_METHOD;
      break;
    case GAME_CLOSE:
      /* Close ongoing game */
      method = GAME_CLOSE_METHOD;
      break;
    case GAME_LOAD:
      /* Load game */
      method = GAME_LOAD_METHOD;
      break;
    case GAME_SAVE:
      /* Save ongoing game */
      method = GAME_SAVE_METHOD;
      break;
    default:
      return FALSE;
  }

  service=game_get_service();
  path=game_get_object_path();
  iface=game_get_iface();
  g_assert(service);
  g_assert(path);
  g_assert(iface);

  /* Send message */
  ret = send_dbus_message(
    service,
    path,
    iface,
    method,
    NULL,
    retval
  );

  return (ret == OSSO_OK); 
}


void
set_game_state_async_dbus_result(const gchar * interface,
                          const gchar * method,
                          osso_rpc_t * retval,
			  gpointer data)
{
	gboolean sdl_games = FALSE, banner_hide = FALSE;
    g_debug("Async message %s return from %s", method, interface);

    app_data->app_ui_data->async_command_sending = FALSE;

    if(g_str_equal(GAME_RUN_METHOD, method))
    switch(retval->type)
    {
	    case DBUS_TYPE_BOOLEAN :
		    if(retval->value.b){
			dimm_everything(app_data->app_ui_data);
			set_gamestate(gs_closed);
			break;
		    }
	    case DBUS_TYPE_UINT32 :
	    case DBUS_TYPE_INT32 :
	    case DBUS_TYPE_STRING :
	    default:
		    {
			    const gchar *service;
			    service=game_get_service();

			    if( (!g_strcmp0(service, "com.nokia.osso_lmarbles")) || (!g_strcmp0(service, "br.org.indt.maemoblocks")) ) {
				    sdl_games = TRUE;
			    }
			    if(sdl_games && ( (!g_strcmp0(method, GAME_RUN_METHOD)) || (!g_strcmp0(method, GAME_RESTART_METHOD)) ) ) {
				   banner_hide = TRUE; 
			    }

			    if( !banner_hide ) {
				    undimm_everything(app_data->app_ui_data);
				    gtk_widget_grab_default(app_data->app_ui_data->play_button);
				    hildon_banner_show_information(NULL, NULL, _("game_ia_startup_not_started"));
			    }
			    else {
			    }
		    }
    }
}

gboolean set_game_state_async(const guint state)
{
  osso_return_t ret;
  const gchar *method, *service, *path, *iface;
  
  g_debug("Setting state: %d\n",state);
  g_debug("State: %d\n",get_game_state());

  /* Select method */
  switch(state) {
    case GAME_RUN:
      /* Start new game */
      method = GAME_RUN_METHOD;
      break;
    case GAME_CONTINUE:
      /* Resume game */
      method = GAME_CONTINUE_METHOD;
      break;
    case GAME_RESTART:
      /* Restart ongoing game */
      method = GAME_RESTART_METHOD;
      break;
    case GAME_CLOSE:
      /* Close ongoing game */
      method = GAME_CLOSE_METHOD;
      break;
    case GAME_LOAD:
      /* Load game */
      method = GAME_LOAD_METHOD;
      break;
    case GAME_SAVE:
      /* Save ongoing game */
      method = GAME_SAVE_METHOD;
      break;
    default:
      return FALSE;
  }

  /*Setting the buttons unsensitive, while it makes the command*/
  dimm_everything(app_data->app_ui_data);
  
  service=game_get_service();
  path=game_get_object_path();
  iface=game_get_iface();
  g_assert(service);
  g_assert(path);
  g_assert(iface);

  /* Send message */
  ret = send_dbus_message_async(
    service,
    path,
    iface,
    method,
    NULL,
    set_game_state_async_dbus_result,
    app_data
  );

  return (ret == OSSO_OK);
}

