#
# Copyright (C) 2021 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ath11k-board
PKG_SOURCE_DATE:=2021-09-16
PKG_SOURCE_VERSION:=0bc83743aab26006c084963760fa7dd7232ef123
PKG_MIRROR_HASH:=c9d4e93fa0911e45b5a17516e5c4d942b7c5de8fbef0fa0ff9504cb6be58b382
PKG_RELEASE:=$(AUTORELEASE)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://source.codeaurora.org/quic/qsdk/oss/ath11k-bdf

PKG_LICENSE_FILES:=LICENSE

PKG_MAINTAINER:=Robert Marko <robimarko@gmail.com>

include $(INCLUDE_DIR)/package.mk

define Package/ath11k-board-default
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=$(PKG_SOURCE_URL)
  DEPENDS:=
endef

define Package/ath11k-board-ipq6018
$(Package/ath11k-board-default)
  TITLE:=ath11k board data for IPQ6018 devices
endef

define Package/ath11k-board-ipq8074
$(Package/ath11k-board-default)
  TITLE:=ath11k board data for IPQ8074 devices
endef

define Package/ath11k-board-qcn9074
$(Package/ath11k-board-default)
  TITLE:=ath11k board data for QCN9074 devices
endef

define Build/Compile

endef

define Package/ath11k-board-ipq6018/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath11k/IPQ6018/hw1.0
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/IPQ6018/hw1.0/WLAN.HK.2.5.0.1/WLAN.HK.2.5.0.1-01192-QCAHKSWPL_SILICONZ-1/board-2.bin \
		$(1)/lib/firmware/ath11k/IPQ6018/hw1.0/
endef

define Package/ath11k-board-ipq8074/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath11k/IPQ8074/hw2.0
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/IPQ8074/hw2.0/WLAN.HK.2.5.0.1/WLAN.HK.2.5.0.1-01192-QCAHKSWPL_SILICONZ-1/board-2.bin \
		$(1)/lib/firmware/ath11k/IPQ8074/hw2.0/
endef

define Package/ath11k-board-qcn9074/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath11k/QCN9074/hw1.0
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/QCN9074/hw1.0/WLAN.HK.2.5.0.1/WLAN.HK.2.5.0.1-01192-QCAHKSWPL_SILICONZ-1/board-2.bin \
		$(1)/lib/firmware/ath11k/QCN9074/hw1.0/
endef

$(eval $(call BuildPackage,ath11k-board-ipq6018))
$(eval $(call BuildPackage,ath11k-board-ipq8074))
$(eval $(call BuildPackage,ath11k-board-qcn9074))
