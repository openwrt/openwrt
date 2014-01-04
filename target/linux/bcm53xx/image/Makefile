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

define Image/Build/Initramfs
	$(call Image/Build/Initramfs/Chk,bcm4708-netgear-r6250,U12H245T00_NETGEAR,2,initramfs)
endef

define Image/Build/Initramfs/Chk
	$(call Image/Build/Initramfs/DTB,$(1))
	$(STAGING_DIR_HOST)/bin/mkchkimg -o $(BIN_DIR)/openwrt-$(1)-$(4).chk -k $(KDIR)/$(IMG_PREFIX)-$(4)-$(1).trx -b $(2) -r $(3)
endef

define Image/Build/Initramfs/DTB
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

define Image/Build/squashfs/Chk
	$(call Image/Build/squashfs/DTB,$(1))
	$(STAGING_DIR_HOST)/bin/mkchkimg -o $(BIN_DIR)/openwrt-$(1)-$(4).chk -k $(KDIR)/$(IMG_PREFIX)-$(4)-$(1).trx -b $(2) -r $(3)
endef

define Image/Build/DTB
	rm -f $(KDIR)/$(1)-$(2).lzma
	rm -f $(KDIR)/$(1)-$(2).dts
	cat $(KDIR)/$(1) $(LINUX_DIR)/arch/$(ARCH)/boot/dts/$(2).dtb > $(KDIR)/$(1)-$(2).dts;
	$(STAGING_DIR_HOST)/bin/lzma e $(KDIR)/$(1)-$(2).dts $(KDIR)/$(1)-$(2).lzma -d16
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
	$(call Image/Build/squashfs/Chk,bcm4708-netgear-r6250,U12H245T00_NETGEAR,2,squashfs)
endef


$(eval $(call BuildImage))
