# SPDX-License-Identifier: GPL-2.0-only
EXTRA_CFLAGS += -Werror -DCONFIG_MT76_LEDS
obj-m += mt7603e.o

mt7603e-y := \
	pci.o soc.o main.o init.o mcu.o \
	core.o dma.o mac.o eeprom.o \
	beacon.o debugfs.o
