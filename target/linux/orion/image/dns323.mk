#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Image/Prepare
	cp $(LINUX_DIR)/arch/arm/boot/uImage $(KDIR)/uImage
endef

define Image/BuildKernel
	# Orion Kernel uImages
 # DNS-323: mach id 1542 (0x606)
	echo -en "\x06\x1c\xa0\xe3\x06\x10\x81\xe3" > $(KDIR)/dns323-zImage
	cat $(LINUX_DIR)/arch/arm/boot/zImage >> $(KDIR)/dns323-zImage
	$(STAGING_DIR_HOST)/bin/mkimage -A arm -O linux -T kernel \
	-C none -a 0x00008000 -e 0x00008000 -n 'Linux-$(LINUX_VERSION)' \
	-d $(KDIR)/dns323-zImage $(KDIR)/dns323-uImage
	cp $(KDIR)/dns323-uImage $(BIN_DIR)/openwrt-dns323-uImage
endef

define Image/Build/D-Link
	# Orion DNS-323 Images
 # mtd image
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/openwrt-dns323-rootfs bs=128k
endef

define Image/Build
$(call Image/Build/$(1),$(1))
$(call Image/Build/D-Link,$(1),dns323,DNS-323,$(1))
endef

define Image/Build/squashfs
$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef
