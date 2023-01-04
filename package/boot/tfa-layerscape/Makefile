#
# Copyright 2019 NXP
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=tfa-layerscape
PKG_VERSION:=21.08
PKG_RELEASE:=$(AUTORELEASE)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://source.codeaurora.org/external/qoriq/qoriq-components/atf
PKG_SOURCE_VERSION:=LSDK-21.08
PKG_MIRROR_HASH:=893f2d28a77dcc9d4413a619b4719ca5f1f4dc78dd824a8488e7d543e66bcf95
PKG_BUILD_DEPENDS:=tfa-layerscape/host

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/trusted-firmware-a.mk
include $(INCLUDE_DIR)/package.mk

HOST_CFLAGS += -Wall -Werror -pedantic -std=c99
define Host/Compile
	$(MAKE) -C \
		$(HOST_BUILD_DIR)/tools/fiptool \
		CFLAGS="$(HOST_CFLAGS)" \
		LDFLAGS="$(HOST_LDFLAGS)" \
		HOSTCCFLAGS="$(HOST_CFLAGS)"
	$(MAKE) -C \
		$(HOST_BUILD_DIR)/tools/nxp \
		CFLAGS="$(HOST_CFLAGS)"
endef

define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/tools/fiptool/fiptool $(STAGING_DIR_HOST)/bin/fiptool-layerscape
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/tools/nxp/create_pbl $(STAGING_DIR_HOST)/bin/tfa-create-pbl
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/tools/nxp/byte_swap $(STAGING_DIR_HOST)/bin/tfa-byte-swap
endef

define Trusted-Firmware-A/Default
  BUILD_TARGET:=layerscape
  BUILD_SUBTARGET:=armv8_64b
  DEPENDS:=+layerscape-rcw +u-boot-fsl_$(1)
endef

define Trusted-Firmware-A/ls1012a-frdm
  NAME:=NXP LS1012AFRDM
  PLAT:=ls1012afrdm
  BOOT_MODE:=qspi
endef

define Trusted-Firmware-A/ls1012a-rdb
  NAME:=NXP LS1012ARDB
  PLAT:=ls1012ardb
  BOOT_MODE:=qspi
endef

define Trusted-Firmware-A/ls1012a-frwy-sdboot
  NAME:=NXP LS1012AFRWY
  PLAT:=ls1012afrwy
  BOOT_MODE:=qspi
endef

define Trusted-Firmware-A/ls1043a-rdb
  NAME:=NXP LS1043ARDB
  PLAT:=ls1043ardb
  BOOT_MODE:=nor
endef

define Trusted-Firmware-A/ls1043a-rdb-sdboot
  NAME:=NXP LS1043ARDB SD Boot
  PLAT:=ls1043ardb
  BOOT_MODE:=sd
endef

define Trusted-Firmware-A/ls1046a-frwy
  NAME:=NXP LS1046AFRWY
  PLAT:=ls1046afrwy
  BOOT_MODE:=qspi
endef

define Trusted-Firmware-A/ls1046a-frwy-sdboot
  NAME:=NXP LS1046AFRWY SD Boot
  PLAT:=ls1046afrwy
  BOOT_MODE:=sd
endef

define Trusted-Firmware-A/ls1046a-rdb
  NAME:=NXP LS1046ARDB
  PLAT:=ls1046ardb
  BOOT_MODE:=qspi
endef

define Trusted-Firmware-A/ls1046a-rdb-sdboot
  NAME:=NXP LS1046ARDB SD Boot
  PLAT:=ls1046ardb
  BOOT_MODE:=sd
endef

define Trusted-Firmware-A/ls1088a-rdb
  NAME:=NXP LS1088ARDB
  PLAT:=ls1088ardb
  BOOT_MODE:=qspi
endef

define Trusted-Firmware-A/ls1088a-rdb-sdboot
  NAME:=NXP LS1088ARDB SD Boot
  PLAT:=ls1088ardb
  BOOT_MODE:=sd
endef

define Trusted-Firmware-A/ls2088a-rdb
  NAME:=NXP LS2088ARDB
  PLAT:=ls2088ardb
  BOOT_MODE:=nor
endef

define Trusted-Firmware-A/lx2160a-rdb
  NAME:=NXP LX2160ARDB
  PLAT:=lx2160ardb
  BOOT_MODE:=flexspi_nor
endef

define Trusted-Firmware-A/lx2160a-rdb-sdboot
  NAME:=NXP LX2160ARDB SD Boot
  PLAT:=lx2160ardb
  BOOT_MODE:=sd
endef

TFA_TARGETS := \
  ls1012a-frdm \
  ls1012a-rdb \
  ls1012a-frwy-sdboot \
  ls1043a-rdb \
  ls1043a-rdb-sdboot \
  ls1046a-frwy \
  ls1046a-frwy-sdboot \
  ls1046a-rdb \
  ls1046a-rdb-sdboot \
  ls1088a-rdb \
  ls1088a-rdb-sdboot \
  ls2088a-rdb \
  lx2160a-rdb \
  lx2160a-rdb-sdboot

TFA_MAKE_FLAGS += \
	fip pbl \
	BOOT_MODE=$(BOOT_MODE) \
	RCW=$(STAGING_DIR_IMAGE)/fsl_$(BUILD_VARIANT)-rcw.bin \
	BL33=$(STAGING_DIR_IMAGE)/fsl_$(BUILD_VARIANT)-uboot.bin \
	FIPTOOL=$(STAGING_DIR_HOST)/bin/fiptool-layerscape \
	CREATE_PBL=$(STAGING_DIR_HOST)/bin/tfa-create-pbl \
	BYTE_SWAP=$(STAGING_DIR_HOST)/bin/tfa-byte-swap

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(CP) $(PKG_BUILD_DIR)/build/$(PLAT)/release/bl2_$(BOOT_MODE).pbl \
		$(STAGING_DIR_IMAGE)/fsl_$(BUILD_VARIANT)-bl2.pbl
	$(CP) $(PKG_BUILD_DIR)/build/$(PLAT)/release/fip.bin \
		$(STAGING_DIR_IMAGE)/fsl_$(BUILD_VARIANT)-fip.bin
endef

define Package/trusted-firmware-a/install/default
endef

$(eval $(call HostBuild))
$(eval $(call BuildPackage/Trusted-Firmware-A))
