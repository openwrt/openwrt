#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ep80579-drivers
PKG_VERSION:=1.0.34
PKG_RELEASE:=1

PKG_SOURCE:=Embedded.L.1.0.34.ADI.R100.tar.gz
PKG_SOURCE_URL:=ftp://ftp.adiengineering.com/Archive/OcracokeIsland/Drivers/Linux/1.0.34/
PKG_MD5SUM:=61df9778f8c1f919257d2f48a0bcb000

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define KernelPackage/ep80579-drivers/Default
  DEPENDS:=@TARGET_x86_ep80579
endef

define KernelPackage/ep80579-eth
$(call KernelPackage/ep80579-drivers/Default)
  SUBMENU:=Network Devices
  TITLE:=Intel EP80579 ethernet driver
  FILES:= \
	$(PKG_BUILD_DIR)/Embedded/src/GbE/gcu.ko \
	$(PKG_BUILD_DIR)/Embedded/src/GbE/iegbe.ko
  AUTOLOAD:=$(call AutoLoad,40,gcu iegbe)
endef

define KernelPackage/ep80579-misc
$(call KernelPackage/ep80579-drivers/Default)
  SUBMENU:=Other modules
  TITLE:=Misc. Intel EP80579 drivers (DMA,, gpio)
  FILES:= \
	$(PKG_BUILD_DIR)/Embedded/src/EDMA/dma.ko \
	$(PKG_BUILD_DIR)/Embedded/src/GPIO/gpio.ko
  AUTOLOAD:=$(call AutoLoad,40,gpio dma)
endef

define KernelPackage/ep80579-can
$(call KernelPackage/ep80579-drivers/Default)
  SUBMENU:=Other modules
  TITLE:=Intel EP80579 CAN driver
  FILES:= \
	$(PKG_BUILD_DIR)/Embedded/src/1588/timesync.ko \
	$(PKG_BUILD_DIR)/Embedded/src/CAN/can.ko
  AUTOLOAD:=$(call AutoLoad,40,timesync can)
endef

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	tar xzvf $(DL_DIR)/$(PKG_SOURCE) -C $(PKG_BUILD_DIR)/
	$(Build/Patch)
endef

define Build/Compile/Subdir
	$(MAKE) -C "$(LINUX_DIR)" \
		KSRC="$(LINUX_DIR)" \
		KOBJ="$(LINUX_DIR)" \
		ENV_DIR=$(PKG_BUILD_DIR)/Embedded \
		SUBDIRS="$(PKG_BUILD_DIR)/Embedded/src/$(1)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCHIVER="$(TARGET_CROSS)ar" \
		COMPILER="$(TARGET_CC)" \
		LINKER="$(TARGET_CROSS)ld" \
		ARCH="$(LINUX_KARCH)"
endef

define Build/Compile
	$(call Build/Compile/Subdir,GbE)
	$(call Build/Compile/Subdir,CAN)
	$(call Build/Compile/Subdir,EDMA)
	$(call Build/Compile/Subdir,GPIO)
	$(call Build/Compile/Subdir,WDT)
	$(call Build/Compile/Subdir,1588)
endef

define KernelPackage/ep80579-eth/install
endef

$(eval $(call KernelPackage,ep80579-can))
$(eval $(call KernelPackage,ep80579-eth))
$(eval $(call KernelPackage,ep80579-misc))

