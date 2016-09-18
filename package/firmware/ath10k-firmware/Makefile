#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ath10k-firmware
PKG_SOURCE_VERSION:=307cb46b06661ebd3186723b5002de769c7add83
PKG_VERSION:=2016-09-13-$(PKG_SOURCE_VERSION)
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL:=https://github.com/kvalo/ath10k-firmware.git

PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>

include $(INCLUDE_DIR)/package.mk

WMENU:=Wireless Drivers

define Package/ath10k-firmware-default
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=$(WMENU)
  URL:=$(PKG_SOURCE_URL)
  DEPENDS:=
endef

define Package/ath10k-firmware-qca9887
$(Package/ath10k-firmware-default)
  TITLE:=ath10k firmware for QCA9887 devices
endef

QCA9887_REV:=3cce88e245f2d685e49411c4f80998f94baf67b8
QCA9887_FIRMWARE_FILE:=firmware-5.bin_10.2.4-1.0-00013
QCA9887_FIRMWARE_FILE_MD5:=bd9cdcbf49561c7176432a81c29e7e87
QCA9887_FIRMWARE_FILE_DL:=$(QCA9887_FIRMWARE_FILE).$(QCA9887_FIRMWARE_FILE_MD5)
QCA9887_BOARD_FILE:=board.bin
QCA9887_BOARD_FILE_MD5:=ebf3af10160c45373f19e0b8226b02ae
QCA9887_BOARD_FILE_DL:=$(QCA9887_BOARD_FILE).$(QCA9887_BOARD_FILE_MD5)

define Download/ath10k-qca9887-firmware
  URL:=https://github.com/kvalo/ath10k-firmware/raw/$(QCA9887_REV)/QCA9887/hw1.0/
  URL_FILE:=$(QCA9887_FIRMWARE_FILE)
  FILE:=$(QCA9887_FIRMWARE_FILE_DL)
  MD5SUM:=$(QCA9887_FIRMWARE_FILE_MD5)
endef
$(eval $(call Download,ath10k-qca9887-firmware))

define Download/ath10k-qca9887-board
  URL:=https://github.com/kvalo/ath10k-firmware/raw/$(QCA9887_REV)/QCA9887/hw1.0/
  URL_FILE:=$(QCA9887_BOARD_FILE)
  FILE:=$(QCA9887_BOARD_FILE_DL)
  MD5SUM:=$(QCA9887_BOARD_FILE_MD5)
endef
$(eval $(call Download,ath10k-qca9887-board))

define Package/ath10k-firmware-qca988x
$(Package/ath10k-firmware-default)
  DEFAULT:=PACKAGE_kmod-ath10k
  TITLE:=ath10k firmware for QCA988x devices
endef

QCA988X_FIRMWARE_FILE_CT:=firmware-2-ct-full-community-16.1.bin-lede
QCA99X0_FIRMWARE_FILE_CT:=firmware-5-ct-full-community-7.bin-lede.004
QCA9984_FIRMWARE_FILE_CT:=firmware-5-ct-full-community-7.bin-lede.004

define Download/ath10k-firmware-qca988x-ct
  URL:=https://www.candelatech.com/downloads/
  FILE:=$(QCA988X_FIRMWARE_FILE_CT)
  MD5SUM:=d7e081e9782936ed544b78994c9133fb
endef
$(eval $(call Download,ath10k-firmware-qca988x-ct))

define Download/ath10k-firmware-qca99x0-ct
  URL:=https://www.candelatech.com/downloads/ath10k-10-4/
  FILE:=$(QCA99X0_FIRMWARE_FILE_CT)
  MD5SUM:=809bb9bf8a18ea218a8e1b9ffc0f8447
endef
$(eval $(call Download,ath10k-firmware-qca99x0-ct))

define Download/ath10k-firmware-qca9984-ct
  URL:=https://www.candelatech.com/downloads/ath10k-9984-10-4/
  FILE:=$(QCA9984_FIRMWARE_FILE_CT)
  MD5SUM:=924eb8ea30de11299b13e207469a3350
