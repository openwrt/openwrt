# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=airoha-dramc
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://sirherobrine23.com.br/airoha/airoha_mips_dramc.git

PKG_SOURCE_DATE:=2026-09-29
PKG_SOURCE_VERSION:=1fb0feaabab39299623cd2a035433a9126593fc1
PKG_MIRROR_HASH:=skip

PKG_BUILD_DEPENDS:=airoha-dramc/host
PKG_BUILD_PARALLEL:=1
HOST_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/host-build.mk

AIROHA_OUT:=$(PKG_BUILD_DIR)/out
AIROHA_HOST_OUT:=$(HOST_BUILD_DIR)/out

define Package/airoha-dramc
  SECTION:=boot
  CATEGORY:=Boot Loaders
  TITLE:=Airoha/Econet DRAM initialization and TCBoot
  URL:=$(PKG_SOURCE_URL)
  DEPENDS:=@(TARGET_airoha_en751221||TARGET_airoha_en751627||TARGET_airoha_en7528)
  DEFAULT:=y
  HIDDEN:=1
endef

define Host/Compile
	+$(MAKE) -C $(HOST_BUILD_DIR) \
		O="$(AIROHA_HOST_OUT)" \
		HOSTCC="$(HOSTCC)" \
		HOSTCFLAGS="$(HOST_CFLAGS) -Wall -Wextra -Werror -std=c11" \
		hosttools
endef

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/bin
	$(INSTALL_BIN) \
		$(AIROHA_HOST_OUT)/host/econet-image \
		$(STAGING_DIR_HOST)/bin/econet-image
endef

AIROHA_MAKE_FLAGS = \
	O="$(AIROHA_OUT)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	HOST_TOOL="$(STAGING_DIR_HOST)/bin/econet-image" \
	PLAIN_VERSION="$(PKG_SOURCE_DATE)-$(PKG_SOURCE_VERSION)"

define Build/Configure
endef

define Build/Compile
	+$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) \
		$(AIROHA_MAKE_FLAGS) \
		$(SUBTARGET) \
		$(SUBTARGET)-tcboot
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)

	$(INSTALL_DATA) \
		$(AIROHA_OUT)/$(SUBTARGET)/$(SUBTARGET)-dramc.bin \
		$(STAGING_DIR_IMAGE)/$(SUBTARGET)-dramc.bin

	$(INSTALL_DATA) \
		$(AIROHA_OUT)/$(SUBTARGET)/tcboot.bin \
		$(STAGING_DIR_IMAGE)/$(SUBTARGET)-tcboot.bin

	$(INSTALL_DATA) \
		$(AIROHA_OUT)/$(SUBTARGET)/bootext.bin \
		$(STAGING_DIR_IMAGE)/$(SUBTARGET)_bootext.bin
endef

define Package/airoha-dramc/install
endef

$(eval $(call HostBuild))
$(eval $(call BuildPackage,airoha-dramc))
