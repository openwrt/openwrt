#
# Copyright (C) 2013 Riccardo Ferrazzo <f.riccardo87@gmail.com>
# Copyright (C) 2017 Paweł Knioła <pawel.kn@gmail.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=qt5
PKG_VERSION:=5.9
PKG_RELEASE:=9
PKG_MD5SUM:=97e81709b57e82ab2b279408eaa9270e

PKG_SOURCE:=qt-everywhere-opensource-src-$(PKG_VERSION).$(PKG_RELEASE).tar.xz
PKG_SOURCE_URL:=http://download.qt-project.org/archive/qt/$(PKG_VERSION)/$(PKG_VERSION).$(PKG_RELEASE)/single
PKG_BUILD_DIR=$(BUILD_DIR)/qt-everywhere-opensource-src-$(PKG_VERSION).$(PKG_RELEASE)
PKG_BUILD_PARALLEL:=1
PKG_USE_MIPS16:=0
PKG_BUILD_DEPENDS:=+libstdcpp

include $(INCLUDE_DIR)/package.mk
-include $(if $(DUMP),,./files/qmake.mk)

# not using sstrip here as this fucks up the .so's somehow
STRIP:=/bin/true
RSTRIP:= \
  NM="$(TOOLCHAIN_DIR)/bin/$(TARGET_CROSS)nm" \
  STRIP="$(STRIP)" \
  STRIP_KMOD="$(STRIP)" \
  $(SCRIPT_DIR)/rstrip.sh

define Package/qt5/Default
  SECTION:=libs
  CATEGORY:=Libraries
  SUBMENU:=Qt5
  TITLE:=qt5
  URL:=http://qt-project.org
  DEPENDS:=+librt +zlib +libstdcpp +libpthread @!LINUX_2_6
endef

define Package/qt5-core
  $(call Package/qt5/Default)
  TITLE+=core
endef

define Package/qt5-concurrent
  $(call Package/qt5/Default)
  TITLE+=concurrent
  DEPENDS+=+qt5-core
endef

define Package/qt5-network
  $(call Package/qt5/Default)
  TITLE+=network
  DEPENDS+=+qt5-core
endef

define Package/qt5-widgets
  $(call Package/qt5/Default)
  TITLE+=widgets
  DEPENDS+=+qt5-core +qt5-network
endef

define Package/qt5-sql
  $(call Package/qt5/Default)
  TITLE+=sql
  DEPENDS+=+qt5-core
endef

define Package/qt5-xml
  $(call Package/qt5/Default)
  TITLE+=xml
  DEPENDS+=+qt5-core
endef

define Package/qt5-xmlpatterns
  $(call Package/qt5/Default)
  TITLE+=xmlpatterns
  DEPENDS+=+qt5-core +qt5-network
endef

define Package/qt5-test
  $(call Package/qt5/Default)
  TITLE+=test
  DEPENDS+=+qt5-core
endef

