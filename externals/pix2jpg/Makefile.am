AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I $(top_srcdir)/src/m4
AM_CPPFLAGS = -I$(top_srcdir)/src -I/usr/include/ImageMagick
EXTRA_DIST =



extradir=$(libdir)/pix2jpg

extra_LTLIBRARIES = pix2jpg.la
 
dist_extra_DATA  = LICENSE.txt
dist_extra_DATA += pix2jpg-help.pd


## some default flags
pix2jpg_la_CXXFLAGS = 
pix2jpg_la_LDFLAGS  = -L/usr/lib -Wl,-Bsymbolic-functions -Wl,-z,relro -L/usr/lib/X11 -module -avoid-version -shared -shrext .@GEM_RTE_EXTENSION@ 
if WINDOWS
pix2jpg_la_LDFLAGS += -no-undefined
endif
pix2jpg_la_LIBADD   =

# RTE flags
pix2jpg_la_CXXFLAGS += @GEM_RTE_CFLAGS@ @GEM_ARCH_CXXFLAGS@
pix2jpg_la_LIBADD   += @GEM_RTE_LIBS@

pix2jpg_la_LDFLAGS += @GEM_ARCH_LDFLAGS@

# special flags for building externals
pix2jpg_la_CXXFLAGS += -g -O2 -fstack-protector --param=ssp-buffer-size=4 -Wformat -Wformat-security -pthread .@GEM_EXTERNAL_CFLAGS@
pix2jpg_la_LIBADD += -L/usr/lib -lMagick++ -lMagickWand -lMagickCore -llcms -ltiff -lfreetype -ljpeg -llqr-1 -lglib-2.0 -lfontconfig -lXext -lSM -lICE -lX11 -lXt -lbz2 -lz -lm -lgomp -lpthread -lltdl -L$(top_builddir) @GEM_EXTERNAL_LIBS@


# convenience symlinks
## uha, this is ugly
pix2jpg.@GEM_RTE_EXTENSION@: pix2jpg.la
	rm -f $@
	test -f .libs/$@ && $(LN_S) .libs/$@ $@ || true

.PHONY: clean-conviencesymlink

clean-conviencesymlink:
	rm -f pix2jpg.@GEM_RTE_EXTENSION@


all-local:: pix2jpg.@GEM_RTE_EXTENSION@

clean-local:: clean-conviencesymlink


### SOURCES
pix2jpg_la_SOURCES = \
	pix2jpg.cpp \
	pix2jpg.h

