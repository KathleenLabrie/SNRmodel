#################################################################
#
# Makefile for the Easyreg package.
#
# Routines to register, align and combine images.
#
# Packages required:
# Libraries required: cfitsio, KLcfitsio, KLutil, KLstats, KLimgmath,
#			KLfile
#
# Coding languages: C
# Compilers: gcc
# OS: Redhat Linux
#
#################################################################

MAKE = make
GZIP = gzip -S .gz

#### Environment Variable Definitions ####
OS = redhat
PREFIX = /home/labrie/prgc
INCLDIR = $(PREFIX)/include
LIBDIR = $(PREFIX)/lib

EPREFIX = $(PREFIX)/img/immatch
LPREFIX = $(EPREFIX)
IPREFIX = $(EPREFIX)
SPREFIX = $(EPREFIX)

#### Compiler Variable Definitions ####
CC = gcc
CFLAGS = -g -O2 -I./ -I$(INCLDIR)

################# DO NOT EDIT BELOW THIS LINE ###################

#### Variable Definitions ####
PKGNAME = register
BASIC_FILES = Makefile Changes TODO
DOCS = doc.txt
VERSION = 0
RELEASE = $(VERSION).1.2

#### Object Definitions ####
OBJ_EASYREG = easyreg.o
OBJ_COMBMOSAIC = combmosaic.o
OBJECTS = $(sort $(OBJ_EASYREG) $(OBJ_COMBMOSAIC))
SRC = $(OBJECTS:.o=.c)
BINS = easyreg combmosaic
BINS_INSTALL = $(addprefix install_,$(BINS))
HEADERS = $(PKGNAME).h

LIBS_COMMON = -L$(LIBDIR) -lKLcfitsio -lcfitsio -lKLutil -lm
LIBS_EASYREG = -lKLstats -lKLimgmath
LIBS_COMBMOSAIC = -lKLfile
LIBS = $(sort $(LIBS_COMMON) $(LIBS_EASYREG) $(LIBS_COMBMOSAIC))

#### Rule Definitions ####
%.o : %.c
	$(CC) -c $(CFLAGS) $<

.PHONY: $(BINS)
main: configure $(OBJECTS) $(BINS)

.PHONY: configure
configure:
# configure register.h
	@echo -n "Configuring register.h ... "
	@\rm -f tmp
	@sed "s!BINDIR \".*\"!BINDIR \"$(PREFIX)/\"!" $(PKGNAME).h  > tmp
	@if [ -s tmp ]; then \
          mv -f tmp $(PKGNAME).h; \
        else \
          echo "Error configuring $(PKGNAME).h"; \
        fi
	@echo "done"

easyreg: $(OBJ_EASYREG)
	$(CC) $(CFLAGS) -o easyreg $(OBJ_EASYREG) $(LIBS_EASYREG) \
		$(LIBS_COMMON)
combmosaic: $(OBJ_COMBMOSAIC)
	$(CC) $(CFLAGS) -o combmosaic $(OBJ_COMBMOSAIC) $(LIBS_COMBMOSAIC) \
		$(LIBS_COMMON)

.PHONY: all
all : main

## Installing ##
.PHONY: $(BINS_INSTALL) install install-src install-all
install : $(BINS_INSTALL)
$(BINS_INSTALL):
	install -d -m 0755 $(EPREFIX)
	install -m 0755 $(subst install_,,$@) $(EPREFIX)/
install-src:
	@install -d -m 0755 $(SPREFIX)/src/$(PKGNAME)
	@install -m 0644 $(SRC) $(HEADERS) $(BASIC_FILES) $(DOCS) \
		$(SPREFIX)/src/$(PKGNAME)/
install-all: install install-src
	

## Packaging ##
package-src:
	tar cvf $(PKGNAME)-$(RELEASE).tar $(BASIC_FILES) $(DOCS) $(SRC) \
        $(HEADERS)
	$(GZIP) $(PKGNAME)-$(RELEASE).tar

## Cleaning ##
.PHONY: clean
clean :
	\rm -f $(BINS) $(OBJECTS)

### End of Makefile ###
