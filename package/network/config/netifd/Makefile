include $(TOPDIR)/rules.mk

PKG_NAME:=netifd
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=$(LEDE_GIT)/project/netifd.git
PKG_SOURCE_DATE:=2017-12-15
PKG_SOURCE_VERSION:=4268193c90bdd1532ee0555dd109a43eb8524b04
PKG_MIRROR_HASH:=4f577ed5838ed9e33cc8c263198bc2b1d4f222e8259a2bbae367d67b10705a9e
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
