# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=tinyproxy
PKG_VERSION:=1.6.3
PKG_RELEASE:=1
PKG_MD5SUM:=bd14d029b12621bcfd7ee71b2f4893da

PKG_SOURCE_URL:=@SF/tinyproxy
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,TINYPROXY,tinyproxy,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

ifeq ($(BR2_COMPILE_TINYPROXY_WITH_TRANSPARENT_PROXY),y)
ENABLE_TRANSPARENT:=--enable-transparent-proxy
endif
ifneq ($(BR2_COMPILE_TINYPROXY_WITH_UPSTREAM),y)
DISABLE_UPSTREAM:=--disable-upstream
endif
ifneq ($(BR2_COMPILE_TINYPROXY_WITH_FILTER),y)
DISABLE_FILTER:=--disable-filter
endif

$(PKG_BUILD_DIR)/.configured:
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
		  $(DISABLE_NLS) \
		  --disable-static \
		  $(ENABLE_TRANSPARENT) \
		  $(DISABLE_UPSTREAM) \
		  $(DISABLE_FILTER) \
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
	find $(PKG_INSTALL_DIR) -name '*dist' | xargs rm -f
	touch $(PKG_BUILD_DIR)/.built

$(IPKG_TINYPROXY):
	install -d -m0755 $(IDIR_TINYPROXY)/usr/sbin \
				$(IDIR_TINYPROXY)/etc/tinyproxy \
				$(IDIR_TINYPROXY)/etc/init.d \
				$(IDIR_TINYPROXY)/var/log \
				$(IDIR_TINYPROXY)/usr/share
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/tinyproxy $(IDIR_TINYPROXY)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/share/tinyproxy $(IDIR_TINYPROXY)/usr/share/
	$(CP) $(PKG_INSTALL_DIR)/etc/tinyproxy $(IDIR_TINYPROXY)/etc/
	$(CP) files/$(PKG_NAME).init $(IDIR_TINYPROXY)/etc/init.d/$(PKG_NAME)
	$(RSTRIP) $(IDIR_TINYPROXY)
	$(IPKG_BUILD) $(IDIR_TINYPROXY) $(PACKAGE_DIR)
