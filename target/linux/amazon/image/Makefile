# 
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Image/BuildKernel
	$(STAGING_DIR_HOST)/bin/lzma e $(KDIR)/vmlinux $(KDIR)/vmlinux.lzma
	mkimage -A mips -O linux -T kernel -C lzma -a 0x80002000 -e \
		0x80002000 \
		-n 'MIPS OpenWrt Linux-$(LINUX_VERSION)' \
		-d $(KDIR)/vmlinux.lzma $(KDIR)/uImage

	cp $(KDIR)/uImage $(BIN_DIR)/$(IMG_PREFIX)-uImage
endef

define Image/Build/squashfs
    $(call prepare_generic_squashfs,$(BIN_DIR)/$(IMG_PREFIX)-$(1).image)
endef

define Image/Build
	cat $(KDIR)/uImage $(KDIR)/root.$(1) > $(BIN_DIR)/$(IMG_PREFIX)-$(1).image
	$(call Image/Build/$(1),$(1))
endef


$(eval $(call BuildImage))
