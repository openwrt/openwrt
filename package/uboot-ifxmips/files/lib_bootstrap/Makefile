#
# (C) Copyright 2003
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

include $(TOPDIR)/config.mk

LIB	:= $(obj)libbootstrap.a

OBJS	:= board.o LzmaDecode.o string.o crc32.o LzmaWrapper.o
CFLAGS	+= -DCFG_BOOTSTRAP_CODE

ifeq ($(BOOTSTRAP_PRINTF_STATUS), BOOTSTRAP_PRINTF_ENABLED)
OBJS	+= time.o console.o ctype.o display_options.o vsprintf.o lists.o devices.o
CFLAGS += -DDEBUG_ENABLE_BOOTSTRAP_PRINTF
endif

SRCS	:= $(OBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(OBJS))

all:	$(SRCS) $(obj).depend $(LIB)

$(LIB):	$(obj).depend $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

vpath %.c ../common ../lib_generic ../lib_$(CPU)

board_bootstrap.c:
	ln -s ../lib_$(CPU)/board.c $@

#LzmaDecode.c LzmaWrapper.c string.c crc32.c:
#	ln -s ../lib_generic/$@ $@

#########################################################################

#include $(SRCTREE)/rules.mk
$(obj).depend:	$(SRCS)
		$(CC) -M $(CFLAGS) $^ > $@

sinclude $(obj).depend

#########################################################################
