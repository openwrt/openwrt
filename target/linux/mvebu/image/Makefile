#
# Copyright (C) 2012-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

LOADADDR:=0x00008000

JFFS2_BLOCKSIZE = 128k

KDIR_TMP:=$(KDIR)/tmp


define Image/Build/DTB
	cp $(KDIR)/zImage$(2) $(KDIR)/zImage$(2)-$(1);
	cat $(DTS_DIR)/$(1).dtb >> $(KDIR)/zImage$(2)-$(1);
	$(call Image/BuildKernel/MkuImage, \
		none, $(LOADADDR), $(LOADADDR), \
		$(KDIR)/zImage$(2)-$(1), $(KDIR)/uImage$(2)-$(1))
endef

# $(1): Profile Name
# $(2): DTB Name
# $(3): Erase Block Size
# $(4): Page Size
# $(5): Sub-Page Size (optional)
# $(6): VID offset (optional)
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
	cp $(KDIR)/uImage-initramfs-$(2) $(BIN_DIR)/$(IMG_PREFIX)-$(2)-initramfs
  endef

  define Image/Build/Profile/$(1)/squashfs
	$(call Image/Build/UbinizeImage,$(2),,squashfs, -p $(3) -m $(4) $(if $(5),-s $(5)) $(if $(6),-O $(6)))
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
$(eval $(call NANDProfile,Mamba,armada-xp-linksys-mamba,128KiB,2048,512,2048))
$(eval $(call NANDProfile,Caiman,armada-385-linksys-caiman,128KiB,2048,512,2048))
$(eval $(call NANDProfile,Cobra,armada-385-linksys-cobra,128KiB,2048,512,2048))
$(eval $(call NANDProfile,Shelby,armada-385-linksys-shelby,128KiB,2048,512,2048))

# Boards with large NOR, where we want to use UBI
$(eval $(call UBINORProfile,OpenBlocks-AX-3-4,armada-xp-openblocks-ax3-4,128KiB))

# Boards with small NOR, where UBI doesn't make sense
$(eval $(call NORProfile,385-RD,armada-385-rd,256KiB))

###
### Linksys
###

# Caiman: Linksys WRT1200AC
define Image/Build/Profile/Caiman/squashfs
	$(call Image/Build/UbinizeImage,armada-385-linksys-caiman,,squashfs, -p 128KiB -m 2048 -s 512 -O 2048)
	( \
		dd if=$(KDIR)/uImage-armada-385-linksys-caiman bs=6M conv=sync; \
		dd if=$(KDIR)/$(IMG_PREFIX)-armada-385-linksys-caiman-squashfs-ubinized.bin \
		bs=2048 conv=sync; \
	) > $(BIN_DIR)/$(IMG_PREFIX)-armada-385-linksys-caiman-squashfs-factory.img
endef

# Cobra: Linksys WRT1900ACv2
define Image/Build/Profile/Cobra/squashfs
	$(call Image/Build/UbinizeImage,armada-385-linksys-cobra,,squashfs, -p 128KiB -m 2048 -s 512 -O 2048)
	( \
		dd if=$(KDIR)/uImage-armada-385-linksys-cobra bs=6M conv=sync; \
		dd if=$(KDIR)/$(IMG_PREFIX)-armada-385-linksys-cobra-squashfs-ubinized.bin \
		bs=2048 conv=sync; \
	) > $(BIN_DIR)/$(IMG_PREFIX)-armada-385-linksys-cobra-squashfs-factory.img
endef

# Mamba: Linksys WRT1900AC
define Image/Build/Profile/Mamba/squashfs
	$(call Image/Build/UbinizeImage,armada-xp-linksys-mamba,,squashfs, -p 128KiB -m 2048 -s 512 -O 2048)
	( \
		dd if=$(KDIR)/uImage-armada-xp-linksys-mamba bs=3072k conv=sync; \
		dd if=$(KDIR)/$(IMG_PREFIX)-armada-xp-linksys-mamba-squashfs-ubinized.bin \
		bs=2048 conv=sync; \
	) > $(BIN_DIR)/$(IMG_PREFIX)-armada-xp-linksys-mamba-squashfs-factory.img
endef

# Shelby: Linksys WRT1900ACS
define Image/Build/Profile/Shelby/squashfs
	$(call Image/Build/UbinizeImage,armada-385-linksys-shelby,,squashfs, -p 128KiB -m 2048 -s 512 -O 2048)
	( \
		dd if=$(KDIR)/uImage-armada-385-linksys-shelby bs=6M conv=sync; \
		dd if=$(KDIR)/$(IMG_PREFIX)-armada-385-linksys-shelby-squashfs-ubinized.bin \
		bs=2048 conv=sync; \
	) > $(BIN_DIR)/$(IMG_PREFIX)-armada-385-linksys-shelby-squashfs-factory.img
endef

###
### Marvell
###

# Marvell Armada 385 Access Point Development board (DB-88F6820-AP)
define Image/Build/Profile/385-DB-AP/squashfs
	$(call Image/Build/UbinizeImage,armada-385-db-ap,,squashfs, -p 256KiB -m 4096)
	( \
		dd if=$(KDIR)/uImage-armada-385-db-ap bs=8M conv=sync; \
		dd if=$(KDIR)/$(IMG_PREFIX)-armada-385-db-ap-squashfs-ubinized.bin \
		bs=4096 conv=sync; \
	) > $(BIN_DIR)/$(IMG_PREFIX)-armada-385-db-ap-squashfs-factory.img
endef


# The Default profile should build everything
$(eval $(call MultiProfile,Default,$(PROFILES_LIST)))

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
	$(call Image/Build/Profile/$(PROFILE)/BuildSysupgrade,$(1))
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(call Image/Build/Profile/$(PROFILE)/Initramfs)
endif
endef

$(eval $(call BuildImage))
