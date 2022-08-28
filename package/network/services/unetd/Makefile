#
# Copyright (C) 2022 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=unetd
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=$(PROJECT_GIT)/project/unetd.git
PKG_SOURCE_DATE:=2022-08-28
PKG_SOURCE_VERSION:=74a14c00abb07d2d8c79c61c459f70d28764dee6
PKG_MIRROR_HASH:=3f575f66d8addd887a194a809c8aa932f79e9a2c6000566fc0c947137bc8b2c8

PKG_LICENSE:=GPL-2.0
PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>

PKG_BUILD_DEPENDS:=bpf-headers

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk
include $(INCLUDE_DIR)/bpf.mk
include $(INCLUDE_DIR)/nls.mk

define Package/unetd
  SECTION:=net
  CATEGORY:=Network
  TITLE:=WireGuard based VPN connection manager for OpenWrt
  DEPENDS:=+libubox +libubus +libblobmsg-json +libnl-tiny +kmod-wireguard +libbpf $(BPF_DEPENDS)
endef

define Package/unet-cli
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=unetd +ucode +ucode-mod-fs
  TITLE:=unetd administration command line utility
endef

TARGET_CFLAGS += \
	-I$(STAGING_DIR)/usr/include/libnl-tiny \
	-I$(STAGING_DIR)/usr/include

CMAKE_OPTIONS += \
	-DLIBNL_LIBS=-lnl-tiny

define Build/Compile
	$(call CompileBPF,$(PKG_BUILD_DIR)/mss-bpf.c)
	$(call Build/Compile/Default,)
endef

define Package/unetd/conffiles
/etc/unetd
endef

define Package/unetd/install
	$(INSTALL_DIR) \
		$(1)/etc/unetd \
		$(1)/lib/bpf \
		$(1)/etc/init.d \
		$(1)/lib/netifd/proto \
		$(1)/usr/sbin \
		$(1)/usr/lib
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/libunet.so* $(1)/usr/lib/
	$(INSTALL_BIN) \
		$(PKG_INSTALL_DIR)/usr/sbin/unetd \
		$(PKG_INSTALL_DIR)/usr/sbin/unet-tool \
		$(1)/usr/sbin/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/mss-bpf.o $(1)/lib/bpf/mss.o
	$(INSTALL_BIN) ./files/unetd.init $(1)/etc/init.d/unetd
	$(INSTALL_BIN) ./files/unetd.sh $(1)/lib/netifd/proto
endef

define Package/unet-cli/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/scripts/unet-cli $(1)/usr/sbin
endef

$(eval $(call BuildPackage,unetd))
$(eval $(call BuildPackage,unet-cli))
