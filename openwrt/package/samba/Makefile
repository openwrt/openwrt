include $(TOPDIR)/rules.mk

PKG_NAME:=samba
PKG_VERSION:=2.0.10
PKG_RELEASE:=2
PKG_MD5SUM:=54870482fe036b7e69dd48c90661eec6

PKG_SOURCE_URL:=ftp://se.samba.org/pub/samba/stable \
	ftp://ftp.easynet.be/samba/stable
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,SAMBA,samba,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,SAMBA_CLIENT,samba-client,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	(cd $(PKG_BUILD_DIR)/source ; rm -rf config.{cache,status} ; \
		./configure \
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
			--localstatedir=/var/log/samba \
			--mandir=/usr/share/man \
			--sbindir=/usr/sbin \
			--sysconfdir=/etc/samba \
			$(DISABLE_LARGEFILE) \
			$(DISABLE_NLS) \
			--with-lockdir=/var/run/samba \
			--with-privatedir=/etc/samba \
	);
	$(MAKE) -C $(PKG_BUILD_DIR)/source \
		bin/make_smbcodepage bin/make_unicodemap
	(cd $(PKG_BUILD_DIR)/source/bin ; \
		mv -f make_smbcodepage make_smbcodepage.old ; \
		mv -f make_unicodemap make_unicodemap.old ; \
	);

	$(MAKE) -C $(PKG_BUILD_DIR)/source \
		clean

	(cd $(PKG_BUILD_DIR)/source; rm -rf config.{cache,status}; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -D_GNU_SOURCE -DNDEBUG -DSHMEM_SIZE=524288 -Dfcntl=fcntl64" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-lgcc_s -L$(STAGING_DIR)/lib -L$(STAGING_DIR)/usr/lib" \
		ac_cv_lib_cups_httpConnect=no \
		ac_cv_sizeof_int=4 \
		ac_cv_sizeof_long=4 \
		ac_cv_sizeof_short=2 \
		samba_cv_FTRUNCATE_NEEDS_ROOT=no \
		samba_cv_HAVE_BROKEN_FCNTL64_LOCKS=no \
		samba_cv_HAVE_BROKEN_GETGROUPS=no \
		samba_cv_HAVE_BROKEN_READDIR=no \
		samba_cv_HAVE_FCNTL_LOCK=yes \
		samba_cv_HAVE_FNMATCH=yes \
		samba_cv_HAVE_FTRUNCATE_EXTEND=no \
		samba_cv_HAVE_GETTIMEOFDAY_TZ=yes \
		samba_cv_HAVE_IFACE_AIX=no \
		samba_cv_HAVE_IFACE_IFCONF=yes \
		samba_cv_HAVE_IFACE_IFREQ=yes \
		samba_cv_HAVE_INO64_T=yes \
		samba_cv_HAVE_IRIX_SPECIFIC_CAPABILITIES=no \
		samba_cv_HAVE_OFF64_T=yes \
		samba_cv_HAVE_ROOT=yes \
		samba_cv_HAVE_SECURE_MKSTEMP=yes \
		samba_cv_HAVE_SHARED_MMAP=yes \
		samba_cv_HAVE_STRUCT_FLOCK64=yes \
		samba_cv_HAVE_SYSV_IPC=no \
		samba_cv_HAVE_TRUNCATED_SALT=no \
		samba_cv_HAVE_UNION_SEMUN=no \
		samba_cv_HAVE_UNSIGNED_CHAR=yes \
		samba_cv_NEED_SGI_SEMUN_HACK=no \
		samba_cv_REPLACE_INET_NTOA=no \
		samba_cv_SIZEOF_INO_T=4 \
		samba_cv_SIZEOF_OFF_T=4 \
		samba_cv_SYSCONF_SC_NGROUPS_MAX=yes \
		samba_cv_USE_SETEUID=yes \
		samba_cv_USE_SETRESUID=no \
		samba_cv_USE_SETREUID=yes \
		samba_cv_USE_SETUIDX=no \
		samba_cv_have_longlong=yes \
		samba_cv_have_setresgid=no \
		samba_cv_have_setresuid=no \
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
			--localstatedir=/var/log/samba \
			--mandir=/usr/share/man \
			--sbindir=/usr/sbin \
			--sysconfdir=/etc/samba \
			$(DISABLE_LARGEFILE) \
			$(DISABLE_NLS) \
			--with-lockdir=/var/run/samba \
			--with-privatedir=/etc/samba \
			--with-syslog \
	);
	$(MAKE) -C $(PKG_BUILD_DIR)/source \
		$(TARGET_CONFIGURE_OPTS) \
		CODEPAGEDIR="/usr/share/samba" \
		all shared
	(cd $(PKG_BUILD_DIR)/source/bin ; \
		mv -f make_smbcodepage.old make_smbcodepage ; touch make_smbcodepage ; \
		mv -f make_unicodemap.old make_unicodemap ; touch make_unicodemap ; \
	);
	mkdir -p $(PKG_INSTALL_DIR)/usr/share/samba
	mkdir -p $(PKG_INSTALL_DIR)/var/log
	$(MAKE) -C $(PKG_BUILD_DIR)/source \
		BASEDIR="$(PKG_INSTALL_DIR)/usr" \
		BINDIR="$(PKG_INSTALL_DIR)/usr/bin" \
		SBINDIR="$(PKG_INSTALL_DIR)/usr/bin" \
		LIBDIR="$(PKG_INSTALL_DIR)/usr/lib" \
		VARDIR="$(PKG_INSTALL_DIR)/var/log/samba" \
		MANDIR="$(PKG_INSTALL_DIR)/usr/share/man" \
		CONFIGDIR="$(PKG_INSTALL_DIR)/etc/samba" \
		PRIVATEDIR="$(PKG_INSTALL_DIR)/etc/samba" \
		SWATDIR="$(PKG_INSTALL_DIR)/usr/swat" \
		LOCKDIR="$(PKG_INSTALL_DIR)/var/run/samba" \
		SAMBABOOK="$(PKG_INSTALL_DIR)/usr/swat/using_samba" \
		CODEPAGEDIR="$(PKG_INSTALL_DIR)/usr/share/samba" \
		install
	touch $@

