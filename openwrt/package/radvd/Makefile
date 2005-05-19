# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=radvd
PKG_VERSION:=0.7.3
PKG_RELEASE:=1
PKG_MD5SUM:=56ce3f8cbf5966a0d531c21813320423

PKG_SOURCE_URL:=http://v6web.litech.org/radvd/dist \
		http://download.sourcemage.org/mirror
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,RADVD,radvd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
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
		  --with-configfile=/etc/radvd.conf \
		  --with-logfile=/var/log/radvd.log \
		  --with-pidfile=/var/run/radvd.pid \
	);
	touch $@

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR) \
		radvd_LDADD=""
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
	touch $@

$(IPKG_RADVD):
	install -d -m0755 $(IDIR_RADVD)/etc
	install -m0644 ./files/radvd.conf $(IDIR_RADVD)/etc/radvd.conf
	install -d -m0755 $(IDIR_RADVD)/etc/init.d
	install -m0755 ./files/radvd.init $(IDIR_RADVD)/etc/init.d/S51radvd
	install -d -m0755 $(IDIR_RADVD)/usr/sbin
	install -m 755 $(PKG_INSTALL_DIR)/usr/sbin/radvd $(IDIR_RADVD)/usr/sbin/
	install -m 755 $(PKG_INSTALL_DIR)/usr/sbin/radvdump $(IDIR_RADVD)/usr/sbin/
	$(RSTRIP) $(IDIR_RADVD)
	$(IPKG_BUILD) $(IDIR_RADVD) $(PACKAGE_DIR)
