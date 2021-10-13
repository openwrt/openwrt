# @(#) $Revision: 4.11 $ $Source: /judy/src/apps/demo/Makefile_deliver $

# Makefile for Judy demo programs.

# Locations of Judy header file and library; correct if necessary:
#
# Note the use of the archive version (libJudy.a) for speed, although the
# difference from shared libs is not great on Linux.

JUDY =		/usr

JUDY_INCDIR =	$(JUDY)/include
JUDY_LIBDIR =	$(JUDY)/lib
JUDY_HEADER =	$(JUDY_INCDIR)/Judy.h
JUDY_LIBBASE =	Judy
JUDY_LIB =	$(JUDY_LIBDIR)/lib$(JUDY_LIBBASE).a

DEBUG =		-O
CFLAGS =	$(DEBUG)
CC =		cc

# Place files locally by default:

OBJS =		interL.o interSL.o funhist.o JudySort.o
EXECS =		interL   interSL   funhist   JudySort

# === RULES ===

all:	$(EXECS)
debug:;	make -f Makefile DEBUG='-g'

# Unfortunately not all make programs understand filename generation via $(@F),
# or at least $$(@F) on a dependencies line, so spell out each $EXECS target
# separately:

interL: interL.c $(JUDY_HEADER) $(JUDY_LIB)
	$(CC) $(CFLAGS) -I $(JUDY_INCDIR) $(@F).c \
	    -L$(JUDY_LIBDIR) -l$(JUDY_LIBBASE) -o $@

interSL: interSL.c $(JUDY_HEADER) $(JUDY_LIB)
	$(CC) $(CFLAGS) -I $(JUDY_INCDIR) $(@F).c \
	    -L$(JUDY_LIBDIR) -l$(JUDY_LIBBASE) -o $@

funhist: funhist.c $(JUDY_HEADER) $(JUDY_LIB)
	$(CC) $(CFLAGS) -I $(JUDY_INCDIR) $(@F).c \
	    -L$(JUDY_LIBDIR) -l$(JUDY_LIBBASE) -o $@

JudySort: JudySort.c $(JUDY_HEADER) $(JUDY_LIB)
	$(CC) $(CFLAGS) -I $(JUDY_INCDIR) $(@F).c \
	    -L$(JUDY_LIBDIR) -l$(JUDY_LIBBASE) -o $@

clean:; rm -rf core $(OBJS) $(EXECS)
