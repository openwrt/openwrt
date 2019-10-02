include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-openclash
PKG_VERSION:=0.34.6
PKG_RELEASE:=beta
PKG_MAINTAINER:=vernesong <https://github.com/vernesong/OpenClash>

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	CATEGORY:=LuCI
	SUBMENU:=3. Applications
	TITLE:=LuCI support for clash
	PKGARCH:=all
	DEPENDS:=+iptables +dnsmasq-full +coreutils +coreutils-nohup +bash +wget
	MAINTAINER:=vernesong
endef

define Package/$(PKG_NAME)/description
    A LuCI support for clash
endef

define Package/$(PKG_NAME)/postinst
endef

define Build/Prepare
	$(foreach po,$(wildcard ${CURDIR}/i18n/zh-cn/*.po), \
		po2lmo $(po) $(PKG_BUILD_DIR)/$(patsubst %.po,%.lmo,$(notdir $(po)));)
	chmod 0755 ${CURDIR}/files/etc/init.d/openclash
	chmod -R 0755 ${CURDIR}/files/usr/share/openclash/
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/$(PKG_NAME)/preinst
#!/bin/sh
if [ -f "/etc/config/openclash" ]; then
	cp "/etc/config/openclash" "/tmp/openclash.bak" >/dev/null 2>&1
	cp "/etc/config/openclash_custom_rules.list" "/tmp/openclash_custom_rules.list.bak" >/dev/null 2>&1
	cp "/etc/config/openclash_custom_hosts.list" "/tmp/openclash_custom_hosts.list.bak" >/dev/null 2>&1
	cp "/etc/config/openclash_custom_fake_black.conf" "/tmp/openclash_custom_fake_black.conf.bak" >/dev/null 2>&1
fi
endef

define Package/$(PKG_NAME)/postinst
#!/bin/sh
uci delete ucitrack.@openclash[-1] >/dev/null 2>&1
uci add ucitrack openclash >/dev/null 2>&1
uci set ucitrack.@openclash[-1].init=openclash >/dev/null 2>&1
uci commit ucitrack >/dev/null 2>&1
uci delete firewall.openclash >/dev/null 2>&1
uci set firewall.openclash=include >/dev/null 2>&1
uci set firewall.openclash.type=script >/dev/null 2>&1
uci set firewall.openclash.path=/var/etc/openclash.include >/dev/null 2>&1
uci set firewall.openclash.reload=1 >/dev/null 2>&1
uci commit firewall >/dev/null 2>&1
if [ -f "/tmp/openclash.bak" ]; then
	mv "/tmp/openclash.bak" "/etc/config/openclash" >/dev/null 2>&1
	mv "/tmp/openclash_custom_rules.list.bak" "/etc/config/openclash_custom_rules.list" >/dev/null 2>&1
	mv "/tmp/openclash_custom_hosts.list.bak" "/etc/config/openclash_custom_hosts.list" >/dev/null 2>&1
	mv "/tmp/openclash_custom_fake_black.conf.bak" "/etc/config/openclash_custom_fake_black.conf" >/dev/null 2>&1
fi
if [ -f "/tmp/config.yaml" ]; then
	mv "/tmp/config.yaml" "/etc/openclash/config.yaml" >/dev/null 2>&1
elif [ -f "/tmp/config.yml" ]; then
	mv "/tmp/config.yml" "/etc/openclash/config.yaml" >/dev/null 2>&1
fi
uci set openclash.config.enable=0 >/dev/null 2>&1
uci commit openclash >/dev/null 2>&1
chmod 0755 /etc/init.d/openclash >/dev/null 2>&1
chmod -R 0755 /usr/share/openclash/ >/dev/null 2>&1
rm -rf /tmp/luci*
endef

define Package/$(PKG_NAME)/prerm
#!/bin/sh
if [ ! -f "/tmp/openclash_update.sh" ]; then
	rm -rf /etc/openclash/clash >/dev/null 2>&1
fi
	cp "/etc/config/openclash" "/tmp/openclash.bak" >/dev/null 2>&1
	cp "/etc/config/openclash_custom_rules.list" "/tmp/openclash_custom_rules.list.bak" >/dev/null 2>&1
	cp "/etc/config/openclash_custom_hosts.list" "/tmp/openclash_custom_hosts.list.bak" >/dev/null 2>&1
	cp "/etc/config/openclash_custom_fake_black.conf" "/tmp/openclash_custom_fake_black.conf.bak" >/dev/null 2>&1
endef

define Package/$(PKG_NAME)/postrm
#!/bin/sh
if [ -f "/etc/openclash/config.yaml" ]; then
	mv "/etc/openclash/config.yaml" "/tmp/config.yaml"
fi
if [ -f "/etc/openclash/clash" ]; then
	rm -rf /etc/openclash/config.* >/dev/null 2>&1
else
	rm -rf /etc/openclash >/dev/null 2>&1
fi
	rm -rf /tmp/openclash.log >/dev/null 2>&1
	rm -rf /tmp/openclash_start.log >/dev/null 2>&1
	rm -rf /tmp/Proxy_Group >/dev/null 2>&1
	rm -rf /tmp/openclash_last_version >/dev/null 2>&1
	rm -rf /tmp/clash_last_version >/dev/null 2>&1
	rm -rf /etc/openclash/dnsmasq_fake_block.conf >/dev/null 2>&1
	uci delete firewall.openclash >/dev/null 2>&1
	uci commit firewall >/dev/null 2>&1
	uci delete ucitrack.@openclash[-1] >/dev/null 2>&1
	uci commit ucitrack >/dev/null 2>&1
	rm -rf /tmp/luci*
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/i18n
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/*.*.lmo $(1)/usr/lib/lua/luci/i18n/
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
