# $Id$

include $(TOPDIR)/rules.mk
include ../../rules.mk

PKG_NAME:=openswan
PKG_VERSION:=2.4.0
PKG_RELEASE:=1
PKG_MD5SUM:=6cbba783658eb62b87cbf4937f5e8464

PKG_SOURCE_URL:=http://www.openswan.org/download
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT:=zcat

include $(TOPDIR)/package/rules.mk

ifeq ($(KERNEL_DIR),)
KERNEL_DIR:=$(LINUX_DIR)
endif

$(eval $(call PKG_template,KMOD_OPENSWAN,kmod-openswan,$(LINUX_VERSION)$(BOARD)+$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH),kernel ($(LINUX_VERSION)-$(BOARD)-$(KERNEL_RELEASE))))

FLAGS := $(TARGET_CFLAGS) -I$(PKG_BUILD_DIR)/linux/include -L$(STAGING_DIR)/usr/lib -I$(STAGING_DIR)/usr/include

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		KERNELSRC="$(KERNEL_DIR)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		USERCOMPILE="$(FLAGS)" \
		module

$(IPKG_KMOD_OPENSWAN):
	mkdir -p $(IDIR_KMOD_OPENSWAN)/lib/modules/$(LINUX_VERSION)
	cp -fpR $(PKG_BUILD_DIR)/modobj*/ipsec.$(LINUX_KMOD_SUFFIX) $(IDIR_KMOD_OPENSWAN)/lib/modules/$(LINUX_VERSION)/
	$(IPKG_BUILD) $(IDIR_KMOD_OPENSWAN) $(PACKAGE_DIR)
