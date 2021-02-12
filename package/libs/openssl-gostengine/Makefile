include $(TOPDIR)/rules.mk

PKG_NAME:=openssl-gostengine
PKG_RELEASE:=1.1.0r2
CMAKE_INSTALL:=1
ENGINES_DIR:=engines-1.1
OPENSSL_INCLUDE_DIR:=/opt/include/openssl

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/openssl-gostengine/Default
  TITLE:=A reference implementation of the Russian GOST crypto algorithms for OpenSSL
  URL:=https://github.com/gost-engine/engine/
  SECTION:=libs
  CATEGORY:=Libraries
endef

define Package/openssl-gostengine/Default/description
A reference implementation of the Russian GOST crypto algorithms for OpenSSL
endef

define Package/openssl-gostengine
  $(call Package/openssl-gostengine/Default)
  SUBMENU:=SSL
  TITLE:=A reference implementation of the Russian GOST crypto algorithms for OpenSSL
  DEPENDS:=+libopenssl
  TITLE+= (libraries)
endef

define Package/openssl-gostengine/description
  $(call Package/openssl-gostengine/Default/description)
  A reference implementation of the Russian GOST crypto algorithms for OpenSSL
endef

define Package/openssl-gostengine/install
	$(INSTALL_DIR) $(1)/opt/lib/$(ENGINES_DIR)$(INSTALL_DIR) $(1)/opt/lib/$(ENGINES_DIR)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/bin/gost.so $(1)/opt/lib/${ENGINES_DIR}/gost.so
	$(INSTALL_DIR) $(1)/opt/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bin/gost12sum $(1)/opt/bin/gost12sum
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bin/gostsum $(1)/opt/bin/gostsum
endef

$(eval $(call BuildPackage,openssl-gostengine))
