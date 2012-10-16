#
# (C) Copyright 2006
# Ingenic Semiconductor, <jlwei@ingenic.cn>
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

COBJS	= $(BOARD).o flash.o

OBJS	= $(addprefix $(obj),$(COBJS))
SOBJS	= 

$(LIB):	$(obj).depend $(OBJS) $(SOBJS)
	$(AR) crv $@ $(OBJS) $(SOBJS)

#########################################################################

$(obj).depend:	Makefile $(SOBJS:.o=.S) $(COBJS:.o=.c)
		$(CC) -M $(CFLAGS) $(SOBJS:.o=.S) $(COBJS:.o=.c) > $@

sinclude $(obj).depend

#########################################################################
