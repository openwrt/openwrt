# Makefile for to build the base openwrt
#
# Copyright (C) 2004 Manuel Novoa III <mjn3@uclibc.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#
######################################################################
#
# WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING!
#
# Currently the dependencies are not all handled.  But that's true of
# our buildroot in general, since it wasn't really set up for end users.

OPENWRT_TARGETS:= gcc3_3 openwrt-linux openwrt-kmodules.tar.bz2 \
	openwrt-shared openwrt-mtd openwrt-nvram openwrt-wlconf \
	bridge dnsmasq1 iptables wtools busybox \
	openwrt-rootprep

.PHONY: $(OPENWRT_TARGETS) openwrt-code.bin

openwrt-base: $(OPENWRT_TARGETS)

######################################################################

WRT54G_SOURCE=wrt54gs.2.07.1.tgz
WRT54G_SITE=http://www.linksys.com/support/opensourcecode/wrt54gs/2.07.1
WRT54G_DIR=$(BUILD_DIR)/WRT54GS

LINUX_DIR=$(WRT54G_DIR)/release/src/linux/linux
LINUX_FORMAT=zImage
LINUX_BINLOC=arch/mips/brcm-boards/bcm947xx/compressed/vmlinuz

TARGET_MODULES_DIR:=$(TARGET_DIR)/lib/modules/2.4.20

$(LINUX_DIR)/.unpacked: $(WRT54G_DIR)/.prepared
	-(cd $(BUILD_DIR); ln -sf $(LINUX_DIR) linux)
	touch $(LINUX_DIR)/.unpacked

$(LINUX_DIR)/.patched: $(WRT54G_DIR)/.prepared
	$(SOURCE_DIR)/patch-kernel.sh $(LINUX_DIR)/../.. $(SOURCE_DIR)/openwrt/kernel/patches
	# use replacement diag module code
	cp -f $(SOURCE_DIR)/openwrt/kernel/diag.c $(LINUX_DIR)/drivers/net/diag/diag_led.c
	cp -f $(SOURCE_DIR)/openwrt/kernel/linux.config $(LINUX_DIR)/.config
	-(cd $(BUILD_DIR); ln -sf $(LINUX_DIR) linux)
	-(cd $(LINUX_DIR)/arch/mips/brcm-boards/bcm947xx/; \
	rm -rf compressed; \
	tar jxvf $(SOURCE_DIR)/openwrt/kernel/compressed-20040531.tar.bz2; \
	)
	touch $(LINUX_DIR)/.patched

$(LINUX_DIR)/.configured: $(LINUX_DIR)/.patched $(LINUX_DIR)/.bbc-patched $(LINUX_DIR)/.nf-patched 
	$(SED) "s,^CROSS_COMPILE.*,CROSS_COMPILE=$(KERNEL_CROSS),g;" $(LINUX_DIR)/Makefile
	$(SED) "s,^CROSS_COMPILE.*,CROSS_COMPILE=$(KERNEL_CROSS),g;" $(LINUX_DIR)/arch/mips/Makefile
	$(SED) "s,\-mcpu=,\-mtune=,g;" $(LINUX_DIR)/arch/mips/Makefile
	make -C $(LINUX_DIR) oldconfig include/linux/version.h
	touch $(LINUX_DIR)/.configured

$(LINUX_DIR)/.depend_done:  $(LINUX_DIR)/.configured $(GCC_BUILD_DIR2)/.installed
	$(MAKE) -C $(LINUX_DIR) dep
	touch $(LINUX_DIR)/.depend_done

$(LINUX_DIR)/$(LINUX_BINLOC): $(LINUX_DIR)/.depend_done
	$(MAKE) -C $(LINUX_DIR) $(LINUX_FORMAT)

openwrt-kmodules.tar.bz2: $(LINUX_DIR)/$(LINUX_BINLOC)
	$(MAKE) -C $(LINUX_DIR) modules
	$(MAKE) -C $(LINUX_DIR) DEPMOD=/bin/true \
		INSTALL_MOD_PATH=$(LINUX_DIR)/modules modules_install
	tar -C $(LINUX_DIR)/modules/lib -cjf openwrt-kmodules.tar.bz2 modules

openwrt-linux: $(LINUX_DIR)/$(LINUX_BINLOC)

$(DL_DIR)/$(WRT54G_SOURCE):
	$(WGET) -P $(DL_DIR) $(WRT54G_SITE)/$(WRT54G_SOURCE)

