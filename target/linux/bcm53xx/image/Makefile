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

	$(CP) ./ubinize.cfg $(KDIR)
endef

define trx2ndpart/serial
-a 1024 -f $(KDIR)/root.squashfs -a 0x10000 -A $(KDIR)/fs_mark
endef

define trx2ndpart/nand-0x20000-0x800
-a 0x20000 -f $(KDIR)/root-block-0x20000-min-0x800.ubi -a 0x20000 -A $(KDIR)/fs_mark
endef

# $(1): dts filename (also used for the firmware file).
# $(2): flash type (not used).
define Image/Build/initramfs/DTB
	$(call Image/Build/DTB,zImage-initramfs,$(1))
	$(STAGING_DIR_HOST)/bin/trx -o $(KDIR)/$(IMG_PREFIX)-$(1)-initramfs.trx \
		-f $(KDIR)/zImage-initramfs-$(1).lzma
endef

# $(1): dts filename (also used for the firmware file).
# $(2): flash type.
define Image/Build/squashfs/DTB
	$(call Image/Build/DTB,zImage,$(1))
	$(STAGING_DIR_HOST)/bin/trx -o $(KDIR)/$(IMG_PREFIX)-$(1)-squashfs.trx \
		-f $(KDIR)/zImage-$(1).lzma \
		$(call trx2ndpart/$(2))
endef

# $(1): kernel (Linux) image file
# $(2): dts filename (also used for the firmware file).
define Image/Build/DTB
	rm -f $(KDIR)/$(1)-$(2).lzma
	rm -f $(KDIR)/$(1)-$(2).dts
	cat $(KDIR)/$(1) $(DTS_DIR)/$(2).dtb > $(KDIR)/$(1)-$(2).dts;
	$(STAGING_DIR_HOST)/bin/lzma e $(KDIR)/$(1)-$(2).dts $(KDIR)/$(1)-$(2).lzma -d16
endef

# $(1): filesystem type (e.g. squashfs, initramfs).
# $(2): dts filename (also used for the firmware file).
# $(3): flash type.
# $(4): device specific magic.
define Image/Build/AsusTrx
	$(call Image/Build/$(1)/DTB,$(2),$(3))
	# TODO: Put magic in TRX file
	cp $(KDIR)/$(IMG_PREFIX)-$(2)-$(1).trx $(BIN_DIR)/
endef

# $(1): filesystem type (e.g. squashfs, initramfs).
# $(2): dts filename (also used for the firmware file).
# $(3): flash type.
define Image/Build/Trx
	$(call Image/Build/$(1)/DTB,$(2),$(3))
	cp $(KDIR)/$(IMG_PREFIX)-$(2)-$(1).trx $(BIN_DIR)/
endef

# $(1): filesystem type (e.g. squashfs, initramfs).
# $(2): dts filename (also used for the firmware file).
# $(3): flash type.
# $(4): board_id (device specific magic).
# $(5): region.
define Image/Build/Chk
	$(call Image/Build/$(1)/DTB,$(2),$(3))
	$(STAGING_DIR_HOST)/bin/mkchkimg -o $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).chk -k $(KDIR)/$(IMG_PREFIX)-$(2)-$(1).trx -b $(4) -r $(5)
endef

# $(1): filesystem type (e.g. squashfs, initramfs).
define Image/Build
	( cd $(KDIR); $(STAGING_DIR_HOST)/bin/ubinize -p 128KiB -m 2048 -o $(KDIR)/root-block-0x20000-min-0x800.ubi ubinize.cfg )

	$(call Image/Build/AsusTrx,$(1),bcm47081-asus-rt-n18u,nand-0x20000-0x800,RT-N18U)

	$(call Image/Build/Chk,$(1),bcm4708-netgear-r6250,nand-0x20000-0x800,U12H245T00_NETGEAR,1)
	$(call Image/Build/Chk,$(1),bcm4708-netgear-r6300-v2,nand-0x20000-0x800,U12H240T00_NETGEAR,1)
 
	$(call Image/Build/Trx,$(1),bcm4708-buffalo-wzr-1750dhp,nand-0x20000-0x800)
	$(call Image/Build/Trx,$(1),bcm47081-buffalo-wzr-600dhp2,nand-0x20000-0x800)
endef

define Image/Build/Initramfs
	$(call Image/Build,initramfs)
endef

$(eval $(call BuildImage))
