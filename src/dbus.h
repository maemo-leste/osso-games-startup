/**
    @file dbus.h

    DBUS functionality, definitions.

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

#ifndef DBUS1_H
#define DBUS1_H

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif /* HAVE_CONFIG_H */

#include <libosso.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <osso-log.h>
#include <log-functions.h>

#include "appdata.h"
#include "startup_communication.h"

/**
 Send DBUS message

 @param app Application 
 @param method Method to be called.
 @param args A GArray of osso_rpc_t structures.
 @param retval The return value of the method.
 @param app_data Application specific data
 */

osso_return_t send_dbus_message(const gchar * app,
                                const gchar * app_op,
                                const gchar * app_if,
                                const gchar * method,
                                GArray * args, osso_rpc_t * retval);

/**
 Set hardware event handler

 @param state Signals that are of interest
 @param cb Callback function.
 @param data Application specific data.
 */

gboolean set_hw_event_handler(osso_hw_state_t * state,
                              osso_hw_cb_f * cb, gpointer data);


/** 
 Handles hardware events.

 @todo Just a skeleton
 @param state State occured.
 @param data Application specific data.
 */

void hw_event_handler(osso_hw_state_t * state, gpointer data);

/**
 Osso topping callback
 
 @param arguments Extra parameters
 @param app_data Application specific data
 */

void osso_top_callback(const gchar * arguments, gpointer data);

/**
 Initialize osso

 @returns TRUE on success, FALSE otherwise
 */

gboolean init_osso(void);

/**
 Deinitialize osso

 @returns TRUE on success, FALSE otherwise
 */
gboolean deinit_osso(void);

/* Send D-BUS message in async mode
 * 
 * @param osso The library context as returned by #osso_initialize. @param
 * service The service name of the target. @param object_path The object path 
 * of the target object. @param interface The interface that the method
 * belongs to. @param method The method to call @param async_cb A pointer to
 * a function to be called when the method @param data An application
 * specific pointer that is given to the cb @returns gint value */
osso_return_t send_dbus_message_async(const gchar * service,
                                      const gchar * object_path,
                                      const gchar * iface,
                                      const gchar * method,
                                      GArray * args,
				      osso_rpc_async_f *result_cb,
				      gpointer data);

#endif
