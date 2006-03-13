# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=lcd4linux
PKG_VERSION:=0.10.0+cvs20051015
PKG_RELEASE:=1
PKG_MD5SUM:=5b5ac629be4bb5c29104fb8f6b7fa444

PKG_SOURCE_URL:=http://ftp.debian.org/debian/pool/main/l/lcd4linux/ \
		http://ftp.de.debian.org/debian/pool/main/l/lcd4linux/
PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).orig.tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION).orig
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LCD4LINUX,lcd4linux,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

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
		  --without-x \
		  --with-drivers='all,!PNG,!RouterBoard,!X11' \
		  --with-plugins=wireless \
	)
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) LD=$(TARGET_CROSS)ld \
		CFLAGS="$(TARGET_CFLAGS) -Wall"
	touch $@

$(IPKG_LCD4LINUX):
	install -d -m0755 $(IDIR_LCD4LINUX)/usr/bin
	install -m0755 $(PKG_BUILD_DIR)/lcd4linux $(IDIR_LCD4LINUX)/usr/bin/
	$(RSTRIP) $(IDIR_LCD4LINUX)
	$(IPKG_BUILD) $(IDIR_LCD4LINUX) $(PACKAGE_DIR)
