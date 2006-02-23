# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=openntpd
PKG_VERSION:=3.7p1
PKG_RELEASE:=1
PKG_MD5SUM:=10ed8eefd760e5819efcf3277b118f47

# space separated list or special @SF for sourceforge projects
PKG_SOURCE_URL:=ftp://ftp.openbsd.org/pub/OpenBSD/OpenNTPD/ \
	ftp://ftp.de.openbsd.org/pub/unix/OpenBSD/OpenNTPD/ \
	ftp://ftp.fr.openbsd.org/pub/OpenBSD/OpenNTPD/ \
	ftp://ftp.sunet.se/pub/OpenBSD/OpenNTPD/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,OPENNTPD,openntpd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		autoconf ; \
		autoheader ; \
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
		  --with-builtin-arc4random \
		  --with-privsep-user=ntp \
		  --with-adjtimex \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR)
	touch $@

$(IPKG_OPENNTPD):
	install -d -m0755 $(IDIR_OPENNTPD)/etc
	install -m0644 ./files/ntpd.conf $(IDIR_OPENNTPD)/etc/
	install -d -m0755 $(IDIR_OPENNTPD)/etc/init.d
	install -m0755 ./files/ntpd.init $(IDIR_OPENNTPD)/etc/init.d/S55ntpd
	install -d -m0755 $(IDIR_OPENNTPD)/usr/sbin/
	install -m0755 $(PKG_BUILD_DIR)/ntpd $(IDIR_OPENNTPD)/usr/sbin/
	$(RSTRIP) $(IDIR_OPENNTPD)
	$(IPKG_BUILD) $(IDIR_OPENNTPD) $(PACKAGE_DIR)

