#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

JFFS2_BLOCKSIZE:=64k

include $(INCLUDE_DIR)/image.mk

INITRAMFS_SUFFIX:=

ifeq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
	INITRAMFS_SUFFIX:=-initramfs
endif

define zimage_name
$(IMG_PREFIX)-$(1)-zImage$(INITRAMFS_SUFFIX)
endef

define uimage_name
$(IMG_PREFIX)-$(1)-uImage$(INITRAMFS_SUFFIX)
endef

define imgname
$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(patsubst jffs2-%,jffs2,$(patsubst squashfs-%,squashfs,$(1)))
endef

define rootfsname
$(BIN_DIR)/$(IMG_PREFIX)-$(2)-root.$(patsubst jffs2-%,jffs2,$(patsubst squashfs-%,squashfs,$(1)))
endef

define build_squashfs
	dd if=$(KDIR)/root.squashfs of=$(KDIR)/root.squashfs-$(1) bs=$(1) conv=sync
	$(call add_jffs2_mark,$(KDIR)/root.squashfs-$(1))
	dd if=$(KDIR)/root.squashfs-$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-root.squashfs-$(1) bs=$(1) conv=sync
endef

define prepare_zimage
	echo -en "\x$(2)\x1c\xa0\xe3\x$(3)\x10\x81\xe3" > $(KDIR)/$(call zimage_name,$(1))
	cat $(LINUX_DIR)/arch/arm/boot/zImage >> $(KDIR)/$(call zimage_name,$(1))
endef

define prepare_uimage
	$(call prepare_zimage,$(1),$(2),$(3))
	mkimage -A arm -O linux -T kernel -a 0x100000 -e 0x100000 -C none \
		-n 'OpenWrt Linux-$(LINUX_VERSION)' \
		-d $(KDIR)/$(call zimage_name,$(1)) \
		$(KDIR)/$(call uimage_name,$(1))
endef

define Image/Prepare
	# NSB3AST : mach id 2917 (0xb65)
	$(call prepare_zimage,nsb3ast,0b,65)
	# NS-K330 : mach id 3108 (0xc24)
	$(call prepare_uimage,ns-k330,0c,24)
endef

define Image/BuildKernel
	cat $(KDIR)/$(call zimage_name,nsb3ast) > $(BIN_DIR)/$(call zimage_name,nsb3ast)
	cat $(KDIR)/$(call zimage_name,ns-k330) > $(BIN_DIR)/$(call zimage_name,ns-k330)
	cat $(KDIR)/$(call uimage_name,ns-k330) > $(BIN_DIR)/$(call uimage_name,ns-k330)
endef

define Image/Build/Generic
	if [ `stat -c%s "$(KDIR)/$(call zimage_name,$(2))"` -gt 1048576 ]; then \
		echo "Warning: $(KDIR)/$(call zimage_name,$(2)) is too big" >&2; \
	else if [ `stat -c%s $(KDIR)/root.$(1)` -gt $(3) ]; then \
		echo "Warning: of=$(BIN_DIR)/$(IMG_PREFIX)-root.$(1) is too big" >&2; \
	else \
		cat $(KDIR)/root.$(1) > $(call rootfsname,$(1),$(2)); \
		( \
			dd if=$(KDIR)/$(call zimage_name,$(2)) bs=1024k conv=sync; \
			dd if=$(BIN_DIR)/$(IMG_PREFIX)-root.$(1) \
		) > $(call imgname,$(1),$(2)).img; \
	fi; fi
endef

define Image/Build/uImage
	if [ `stat -c%s "$(KDIR)/$(call uimage_name,$(2))"` -gt 1048576 ]; then \
		echo "Warning: $(KDIR)/$(call uimage_name,$(2)) is too big" >&2; \
	else if [ `stat -c%s $(KDIR)/root.$(1)` -gt $(3) ]; then \
		echo "Warning: of=$(BIN_DIR)/$(IMG_PREFIX)-root.$(1) is too big" >&2; \
	else \
		cat $(KDIR)/root.$(1) > $(call rootfsname,$(1),$(2)); \
		( \
			dd if=$(KDIR)/$(call uimage_name,$(2)) bs=1024k conv=sync; \
			dd if=$(BIN_DIR)/$(IMG_PREFIX)-root.$(1) \
		) > $(call imgname,$(1),$(2))-uImage.img; \
	fi; fi
endef

define Image/Build/NSB3AST
	$(call Image/Build/Generic,$(1),nsb3ast,7077888)
endef

define Image/Build/NSK330
	$(call Image/Build/Generic,$(1),ns-k330,2686976)
	$(call Image/Build/uImage,$(1),ns-k330,2686976)
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)

	$(call build_squashfs,64k)
	$(call Image/Build/NSB3AST,squashfs-64k)
	$(call Image/Build/NSK330,squashfs-64k)
endef

define Image/Build/jffs2-64k
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-root.$(1) bs=64k conv=sync
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
endef

$(eval $(call BuildImage))
