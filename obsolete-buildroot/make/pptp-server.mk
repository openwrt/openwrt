## pptp-server

PPTPD_VERSION := 1.1.3
PPTPD_RELEASE := 1

PPTPD_SOURCE := pptpd-1.1.3-20030409.tar.gz
PPTPD_SITE := http://unc.dl.sourceforge.net/sourceforge/poptop/
PPTPD_DIR := $(BUILD_DIR)/poptop
PPTPD_CAT := zcat

PPTPD_PATCH_DIR := $(SOURCE_DIR)/openwrt/patches/pptp-server

PPTPD_BUILD_DIR := $(BUILD_DIR)/pptp-server_$(PPTPD_VERSION)-$(PPTPD_RELEASE)
PPTPD_IPK_DIR := $(OPENWRT_IPK_DIR)/pptp-server
PPTPD_IPK := $(PPTPD_BUILD_DIR)_$(ARCH).ipk


$(DL_DIR)/$(PPTPD_SOURCE):
	 $(WGET) -P $(DL_DIR) $(PPTPD_SITE)/$(PPTPD_SOURCE)


$(PPTPD_DIR)/.stamp-unpacked: $(DL_DIR)/$(PPTPD_SOURCE)
	$(PPTPD_CAT) $(DL_DIR)/$(PPTPD_SOURCE) | tar -C $(BUILD_DIR) -xvf -

	touch $(PPTPD_DIR)/.stamp-unpacked


$(PPTPD_DIR)/.stamp-patched: $(PPTPD_DIR)/.stamp-unpacked
	$(SOURCE_DIR)/patch-kernel.sh $(PPTPD_DIR) $(PPTPD_PATCH_DIR)

	touch $(PPTPD_DIR)/.stamp-patched


$(PPTPD_DIR)/.stamp-configured: $(PPTPD_DIR)/.stamp-patched
	cd $(PPTPD_DIR) ; \
	rm -rf config.cache ; \
	aclocal ; \
	autoconf ; \
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
	  --infodir=/usr/share/info \
	  --mandir=/usr/share/man \
	  --localstatedir=/var \
	  --sysconfdir=/etc \
	  $(DISABLE_NLS) \
	  --with-pppd-ip-alloc \
	  
	touch $(PPTPD_DIR)/.stamp-configured


$(PPTPD_DIR)/.stamp-built: $(PPTPD_DIR)/.stamp-configured
	cd  $(PPTPD_DIR) ; \
	touch config.guess ; \
	touch config.sub ; \
	$(MAKE) \
	 CC=$(TARGET_CC) \
	 COPTS="$(TARGET_CFLAGS)" \
	 all

	touch $(PPTPD_DIR)/.stamp-built


$(PPTPD_BUILD_DIR)/CONTROL/control: $(PPTPD_DIR)/.stamp-built
	rm -rf $(PPTPD_BUILD_DIR)
	mkdir -p $(PPTPD_BUILD_DIR)/usr/sbin
	cp -a $(PPTPD_DIR)/pptpctrl $(PPTPD_BUILD_DIR)/usr/sbin/
	cp -a $(PPTPD_DIR)/pptpd $(PPTPD_BUILD_DIR)/usr/sbin/
	$(STRIP) $(PPTPD_BUILD_DIR)/usr/sbin/*
	cp -a $(PPTPD_IPK_DIR)/root/* $(PPTPD_BUILD_DIR)/
	chmod 0755 $(PPTPD_BUILD_DIR)/etc
	chmod 0755 $(PPTPD_BUILD_DIR)/etc/init.d
	chmod 0755 $(PPTPD_BUILD_DIR)/etc/init.d/*
	chmod 0755 $(PPTPD_BUILD_DIR)/etc/ppp
	chmod 0644 $(PPTPD_BUILD_DIR)/etc/ppp/pptp-server-options
	chmod 0644 $(PPTPD_BUILD_DIR)/etc/pptpd.conf
	chmod 0755 $(PPTPD_BUILD_DIR)/usr
	chmod 0755 $(PPTPD_BUILD_DIR)/usr/sbin
	chmod 0755 $(PPTPD_BUILD_DIR)/usr/sbin/*
	cp -a $(PPTPD_IPK_DIR)/CONTROL $(PPTPD_BUILD_DIR)/
	perl -pi -e "s/^Vers.*:.*$$/Version: $(PPTPD_VERSION)-$(PPTPD_RELEASE)/" $(PPTPD_BUILD_DIR)/CONTROL/control
	perl -pi -e "s/^Arch.*:.*$$/Architecture: $(ARCH)/" $(PPTPD_BUILD_DIR)/CONTROL/control

	touch $(PPTPD_BUILD_DIR)/CONTROL/control


$(PPTPD_IPK): $(PPTPD_BUILD_DIR)/CONTROL/control
	cd $(BUILD_DIR); $(IPKG_BUILD) $(PPTPD_BUILD_DIR)


pptp-server-ipk: ipkg-utils $(PPTPD_IPK)

pptp-server-clean:
	rm -rf $(PPTPD_IPK)
	rm -rf $(PPTPD_BUILD_DIR)
	rm -rf $(PPTPD_DIR)
