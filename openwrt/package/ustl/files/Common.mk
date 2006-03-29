LIBNAME		= ustl
MAJOR		= 0
MINOR		= 8
BUILD		= 0

CC		= $(CROSS)gcc 
CXX		= $(CROSS)g++ 
LD		= $(CROSS)gcc 
AR		= $(CROSS)ar
RANLIB		= $(CROSS)ranlib
DOXYGEN		= echo
INSTALL		= install
RM		= rm -f
LN		= ln -sf

prefix		= /usr
exec_prefix	= /usr
BINDIR		= /usr/bin
INCDIR		= /usr/include
LIBDIR		= /usr/lib

DESTDIR		=

INSTALLDIR	= ${INSTALL} -d
INSTALLLIB	= ${INSTALL} -p -m 644
INSTALLEXE	= ${INSTALL} -p -m 755
INSTALLDATA	= ${INSTALL} -p -m 644

CWARNOPTS	= -Wall -W -Wpointer-arith -Wno-cast-align \
		-Wcast-qual -Wwrite-strings -Wredundant-decls 
CXXWARNOPTS	= ${CWARNOPTS} -Wconversion -Wsign-promo -Wsynth -Woverloaded-virtual
PROCESSOR_OPTS	=
INLINE_OPTS	= -finline-limit=65535
CUSTOMINCDIR	=
CUSTOMLIBDIR	=

BUILD_SHARED	= 1
BUILD_STATIC	= 1 
#DEBUG		= 1
#PROFILE	= 1
STANDALONE	= -nodefaultlibs 

CFLAGS		=  \
		${CUSTOMINCDIR} ${OPTFLAGS} ${CWARNOPTS} ${STANDALONE}
CXXFLAGS	=  \
		${CUSTOMINCDIR} ${OPTFLAGS} ${INLINE_OPTS} \
		${CXXWARNOPTS} ${STANDALONE} -fno-builtin
LDFLAGS		= ${CUSTOMLIBDIR} ${STANDALONE}
OPTFLAGS	= ${PROCESSOR_OPTS}
ifdef DEBUG
    OPTFLAGS	+= -O0 -g
else
    OPTFLAGS	+= -O3 -DNDEBUG=1
endif
ifdef PROFILE
    OPTFLAGS	+= -pg
endif
ifdef STANDALONE
    LIBS	+= -lsupc++ -lgcc_eh -lgcc -lc
endif
ifdef BUILD_SHARED
    CFLAGS	+= -fPIC
    CXXFLAGS	+= -fPIC
endif
SHBLDFL		= -shared

LIBA		= lib${LIBNAME}.a
LIBSO		= lib${LIBNAME}.so
ifdef MAJOR
LIBSOLNK	= ${LIBSO}.${MAJOR}.${MINOR}
LIBSOBLD	= ${LIBSO}.${MAJOR}.${MINOR}.${BUILD}
endif
TOCLEAN		+= ${LIBSO} ${LIBA} ${LIBSOBLD}

