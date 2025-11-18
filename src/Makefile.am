# Makefile.am for OSSO Games Start-Up

bin_PROGRAMS = osso_games_startup

osso_games_startup_CFLAGS = \
	-DLOCALEDIR=\"$(localedir)\" \
	-DPIXMAPSDIR=\"$(pixmapsdir)\" \
	-DPREFIX=\"$(prefix)\" -I$(top_srcdir) \
	$(HILDON_CFLAGS) \
	$(GDEPS_CFLAGS) \
	$(MAEMO_LAUNCHER_CFLAGS)

osso_games_startup_SOURCES = \
	main.c main.h \
	dbus.c dbus.h \
	settings.c settings.h \
	state_save.c state_save.h \
	startup_communication.c startup_communication.h \
	startup_gamemanager.c startup_gamemanager.h \
	startup_state.h appdata.h \
	startup_i18n.h \
	ui/interface.h ui/interface.c \
	ui/callbacks.h ui/callbacks.c

osso_games_startup_LDFLAGS = $(MAEMO_LAUNCHER_LIBS)
osso_games_startup_LDADD = $(HILDON_LIBS) $(GDEPS_LIBS)

osso_games_startup_includedir = $(includedir)
osso_games_startup_include_HEADERS = ui/startup_plugin.h ui/startup_i18n.h startup_app.h startup_config.h
