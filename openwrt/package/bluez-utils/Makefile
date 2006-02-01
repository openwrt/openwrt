# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=bluez-utils
PKG_VERSION:=2.24
PKG_RELEASE:=1
PKG_MD5SUM:=

PKG_SOURCE_URL:=http://bluez.sourceforge.net/download
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,BLUEZ_UTILS,bluez-utils,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.source: $(DL_DIR)/$(PKG_SOURCE)
	zcat $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(BLUEZLIBS_DIR)/.source

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
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
			--disable-rpath \
			--disable-dbus \
			--disable-fuse \
			--disable-obex \
			--disable-alsa \
			--disable-cups \
			--disable-pcmcia \
			--disable-initscripts \
			--disable-bccmd \
			--disable-avctrl \
			--disable-hid2hci \
			--disable-dfutool \
			--disable-bcm203x \
			--disable-bluepin \
			--with-bluez=$(STAGING_DIR)/usr/include \
			--with-usb=$(STAGING_DIR)/usr/include \
	)
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_BLUEZ_UTILS):
	install -d -m0755 $(IDIR_BLUEZ_UTILS)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/* $(IDIR_BLUEZ_UTILS)/usr/bin/
	install -d -m0755 $(IDIR_BLUEZ_UTILS)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/* $(IDIR_BLUEZ_UTILS)/usr/sbin/
	install -d -m0755 $(IDIR_BLUEZ_UTILS)/etc/bluetooth
	$(CP) $(PKG_INSTALL_DIR)/../hcid/hcid.conf $(IDIR_BLUEZ_UTILS)/etc/bluetooth/
	$(CP) $(PKG_INSTALL_DIR)/../rfcomm/rfcomm.conf $(IDIR_BLUEZ_UTILS)/etc/bluetooth/
	install -m0700 ./files/givepin $(IDIR_BLUEZ_UTILS)/etc/bluetooth/
	$(RSTRIP) $(IDIR_BLUEZ_UTILS)
	$(IPKG_BUILD) $(IDIR_BLUEZ_UTILS) $(PACKAGE_DIR)

