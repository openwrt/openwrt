# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=bcm3380-msp-firmware
PKG_RELEASE:=1
PKG_FLAGS:=nonshared
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

BCM3380_MSP_FW_SRC := $(PKG_BUILD_DIR)/bcm3380-msp-fw.c
BCM3380_MSP_FW_LDS := $(PKG_BUILD_DIR)/bcm3380-msp-fw.lds
BCM3380_MSP_FW_BIN := $(PKG_BUILD_DIR)/bcm3380-msp-fw.bin
BCM3380_MSP_FW_ELF := $(PKG_BUILD_DIR)/bcm3380-msp-fw.elf
BCM3380_MSP_FW_OBJ := $(PKG_BUILD_DIR)/bcm3380-msp-fw.o

BCM3380_MSP_FW_CFLAGS := \
	-std=gnu11 -Os -Wall -Wextra -ffreestanding -fno-builtin \
	-fno-stack-protector -fno-jump-tables -fno-pic -mno-abicalls \
	-G 0 -EB -march=4kc -I$(PKG_BUILD_DIR)

BCM3380_MSP_FW_LDFLAGS := \
	-nostdlib -nostartfiles -mno-abicalls -G 0 -EB -march=4kc \
	-Wl,--build-id=none -Wl,-T,$(BCM3380_MSP_FW_LDS)

define Package/bcm3380-msp-firmware
  SECTION:=firmware
  CATEGORY:=Firmware
  DEPENDS:=@(TARGET_bmips_bcm3380||TARGET_bmips_bcm3383)
  TITLE:=BCM3380 MSP 4KE firmware
endef

define Package/bcm3380-msp-firmware/description
  Raw 4KE code loaded by the BCM3380 MSP driver through request_firmware().
endef

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) $(CURDIR)/bcm3380-msp-fw.c $(PKG_BUILD_DIR)
	$(CP) $(TOPDIR)/target/linux/bmips/files/include/soc/bcm/bcm3380-msp-fw.h $(PKG_BUILD_DIR)
	$(CP) $(CURDIR)/bcm3380-msp-fw.lds $(PKG_BUILD_DIR)
endef

define Build/Compile
	$(TARGET_CC) $(BCM3380_MSP_FW_CFLAGS) -c -o $(BCM3380_MSP_FW_OBJ) $(BCM3380_MSP_FW_SRC)
	$(TARGET_CC) $(BCM3380_MSP_FW_LDFLAGS) -o $(BCM3380_MSP_FW_ELF) $(BCM3380_MSP_FW_OBJ)
	$(TARGET_CROSS)objcopy -O binary $(BCM3380_MSP_FW_ELF) $(BCM3380_MSP_FW_BIN)
endef

define Package/bcm3380-msp-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(BCM3380_MSP_FW_BIN) $(1)/lib/firmware/
endef

$(eval $(call BuildPackage,bcm3380-msp-firmware))
