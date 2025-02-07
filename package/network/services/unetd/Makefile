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
PKG_SOURCE_DATE:=2025-01-29
PKG_SOURCE_VERSION:=082b5482b97f20dc4745bc3d645e33b584cc28e4
PKG_MIRROR_HASH:=090e7dab3b9a3358706dcee4f1889b7a1f0bdf535f2d6a0580f4160e23ccf9cb

PKG_LICENSE:=GPL-2.0
PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>

PKG_BUILD_DEPENDS:=HAS_BPF_TOOLCHAIN:bpf-headers

PKG_BUILD_PARALLEL:=1

PKG_CONFIG_DEPENDS += CONFIG_UNETD_VXLAN_SUPPORT

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk
include $(INCLUDE_DIR)/bpf.mk
include $(INCLUDE_DIR)/nls.mk

define Package/unetd
  SECTION:=net
  CATEGORY:=Network
  TITLE:=WireGuard based VPN connection manager for OpenWrt
  DEPENDS:=+libubox +libubus +libblobmsg-json +libnl-tiny +kmod-wireguard +UNETD_VXLAN_SUPPORT:libbpf
endef

define Package/unetd/config
	config UNETD_VXLAN_SUPPORT
		bool "VXLAN support"
		depends on PACKAGE_unetd
		depends on HAS_BPF_TOOLCHAIN
		select NEED_BPF_TOOLCHAIN
		default y

endef

define Package/unet-dht
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=unetd
  TITLE:=unetd DHT discovery support
endef

define Package/unet-cli
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+unetd +ucode +ucode-mod-fs
  TITLE:=unetd administration command line utility
endef

TARGET_CFLAGS += \
	-I$(STAGING_DIR)/usr/include/libnl-tiny \
	-I$(STAGING_DIR)/usr/include

CMAKE_OPTIONS += \
	-DLIBNL_LIBS=-lnl-tiny \
	-DVXLAN_SUPPORT=$(if $(CONFIG_UNETD_VXLAN_SUPPORT),ON,OFF)

ifdef CONFIG_UNETD_VXLAN_SUPPORT
  define Build/Compile
	$(call CompileBPF,$(PKG_BUILD_DIR)/mss-bpf.c)
	$(call Build/Compile/Default,)
  endef
endif

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
	$(if $(CONFIG_UNETD_VXLAN_SUPPORT),$(INSTALL_DATA) $(PKG_BUILD_DIR)/mss-bpf.o $(1)/lib/bpf/mss.o)
	$(INSTALL_BIN) ./files/unetd.init $(1)/etc/init.d/unetd
	$(INSTALL_BIN) ./files/unetd.sh $(1)/lib/netifd/proto
endef

define Package/unet-dht/install
	$(INSTALL_DIR) \
		$(1)/etc/init.d \
		$(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/unet-dht $(1)/usr/sbin
	$(INSTALL_BIN) ./files/unet-dht.init $(1)/etc/init.d/unet-dht
endef

define Package/unet-cli/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/scripts/unet-cli $(1)/usr/sbin
endef

$(eval $(call BuildPackage,unetd))
$(eval $(call BuildPackage,unet-dht))
$(eval $(call BuildPackage,unet-cli))
