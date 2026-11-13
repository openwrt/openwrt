include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=gpio-tone-beeper
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0-or-later
PKG_LICENSE_FILES:=
PKG_MAINTAINER:=Santiago Hormazabal <santiagohssl@gmail.com>

include $(INCLUDE_DIR)/package.mk

define KernelPackage/gpio-tone-beeper
  SUBMENU:=GPIO support
  TITLE:=GPIO tone beeper driver
  DEPENDS:=+kmod-input-core
  FILES:=$(PKG_BUILD_DIR)/gpio-tone-beeper.ko
  AUTOLOAD:=$(call AutoLoad,50,gpio-tone-beeper)
  KCONFIG:=
endef

define KernelPackage/gpio-tone-beeper/description
 Plays tones on a passive piezo buzzer by toggling a GPIO from a
 high-resolution timer. Unlike pwm-beeper, it needs no PWM support in
 the kernel.
endef

define Build/Compile
	$(KERNEL_MAKE) M="$(PKG_BUILD_DIR)" modules
endef

$(eval $(call KernelPackage,gpio-tone-beeper))
