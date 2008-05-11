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

LIB	= libbootstrap.a

#OBJS_PRINTF_ENABLED	= bootstrap_board.o time.o console.o LzmaWrapper.o LzmaDecode.o crc32.o ctype.o display_options.o string.o vsprintf.o lists.o devices.o
#OBJS_PRINTF_DISBALED	= bootstrap_board.o LzmaDecode.o string.o crc32.o LzmaWrapper.o 

OBJS	= bootstrap_board_$(BOARDDIR).o LzmaDecode.o string.o crc32.o LzmaWrapper.o 

ifeq ($(BOOTSTRAP_PRINTF_STATUS), BOOTSTRAP_PRINTF_ENABLED)
#overwrite objs
OBJS	= bootstrap_board_$(BOARDDIR).o time.o console.o LzmaWrapper.o LzmaDecode.o crc32.o ctype.o display_options.o string.o vsprintf.o lists.o devices.o
CFLAGS += -DDEBUG_ENABLE_BOOTSTRAP_PRINTF
endif

all:	.depend $(LIB)

$(LIB):	$(OBJS)
	$(AR) crv $@ $(OBJS)

#########################################################################

.depend:	Makefile $(OBJS:.o=.c)
		echo "make libbootstrap.a with HEAD_SIZE $(HEAD_SIZE)"
		$(CC) -M $(CFLAGS) $(OBJS:.o=.c) > $@

sinclude .depend

#########################################################################
