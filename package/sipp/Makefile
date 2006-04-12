# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=sipp
PKG_VERSION:=1.0
PKG_RELEASE:=1
PKG_MD5SUM:=8866e9af0d3bc29e91ebb6eab89a7f1f

PKG_SOURCE_URL:=@SF/sipp
PKG_SOURCE:=$(PKG_NAME).$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,SIPP,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) \
		CC_linux=$(TARGET_CC) \
		CPP_linux=$(TARGET_CC) \
		CCLINK_linux=$(TARGET_CC) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS_linux="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include -fno-builtin -nostdinc++" \
		LFLAGS_linux="-nodefaultlibs -L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		LIBS="-luClibc++ -lc -lm -lgcc -lpthread -ldl -lncurses" \
		STAGING_DIR=$(STAGING_DIR) \
		all
	touch $@

$(IPKG_SIPP): 
	mkdir -p $(IDIR_SIPP)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/$(PKG_NAME) $(IDIR_SIPP)/usr/sbin/
	$(STRIP) $(IDIR_SIPP)/usr/sbin/*
	$(IPKG_BUILD) $(IDIR_SIPP) $(PACKAGE_DIR)

mostlyclean:
	$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built
