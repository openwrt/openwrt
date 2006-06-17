# $Id$

include $(TOPDIR)/rules.mk
include $(TOPDIR)/package/kernel.mk

PKG_NAME:=hostap-driver
PKG_VERSION:=0.4.9
PKG_RELEASE:=1
PKG_MD5SUM:=c7534dc040ab90218257a78488ecd378

PKG_SOURCE_URL:=http://hostap.epitest.fi/releases/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

define Package/kmod-hostap
 SECTION:=drivers
 CATEGORY:=Drivers
 DEPENDS:=@LINUX_2_4_BRCM||LINUX_2_4_X86
 TITLE:=Host AP support for Prism2/2.5/3 IEEE 802.11b
 DESCRIPTION:=Host AP support for Prism2/2.5/3 IEEE 802.11b \\\
A driver for 802.11b wireless cards based on Intersil Prism2/2.5/3 \\\
chipset. This driver supports so called Host AP mode that allows the card \\\
to act as an IEEE 802.11 access point. \\\
\\\
This option includes the base Host AP driver code that is shared by \\\
different hardware models. You will also need to enable support for \\\
PLX/PCI/CS version of the driver to actually use the driver. \\\
 URL:=http://hostap.epitest.fi/
 VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)
endef

define Package/kmod-hostap-pci
 SECTION:=drivers
 CATEGORY:=Drivers
 DEPENDS:=kmod-hostap
 TITLE:=Host AP driver for Prism2.5 PCI adaptors
 DESCRIPTION:=Host AP driver for Prism2.5 PCI adaptors
 URL:=http://hostap.epitest.fi/
 VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)
endef

define Package/kmod-hostap-plx
 SECTION:=drivers
 CATEGORY:=Drivers
 DEPENDS:=kmod-hostap
 TITLE:=Host AP driver for Prism2/2.5/3 in PLX9052 based PCI adaptors
 DESCRIPTION:=Host AP driver for Prism2/2.5/3 in PLX9052 based PCI adaptors
 URL:=http://hostap.epitest.fi/
 VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)
endef


define Build/Compile
	$(MAKE) -C $(LINUX_DIR)/ \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		SUBDIRS="$(PKG_BUILD_DIR)/driver/modules" \
		modules
endef

define Package/kmod-hostap/install
	install -m0755 -d $(1)/lib/modules/$(LINUX_VERSION)
	$(CP) \
		$(PKG_BUILD_DIR)/driver/modules/hostap.$(LINUX_KMOD_SUFFIX) \
		$(PKG_BUILD_DIR)/driver/modules/hostap_crypt_ccmp.$(LINUX_KMOD_SUFFIX) \
		$(PKG_BUILD_DIR)/driver/modules/hostap_crypt_tkip.$(LINUX_KMOD_SUFFIX) \
		$(PKG_BUILD_DIR)/driver/modules/hostap_crypt_wep.$(LINUX_KMOD_SUFFIX) \
		$(1)/lib/modules/$(LINUX_VERSION)/
endef

define Package/kmod-hostap-pci/install
	install -m0755 -d $(1)/lib/modules/$(LINUX_VERSION)
	$(CP) \
		$(PKG_BUILD_DIR)/driver/modules/hostap_pci.$(LINUX_KMOD_SUFFIX) \
		$(1)/lib/modules/$(LINUX_VERSION)/
endef

define Package/kmod-hostap-plx/install
	install -m0755 -d $(1)/lib/modules/$(LINUX_VERSION)
	$(CP) \
		$(PKG_BUILD_DIR)/driver/modules/hostap_plx.$(LINUX_KMOD_SUFFIX) \
		$(1)/lib/modules/$(LINUX_VERSION)/
endef

$(eval $(call BuildPackage,kmod-hostap))
$(eval $(call BuildPackage,kmod-hostap-pci))
$(eval $(call BuildPackage,kmod-hostap-plx))
