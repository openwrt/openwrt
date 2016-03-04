#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

DTS_DIR:=$(LINUX_DIR)/arch/mips/boot/dts/pistachio

define CompressGzip
	gzip -9 -c $(1) > $(2)
endef

define MkDTB
	$(CPP) -E -Wp,-MD,$(DTS_DIR)/.$(1).pre.tmp -nostdinc -I$(LINUX_DIR)/arch/mips/boot/dts/include -undef -D__DTS__  -x assembler-with-cpp -o $(DTS_DIR)/.$(1).tmp $(DTS_DIR)/$(1).dts
	$(LINUX_DIR)/scripts/dtc/dtc -I dts -o $(DTS_DIR)/$(1).dtb -O dtb -b 0 -i $(LINUX_DIR)/arch/mips/boot/dts -d $(DTS_DIR)/.$(1).dtc.tmp $(DTS_DIR)/.$(1).tmp
	$(CP) $(DTS_DIR)/$(1).dtb $(BIN_DIR)/$(1).dtb
	$(CP) $(DTS_DIR)/$(1).dtb $(KDIR)/$(1).dtb
endef

define MkImageGzip
	$(CP) $(KDIR)/vmlinux$(2) $(KDIR)/vmlinux$(2)-$(1)
	$(call CompressGzip,$(KDIR)/vmlinux$(2)-$(1),$(KDIR)/vmlinux$(2)-$(1).gz)
	mkimage -A mips -O linux -T kernel -a 0x84000000 -C gzip \
			-e 0x84000000 -n 'MIPS OpenWrt Linux-$(LINUX_VERSION)' \
			-d $(KDIR)/vmlinux$(2)-$(1).gz $(KDIR)/uImage-$(1)$(2)
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
	$(call MkDTB,pistachio_$(BOARDNAME))
	$(call Image/BuildKernel/uImage,$(BOARDNAME))
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
	$(CP) $(KDIR)/pistachio_$(1).dtb $(TARGET_DIR)/boot/
	ln -sf boot/pistachio_$(1).dtb $(TARGET_DIR)/pistachio_$(1).dtb
endif
#endif

endef

define Image/InstallKernel
        $(call Image/InstallKernel/copy,$(BOARDNAME))
endef

$(eval $(call BuildImage))
