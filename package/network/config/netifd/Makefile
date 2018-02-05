include $(TOPDIR)/rules.mk

PKG_NAME:=netifd
PKG_RELEASE:=3

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=$(PROJECT_GIT)/project/netifd.git
PKG_SOURCE_DATE:=2018-02-05
PKG_SOURCE_VERSION:=1be329c66326c86d7a48ba71004fcef7691bbbf9
PKG_MIRROR_HASH:=06c1c24ed56b4fe39b92b33719f8bd4f9a8fe3c6a25ce02a6a245e859f1237bf
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