$(WRT54G_DIR)/.source: $(DL_DIR)/$(WRT54G_SOURCE)
	zcat $(DL_DIR)/$(WRT54G_SOURCE) | tar -C $(BUILD_DIR) -xvf - WRT54GS/README.TXT WRT54GS/release
	touch $(WRT54G_DIR)/.source

$(WRT54G_DIR)/.prepared: $(WRT54G_DIR)/.source
	$(SOURCE_DIR)/patch-kernel.sh $(WRT54G_DIR) $(SOURCE_DIR)/openwrt/patches
	touch $(WRT54G_DIR)/.prepared

######################################################################

OPENWRT_ROOT_SKEL:=root.tar.gz
OPENWRT_SITE=http://openwrt.ksilebo.net/cgi-bin/viewcvs.cgi/root

$(DL_DIR)/$(OPENWRT_ROOT_SKEL):
	$(WGET) -P $(DL_DIR) $(OPENWRT_SITE)/$(OPENWRT_ROOT_SKEL)

######################################################################

OPENWRT_SRCBASE:=$(WRT54G_DIR)/release/src
OPENWRT_SHARED_BUILD_DIR:=$(OPENWRT_SRCBASE)/router/shared
OPENWRT_SHARED_TARGET_BINARY:=usr/lib/libshared.so

$(TARGET_DIR)/$(OPENWRT_SHARED_TARGET_BINARY): $(WRT54G_DIR)/.source
	$(MAKE) -C $(OPENWRT_SHARED_BUILD_DIR) -f Makefile-openwrt \
		SRCBASE=$(OPENWRT_SRCBASE) INSTALLDIR=$(TARGET_DIR) \
		CC=$(TARGET_CC) LD=$(TARGET_CROSS)ld STRIP="$(STRIP)" \
		CFLAGS="$(TARGET_CFLAGS) -I. -I$(OPENWRT_SRCBASE)/include -Wall -I$(OPENWRT_SRCBASE)/" \
		install

openwrt-shared: $(TARGET_DIR)/$(OPENWRT_SHARED_TARGET_BINARY)

openwrt-shared-clean:
	-$(MAKE) -C $(OPENWRT_SHARED_BUILD_DIR) clean

######################################################################

OPENWRT_NVRAM_BUILD_DIR:=$(OPENWRT_SRCBASE)/router/nvram
OPENWRT_NVRAM_TARGET_BINARY:=usr/sbin/nvram

$(TARGET_DIR)/$(OPENWRT_NVRAM_TARGET_BINARY): $(WRT54G_DIR)/.source
	$(MAKE) -C $(OPENWRT_NVRAM_BUILD_DIR) \
		SRCBASE=$(OPENWRT_SRCBASE) INSTALLDIR=$(TARGET_DIR) \
		CC=$(TARGET_CC) LD=$(TARGET_CROSS)ld STRIP="$(STRIP)" \
		CFLAGS="$(TARGET_CFLAGS) -I. -I$(OPENWRT_SRCBASE)/include -Wall -DOPENWRT_NVRAM" \
		install


openwrt-nvram: $(TARGET_DIR)/$(OPENWRT_NVRAM_TARGET_BINARY)

openwrt-nvram-clean:
	-$(MAKE) -C $(OPENWRT_NVRAM_BUILD_DIR) clean

######################################################################

OPENWRT_MTD_BUILD_DIR:=$(OPENWRT_SRCBASE)/router/rc
OPENWRT_MTD_TARGET_BINARY:=sbin/mtd

$(TARGET_DIR)/$(OPENWRT_MTD_TARGET_BINARY): $(WRT54G_DIR)/.source $(TARGET_DIR)/$(OPENWRT_NVRAM_TARGET_BINARY) # need libnvram
	$(MAKE) -C $(OPENWRT_MTD_BUILD_DIR) -f Makefile-openwrt \
		TOP=$(OPENWRT_SRCBASE)/router \
		SRCBASE=$(OPENWRT_SRCBASE) INSTALLDIR=$(TARGET_DIR) \
		CC=$(TARGET_CC) LD=$(TARGET_CROSS)ld STRIP="$(STRIP)" \
		CFLAGS="$(TARGET_CFLAGS) -I. -I$(OPENWRT_SRCBASE)/router/shared -I$(OPENWRT_SRCBASE)/include -Wall -I$(OPENWRT_SRCBASE)/" \
		install

openwrt-mtd: $(TARGET_DIR)/$(OPENWRT_MTD_TARGET_BINARY)

