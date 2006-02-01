# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=srelay
PKG_VERSION:=0.4.6
PKG_RELEASE:=1
PKG_MD5SUM:=4a9f3298b38d8588f0ffde31b946a47f

PKG_SOURCE_URL:=@SF/socks-relay
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install
PKG_INIT_LEVEL:=60

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,SRELAY,srelay,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
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
		  --includedir=/usr/include \
		  --infodir=/usr/share/info \
		  --libdir=/usr/lib \
		  --libexecdir=/usr/lib \
		  --localstatedir=/var \
		  --mandir=/usr/share/man \
		  --sbindir=/usr/sbin \
		  --sysconfdir=/etc \
		  --disable-thread \
		  $(DISABLE_NLS) \
		  $(DISABLE_LARGEFILE) \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) \
		all
	$(CP) $(PKG_BUILD_DIR)/srelay $(PKG_INSTALL_DIR)
	touch $@

$(IPKG_SRELAY):
	install -m0755 -d $(IDIR_SRELAY)/usr/bin
	install -m0755 -d $(IDIR_SRELAY)/etc/init.d
	$(CP) $(PKG_INSTALL_DIR)/srelay $(IDIR_SRELAY)/usr/bin/
	$(CP) files/srelay.init $(IDIR_SRELAY)/etc/init.d/S$(PKG_INIT_LEVEL)srelay
	$(CP) files/srelay.conf $(IDIR_SRELAY)/etc
	chmod 755 $(IDIR_SRELAY)/etc/init.d/S$(PKG_INIT_LEVEL)srelay
	$(RSTRIP) $(IDIR_SRELAY)
	$(IPKG_BUILD) $(IDIR_SRELAY) $(PACKAGE_DIR)
