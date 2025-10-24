include $(TOPDIR)/rules.mk

PKG_VERSION:=2024.04
PKG_HASH:=18a853fe39fad7ad03a90cc2d4275aeaed6da69735defac3492b80508843dd4a
PKG_RELEASE:=$(AUTORELEASE)

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk

define U-Boot/Default
  BUILD_TARGET:=bmips
  HCS_IMAGE:=
endef

define U-Boot/xg6846
  NAME:=Inteno XG6846
  UBOOT_CONFIG:=inteno_xg6846_ram
  BUILD_DEVICES:=inteno_xg6846
  BUILD_TARGET:=bmips
  BUILD_SUBTARGET:=bcm6328
  UBOOT_BOARD:=$(1)
endef

define U-Boot/netgear_cg3100d_ram
  NAME:=Netgear CG3100D
  UBOOT_CONFIG:=netgear_cg3100d_ram
  BUILD_DEVICES:=netgear_cg3100d_ram
  BUILD_TARGET:=bmips
  BUILD_SUBTARGET:=bcm3380
  UBOOT_IMAGE:=u-boot.bin

  HCS_IMAGE := u-boot-with-hcs.bin
  HCS_LOADADDR := 0x80010000
  HCS_MAGIC_BYTES := 0xa0e3
  HCS_REV_MIN := 5555
  HCS_REV_MAJ := 0003
endef

UBOOT_TARGETS := \
	netgear_cg3100d_ram \
	xg6846

# If HCS_IMAGE is specified, use aeolus or hcsmakeimage
# to prepend header to the u-boot binary.
define Package/u-boot/install/post-process

ifneq ($(HCS_IMAGE),)
ifeq ($(CONFIG_BCM33XX_PACKER_AEOLUS),y)
	$(STAGING_DIR_HOST)/bin/aeolus_ProgramStore \
		-c 4 \
		-f $(patsubst %,$(PKG_BUILD_DIR)/%,$(UBOOT_IMAGE)) \
		-o $(1)/$(HCS_IMAGE) \
		-s $(HCS_MAGIC_BYTES) -v $(HCS_REV_MAJ).$(HCS_REV_MIN) \
		-a $(HCS_LOADADDR)
else ifeq ($(CONFIG_BCM33XX_PACKER_HCS),y)
	$(STAGING_DIR_HOST)/bin/hcsmakeimage --magic_bytes=$(HCS_MAGIC_BYTES) \
		--rev_maj=$(HCS_REV_MAJ) --rev_min=$(HCS_REV_MIN) \
		--input_file=$(patsubst %,$(PKG_BUILD_DIR)/%,$(UBOOT_IMAGE)) \
		--output_file=$(1)/$(HCS_IMAGE) \
		--ldaddress=$(HCS_LOADADDR)
endif
endif

endef # Package/u-boot/install/post-process

Package/u-boot/install = \
	$(Package/u-boot/install/default); \
	$(Package/u-boot/install/post-process)

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(UBOOT_IMAGE) $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-$(UBOOT_IMAGE)
endef

$(eval $(call BuildPackage/U-Boot))
