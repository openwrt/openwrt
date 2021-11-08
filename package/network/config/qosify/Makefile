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
PKG_SOURCE_DATE:=2021-11-08
PKG_SOURCE_VERSION:=f5ae89e8d8699a144bbdf16da859185f31d272f6
PKG_MIRROR_HASH:=91831ec8c03c091431b7207b8b5582d396455a027c05c26a9b563fdd26f58186

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
  DEPENDS:=+libbpf +libubox +libubus +kmod-sched-cake +kmod-sched-bpf +tc-full $(BPF_DEPENDS)
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
