#
# arch/ubicom32/Makefile
#	<TODO: Replace with short file description>
#
# (C) Copyright 2009, Ubicom, Inc.
#
# This file is part of the Ubicom32 Linux Kernel Port.
#
# The Ubicom32 Linux Kernel Port is free software: you can redistribute
# it and/or modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation, either version 2 of the
# License, or (at your option) any later version.
#
# The Ubicom32 Linux Kernel Port is distributed in the hope that it
# will be useful, but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
# the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with the Ubicom32 Linux Kernel Port.  If not,
# see <http://www.gnu.org/licenses/>.
#
# Ubicom32 implementation derived from (with many thanks):
#   arch/m68knommu
#   arch/blackfin
#   arch/parisc
#

KBUILD_DEFCONFIG :=

# setup the machine name and machine dependent settings
machine-$(CONFIG_UBICOM32_V3)	:= ip5k
machine-$(CONFIG_UBICOM32_V4)	:= ip7k
MACHINE := $(machine-y)
export MACHINE

model-$(CONFIG_RAMKERNEL)	:= ram
model-$(CONFIG_ROMKERNEL)	:= rom
MODEL := $(model-y)
export MODEL

CPUCLASS := $(cpuclass-y)

export CPUCLASS

#
# We want the core kernel built using the fastcall ABI but modules need
# to be built using the slower calling convention because they could be
# loaded out of range for fast calls.
#
CFLAGS_KERNEL    += -mfastcall
CFLAGS_MODULE    += -mno-fastcall

#
# Some CFLAG additions based on specific CPU type.
#
cflags-$(CONFIG_UBICOM32_V3)		:= -march=ubicom32v3 -DIP5000
cflags-$(CONFIG_UBICOM32_V4)		:= -march=ubicom32v4 -DIP7000

ldflags-$(CONFIG_LINKER_RELAXATION)	:= --relax
LDFLAGS_vmlinux := $(ldflags-y)

GCCLIBDIR := $(dir $(shell $(CC) $(cflags-y) -print-libgcc-file-name))
GCC_LIBS := $(GCCLIBDIR)/libgcc.a

KBUILD_CFLAGS += $(cflags-y) -ffunction-sections
KBUILD_AFLAGS += $(cflags-y)

KBUILD_CFLAGS += -D__linux__ -Dlinux
KBUILD_CFLAGS += -DUTS_SYSNAME=\"uClinux\"

# include any machine specific directory
ifneq ($(machine-y),)
core-y += arch/$(ARCH)/mach-$(MACHINE)/
endif

head-y := arch/$(ARCH)/kernel/head.o

core-y	+= arch/$(ARCH)/kernel/ \
	   arch/$(ARCH)/mm/ \
	   arch/$(ARCH)/crypto/ \
	   arch/$(ARCH)/mach-common/

drivers-$(CONFIG_OPROFILE)	+= arch/ubicom32/oprofile/

libs-y	+= arch/$(ARCH)/lib/
libs-y	+= $(GCC_LIBS)

archclean:

# make sure developer has selected a valid board
ifeq ($(CONFIG_NOBOARD),y)
# $(error have to select a valid board file $(CONFIG_NOBOARD), please run kernel config again)
_all: config_board_error
endif

config_board_error:
	@echo "*************************************************"
	@echo "You have not selected a proper board."
	@echo "Please run menuconfig (or config) against your"
	@echo "kernel and choose your board under Processor"
	@echo "options"
	@echo "*************************************************"
	@exit 1
