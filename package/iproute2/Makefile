# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=iproute2
PKG_VERSION:=2.6.29-1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://devresources.linux-foundation.org/dev/iproute2/download/
PKG_MD5SUM:=c1bc258a6c345905e79935ac7a3cc582

PKG_BUILD_DIR:=$(BUILD_DIR)/iproute2-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/iproute2/Default
  SECTION:=net
  CATEGORY:=Network
  URL:=http://linux-net.osdl.org/index.php/Iproute2
endef

define Package/ip
$(call Package/iproute2/Default)
  TITLE:=Routing control utility
endef

define Package/tc
$(call Package/iproute2/Default)
  TITLE:=Traffic control utility
endef

define Package/genl
$(call Package/iproute2/Default)
  TITLE:=General netlink utility frontend
endef

define Build/Configure
	$(SED) "s,-I/usr/include/db3,," $(PKG_BUILD_DIR)/Makefile
	$(SED) "s,^KERNEL_INCLUDE.*,KERNEL_INCLUDE=$(LINUX_DIR)/include," \
		$(PKG_BUILD_DIR)/Makefile
	$(SED) "s,^LIBC_INCLUDE.*,LIBC_INCLUDE=$(STAGING_DIR)/include," \
		$(PKG_BUILD_DIR)/Makefile
	# For now disable compiling of the misc directory because it seems to fail
	rm -rf $(PKG_BUILD_DIR)/misc 
	$(SED) "s, misc,," $(PKG_BUILD_DIR)/Makefile
endef

MAKE_FLAGS += \
	EXTRA_CCOPTS="$(TARGET_CFLAGS)" \
	KERNEL_INCLUDE="$(LINUX_DIR)/include" \
	FPIC="$(FPIC)" \
	all tc/tc ip/ip

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/netem HOSTCC="$(HOSTCC)" EXTRA_CCOPTS="$(TARGET_CFLAGS)" CFLAGS="-D_GNU_SOURCE -O2 -Wstrict-prototypes -Wall -I ../include -DRESOLVE_HOSTNAMES"
	$(Build/Compile/Default)
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

define Package/tc/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tc/tc $(1)/usr/sbin/
endef

define Package/genl/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/genl/genl $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,ip))
$(eval $(call BuildPackage,tc))
$(eval $(call BuildPackage,genl))


