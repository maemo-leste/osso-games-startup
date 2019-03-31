/**
    @file interface.c

    Implementation of the user interface functions.

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

/* Include function prototypes */
#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include <hildon/hildon.h>
#include "interface.h"
#include "dbus.h"
#include "startup_plugin.h"
#include "../state_save.h"
#include "main.h"


GtkWidget *open_item = NULL;
gchar *game_title = NULL;

static gboolean
show_picture(gpointer user_data)
{
    AppUIData *app_ui_data = (AppUIData *) user_data;
    gtk_image_set_from_file(GTK_IMAGE(app_ui_data->game_image),
                            startup_config_read_entry(startup_app_get_config (app_data->startup_app), "Image", NULL));
    return FALSE;
}


static void
on_window_realized(GtkWidget * widget, gpointer data)
{
    AppUIData *app_ui_data = (AppUIData *) data;
    app_ui_data->timeout = g_timeout_add(1, show_picture, data);
    return;
}

/* Create INDT specific menu */
void
ui_create_indt_menu(AppUIData * ui)
{
    HildonAppMenu *main_menu_bar;
    GtkWidget **plugin_menu;
    guint nmenu_items = 0, i;
    GtkAccelGroup * accel_group;
    HildonSizeType buttonsize = HILDON_SIZE_FINGER_HEIGHT | HILDON_SIZE_AUTO_WIDTH;

    osso_log(LOG_DEBUG, "%s\n", __FUNCTION__);

    // Get application view's menu bar 
    main_menu_bar = HILDON_APP_MENU (hildon_app_menu_new ());
    ui->main_menu_bar = main_menu_bar;
    
    accel_group = gtk_accel_group_new();
    
    ui->game_game_menu = NULL;
    ui->game_play_menu = NULL;
    ui->game_restart_menu = NULL;

    if (app_data->plugin && app_data->plugin->info->load_menu)
    {
        if (app_data->plugin->menu_open_save && app_data->plugin->info->plugin_cb)
        {
	    /* Open */
	    open_item = hildon_gtk_button_new (buttonsize);
	    gtk_button_set_label (GTK_BUTTON (open_item), _("game_me_chess_menu_game_open"));
            g_signal_connect_after(G_OBJECT(open_item), "clicked",
				   G_CALLBACK(app_data->plugin->info->plugin_cb),
				   (gpointer) ME_GAME_OPEN);
	    hildon_app_menu_append (main_menu_bar, GTK_BUTTON (open_item));

	    /* Save */
	    ui->game_save_menu = hildon_gtk_button_new (buttonsize);
	    gtk_button_set_label (GTK_BUTTON (ui->game_save_menu), _("game_me_chess_menu_game_save"));
            g_signal_connect_after(G_OBJECT(ui->game_save_menu), "clicked",
				   G_CALLBACK(app_data->plugin->info->plugin_cb),
				   (gpointer) ME_GAME_SAVE);
	    hildon_app_menu_append (main_menu_bar, GTK_BUTTON (ui->game_save_menu));

            /* Save reference of save menu item to plugin so it can handle
             * showing/hiding if/when savefile is changed to readonly */
            app_data->plugin->info->plugin_cb(GTK_WIDGET(ui->game_save_menu), (gpointer) MA_GAME_SAVEMENU_REFERENCE);

            ui->game_saveas_menu = NULL;  //("game_me_chess_menu_game_save_as"));
            /* g_signal_connect(G_OBJECT(ui->game_saveas_menu), "activate",
                             G_CALLBACK(app_data->plugin->info->plugin_cb),
                             (gpointer) ME_GAME_SAVE_AS);
	    */

            if (get_game_state() == gs_unloaded ||
                get_game_state() == gs_closed)
            {
                gtk_widget_hide(ui->game_save_menu);
                gtk_widget_hide(ui->game_saveas_menu);
            }
        }

        plugin_menu = app_data->plugin->info->load_menu(&nmenu_items);
        // Conditional creation, by game type.
        for (i = 0; i < nmenu_items; i++)
        {
            if (GTK_IS_BUTTON(plugin_menu[i]))
            {
	        hildon_app_menu_append (main_menu_bar, GTK_BUTTON (plugin_menu[i]));
            }
        }
        if (app_data->plugin->info->update_menu)
            app_data->plugin->info->update_menu();
    }

    if (get_game_state() == gs_unloaded || get_game_state() == gs_closed)
        gtk_widget_hide(ui->game_restart_menu);

    gtk_widget_show_all(GTK_WIDGET(main_menu_bar));

    hildon_window_set_app_menu (HILDON_WINDOW (ui->main_view), main_menu_bar);

    if (accel_group != NULL) {
        gtk_window_add_accel_group( GTK_WINDOW( ui->main_view ), accel_group );
    }

}


