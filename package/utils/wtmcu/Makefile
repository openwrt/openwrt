include $(TOPDIR)/rules.mk

PKG_NAME:=wtmcu
PKG_VERSION:=0.1.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-v$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=https://gitlab.com/evgenyz/$(PKG_NAME)/-/archive/v$(PKG_VERSION)
PKG_HASH:=557f8b6a9771dcee094692da4480274695ec2af53871a3ddea069ee94e207509

PKG_LICENSE:=BSD-2-Clause
PKG_MAINTAINER:=Evgeny Kolesnikov <evgenyz@gmail.com>

PKG_BUILD_PARALLEL:=1
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-v$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/wtmcu
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Weltrend MCU control daemon and console tool
	ABI_VERSION:=$(PKG_VERSION)
	DEPENDS:=+libubox +libubus
endef

define Package/wtmcu/description
	This package provides facilities to control Weltrend MCU.
endef

define Package/wtmcu/conffiles
	/etc/config/wtmcu
endef

define Package/wtmcu/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/wtmcud.init $(1)/etc/init.d/wtmcud
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/wtmcu.config $(1)/etc/config/wtmcu
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wtmcuctl $(1)/sbin/wtmcuctl
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wtmcud $(1)/sbin/wtmcud
endef

$(eval $(call BuildPackage,wtmcu))
