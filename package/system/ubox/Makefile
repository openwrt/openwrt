include $(TOPDIR)/rules.mk

PKG_NAME:=ubox
PKG_VERSION:=2014-10-06
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://nbd.name/luci2/ubox.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=0b274c16a3f9d235735a4b84215071e1e004caa9
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
CMAKE_INSTALL:=1
PKG_CHECK_FORMAT_SECURITY:=0

PKG_LICENSE:=LGPL-2.1
PKG_LICENSE_FILES:=

PKG_MAINTAINER:=John Crispin <blogic@openwrt.org>

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

TARGET_LDFLAGS += $(if $(CONFIG_USE_EGLIBC),-lrt)

define Package/ubox
  SECTION:=base
  CATEGORY:=Base system
  DEPENDS:=+libubox +ubusd +ubus +libubus +libuci +USE_EGLIBC:librt
  TITLE:=OpenWrt system helper toolbox
endef

define Package/ubox/install
	$(INSTALL_DIR) $(1)/sbin $(1)/usr/sbin $(1)/lib/ $(1)/etc/init.d/

	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/{kmodloader,logd,logread,validate_data} $(1)/sbin/
	$(INSTALL_BIN) ./files/log.init $(1)/etc/init.d/log
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/libvalidate.so $(1)/lib

	ln -s /sbin/kmodloader $(1)/usr/sbin/rmmod
	ln -s /sbin/kmodloader $(1)/usr/sbin/insmod
	ln -s /sbin/kmodloader $(1)/usr/sbin/lsmod
	ln -s /sbin/kmodloader $(1)/usr/sbin/modinfo
	ln -s /sbin/kmodloader $(1)/usr/sbin/modprobe
endef

$(eval $(call BuildPackage,ubox))
