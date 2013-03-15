#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define imgname
$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(patsubst jffs2-%,jffs2,$(patsubst squashfs-%,squashfs,$(1)))
endef

define sysupname
$(call imgname,$(1),$(2))-sysupgrade.bin
endef

define factoryname
$(call imgname,$(1),$(2))-factory.bin
endef

ifeq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
  IMAGE_SUFFIX=-initramfs
endif

zImage:=$(BIN_DIR)/$(IMG_PREFIX)-zImage$(IMAGE_SUFFIX)

DTS_TARGETS = mpc8548cds_32b p1010rdb tl-wdr4900-v1
BOOT_IMAGES:=zImage cuImage.tl-wdr4900-v1

define Image/Prepare
	$(foreach image,$(BOOT_IMAGES),
		cp $(LINUX_DIR)/arch/powerpc/boot/$(image) $(KDIR)/$(image)
	)
endef

define Image/BuildKernel
	cp $(KDIR)/zImage $(zImage)
	$(foreach dts,$(DTS_TARGETS),
		$(LINUX_DIR)/scripts/dtc/dtc -I dts -O dtb $(LINUX_DIR)/arch/powerpc/boot/dts/$(dts).dts > $(BIN_DIR)/$(IMG_PREFIX)-$(dts).fdt
	)
endef

define Image/Build/TPLINK
	-$(STAGING_DIR_HOST)/bin/mktplinkfw \
		-H $(4) -W $(5) -F $(6) -N OpenWrt -V $(REVISION) $(7) \
		-k $(KDIR)/$(3) \
		-r $(KDIR)/root.$(1) \
		-o $(call factoryname,$(1),$(2))
	-$(STAGING_DIR_HOST)/bin/mktplinkfw \
		-H $(4) -W $(5) -F $(6) -N OpenWrt -V $(REVISION) $(7) -s \
		-k $(KDIR)/$(3) \
		-r $(KDIR)/root.$(1) \
		-o $(call sysupname,$(1),$(2))
endef

define Image/Build/Profile/TLWDR4900
	$(call Image/Build/TPLINK,$(1),tl-wdr4900-v1,cuImage.tl-wdr4900-v1,0x49000001,1,16Mppc)
endef

define Image/Build/Profile/Default
	$(call Image/Build/Profile/TLWDR4900,$(1))
endef

define Image/Build/ext2
	cp $(KDIR)/root.$(1) $(BIN_DIR)/$(IMG_PREFIX)-ext2.img
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
	cp $(KDIR)/root.squashfs $(BIN_DIR)/$(IMG_PREFIX)-root.squashfs
endef

PROFILE ?= Default

define Image/Build
	$(call Image/Build/$(1),$(1))
	$(call Image/Build/Profile/$(PROFILE),$(1))
endef

$(eval $(call BuildImage))
