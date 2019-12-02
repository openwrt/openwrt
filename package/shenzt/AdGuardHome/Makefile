#
# Copyright (C) 2015-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v3.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=AdGuardHome
PKG_VERSION:=v0.99.3
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=net
	CATEGORY:=Network
	TITLE:=AdGuardHome
	DEPENDS:=
	URL:=https://github.com/AdguardTeam/AdGuardHome/releases
endef

define Package/$(PKG_NAME)/description
AdGuardHome
endef

ifeq ($(ARCH),x86_64)
	PKG_ARCH_AdGuardHome:=linux_amd64
endif
ifeq ($(ARCH),i386)
	PKG_ARCH_AdGuardHome:=linux_386
endif
ifeq ($(ARCH),mipsel)
	PKG_ARCH_AdGuardHome:=linux_mipsle
endif
ifeq ($(ARCH),mips)
	PKG_ARCH_AdGuardHome:=linux_mips
endif
ifeq ($(ARCH),arm)
	PKG_ARCH_AdGuardHome:=linux_arm
endif
ifeq ($(ARCH),aarch64)
	PKG_ARCH_AdGuardHome:=linux_amd64
endif

define Build/Prepare
	[ ! -f $(PKG_BUILD_DIR)/AdGuardHome_$(PKG_ARCH_AdGuardHome).tar.gz ] && wget  https://github.com/AdguardTeam/AdGuardHome/releases/download/$(PKG_VERSION)/AdGuardHome_$(PKG_ARCH_AdGuardHome).tar.gz -O $(PKG_BUILD_DIR)/AdGuardHome_$(PKG_ARCH_AdGuardHome).tar.gz
	tar -xzvf $(PKG_BUILD_DIR)/AdGuardHome_$(PKG_ARCH_AdGuardHome).tar.gz -C $(PKG_BUILD_DIR)
endef

define Build/Configure
endef

define Build/Compile
	chmod +x $(PKG_BUILD_DIR)/AdGuardHome/AdGuardHome && upx --lzma $(PKG_BUILD_DIR)/AdGuardHome/AdGuardHome
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	
ifeq ($(ARCH),arm)
ifeq ($(BOARD),bcm53xx)
	$(INSTALL_BIN) ./files/AdGuardHome/AdGuardHome $(1)/usr/bin/AdGuardHome
else
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/AdGuardHome/AdGuardHome $(1)/usr/bin/AdGuardHome
endif
else
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/AdGuardHome/AdGuardHome $(1)/usr/bin/AdGuardHome
endif	
	
	$(INSTALL_DIR) $(1)/etc/AdGuardHome
	$(CP) -r ./files/etc/AdGuardHome $(1)/etc
endef

$(eval $(call BuildPackage,$(PKG_NAME)))

