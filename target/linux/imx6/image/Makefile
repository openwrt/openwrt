#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define get_human_name
$(shell echo $(PROFILE) | tr '[:upper:]' '[:lower:]' | sed 's/_/-/g')
endef

define Image/BuildKernel/Template
	$(CP) $(LINUX_DIR)/arch/arm/boot/dts/$(1).dtb $(BIN_DIR)/$(IMG_PREFIX)-$(1).dtb

	$(call Image/BuildKernel/MkFIT,$(1),$(KDIR)/zImage,$(BIN_DIR)/$(IMG_PREFIX)-$(1).dtb,none,0x10008000,0x10008000)
	$(CP) $(KDIR)/fit-$(1).itb $(BIN_DIR)/$(IMG_PREFIX)-$(1)-fit-uImage.itb

 ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(call Image/BuildKernel/MkFIT,$(1),$(KDIR)/zImage-initramfs,$(BIN_DIR)/$(IMG_PREFIX)-$(1).dtb,none,0x10008000,0x10008000,-initramfs)
	$(CP) $(KDIR)/fit-$(1)-initramfs.itb $(BIN_DIR)/$(IMG_PREFIX)-$(1)-fit-uImage-initramfs.itb
 endif

	$(CP) $(KDIR)/zImage $(BIN_DIR)/$(IMG_PREFIX)-zImage
	$(call Image/BuildKernel/MkuImage, \
		none, 0x10008000, 0x10008000, \
		$(BIN_DIR)/$(IMG_PREFIX)-zImage, \
		$(BIN_DIR)/$(IMG_PREFIX)-uImage \
	)

 ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(CP) $(KDIR)/zImage-initramfs $(BIN_DIR)/$(IMG_PREFIX)-zImage-initramfs
	$(call Image/BuildKernel/MkuImage, \
		none, 0x10008000, 0x10008000, \
		$(BIN_DIR)/$(IMG_PREFIX)-zImage-initramfs, \
		$(BIN_DIR)/$(IMG_PREFIX)-uImage-initramfs \
	)
 endif
endef

define Image/InstallKernel/Template

 ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_KERNEL),)
	$(INSTALL_DIR) $(TARGET_DIR)/boot
   ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_UIMAGE),)
	$(CP) $(BIN_DIR)/$(IMG_PREFIX)-uImage $(TARGET_DIR)/boot/
	ln -sf $(IMG_PREFIX)-uImage $(TARGET_DIR)/boot/uImage
   endif
   ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_ZIMAGE),)
	$(CP) $(BIN_DIR)/$(IMG_PREFIX)-zImage $(TARGET_DIR)/boot/
	ln -sf $(IMG_PREFIX)-zImage $(TARGET_DIR)/boot/zImage
   endif
   ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_FIT),)
	$(CP) $(BIN_DIR)/$(IMG_PREFIX)-$(1)-fit-uImage.itb $(TARGET_DIR)/boot/
	ln -sf $(IMG_PREFIX)-$(1)-fit-uImage.itb $(TARGET_DIR)/boot/uImage.itb
   endif
 endif
 
 ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_DTB),)
	$(INSTALL_DIR) $(TARGET_DIR)/boot
  ifneq ($(1),)
	$(CP) $(BIN_DIR)/$(IMG_PREFIX)-$(1).dtb $(TARGET_DIR)/boot/
	ln -sf $(IMG_PREFIX)-$(1).dtb $(TARGET_DIR)/boot/$(1).dtb
  endif
 endif
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.$(1))
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-$(1).bin bs=128k conv=sync
	( \
		dd if=$(BIN_DIR)/$(IMG_PREFIX)-$(call get_human_name)-fit-uImage.itb bs=2048k conv=sync; \
		dd if=$(KDIR)/root.$(1) bs=128k conv=sync; \
	) > $(BIN_DIR)/$(IMG_PREFIX)-$(call get_human_name)-fit-$(1).bin
endef

define Image/mkfs/targz

	$(TAR) -czpf $(BIN_DIR)/$(IMG_PREFIX)-$(call get_human_name)-rootfs.tar.gz --numeric-owner --owner=0 --group=0 -C $(TARGET_DIR)/ .
endef

define Image/Build/ubifs

 ifneq ($($(PROFILE)_UBIFS_OPTS),)
	$(CP) $(KDIR)/root.ubifs $(BIN_DIR)/$(IMG_PREFIX)-$(call get_human_name)-ubifs.img
 endif
endef

define Image/Build/ubi

 ifneq ($($(PROFILE)_UBI_OPTS),)
	$(CP) $(KDIR)/root.ubi $(BIN_DIR)/$(IMG_PREFIX)-$(call get_human_name)-ubi.img
 endif
endef


Image/BuildKernel/Template/generic=$(call Image/BuildKernel/Template)
Image/InstallKernel/Template/generic=$(call Image/InstallKernel/Template)

Image/BuildKernel/Template/IMX6DL_WANDBOARD=$(call Image/BuildKernel/Template,imx6dl-wandboard)
Image/InstallKernel/Template/IMX6DL_WANDBOARD=$(call Image/InstallKernel/Template,imx6dl-wandboard)

Image/BuildKernel/Template/IMX6Q_GW5400_A=$(call Image/BuildKernel/Template,imx6q-gw5400-a)
Image/InstallKernel/Template/IMX6Q_GW5400_A=$(call Image/InstallKernel/Template,imx6q-gw5400-a)

Image/BuildKernel/Template/IMX6Q_GW54XX=$(call Image/BuildKernel/Template,imx6q-gw54xx)
Image/InstallKernel/Template/IMX6Q_GW54XX=$(call Image/InstallKernel/Template,imx6q-gw54xx)


define Image/BuildKernel
	$(call Image/BuildKernel/Template/$(PROFILE))
endef

define Image/InstallKernel
	$(call Image/InstallKernel/Template/$(PROFILE))
endef

define Image/Build
	$(if $(Image/Build/$(1)), \
		$(call Image/Build/$(1),$(1)), \
		$(CP) $(KDIR)/root.$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE)-$(1).img \
	)
endef

$(eval $(call BuildImage))
