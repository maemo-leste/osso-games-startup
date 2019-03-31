/**
    @file main.h

    Prototypes for main

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

#ifndef MAIN_H
#define MAIN_H

#include "ui/startup_i18n.h"
#include <stdio.h>
#include <string.h>
#include <libosso.h>
#include <locale.h>
#include "ui/interface.h"
#include "startup_gamemanager.h"
#include "startup_communication.h"
#include "dbus.h"
#include "appdata.h"
#include "settings.h"

/**
   Returns the application data
   
   @return returns current application data
*/
AppData *get_app_data(void);

#endif
