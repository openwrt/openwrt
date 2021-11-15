#
# Copyright (C) 2021 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=qosify
PKG_SOURCE_URL=$(PROJECT_GIT)/project/qosify.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2021-11-12
PKG_SOURCE_VERSION:=bfc2cafe2a8c66ffab88a7e5c1ad1b0a4d886271
PKG_MIRROR_HASH:=88990d5816f37ba1e334499e7ee53a364fa0ba1e5751a91ef0a94934cd2a5451

PKG_LICENSE:=GPL-2.0
PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>

PKG_BUILD_DEPENDS:=bpf-headers
PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk
include $(INCLUDE_DIR)/bpf.mk
include $(INCLUDE_DIR)/nls.mk

define Package/qosify
  SECTION:=utils
  CATEGORY:=Base system
  TITLE:=A simple QoS solution based eBPF + CAKE
  DEPENDS:=+libbpf +libubox +libubus +kmod-sched-cake +kmod-sched-bpf +kmod-ifb +tc-full $(BPF_DEPENDS)
endef

define Build/Compile
	$(call CompileBPF,$(PKG_BUILD_DIR)/qosify-bpf.c)
	$(Build/Compile/Default)
endef

define Package/qosify/conffiles
/etc/config/qosify
/etc/qosify/00-defaults.conf
endef

define Package/qosify/install
	$(INSTALL_DIR) $(1)/lib/bpf $(1)/usr/sbin $(1)/etc/init.d $(1)/etc/config $(1)/etc/hotplug.d/net $(1)/etc/qosify
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/qosify-bpf.o $(1)/lib/bpf
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/qosify $(1)/usr/sbin/
	$(INSTALL_BIN) ./files/qosify.init $(1)/etc/init.d/qosify
	$(INSTALL_DATA) ./files/qosify-defaults.conf $(1)/etc/qosify/00-defaults.conf
	$(INSTALL_DATA) ./files/qosify.conf $(1)/etc/config/qosify
	$(INSTALL_DATA) ./files/qosify.hotplug $(1)/etc/hotplug.d/net/10-qosify
endef

$(eval $(call BuildPackage,qosify))
