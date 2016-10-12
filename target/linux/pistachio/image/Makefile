#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

DTS_DIR:=$(LINUX_DIR)

define CompressGzip
	gzip -9 -c $(1) > $(2)
endef

define MkDTB
	$(call Image/BuildDTB,../dts/$(1).dts,$(1).dtb)
	$(CP) $(1).dtb $(BIN_DIR)/$(1).dtb
	$(CP) $(1).dtb $(KDIR)/$(1).dtb
	$(RM) $(1).dtb
endef

define MkImageGzip
	$(CP) $(KDIR)/vmlinux$(2) $(KDIR)/vmlinux$(2)-$(1)
	$(call CompressGzip,$(KDIR)/vmlinux$(2)-$(1),$(KDIR)/vmlinux$(2)-$(1).gz)
	mkimage -A mips -O linux -T kernel -a 0x84000000 -C gzip \
			-e 0x84000000 -n 'MIPS OpenWrt Linux-$(LINUX_VERSION)' \
			-d $(KDIR)/vmlinux$(2)-$(1).gz $(KDIR)/uImage-$(1)$(2)
endef

define Image/Build/ubifs

ifneq ($($(PROFILE)_UBIFS_OPTS),)
	$(CP) $(KDIR)/root.ubifs $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE)-rootfs.ubifs
endif

endef

define Image/Build/ubi

ifneq ($($(PROFILE)_UBI_OPTS),)
	$(CP) $(KDIR)/root.ubi $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE)-rootfs.ubi
endif

endef

define Image/BuildNAND/ubifs

ifneq ($($(PROFILE)_UBIFS_OPTS),)
ifneq ($($(PROFILE)_UBI_OPTS),)
	$(call Image/Build/UbinizeImage,$(PROFILE),,ubifs,$($(PROFILE)_UBI_OPTS))
	$(CP) $(KDIR)/$(IMG_PREFIX)-$(PROFILE)-ubifs-ubinized.bin $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE)-ubifs.img
endif
endif

endef

define Image/Build
	$(call Image/BuildNAND/$(1),$(1),$(PROFILE))
endef

define Image/BuildKernel/uImage
	$(call MkImageGzip,$(1))
	$(CP) $(KDIR)/uImage-$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(1)-uImage
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(call MkImageGzip,$(1),-initramfs)
	$(CP) $(KDIR)/uImage-$(1)-initramfs $(BIN_DIR)/$(IMG_PREFIX)-$(1)-uImage-initramfs
endif
endef

define Image/BuildKernel
	$(if $(IB),,$(call MkDTB,$($(PROFILE)_DEVICE_DTS)))
	$(call Image/BuildKernel/uImage,$(PROFILE))
endef

define Image/InstallKernel/copy
#UImage and DTB must always be in FS
#ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_KERNEL),)
	$(INSTALL_DIR) $(TARGET_DIR)/boot

#ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_UIMAGE),)
	$(CP) $(BIN_DIR)/$(IMG_PREFIX)-$(1)-uImage $(TARGET_DIR)/boot/
	ln -sf boot/$(IMG_PREFIX)-$(1)-uImage $(TARGET_DIR)/uImage
#endif

#endif

#ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_DTB),)
	$(INSTALL_DIR) $(TARGET_DIR)/boot

ifneq ($(1),)
	$(CP) $(KDIR)/$($(PROFILE)_DEVICE_DTS).dtb $(TARGET_DIR)/boot/
	ln -sf boot/$($(PROFILE)_DEVICE_DTS).dtb $(TARGET_DIR)/pistachio_$(BOARDNAME).dtb
endif
#endif

endef

define Image/InstallKernel
        $(call Image/InstallKernel/copy,$(PROFILE))
endef

$(eval $(call BuildImage))
