#
# High Capacity Media card driver. Handles high capacity and standard multimedia
# and secure digital cards
#
# This makefile is compatible for building under the kamikaze 8.09 branch
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=broadcom-sdhc
PKG_VERSION:=2.0.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.4mul8.ca/openwrt/source/broadcom-sdhc-2.0.2
PKG_MD5SUM:=6238463bce27e64829eda1d88f1a7edd
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define KernelPackage/broadcom-sdhc
  SUBMENU:=Other modules
  DEPENDS:=@TARGET_brcm_2_4
  TITLE:= MMHC/SDHC card driver - Linksys WRT54G/GS/GL
  VERSION:=$(LINUX_VERSION)-$(BOARD)-$(PKG_VERSION)-$(PKG_RELEASE)
  FILES:=\
	$(PKG_BUILD_DIR)/sdhc.$(LINUX_KMOD_SUFFIX)\
	$(PKG_BUILD_DIR)/sdhcd.$(LINUX_KMOD_SUFFIX)
  URL:=http://www.4mul8.ca/openwrt
endef 

define KernelPackage/broadcom-sdhc/description
	Driver for Linksys WRT54G/GS/GL MM/SD card modification.
	Supports standard and high capacity cards.
	May work for other broadcom based routers (Buffalo, etc).
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	ARCH="$(LINUX_KARCH)" \
	SUBDIRS="$(PKG_BUILD_DIR)" \
	modules
endef

define KernelPackage/broadcom-sdhc/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sdcard $(1)/etc/init.d/sdcard
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/sdcard.conf $(1)/etc/sdcard.conf
	$(CP) $(PKG_BUILD_DIR)/sdhc.o $(1)/lib/modules/$(LINUX_VERSION)/
	$(CP) $(PKG_BUILD_DIR)/sdhcd.o $(1)/lib/modules/$(LINUX_VERSION)/
endef

$(eval $(call KernelPackage,broadcom-sdhc))