define Build/Configure
	$(INSTALL_DIR) $(PKG_BUILD_DIR)/qtbase/lib/fonts
	$(INSTALL_DIR) $(PKG_BUILD_DIR)/qtbase/mkspecs/linux-openwrt-g++
	$(CP) ./files/fonts/* $(PKG_BUILD_DIR)/qtbase/lib/fonts/
	$(CP) ./files/qplatformdefs.h $(PKG_BUILD_DIR)/qtbase/mkspecs/linux-openwrt-g++/qplatformdefs.h
	$(CP) ./files/qmake.conf $(PKG_BUILD_DIR)/qtbase/mkspecs/linux-openwrt-g++/qmake.conf
	$(SED) 's@$$$$(TARGET_CROSS)@$(TARGET_CROSS)@g' $(PKG_BUILD_DIR)/qtbase/mkspecs/linux-openwrt-g++/qmake.conf
	( cd $(PKG_BUILD_DIR) ; \
		TARGET_CC="$(TARGET_CROSS)gcc" \
		TARGET_CXX="$(TARGET_CROSS)g++" \
		TARGET_AR="$(TARGET_CROSS)ar cqs" \
		TARGET_OBJCOPY="$(TARGET_CROSS)objcopy" \
		TARGET_RANLIB="$(TARGET_CROSS)ranlib" \
		TARGET_CFLAGS="$(TARGET_CFLAGS) $(EXTRA_CFLAGS)" \
		TARGET_CXXFLAGS="$(TARGET_CFLAGS) $(EXTRA_CFLAGS)" \
		TARGET_LDFLAGS="$(TARGET_LDFLAGS) $(EXTRA_LDFLAGS) -lpthread -lrt" \
		TARGET_INCDIRS="$(TARGET_INCDIRS)" \
		TARGET_LIBDIRS="$(TARGET_LIBDIRS) $(STAGING_DIR)/usr/lib/" \
		CFLAGS= \
		CXXFLAGS= \
		LDFLAGS= \
		./configure \
			-prefix /usr \
			-extprefix $(TOOLCHAIN_DIR) \
			-sysroot $(TOOLCHAIN_DIR) \
			-plugindir /usr/lib/Qt/plugins \
			-xplatform linux-openwrt-g++ \
			-opensource \
			-confirm-license \
			-no-iconv \
			-no-pch \
			-no-rpath \
			-no-qml-debug \
			-no-strip \
			-no-cups \
			-no-dbus \
			-no-eglfs \
			-no-kms \
			-no-opengl \
			-no-directfb \
			-no-xcb \
			-qt-zlib \
			-qt-freetype \
			-make libs \
			-nomake examples \
			-nomake tests \
			-skip qtmultimedia \
			-skip activeqt \
			-skip qtdoc \
			-skip qtconnectivity \
			-skip wayland \
			-skip qtscript \
			-v \
	)
endef

define Build/Compile
	TARGET_CC="$(TARGET_CROSS)gcc" \
	TARGET_CXX="$(TARGET_CROSS)g++" \
	TARGET_AR="$(TARGET_CROSS)ar cqs" \
	TARGET_OBJCOPY="$(TARGET_CROSS)objcopy" \
	TARGET_RANLIB="$(TARGET_CROSS)ranlib" \
	TARGET_CFLAGS="$(TARGET_CFLAGS) $(EXTRA_CFLAGS)" \
	TARGET_CXXFLAGS="$(TARGET_CFLAGS) $(EXTRA_CFLAGS)" \
	TARGET_LDFLAGS="$(TARGET_LDFLAGS) $(EXTRA_LDFLAGS) -ldl -lpthread -lrt" \
	TARGET_INCDIRS="$(TARGET_INCDIRS)" \
	TARGET_LIBDIRS="$(TARGET_LIBDIRS) $(STAGING_DIR)/usr/lib/" \
	STAGING_DIR="$(STAGING_DIR)" \
	STAGING_DIR_HOST="$(STAGING_DIR)/../host" \
	PKG_CONFIG_SYSROOT="$(STAGING_DIR)" \
	$(MAKE) -C $(PKG_BUILD_DIR)
endef

define Build/InstallDev
	$(MAKE) -C $(PKG_BUILD_DIR) install
	$(CP) $(PKG_BUILD_DIR)/qtbase/bin/qmake $(TOOLCHAIN_DIR)/bin/
endef

define Package/qt5-core/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/qtbase/lib/libQt5Core.so* $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/qtbase/lib/libQt5Core.prl $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/qtbase/lib/libQt5Core.la $(1)/usr/lib/
	$(CP) $(TOOLCHAIN_DIR)/lib/libatomic.so* $(1)/usr/lib/
endef

define Package/qt5-concurrent/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/qtbase/lib/libQt5Concurrent.so* $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/qtbase/lib/libQt5Concurrent.prl $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/qtbase/lib/libQt5Concurrent.la $(1)/usr/lib/
endef

define Package/qt5-network/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/qtbase/lib/libQt5Network.so* $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/qtbase/lib/libQt5Network.prl $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/qtbase/lib/libQt5Network.la $(1)/usr/lib/
endef

define Package/qt5-widgets/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/Qt/plugins/generic/
	$(INSTALL_DIR) $(1)/usr/lib/Qt/plugins/platforms/
	$(INSTALL_DIR) $(1)/usr/lib/Qt/plugins/imageformats/
	$(INSTALL_DIR) $(1)/usr/lib/fonts/
	$(CP) $(PKG_BUILD_DIR)/qtbase/lib/libQt5Gui.so* $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/qtbase/lib/libQt5Gui.prl $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/qtbase/lib/libQt5Gui.la 	$(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/qtsvg/lib/libQt5Svg.so* $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/qtsvg/lib/libQt5Svg.prl $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/qtsvg/lib/libQt5Svg.la $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/qtbase/lib/libQt5Widgets.so* $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/qtbase/lib/libQt5Widgets.prl $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/qtbase/lib/libQt5Widgets.la $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/qtbase/plugins/generic/*.so $(1)/usr/lib/Qt/plugins/generic/
	$(CP) $(PKG_BUILD_DIR)/qtbase/plugins/platforms/*.so $(1)/usr/lib/Qt/plugins/platforms/
	$(CP) $(PKG_BUILD_DIR)/qtbase/plugins/imageformats/*.so $(1)/usr/lib/Qt/plugins/imageformats/
	$(CP) $(PKG_BUILD_DIR)/qtbase/lib/fonts/* $(1)/usr/lib/fonts/
endef

define Package/qt5-sql/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/Qt/plugins/sqldrivers/
	$(CP) $(PKG_BUILD_DIR)/qtbase/lib/libQt5Sql.so* $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/qtbase/lib/libQt5Sql.prl $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/qtbase/lib/libQt5Sql.la 	$(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/qtbase/plugins/sqldrivers/*.so $(1)/usr/lib/Qt/plugins/sqldrivers/
endef

define Package/qt5-xml/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/qtbase/lib/libQt5Xml.so* $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/qtbase/lib/libQt5Xml.prl $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/qtbase/lib/libQt5Xml.la 	$(1)/usr/lib/
endef

define Package/qt5-xmlpatterns/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/qtxmlpatterns/lib/libQt5XmlPatterns.so* $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/qtxmlpatterns/lib/libQt5XmlPatterns.prl $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/qtxmlpatterns/lib/libQt5XmlPatterns.la $(1)/usr/lib/
endef

define Package/qt5-test/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/qtbase/lib/libQt5Test.so* $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/qtbase/lib/libQt5Test.prl $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/qtbase/lib/libQt5Test.la $(1)/usr/lib/
endef

$(eval $(call BuildPackage,qt5-core))
$(eval $(call BuildPackage,qt5-concurrent))
$(eval $(call BuildPackage,qt5-network))
$(eval $(call BuildPackage,qt5-widgets))
$(eval $(call BuildPackage,qt5-sql))
$(eval $(call BuildPackage,qt5-xml))
$(eval $(call BuildPackage,qt5-xmlpatterns))
$(eval $(call BuildPackage,qt5-test))
