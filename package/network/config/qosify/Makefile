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
PKG_SOURCE_DATE:=2021-11-08.1
PKG_SOURCE_VERSION:=3a420e272c187885120b964059abdf3e06f10199
PKG_MIRROR_HASH:=49a6bbb0801b790ce603aa8e3dfa73e681dc3f58fb9cbd524956bffe4c36eb7e

PKG_LICENSE:=GPL-2.0
PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>

PKG_BUILD_DEPENDS:=bpf-headers

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk
include $(INCLUDE_DIR)/bpf.mk
include $(INCLUDE_DIR)/nls.mk

define Package/qosify
  SECTION:=net
  CATEGORY:=Network
  TITLE:=A simple QoS solution based eBPF + CAKE
  DEPENDS:=+libbpf +libubox +libubus +kmod-sched-cake +kmod-sched-bpf +kmod-ifb +tc-full $(BPF_DEPENDS)
endef

define Build/Compile
	$(call CompileBPF,$(PKG_BUILD_DIR)/qosify-bpf.c)
	$(Build/Compile/Default)
endef

define Package/qosify/conffiles
/etc/config/qosify
/etc/qosify-defaults.conf
endef

define Package/qosify/install
	$(INSTALL_DIR) $(1)/lib/bpf $(1)/usr/sbin $(1)/etc/init.d $(1)/etc/config $(1)/etc/hotplug.d/net
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/qosify-bpf.o $(1)/lib/bpf
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/qosify $(1)/usr/sbin/
	$(INSTALL_BIN) ./files/qosify.init $(1)/etc/init.d/qosify
	$(INSTALL_DATA) ./files/qosify-defaults.conf $(1)/etc/qosify-defaults.conf
	$(INSTALL_DATA) ./files/qosify.conf $(1)/etc/config/qosify
	$(INSTALL_DATA) ./files/qosify.hotplug $(1)/etc/hotplug.d/net/10-qosify
endef

$(eval $(call BuildPackage,qosify))
