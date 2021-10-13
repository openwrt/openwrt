include $(TOPDIR)/rules.mk

PKG_NAME:=libJudy
PKG_VERSION:=1.0.5
PKG_RELEASE:=1

PKG_FIXUP=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libJudy
  SECTION:=base
  CATEGORY:=Libraries
  DEFAULT:=y
  TITLE:=General purpose dynamic array
  URL:=http://judy.sourceforge.net/
endef

MAKE_FLAGS += \
	HOSTCC="$(HOSTCC)" \
	HOSTCCFLAGS=""

#define Build/Configure
#	$(call Build/Configure/Default,--prefix=/usr)
#endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/Judy.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libJudy.{a,so*} $(1)/usr/lib/
endef

define Package/libJudy/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libJudy.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libJudy))
