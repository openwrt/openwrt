# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=openssh
PKG_VERSION:=4.2p1
PKG_RELEASE:=2
PKG_MD5SUM:=df899194a340c933944b193477c628fa

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

$(eval $(call PKG_template,OPENSSH_CLIENT,openssh-client,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSSH_CLIENT_UTILS,openssh-client-utils,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSSH_SERVER,openssh-server,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSSH_SFTP_CLIENT,openssh-sftp-client,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSSH_SFTP_SERVER,openssh-sftp-server,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LD="$(TARGET_CC)" \
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
			--sysconfdir=/etc/ssh \
			$(DISABLE_LARGEFILE) \
			$(DISABLE_NLS) \
			--enable-shared \
			--disable-static \
			--disable-debug \
			--disable-strip \
			--disable-etc-default-login \
			--disable-lastlog \
			--disable-utmp \
			--disable-utmpx \
			--disable-wtmp \
			--disable-wtmpx \
			--without-bsd-auth \
			--without-kerberos5 \
			--without-pam \
			--without-x \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_OPENSSH_CLIENT):
	install -m0755 -d $(IDIR_OPENSSH_CLIENT)/etc/ssh
	chmod 0700 $(IDIR_OPENSSH_CLIENT)/etc/ssh
	$(CP) $(PKG_INSTALL_DIR)/etc/ssh/ssh_config $(IDIR_OPENSSH_CLIENT)/etc/ssh/
	install -m0755 -d $(IDIR_OPENSSH_CLIENT)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/ssh $(IDIR_OPENSSH_CLIENT)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/scp $(IDIR_OPENSSH_CLIENT)/usr/bin/
	$(RSTRIP) $(IDIR_OPENSSH_CLIENT)
	$(IPKG_BUILD) $(IDIR_OPENSSH_CLIENT) $(PACKAGE_DIR)

$(IPKG_OPENSSH_CLIENT_UTILS):
	install -m0755 -d $(IDIR_OPENSSH_CLIENT_UTILS)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/ssh-add $(IDIR_OPENSSH_CLIENT_UTILS)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/ssh-agent $(IDIR_OPENSSH_CLIENT_UTILS)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/ssh-keyscan $(IDIR_OPENSSH_CLIENT_UTILS)/usr/bin/
	$(RSTRIP) $(IDIR_OPENSSH_CLIENT_UTILS)
	$(IPKG_BUILD) $(IDIR_OPENSSH_CLIENT_UTILS) $(PACKAGE_DIR)

$(IPKG_OPENSSH_SERVER):
	install -m0755 -d $(IDIR_OPENSSH_SERVER)/etc/ssh
	chmod 0700 $(IDIR_OPENSSH_SERVER)/etc/ssh
	$(CP) $(PKG_INSTALL_DIR)/etc/ssh/sshd_config $(IDIR_OPENSSH_SERVER)/etc/ssh/
	install -m0755 -d $(IDIR_OPENSSH_SERVER)/etc/init.d
	install -m0755 ./files/S50sshd $(IDIR_OPENSSH_SERVER)/etc/init.d/
	install -m0755 -d $(IDIR_OPENSSH_SERVER)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/ssh-keygen $(IDIR_OPENSSH_SERVER)/usr/bin/
	install -m0755 -d $(IDIR_OPENSSH_SERVER)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/sshd $(IDIR_OPENSSH_SERVER)/usr/sbin/
	$(RSTRIP) $(IDIR_OPENSSH_SERVER)
	$(IPKG_BUILD) $(IDIR_OPENSSH_SERVER) $(PACKAGE_DIR)

$(IPKG_OPENSSH_SFTP_CLIENT):
	install -m0755 -d $(IDIR_OPENSSH_SFTP_CLIENT)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/sftp $(IDIR_OPENSSH_SFTP_CLIENT)/usr/bin/
	$(RSTRIP) $(IDIR_OPENSSH_SFTP_CLIENT)
	$(IPKG_BUILD) $(IDIR_OPENSSH_SFTP_CLIENT) $(PACKAGE_DIR)

$(IPKG_OPENSSH_SFTP_SERVER):
	install -m0755 -d $(IDIR_OPENSSH_SFTP_SERVER)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/sftp-server $(IDIR_OPENSSH_SFTP_SERVER)/usr/lib/
	install -m0755 -d $(IDIR_OPENSSH_SFTP_SERVER)/usr/libexec
	ln -sf ../lib/sftp-server $(IDIR_OPENSSH_SFTP_SERVER)/usr/libexec/sftp-server
	$(RSTRIP) $(IDIR_OPENSSH_SFTP_SERVER)
	$(IPKG_BUILD) $(IDIR_OPENSSH_SFTP_SERVER) $(PACKAGE_DIR)