endef
$(eval $(call Download,ath10k-firmware-qca9984-ct))

define Package/ath10k-firmware-qca99x0
$(Package/ath10k-firmware-default)
  TITLE:=ath10k firmware for QCA99x0 devices
endef

define Package/ath10k-firmware-qca988x-ct
$(Package/ath10k-firmware-default)
  TITLE:=ath10k CT 10.1 firmware for QCA988x devices
endef

define Package/ath10k-firmware-qca988x-ct/description
Alternative ath10k firmware for QCA988X from Candela Technologies.
Enables IBSS and other features.  See:
http://www.candelatech.com/ath10k-10.1.php
This firmware will NOT be used unless the standard ath10k-firmware-qca988x
is un-selected since the driver will try to load firmware-5.bin before
firmware-2.bin
endef

define Package/ath10k-firmware-qca99x0-ct/description
Alternative ath10k firmware for QCA99x0 from Candela Technologies.
Enables IBSS and other features.  See:
http://www.candelatech.com/ath10k-10.4.php
This firmware conflicts with the standard 99x0 firmware, so select only
one.
endef

define Package/ath10k-firmware-qca9984-ct/description
Alternative ath10k firmware for QCA9984 from Candela Technologies.
Enables IBSS and other features.  See:
http://www.candelatech.com/ath10k-10.4-9984.php
This firmware conflicts with the standard 9984 firmware, so select only
one.
endef

define Package/ath10k-firmware-qca99x0/description
Standard ath10k firmware for QCA99x0 from QCA
This firmware conflicts with the CT 99x0 firmware, so select only
one.
endef

define Package/ath10k-firmware-qca99x0-ct
$(Package/ath10k-firmware-default)
  TITLE:=ath10k CT 10.4.3 firmware for QCA99x0 devices
endef

define Package/ath10k-firmware-qca9984-ct
$(Package/ath10k-firmware-default)
  TITLE:=ath10k CT 10.4.3 firmware for QCA9984 devices
endef

define Package/ath10k-firmware-qca9984
$(Package/ath10k-firmware-default)
  TITLE:=ath10k firmware for QCA9984 devices
endef

define Package/ath10k-firmware-qca6174
$(Package/ath10k-firmware-default)
  TITLE:=ath10k firmware for QCA6174 devices
endef

QCA99X0_BOARD_REV:=ddcec9efd245da9365c474f513a855a55f3ac7fe
QCA99X0_BOARD_FILE:=board-2.bin.$(QCA99X0_BOARD_REV)

define Download/qca99x0-board
  URL:=https://source.codeaurora.org/quic/qsdk/oss/firmware/ath10k-firmware/plain/ath10k/QCA99X0/hw2.0
  URL_FILE:=board-2.bin?id=$(QCA99X0_BOARD_REV)
  FILE:=$(QCA99X0_BOARD_FILE)
  MD5SUM:=a2b3c653c2363a5641200051d6333d0a
endef
$(eval $(call Download,qca99x0-board))

define Build/Compile

endef

define Package/ath10k-firmware-qca9887/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath10k/QCA9887/hw1.0
	$(INSTALL_DATA) \
		$(DL_DIR)/$(QCA9887_FIRMWARE_FILE_DL) \
		$(1)/lib/firmware/ath10k/QCA9887/hw1.0/firmware-5.bin
	$(INSTALL_DATA) \
		$(DL_DIR)/$(QCA9887_BOARD_FILE_DL) \
		$(1)/lib/firmware/ath10k/QCA9887/hw1.0/board.bin
endef

define Package/ath10k-firmware-qca988x/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath10k/QCA988X/hw2.0
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/QCA988X/hw2.0/board.bin \
		$(1)/lib/firmware/ath10k/QCA988X/hw2.0/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/QCA988X/hw2.0/10.2.4.70/firmware-5.bin_10.2.4.70.54 \
		$(1)/lib/firmware/ath10k/QCA988X/hw2.0/firmware-5.bin
