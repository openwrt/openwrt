# SPDX-License-Identifier: GPL-2.0-or-later

include $(TOPDIR)/rules.mk

PKG_NAME:=bcmplcd
PKG_RELEASE:=1

PKG_LICENSE:=GPL-2.0-or-later
PKG_LICENSE_FILES:=
PKG_MAINTAINER:=Marco Martins <marcomartins86@gmail.com>

include $(INCLUDE_DIR)/package.mk

define Package/bcmplcd
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libubox
  TITLE:=Host daemon and control tool for Broadcom powerline chips
endef

define Package/bcmplcd/description
  Broadcom BCM60xxx HomePlug AV2 chips that have no flash of their own ask
  the host for their firmware after a reset, and keep their parameters on
  the host. This daemon answers both, from the flash partitions that the
  stock firmware of the device uses for it. bcmplc talks to the running
  chip: pairing, the stations of the powerline network, the network key,
  the LEDs of the adapters, the Ethernet socket on the chip.
endef

define Package/bcmplcd/conffiles
/etc/bcmplcd/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_CPPFLAGS) -Wall -Wextra \
		-o $(PKG_BUILD_DIR)/bcmplcd $(PKG_BUILD_DIR)/bcmplcd.c \
		$(PKG_BUILD_DIR)/common.c $(TARGET_LDFLAGS) -lubox
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_CPPFLAGS) -Wall -Wextra \
		-o $(PKG_BUILD_DIR)/bcmplc $(PKG_BUILD_DIR)/bcmplc.c \
		$(PKG_BUILD_DIR)/common.c $(TARGET_LDFLAGS)
endef

define Package/bcmplcd/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bcmplcd $(PKG_BUILD_DIR)/bcmplc $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/bcmplcd.init $(1)/etc/init.d/bcmplcd
	$(INSTALL_DIR) $(1)/etc/rc.button
	$(INSTALL_BIN) ./files/bcmplcd.button $(1)/etc/rc.button/BTN_1
	$(INSTALL_DIR) $(1)/lib/preinit
	$(INSTALL_DATA) ./files/bcmplcd.failsafe $(1)/lib/preinit/99_05_failsafe_bcmplcd
endef

$(eval $(call BuildPackage,bcmplcd))
