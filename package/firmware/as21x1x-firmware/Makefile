# SPDX-License-Identifier: GPL-2.0-or-later

include $(TOPDIR)/rules.mk

PKG_NAME:=as21x1x-firmware
PKG_VERSION:=1.8.2
PKG_RELEASE:=1

PKG_LICENSE_FILES:=LICENSE.aeonsemi

PKG_MAINTAINER:=Christian Marangi <ansuelsmth@gmail.com>

include $(INCLUDE_DIR)/package.mk

define Package/as21x1x-firmware
  TITLE:=AS21x1x Ethrnet PHY firmware
  SECTION:=firmware
  CATEGORY:=Firmware
endef

define Build/Compile

endef

define Package/as21x1x-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) \
		./files/as21x1x_fw.bin \
		$(1)/lib/firmware/
endef

$(eval $(call BuildPackage,as21x1x-firmware))
