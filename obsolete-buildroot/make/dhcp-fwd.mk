## dhcp-fwd

DHCP_FWD_VERSION := 0.7
DHCP_FWD_RELEASE := 1

DHCP_FWD_SOURCE := dhcp-forwarder-$(DHCP_FWD_VERSION).tar.bz2
DHCP_FWD_SITE := http://www-user.tu-chemnitz.de/~ensc/dhcp-fwd/files/
DHCP_FWD_DIR := $(BUILD_DIR)/dhcp-forwarder-$(DHCP_FWD_VERSION)
DHCP_FWD_CAT := bzcat

DHCP_FWD_BUILD_DIR := $(BUILD_DIR)/dhcp-fwd_$(DHCP_FWD_VERSION)-$(DHCP_FWD_RELEASE)
DHCP_FWD_IPK_DIR := $(OPENWRT_IPK_DIR)/dhcp-fwd
DHCP_FWD_IPK := $(DHCP_FWD_BUILD_DIR)_$(ARCH).ipk


$(DL_DIR)/$(DHCP_FWD_SOURCE):
	 $(WGET) -P $(DL_DIR) $(DHCP_FWD_SITE)/$(DHCP_FWD_SOURCE)


$(DHCP_FWD_DIR)/.stamp-unpacked: $(DL_DIR)/$(DHCP_FWD_SOURCE)
	$(DHCP_FWD_CAT) $(DL_DIR)/$(DHCP_FWD_SOURCE) | tar -C $(BUILD_DIR) -xvf -

	touch $(DHCP_FWD_DIR)/.stamp-unpacked


$(DHCP_FWD_DIR)/.stamp-configured: $(DHCP_FWD_DIR)/.stamp-unpacked
	cd $(DHCP_FWD_DIR) ; \
	rm -rf config.cache ; \
	$(TARGET_CONFIGURE_OPTS) \
	CFLAGS="$(TARGET_CFLAGS)" \
	ac_cv_func_malloc_0_nonnull="yes" \
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
	  
	touch $(DHCP_FWD_DIR)/.stamp-configured


$(DHCP_FWD_DIR)/.stamp-built: $(DHCP_FWD_DIR)/.stamp-configured
	cd  $(DHCP_FWD_DIR) ; \
	touch config.guess ; \
	touch config.sub ; \
	$(MAKE) \
	  CC=$(TARGET_CC) \
	  cfg_filename="/etc/dhcp-fwd.conf" \
	 all \

	touch $(DHCP_FWD_DIR)/.stamp-built


$(DHCP_FWD_BUILD_DIR)/CONTROL/control: $(DHCP_FWD_DIR)/.stamp-built
	rm -rf $(DHCP_FWD_BUILD_DIR)
	mkdir -p $(DHCP_FWD_BUILD_DIR)/usr/sbin
	cp -a $(DHCP_FWD_DIR)/dhcp-fwd $(DHCP_FWD_BUILD_DIR)/usr/sbin/
	$(STRIP) $(DHCP_FWD_BUILD_DIR)/usr/sbin/*
	cp -a $(DHCP_FWD_IPK_DIR)/root/* $(DHCP_FWD_BUILD_DIR)/
	chmod 0755 $(DHCP_FWD_BUILD_DIR)/etc
	chmod 0755 $(DHCP_FWD_BUILD_DIR)/etc/init.d
	chmod 0755 $(DHCP_FWD_BUILD_DIR)/etc/init.d/*
	chmod 0644 $(DHCP_FWD_BUILD_DIR)/etc/dhcp-fwd.conf
	chmod 0755 $(DHCP_FWD_BUILD_DIR)/usr
	chmod 0755 $(DHCP_FWD_BUILD_DIR)/usr/sbin
	chmod 0755 $(DHCP_FWD_BUILD_DIR)/usr/sbin/*
	cp -a $(DHCP_FWD_IPK_DIR)/CONTROL $(DHCP_FWD_BUILD_DIR)/
	perl -pi -e "s/^Vers.*:.*$$/Version: $(DHCP_FWD_VERSION)-$(DHCP_FWD_RELEASE)/" $(DHCP_FWD_BUILD_DIR)/CONTROL/control
	perl -pi -e "s/^Arch.*:.*$$/Architecture: $(ARCH)/" $(DHCP_FWD_BUILD_DIR)/CONTROL/control

	touch $(DHCP_FWD_BUILD_DIR)/CONTROL/control


$(DHCP_FWD_IPK): $(DHCP_FWD_BUILD_DIR)/CONTROL/control
	cd $(BUILD_DIR); $(IPKG_BUILD) $(DHCP_FWD_BUILD_DIR)


dhcp-fwd-ipk: ipkg-utils $(DHCP_FWD_IPK)

dhcp-fwd-clean:
	rm -rf $(DHCP_FWD_IPK)
	rm -rf $(DHCP_FWD_BUILD_DIR)
	rm -rf $(DHCP_FWD_DIR)
