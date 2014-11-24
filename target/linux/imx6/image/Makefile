#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define sanitize_profile_name
$(shell echo $(PROFILE) | tr '[:upper:]' '[:lower:]' | sed 's/_/-/g')
endef

define Image/BuildKernel/Template

 ifneq ($(1),)
	$(CP) $(DTS_DIR)/$(1).dtb $(BIN_DIR)/$(IMG_PREFIX)-$(1).dtb

	$(call Image/BuildKernel/MkFIT,$(1),$(KDIR)/zImage,$(BIN_DIR)/$(IMG_PREFIX)-$(1).dtb,none,0x10008000,0x10008000)
	$(CP) $(KDIR)/fit-$(1).itb $(BIN_DIR)/$(IMG_PREFIX)-$(1)-fit-uImage.itb

  ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(call Image/BuildKernel/MkFIT,$(1),$(KDIR)/zImage-initramfs,$(BIN_DIR)/$(IMG_PREFIX)-$(1).dtb,none,0x10008000,0x10008000,-initramfs)
	$(CP) $(KDIR)/fit-$(1)-initramfs.itb $(BIN_DIR)/$(IMG_PREFIX)-$(1)-fit-uImage-initramfs.itb
  endif
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

 ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_KERNEL)$(CONFIG_TARGET_imx6_VENTANA),)
	$(INSTALL_DIR) $(TARGET_DIR)/boot
   ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_UIMAGE)$(CONFIG_TARGET_imx6_VENTANA),)
	$(CP) $(BIN_DIR)/$(IMG_PREFIX)-uImage $(TARGET_DIR)/boot/
	ln -sf $(IMG_PREFIX)-uImage $(TARGET_DIR)/boot/uImage
   endif
   ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_ZIMAGE),)
	$(CP) $(BIN_DIR)/$(IMG_PREFIX)-zImage $(TARGET_DIR)/boot/
	ln -sf $(IMG_PREFIX)-zImage $(TARGET_DIR)/boot/zImage
   endif
   ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_FIT),)
	$(foreach dts,$(shell echo $(1)), \
		$(CP) $(BIN_DIR)/$(IMG_PREFIX)-$(dts)-fit-uImage.itb $(TARGET_DIR)/boot/
	)
   endif
 endif

 ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_DTB)$(CONFIG_TARGET_imx6_VENTANA),)
	$(INSTALL_DIR) $(TARGET_DIR)/boot
	$(foreach dts,$(shell echo $(1)), \
		$(CP) $(BIN_DIR)/$(IMG_PREFIX)-$(dts).dtb $(TARGET_DIR)/boot/ ; \
		ln -sf $(IMG_PREFIX)-$(dts).dtb $(TARGET_DIR)/boot/$(dts).dtb ; \
	)
 endif
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.$(1))
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-$(1).bin bs=128k conv=sync
	[ -f "$(BIN_DIR)/$(IMG_PREFIX)-$(call sanitize_profile_name)-fit-uImage.itb" ] && ( \
		dd if=$(BIN_DIR)/$(IMG_PREFIX)-$(call sanitize_profile_name)-fit-uImage.itb bs=2048k conv=sync; \
		dd if=$(KDIR)/root.$(1) bs=128k conv=sync; \
	) > $(BIN_DIR)/$(IMG_PREFIX)-$(call sanitize_profile_name)-fit-$(1).bin || true
endef

define Image/mkfs/targz

	$(TAR) -czpf $(BIN_DIR)/$(IMG_PREFIX)-$(call sanitize_profile_name)-rootfs.tar.gz --numeric-owner --owner=0 --group=0 -C $(TARGET_DIR)/ .
endef

define ubifs_imx_gateworks_ventana
	# Micron MT29F1G08ABAD/MT29F2G08ABAE/MT29F4G08ABAD/MT29F8G08ADAD NAND
	$(eval VENTANA_UBIFS_OPTS:="-m 2048 -e 124KiB -c 8124")
	$(eval VENTANA_UBI_OPTS:="-m 2048 -p 128KiB -s 2048")
	$(call Image/mkfs/ubifs)
	$(CP) $(KDIR)/root.ubifs $(BIN_DIR)/$(IMG_PREFIX)-$(call sanitize_profile_name)-rootfs_normal.ubifs
	$(CP) $(KDIR)/root.ubi $(BIN_DIR)/$(IMG_PREFIX)-$(call sanitize_profile_name)-rootfs_normal.ubi

	# Micron MT29F8G08ABAC/MT29F16G08ADAC 1GB/2GB NAND
	$(eval VENTANA_UBIFS_OPTS:="-m 4096 -e 248KiB -c 8124")
	$(eval VENTANA_UBI_OPTS:="-m 4096 -p 256KiB -s 4096")
	$(call Image/mkfs/ubifs)
	$(CP) $(KDIR)/root.ubifs $(BIN_DIR)/$(IMG_PREFIX)-$(call sanitize_profile_name)-rootfs_large.ubifs
	$(CP) $(KDIR)/root.ubi $(BIN_DIR)/$(IMG_PREFIX)-$(call sanitize_profile_name)-rootfs_large.ubi
endef

define Image/Build/ubifs
	true
endef

define Image/Build/ubi
	true
endef


Image/BuildKernel/Template/Generic=$(call Image/BuildKernel/Template)
Image/InstallKernel/Template/Generic=$(call Image/InstallKernel/Template)

Image/BuildKernel/Template/IMX6DL_WANDBOARD=$(call Image/BuildKernel/Template,imx6dl-wandboard)
Image/InstallKernel/Template/IMX6DL_WANDBOARD=$(call Image/InstallKernel/Template,imx6dl-wandboard)

Image/BuildKernel/Template/VENTANA=$(foreach dts,$(shell echo $(VENTANA_DTS)),$(call Image/BuildKernel/Template,$(dts)))
Image/InstallKernel/Template/VENTANA=$(call Image/InstallKernel/Template,$(VENTANA_DTS))
Image/ubifs/VENTANA=$(call ubifs_imx_gateworks_ventana)


define Image/BuildKernel
	$(call Image/BuildKernel/Template/$(PROFILE))
endef

define Image/InstallKernel
	$(call Image/InstallKernel/Template/$(PROFILE))
	$(if $(Image/ubifs/$(PROFILE)), \
		$(call Image/ubifs/$(PROFILE))
	)
endef

define Image/Build
	$(if $(Image/Build/$(1)), \
		$(call Image/Build/$(1),$(1)), \
		$(CP) $(KDIR)/root.$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(call sanitize_profile_name)-$(1).img \
	)
endef

$(eval $(call BuildImage))
