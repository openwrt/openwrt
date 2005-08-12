# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=linux-atm
PKG_VERSION:=2.4.1
PKG_RELEASE:=1
PKG_MD5SUM:=84fef49cc39ff2605204246666f65864

PKG_SOURCE_URL:=http://ftp.debian.org/debian/pool/main/l/linux-atm/
PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).orig.tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

ifneq ($(BOARD),ar7)
BR2_PACKAGE_LINUX_ATM:=m
endif

$(eval $(call PKG_template,LINUX_ATM,linux-atm,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		autoconf; \
		$(TARGET_CONFIGURE_OPTS) \
		./configure \
		  --target=$(GNU_TARGET_NAME) \
		  --host=$(GNU_TARGET_NAME) \
		  --build=$(GNU_HOST_NAME) \
		  --program-prefix="" \
		  --program-suffix="" \
		  --prefix=/usr \
		  --exec-prefix=/usr \
		  --bindir=/usr/bin \
		  --datadir=/usr/share \
		  --includedir=/usr/src/openwrt/build_mipsel/linux/include/ \
		  --infodir=/usr/share/info \
		  --libdir=/usr/lib \
		  --libexecdir=/usr/lib \
		  --localstatedir=/var \
		  --mandir=/usr/share/man \
		  --sbindir=/usr/sbin \
		  --sysconfdir=/etc \
	)
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) 
	touch $@

$(IPKG_LINUX_ATM): 
	mkdir -p $(IDIR_LINUX_ATM)/usr/sbin
	mkdir -p $(IDIR_LINUX_ATM)/lib
	cp $(PKG_BUILD_DIR)/src/br2684/.libs/br2684ctl $(IDIR_LINUX_ATM)/usr/sbin/
	cp $(PKG_BUILD_DIR)/src/lib/.libs/libatm.1.0.0 $(IDIR_LINUX_ATM)/lib
	ln -sf libatm.1.0.0 $(IDIR_LINUX_ATM)/lib/libatm
	ln -sf libatm.1.0.0 $(IDIR_LINUX_ATM)/lib/libatm.1
	$(RSTRIP) $(IDIR_LINUX_ATM)/
	$(IPKG_BUILD) $(IDIR_LINUX_ATM) $(PACKAGE_DIR)

mostlyclean:
	$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built

