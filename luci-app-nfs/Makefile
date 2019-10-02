#
# Copyright (C) 2018 Simon Shi <simonsmh@gmail.com>
#
# This is free software, licensed under the GNU General Public License v3.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-nfs
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

PKG_LICENSE:=GPLv3
PKG_LICENSE_FILES:=LICENSE
PKG_MAINTAINER:=Simon Shi <simonsmh@gmail.com>

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/luci-app-nfs
	SECTION:=LuCI
	CATEGORY:=LuCI
	SUBMENU:=3. Applications
	TITLE:=LuCI Support for NFS
	PKGARCH:=all
endef

define Package/luci-app-nfs/description
	LuCI Support for NFS.
endef

define Build/Prepare
	$(foreach po,$(wildcard ${CURDIR}/files/luci/i18n/*.po), \
		po2lmo $(po) $(PKG_BUILD_DIR)/$(patsubst %.po,%.lmo,$(notdir $(po)));)
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/luci-app-nfs/postinst
#!/bin/sh
if [ -z "$${IPKG_INSTROOT}" ]; then
	if [ -f /etc/uci-defaults/luci-nfs ]; then
		( . /etc/uci-defaults/luci-nfs ) && \
		rm -f /etc/uci-defaults/luci-nfs
	fi
	rm -rf /tmp/luci-indexcache /tmp/luci-modulecache
fi
exit 0
endef

define Package/luci-app-nfs/conffiles
/etc/config/nfs
endef

define Package/luci-app-nfs/postrm
#!/bin/sh
uci -q batch <<-EOF >/dev/null
	delete ucitrack.@nfs[-1]
	commit ucitrack
EOF
rm -f /tmp/luci-indexcache
rm -rf /tmp/luci-modulecache
exit 0
endef

define Package/luci-app-nfs/install
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/i18n
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/nfs.*.lmo $(1)/usr/lib/lua/luci/i18n/
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/controller
	$(INSTALL_DATA) ./files/luci/controller/*.lua $(1)/usr/lib/lua/luci/controller/
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/model/cbi
	$(INSTALL_DATA) ./files/luci/model/cbi/*.lua $(1)/usr/lib/lua/luci/model/cbi/
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_BIN) ./files/root/etc/firewall.nfs $(1)/etc/firewall.nfs
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/root/etc/config/nfs $(1)/etc/config/nfs
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/root/etc/init.d/nfs $(1)/etc/init.d/nfs
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_BIN) ./files/root/etc/uci-defaults/luci-nfs $(1)/etc/uci-defaults/luci-nfs
endef

$(eval $(call BuildPackage,luci-app-nfs))
