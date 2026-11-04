# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=xpon-agent
PKG_VERSION:=1.0.0
PKG_RELEASE:=1
PKG_MAINTAINER:=Matheus Sampaio Queiroga <srherobrine20@gmail.com>
PKG_LICENSE:=GPL-2.0-only

include $(INCLUDE_DIR)/package.mk

define Package/xpon-agent
  SECTION:=utils
  CATEGORY:=Utilities
  SUBMENU:=Airoha
  TITLE:=Native setup and observation for Linux xPON
  DEPENDS:=+ucode +libucode +ucode-mod-ubus +ucode-mod-uci +ucode-mod-uloop +rpcd +rpcd-mod-ucode
endef

define Package/xpon-agent/description
 Native xPON and OMCI ucode bindings, xpon.agent over ubus and xponctl.
 The kernel owns OMCI and Ethernet OAM protocol processing. Userspace
 applies local configuration and reads runtime state without helper processes.
endef

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/. $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_CPPFLAGS) \
		-fPIC -Wall -Wextra -Werror \
		-I$(STAGING_DIR)/usr/include -I$(LINUX_DIR)/include \
		-o $(PKG_BUILD_DIR)/xpon_omci.so $(PKG_BUILD_DIR)/omci_ucode.c \
		-shared $(TARGET_LDFLAGS) -lucode
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_CPPFLAGS) \
		-fPIC -Wall -Wextra -Werror \
		-I$(STAGING_DIR)/usr/include -I$(LINUX_DIR)/include \
		-o $(PKG_BUILD_DIR)/xpon.so $(PKG_BUILD_DIR)/xpon_ucode.c \
		-shared $(TARGET_LDFLAGS) -lucode
endef

define Package/xpon-agent/install
	$(INSTALL_DIR) $(1)/usr/lib/ucode
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/xpon_omci.so $(PKG_BUILD_DIR)/xpon.so $(1)/usr/lib/ucode/
	$(INSTALL_DIR) $(1)/usr/share/rpcd/ucode
	$(INSTALL_BIN) ./files/usr/share/rpcd/ucode/xpon.agent $(1)/usr/share/rpcd/ucode/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) ./files/usr/sbin/xponctl $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/share/xpon $(1)/etc/uci-defaults
	$(INSTALL_CONF) ./files/usr/share/xpon/xpon.config $(1)/usr/share/xpon/
	$(INSTALL_DIR) $(1)/lib/upgrade/keep.d
	echo /etc/config/xpon > $(1)/lib/upgrade/keep.d/xpon-agent
	$(INSTALL_BIN) ./files/etc/uci-defaults/90-xpon-config $(1)/etc/uci-defaults/
endef

define Package/xpon-agent/conffiles
/etc/config/xpon
endef

define Package/xpon-agent/postinst
#!/bin/sh
if [ -z "$${IPKG_INSTROOT}" ]; then
	/etc/uci-defaults/90-xpon-config && rm -f /etc/uci-defaults/90-xpon-config
	/etc/init.d/rpcd reload >/dev/null 2>&1 || true
fi
endef

$(eval $(call BuildPackage,xpon-agent))
