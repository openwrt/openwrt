#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=vsc73x5-ucode
PKG_RELEASE:=1

PKG_SOURCE_URL:=http://www.tp-link.com/GPL

PKG_SOURCE:=WirelessNRouer_AP.tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/vsc73x5-ucode

PKG_MD5SUM:=7dd6069a5f0e44cc9965fc7b43eb3046

UCODE_SRC_DIR:=u-boot/board/ar7100/common
UCODE_BUILD_DIR:=$(PKG_BUILD_DIR)/$(UCODE_SRC_DIR)

include $(INCLUDE_DIR)/package.mk

define Package/vsc73x5-defaults
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=@TARGET_ar71xx
  DEFAULT:=n
  TITLE:=$(1)
endef

define Package/vsc73x5/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(UCODE_BUILD_DIR)/$(2) $(1)/lib/firmware/$(3)
endef

define Package/vsc7385-ucode-ap83
  $(call Package/vsc73x5-defaults,Vitesse VSC7385 microcode for the Atheros AP83 boards)
endef

define Package/vsc7385-ucode-ap83/description
  This package contains the Atheros AP83 board specific microcode for
  the Vitesse VSC7385 ethernet switch.
endef

define Package/vsc7385-ucode-ap83/install
  $(call Package/vsc73x5/install,$(1),g5_Plus1_2_31_unmanaged_Atheros_v3.bin,vsc7385_ucode_ap83.bin)
endef

define Package/vsc7395-ucode-ap83
  $(call Package/vsc73x5-defaults, Vitesse VSC7395 microcode for the Atheros AP83 boards)
endef

define Package/vsc7395-ucode-ap83/description
  This package contains the Atheros AP83 board specific microcode for
  the Vitesse VSC7395 ethernet switch.
endef

define Package/vsc7395-ucode-ap83/install
  $(call Package/vsc73x5/install,$(1),g5_Plus1_2_31_unmanaged_Atheros_v4.bin,vsc7395_ucode_ap83.bin)
endef

define Package/vsc7385-ucode-pb44
  $(call Package/vsc73x5-defaults, Vitesse VSC7395 microcode for the Atheros PB44 boards)
endef

define Package/vsc7385-ucode-pb44/description
  This package contains the Atheros PB44 board specific microcode for
  the Vitesse VSC7385 ethernet switch.
endef

define Package/vsc7385-ucode-pb44/install
  $(call Package/vsc73x5/install,$(1),g5_Plus1_2_29b_unmanaged_Atheros_v5.bin,vsc7385_ucode_pb44.bin)
endef

define Package/vsc7395-ucode-pb44
  $(call Package/vsc73x5-defaults, Vitesse VSC7395 microcode for the Atheros PB44 boards)
endef

define Package/vsc7395-ucode-pb44/description
  This package contains the Atheros AP83 board specific microcode for
  the Vitesse VSC7395 ethernet switch.
endef

define Package/vsc7395-ucode-pb44/install
  $(call Package/vsc73x5/install,$(1),g5e_Plus1_2_29a_unmanaged_Atheros_v3.bin,vsc7395_ucode_pb44.bin)
endef

define Build/Prepare
	tar -xzf "$(DL_DIR)/$(PKG_SOURCE)" ap-9x-gpl/ap93-u-boot.tar.bz2 -O | tar -C "$(PKG_BUILD_DIR)" -xj $(UCODE_SRC_DIR)
	cp files/Makefile $(UCODE_BUILD_DIR)
endef

define Build/Compile
	$(MAKE) -C $(UCODE_BUILD_DIR) $(TARGET_CONFIGURE_OPTS) all
endef

$(eval $(call BuildPackage,vsc7385-ucode-ap83))
$(eval $(call BuildPackage,vsc7395-ucode-ap83))
$(eval $(call BuildPackage,vsc7385-ucode-pb44))
$(eval $(call BuildPackage,vsc7395-ucode-pb44))