/* Create the main window */
void
ui_create_main_window()
{
    AppUIData *app_ui_data;

    g_assert(app_data);

    app_ui_data = app_data->app_ui_data;


    app_ui_data->app = ui_create_main_app(app_data);
    g_assert(app_ui_data->app);
   
    ui_create_indt_view(app_ui_data);

    g_assert(app_ui_data->main_view);

    /* Connect delete signal */
	
    g_signal_connect(G_OBJECT(app_ui_data->app), "notify::is-topmost",
                     G_CALLBACK(top_changed), (gpointer) app_data);

    hildon_program_add_window(app_ui_data->app, app_ui_data->main_view);

    g_signal_connect(G_OBJECT(app_ui_data->main_view), "map-event",
		    	G_CALLBACK(game_get_screenshot), (gpointer)NULL);

    ui_create_indt_menu(app_ui_data);

    gtk_widget_show_all(GTK_WIDGET(app_ui_data->main_view));

    /* read state */
    read_state();

    ui_update_status(app_ui_data);
}

HildonProgram *
ui_create_main_app(AppData * app_data)
{
    HildonProgram *result = NULL;

    /* Create HildonApp */
    result = HILDON_PROGRAM(hildon_program_get_instance());

    hildon_program_set_can_hibernate(result, FALSE);

    return result;
}

void
ui_change_play_continue_button(AppData * app_data,ChangeButton next_button)
{
    AppUIData *app_ui_data=app_data->app_ui_data;
    StartupUI *ui = app_data->plugin->gs->ui;
    GtkWidget *button_show=NULL,*button_hide=NULL;
    switch (next_button)
    {
        case CHANGE_PLAY:
        {
            button_hide=app_ui_data->continue_button;
            button_show=app_ui_data->play_button;
            break;
        }
        case CHANGE_CONTINUE:
        {
            button_hide=app_ui_data->play_button;
            button_show=app_ui_data->continue_button;
            break;
        }
    }
    gtk_widget_hide(button_hide);
    gtk_widget_show(button_show);
    gtk_widget_grab_focus(button_show);
    gtk_widget_set_sensitive(button_show,GTK_WIDGET_IS_SENSITIVE(button_hide));
    ui->play_button=button_show;
}

