## ppp & ppp-radius-plugin

PPP_VERSION := 2.4.2
PPP_RELEASE := 1

PPP_SOURCE:=ppp-$(PPP_VERSION).tar.gz
PPP_SITE:=ftp://ftp.samba.org/pub/ppp
PPP_DIR:=$(BUILD_DIR)/ppp-$(PPP_VERSION)
PPP_CAT:=zcat

PPP_PATCH_DIR := $(SOURCE_DIR)/openwrt/patches/ppp

PPP_BUILD_DIR := $(BUILD_DIR)/ppp_$(PPP_VERSION)-$(PPP_RELEASE)
PPP_IPK_DIR := $(OPENWRT_IPK_DIR)/ppp
PPP_IPK := $(PPP_BUILD_DIR)_$(ARCH).ipk

PPP_RADIUS_PLUGIN_BUILD_DIR := $(BUILD_DIR)/ppp-radius-plugin_$(PPP_VERSION)-$(PPP_RELEASE)
PPP_RADIUS_PLUGIN_IPK_DIR := $(OPENWRT_IPK_DIR)/ppp-radius-plugin
PPP_RADIUS_PLUGIN_IPK := $(PPP_RADIUS_PLUGIN_BUILD_DIR)_$(ARCH).ipk


$(DL_DIR)/$(PPP_SOURCE):
	 $(WGET) -P $(DL_DIR) $(PPP_SITE)/$(PPP_SOURCE)


$(PPP_DIR)/.stamp-unpacked: $(DL_DIR)/$(PPP_SOURCE)
	$(PPP_CAT) $(DL_DIR)/$(PPP_SOURCE) | tar -C $(BUILD_DIR) -xvf -

	touch $(PPP_DIR)/.stamp-unpacked


$(PPP_DIR)/.stamp-patched: $(PPP_DIR)/.stamp-unpacked
	$(SOURCE_DIR)/patch-kernel.sh $(PPP_DIR) $(PPP_PATCH_DIR)

	touch $(PPP_DIR)/.stamp-patched


$(PPP_DIR)/.stamp-configured: $(PPP_DIR)/.stamp-patched
	cd $(PPP_DIR)/pppd/plugins/radius/radiusclient ; \
	rm -rf config.cache ; \
	$(TARGET_CONFIGURE_OPTS) \
	ac_cv_func_setvbuf_reversed=no \
	ac_cv_func_uname=no \
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
	 --mandir=/usr/share/man \
	 --infodir=/usr/share/info \
	 $(DISABLE_NLS) \
	 --disable-shared \
	 --enable-static \

	cd $(PPP_DIR) ; \
	rm -rf config.cache ; \
	$(TARGET_CONFIGURE_OPTS) \
	CFLAGS="$(TARGET_CFLAGS)" \
	./configure \
	 --target=$(GNU_TARGET_NAME) \
	 --host=$(GNU_TARGET_NAME) \
	 --build=$(GNU_HOST_NAME) \
	 --prefix=/usr \
	 --exec-prefix=/usr \
	 --bindir=/usr/bin \
	 --sbindir=/usr/sbin \
	 --libexecdir=/usr/lib \
	 --datadir=/usr/share \
	 --localstatedir=/var \
	 --mandir=/usr/share/man \
	 --infodir=/usr/share/info \
	 --sysconfdir=/etc \
	 $(DISABLE_NLS) \

	touch $(PPP_DIR)/.stamp-configured


$(PPP_DIR)/.stamp-built: $(PPP_DIR)/.stamp-configured
	cd  $(PPP_DIR) ; \
	$(MAKE) \
	 CC=$(TARGET_CC) \
	 COPTS="$(TARGET_CFLAGS)" \
	 all

	touch $(PPP_DIR)/.stamp-built


