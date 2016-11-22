include $(TOPDIR)/rules.mk

PKG_NAME:=netifd
PKG_VERSION:=2016-11-21
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=$(LEDE_GIT)/project/netifd.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=153a12143b9fef4b5d3c3a6597f6fe967a17c9d7
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.xz
PKG_MIRROR_MD5SUM:=5d4126e26a88102bc06d36564b204a989a42a1e4aadf6b2776e123fd3f8883f7
PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/netifd
  SECTION:=base
  CATEGORY:=Base system
  DEPENDS:=+libuci +libnl-tiny +libubus +ubus +ubusd +jshn +libubox
  TITLE:=OpenWrt Network Interface Configuration Daemon
endef

TARGET_CFLAGS += \
	-I$(STAGING_DIR)/usr/include/libnl-tiny \
	-I$(STAGING_DIR)/usr/include

CMAKE_OPTIONS += \
	-DLIBNL_LIBS=-lnl-tiny \
	-DDEBUG=1

define Package/netifd/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/netifd $(1)/sbin/
	$(CP) ./files/* $(1)/
	$(CP) $(PKG_BUILD_DIR)/scripts/* $(1)/lib/netifd/
endef

$(eval $(call BuildPackage,netifd))
