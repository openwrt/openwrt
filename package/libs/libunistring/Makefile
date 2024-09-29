include $(TOPDIR)/rules.mk

PKG_NAME:=libunistring
PKG_VERSION:=1.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=@GNU/$(PKG_NAME)
PKG_HASH:=827c1eb9cb6e7c738b171745dac0888aa58c5924df2e59239318383de0729b98

PKG_BUILD_PARALLEL:=1
PKG_INSTALL:=1

PKG_LICENSE:=GPL-3.0
PKG_LICENSE_FILES:=COPYING
PKG_CPE_ID:=cpe:/a:gnu:$(PKG_NAME)

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/package.mk

define Package/libunistring
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=libunistring
  URL:=https://www.gnu.org/software/libunistring
endef

define Package/libunistring/description
  This library provides functions for manipulating Unicode strings
  and for manipulating C strings according to the Unicode standard.
endef

HOST_CFLAGS += $(HOST_FPIC)

TARGET_CFLAGS += $(FPIC)

HOST_CONFIGURE_ARGS += \
	--enable-static \
	--disable-shared \
	--without-libiconv-prefix \
	--with-pic

CONFIGURE_ARGS += \
	--enable-static \
	--enable-shared \
	--without-libiconv-prefix \
	--with-pic

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/*.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/include/unistring
	$(CP) $(PKG_INSTALL_DIR)/usr/include/unistring/*.h $(1)/usr/include/unistring/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libunistring.{a,so*} $(1)/usr/lib/
endef

define Package/libunistring/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libunistring.so.* $(1)/usr/lib/
endef

$(eval $(call HostBuild))
$(eval $(call BuildPackage,libunistring))
