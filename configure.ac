#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([triglav], [0.0.0], [pawel.f.biernacki@gmail.com])
AC_CONFIG_SRCDIR([inc/triglav.h])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE()

AC_CONFIG_MACRO_DIRS([m4])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AM_PROG_LEX
AC_PROG_YACC

LT_INIT

PKG_PROG_PKG_CONFIG
PKG_INSTALLDIR

# Checks for libraries.
PKG_CHECK_MODULES([GTK], [gtkmm-3.0 >= 3.0.0])


#
# --without-xml can be used to disable XML handling, then libxml-2.0 is not used
#
AC_ARG_WITH([xml],
		[AS_HELP_STRING([--without-xml],
			[disable XML])],
		[],
		[xml=yes])
if test "$xml" == "yes"
then
	if [ pkg-config --exists libxml-2.0 ]; then
		AC_MSG_NOTICE([XML enabled])
		AC_DEFINE(TRIGLAV_XML_ON, [1], [Undefine to disable XML.])
		AC_SUBST(XML_CFLAGS, `pkg-config --cflags libxml-2.0`)
		AC_SUBST(XML_LIBS, `pkg-config --libs libxml-2.0`)
		AC_SUBST(CFLAGS, ["$CFLAGS $XML_CFLAGS"])
		AC_SUBST(CPPFLAGS, ["$CPPFLAGS $XML_CFLAGS"])
		AC_CHECK_HEADERS([libxml/xmlreader.h], [], 
		[
	AC_MSG_ERROR(
[The libxml-2.0 library was found, but there is no corresponding header file. Check that the DEVEL version of the library 
is installed on your system.])
		])
	else
		AC_MSG_WARN([
The library libxml-2.0 not found. Check that it is installed on your system. If it is not, download and install it 
(preferably using your configuration manager -	i.e. the application called Add/Remove Programs or something like dnf.) 
Remember to install the DEVEL version. If it is installed elswehere you can run the script "configure" with the option --libdir=DIR. 
However, since the XML support is optional, you may ignore this message. The triglav build system has automatically disabled XML parsing and XML writing.
You can suppress this message by calling "configure --without-xml".
Execute "configure -h" for more info.	
])
	fi
else
	AC_MSG_NOTICE([xml disabled])
fi


# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 examples/Makefile
                 image/Makefile
                 inc/Makefile
                 perl/Makefile
                 src/Makefile
                 libtriglav.pc
                 demo/Makefile
                 dox/Makefile
                 ])
AC_OUTPUT
