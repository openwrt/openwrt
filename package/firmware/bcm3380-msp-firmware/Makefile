# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=bcm3380-msp-firmware
PKG_RELEASE:=1
PKG_FLAGS:=nonshared
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

BCM3380_MSP_FW_SOURCE := $(CURDIR)/bcm3380-msp-fw.S
BCM3380_MSP_FW_SRC := $(PKG_BUILD_DIR)/bcm3380-msp-fw.S
BCM3380_MSP_FW_BIN := $(PKG_BUILD_DIR)/bcm3380-msp-fw.bin
BCM3380_MSP_FW_OBJ := $(PKG_BUILD_DIR)/bcm3380-msp-fw.o

define Package/bcm3380-msp-firmware
  SECTION:=firmware
  CATEGORY:=Firmware
  DEPENDS:=@TARGET_bmips_bcm3380
  TITLE:=BCM3380 MSP 4KE firmware
endef

define Package/bcm3380-msp-firmware/description
  Raw 4KE code loaded by the BCM3380 MSP driver through request_firmware().
endef

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) $(BCM3380_MSP_FW_SOURCE) $(BCM3380_MSP_FW_SRC)
endef

define Build/Compile
	$(TARGET_CROSS)as -EB -march=4kc -o $(BCM3380_MSP_FW_OBJ) $(BCM3380_MSP_FW_SRC)
	$(TARGET_CROSS)objcopy -O binary -j .text $(BCM3380_MSP_FW_OBJ) $(BCM3380_MSP_FW_BIN)
endef

define Package/bcm3380-msp-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(BCM3380_MSP_FW_BIN) $(1)/lib/firmware/
endef

$(eval $(call BuildPackage,bcm3380-msp-firmware))
