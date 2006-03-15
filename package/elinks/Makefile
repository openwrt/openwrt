# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=elinks
PKG_VERSION:=0.11.1
PKG_RELEASE:=1

PKG_SOURCE_URL:=http://elinks.or.cz/download/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_MD5SUM:=c09d726075e4b5810fa07f8654088755
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,ELINKS,elinks,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
	        x=`echo ac_cv_file_$(PKG_BUILD_DIR)/features.conf | tr [:blank:]-/. _` && export $${x}=yes && \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include" \
		ac_cv_c_bigendian=no \
		ac_cv_sizeof_off_t=8 \
		ac_cv_file___features_conf=yes \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--disable-largefile\
		--disable-formhist\
		--disable-mailcap\
		--disable-data\
		--disable-uri-rewrite\
		--enable-256-colors\
		--disable-backtrace\
		--enable-small\
		--without-spidermonkey\
		--without-x\
		--disable-ipv6 \
		$(DISABLE_NLS) \
	);
	touch $@

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR)\
		CC=$(TARGET_CC) LD=$(TARGET_CROSS)ld
	touch $@

$(IPKG_ELINKS): 
	mkdir -p $(IDIR_ELINKS)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/src/elinks $(IDIR_ELINKS)/usr/bin/
	$(RSTRIP) $(IDIR_ELINKS)
	$(IPKG_BUILD) $(IDIR_ELINKS) $(PACKAGE_DIR)
