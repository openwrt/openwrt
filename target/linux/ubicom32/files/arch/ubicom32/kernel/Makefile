#
# arch/ubicom32/kernel/Makefile
#	Main Makefile for the Ubicom32 arch directory.
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

extra-y := head.o vmlinux.lds

obj-y += \
	devtree.o \
	dma.o \
	flat.o \
	init_task.o \
	irq.o \
	ldsr.o \
	os_node.o \
	process.o \
	processor.o \
	ptrace.o \
	setup.o \
	signal.o \
	stacktrace.o \
	sys_ubicom32.o \
	syscalltable.o \
	thread.o \
	time.o \
	traps.o \
	ubicom32_context_switch.o \
	ubicom32_ksyms.o \
	ubicom32_syscall.o \
	unaligned_trap.o

obj-$(CONFIG_MODULES)				+= module.o
obj-$(CONFIG_COMEMPCI)				+= comempci.o
obj-$(CONFIG_SMP)				+= smp.o topology.o
obj-$(CONFIG_ACCESS_OK_CHECKS_ENABLED)		+= uaccess.o
obj-$(CONFIG_GENERIC_CLOCKEVENTS)		+= timer_device.o
obj-$(CONFIG_GENERIC_CLOCKEVENTS_BROADCAST)	+= timer_broadcast.o

ifndef CONFIG_GENERIC_CLOCKEVENTS
obj-y			+= timer_tick.o
endif
