# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=collectd
PKG_VERSION:=3.5.1
PKG_RELEASE:=1
PKG_MD5SUM:=46a4b81e46d3bb58faab1d603649d737

PKG_SOURCE_URL:=http://verplant.org/collectd/files
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,COLLECTD,collectd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

ifneq ($(BR2_PACKAGE_COLLECTD_CPU),y)
DISABLE_CPU:=--disable-cpu
endif
ifneq ($(BR2_PACKAGE_COLLECTD_LOAD),y)
DISABLE_LOAD:=--disable-load
endif
ifneq ($(BR2_PACKAGE_COLLECTD_MEMORY),y)
DISABLE_MEMORY:=--disable-memory
endif
ifneq ($(BR2_PACKAGE_COLLECTD_PING),y)
DISABLE_PING:=--disable-ping
endif
ifneq ($(BR2_PACKAGE_COLLECTD_TRAFFIC),y)
DISABLE_TRAFFIC:=--disable-traffic
endif

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(strip $(TARGET_CFLAGS))" \
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
		  --enable-shared \
		  --disable-static \
		  --disable-debug \
		  $(DISABLE_CPU) \
		  $(DISABLE_LOAD) \
		  $(DISABLE_MEMORY) \
		  $(DISABLE_PING) \
		  $(DISABLE_TRAFFIC) \
		  --disable-cpufreq \
		  --disable-disk \
		  --disable-hddtemp \
		  --disable-nfs \
		  --disable-processes \
		  --disable-sensors \
		  --disable-serial \
		  --disable-swap \
		  --disable-tape \
		  --disable-users \
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
	touch $(PKG_BUILD_DIR)/.built

$(IPKG_COLLECTD):
	install -d -m0755 $(IDIR_COLLECTD)/etc/init.d
	install -d -m0755 $(IDIR_COLLECTD)/usr/sbin
	install -d -m0755 $(IDIR_COLLECTD)/usr/lib/collectd
	install -d -m0755 $(IDIR_COLLECTD)/var/lib/collectd
	install -d -m0755 $(IDIR_COLLECTD)/var/run
	$(CP) ./files/S80collectd $(IDIR_COLLECTD)/etc/init.d/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/collectd $(IDIR_COLLECTD)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/collectd/*.so $(IDIR_COLLECTD)/usr/lib/collectd/
	$(RSTRIP) $(IDIR_COLLECTD)
	$(IPKG_BUILD) $(IDIR_COLLECTD) $(PACKAGE_DIR)

