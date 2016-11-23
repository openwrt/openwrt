#
# Copyright (C) 2009-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=spidev-test

include $(INCLUDE_DIR)/package.mk

define Package/spidev-test
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+kmod-spi-dev
  TITLE:=SPI testing utility
  VERSION:=$(LINUX_VERSION)
  URL:=http://www.kernel.org
endef

define Package/spidev-test/description
  SPI testing utility.
endef

ifeq ($(call kernel_patchver_ge,4.5.0),1)
  SPIDEV_TEST_C:=$(LINUX_DIR)/tools/spi/spidev_test.c
else
  SPIDEV_TEST_C:=$(LINUX_DIR)/Documentation/spi/spidev_test.c
endif

define Build/Prepare
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) -o $(PKG_BUILD_DIR)/spidev_test \
		$(SPIDEV_TEST_C)
endef

define Package/spidev-test/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/spidev_test $(1)/sbin/
endef

$(eval $(call BuildPackage,spidev-test))
