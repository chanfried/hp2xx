###########################################################################
#    Copyright (c) 1991 - 1994 Heinz W. Werntges.  All rights reserved.
#    Distributed by Free Software Foundation, Inc.
#
# This file is part of HP2xx.
#
# HP2xx is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY.  No author or distributor accepts responsibility
# to anyone for the consequences of using it or for whether it serves any
# particular purpose or works at all, unless he says so in writing.  Refer
# to the GNU General Public License, Version 2 or later, for full details
#
# Everyone is granted permission to copy, modify and redistribute
# HP2xx, but only under the conditions described in the GNU General Public
# License.  A copy of this license is supposed to have been
# given to you along with HP2xx so you can know your rights and
# responsibilities.  It should be in a file named COPYING.  Among other
# things, the copyright notice and this notice must be preserved on all
# copies.
###########################################################################
#
# Windows 32 Makefile for misc. versions of hp2xx; strong emphasis on UNIXes
#

# Utilities etc.:
#
SHELL    = /bin/sh

CHMOD    = chmod
CP       = cp
MKINFO   = makeinfo
RMCMD    = rm -f
STRIP    = strip

# Installation sites (GNU conventions):
#
prefix   = /usr/local
bindir   = $(prefix)/bin
includedir = $(prefix)/include
mandir   = $(prefix)/man
man1dir  = $(mandir)/man1
infodir  = $(prefix)/info

# CC and CFLAGS set for gcc, but any ANSI-C compiler should work.
# For non-gcc compilers, simply set CFLAGS = -O (to start with)
#
CC       = gcc -ggdb3 
CFLAGS   = -O3 -Wall -Wextra -Wno-unused-parameter 

#LFLAGS   =                # only sometimes needed; see below for examples
#CPPFLAGS = -I.            # only sometimes needed; see below for examples
#BINDCMD  =                # only needed for EMX and DJGPP DOS extenders
LFLAGS = 
CPPFLAGS = 
BINDCMD = 
0=o


# MS Windows, no previewer
#
#DEFINES   = -DUNIX -D_NO_VCL -DWIN32
DEFINES = 
PREVIEWER = no_prev
ALL_LIBS  = 
O = o
EXE = .exe
#
# EX_SRC	=
# EX_OBJ	=
# EX_DEFS	=
#
# PNG support (requires -lpng and -lz on the ALL_LIBS line)
EX_SRC	+= png.c to_png.c 
EX_OBJ	+= png.$O to_png.$O 
EX_DEFS	+= -DPNG 
PNGVER	= png14
PNGDIR	= ../$(PNGVER)
EX_INC	+= -I$(PNGDIR)/include 
LFLAGS	+= -L$(PNGDIR)/lib 
EX_LIBS	+= -l$(PNGVER).dll 
#
# TIFF support (requires -ltiff and possibly -lz on the ALL_LIBS line)
#EX_SRC	= to_tif.c
#EX_OBJ	= to_tif.$O
#EX_DEFS	= -DTIF
#
# Include extras:
# EX_SRC	= to_pic.c to_pac.c
# EX_OBJ	= to_pic.$O to_pac.$O
# EX_DEFS	= -DPIC_PAC
#
#############################################################################
# No user-serviceable parts below!
#############################################################################

COMMON_INCS	= hp2xx.h bresnham.h pendef.h lindef.h
ALL_INCS	= $(COMMON_INCS) chardraw.h charset0.h getopt.h x11.h ilbm.h iff.h
ALL_CFLAGS	= -c $(CFLAGS)


# $(SRCS) NOT USED YET:
SRCS	= hp2xx.c hpgl.c picbuf.c bresnham.c chardraw.c getopt.c getopt1.c \
	  $(ALL_INCS)\
	  to_vec.c to_pcx.c to_pcl.c to_eps.c to_img.c to_pbm.c to_rgip.c \
	  std_main.c to_fig.c clip.c fillpoly.c pendef.c lindef.c murphy.c \
	  $(PREVIEWER).c $(EX_SRC)

OBJS	= hp2xx.$O hpgl.$O picbuf.$O bresnham.$O chardraw.$O getopt.$O getopt1.$O \
	  to_vec.$O to_pcx.$O to_pcl.$O to_eps.$O to_img.$O to_pbm.$O to_rgip.$O \
	  std_main.$O to_fig.$O clip.$O fillpoly.$O pendef.$O lindef.$O murphy.$O \
	  $(PREVIEWER).$O $(EX_OBJ)

PROGRAM	= hp2xx$(EXE)

default:
	@echo "This makefile needs manual configuration! Edit it now!"
	@echo "After configuration, you may use the following commands:"
	@echo   ""
	@echo   "make all             Compile everything"
	@echo   "make check           Run a color application in preview mode"
	@echo   "make install-bin     Install the binary file"
	@echo   "make install-man     Install the manual page"
	@echo   "make install-info    Create and install the info file"
	@echo   "make install         Install everything"
	@echo   "make mostlyclean"
	@echo   "make clean"
	@echo   "make realclean"



#########################################################################
#									#
# 			Implicit Rules					#
#									#
#########################################################################

all: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o $(PROGRAM) $(EX_LIBS) 
	$(CP) $(PROGRAM) $(PNGDIR)
	$(BINDCMD)


.c.$O:	$< $(COMMON_INCS)
#	-$(RMCMD) $@
	$(CC) $(DEFINES) $(EX_DEFS) $(EX_INC) $(ALL_CFLAGS) $(CPPFLAGS) $<


#########################################################################
#									#
# 			Explicit Rules					#
#									#
#########################################################################

hp2xx.info:	../doc/hp2xxinf.tex
		$(CP) ../doc/hp2xxinf.tex hp2xx.texinfo
		$(MKINFO) hp2xx.texinfo

bresnham.$O:	bresnham.c bresnham.h

murphy.$O:	murphy.c murphy.h

chardraw.$O:	chardraw.c $(COMMON_INCS) chardraw.h charset0.h

getopt.$O:	getopt.c getopt.h

getopt1.$O:	getopt1.c getopt.h

hp2xx.$O:	hp2xx.c $(COMMON_INCS) getopt.h

hpgl.$O:		hpgl.c $(COMMON_INCS) chardraw.h

ilbm.$O:		ilbm.c $(COMMON_INCS) ilbm.h iff.h

to_x11.$O:	to_x11.c $(COMMON_INCS) x11.h

#########################################################################

check:
	./$(PROGRAM) -m pre -c1234567 -h150 -w150 ../hp-tests/colors.hp

clean:
	-make mostlyclean
	-$(RMCMD) core a.out ./hp2xx.texinfo ./hp2xx.info

distclean:
	@echo make distclean -- not supported yet.

dist:
	@echo make dist -- not supported yet.

mostlyclean:
	-$(RMCMD) *.$O

realclean:
	-make clean
	-$(RMCMD) $(PROGRAM)

TAGS:
	@echo make TAGS -- not supported nor needed.

install:	install-bin install-info install-man

install-bin:	$(PROGRAM)
		-$(STRIP) $(PROGRAM)
		$(CP) $(PROGRAM) $(bindir)
		$(CHMOD) 755 $(bindir)/$(PROGRAM)

install-info:	hp2xx.info
		$(CP) hp2xx.info $(infodir)
		$(CHMOD) 644 $(infodir)/hp2xx.info

install-man:	../doc/hp2xx.1
		$(CP) ../doc/hp2xx.1 $(man1dir)
		$(CHMOD) 644 $(man1dir)/hp2xx.1
