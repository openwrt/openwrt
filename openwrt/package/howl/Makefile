# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=howl
PKG_VERSION:=1.0.0
PKG_RELEASE:=1
PKG_MD5SUM:=c389d3ffba0e69a179de2ec650f1fdcc

PKG_SOURCE_URL:=http://www.porchdogsoft.com/download/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBHOWL,libhowl,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,MDNSRESPONDER,mdnsresponder,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,HOWL_UTILS,howl-utils,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,AUTOIPD,autoipd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,NIFD,nifd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

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
		  --enable-shared \
		  --enable-static \
	)
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_LIBHOWL):
	install -m0755 -d $(IDIR_LIBHOWL)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib{howl,mDNSResponder}.so.* $(IDIR_LIBHOWL)/usr/lib/
	$(RSTRIP) $(IDIR_LIBHOWL)
	$(IPKG_BUILD) $(IDIR_LIBHOWL) $(PACKAGE_DIR)

$(IPKG_HOWL_UTILS):
	install -m0755 -d $(IDIR_HOWL_UTILS)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/mDNS{Browse,Publish,Query,Resolve} $(IDIR_HOWL_UTILS)/usr/bin/
	$(RSTRIP) $(IDIR_HOWL_UTILS)
	$(IPKG_BUILD) $(IDIR_HOWL_UTILS) $(PACKAGE_DIR)
	
$(IPKG_MDNSRESPONDER):
	install -m0755 -d $(IDIR_MDNSRESPONDER)/etc
	install -m0644 ./files/mDNSResponder.conf $(IDIR_MDNSRESPONDER)/etc/mDNSResponder.conf
	install -m0755 -d $(IDIR_MDNSRESPONDER)/etc/init.d
	install -m0644 ./files/mDNSResponder.init $(IDIR_MDNSRESPONDER)/etc/init.d/mDNSResponder
	install -m0755 -d $(IDIR_MDNSRESPONDER)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/mDNSResponder $(IDIR_MDNSRESPONDER)/usr/sbin/
	$(RSTRIP) $(IDIR_MDNSRESPONDER)
	$(IPKG_BUILD) $(IDIR_MDNSRESPONDER) $(PACKAGE_DIR)
	
$(IPKG_AUTOIPD):
	install -m0755 -d $(IDIR_AUTOIPD)/etc/init.d
	install -m0644 ./files/autoipd.init $(IDIR_AUTOIPD)/etc/init.d/autoipd
	install -m0755 -d $(IDIR_AUTOIPD)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/autoipd $(IDIR_AUTOIPD)/usr/sbin/
	$(RSTRIP) $(IDIR_AUTOIPD)
	$(IPKG_BUILD) $(IDIR_AUTOIPD) $(PACKAGE_DIR)
	
$(IPKG_NIFD):
	install -m0755 -d $(IDIR_NIFD)/etc/init.d
	install -m0644 ./files/nifd.init $(IDIR_NIFD)/etc/init.d/nifd
	install -m0755 -d $(IDIR_NIFD)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/nifd $(IDIR_NIFD)/usr/sbin/
	$(RSTRIP) $(IDIR_NIFD)
	$(IPKG_BUILD) $(IDIR_NIFD) $(PACKAGE_DIR)
	
$(STAGING_DIR)/usr/lib/libhowl.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/howl $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib{howl,mDNSResponder}.{a,so*} $(STAGING_DIR)/usr/lib/
	mkdir -p $(STAGING_DIR)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/howl.pc $(STAGING_DIR)/usr/lib/pkgconfig/
	$(SED) 's,-I/usr/include/howl,,g' $(STAGING_DIR)/usr/lib/pkgconfig/howl.pc
	$(SED) 's,-L/usr/lib,,g' $(STAGING_DIR)/usr/lib/pkgconfig/howl.pc
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/libhowl.so

uninstall-dev:
	rm -rf \
	  $(STAGING_DIR)/usr/include/howl \
	  $(STAGING_DIR)/usr/lib/lib{howl,mDNSResponder}.{a,so*} \
	  $(STAGING_DIR)/usr/lib/pkgconfig/howl.pc \

compile-targets: install-dev
clean-targets: uninstall-dev
