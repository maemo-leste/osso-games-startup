AC_INIT([osso-games-startup], [1.4.38])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([src/main.h])
AM_CONFIG_HEADER([config.h])

AC_ISC_POSIX
AC_PROG_CC
AC_PROG_CC_STDC
AC_HEADER_STDC
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_LIBTOOL

CFLAGS="$CFLAGS -Werror -Wall -Wmissing-prototypes -Wmissing-declarations"

PKG_CHECK_MODULES(GDEPS, gtk+-2.0 >= 2.2 glib-2.0 >= 2.2 gconf-2.0 >= 2.2 gnome-vfs-2.0 >= 2.2 hildon-fm-2 >= 0.8.1)
AC_SUBST(GDEPS_CFLAGS)
AC_SUBST(GDEPS_LIBS)

PKG_CHECK_MODULES(HILDON, hildon-1 >= 0.8.1 hildon-fm-2 >= 0.8.1 libosso >= 0.8.1 )
AC_SUBST(HILDON_CFLAGS)
AC_SUBST(HILDON_LIBS)

desktopentrydir=/usr/share/applications
AC_SUBST(desktopentrydir)

hildondesktopentrydir=${desktopentrydir}/hildon
hildoncpdesktopentrydir=`$PKG_CONFIG --variable=desktopentrydir hildon-control-panel`
hildonpixmapdir=`$PKG_CONFIG --variable=hildonpixmapdir hildon-base`
hildonstatusbarlibdir=`$PKG_CONFIG --variable=hildonstatusbarlibdir hildon-base`

AC_SUBST(hildondesktopentrydir)
AC_SUBST(hildoncpdesktopentrydir)
AC_SUBST(hildonpixmapdir)
AC_SUBST(hildonstatusbarlibdir)

localedir=${datadir}/locale
AC_SUBST(localedir)

pixmapsdir=${datadir}/pixmaps
AC_SUBST(pixmapsdir)

dbuslibpathdir=`pkg-config --variable=libdir dbus-1`
dbusconfdir=/etc/dbus-1/system.d
dbusservdir=${dbuslibpathdir}/dbus-1.0/services
AC_SUBST(dbuslibpathdir)
AC_SUBST(dbusconfdir)
AC_SUBST(dbusservdir)

GETTEXT_PACKAGE=osso-games
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED( GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Name of gettext package] )
ALL_LINGUAS="de_DE en_GB en_US es_ES es_MX fi_FI fr_CA fr_FR it_IT pt_BR ru_RU"
AM_GLIB_GNU_GETTEXT

AC_CONFIG_FILES(
Makefile
src/Makefile
)

AC_OUTPUT
