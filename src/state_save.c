/**
    @file state_save.c

    Application state saving.

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

#include <glib.h>
#include "state_save.h"
#include "startup_gamemanager.h"
#include "settings.h"

typedef struct
{
  guint32 game_state;
} StateData;

void save_state()
{
  StateData statedata;
  osso_state_t state;

  memset(&statedata, 0, sizeof(statedata));

  state.state_size = sizeof(statedata);
  state.state_data = (gpointer)&statedata;

  statedata.game_state = game_state;

  osso_state_write(app_data->startup_app->osso, &state);
}

void read_state()
{
  StateData statedata;
  osso_state_t state;
  osso_return_t ret;

  memset(&statedata, 0, sizeof(statedata));

  state.state_size = sizeof(statedata);
  state.state_data = (gpointer)&statedata;
  
  ret = osso_state_read(app_data->startup_app->osso, &state);
  if (ret != OSSO_OK)
    return;

  game_state = statedata.game_state;
}

void clear_state()
{
  StateData statedata;
  osso_state_t state;

  memset(&statedata, 0, sizeof(statedata));

  state.state_size = sizeof(statedata);
  state.state_data = &statedata;

  osso_state_write(app_data->startup_app->osso, &state);
}

