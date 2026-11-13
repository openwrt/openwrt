include $(TOPDIR)/rules.mk

PKG_NAME:=lvgl
PKG_VERSION:=9.3.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://codeload.github.com/lvgl/lvgl/tar.gz/v$(PKG_VERSION)?
PKG_HASH:=4933becfd3603b29158a5d04138139582836ef2bc17beb6c39dccda9cb0d32e7

PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=LICENCE.txt
PKG_MAINTAINER:=John Crispin <john@phrozen.org>

PKG_BUILD_FLAGS:=gc-sections

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

CMAKE_INSTALL:=1

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include/libdrm

CMAKE_OPTIONS += \
	-DBUILD_SHARED_LIBS=OFF \
	-DCMAKE_POSITION_INDEPENDENT_CODE=ON \
	-DLV_BUILD_CONF_DIR=$(CURDIR)/files \
	-DCONFIG_LV_BUILD_DEMOS=OFF \
	-DCONFIG_LV_BUILD_EXAMPLES=OFF \
	-DCONFIG_LV_USE_THORVG_INTERNAL=OFF \
	-DCONFIG_LV_USE_PRIVATE_API=ON

define Package/lvgl
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Light and Versatile Graphics Library
  URL:=https://lvgl.io/
  DEPENDS:=+libdrm
  BUILDONLY:=1
endef

define Package/lvgl/description
LVGL built for a Linux DRM display, as a static library staged for other
packages to link. Carries the widgets, the DRM backend and the evdev input.
The configuration is files/lv_conf.h, which is installed beside the headers so
that every user compiles against the same one.
endef

$(eval $(call BuildPackage,lvgl))
