# SPDX-License-Identifier: GPL-2.0
#
# Out-of-tree build of the EN7571 GPON laser against the OpenWrt kernel tree.
#
#   make KDIR=/path/to/linux-6.18.52
#
# KDIR must point at a configured and prepared kernel build directory (one that
# has .config and autoconf.h); OpenWrt keeps it under
# build_dir/target-arm_cortex-a7_musl_eabi/linux-airoha_an7563/linux-6.18.52.

obj-m += en7571.o

# <linux/pon_laser.h> comes from kmod-an7552-gpon (Build/InstallDev there).
ccflags-y += -I$(STAGING_INC)

# Four translation units, by what the code is about:
#   en7571-regs.o        I2C register access, the DDMI tick, the ADC readers
#   en7571-calibration.o the flash calibration and the runtime bring-up
#   en7571-ctrl.o        the control loops: tx, power, APD, RSSI
#   en7571-main.o        probe and the outside: hwmon, debugfs, regulator
en7571-y := en7571-regs.o \
	    en7571-calibration.o \
	    en7571-ctrl.o \
	    en7571-main.o

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
