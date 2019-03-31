/**
    @file state_save.h

    Functionality, definitions.

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

#ifndef STATE_SAVE_H
#define STATE_SAVE_H

#include "appdata.h"

/**
 Saves the current application state with osso state saving methods.
 The saved variables as mentioned in technocal specification are:
 GameState the state of the game startup.
 */
void save_state(void);

/**
 Reads the application state with osso state saving metods.
 @see save_state for read parameters.
 */
void read_state(void);

/**
 Clears the state file.
*/
void clear_state(void);

#endif
