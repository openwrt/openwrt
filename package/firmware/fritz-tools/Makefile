include $(TOPDIR)/rules.mk

PKG_NAME:=fritz-tools
PKG_RELEASE:=1
CMAKE_INSTALL:=1
PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/fritz-tools
  SECTION:=firmware
  CATEGORY:=Firmware
  DEPENDS:=@TARGET_ipq806x +zlib
  TITLE:=Utilites for accessing AVM Calibration data and TFFS
endef

define Package/fritz-tools/description
 This package contains:
 fritz_cal_extract.c: tool to extract WLAN calibration data.
 fritz_tffs_read.c: partially read the TFFS filesystems.
endef

define Package/fritz-tools/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/fritz_cal_extract $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/fritz_tffs_read $(1)/usr/bin/
endef

$(eval $(call BuildPackage,fritz-tools))
