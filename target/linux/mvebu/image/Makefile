#
# Copyright (C) 2012-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

LOADADDR:=0x00008000

JFFS2_BLOCKSIZE = 128k

KDIR_TMP:=$(KDIR)/tmp


UIMAGE:=$(BIN_DIR)/$(IMG_PREFIX)-uImage

define Image/Build/DTB
	cp $(KDIR)/zImage$(2) $(KDIR)/zImage$(2)-$(1);
	cat $(DTS_DIR)/$(1).dtb >> $(KDIR)/zImage$(2)-$(1);
	$(call Image/BuildKernel/MkuImage, \
		none, $(LOADADDR), $(LOADADDR), \
		$(KDIR)/zImage$(2)-$(1), $(KDIR)/uImage$(2)-$(1))
	cp $(KDIR)/uImage$(2)-$(1) $(UIMAGE)$(2)-$(1);
endef

# $(1): Profile Name
# $(2): DTB Name
# $(3): Erase Block Size
# $(4): Page Size
# $(5): Sub-Page Size (optional)
define NANDProfile
  define Image/BuildKernel/Profile/$(1)
	$(call Image/Build/DTB,$(2))
    ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(call Image/Build/Profile,$(1)/Initramfs)
    endif
  endef

  define Image/Build/Profile/$(1)/BuildSysupgrade
	$(call Image/Build/SysupgradeNAND,$(2),$$(1),$(KDIR)/uImage-$(2))
  endef

  define Image/Build/Profile/$(1)/Initramfs
	$(call Image/Build/DTB,$(2),-initramfs)
  endef

  define Image/Build/Profile/$(1)/squashfs
	$(call Image/Build/UbinizeImage,$(2),,squashfs, -p $(3) -m $(4) $(if $(5),-s $(5)))
	cp $(KDIR)/$$(IMG_PREFIX)-$(2)-squashfs-ubinized.bin $(BIN_DIR)
  endef

  PROFILES_LIST += $(1)
endef

# $(1): Profile Name
# $(2): DTB Name
# $(3): Erase Block Size
define UBINORProfile
  define Image/BuildKernel/Profile/$(1)
	$(call Image/Build/DTB,$(2))
    ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(call Image/Build/Profile,$(1)/Initramfs)
    endif
  endef

  define Image/Build/Profile/$(1)/Initramfs
	$(call Image/Build/DTB,$(2),-initramfs)
  endef

  define Image/Build/Profile/$(1)/squashfs
	$(call Image/Build/UbinizeImage,$(2),,squashfs, -p $(3) -m 1)
  endef

  PROFILES_LIST += $(1)
endef

# $(1): Profile Name
# $(2): DTB Name
# $(3): Erase Block Size
define NORProfile
  define Image/BuildKernel/Profile/$(1)
	$(call Image/Build/DTB,$(2))
    ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(call Image/Build/Profile,$(1)/Initramfs)
    endif
  endef

  define Image/Build/Profile/$(1)/Initramfs
	$(call Image/Build/DTB,$(2),-initramfs)
  endef

  define Image/Build/Profile/$(1)/squashfs
	( \
		dd if=$(KDIR)/uImage-$(2) bs=$(3) conv=sync; \
		dd if=$(KDIR)/root.squashfs bs=$(3) conv=sync; \
	) > $$(BIN_DIR)/$$(IMG_PREFIX)-$(2)-squashfs-firmware.bin
  endef

  PROFILES_LIST += $(1)
endef

# $(1): Profile Name
# $(2): Sub Profiles list
define MultiProfile
  define Image/BuildKernel/Profile/$(1)
	$(foreach profile, $(2),
		$(call Image/BuildKernel/Profile/$(profile)))
  endef

  define Image/Build/Profile/$(1)/BuildSysupgrade
	$(foreach profile, $(2),
		$(call Image/Build/Profile/$(profile)/BuildSysupgrade,$$(1)))
  endef

  define Image/Build/Profile/$(1)/Initramfs
	$(foreach profile, $(2),
		$(call Image/Build/Profile/$(profile)/Initramfs))
  endef

  define Image/Build/Profile/$(1)/squashfs
	$(foreach profile, $(2),
		$(call Image/Build/Profile/$(profile)/squashfs))
  endef
endef

# Boards with NAND, without subpages
$(eval $(call NANDProfile,370-DB,armada-370-db,512KiB,4096))
$(eval $(call NANDProfile,370-RD,armada-370-rd,512KiB,4096))
$(eval $(call NANDProfile,385-DB-AP,armada-385-db-ap,256KiB,4096))
$(eval $(call NANDProfile,Mirabox,armada-370-mirabox,512KiB,4096))
$(eval $(call NANDProfile,XP-DB,armada-xp-db,512KiB,4096))
$(eval $(call NANDProfile,XP-GP,armada-xp-gp,512KiB,4096))

# Boards with NAND, with subpages
$(eval $(call NANDProfile,Mamba,armada-xp-mamba,128KiB,2048,512))

# Boards with large NOR, where we want to use UBI
$(eval $(call UBINORProfile,OpenBlocks-AX-3-4,armada-xp-openblocks-ax3-4,128KiB))

# Boards with small NOR, where UBI doesn't make sense
$(eval $(call NORProfile,385-RD,armada-385-rd,256KiB))

define Image/Build/Profile/Mamba/squashfs
	$(call Image/Build/UbinizeImage,armada-xp-mamba,,squashfs, -p 128KiB -m 2048 -s 512)
	( \
		dd if=$(KDIR)/uImage-armada-xp-mamba bs=3072k conv=sync; \
		dd if=$(KDIR)/$(IMG_PREFIX)-armada-xp-mamba-squashfs-ubinized.bin \
		bs=2048 conv=sync; \
	) > $(BIN_DIR)/$(IMG_PREFIX)-armada-xp-mamba-squashfs-factory.img
endef

$(eval $(call MultiProfile,Generic,$(PROFILES_LIST)))
$(eval $(call MultiProfile,Evalboards,$(PROFILES_LIST)))

define Image/BuildKernel
	$(call Image/BuildKernel/Profile/$(PROFILE))
endef

define Image/Build/squashfs
	# Align the squashfs image size before calling the profiles,
	# otherwise the size would keep growing
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
	$(call Image/Build/Profile/$(PROFILE)/squashfs)
endef

define Image/Build
	$(call Image/Build/$(1))
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-root.$(1) bs=128k conv=sync
	$(call Image/Build/Profile/$(PROFILE)/BuildSysupgrade,$(1))
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(call Image/Build/Profile/$(PROFILE)/Initramfs)
endif
endef

$(eval $(call BuildImage))
