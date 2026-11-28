# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=t20pro-calibration
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=COPYING
PKG_MAINTAINER:=wjjsn <wjjsn@qq.com>

include $(INCLUDE_DIR)/package.mk

define Package/t20pro-calibration
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=TCL T20 Pro factory wireless calibration
  DEPENDS:=@TARGET_realtek_rtl960x +libxml2
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	$(CP) $(TOPDIR)/LICENSES/GPL-2.0 $(PKG_BUILD_DIR)/COPYING
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CPPFLAGS) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) \
		-I$(STAGING_DIR)/usr/include/libxml2 \
		-o $(PKG_BUILD_DIR)/t20pro-calibration $(PKG_BUILD_DIR)/calibration.c -lxml2
endef

define Package/t20pro-calibration/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/t20pro-calibration $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/lib/firmware/rtw89
	$(LN) /tmp/t20pro-firmware/tcl-t20-pro-5g-efuse.bin $(1)/lib/firmware/rtw89/
	$(LN) /tmp/t20pro-firmware/tcl-t20-pro-2g-efuse.bin $(1)/lib/firmware/rtw89/
endef

$(eval $(call BuildPackage,t20pro-calibration))
