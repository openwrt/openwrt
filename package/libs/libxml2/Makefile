include $(TOPDIR)/rules.mk

PKG_NAME:=libxml2
PKG_VERSION:=2.12.6
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=@GNOME/libxml2/$(basename $(PKG_VERSION))
PKG_HASH:=889c593a881a3db5fdd96cc9318c87df34eb648edfc458272ad46fd607353fbb

PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=COPYING
PKG_CPE_ID:=cpe:/a:xmlsoft:libxml2
HOST_BUILD_DEPENDS := libiconv-full/host

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk
include $(INCLUDE_DIR)/nls.mk

define Package/libxml2
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Gnome XML library
  URL:=http://xmlsoft.org/
  DEPENDS:=+libpthread +zlib $(ICONV_DEPENDS)
endef

define Package/libxml2/description
 A library for manipulating XML and HTML resources.
endef

define Package/libxml2-dev
  SECTION:=devel
  CATEGORY:=Development
  SUBMENU:=Libraries
  TITLE:=Development files for libxml2
  URL:=http://xmlsoft.org/
  DEPENDS:=+libxml2
endef

define Package/libxml2-dev/description
  A library for manipulating XML and HTML resources.

  This package contains the headers and xml2-config binary.
endef

define Package/libxml2-utils
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=XML command line utilities (xmllint...)
  URL:=http://xmlsoft.org/
  DEPENDS:=+libxml2
endef

define Package/libxml2-utils/description
  This package contains the binaries xmllint and xmlcatalog
  from libxml2, a library for manipulating XML and HTML resources.
endef

CMAKE_HOST_OPTIONS += \
	-DBUILD_SHARED_LIBS=OFF \
	-DLIBXML2_WITH_C14N=ON \
	-DLIBXML2_WITH_CATALOG=OFF \
	-DLIBXML2_WITH_DEBUG=ON \
	-DLIBXML2_WITH_FTP=OFF \
	-DLIBXML2_WITH_HTML=ON \
	-DLIBXML2_WITH_HTTP=OFF \
	-DLIBXML2_WITH_ICONV=ON \
	-DLIBXML2_WITH_ICU=OFF \
	-DLIBXML2_WITH_ISO8859X=OFF \
	-DLIBXML2_WITH_LEGACY=OFF \
	-DLIBXML2_WITH_LZMA=OFF \
	-DLIBXML2_WITH_MEM_DEBUG=OFF \
	-DLIBXML2_WITH_MODULES=OFF \
	-DLIBXML2_WITH_OUTPUT=ON \
	-DLIBXML2_WITH_PATTERN=ON \
	-DLIBXML2_WITH_PROGRAMS=OFF \
	-DLIBXML2_WITH_PUSH=ON \
	-DLIBXML2_WITH_PYTHON=OFF \
	-DLIBXML2_WITH_READER=ON \
	-DLIBXML2_WITH_REGEXPS=ON \
	-DLIBXML2_WITH_RUN_DEBUG=OFF \
	-DLIBXML2_WITH_SAX1=ON \
	-DLIBXML2_WITH_SCHEMAS=ON \
	-DLIBXML2_WITH_SCHEMATRON=OFF \
	-DLIBXML2_WITH_TESTS=OFF \
	-DLIBXML2_WITH_THREADS=ON \
	-DLIBXML2_WITH_THREAD_ALLOC=OFF \
	-DLIBXML2_WITH_TREE=ON \
	-DLIBXML2_WITH_VALID=ON \
	-DLIBXML2_WITH_WRITER=ON \
	-DLIBXML2_WITH_XINCLUDE=ON \
	-DLIBXML2_WITH_XPATH=ON \
	-DLIBXML2_WITH_XPTR=ON \
	-DLIBXML2_WITH_XPTR_LOCS=ON \
	-DLIBXML2_WITH_ZLIB=ON

