# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=bcm27xx-utils
PKG_VERSION:=2025.03.14
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/raspberrypi/utils.git
PKG_SOURCE_VERSION:=685afa8c0d6f2310eaefe1b528627a8bf3154ca0
PKG_MIRROR_HASH:=04528742fc5b55ba31f448a27588a9df707cbcc27823c1d219b7f877dd4ac200

PKG_FLAGS:=nonshared
PKG_BUILD_FLAGS:=no-lto

PKG_LICENSE:=BSD-3-Clause
PKG_LICENSE_FILES:=LICENCE

CMAKE_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/bcm27xx-utils
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=@TARGET_bcm27xx +libfdt
  TITLE:=BCM27xx scripts and simple applications
  PROVIDES:=bcm27xx-userland
endef

define Package/bcm27xx-utils/description
  BCM27xx scripts and simple applications.
  Replaces bcm27xx-userland scripts and applications.
endef

define Package/bcm27xx-utils/install
	$(INSTALL_DIR) $(1)/usr/bin

	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/dtmerge $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/dtoverlay $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/dtparam $(1)/usr/bin

	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/eepdump $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/eepflash.sh $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/eepmake $(1)/usr/bin

	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/kdtc $(1)/usr/bin

	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/otpset $(1)/usr/bin

	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/overlaycheck $(1)/usr/bin
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/bin/overlaycheck_exclusions.txt $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/ovmerge $(1)/usr/bin

	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/pinctrl $(1)/usr/bin

	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/raspinfo $(1)/usr/bin

	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/vcgencmd $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/vclog $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/vcmailbox $(1)/usr/bin
endef

$(eval $(call BuildPackage,bcm27xx-utils))
