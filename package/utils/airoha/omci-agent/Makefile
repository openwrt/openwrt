# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=omci-agent
PKG_VERSION:=0.6.1
PKG_RELEASE:=1

PKG_MAINTAINER:=Matheus Sampaio Queiroga <srherobrine20@gmail.com>
PKG_LICENSE:=GPL-2.0-only

include $(INCLUDE_DIR)/package.mk

define Package/omci-agent
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Routing and Redirection
  TITLE:=OMCI in-kernel agent RPC service
  DEPENDS:=@TARGET_airoha +ucode +ucode-mod-fs +ucode-mod-ubus \
	+ucode-mod-uci +rpcd-mod-ucode
endef

define Package/omci-agent/description
  Provides a ucode RPC service, an RPC client CLI and OpenWrt integration for
  the Linux in-kernel OMCI agent used by Airoha GPON devices. A private native
  helper transports validated requests over Generic Netlink.
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	$(CP) ./src/. $(PKG_BUILD_DIR)/
endef

define Build/Configure
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CPPFLAGS) $(TARGET_CFLAGS) \
		-I$(PKG_BUILD_DIR)/include -Wall -Wextra -Werror -std=c11 \
		-o $(PKG_BUILD_DIR)/omci-netlink $(PKG_BUILD_DIR)/omcictl.c \
		$(TARGET_LDFLAGS)
endef

define Package/omci-agent/conffiles
/etc/config/omci
endef

define Package/omci-agent/postinst
#!/bin/sh
[ -n "$${IPKG_INSTROOT}" ] || {
	/etc/init.d/rpcd reload 2>/dev/null || true
	/etc/init.d/omci-agent reload 2>/dev/null || true
}
endef

define Package/omci-agent/prerm
#!/bin/sh
[ -n "$${IPKG_INSTROOT}" ] || {
	rm -f /var/run/omci-agent/available
	rm -f /tmp/luci-indexcache.*.json
}
endef

define Package/omci-agent/install
	$(INSTALL_DIR) $(1)/usr/libexec
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/omci-netlink $(1)/usr/libexec/omci-netlink
	$(INSTALL_BIN) ./files/usr/libexec/omci-agent-availability $(1)/usr/libexec/omci-agent-availability
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) ./files/usr/sbin/omcictl $(1)/usr/sbin/omcictl
	$(INSTALL_DIR) $(1)/usr/share/rpcd/ucode
	$(INSTALL_BIN) ./files/usr/share/rpcd/ucode/omci.agent $(1)/usr/share/rpcd/ucode/omci.agent
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/etc/init.d/omci-agent $(1)/etc/init.d/omci-agent
	$(INSTALL_DIR) $(1)/etc/hotplug.d/platform
	$(INSTALL_BIN) ./files/etc/hotplug.d/platform/50-omci-agent $(1)/etc/hotplug.d/platform/50-omci-agent
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/etc/config/omci $(1)/etc/config/omci
endef

$(eval $(call BuildPackage,omci-agent))
