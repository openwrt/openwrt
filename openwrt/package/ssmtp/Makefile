# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=ssmtp
PKG_VERSION:=2.61
PKG_RELEASE:=1
PKG_MD5SUM:=957e6fff08625fe34f4fc33d0925bbc9

PKG_SOURCE_URL:=http://ftp.debian.org/debian/pool/main/s/ssmtp/
PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).orig.tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

BR2_PACKAGE_SSMTP=m	

$(eval $(call PKG_template,SSMTP,ssmtp,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

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
		  $(DISABLE_NLS) \
		  $(DISABLE_LARGEFILE) \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		all
	touch $@

$(IPKG_SSMTP):
	install -m0755 -d $(IDIR_SSMTP)/etc/ssmtp
	install -m0644 $(PKG_BUILD_DIR)/ssmtp.conf $(IDIR_SSMTP)/etc/ssmtp/
	install -m0644 $(PKG_BUILD_DIR)/revaliases $(IDIR_SSMTP)/etc/ssmtp/
	install -m0755 -d $(IDIR_SSMTP)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/ssmtp $(IDIR_SSMTP)/usr/sbin/
	ln -sf ssmtp $(IDIR_SSMTP)/usr/sbin/sendmail
	$(RSTRIP) $(IDIR_SSMTP)
	$(IPKG_BUILD) $(IDIR_SSMTP) $(PACKAGE_DIR)

