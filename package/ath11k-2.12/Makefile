#
# Copyright (C) 2022 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ath11k-extfw
PKG_SOURCE_DATE:=2024-11-11
PKG_RELEASE:=2.12


include $(INCLUDE_DIR)/package.mk

RSTRIP:=:
STRIP:=:

define Package/ath11k-extfw-default
  SECTION:=firmware
  CATEGORY:=Firmware
  DEPENDS:=
endef

define Package/ath11k-extfw-ipq6018-212-01460
$(Package/ath11k-extfw-default)
  TITLE:=IPQ6018 ath11k firmware 2.12-01460
endef

define Package/ath11k-extfw-ipq9074-212-01460
$(Package/ath11k-extfw-default)
	TITLE:=QCN9074 ath11k firmware 2.12-01460
endef

define Build/Compile

endef

define Package/ath11k-extfw-ipq6018-212-01460/install
	$(INSTALL_DIR) $(1)/lib/firmware/IPQ6018/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/IPQ6018/hw1.0/* \
		$(1)/lib/firmware/IPQ6018/
endef

define Package/ath11k-extfw-ipq9074-212-01460/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath11k/QCN9074/hw1.0/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/QCN9074/hw1.0/* \
		$(1)/lib/firmware/ath11k/QCN9074/hw1.0/
endef


$(eval $(call BuildPackage,ath11k-extfw-ipq6018-212-01460))
$(eval $(call BuildPackage,ath11k-extfw-ipq9074-212-01460))

