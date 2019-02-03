include $(TOPDIR)/rules.mk

PKG_NAME:=cifsd-tools
PKG_SOURCE_DATE:=2019-05-31
PKG_SOURCE_VERSION:=d2b3a1b94e7bd09b75413a70f182c738f6528856
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_SOURCE_VERSION).tar.gz
PKG_SOURCE_URL:=https://codeload.github.com/cifsd-team/cifsd-tools/tar.gz/$(PKG_SOURCE_VERSION)?
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_SOURCE_VERSION)
PKG_HASH:=678d6a1f1f1620369c8f14d37070b38d016811177d72a227eeedccf34e80a0d3

PKG_MAINTAINER:=Andy Walsh <andy.walsh44+github@gmail.com>
PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=COPYING

PKG_INSTALL:=1
PKG_FIXUP:=autoreconf
PKG_REMOVE_FILES:=autogen.sh aclocal.m4

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

define Package/cifsd-tools
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Filesystem
  TITLE:=Kernel CIFS/SMB server support and userspace tools
  DEPENDS:=+kmod-fs-cifsd +glib2 +libnl-core +libnl-genl $(ICONV_DEPENDS)
endef

define Package/cifsd-tools/description
  Userspace tools (cifsd, cifsadmin) for the CIFS/SMB kernel fileserver.
  The config file location is /etc/cifs/smb.conf
endef

define Package/cifsd-tools/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libcifsdtools.so* $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/{cifsadmin,cifsd} $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/config $(1)/etc/cifs $(1)/etc/init.d
	$(INSTALL_DATA) ./files/cifsd.config $(1)/etc/config/cifsd
	$(INSTALL_DATA) ./files/smb.conf.template $(1)/etc/cifs/
	$(INSTALL_BIN) ./files/cifsd.init $(1)/etc/init.d/cifsd
	# copy examples until we have a wiki page
	$(INSTALL_DATA) ./files/cifsd.config.example $(1)/etc/cifs/
	$(INSTALL_DATA) ./files/smb.conf.help $(1)/etc/cifs/
endef

define Package/cifsd-tools/conffiles
/etc/config/cifsd
/etc/cifs/smb.conf.template 
/etc/cifs/smb.conf
/etc/cifs/cifsdpwd.db
endef 

$(eval $(call BuildPackage,cifsd-tools))
