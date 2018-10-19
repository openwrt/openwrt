#
# Copyright (C) 2018 TDT AG <development@tdt.de>
#
# This is free software, licensed under the GNU General Public License v2.
# See https://www.gnu.org/licenses/gpl-2.0.txt for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=iptunnel
PKG_VERSION:=1
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/iptunnel/Default
endef

define Package/iptunnel
  SECTION:=net
  CATEGORY:=Network
  MAINTAINER:=Florian Eckert <fe@dev.tdt.de>
  TITLE:=Protocol wrapper for ip tunnel protocols (gre)
  DEPENDS:=+gre
endef

define Package/iptunnel/description
 Protocol wrapper for ip tunnel protocols.
endef

define Build/Compile
endef

define Build/Configure
endef

define Package/iptunnel/install
	$(INSTALL_DIR) $(1)/lib/netifd/proto
	$(INSTALL_BIN) ./files/lib/netifd/proto/iptunnel.sh \
		$(1)/lib/netifd/proto/iptunnel.sh
endef

$(eval $(call BuildPackage,iptunnel))
