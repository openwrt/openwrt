#
# Copyright (C) 2006-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libpcap
PKG_VERSION:=1.9.1
PKG_RELEASE:=3.1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.us.tcpdump.org/release/ \
        http://www.tcpdump.org/release/
PKG_HASH:=635237637c5b619bcceba91900666b64d56ecb7be63f298f601ec786ce087094

PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>

PKG_ASLR_PIE_REGULAR:=1
PKG_BUILD_PARALLEL:=1

PKG_LICENSE:=BSD-3-Clause
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/libpcap
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Low-level packet capture library
  URL:=http://www.tcpdump.org/
  MENU:=1
  ABI_VERSION:=1
endef

define Package/libpcap/description
This package contains a system-independent library for user-level network packet
capture.
endef

define Package/libpcap/config
	source "$(SOURCE)/Config.in"
endef

define Package/rpcapd
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Capture daemon to be controlled by a remote libpcap application
  URL:=http://www.tcpdump.org/
  DEPENDS+= +libpcap
endef

ifdef CONFIG_PACKAGE_rpcapd
	CMAKE_OPTIONS += \
		-DENABLE_REMOTE=ON
endif

CMAKE_OPTIONS += \
	-DBUILD_SHARED_LIBS=ON \
	-DBUILD_WITH_LIBNL=OFF \

# grep 'option(DISABLE_' CMakeLists.txt | cut -f2 -d'(' | cut -f1 -d' ' | sort --unique
CMAKE_OPTIONS += \
	-DDISABLE_DAG=ON \
	-DDISABLE_DBUS=ON \
	-DDISABLE_NETMAP=ON \
	-DDISABLE_RDMA=ON \
	-DDISABLE_SEPTEL=ON \
	-DDISABLE_SNF=ON \
	-DDISABLE_TC=ON \

# Debugging options
CMAKE_OPTIONS += \
	-DBDEBUG=OFF \
	-DYYDEBUG=OFF \

CMAKE_OPTIONS += $(if $(CONFIG_PCAP_HAS_USB)       ,,-DDISABLE_USB=ON)
CMAKE_OPTIONS += $(if $(CONFIG_PCAP_HAS_BT)        ,,-DDISABLE_BLUETOOTH=ON)
CMAKE_OPTIONS += $(if $(CONFIG_PCAP_HAS_NETFILTER) ,,-DPCAP_SUPPORT_NETFILTER=OFF)

CMAKE_OPTIONS += $(if $(CONFIG_IPV6),-DINET6=ON,-DINET6=OFF)

define Build/InstallDev
	$(call Build/InstallDev/cmake,$(1))
	$(SED) \
		's,^\(prefix\|exec_prefix\)=.*,\1=$(STAGING_DIR)/usr,g' \
		$(1)/usr/bin/pcap-config
	$(INSTALL_DIR) $(2)/bin
	$(LN) ../../usr/bin/pcap-config $(2)/bin/pcap-config
endef

define Package/libpcap/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libpcap.so.* $(1)/usr/lib/
endef

define Package/rpcapd/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/rpcapd $(1)/usr/bin/
endef

$(eval $(call BuildPackage,libpcap))
$(eval $(call BuildPackage,rpcapd))
