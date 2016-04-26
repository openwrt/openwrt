#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=vsc73x5-ucode
PKG_RELEASE:=1

PKG_SOURCE_URL:=http://mirror2.openwrt.org/sources

PKG_SOURCE:=vsc73x5-ucode.tar.bz2
PKG_BUILD_DIR:=$(BUILD_DIR)/vsc73x5-ucode

PKG_MD5SUM:=b32e3debcd118f263c79199a7b5afa68

PKG_FLAGS:=nonshared

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
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(2) $(1)/lib/firmware/$(3)
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

define Build/Compile

endef

$(eval $(call BuildPackage,vsc7385-ucode-ap83))
$(eval $(call BuildPackage,vsc7395-ucode-ap83))
$(eval $(call BuildPackage,vsc7385-ucode-pb44))
$(eval $(call BuildPackage,vsc7395-ucode-pb44))
