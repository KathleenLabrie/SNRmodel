################################################################
#
# Makefile for imfilter
#
# Filter/Convolution Routines.
#
# Libraries needed: KLutil, KLimgutil, KLstats, KLcfitsio, CFITSIO,
#
# Coding language: C
# Compiler: gcc
# OS: Redhat Linux
#
################################################################

MAKE = make
GZIP = gzip -S .gz

#### Environment Variable Definitions ####
OS = redhat
PREFIX = /home/labrie/prgc
INCLDIR = $(PREFIX)/include
LIBDIR = $(PREFIX)/lib

EPREFIX = $(PREFIX)/img/imfilter
LPREFIX = $(EPREFIX)
IPREFIX = $(EPREFIX)

#### Compiler Variable Definitions ####
CC = gcc
CFLAGS = -g -O2 -I./ -I$(INCLDIR)

################## DO NOT EDIT BELOW THIS LINE #################

#### Variable Definitions ####
PKGNAME = imfilter
BASIC_FILES = Makefile Changes TODO
DOCS = doc.txt
VERSION = 0
RELEASE = $(VERSION).1.2

#### Object Definitions ####
OBJ_FILTER = boxcar.o dobx.o filter.o initbx.o
OBJECTS = $(sort $(OBJ_FILTER))
SRC = $(OBJECTS:.o=.c)
HEADERS = $(PKGNAME).h
BINS = filter
BINS_INSTALL = $(addprefix install_,$(BINS))

LIBS_COMMON = -L$(LIBDIR) -lKLcfitsio -lcfitsio -lKLimgutil -lKLstats \
        -lKLutil -lm
LIBS_FILTER =
LIBS = $(sort $(LIBS_COMMON) $(LIBS_FILTER))

#### Rule Definition ####
%.o : %.c
	$(CC) -c $(CFLAGS) $<

.PHONY: main $(BINS)
main : $(OBJECTS) $(BINS)

filter: $(OBJ_FILTER)
	$(CC) $(CFLAGS) -o filter $(OBJ_FILTER) $(LIBS_COMMON) $(LIBS_FILTER)

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
$(OBJECTS): $(HEADERS)

.PHONY: clean
clean :
	\rm -f $(BINS) $(OBJECTS)

### End of Makefile ###

