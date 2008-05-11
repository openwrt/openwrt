#
# (C) Copyright 2003
# Wolfgang Denk, DENX Software Engineering, <wd@denx.de>
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
v=$(shell \
$(CROSS_COMPILE)as --version|grep "GNU assembler"|awk '{print $$3}'|awk -F . '{print $$2}')

ifndef PLATFORM_CPU
PLATFORM_CPU = mips32r2
endif

MIPSFLAGS=$(shell \
if [ "$v" -lt "14" ]; then \
	echo "-mcpu=$(PLATFORM_CPU)"; \
else \
	echo "-march=$(PLATFORM_CPU) -mtune=$(PLATFORM_CPU)"; \
fi)

ifeq ($(CROSS_COMPILE_UCLIBC),1)
ifneq (,$(findstring mipsel,$(CROSS_COMIPLE)))
ENDIANNESS = -el
else
ENDIANNESS = -eb
endif
else
ifneq (,$(findstring 4KCle,$(CROSS_COMPILE)))
ENDIANNESS = -EL
else
ENDIANNESS = -EB
endif
endif

MIPSFLAGS += $(ENDIANNESS) -mabicalls

PLATFORM_CPPFLAGS += $(MIPSFLAGS)
