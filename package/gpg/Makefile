# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=gnupg
PKG_VERSION:=1.4.2.1
PKG_RELEASE:=1

PKG_SOURCE_URL:=ftp://ftp.gnupg.org/gcrypt/gnupg/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_MD5:=2d59ed50d92f69994dbfbe00fe1612e4
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,GPG,gpg,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include" \
		ac_cv_c_bigendian=no \
		ac_cv_sizeof_off_t=8 \
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
		--disable-gnupg-iconv \
		--disable-card-support \
		--disable-agent-support \
		--disable-bzip2 \
		--disable-exec \
		--disable-ldap \
		--disable-hkp \
		--disable-finger \
		--disable-ftp \
		--disable-largefile \
		--disable-dns-srv \
		--enable-fake-curl \
		--disable-nls \
		--disable-rpath \
		--disable-regex \
		$(DISABLE_NLS) \
	);
	touch $@

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC)
	touch $@

$(IPKG_GPG): 
	mkdir -p $(IDIR_GPG)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/g10/gpg $(IDIR_GPG)/usr/bin/
	$(RSTRIP) $(IDIR_GPG)
	$(IPKG_BUILD) $(IDIR_GPG) $(PACKAGE_DIR)
