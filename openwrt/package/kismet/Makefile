# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=kismet
PKG_VERSION:=2006-04-R1
PKG_RELEASE:=1
PKG_MD5SUM:=8ec2de513f2911df1b7edfcba5ad1c26

PKG_SOURCE_URL:=http://www.kismetwireless.net/code
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,KISMET,kismet,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,KISMET_CLIENT,kismet-client,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,KISMET_DRONE,kismet-drone,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,KISMET_SERVER,kismet-server,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)"  \
		CXXFLAGS="$(TARGET_CFLAGS) -fno-builtin -fno-rtti -nostdinc++"  \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include -I$(LINUX_DIR)/include" \
		LDFLAGS="-nodefaultlibs -L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		LIBS="-luClibc++ -lc -lm -lgcc" \
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
			--sysconfdir=/etc/kismet \
			$(DISABLE_LARGEFILE) \
			$(DISABLE_NLS) \
			--enable-syspcap=yes \
			--disable-setuid \
			--disable-wsp100 \
			--disable-gpsmap \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		LIBS="-luClibc++ -lc -lm -lgcc" \
		all
	touch $@

$(IPKG_KISMET):
	install -d -m0755 $(IDIR_KISMET)/usr/bin/
	install -m0755 $(PKG_BUILD_DIR)/scripts/kismet $(IDIR_KISMET)/usr/bin/kismet
	$(IPKG_BUILD) $(IDIR_KISMET) $(PACKAGE_DIR)

$(IPKG_KISMET_CLIENT):
	install -d -m0755 $(IDIR_KISMET_CLIENT)/etc/kismet/
	install -m0644 ./files/ap_manuf $(IDIR_KISMET_CLIENT)/etc/kismet/
	install -m0644 ./files/client_manuf $(IDIR_KISMET_CLIENT)/etc/kismet/
	install -m0644 ./files/kismet.conf $(IDIR_KISMET_CLIENT)/etc/kismet/
	install -m0644 ./files/kismet_ui.conf $(IDIR_KISMET_CLIENT)/etc/kismet/
	install -d -m0755 $(IDIR_KISMET_CLIENT)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/kismet_client $(IDIR_KISMET_CLIENT)/usr/bin/
	$(RSTRIP) $(IDIR_KISMET_CLIENT)
	$(IPKG_BUILD) $(IDIR_KISMET_CLIENT) $(PACKAGE_DIR)

$(IPKG_KISMET_DRONE):
	install -d -m0755 $(IDIR_KISMET_DRONE)/etc/kismet/
	install -m0644 ./files/kismet_drone.conf $(IDIR_KISMET_DRONE)/etc/kismet/
	install -d -m0755 $(IDIR_KISMET_DRONE)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/kismet_drone $(IDIR_KISMET_DRONE)/usr/bin/
	$(RSTRIP) $(IDIR_KISMET_DRONE)
	$(IPKG_BUILD) $(IDIR_KISMET_DRONE) $(PACKAGE_DIR)

$(IPKG_KISMET_SERVER):
	install -d -m0755 $(IDIR_KISMET_SERVER)/etc/kismet/
	install -m0644 ./files/ap_manuf $(IDIR_KISMET_SERVER)/etc/kismet/
	install -m0644 ./files/client_manuf $(IDIR_KISMET_SERVER)/etc/kismet/
	install -m0644 ./files/kismet.conf $(IDIR_KISMET_SERVER)/etc/kismet/
	mkdir -p $(IDIR_KISMET_SERVER)/usr/bin
	$(CP)  $(PKG_BUILD_DIR)/kismet_server $(IDIR_KISMET_SERVER)/usr/bin/
	$(RSTRIP) $(IDIR_KISMET_SERVER)
	$(IPKG_BUILD) $(IDIR_KISMET_SERVER) $(PACKAGE_DIR)

mostlyclean:
#	$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built