gboolean
ui_update_status(AppUIData * app_ui_data)
{
    g_assert(app_ui_data);
    osso_log(LOG_DEBUG, "%s\n", __FUNCTION__);
    /* Get game state */
    GameState gs = get_game_state();

    g_return_val_if_fail(app_ui_data->play_button
		    && app_ui_data->continue_button 
		    && app_ui_data->controls_state_label, FALSE);

    /* Game is unloaded */
    if (gs == gs_unloaded)
    {
        gtk_label_set_text(GTK_LABEL(app_ui_data->controls_state_label),
                           _("game_ia_startup_not_started"));

        ui_change_play_continue_button(get_app_data(),CHANGE_PLAY);

        gtk_widget_set_sensitive(app_ui_data->restart_button, FALSE);
        gtk_widget_set_sensitive(app_ui_data->play_button, TRUE);

        menu_set_restart_dimmed(app_ui_data, FALSE);
        menu_set_save_dimmed(app_ui_data, FALSE);
    }
    /* Game is paused */
    else if (gs == gs_paused)
    {

        gtk_label_set_text(GTK_LABEL(app_ui_data->controls_state_label),
                           _("game_ia_startup_paused"));
        ui_change_play_continue_button(get_app_data(),CHANGE_CONTINUE);

        gtk_widget_set_sensitive(app_ui_data->restart_button, TRUE);
        if (app_ui_data->window_front_needed)
        {
            gtk_window_present(GTK_WINDOW(app_ui_data->main_view));
            app_ui_data->window_front_needed = FALSE;
        }

        menu_set_restart_dimmed(app_ui_data, TRUE);
        menu_set_save_dimmed(app_ui_data, TRUE);
    }
    /* Game is closed */
    else if (gs == gs_closed)
    {
        gtk_label_set_text(GTK_LABEL(app_ui_data->controls_state_label),
                           _("game_ia_startup_not_started"));

        ui_change_play_continue_button(get_app_data(),CHANGE_PLAY);

        gtk_widget_set_sensitive(app_ui_data->restart_button, FALSE);
        gtk_widget_set_sensitive(app_ui_data->play_button, TRUE);

        menu_set_restart_dimmed(app_ui_data, FALSE);
        menu_set_save_dimmed(app_ui_data, FALSE);
    }

    /* For show infoprint if user selects dimmed command in menu */
    ui_set_dimmed_menu_item_callback(app_ui_data);

    return TRUE;
}

