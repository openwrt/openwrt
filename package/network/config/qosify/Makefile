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
PKG_SOURCE_DATE:=2022-02-20
PKG_SOURCE_VERSION:=65b42032063f75d8efc37cdb7215a04818be2fa7
PKG_MIRROR_HASH:=0458a9fd7e90fc64239712435f24e7d74b2a3aefcfb0c5f64a9fd70bfd0fe7ae

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
	$(INSTALL_DIR) \
		$(1)/lib/bpf \
		$(1)/usr/sbin \
		$(1)/etc/init.d \
		$(1)/etc/config \
		$(1)/etc/qosify \
		$(1)/etc/hotplug.d/net \
		$(1)/etc/hotplug.d/iface
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/qosify-bpf.o $(1)/lib/bpf
	$(INSTALL_BIN) \
		$(PKG_INSTALL_DIR)/usr/bin/qosify \
		./files/qosify-status \
		$(1)/usr/sbin/
	$(INSTALL_BIN) ./files/qosify.init $(1)/etc/init.d/qosify
	$(INSTALL_DATA) ./files/qosify-defaults.conf $(1)/etc/qosify/00-defaults.conf
	$(INSTALL_DATA) ./files/qosify.conf $(1)/etc/config/qosify
	$(INSTALL_DATA) ./files/qosify.hotplug $(1)/etc/hotplug.d/net/10-qosify
	$(INSTALL_DATA) ./files/qosify.hotplug $(1)/etc/hotplug.d/iface/10-qosify
endef

$(eval $(call BuildPackage,qosify))
