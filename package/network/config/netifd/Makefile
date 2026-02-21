include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=netifd
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=$(PROJECT_GIT)/project/netifd.git
PKG_SOURCE_DATE:=2026-02-15
PKG_SOURCE_VERSION:=c6122254eb7003377b67a6ad14d284b69725bbee
PKG_MIRROR_HASH:=feb62dba4dfecba1e3e758e5e4e822e9776b4104ee133dccfc3feb7ae6c5182d
PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=

PKG_BUILD_FLAGS:=lto

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/netifd
  SECTION:=base
  CATEGORY:=Base system
  DEPENDS:=+libuci +libnl-tiny +libubus +ubus +ubusd +jshn +libubox +libudebug \
	+ucode +ucode-mod-fs +ucode-mod-ubus +ucode-mod-uloop +ucode-mod-uci
  TITLE:=OpenWrt Network Interface Configuration Daemon
endef

define Package/netifd/conffiles
/etc/udhcpc.user
/etc/udhcpc.user.d/
endef

# Stage kernel ethtool headers separately to avoid ioctl.h conflicts
# Only ethtool_netlink.h needs the newer kernel version (for PSE attributes)
ETHTOOL_HEADERS_DIR := $(PKG_BUILD_DIR)/kernel-headers

define Build/Prepare
	$(call Build/Prepare/Default)
	$(INSTALL_DIR) $(ETHTOOL_HEADERS_DIR)/linux
	$(CP) $(LINUX_DIR)/user_headers/include/linux/ethtool.h $(ETHTOOL_HEADERS_DIR)/linux/
	$(CP) $(LINUX_DIR)/user_headers/include/linux/ethtool_netlink.h $(ETHTOOL_HEADERS_DIR)/linux/
endef

TARGET_CFLAGS += \
	-I$(STAGING_DIR)/usr/include/libnl-tiny \
	-I$(ETHTOOL_HEADERS_DIR) \
	-I$(STAGING_DIR)/usr/include

CMAKE_OPTIONS += \
	-DLIBNL_LIBS=-lnl-tiny \
	-DDEBUG=1

define Package/netifd/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/netifd $(1)/sbin/
	$(CP) ./files/* $(1)/
	$(INSTALL_DIR) $(1)/etc/udhcpc.user.d/
	$(CP) \
		$(PKG_BUILD_DIR)/scripts/utils.sh \
		$(PKG_BUILD_DIR)/scripts/netifd-proto.sh \
		$(1)/lib/netifd/
endef

$(eval $(call BuildPackage,netifd))
