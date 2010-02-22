#
# arch/ubicom32/mach-common/Makefile
#	Makefile for Ubicom32 generic drivers/code.
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

obj-y += cachectl.o common.o usb_tio.o usb.o ubi32-gpio.o board.o bootargs.o profile.o
obj-$(CONFIG_PCI) += pci.o io.o

obj-$(CONFIG_FB_UBICOM32) += vdc_tio.o
obj-$(CONFIG_UBICOM_HID) += ubicom32hid.o
obj-$(CONFIG_UBICOM_INPUT) += ubicom32input.o
obj-$(CONFIG_UBICOM_INPUT_I2C) += ubicom32input_i2c.o
obj-$(CONFIG_UBICOM_SWITCH) += switch-core.o
obj-$(CONFIG_UBICOM_SWITCH_BCM539X) += switch-bcm539x.o
obj-$(CONFIG_UIO_UBICOM32RING) += ring_tio.o
obj-$(CONFIG_SND_UBI32) += audio.o
obj-$(CONFIG_UBICOM32_PLIO) += plio.o