void
ui_create_games_common_view(AppUIData * app_ui_data)
{
    /* Get game state */
    GameState gs = get_game_state();

    GtkWidget *alignment;

    alignment = gtk_alignment_new(0.5f, 0.5f, 0.0f, 1.0f);

    gtk_alignment_set_padding(GTK_ALIGNMENT(alignment),
		    	      HILDON_MARGIN_HALF,
		    	      HILDON_MARGIN_DEFAULT,
		    	      HILDON_MARGIN_DOUBLE,
		    	      HILDON_MARGIN_DOUBLE);

    /* Create a vbox for all our stuff inside main view */
    app_ui_data->main_vbox = gtk_vbox_new(FALSE, HILDON_MARGIN_DEFAULT);    
    g_assert(app_ui_data->main_vbox);

    gtk_widget_set_size_request (app_ui_data->main_vbox, GAME_IMAGE_WIDTH, -1);
     
    gtk_container_add(GTK_CONTAINER(app_ui_data->main_view),
   		      alignment);

    gtk_container_add (GTK_CONTAINER(alignment),
		       app_ui_data->main_vbox);	

    /* Game title graph */
    app_ui_data->game_image = gtk_image_new();
    g_assert(app_ui_data->game_image);
    
    gtk_box_pack_start(GTK_BOX(app_ui_data->main_vbox),
		       app_ui_data->game_image, FALSE, FALSE, 0);
     
    /* Status label */
    app_ui_data->controls_state_label = NULL;
    if (gs == gs_unloaded)
    {
        app_ui_data->controls_state_label =
            gtk_label_new(_("game_ia_startup_not_started"));
    }
    else if (gs == gs_closed)
    {
        app_ui_data->controls_state_label =
            gtk_label_new(_("game_ia_startup_not_started"));
    }
    else
    {
        app_ui_data->controls_state_label =
            gtk_label_new(_("game_ia_startup_paused"));
    }

    if (app_ui_data->controls_state_label != NULL)
    {
        g_assert(app_ui_data->controls_state_label);
	gtk_box_pack_start(GTK_BOX(app_ui_data->main_vbox),
			   app_ui_data->controls_state_label,
			   FALSE, FALSE, 0);	
    }

    /* Create a hbox for common controls */
    app_ui_data->controls_hbox = gtk_hbox_new(FALSE, 0);
    g_assert(app_ui_data->controls_hbox);
    gtk_box_pack_start(GTK_BOX(app_ui_data->main_vbox),
		       app_ui_data->controls_hbox, FALSE, FALSE, 0);

    /* Continue button */
    app_ui_data->continue_button =
        gtk_button_new_with_label(_("game_bv_startup_continue"));
    g_assert(app_ui_data->continue_button);
  
  hildon_gtk_widget_set_theme_size (app_ui_data->continue_button,
				      HILDON_SIZE_FINGER_HEIGHT);
  g_object_set (app_ui_data->continue_button,
	        "width-request", CTRL_BUTTON_WIDTH,
	         NULL);	

    g_signal_connect(G_OBJECT(app_ui_data->continue_button), "clicked",
                     G_CALLBACK(game_continue_cb), (gpointer) app_ui_data);

     gtk_container_add (GTK_CONTAINER(app_ui_data->controls_hbox),
		       app_ui_data->continue_button);		    

    gtk_widget_hide(app_ui_data->continue_button);

    /* Play button */
    app_ui_data->play_button =
        gtk_button_new_with_label(_("game_bv_startup_play"));
    g_assert(app_ui_data->play_button);
  
     hildon_gtk_widget_set_theme_size (app_ui_data->play_button,
				      HILDON_SIZE_FINGER_HEIGHT);

    g_object_set (app_ui_data->play_button,
		  "width-request", CTRL_BUTTON_WIDTH,
		  NULL);


    g_signal_connect(G_OBJECT(app_ui_data->play_button), "clicked",
                     G_CALLBACK(game_play_cb), (gpointer) app_ui_data);

   gtk_container_add(GTK_CONTAINER(app_ui_data->controls_hbox),
		      app_ui_data->play_button);

    /* Show proper buttons */
    if (gs == gs_unloaded || gs == gs_closed)
    {
        ui_change_play_continue_button(get_app_data(),CHANGE_PLAY);
    }
    else if (gs == gs_paused)
    {
        ui_change_play_continue_button(get_app_data(),CHANGE_CONTINUE);
    }

    /* Restart button */
    app_ui_data->restart_button =
        gtk_button_new_with_label(_("game_bv_startup_restart"));
    g_assert(app_ui_data->restart_button);


    hildon_gtk_widget_set_theme_size (app_ui_data->restart_button,
				      HILDON_SIZE_FINGER_HEIGHT);

    g_object_set (app_ui_data->restart_button,
		  "width-request", CTRL_BUTTON_WIDTH,
  		   NULL);

    g_signal_connect(G_OBJECT(app_ui_data->restart_button), "clicked",
                     G_CALLBACK(game_restart_cb), (gpointer) app_ui_data);

   
   gtk_container_add(GTK_CONTAINER(app_ui_data->controls_hbox),
  	             app_ui_data->restart_button);		   

    /* Set button dimmed if required */
    if (gs == gs_unloaded || gs == gs_closed)
    {
        gtk_widget_set_sensitive(app_ui_data->restart_button, FALSE);
    }
    if (gs == gs_paused)
    {
        gtk_widget_set_sensitive(app_ui_data->restart_button, TRUE);
    }
}


