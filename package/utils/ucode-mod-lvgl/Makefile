include $(TOPDIR)/rules.mk

PKG_NAME:=ucode-mod-lvgl
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/blogic/ucode-mod-lvgl.git
PKG_MIRROR_HASH:=e8ab35789be25780d9909ba20eaa88f174f240e4201ea2d6f98ea94a667515d0
PKG_SOURCE_DATE:=2026-10-22
PKG_SOURCE_VERSION:=df617f0018616bd860ad1a0a1aa77e088ce7b1d4

PKG_BUILD_DEPENDS:=lvgl

PKG_LICENSE:=GPL-2.0-only OFL-1.1
PKG_LICENSE_FILES:=LICENSE.txt assets/fonts/OFL.txt
PKG_MAINTAINER:=John Crispin <john@phrozen.org>

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

CMAKE_INSTALL:=1

define Package/ucode-mod-lvgl
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=LVGL module for ucode
  DEPENDS:=+libucode +libdrm +ucode-mod-fs +ucode-mod-uci \
	+ucode-mod-uloop +ucode-mod-ubus
endef

define Package/ucode-mod-lvgl/description
Binds LVGL to ucode, so a user interface for a small DRM display can be written
in ucode rather than in C. Also carries the panel library the applications on it
share, imported as lvgl.<name>, with the Inter faces and the shared icons it
draws. Application artwork is loaded at run time with lv.image_load().
endef

define Package/ucode-mod-lvgl/install
	$(INSTALL_DIR) $(1)/usr/lib/ucode $(1)/usr/share/ucode $(1)/usr/share/lvgl
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/ucode/lv.so $(1)/usr/lib/ucode/
	$(CP) $(PKG_INSTALL_DIR)/usr/share/ucode/lvgl $(1)/usr/share/ucode/
	$(CP) $(PKG_INSTALL_DIR)/usr/share/lvgl/. $(1)/usr/share/lvgl/
endef

$(eval $(call BuildPackage,ucode-mod-lvgl))
