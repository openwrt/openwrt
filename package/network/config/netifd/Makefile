include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=netifd
PKG_RELEASE:=3

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=$(PROJECT_GIT)/project/netifd.git
PKG_SOURCE_DATE:=2025-10-20
PKG_SOURCE_VERSION:=777f5942fa7d6245f6ad29daa1daecc400344d37
PKG_MIRROR_HASH:=7fc56f436faa1a5b05a147febed52c6f58c479125a38c9737736b352cd8d4409
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
