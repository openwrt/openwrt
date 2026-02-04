include $(TOPDIR)/rules.mk

PKG_NAME:=ucode-mod-pkgen
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0-or-later
PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

CMAKE_INSTALL := 1

define Package/ucode-mod-pkgen
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=ucode module for generating public keys/certificates
  DEPENDS:=+libucode +libmbedtls
endef

define Package/ucode-mod-pkgen/description
The pkgen module provides functionality for generating cryptographic keys and
(self-)signed certificates. It supports exporting PEM/DER format files, as
well as PKCS#12 bundle for client cert/key pairs with CA.
endef

define Package/pkgen
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=ucode script for generating public keys/certificates
  DEPENDS:=+ucode +ucode-mod-pkgen +ucode-mod-fs
endef

define Package/ucode-mod-pkgen/install
	$(INSTALL_DIR) $(1)/usr/lib/ucode
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/ucode/pkgen.so $(1)/usr/lib/ucode/
endef

define Package/pkgen/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) ./files/pkgen $(1)/usr/bin
endef

$(eval $(call BuildPackage,ucode-mod-pkgen))
$(eval $(call BuildPackage,pkgen))
