include $(TOPDIR)/rules.mk

PKG_NAME:=netifd
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=$(LEDE_GIT)/project/netifd.git
PKG_SOURCE_DATE:=2017-08-22
PKG_SOURCE_VERSION:=b5731bfbabaf7aab0fc47143bd6ad0b12cc3df0a
PKG_MIRROR_HASH:=c2b9f61a1433c9ae8385e72cc648528a1c6d514da26180ec80dba61b07869095
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
