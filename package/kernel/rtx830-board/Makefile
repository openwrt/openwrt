include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=rtx830-board
PKG_VERSION:=1
PKG_RELEASE:=1
PKG_MAINTAINER:=Kosuke Shibuya <kakiringopcneko@gmail.com>
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=COPYING
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define KernelPackage/rtx830-board
  SUBMENU:=Other modules
  TITLE:=Yamaha RTX830 board controls and restart
  DEPENDS:=@TARGET_mvebu_cortexa9
  KCONFIG:= \
	CONFIG_LEDS_CLASS=y \
	CONFIG_SPI=y \
	CONFIG_GPIOLIB=y
  FILES:=$(PKG_BUILD_DIR)/rtx830_board.ko $(PKG_BUILD_DIR)/rtx830_restart.ko
  AUTOLOAD:=$(call AutoLoad,70,rtx830_board rtx830_restart,1)
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	$(CP) ./src/. $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(KERNEL_MAKE) M="$(PKG_BUILD_DIR)" modules
endef

define KernelPackage/rtx830-board/install
	$(INSTALL_DIR) $(1)/etc/init.d $(1)/usr/sbin
	$(INSTALL_BIN) ./files/rtx830-board $(1)/etc/init.d/
	$(INSTALL_BIN) ./files/rtx830-media-led-monitor $(1)/usr/sbin/
	$(INSTALL_BIN) ./files/rtx830-beep $(1)/usr/sbin/
endef

$(eval $(call KernelPackage,rtx830-board))
