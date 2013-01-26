################################################################
#
# Makefile for the artdata package.
#
# Artificial data.
#
# Libraries needed: KLutil, KLran, KLfunc, KLcfitio, cfitsio, KLfft
#
# Coding language: C
# Compiler: gcc
# OS: Redhat Linux
#
################################################################

MAKE = make
GZIP = gzip -S .gz

#### Environment Variable Definitions ####
UNAME = `uname -s`
OS = `uname -s`
PREFIX = $(HOME)/local
INCLDIR = $(PREFIX)/include
LIBDIR = $(PREFIX)/lib

EPREFIX = $(PREFIX)
LPREFIX = $(EPREFIX)
IPREFIX = $(EPREFIX)
SPREFIX = $(EPREFIX)/packages

#### Compiler Variation Definitions ####
CC = gcc
CFLAGS = -g -O2 -I$(INCLDIR)

################## DO NOT EDIT BELOW THIS LINE #################

#### Variable Definitions ####
PKGNAME = artdata
PARAM_FILES = mksnrpop.param artimg.param
OTHER_FILES = testsnr.pop noiseimg.fits
BASIC_FILES = Makefile Changes TODO $(PARAM_FILES) $(OTHER_FILES)
DOCS = doc.txt
VERSION = 0
RELEASE = $(VERSION).1.4

#### Object Definitions ####
OBJ_ARTDATA = rd_param.o artdata.o
OBJ_MKPOP = mksnrpop.o gcpos.o snrdiam.o snrlum.o
OBJ_ARTIMG = artimg.o artimg_bkg.o artimg_integrate.o artimg_pnoise.o \
	artimg_rdnoise.o artimg_seeing.o artimg_snr.o artimg_xnoise.o \
	parse_popheader.o
OBJECTS = $(OBJ_ARTDATA) $(OBJ_MKPOP) $(OBJ_ARTIMG)
SRCS = $(OBJECTS:.o=.c)
HEADERS = $(PKGNAME).h rd_param.h mksnrpop.h
BINS = mksnrpop artimg
LIBS_COMMON = -L$(LIBDIR) -lKLutil -lm
LIBS_MKPOP = -lKLran
LIBS_ARTIMG = -lKLran -lKLfunc -lKLfft -lKLcfitsio -lcfitsio 

#### Rule Definition ####
%.o : %.c
	$(CC) -c $(CFLAGS) $<

main : configure $(OBJECTS) $(BINS)

.PHONY: configure
configure:
# configure artdata.h
	@echo -n "Configuring artdata.h ... "
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

.PHONY: mkpop $(BINS)
mkpop: $(OBJ_ARTDATA) $(OBJ_MKPOP) $(HEADERS) $(BINS)

mksnrpop: $(OBJ_ARTDATA) $(OBJ_MKPOP) $(HEADERS)
	$(CC) $(CFLAGS) -o mksnrpop $(OBJ_ARTDATA) $(OBJ_MKPOP) \
		$(LIBS_MKPOP) $(LIBS_COMMON)

artimg: $(OBJ_ARTDATA) $(OBJ_ARTIMG) $(HEADERS)
	$(CC) $(CFLAGS) -o artimg $(OBJ_ARTDATA) $(OBJ_ARTIMG) \
		$(LIBS_ARTIMG) $(LIBS_COMMON)

.PHONY: all
all : main

## Installing ##
.PHONY: install install-src install-all
install:
	install -d -m 0755 $(EPREFIX)/bin
	for bin in $(BINS) ; do \
          install -m 0755 $$bin $(EPREFIX)/bin/; \
        done
	for file in $(PARAM_FILES) ; do \
          install -m 0644 $$file $(EPREFIX)/bin/; \
        done

install-src:
	install -d -m 0755 $(SPREFIX)/$(PKGNAME)
	for src in $(SRCS) ; do \
          install -m 0644 $$src $(SPREFIX)/$(PKGNAME); \
        done
	for file in $(HEADERS) ; do \
          install -m 0644 $$file $(SPREFIX)/$(PKGNAME)/; \
        done
	for file in $(BASIC_FILES) ; do \
          install -m 0644 $$file $(SPREFIX)/$(PKGNAME); \
        done
	for file in $(DOCS) ; do \
          install -m 0644 $$file $(SPREFIX)/$(PKGNAME)/; \
        done

install-all: install install-src

## Packaging ##
.PHONY: package-src
package-src:
	tar cvf $(PKGNAME)-$(RELEASE).tar $(BASIC_FILES) $(DOCS) $(SRCS) \
                $(HEADERS)
	$(GZIP) $(PKGNAME)-$(RELEASE).tar

## Cleaning ##
.PHONY: clean
clean:
	\rm -f $(OBJECTS) $(BINS)

### End of Makefile ###
