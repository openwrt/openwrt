#
# sound/ubicom32/Makefile
#	Makefile for ALSA
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

CFLAGS_ubi32.o += -O2
snd-ubi32-pcm-objs := ubi32-pcm.o
snd-ubi32-generic-objs := ubi32-generic.o
snd-ubi32-generic-capture-objs := ubi32-generic-capture.o
snd-ubi32-cs4350-objs := ubi32-cs4350.o
snd-ubi32-cs4384-objs := ubi32-cs4384.o

# Toplevel Module Dependency
obj-$(CONFIG_SND_UBI32) += snd-ubi32-pcm.o
obj-$(CONFIG_SND_UBI32_AUDIO_GENERIC) += snd-ubi32-generic.o
obj-$(CONFIG_SND_UBI32_AUDIO_GENERIC_CAPTURE) += snd-ubi32-generic-capture.o
obj-$(CONFIG_SND_UBI32_AUDIO_CS4350) += snd-ubi32-cs4350.o
obj-$(CONFIG_SND_UBI32_AUDIO_CS4384) += snd-ubi32-cs4384.o
