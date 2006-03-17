# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=bitlbee
PKG_VERSION:=1.0.1
PKG_RELEASE:=1
PKG_MD5SUM:=a4d7daa9b64898dd41a3a6d104692d9a

PKG_SOURCE_URL:=http://get.bitlbee.org/src \
	http://get.bitlbee.be/src \
	http://get.us.bitlbee.org/src \
	http://ftp.snt.utwente.nl/pub/software/bitlbee/src
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,BITLBEE,bitlbee,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		STAGING_DIR="$(STAGING_DIR)" \
		STRIP="$(STRIP)" \
		./configure \
		--prefix=/usr/ \
		--bindir=/usr/sbin/ \
		--etcdir=/etc/bitlbee/ \
		--datadir=/usr/share/bitlbee/ \
		--config=/var/lib/bitlbee/ \
		--arch=OpenWrt \
		--cpu="$(ARCH)" \
		--debug=0 \
		--strip=1 \
		--sizeopt=1 \
		--ipv6=1 \
		--ssl=openssl \
		--glib1=1 \
		--glib1prefix="$(STAGING_DIR)/usr" \
	);
	@echo 'CFLAGS+="-I$(STAGING_DIR)/usr/include/iconv"' >> \
		$(PKG_BUILD_DIR)/Makefile.settings
	@echo 'LFLAGS+="-L$(STAGING_DIR)/usr/lib/iconv"' >> \
		$(PKG_BUILD_DIR)/Makefile.settings
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR)
	touch $@

$(IPKG_BITLBEE):
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(IDIR_BITLBEE)" install-etc install-bin 
	$(MAKE) -C $(PKG_BUILD_DIR)/doc/user-guide \
		DESTDIR="$(IDIR_BITLBEE)" install 
	install -d -m0755 $(IDIR_BITLBEE)/etc/init.d
	install -m0755 ./files/S55bitlbee $(IDIR_BITLBEE)/etc/init.d/S55bitlbee
	$(RSTRIP) $(IDIR_BITLBEE)
	$(IPKG_BUILD) $(IDIR_BITLBEE) $(PACKAGE_DIR)
	touch $@
