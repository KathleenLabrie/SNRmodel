#################################################################
#
# Makefile for snrpopfit
#
# Just snrpopfit.  Generate the chisq map for a parameter space
# in n0, snrate, and nsnr.  Compare artificial images to real 
# image pixel by pixel.
#
# Libraries needed: KLstats, KLfile, KLutil, KLsort, KLfft, KLfunc
#                   KLcfitsio, CFITSIO, phot, PGPlot, PGxtal
#
# Coding language: C
# Compiler: gcc
# OS: Redhat Linux
#
#################################################################

MAKE = make
GZIP = gzip -S .gz

#### Environment Variable Definitions ####
UNAME = `uname -s`
OS = redhat
PREFIX = $(HOME)/prgc
INCLDIR = $(PREFIX)/include
LIBDIR = $(PREFIX)/lib

EPREFIX = $(PREFIX)
LPREFIX = $(EPREFIX)
IPREFIX = $(EPREFIX)
SPREFIX = $(EPREFIX)/packages

#PGPLOT_DIR = /usr/local/packages/pgplot

#### Compiler Variable Definitions ####
CC = gcc
CFLAGS = -g -O2 -I./ -I$(INCLDIR) -I$(PGPLOT_DIR)
FC = g77
FFLAGS = -g -O2 -I$(PGPLOT_DIR)

################## DO NOT EDIT BELOW THIS LINE ##################

#### Variable Definitions ####
PKGNAME = snrpopfit
PARAM_FILES = snrpopfit.param
BASIC_FILES = Makefile Changes TODO $(PARAM_FILES)
DOCS = doc.txt
VERSION = 0
RELEASE = $(VERSION).2.3

DEVEL_FILES = masksaturated.dat artimg.param n1569f1_e9801_FeIIline.fits \
	n1569f1.fits noiseimg.fits mksnrpop.param apphot.param \
	data.param phot.param fitsky.param

UNKNOWN_FILES = addsrc.pl fitn5253chisq1.dat ggobidata.xml junk junk.std \
        testf.f

#### Object Definitions ####
OBJ_SNRPOPFIT = snrpopfit.o rd_param.o aperture_phot.o
OBJ_SNRPOPBFIT = snrpopbfit.o
OBJ_PLOT = plotsurf.o ggobixmlfmt4.o
FOBJ_PLOT = pgplot2d.o
OBJECTS = $(OBJ_SNRPOPFIT) $(OBJ_SNRPOPBFIT) $(OBJ_PLOT)
FOBJECTS = $(FOBJ_PLOT) 
SRCS = $(OBJECTS:.o=.c) $(FOBJECTS:.o=.f)
HEADERS = $(PKGNAME).h rd_param.h
BINS = snrpopfit snrpopbfit #plotsurf
LIB_COMMON = -L$(LIBDIR) -lKLfile -lKLutil -lm
LIB_SNRPOPFIT = -lphot -lKLstats -lKLsort -lKLfft -lKLfunc -lKLcfitsio \
	-lcfitsio $(LIB_COMMON)
LIB_SNRPOPBFIT = $(LIB_COMMON)
LIB_PLOT = -L$(PGPLOT_DIR) -lcpgplot -lpgplot \
	-L/usr/X11R6/lib -lX11 -lnr $(LIB_COMMON)
PGXTAL_OBJ = /usr/local/src/pgxtal/plot2db3.o /usr/local/src/pgxtal/pgcell.o \
	/usr/local/src/pgxtal/dsqinf.o /usr/local/src/pgxtal/pgxtal.o

#### Rule Definition ####
%.o : %.c
	$(CC) -c $(CFLAGS) $<
%.o : %.f
	$(FC) -c $(FFLAGS) $<

main : configure $(OBJECTS) $(FOBJECTS) $(BINS)

.PHONY: configure
configure:
# configure snrpopfit.h
	@echo -n "Configuring snrpopfit.h ... "
	@\rm -f tmp
	@sed "s!DEFAULT_DIR \".*\"!DEFAULT_DIR \"$(EPREFIX)/bin/\"!" \
                                                        $(PKGNAME).h  > tmp
	@if [ -s tmp ]; then \
          mv -f tmp $(PKGNAME).h; \
	else \
          echo "Error configuring $(PKGNAME).h"; \
	fi
	@\rm -f tmp
	@sed "s!BINDIR \".*\"!BINDIR \"$(PREFIX)/\"!" $(PKGNAME).h  > tmp
	@if [ -s tmp ]; then \
          mv -f tmp $(PKGNAME).h; \
	else \
          echo "Error configuring $(PKGNAME).h"; \
	fi
	@echo "done"


.PHONY: $(BINS)
snrpopfit: $(OBJ_SNRPOPFIT) $(HEADERS)
	$(CC) $(CFLAGS) -o snrpopfit $(OBJ_SNRPOPFIT) $(LIB_SNRPOPFIT)
snrpopbfit: $(OBJ_SNRPOPBFIT) $(HEADERS)
	$(CC) $(CFLAGS) -o snrpopbfit $(OBJ_SNRPOPBFIT) $(LIB_SNRPOPBFIT)
plotsurf: $(FOBJ_PLOT) $(OBJ_PLOT) $(HEADERS)
	$(FC) $(CFLAGS) -o plotsurf $(FOBJ_PLOT) $(OBJ_PLOT) $(LIB_PLOT) \
	  $(PGXTAL_OBJ)

.PHONY: all
all : main

## Installing ##
.PHONY: install install-src
install:
	@install -d -m 0755 $(EPREFIX)/bin
	@for bin in $(BINS) ; do \
	  install -m 0755 $$bin $(EPREFIX)/bin; \
	done
	@for file in $(PARAM_FILES) ; do \
	  install -m 0644 $$file $(EPREFIX)/bin; \
	done

install-src:
	@install -d -m 0755 $(SPREFIX)/$(PKGNAME)
	@install -m 0644 $(SRCS) $(HEADERS) $(BASIC_FILES) $(DOCS) \
		$(SPREFIX)/$(PKGNAME)/
	@install -m 0644 $(DEVEL_FILES) $(SPREFIX)/$(PKGNAME)/
	@install -d -m 0755 $(SPREFIX)/$(PKGNAME)/devel
	@cd devel; install -m 0644 $(UNKNOWN_FILES) $(SPREFIX)/$(PKGNAME)/devel/

## Packaging ##
.PHONY: package-src
package-src:
	tar cvf $(PKGNAME)-$(RELEASE).tar $(BASIC_FILES) $(DOCS) $(SRCS) \
		$(HEADERS) $(DEVEL_FILES) devel/*
	$(GZIP) $(PKGNAME)-$(RELEASE).tar

## Cleaning ##
.PHONY: clean
clean:
	\rm -f $(OBJECTS) $(BINS)

### End of Makefile ###
