#
# (C) Copyright 2003-2006
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

LIB	= $(obj)lib$(BOARD).a
BOOTSTRAP_LIB = $(obj)lib$(BOARD)_bootstrap.a

BOOTSTRAP_LIB-$(CONFIG_BOOTSTRAP) = $(BOOTSTRAP_LIB)

COBJS-y	+= danube.o

SOBJS	= lowlevel_init.o pmuenable.o

BOOTSTRAP_COBJS-$(CONFIG_BOOTSTRAP) = $(BOARD)_bootstrap.o
BOOTSTRAP_SOBJS-$(CONFIG_BOOTSTRAP) = lowlevel_bootstrap_init.o

BOOTSTRAP_SRCS	:= $(BOOTSTRAP_SOBJS-y:.o=.S) $(BOOTSTRAP_COBJS-y:.o=.c)

SRCS	:= $(sort $(SOBJS:.o=.S) $(COBJS:.o=.c) $(BOOTSTRAP_SOBJS:.o=.S))
OBJS	:= $(addprefix $(obj),$(COBJS-y))
SOBJS	:= $(addprefix $(obj),$(SOBJS))
BOOTSTRAP_OBJS	:= $(addprefix $(obj),$(BOOTSTRAP_COBJS-y))
BOOTSTRAP_SOBJS	:= $(addprefix $(obj),$(BOOTSTRAP_SOBJS-y))


all: $(obj).depend $(LIB) $(BOOTSTRAP_LIB)

$(LIB):	$(obj).depend $(OBJS) $(SOBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS) $(SOBJS)

$(BOOTSTRAP_LIB):	 $(BOOTSTRAP_OBJS) $(BOOTSTRAP_SOBJS)
	$(AR) $(ARFLAGS) $@ $(BOOTSTRAP_OBJS) $(BOOTSTRAP_SOBJS)

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
