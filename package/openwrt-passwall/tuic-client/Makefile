# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2017-2020 Yousong Zhou <yszhou4tech@gmail.com>
# Copyright (C) 2021 ImmortalWrt.org

include $(TOPDIR)/rules.mk

PKG_NAME:=tuic-client
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

PKG_LICENSE_FILES:=LICENSE
PKG_MAINTAINER:=Tianling Shen <cnsztl@immortalwrt.org>

include $(INCLUDE_DIR)/package.mk

TUIC_TYPE:=tuic-client-$(PKG_VERSION)
TUIC_FOOT:=unknown-linux-musl
ifeq ($(ARCH),aarch64)
	TUIC_ARCH:=$(TUIC_TYPE)-aarch64-$(TUIC_FOOT)
	PKG_HASH:=c29eaaf3bc05115acc7453ac26bacf9aff65211b1e8ca7f771b818248bec8601
else ifeq ($(ARCH),arm)
# Referred to golang/golang-values.mk
	ARM_CPU_FEATURES:=$(word 2,$(subst +,$(space),$(call qstrip,$(CONFIG_CPU_TYPE))))
	ifeq ($(ARM_CPU_FEATURES),)
		TUIC_ARCH:=$(TUIC_TYPE)-armv7-$(TUIC_FOOT)eabi
		PKG_HASH:=33a83ab05dc1a598552bf1f27114502b12c94b6e5333c6ac2def3739c00a4daf
	else
		TUIC_ARCH:=$(TUIC_TYPE)-armv7-$(TUIC_FOOT)eabihf
		PKG_HASH:=e2cc1d81ac376ff2a94976e78c861f5cd21ad809ff5b587ae967e2a63e4c35e4
	endif
else ifeq ($(ARCH),i386)
	TUIC_ARCH:=$(TUIC_TYPE)-i686-$(TUIC_FOOT)
	PKG_HASH:=67d930b4381a8848bf98acbccb538c73b72ca9dde0aed3705f73e62a9922f758
else ifeq ($(ARCH),x86_64)
	TUIC_ARCH:=$(TUIC_TYPE)-x86_64-$(TUIC_FOOT)
	PKG_HASH:=180c562691247a7feddf553706bc8fd5b5b9de3027154f94a767ea907a45e2de
# Set the default value to make OpenWrt Package Checker happy
else
  PKG_SOURCE:=dummy
  PKG_HASH:=dummy
endif

define Download/tuic-client
	URL:=https://github.com/EAimTY/tuic/releases/download/$(TUIC_TYPE)/
	URL_FILE:=$(TUIC_ARCH)
	FILE:=$(TUIC_ARCH)
	HASH:=$(PKG_HASH)
endef

define Package/tuic-client
	SECTION:=net
	CATEGORY:=Network
	SUBMENU:=Web Servers/Proxies
	TITLE:=Delicately-TUICed 0-RTT proxy protocol
	URL:=https://github.com/EAimTY/tuic/
	DEPENDS:=@USE_MUSL @(aarch64||arm||i386||x86_64) @!(TARGET_x86_geode||TARGET_x86_legacy)
endef

define Build/Prepare
	$(call Build/Prepare/Default)
ifneq ($(CONFIG_PACKAGE_tuic-client),)
	$(call Download,tuic-client)
endif
endef

define Build/Compile
endef

define Package/tuic-client/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(DL_DIR)/$(TUIC_ARCH) $(1)/usr/bin/tuic-client
endef

$(eval $(call BuildPackage,tuic-client))
