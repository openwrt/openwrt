include $(TOPDIR)/rules.mk

PKG_NAME:=samba
PKG_VERSION:=2.0.10
PKG_RELEASE:=1
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

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR)/source; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -D_GNU_SOURCE -DNDEBUG -DSHMEM_SIZE=524288 -Dfcntl=fcntl64" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/lib -L$(STAGING_DIR)/usr/lib" \
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
			--enable-cups \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR)/source \
		$(TARGET_CONFIGURE_OPTS) \
		all
	touch $@

$(IPKG_SAMBA):
	install -d -m0755 $(IDIR_SAMBA)/etc/init.d
	install -m0755 ./files/samba.init $(IDIR_SAMBA)/etc/init.d/samba
	install -d -m0755 $(IDIR_SAMBA)/etc/samba
	install -m0644 ./files/smb.conf $(IDIR_SAMBA)/etc/samba/smb.conf
	install -d -m0755 $(IDIR_SAMBA)/usr/bin
	install -m0755 $(PKG_BUILD_DIR)/source/bin/smbpasswd $(IDIR_SAMBA)/usr/bin/
	install -d -m0755 $(IDIR_SAMBA)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/source/bin/{n,s}mbd $(IDIR_SAMBA)/usr/sbin/
	$(RSTRIP) $(IDIR_SAMBA)
	$(IPKG_BUILD) $(IDIR_SAMBA) $(PACKAGE_DIR)

$(IPKG_SAMBA_CLIENT):
	install -d -m0755 $(IDIR_SAMBA_CLIENT)/usr/bin
	install -m0755 $(PKG_BUILD_DIR)/source/bin/smbclient $(IDIR_SAMBA_CLIENT)/usr/bin/
	$(RSTRIP) $(IDIR_SAMBA_CLIENT)
	$(IPKG_BUILD) $(IDIR_SAMBA_CLIENT) $(PACKAGE_DIR)

mostlyclean:
	make -C $(PKG_BUILD_DIR) clean
	rm $(PKG_BUILD_DIR)/.built
