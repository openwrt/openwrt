#
# arch/ubicom32/Makefile
#       Makefile for Oprofile support on Ubicom32
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

obj-$(CONFIG_OPROFILE) += oprofile.o

DRIVER_OBJS = $(addprefix ../../../drivers/oprofile/, \
		oprof.o cpu_buffer.o buffer_sync.o \
		event_buffer.o oprofile_files.o \
		oprofilefs.o oprofile_stats.o \
		timer_int.o )

oprofile-y	:= $(DRIVER_OBJS) profile.o
