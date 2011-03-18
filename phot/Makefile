################################################################
#
# Makefile for phot package
#
# Photometry package
#
# Libraries needed: KLutil, KLfile, KLinter, KLcfitsio, CFITSIO
#
# Coding language: C
# Compiler: gcc
# OS: Redhat Linux, Mac OS X
#
################################################################

MAKE = make
GZIP = gzip -S .gz

#### Environment Variable Definitions ####
OS = macosx
PREFIX = /Users/klabrie/prgc
INCLDIR = $(PREFIX)/include
LIBDIR = $(PREFIX)/lib

EPREFIX = $(PREFIX)
LPREFIX = $(EPREFIX)
IPREFIX = $(EPREFIX)
SPREFIX = $(EPREFIX)/packages/

#### Compiler Variable Definitions ####
CC = gcc
CFLAGS = -g -O2 -I$(INCLDIR)

################## DO NOT EDIT BELOW THIS LINE #################

#### Variable Definitions ####
PKGNAME = phot
BASIC_FILES = Makefile Changes TODO apphot.param data.param fitsky.param \
	phot.param
FILTERS = cfh5103c_approx.dat cfh5201c_approx.dat
DOCS = doc.txt
VERSION = 0
RELEASE = $(VERSION).3.1

#### Object Definitions ####
OBJ_PHOT = rd_param.o rd_coords.o phot.o mag.o
OBJ_APPHOT = apphot.o fitsky.o aptotflux.o print_result.o skyvalue.o
OBJ_ABSCALIB = abscalib.o
OBJ_REDCOR = redcor.o filterID.o get_transmission.o int_correction.o \
	frg_correction.o
OBJECTS = $(OBJ_PHOT) $(OBJ_APPHOT) $(OBJ_ABSCALIB) $(OBJ_REDCOR)
SRCS = $(OBJECTS:.o=.c)
HEADERS = $(PKGNAME).h redcor.h
BINS = apphot abscalib redcor
LIBS_COMMON = -lKLutil -lm
LIBS_FITSIO = -lKLcfitsio -lcfitsio
LIBS_APPHOT = -L$(LIBDIR) $(LIBS_FITSIO) -lKLstats -lKLsort $(LIBS_COMMON)
LIBS_ABSCALIB = -L$(LIBDIR) -lKLfile -lKLinter $(LIBS_COMMON)
LIBS_REDCOR = -L$(LIBDIR) -lKLfile $(LIBS_COMMON)

#### Test suite ####
TEST_FILES = cfh5002.dat fakeimg.coo fakeimg.fits
TEST_OBJ = fakeimg.o
TEST_SRC = $(TEST_OBJ:.o=.c)
TEST_BINS = fakeimg
TEST_LIBS = -L$(LIBDIR) $(LIBS_FITSIO) $(LIBS_COMMON)

#### Rule Definition ####
%.o : %.c
	$(CC) -c $(CFLAGS) $<

main : configure $(OBJECTS) $(BINS)

configure:
# configure phot.h
	@echo -n "Configuring phot.h ... "
	@\rm -f tmp
	@sed "s!DEFAULT_DIR \".*\"!DEFAULT_DIR \"$(EPREFIX)/bin/\"!" phot.h > tmp
	@if [ -s tmp ]; then \
	  mv -f tmp phot.h; \
	else \
	  echo "Error configuring $(PKGNAME).h"; \
	fi
	@echo "done"

.PHONY: $(BINS)
apphot: $(OBJ_APPHOT) $(HEADERS)
	$(CC) $(CFLAGS) -o apphot $(OBJ_PHOT) $(OBJ_APPHOT) $(LIBS_APPHOT)

abscalib: $(OBJ_ABSCALIB) $(HEADERS)
	$(CC) $(CFLAGS) -o abscalib $(OBJ_ABSCALIB) $(LIBS_ABSCALIB)

redcor: $(OBJ_REDCOR) $(HEADERS)
	$(CC) $(CFLAGS) -o redcor $(OBJ_REDCOR) $(LIBS_REDCOR)

.PHONY: all
all : main test

.PHONY: $(TEST_BINS)
test: $(TEST_OBJ)
	$(CC) $(CFLAGS) -o fakeimg fakeimg.o $(TEST_LIBS)

## Installing ##
.PHONY: install install-src
install:
	@install -d -m 0755 $(EPREFIX)/bin
	@for bin in $(BINS) ; do \
	  install -m 0755 $$bin $(EPREFIX)/bin/; \
	done
	@install -d -m 0755 $(EPREFIX)/bin/filters
	@for filter in $(FILTERS) ; do \
	  install -m 0644 $$filter $(EPREFIX)/bin/filters; \
	done

install-src:
	@install -d -m 0755 $(SPREFIX)/$(PKGNAME)
	@for src in $(SRCS) ; do \
	  install -m 0644 $$src $(SPREFIX)/$(PKGNAME)/; \
	done
	@for file in $(HEADERS) ; do \
	  install -m 0644 $$file $(SPREFIX)/$(PKGNAME)/; \
	done
	@for file in $(BASIC_FILES) ; do \
	  install -m 0644 $$file $(SPREFIX)/$(PKGNAME)/; \
	done
	@for file in $(DOCS) ; do \
	  install -m 0644 $$file $(SPREFIX)/$(PKGNAME)/; \
	done
	@for file in $(TEST_FILES) ; do \
	  install -m 0644 $$file $(SPREFIX)/$(PKGNAME)/; \
	done
	@for file in $(TEST_SRC) ; do \
	  install -m 0644 $$file $(SPREFIX)/$(PKGNAME)/; \
	done
	@for file in $(FILTERS) ; do \
	  install -m 0644 $$file $(SPREFIX)/$(PKGNAME)/; \
	done

## Packaging ##
.PHONY: package-src
package-src:
	tar cvf $(PKGNAME)-$(RELEASE).tar $(BASIC_FILES) $(DOCS) $(SRCS) \
		$(HEADERS) $(TEST_FILES) $(TEST_SRC) $(FILTERS)
	$(GZIP) $(PKGNAME)-$(RELEASE).tar

## Cleaning ##
.PHONY: clean
clean:
	\rm -f $(OBJECTS) $(BINS) $(TEST_OBJ) $(TEST_BINS)

### End of Makefile ###
