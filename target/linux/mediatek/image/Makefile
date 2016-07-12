# Copyright (c) 2014 The Linux Foundation. All rights reserved.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Image/BuilduImage
	$(CP) $(KDIR)/zImage$(2) $(KDIR)/zImage-$(1)$(2)
	cat $(LINUX_DIR)/arch/arm/boot/dts/mt7623-$1.dtb >> $(KDIR)/zImage-$(1)$(2)
	mkimage -A arm -O linux -T kernel -C none -a 0x80008000 -e 0x80008000 -n 'MIPS OpenWrt Linux-$(LINUX_VERSION)'  -d $(KDIR)/zImage-$(1)$(2) $(KDIR)/uImage-$(1)$(2)
endef

define Image/Build/SysupgradeCombined
	$(call Image/BuilduImage,$1)
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(call Image/BuilduImage,$1,-initramfs)
	$(CP) $(KDIR)/uImage-$(1)-initramfs $(BIN_DIR)/$(IMG_PREFIX)-uImage-$(1)-initramfs
endif
	mkdir -p "$(KDIR_TMP)/sysupgrade-$(1)/"
	echo "BOARD=$(1)" > "$(KDIR_TMP)/sysupgrade-$(1)/CONTROL"
	$(CP) "$(KDIR)/root.$(2)" "$(KDIR_TMP)/sysupgrade-$(1)/root"
	$(CP) "$(KDIR)/uImage-$(1)" "$(KDIR_TMP)/sysupgrade-$(1)/kernel"
	(cd "$(KDIR_TMP)"; $(TAR) cvf \
		"$(BIN_DIR)/$(IMG_PREFIX)-$(1)-sysupgrade.tar" sysupgrade-$(1) \
			$(if $(SOURCE_DATE_EPOCH),--mtime="@$(SOURCE_DATE_EPOCH)") \
	)
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
	$(CP) $(KDIR)/root.squashfs $(BIN_DIR)/$(IMG_PREFIX)-root.squashfs

	$(call Image/Build/SysupgradeCombined,eMMC,squashfs)

	$(call Image/BuilduImage,NAND)
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(call Image/BuilduImage,NAND,-initramfs)
	$(CP) $(KDIR)/uImage-NAND-initramfs $(BIN_DIR)/$(IMG_PREFIX)-uImage-NAND-initramfs
endif
	$(call Image/Build/SysupgradeNAND,NAND,$(1),$(KDIR)/uImage-NAND)
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
endef

$(eval $(call BuildImage))
