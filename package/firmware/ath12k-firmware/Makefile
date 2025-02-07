
include $(TOPDIR)/rules.mk

PKG_NAME:=ath12k-firmware
PKG_SOURCE_DATE:=2024-10-21
PKG_SOURCE_VERSION:=3ee26f4705a4bf8c3ee1c935f5d21bad9b6d2612
PKG_MIRROR_HASH:=148664310dace5489a43d3d9322482a4da8030e4e5ed1ffbea0bee9207dee5b4
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://git.codelinaro.org/clo/ath-firmware/ath12k-firmware.git

PKG_LICENSE_FILES:=LICENSE.qca_firmware


include $(INCLUDE_DIR)/package.mk

RSTRIP:=:
STRIP:=:

define Package/ath12k-firmware-default
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=$(PKG_SOURCE_URL)
  DEPENDS:=
endef

define Package/ath12k-firmware-qcn9274
$(Package/ath12k-firmware-default)
  TITLE:=IPQ9574 qcn9274 ath12k firmware
endef

define Build/Compile

endef

define Package/ath12k-firmware-qcn9274/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath12k/QCN9274/hw2.0/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/QCN9274/hw2.0/1.3.1/WLAN.WBE.1.3.1-00173-QCAHKSWPL_SILICONZ-1/* \
		$(1)/lib/firmware/ath12k/QCN9274/hw2.0/
	$(INSTALL_BIN) \
		$(PKG_BUILD_DIR)/QCN9274/hw2.0/board-2.bin $(1)/lib/firmware/ath12k/QCN9274/hw2.0/board-2.bin
endef

$(eval $(call BuildPackage,ath12k-firmware-qcn9274))
