include $(TOPDIR)/rules.mk

PKG_NAME:=peerguardian
PKG_VERSION:=1.5beta
PKG_RELEASE:=1
PKG_MD5SUM:=0fb2bc5501b031604fc56eec3bd35fa4

PKG_SOURCE_URL:=@SF/peerguardian
PKG_SOURCE:=pglinux-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/pglinux-$(PKG_VERSION)
PKG_CAT:=zcat
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,PEERGUARDIAN,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/usr \
		--exec-prefix=/usr \
		--bindir=/usr/bin \
		--sbindir=/usr/sbin \
		--libexecdir=/usr/lib \
		--sysconfdir=/etc \
		--datadir=/usr/share \
		--localstatedir=/var \
		--mandir=/usr/man \
		--infodir=/usr/info \
		--program-prefix="" \
		--with-gnu-ld \
	);
	touch $@
	
$(PKG_BUILD_DIR)/.built:
	$(MAKE) STAGING_DIR=$(STAGING_DIR) DESTDIR=$(PKG_INSTALL_DIR) -C $(PKG_BUILD_DIR)
	install -d -m0755 $(PKG_INSTALL_DIR)/usr/sbin $(PKG_INSTALL_DIR)/etc/init.d
	install -m0755 $(PKG_BUILD_DIR)/peerguardnf $(PKG_INSTALL_DIR)/usr/sbin/
	install -m0755 $(PKG_BUILD_DIR)/pgtext $(PKG_INSTALL_DIR)/usr/sbin/
	install -m0644 $(PKG_BUILD_DIR)/PG.conf $(PKG_INSTALL_DIR)/etc/
	install -m0644 $(PKG_BUILD_DIR)/p2p.p2b.p2p $(PKG_INSTALL_DIR)/etc/
	install -m0644 files/$(PKG_NAME).init $(PKG_INSTALL_DIR)/etc/init.d/$(PKG_NAME)
	
$(IPKG_PEERGUARDIAN):
	mkdir -p $(IDIR_PEERGUARDIAN)
	$(CP) $(PKG_INSTALL_DIR)/* $(IDIR_PEERGUARDIAN)/
	$(RSTRIP) $(IDIR_PEERGUARDIAN)/
	$(IPKG_BUILD) $(IDIR_PEERGUARDIAN) $(PACKAGE_DIR)
