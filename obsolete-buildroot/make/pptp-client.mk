## pptp-client

PPTPC_VERSION := 1.5.0
PPTPC_RELEASE := 1

PPTPC_SOURCE := pptp-linux-$(PPTPC_VERSION).tar.gz
PPTPC_SITE := dl.sourceforge.net/sourceforge/pptpclient/
PPTPC_DIR := $(BUILD_DIR)/pptp-linux-$(PPTPC_VERSION)
PPTPC_CAT := zcat

PPTPC_PATCH_DIR := $(SOURCE_DIR)/openwrt/patches/pptp-client

PPTPC_BUILD_DIR := $(BUILD_DIR)/pptp-client_$(PPTPC_VERSION)-$(PPTPC_RELEASE)
PPTPC_IPK_DIR := $(OPENWRT_IPK_DIR)/pptp-client
PPTPC_IPK := $(PPTPC_BUILD_DIR)_$(ARCH).ipk


$(DL_DIR)/$(PPTPC_SOURCE):
	 $(WGET) -P $(DL_DIR) $(PPTPC_SITE)/$(PPTPC_SOURCE)


$(PPTPC_DIR)/.stamp-unpacked: $(DL_DIR)/$(PPTPC_SOURCE)
	$(PPTPC_CAT) $(DL_DIR)/$(PPTPC_SOURCE) | tar -C $(BUILD_DIR) -xvf -

	touch $(PPTPC_DIR)/.stamp-unpacked


$(PPTPC_DIR)/.stamp-patched: $(PPTPC_DIR)/.stamp-unpacked

	touch $(PPTPC_DIR)/.stamp-patched


$(PPTPC_DIR)/.stamp-configured: $(PPTPC_DIR)/.stamp-patched
	  
	touch $(PPTPC_DIR)/.stamp-configured


$(PPTPC_DIR)/.stamp-built: $(PPTPC_DIR)/.stamp-configured
	cd  $(PPTPC_DIR) ; \
	$(MAKE) \
	  CC=$(TARGET_CC) \
	  DEBUG="" \
	  OPTIMIZE="$(TARGET_CFLAGS)" \
	 all \

	touch $(PPTPC_DIR)/.stamp-built


$(PPTPC_BUILD_DIR): $(PPTPC_DIR)/.stamp-built
	mkdir -p $(PPTPC_BUILD_DIR)

	install -m0755 -d $(PPTPC_BUILD_DIR)/usr/sbin
	install -m0755 $(PPTPC_DIR)/pptp $(PPTPC_BUILD_DIR)/usr/sbin

	install -m0755 -d $(PPTPC_BUILD_DIR)/etc/ppp
	install -m0644 $(PPTPC_IPK_DIR)/root/etc/ppp/pptp-client-options $(PPTPC_BUILD_DIR)/etc/ppp/
	install -m0755 -d $(PPTPC_BUILD_DIR)/etc/ppp/peers
	install -m0644 $(PPTPC_IPK_DIR)/root/etc/ppp/peers/pptp-client-sample $(PPTPC_BUILD_DIR)/etc/ppp/peers/
	
	$(STRIP) $(PPTPC_BUILD_DIR)/usr/sbin/*

	touch $(PPTPC_DIR)/.stamp-installed


$(PPTPC_IPK): $(PPTPC_BUILD_DIR)
	cp -a $(PPTPC_IPK_DIR)/CONTROL $(PPTPC_BUILD_DIR)/
	perl -pi -e "s/^Vers.*:.*$$/Version: $(PPTPC_VERSION)-$(PPTPC_RELEASE)/" $(PPTPC_BUILD_DIR)/CONTROL/control
	perl -pi -e "s/^Arch.*:.*$$/Architecture: $(ARCH)/" $(PPTPC_BUILD_DIR)/CONTROL/control
	cd $(BUILD_DIR); $(IPKG_BUILD) $(PPTPC_BUILD_DIR)


pptp-client-ipk: ipkg-utils $(PPTPC_IPK)

pptp-client-clean:
	$(MAKE) -C $(PPTPC_DIR) clean

pptp-client-clean-all:
	rm -rf $(PPTPC_IPK)
	rm -rf $(PPTPC_BUILD_DIR)
	rm -rf $(PPTPC_DIR)
