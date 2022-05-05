

SUBDIRS = examples dox image inc perl src demo

# Install the pkg-config file; the directory is set using
# PKG_INSTALLDIR in configure.ac.
pkgconfig_DATA = libtriglav.pc


EXTRA_DIST = Doxyfile

documents:
	doxygen && cp dox/www_perkun_org.html_files/*.png doc/html
	
