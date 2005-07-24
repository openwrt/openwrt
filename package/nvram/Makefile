# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=nvram
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(TOPDIR)/package/rules.mk

# does not depend on menuconfig
ifneq ($(BOARD),brcm)
BR2_PACKAGE_NVRAM:=m
else
BR2_PACKAGE_NVRAM:=y
endif

$(eval $(call PKG_template,NVRAM,nvram,$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.prepared:
	mkdir -p $(PKG_BUILD_DIR)
	cp -a ./src/* $(PKG_BUILD_DIR)
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -I $(STAGING_DIR)/usr/include"
	touch $@

$(IPKG_NVRAM):
	mkdir -p $(IDIR_NVRAM)/usr/lib
	cp $(PKG_BUILD_DIR)/*.so $(IDIR_NVRAM)/usr/lib
	mkdir -p $(IDIR_NVRAM)/usr/sbin
	cp $(PKG_BUILD_DIR)/nvram $(IDIR_NVRAM)/usr/sbin
	$(RSTRIP) $(IDIR_NVRAM)
	$(IPKG_BUILD) $(IDIR_NVRAM) $(PACKAGE_DIR)


compile: install-dev
install-dev:
	mkdir -p $(STAGING_DIR)/usr/lib
	cp $(PKG_BUILD_DIR)/*.so $(STAGING_DIR)/usr/lib
	

