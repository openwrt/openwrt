#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Image/Prepare
	rm -f $(KDIR)/fs_mark
	echo -ne '\xde\xad\xc0\xde' > $(KDIR)/fs_mark
	$(call prepare_generic_squashfs,$(KDIR)/fs_mark)
endef

define Image/Build/initramfs/DTB
	$(call Image/Build/DTB,zImage-initramfs,$(1))
	$(STAGING_DIR_HOST)/bin/trx -o $(KDIR)/$(IMG_PREFIX)-initramfs-$(1).trx \
		-f $(KDIR)/zImage-initramfs-$(1).lzma
endef

define Image/Build/squashfs/DTB
	$(call Image/Build/DTB,zImage,$(1))
	$(STAGING_DIR_HOST)/bin/trx -o $(KDIR)/$(IMG_PREFIX)-squashfs-$(1).trx \
		-f $(KDIR)/zImage-$(1).lzma \
		-a 1024 -f $(KDIR)/root.squashfs -a 0x10000 -A $(KDIR)/fs_mark
endef

define Image/Build/DTB
	rm -f $(KDIR)/$(1)-$(2).lzma
	rm -f $(KDIR)/$(1)-$(2).dts
	cat $(KDIR)/$(1) $(DTS_DIR)/$(2).dtb > $(KDIR)/$(1)-$(2).dts;
	$(STAGING_DIR_HOST)/bin/lzma e $(KDIR)/$(1)-$(2).dts $(KDIR)/$(1)-$(2).lzma -d16
endef

define Image/Build/Chk
	$(call Image/Build/$(1)/DTB,$(2))
	$(STAGING_DIR_HOST)/bin/mkchkimg -o $(BIN_DIR)/openwrt-$(2)-$(1).chk -k $(KDIR)/$(IMG_PREFIX)-$(1)-$(2).trx -b $(3) -r $(4)
endef

# $(1): filesystem type (e.g. squashfs, initramfs).
define Image/Build
	$(call Image/Build/Chk,$(1),bcm4708-netgear-r6250,U12H245T00_NETGEAR,2)
endef

define Image/Build/Initramfs
	$(call Image/Build,initramfs)
endef

$(eval $(call BuildImage))