openwrt-mtd-clean:
	-$(MAKE) -C $(OPENWRT_MTD_BUILD_DIR) clean

######################################################################

OPENWRT_WLCONF_BUILD_DIR:=$(OPENWRT_SRCBASE)/router/wlconf
OPENWRT_WLCONF_TARGET_BINARY:=usr/sbin/wlconf

$(TARGET_DIR)/$(OPENWRT_WLCONF_TARGET_BINARY): $(WRT54G_DIR)/.source
	$(MAKE) -C $(OPENWRT_WLCONF_BUILD_DIR) \
		TOP=$(OPENWRT_SRCBASE)/router \
		SRCBASE=$(OPENWRT_SRCBASE) INSTALLDIR=$(TARGET_DIR) \
		CC=$(TARGET_CC) LD=$(TARGET_CROSS)ld STRIP="$(STRIP)" \
		CFLAGS="$(TARGET_CFLAGS) -I. -I$(OPENWRT_SRCBASE)/router/shared -I$(OPENWRT_SRCBASE)/include -Wall" \
		install


openwrt-wlconf: $(TARGET_DIR)/$(OPENWRT_WLCONF_TARGET_BINARY)

openwrt-wlconf-clean:
	-$(MAKE) -C $(OPENWRT_WLCONF_BUILD_DIR) clean

######################################################################

openwrt-rootprep:
	# tmp
	mkdir -p $(TARGET_DIR)/tmp
	chmod a+rwxt $(TARGET_DIR)/tmp
	ln -sf /tmp $(TARGET_DIR)/var
	rm -f $(TARGET_DIR)/usr/tmp
	ln -sf ../tmp $(TARGET_DIR)/usr/tmp
	# dev
	mkdir -p $(TARGET_DIR)/dev
	# etc
	mkdir -p $(TARGET_DIR)/etc
	ln -sf /tmp/resolv.conf $(TARGET_DIR)/etc/resolv.conf
	# miscellaneous
	mkdir -p $(TARGET_DIR)/mnt
	mkdir -p $(TARGET_DIR)/proc
	mkdir -p $(TARGET_DIR)/jffs
	mkdir -p $(TARGET_DIR)/rom
	# modules
	mkdir -p $(TARGET_MODULES_DIR)
	cp $(LINUX_DIR)/drivers/net/wl/wl.o $(TARGET_MODULES_DIR)
	#cp $(LINUX_DIR)/drivers/net/et.4702/et.4702.o $(TARGET_MODULES_DIR)
	cp $(LINUX_DIR)/drivers/net/et/et.o $(TARGET_MODULES_DIR)
	cp $(LINUX_DIR)/drivers/net/diag/diag.o $(TARGET_MODULES_DIR)

######################################################################

openwrt-prune: openwrt-base
	-@find $(TARGET_DIR) -type f -perm +111 | xargs $(STRIP) 2>/dev/null || true;
	# remove unneeded uClibc libs
	rm -rf $(TARGET_DIR)/lib/libthread_db*
	rm -rf $(TARGET_DIR)/lib/libpthread*
	# remove unneeded uClibc utils
	rm -f $(TARGET_DIR)/sbin/ldconfig
	rm -f $(TARGET_DIR)/usr/bin/ldd
	# remove other unneeded files
	rm -f $(TARGET_DIR)/usr/sbin/iptables-save
	rm -f $(TARGET_DIR)/usr/sbin/iptables-restore
	rm -f $(TARGET_DIR)/usr/sbin/ip6tables

######################################################################

wrt-tools:
	$(CC) -o $(WRT54G_DIR)/release/tools/trx $(SOURCE_DIR)/openwrt/tools/trx.c
	$(CC) -o $(WRT54G_DIR)/release/tools/addpattern $(SOURCE_DIR)/openwrt/tools/addpattern.c

openwrt-linux.trx:  openwrt-prune squashfsroot wrt-tools
	$(WRT54G_DIR)/release/tools/trx -o openwrt-linux.trx \
		$(LINUX_DIR)/$(LINUX_BINLOC) $(IMAGE)

openwrt-gs-code.bin: openwrt-linux.trx
	$(WRT54G_DIR)/release/tools/addpattern -i openwrt-linux.trx \
		-o openwrt-gs-code.bin -g

openwrt-g-code.bin: openwrt-gs-code.bin
	sed -e "1s,^W54S,W54G," < openwrt-gs-code.bin > openwrt-g-code.bin

openwrt-code.bin: openwrt-gs-code.bin openwrt-g-code.bin

######################################################################