$(PPP_BUILD_DIR)/CONTROL/control: $(PPP_DIR)/.stamp-built
	rm -rf $(PPP_BUILD_DIR)
	mkdir -p $(PPP_BUILD_DIR)/usr/sbin
	cp -a $(PPP_DIR)/pppd/pppd $(PPP_BUILD_DIR)/usr/sbin/
	$(STRIP) $(PPP_BUILD_DIR)/usr/sbin/*
	cp -a $(PPP_IPK_DIR)/root/* $(PPP_BUILD_DIR)/
	chmod 0755 $(PPP_BUILD_DIR)/etc
	chmod 0755 $(PPP_BUILD_DIR)/etc/ppp
	chmod 0600 $(PPP_BUILD_DIR)/etc/ppp/chap-secrets
	chmod 0644 $(PPP_BUILD_DIR)/etc/ppp/options
	chmod 0755 $(PPP_BUILD_DIR)/etc/ppp/peers
	chmod 0755 $(PPP_BUILD_DIR)/usr
	chmod 0755 $(PPP_BUILD_DIR)/usr/sbin
	chmod 0755 $(PPP_BUILD_DIR)/usr/sbin/*
	cp -a $(PPP_IPK_DIR)/CONTROL $(PPP_BUILD_DIR)/
	perl -pi -e "s/^Vers.*:.*$$/Version: $(PPP_VERSION)-$(PPP_RELEASE)/" $(PPP_BUILD_DIR)/CONTROL/control
	perl -pi -e "s/^Arch.*:.*$$/Architecture: $(ARCH)/" $(PPP_BUILD_DIR)/CONTROL/control

	touch $(PPP_BUILD_DIR)/CONTROL/control


$(PPP_IPK): $(PPP_BUILD_DIR)/CONTROL/control
	cd $(BUILD_DIR); $(IPKG_BUILD) $(PPP_BUILD_DIR)


$(PPP_RADIUS_PLUGIN_BUILD_DIR)/CONTROL/control: $(PPP_DIR)/.stamp-built
	rm -rf $(PPP_RADIUS_PLUGIN_BUILD_DIR)
	mkdir -p $(PPP_RADIUS_PLUGIN_BUILD_DIR)/usr/lib/pppd/$(PPP_VERSION)
	cp -a $(PPP_DIR)/pppd/plugins/radius/radius.so $(PPP_RADIUS_PLUGIN_BUILD_DIR)/usr/lib/pppd/$(PPP_VERSION)/
	$(STRIP) $(PPP_RADIUS_PLUGIN_BUILD_DIR)/usr/lib/pppd/$(PPP_VERSION)/*
	cp -a $(PPP_RADIUS_PLUGIN_IPK_DIR)/root/* $(PPP_RADIUS_PLUGIN_BUILD_DIR)/
	chmod 0755 $(PPP_RADIUS_PLUGIN_BUILD_DIR)/etc
	chmod 0755 $(PPP_RADIUS_PLUGIN_BUILD_DIR)/etc/ppp
	chmod 0644 $(PPP_RADIUS_PLUGIN_BUILD_DIR)/etc/ppp/radius.conf
	chmod 0755 $(PPP_RADIUS_PLUGIN_BUILD_DIR)/etc/ppp/radius
	chmod 0644 $(PPP_RADIUS_PLUGIN_BUILD_DIR)/etc/ppp/radius/dict*
	chmod 0600 $(PPP_RADIUS_PLUGIN_BUILD_DIR)/etc/ppp/radius/servers
	chmod 0755 $(PPP_RADIUS_PLUGIN_BUILD_DIR)/usr
	chmod 0755 $(PPP_RADIUS_PLUGIN_BUILD_DIR)/usr/lib
	chmod 0755 $(PPP_RADIUS_PLUGIN_BUILD_DIR)/usr/lib/pppd
	chmod 0755 $(PPP_RADIUS_PLUGIN_BUILD_DIR)/usr/lib/pppd/$(PPP_VERSION)
	cp -a $(PPP_RADIUS_PLUGIN_IPK_DIR)/CONTROL $(PPP_RADIUS_PLUGIN_BUILD_DIR)/
	perl -pi -e "s/^Vers.*:.*$$/Version: $(PPP_VERSION)-$(PPP_RELEASE)/" $(PPP_RADIUS_PLUGIN_BUILD_DIR)/CONTROL/control
	perl -pi -e "s/^Arch.*:.*$$/Architecture: $(ARCH)/" $(PPP_RADIUS_PLUGIN_BUILD_DIR)/CONTROL/control

	touch $(PPP_RADIUS_PLUGIN_BUILD_DIR)/CONTROL/control


$(PPP_RADIUS_PLUGIN_IPK): $(PPP_RADIUS_PLUGIN_BUILD_DIR)/CONTROL/control
	cd $(BUILD_DIR); $(IPKG_BUILD) $(PPP_RADIUS_PLUGIN_BUILD_DIR)


ppp-ipk: ipkg-utils $(PPP_IPK) $(PPP_RADIUS_PLUGIN_IPK)

ppp-ipk-clean:
	rm -rf $(PPP_IPK) $(PPP_RADIUS_PLUGIN_IPK)
	rm -rf $(PPP_BUILD_DIR) $(PPP_RADIUS_PLUGIN_BUILD_DIR)

ppp-clean:
	cd $(PPP_DIR) ; \
	$(MAKE) clean

ppp-dirclean:
	rm -rf $(PPP_DIR)


