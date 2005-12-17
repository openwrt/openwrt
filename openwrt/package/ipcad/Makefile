include $(TOPDIR)/rules.mk

PKG_NAME:=ipcad
PKG_VERSION:=3.7
PKG_RELEASE:=1
PKG_MD5SUM:=34c542c99593dfe9d5b408deffcfe70e
PKG_SOURCE_URL:=@SF/ipcad
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT:=zcat

PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,IPCAD,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
                $(TARGET_CONFIGURE_OPTS) \
                CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include -L$(STAGING_DIR)/usr/lib" \
                ./configure \
                --target=$(GNU_TARGET_NAME) \
                --host=$(GNU_TARGET_NAME) \
                --build=$(GNU_HOST_NAME) \
                --prefix=/usr \
                --exec-prefix=/usr \
                --bindir=/usr/bin \
                --sbindir=/usr/sbin \
                --libexecdir=/usr/lib \
                --sysconfdir=/etc \
                --datadir=/usr/share \
                --localstatedir=/var \
                --mandir=/usr/man \
		--mandir=/usr/man \
                --infodir=/usr/info \
                --program-prefix="" \
		--with-gnu-ld \
		--with-psrc=pcap \
		--with-pcap-include=$(STAGING_DIR)/usr/include \
		--with-pcap-libraries=$(STAGING_DIR)/usr/lib \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) STAGING_DIR=$(STAGING_DIR) DESTDIR=$(PKG_INSTALL_DIR) -C $(PKG_BUILD_DIR) all install-bin install-conf
	touch $@

$(IPKG_IPCAD):
	mkdir -p $(IDIR_IPCAD)
	cp -fpR $(PKG_INSTALL_DIR)/* $(IDIR_IPCAD)/
	$(RSTRIP) $(IDIR_IPCAD)
	$(IPKG_BUILD) $(IDIR_IPCAD) $(PACKAGE_DIR)
