include $(TOPDIR)/rules.mk

PKG_NAME:=umbim
PKG_VERSION:=2014-09-09
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://git.openwrt.org/project/umbim.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=4ae19ab4fcdd70dbfc8c303d752502f2da5a7e75
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
PKG_MAINTAINER:=John Crispin <blogic@openwrt.org>

PKG_LICENSE:=GPLv2
PKG_LICENSE_FILES:=

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/umbim
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libubox +kmod-usb-net +kmod-usb-net-cdc-mbim
  TITLE:=Control utility for mobile broadband modems
endef

define Package/umbim/description
  umbim is a command line tool for controlling mobile broadband modems using
  the MBIM-protocol.
endef

TARGET_CFLAGS += \
	-I$(STAGING_DIR)/usr/include -ffunction-sections -fdata-sections

TARGET_LDFLAGS += -Wl,--gc-sections

define Package/umbim/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/umbim $(1)/sbin/
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,umbim))
