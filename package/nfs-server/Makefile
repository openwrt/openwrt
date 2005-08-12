# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=nfs-server
PKG_VERSION:=2.2beta47
PKG_RELEASE:=1
PKG_MD5SUM:=79a29fe9f79b2f3241d4915767b8c511

PKG_SOURCE_URL:=http://ftp.debian.org/debian/pool/main/n/nfs-user-server
PKG_SOURCE:=nfs-user-server_$(PKG_VERSION).orig.tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,NFS_SERVER,nfs-server,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

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
	$(MAKE) -C $(PKG_BUILD_DIR)
	touch $@

$(IPKG_NFS_SERVER):
	install -d -m0755 $(IDIR_NFS_SERVER)/etc
	install -m0644 ./files/nfsd.exports $(IDIR_NFS_SERVER)/etc/exports
	install -d -m0755 $(IDIR_NFS_SERVER)/etc/init.d
	install -m0755 ./files/nfsd.init $(IDIR_NFS_SERVER)/etc/init.d/S60nfsd
	install -d -m0755 $(IDIR_NFS_SERVER)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/rpc.* $(IDIR_NFS_SERVER)/usr/sbin/
	$(RSTRIP) $(IDIR_NFS_SERVER)
	$(IPKG_BUILD) $(IDIR_NFS_SERVER) $(PACKAGE_DIR)
