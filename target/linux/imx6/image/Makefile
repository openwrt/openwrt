#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

BOARDS := \
	imx6dl-wandboard \
	imx6q-gw5400-a

# board-specific Flattened Image Tree
define mkfit
	/bin/sh $(LINUX_DIR)/scripts/mkits.sh -A arm -C none -a $(2) -e $(2) \
		-v 'OpenWrt Linux-$(LINUX_VERSION)' \
		-d $(LINUX_DIR)/arch/arm/boot/dts/$(1).dtb -k $(LINUX_DIR)/arch/arm/boot/zImage \
		-o $(KDIR)/$(1)-uImage.its
	PATH=$(LINUX_DIR)/scripts/dtc:$(PATH) mkimage -f $(KDIR)/$(1)-uImage.its $(BIN_DIR)/openwrt-$(1)-uImage.itb
endef

# emit FIT images, DTB's and generic zImage
define Image/BuildKernel
	$(foreach board,$(BOARDS),
		$(call mkfit,$(board),0x10008000)
		cp $(LINUX_DIR)/arch/arm/boot/dts/$(board).dtb $(BIN_DIR)
	)
	cp $(KDIR)/zImage $(BIN_DIR)/openwrt-$(BOARD)-zImage
endef

# board-specific sysupgrade image
define BuildFirmware/Generic
	dd if=$(BIN_DIR)/openwrt-$(1)-uImage.itb of=$(KDIR)/uImage.pad bs=64k conv=sync; \
	dd if=$(KDIR)/root.$(2) of=$(KDIR)/root.$(2).pad bs=128k conv=sync; \
	sh $(TOPDIR)/scripts/combined-image.sh \
		$(KDIR)/uImage.pad \
		$(KDIR)/root.$(2).pad \
		$(BIN_DIR)/$(IMG_PREFIX)-$(patsubst jffs2-%,jffs2,$(patsubst squashfs-%,squashfs,$(1)))-sysupgrade.bin
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
	$(foreach board,$(BOARDS),
		$(call BuildFirmware/Generic,$(board),$(1))
	)
endef

# rootfs and board-specific combined kernel+rootfs for convenience
define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.$(1))
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/openwrt-$(BOARD)-$(1).img bs=128k conv=sync
	$(foreach board,$(BOARDS),
		( \
		dd if=$(BIN_DIR)/openwrt-$(board)-uImage.itb bs=2048k conv=sync; \
		dd if=$(KDIR)/root.$(1) bs=64k conv=sync; \
		) > $(BIN_DIR)/$(IMG_PREFIX)-$(board)-$(1).bin
	)
endef

$(eval $(call BuildImage))