CMAKE_OPTIONS += \
	-DBUILD_SHARED_LIBS=ON \
	-DLIBXML2_WITH_C14N=ON \
	-DLIBXML2_WITH_CATALOG=OFF \
	-DLIBXML2_WITH_DEBUG=ON \
	-DLIBXML2_WITH_FTP=OFF \
	-DLIBXML2_WITH_HTML=ON \
	-DLIBXML2_WITH_HTTP=OFF \
	-DLIBXML2_WITH_ICONV=ON \
	-DLIBXML2_WITH_ICU=OFF \
	-DLIBXML2_WITH_ISO8859X=OFF \
	-DLIBXML2_WITH_LEGACY=OFF \
	-DLIBXML2_WITH_LZMA=OFF \
	-DLIBXML2_WITH_MEM_DEBUG=OFF \
	-DLIBXML2_WITH_MODULES=OFF \
	-DLIBXML2_WITH_OUTPUT=ON \
	-DLIBXML2_WITH_PATTERN=ON \
	-DLIBXML2_WITH_PROGRAMS=ON \
	-DLIBXML2_WITH_PUSH=ON \
	-DLIBXML2_WITH_PYTHON=OFF \
	-DLIBXML2_WITH_READER=ON \
	-DLIBXML2_WITH_REGEXPS=ON \
	-DLIBXML2_WITH_RUN_DEBUG=OFF \
	-DLIBXML2_WITH_SAX1=ON \
	-DLIBXML2_WITH_SCHEMAS=ON \
	-DLIBXML2_WITH_SCHEMATRON=OFF \
	-DLIBXML2_WITH_TESTS=OFF \
	-DLIBXML2_WITH_THREADS=ON \
	-DLIBXML2_WITH_THREAD_ALLOC=OFF \
	-DLIBXML2_WITH_TREE=ON \
	-DLIBXML2_WITH_VALID=ON \
	-DLIBXML2_WITH_WRITER=ON \
	-DLIBXML2_WITH_XINCLUDE=ON \
	-DLIBXML2_WITH_XPATH=ON \
	-DLIBXML2_WITH_XPTR=ON \
	-DLIBXML2_WITH_XPTR_LOCS=ON \
	-DLIBXML2_WITH_ZLIB=ON \
	-DHAVE_LIBHISTORY=OFF \
	-DHAVE_LIBREADLINE=OFF

define Build/InstallDev
	$(INSTALL_DIR) $(2)/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/xml2-config \
		$(2)/bin/$(GNU_TARGET_NAME)-xml2-config
	$(SED) 's,^\(prefix\|exec_prefix\)=.*,\1=$(STAGING_DIR)/usr,g' \
		$(2)/bin/$(GNU_TARGET_NAME)-xml2-config
	$(LN) $(GNU_TARGET_NAME)-xml2-config $(2)/bin/xml2-config

	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/xmlcatalog $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/xmllint $(1)/usr/bin/

	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/libxml2 $(1)/usr/include/

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libxml2.so* $(1)/usr/lib/

	$(INSTALL_DIR) $(1)/usr/lib/cmake/libxml2
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/cmake/libxml2-$(PKG_VERSION)/*.cmake \
		$(1)/usr/lib/cmake/libxml2

	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libxml-2.0.pc $(1)/usr/lib/pkgconfig/

	$(INSTALL_DIR) $(2)/share/aclocal/
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/share/aclocal/* $(2)/share/aclocal
endef

define Host/Install
	$(call Host/Install/Default)
	mv $(1)/bin/xml2-config $(1)/bin/$(GNU_HOST_NAME)-xml2-config
	$(LN) $(GNU_HOST_NAME)-xml2-config $(1)/bin/xml2-config
endef

define Package/libxml2/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libxml2.so.* $(1)/usr/lib/
endef

define Package/libxml2-dev/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/xml2-config $(1)/usr/bin/
	$(SED) "s,$(STAGING_DIR),,g" $(1)/usr/bin/xml2-config

	$(INSTALL_DIR) $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include/

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libxml2.so $(1)/usr/lib/

	$(INSTALL_DIR) $(1)/usr/lib/{cmake,pkgconfig}
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/{cmake,pkgconfig} $(1)/usr/lib/
	$(SED) "s,$(STAGING_DIR),,g" $(1)/usr/lib/pkgconfig/*.pc

	$(INSTALL_DIR) $(1)/usr/share/aclocal
	$(CP) $(PKG_INSTALL_DIR)/usr/share/aclocal/* $(1)/usr/share/aclocal
endef

define Package/libxml2-utils/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/xmllint $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/xmlcatalog $(1)/usr/bin/
endef

$(eval $(call HostBuild))
$(eval $(call BuildPackage,libxml2))
$(eval $(call BuildPackage,libxml2-dev))
$(eval $(call BuildPackage,libxml2-utils))
