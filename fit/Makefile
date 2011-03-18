#################################################################
#
# Makefile for the Fit package.
#
# Routines to fit functions to data, images excluded.  To fit
# functions to images see the 'imfit' package.
#
# Packages required:
# Libraries required: KLutil, KLfit, KLfunc
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

EPREFIX = $(PREFIX)/fit
LPREFIX = $(EPREFIX)
IPREFIX = $(EPREFIX)

#### Compiler Variable Definitions ####
CC = gcc
CFLAGS = -g -O2 -I./ -I$(INCLDIR)

################# DO NOT EDIT BELOW THIS LINE ###################

#### Variable Definitions ####
PKGNAME = fit
BASIC_FILES = Makefile Changes TODO
DOCS = doc.txt
VERSION = 0
RELEASE = $(VERSION).1.1

#### Object Definitions ####
OBJ_FITGSURF = fitgsurf.o
OBJECTS = $(sort $(OBJ_FITGSURF))
SRC = $(OBJECTS:.o=.c)
BINS = fitgsurf
BINS_INSTALL = $(addprefix install_,$(BINS))
HEADERS = $(PKGNAME).h

LIBS_COMMON = -L$(LIBDIR) -lKLfit -lKLlinear -lKLfunc -lKLutil -lm
LIBS_FITGSURF = 
LIBS = $(sort $(LIBS_COMMON) $(LIBS_FITGSURF))

#### Rule Definitions ####
%.o : %.c
	$(CC) -c $(CFLAGS) $<

.PHONY: $(BINS)
main: $(OBJECTS) $(BINS)

fitgsurf: $(OBJ_FITGSURF)
	$(CC) $(CFLAGS) -o fitgsurf $(OBJ_FITGSURF) $(LIBS_COMMON) \
	$(LIBS_FITGSURF)

.PHONY: all
all : main

## Installing ##
.PHONY: $(BINS_INSTALL)
install : $(BINS_INSTALL)
$(BINS_INSTALL):
	install -d -m 0755 $(EPREFIX)
	install -m 0755 $(subst install_,,$@) $(EPREFIX)/

## Packaging ##
package-src:
	tar cvf $(PKGNAME)-$(RELEASE).tar $(BASIC_FILES) $(DOCS) $(SRC) \
        $(HEADERS)
	$(GZIP) $(PKGNAME)-$(RELEASE).tar

#### Dependencies ####
$(OBJECTS) : $(HEADERS)

## Cleaning ##
.PHONY: clean
clean :
	\rm -f $(BINS) $(OBJECTS)

### End of Makefile ###
