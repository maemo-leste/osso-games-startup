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

#ifndef STARTUP_CONFIG_H
#define STARTUP_CONFIG_H

#include <glib/gtypes.h>
#include <glib/ghash.h>

G_BEGIN_DECLS

typedef struct _StartupConfig StartupConfig;
typedef struct _StartupConfigPrivate StartupConfigPrivate;

struct _StartupConfig 
{
  gchar *file;
  
  StartupConfigPrivate *priv;
};

StartupConfig        *startup_config_new        (const gchar *file);
void                  startup_config_destroy    (StartupConfig *sc);
gboolean              startup_config_parse_file (StartupConfig *sc);
void                  startup_config_set_group  (StartupConfig *sc, 
                                                 const gchar   *group);
G_CONST_RETURN gchar *startup_config_read_entry (StartupConfig *sc,
                                                 const gchar   *entry,
                                                 const gchar   *default_value);

G_END_DECLS

#endif
