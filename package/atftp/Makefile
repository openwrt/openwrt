# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=atftp
PKG_VERSION:=0.7
PKG_RELEASE:=1
PKG_MD5SUM:=3b27365772d918050b2251d98a9c7c82

PKG_SOURCE_URL:=ftp://ftp.mamalinux.com/pub/atftp/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,ATFTP,atftp,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,ATFTPD,atftpd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured: 
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
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
		  $(DISABLE_LARGEFILE) \
		  $(DISABLE_NLS) \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -Wall -D_REENTRANT" \
		DESTDIR=$(PKG_INSTALL_DIR) \
		all install
	touch $@

$(IPKG_ATFTP):
	install -d -m0755 $(IDIR_ATFTP)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/atftp $(IDIR_ATFTP)/usr/bin
	$(RSTRIP) $(IDIR_ATFTP)
	$(IPKG_BUILD) $(IDIR_ATFTP) $(PACKAGE_DIR)

$(IPKG_ATFTPD):
	install -d -m0755 $(IDIR_ATFTPD)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/atftpd $(IDIR_ATFTPD)/usr/sbin
	$(RSTRIP) $(IDIR_ATFTPD)
	$(IPKG_BUILD) $(IDIR_ATFTPD) $(PACKAGE_DIR)