endef

define Package/ath10k-firmware-qca988x-ct/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath10k/QCA988X/hw2.0
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/QCA988X/hw2.0/board.bin \
		$(1)/lib/firmware/ath10k/QCA988X/hw2.0/
	$(INSTALL_DATA) \
		$(DL_DIR)/$(QCA988X_FIRMWARE_FILE_CT) \
		$(1)/lib/firmware/ath10k/QCA988X/hw2.0/firmware-2.bin
endef

define Package/ath10k-firmware-qca6174/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath10k
	$(CP) $(PKG_BUILD_DIR)/QCA6174 $(1)/lib/firmware/ath10k/
endef

define Package/ath10k-firmware-qca99x0/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath10k/QCA99X0/hw2.0
	$(INSTALL_DATA) \
		$(DL_DIR)/$(QCA99X0_BOARD_FILE) \
		$(1)/lib/firmware/ath10k/QCA99X0/hw2.0/board-2.bin
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/QCA99X0/hw2.0/boardData_AR900B_CUS239_5G_v2_001.bin \
		$(1)/lib/firmware/ath10k/QCA99X0/hw2.0/board.bin
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/QCA99X0/hw2.0/firmware-5.bin_10.4.1.00030-1 \
		$(1)/lib/firmware/ath10k/QCA99X0/hw2.0/firmware-5.bin
endef

define Package/ath10k-firmware-qca99x0-ct/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath10k/QCA99X0/hw2.0
	$(INSTALL_DATA) \
		$(DL_DIR)/$(QCA99X0_BOARD_FILE) \
		$(1)/lib/firmware/ath10k/QCA99X0/hw2.0/board-2.bin
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/QCA99X0/hw2.0/boardData_AR900B_CUS239_5G_v2_001.bin \
		$(1)/lib/firmware/ath10k/QCA99X0/hw2.0/board.bin
	$(INSTALL_DATA) \
		$(DL_DIR)/$(QCA99X0_FIRMWARE_FILE_CT) \
		$(1)/lib/firmware/ath10k/QCA99X0/hw2.0/firmware-5.bin
endef

define Package/ath10k-firmware-qca9984/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath10k/QCA9984/hw1.0
	ln -s \
		../../cal-pci-0000:01:00.0.bin \
		$(1)/lib/firmware/ath10k/QCA9984/hw1.0/board.bin
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/QCA9984/hw1.0/board-2.bin \
		$(1)/lib/firmware/ath10k/QCA9984/hw1.0/board-2.bin
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/QCA9984/hw1.0/firmware-5.bin_10.4-3.2-00072 \
		$(1)/lib/firmware/ath10k/QCA9984/hw1.0/firmware-5.bin
endef

define Package/ath10k-firmware-qca9984-ct/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath10k/QCA9984/hw1.0
	ln -s \
		../../cal-pci-0000:01:00.0.bin \
		$(1)/lib/firmware/ath10k/QCA9984/hw1.0/board.bin
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/QCA9984/hw1.0/board-2.bin \
		$(1)/lib/firmware/ath10k/QCA9984/hw1.0/board-2.bin
	$(INSTALL_DATA) \
		$(DL_DIR)/$(QCA9984_FIRMWARE_FILE_CT) \
		$(1)/lib/firmware/ath10k/QCA9984/hw1.0/firmware-5.bin
endef

$(eval $(call BuildPackage,ath10k-firmware-qca9887))
$(eval $(call BuildPackage,ath10k-firmware-qca988x))
$(eval $(call BuildPackage,ath10k-firmware-qca99x0))
$(eval $(call BuildPackage,ath10k-firmware-qca6174))
$(eval $(call BuildPackage,ath10k-firmware-qca9984))

$(eval $(call BuildPackage,ath10k-firmware-qca988x-ct))
$(eval $(call BuildPackage,ath10k-firmware-qca99x0-ct))
$(eval $(call BuildPackage,ath10k-firmware-qca9984-ct))
