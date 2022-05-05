

bin_PROGRAMS = triglav_demo

triglav_demo_SOURCES = demo.cc triglav_demo_window.cc

triglav_demo_LDADD = -L../src -ltriglav @GTK_LIBS@ @XML_LIBS@

AM_CXXFLAGS = @CXXFLAGS@ \
	-I.. -I../inc @GTK_CFLAGS@ @XML_CFLAGS@ -fconcepts -std=c++17 \
	-DDATADIR=\"$(datadir)/triglav_demo\"

triglav_demodir = $(datadir)

triglav_demo_DATA = ../image/*.png


	
