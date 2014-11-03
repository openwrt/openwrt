#
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=iproute2
PKG_VERSION:=3.16.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=http://kernel.org/pub/linux/utils/net/iproute2/
PKG_MD5SUM:=6c823b40fdcfa7b8120743349a52ac18
PKG_BUILD_PARALLEL:=1
PKG_LICENSE:=GPL-2.0

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(BUILD_VARIANT)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/iproute2/Default
  TITLE:=Routing control utility ($(2))
  SECTION:=net
  CATEGORY:=Network
  URL:=http://www.linuxfoundation.org/collaborate/workgroups/networking/iproute2
  SUBMENU:=Routing and Redirection
  MAINTAINER:=Russell Senior <russell@personaltelco.net>
  DEPENDS:= +libnl-tiny
  VARIANT:=$(1)
endef

define Package/ip
$(call Package/iproute2/Default,tiny,Minimal)
  CONFLICTS:=ip-full
endef

Package/ip-full=$(call Package/iproute2/Default,full,Full)

define Package/ip/conffiles
/etc/iproute2/rt_tables
endef

define Package/ip-$(BUILD_VARIANT)/conffiles
$(Package/ip/conffiles)
endef

define Package/tc
$(call Package/iproute2/Default)
  TITLE:=Traffic control utility
  DEPENDS:=+kmod-sched-core
endef

define Package/genl
$(call Package/iproute2/Default)
  TITLE:=General netlink utility frontend
endef

define Package/ss
$(call Package/iproute2/Default)
  TITLE:=Socket statistics utility
endef

ifeq ($(BUILD_VARIANT),tiny)
  IP_CONFIG_TINY:=y
endif

define Build/Configure
	$(SED) "s,-I/usr/include/db3,," $(PKG_BUILD_DIR)/Makefile
	$(SED) "s,^KERNEL_INCLUDE.*,KERNEL_INCLUDE=$(LINUX_DIR)/include," \
		$(PKG_BUILD_DIR)/Makefile
	$(SED) "s,^LIBC_INCLUDE.*,LIBC_INCLUDE=$(STAGING_DIR)/include," \
		$(PKG_BUILD_DIR)/Makefile
	echo "static const char SNAPSHOT[] = \"$(PKG_VERSION)-$(PKG_RELEASE)-openwrt\";" \
		> $(PKG_BUILD_DIR)/include/SNAPSHOT.h
endef

ifdef CONFIG_USE_EGLIBC
  ifndef CONFIG_EGLIBC_VERSION_2_13
    TARGET_CFLAGS += -DHAVE_SETNS
  endif
endif
ifdef CONFIG_USE_MUSL
  TARGET_CFLAGS += -DHAVE_SETNS
endif

TARGET_CFLAGS += -ffunction-sections -fdata-sections

MAKE_FLAGS += \
	EXTRA_CCOPTS="$(TARGET_CFLAGS) -I../include -I$(STAGING_DIR)/usr/include/libnl-tiny" \
	KERNEL_INCLUDE="$(LINUX_DIR)/include" \
	SHARED_LIBS="" \
	LDFLAGS="-Wl,--gc-sections" \
	IP_CONFIG_TINY=$(IP_CONFIG_TINY) \
	FPIC="$(FPIC)"

define Build/Compile
	+$(MAKE_VARS) $(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) $(MAKE_FLAGS)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/include/libnetlink.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/lib/libnetlink.a $(1)/usr/lib/
endef

define Package/ip/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/etc/iproute2
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/etc/iproute2/rt_tables $(1)/etc/iproute2
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ip/ip $(1)/usr/sbin/
endef

define Package/ip-$(BUILD_VARIANT)/install
	$(Package/ip/install)
endef

define Package/tc/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tc/tc $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_BIN) ./files/15-teql $(1)/etc/hotplug.d/iface/
endef

define Package/genl/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/genl/genl $(1)/usr/sbin/
endef

define Package/ss/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/misc/ss $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,ip))
$(eval $(call BuildPackage,ip-full))
$(eval $(call BuildPackage,tc))
$(eval $(call BuildPackage,genl))
$(eval $(call BuildPackage,ss))
