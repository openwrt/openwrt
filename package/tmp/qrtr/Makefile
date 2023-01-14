include $(TOPDIR)/rules.mk

PKG_NAME:=qrtr
PKG_VERSION:=0.3+git20230105
PKG_RELEASE:=1
_COMMIT:=a4398c8bf271f90338f95e1230373dde977d9cff

PKG_BUILD_DIR:=$(BUILD_DIR)/qrtr-$(_COMMIT)
PKG_SOURCE:=$(_COMMIT).tar.gz
PKG_SOURCE_URL:=https://github.com/andersson/qrtr/archive/
PKG_HASH:=29b22c4b763b07b7234babb0e6423c99a8016099e3fa4a3c8706d847c8ad7c16

PKG_LICENSE:=BSD-3-Clause

include $(INCLUDE_DIR)/package.mk

define Package/qrtr-lib
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=Userspace reference for net/qrtr in the Linux kernel
	URL:=https://github.com/andersson/qrtr
endef

define Package/qrtr
	SECTION:=base
	CATEGORY:=Network
	DEPENDS:=+qrtr-lib
	TITLE:=Various qrtr utilities
	URL:=https://github.com/andersson/qrtr
endef

define Build/Compile
	$(call Build/Compile/Default,prefix=/usr all)
endef

define Build/Install
	$(call Build/Install/Default,prefix=/usr DESTDIR="$(PKG_INSTALL_DIR)" install)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/libqrtr.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/* $(1)/usr/lib/
endef

define Package/qrtr-lib/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/* $(1)/usr/lib/
endef

define Package/qrtr/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/qrtr-* $(1)/usr/bin/
endef

$(eval $(call BuildPackage,qrtr-lib))
$(eval $(call BuildPackage,qrtr))
