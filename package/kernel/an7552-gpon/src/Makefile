# SPDX-License-Identifier: GPL-2.0
#
# Out-of-tree build of the AN7552/AN7563 xPON MAC and SerDes PHY, one module.
#
# OpenWrt builds it through package/kernel/an7552-gpon; kernel.mk supplies
# KBUILD_EXTRA_SYMBOLS.  Standalone:  make KDIR=/path/to/linux
#
obj-m += an7552-gpon.o

an7552-gpon-y := \
	an7552_mac_main.o \
	an7552_mac_gpon.o \
	an7552_mac_genl.o \
	an7552_led.o \
	an7552_gpon_debugfs.o \
	an7552_laser.o \
	an7552_phy_main.o \
	an7552_phy_ops.o \
	an7552_phy_serdes.o \
	an7552_phy_irq.o \
	an7552_phy_tx.o \
	an7552_phy_debugfs.o

# uapi/gpon_genl.h for the netlink ABI, uapi/linux/pon_laser.h for the
# optical front end interface, both reached as <linux/...> or bare name.
ccflags-y := -I$(src) -I$(src)/uapi -I$(src)/uapi/linux

KDIR ?= /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

install:
	$(MAKE) -C $(KDIR) M=$(PWD) modules_install
	depmod -a

.PHONY: all clean install
