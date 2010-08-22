#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

JFFS2OPTS_NAND += --little-endian --pagesize=0x800 --no-cleanmarkers --pad

define Image/mkfs/jffs2/sub/sub
	$(STAGING_DIR_HOST)/bin/mkfs.jffs2 $(3) \
		-e $(patsubst %k,%KiB,$(1)) -o $(KDIR)/root.jffs2-$(1)-$(2) -d $(TARGET_DIR) -v 2>&1 1>/dev/null | awk '/^.+$$$$/'
endef

define Image/mkfs/jffs2/sub
	# Do not remov this line
	$(call Image/mkfs/jffs2/sub/sub,$(1),nor,$(JFFS2OPTS))
	( \
		dd if=$(KDIR)/uImage bs=4096k conv=sync; \
		dd if=$(KDIR)/root.jffs2-$(1)-nor bs=128k conv=sync; \
	) > $(BIN_DIR)/$(IMG_PREFIX)-jffs2-$(1)-nor.img

	$(call Image/mkfs/jffs2/sub/sub,$(1),nand,$(JFFS2OPTS) $(JFFS2OPTS_NAND))
	$(call add_jffs2_mark,$(KDIR)/root.jffs2-$(1)-nand)
	cp $(KDIR)/root.jffs2-$(1)-nand $(BIN_DIR)/$(IMG_PREFIX)-jffs2-$(1)-nand.img
endef

define Image/Prepare
	cp $(LINUX_DIR)/arch/arm/boot/uImage $(KDIR)/uImage
endef

define Image/BuildKernel
# do mach-id fixup here, if needed
	cp $(KDIR)/uImage $(BIN_DIR)/$(IMG_PREFIX)-uImage
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
endef

define Image/Build/jffs2-128k
	( \
		dd if=$(KDIR)/uImage bs=4096k conv=sync; \
		dd if=$(KDIR)/root.$(1) bs=128k conv=sync; \
	) > $(BIN_DIR)/$(IMG_PREFIX)-$(1)-nor.img

	cp $(KDIR)/root.$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(1)-nand.img
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
	( \
		dd if=$(KDIR)/uImage bs=4096k conv=sync; \
		dd if=$(KDIR)/root.$(1) bs=128k conv=sync; \
	) > $(BIN_DIR)/$(IMG_PREFIX)-$(1).img
endef

$(eval $(call BuildImage))
