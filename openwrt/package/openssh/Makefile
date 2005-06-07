# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=openssh
PKG_VERSION:=4.0p1
PKG_RELEASE:=1
PKG_MD5SUM:=7b36f28fc16e1b7f4ba3c1dca191ac92

PKG_SOURCE_URL:=ftp://ftp.openbsd.org/pub/OpenBSD/OpenSSH/portable/ \
		ftp://openbsd.wiretapped.net/pub/OpenBSD/OpenSSH/portable/ \
	 	ftp://ftp.belnet.be/packages/openbsd/OpenSSH/portable/ \
	   	ftp://ftp.de.openbsd.org/pub/unix/OpenBSD/OpenSSH/portable/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

PKG_DEPEND:="openssl"

$(eval $(call PKG_template,OPENSSH,openssh,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
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
		--sysconfdir=/etc/ssh \
		--datadir=/usr/share \
		--localstatedir=/var \
		--mandir=/usr/man \
		--infodir=/usr/info \
		--with-zlib=$(STAGING_DIR)/usr \
		--with-ssl-dir=$(STAGING_DIR)/usr \
		--disable-strip \
		--disable-lastlog \
		--disable-etc-default-login \
		--disable-utmp \
		--disable-utmpx \
		--disable-wtmp \
		--disable-wtmpx \
		--with-cflags="$(TARGET_CFLAGS)" \
		--includedir=$(STAGING_DIR)/include \
		--without-pam \
		--without-bsd-auth \
		--without-kerberos5 \
		--without-x \
		--disable-debug \
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	$(MAKE) CC=$(TARGET_CC) -C $(PKG_BUILD_DIR)
	touch $(PKG_BUILD_DIR)/.built

$(PKG_BUILD_DIR)/.installed: $(PKG_BUILD_DIR)/.built
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) CC=$(TARGET_CC) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
	touch $(PKG_BUILD_DIR)/.installed

$(IPKG_OPENSSH): $(IDIR_OPENSSH)/CONTROL/control $(PKG_BUILD_DIR)/.installed
	mkdir -p $(IDIR_OPENSSH){/etc/ssh,/usr/sbin,/usr/bin}
	cp -fpR $(PKG_INSTALL_DIR)/usr/sbin/sshd $(IDIR_OPENSSH)/usr/sbin/
	cp -fpR $(PKG_INSTALL_DIR)/usr/bin/ssh-keygen $(IDIR_OPENSSH)/usr/bin/
	#$(RSTRIP) $(IDIR_OPENSSH)
	mkdir -p $(PACKAGE_DIR)
	cp $(PKG_INSTALL_DIR)/etc/ssh/* $(IDIR_OPENSSH)/etc/ssh/
	mkdir -p $(IDIR_OPENSSH)/etc/init.d
	install -m 755 ./files/S50sshd $(IDIR_OPENSSH)/etc/init.d/
	$(IPKG_BUILD) $(IDIR_OPENSSH) $(PACKAGE_DIR)

