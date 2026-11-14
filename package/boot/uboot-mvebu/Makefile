#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_VERSION:=2026.10
PKG_RELEASE:=2

PKG_HASH:=3f856da3cbbc93af51ff95fde18588a348b4a26723b01d9841e999d988481015

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/host-build.mk

define U-Boot/Default
  BUILD_TARGET:=mvebu
  HIDDEN:=1
endef

define U-Boot/mox
  NAME:=CZ.NIC Turris Mox
  BUILD_DEVICES:=cznic_turris-mox
  BUILD_SUBTARGET:=cortexa53
  UBOOT_CONFIG:=turris_mox
endef

define U-Boot/clearfog
  NAME:=SolidRun ClearFog A1
  BUILD_DEVICES:=solidrun_clearfog-base-a1 solidrun_clearfog-pro-a1
  BUILD_SUBTARGET:=cortexa9
  UBOOT_IMAGE:=u-boot-with-spl.kwb
endef

define U-Boot/helios4
  NAME:=Kobol Helios 4
  BUILD_DEVICES:=kobol_helios4
  BUILD_SUBTARGET:=cortexa9
  UBOOT_IMAGE:=u-boot-with-spl.kwb
endef

define U-Boot/omnia
  NAME:=Turris Omnia
  BUILD_DEVICES:=cznic_turris-omnia
  BUILD_SUBTARGET:=cortexa9
  UBOOT_CONFIG:=turris_omnia
  UBOOT_IMAGE:=u-boot-with-spl.kwb
endef

define U-Boot/espressobin
  NAME:=Marvell ESPRESSObin
  BUILD_SUBTARGET:=cortexa53
  UBOOT_CONFIG:=mvebu_espressobin-88f3720
endef

define U-Boot/uDPU
  NAME:=Methode uDPU
  BUILD_SUBTARGET:=cortexa53
endef

define U-Boot/eDPU
  NAME:=Methode eDPU
  BUILD_SUBTARGET:=cortexa53
endef

define U-Boot/rb5009
  NAME:=MikroTik RB5009
  BUILD_SUBTARGET:=cortexa72
  BUILD_DEVICES:=mikrotik_rb5009ug mikrotik_rb5009upr
  UBOOT_CONFIG:=mvebu_rb5009
  UBOOT_IMAGE:=u-boot.elf
endef

define U-Boot/atlas
  NAME:=RIPE Atlas Probe v5
  BUILD_DEVICES:=ripe_atlas-v5
  BUILD_SUBTARGET:=cortexa53
  UBOOT_CONFIG:=turris_mox
endef

define U-Boot/rtx830
  NAME:=Yamaha RTX830
  BUILD_DEVICES:=yamaha_rtx830
  BUILD_SUBTARGET:=cortexa9
  UBOOT_CONFIG:=rtx830_sdboot
  UBOOT_IMAGE:=rtx830.bin
endef

UBOOT_TARGETS:= \
	mox \
	clearfog \
	helios4 \
	omnia \
	espressobin \
	atlas \
	uDPU \
	eDPU \
	rb5009 \
	rtx830

# This is a chainloaded image, not a Marvell BootROM KWB image.
UBOOT_MAKE_FLAGS += $(if $(filter rtx830,$(BUILD_VARIANT)),u-boot.bin)

UBOOT_CUSTOMIZE_CONFIG := \
	--disable TOOLS_MKEFICAPSULE

define Package/u-boot/install
	$(if $(findstring cortexa53,$(BUILD_SUBTARGET)),,$(Package/u-boot/install/default))
endef

define Build/Compile
	$(call Build/Compile/U-Boot)
ifeq ($(BUILD_VARIANT),rtx830)
	cd $(PKG_BUILD_DIR) && $(TARGET_CC) -c -x assembler-with-cpp \
		-march=armv7-a -marm -o rtx830_canz_entry.o rtx830_canz_entry.S
	cd $(PKG_BUILD_DIR) && $(TARGET_LD) -T rtx830_canz_entry.ld \
		-o rtx830_canz_entry.elf rtx830_canz_entry.o
	$(TARGET_CROSS)objcopy -O binary $(PKG_BUILD_DIR)/rtx830_canz_entry.elf \
		$(PKG_BUILD_DIR)/rtx830_canz_entry.bin
	rm -f $(PKG_BUILD_DIR)/rtx830.bin $(PKG_BUILD_DIR)/rtx830.bin.manifest.json
	$(STAGING_DIR_HOST)/bin/python3 $(PKG_BUILD_DIR)/rtx830_pack_canz.py create \
		--main $(PKG_BUILD_DIR)/rtx830_canz_entry.bin --output $(PKG_BUILD_DIR)/rtx830.bin
	$(STAGING_DIR_HOST)/bin/python3 $(PKG_BUILD_DIR)/rtx830_pack_canz.py validate $(PKG_BUILD_DIR)/rtx830.bin
endif
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(CP) $(PKG_BUILD_DIR)/$(UBOOT_IMAGE) $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-$(UBOOT_IMAGE)
endef

$(eval $(call BuildPackage/U-Boot))