$(IPKG_SAMBA):
	install -d -m0755 $(IDIR_SAMBA)/etc/init.d
	install -m0755 ./files/samba.init $(IDIR_SAMBA)/etc/init.d/samba
	install -d -m0755 $(IDIR_SAMBA)/etc/samba
	install -m0644 ./files/smb.conf $(IDIR_SAMBA)/etc/samba/smb.conf
	install -d -m0755 $(IDIR_SAMBA)/usr/bin
	install -m0755 $(PKG_BUILD_DIR)/source/bin/smbpasswd.shared $(IDIR_SAMBA)/usr/bin/smbpasswd
	install -d -m0755 $(IDIR_SAMBA)/usr/lib
	install -m0755 $(PKG_BUILD_DIR)/source/bin/libsmb.so $(IDIR_SAMBA)/usr/lib/
	install -d -m0755 $(IDIR_SAMBA)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/source/bin/nmbd.shared $(IDIR_SAMBA)/usr/sbin/nmbd
	install -m0755 $(PKG_BUILD_DIR)/source/bin/smbd.shared $(IDIR_SAMBA)/usr/sbin/smbd
	install -d -m0755 $(IDIR_SAMBA)/usr/share/samba
	install -m0644 $(PKG_INSTALL_DIR)/usr/share/samba/codepage.{437,850} $(IDIR_SAMBA)/usr/share/samba/
	install -m0644 $(PKG_INSTALL_DIR)/usr/share/samba/unicode_map.{437,850} $(IDIR_SAMBA)/usr/share/samba/
	$(RSTRIP) $(IDIR_SAMBA)
	$(IPKG_BUILD) $(IDIR_SAMBA) $(PACKAGE_DIR)

$(IPKG_SAMBA_CLIENT):
	install -d -m0755 $(IDIR_SAMBA_CLIENT)/usr/bin
	install -m0755 $(PKG_INSTALL_DIR)/usr/bin/smbclient $(IDIR_SAMBA_CLIENT)/usr/bin/
	$(RSTRIP) $(IDIR_SAMBA_CLIENT)
	$(IPKG_BUILD) $(IDIR_SAMBA_CLIENT) $(PACKAGE_DIR)

mostlyclean:
	-$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built