/* Create the INDT games view */
void
ui_create_indt_view(AppUIData * app_ui_data)
{
    HildonWindow *result;
    const gchar *game_gettext_package;
    const gchar *msgid;
    GtkWidget *game_widget = NULL;
    
    result = HILDON_WINDOW(hildon_window_new());
    game_gettext_package =
        startup_config_read_entry(startup_app_get_config(app_data->startup_app),
                                  "GettextPackage", NULL);
    if (app_data->plugin && game_gettext_package)
        app_data->plugin->gettext_package = game_gettext_package;

    game_gettext_package =
        startup_config_read_entry(startup_app_get_config(app_data->startup_app),
                                  "GettextPackage", NULL);
    msgid =
        startup_config_read_entry(startup_app_get_config(app_data->startup_app),
                                  "TitleId", NULL);

    if (game_gettext_package && msgid)
    {
        app_ui_data->game_title = dgettext(game_gettext_package, msgid);
    }

    if (!app_ui_data->game_title || app_ui_data->game_title == msgid)
    {
        app_ui_data->game_title =
            (gchar *)
            startup_config_read_entry(startup_app_get_config(app_data->startup_app),
                                      "Title", NULL);
    }
    g_assert(app_ui_data->game_title);

    g_set_application_name(app_ui_data->game_title);
    g_signal_connect(G_OBJECT(result), "key_press_event",
                     G_CALLBACK(key_press), (gpointer) app_ui_data);

    /* Catch delete event for save query */
    g_signal_connect(G_OBJECT(result), "delete_event",
                     G_CALLBACK(on_delete_event), (gpointer) (app_ui_data));
    g_signal_connect(G_OBJECT(result), "show",
                     G_CALLBACK(on_window_realized),
                     (gpointer) (app_ui_data));
    g_assert(result);
    app_ui_data->main_view = result;

    g_signal_connect(G_OBJECT(app_ui_data->main_view), "destroy",
                     G_CALLBACK(game_quit_cb), (gpointer) app_ui_data);

    /* Make common view */
    ui_create_games_common_view(app_ui_data);

    /* Load the application specific part from the plugin */
    app_ui_data->game_hbox = gtk_hbox_new(FALSE, 0);
    g_assert(app_ui_data->game_hbox);

    gtk_container_add(GTK_CONTAINER(app_ui_data->main_vbox),
                      app_ui_data->game_hbox);

    if (app_data->plugin)
        game_widget = app_data->plugin->info->load();
    else
        game_widget = gtk_vbox_new(FALSE, 0);
    g_assert(game_widget);

    gtk_container_add(GTK_CONTAINER(app_ui_data->main_vbox), game_widget);

    hildon_program_add_window(app_ui_data->app, app_ui_data->main_view);

    gtk_widget_show_all(GTK_WIDGET(app_ui_data->main_view));
    return;
}

void
ui_repaint(AppData * app_data)
{
    osso_log(LOG_DEBUG, "%s\n", __FUNCTION__);
    ui_update_status(app_data->app_ui_data);
}


gboolean
ui_show_restart_game_dialog(AppUIData * app_ui_data)
{
    gboolean answer = FALSE;
    HildonNote *note =
        HILDON_NOTE(hildon_note_new_confirmation
                    (GTK_WINDOW(app_ui_data->main_view),
                     _("game_nc_restart_game")));
    g_signal_connect(G_OBJECT(note), "key_press_event",
                     G_CALLBACK(dialog_key_press), NULL);

    /* Rama - store current active dialog */
    app_ui_data->active_dialog=(GtkWidget *)note;

    answer = gtk_dialog_run(GTK_DIALOG(note));
    gtk_widget_destroy(GTK_WIDGET(note));

    app_ui_data->active_dialog=NULL;
    return answer;
}

void
ui_set_dimmed_menu_item_callback(AppUIData * app_ui_data)
{
}


void
ui_show_save_nomem_dialog(AppUIData * app_ui_data)
{
    HildonNote *note =
        HILDON_NOTE(hildon_note_new_information
                    (GTK_WINDOW(app_ui_data->main_view),
                     dgettext("hildon-common-strings",
                              "sfil_ni_not_enough_memory")));
    hildon_note_set_button_text(note,
                                dgettext("hildon-common-strings",
                                         "sfil_ni_not_enough_memory_ok"));
    g_signal_connect(G_OBJECT(note), "key_press_event",
                     G_CALLBACK(save_nomem_key_press), app_ui_data);
    g_signal_connect(G_OBJECT(note), "response",
                     G_CALLBACK(save_nomem_response), app_ui_data);
    app_ui_data->nomem_dialog = GTK_WIDGET(note);
    gtk_widget_show_all(GTK_WIDGET(note));

    gtk_window_set_title(GTK_WINDOW(app_ui_data->main_view), "");
    settings_set_string(SETTINGS_CHESS_SAVE_FILE, "");
}


void
ui_hide_save_nomem_dialog(AppUIData * app_ui_data)
{
    if (app_ui_data->nomem_dialog)
        gtk_widget_destroy(GTK_WIDGET(app_ui_data->nomem_dialog));
    app_ui_data->nomem_dialog = NULL;
}
