# vsftpd

VSFTPD_VERSION:=1.2.2
VSFTPD_RELEASE:=1

VSFTPD_SOURCE:=vsftpd-$(VSFTPD_VERSION).tar.gz
VSFTPD_SITE:=ftp://vsftpd.beasts.org/users/cevans/
VSFTPD_DIR:=$(BUILD_DIR)/vsftpd-$(VSFTPD_VERSION)
VSFTPD_CAT:=zcat

VSFTPD_PATCH_DIR := $(SOURCE_DIR)/openwrt/patches/vsftpd

VSFTPD_BUILD_DIR := $(BUILD_DIR)/vsftpd_$(VSFTPD_VERSION)-$(VSFTPD_RELEASE)
VSFTPD_IPK_DIR := $(OPENWRT_IPK_DIR)/vsftpd
VSFTPD_IPK := $(VSFTPD_BUILD_DIR)_$(ARCH).ipk


$(DL_DIR)/$(VSFTPD_SOURCE):
	 $(WGET) -P $(DL_DIR) $(VSFTPD_SITE)/$(VSFTPD_SOURCE)


$(VSFTPD_DIR)/.stamp-unpacked: $(DL_DIR)/$(VSFTPD_SOURCE)
	$(VSFTPD_CAT) $(DL_DIR)/$(VSFTPD_SOURCE) | tar -C $(BUILD_DIR) -xvf -

	touch $(VSFTPD_DIR)/.stamp-unpacked


$(VSFTPD_DIR)/.stamp-patched: $(VSFTPD_DIR)/.stamp-unpacked
	$(SOURCE_DIR)/patch-kernel.sh $(VSFTPD_DIR) $(VSFTPD_PATCH_DIR)

	touch  $(VSFTPD_DIR)/.stamp-patched


$(VSFTPD_DIR)/.stamp-built: $(VSFTPD_DIR)/.stamp-patched
	cd $(VSFTPD_DIR) ; \
	$(MAKE) \
	  $(TARGET_CONFIGURE_OPTS) \
	  CFLAGS="$(TARGET_CFLAGS)" \
	 
	touch  $(VSFTPD_DIR)/.stamp-built


$(VSFTPD_BUILD_DIR): $(VSFTPD_DIR)/.stamp-built
	mkdir -p $(VSFTPD_BUILD_DIR)

	install -m0755 -d $(VSFTPD_BUILD_DIR)/usr/sbin
	install -m0755 $(VSFTPD_DIR)/vsftpd $(VSFTPD_BUILD_DIR)/usr/sbin/

	install -m0755 -d $(VSFTPD_BUILD_DIR)/etc
	install -m0644 $(VSFTPD_IPK_DIR)/root/etc/vsftpd.conf $(VSFTPD_BUILD_DIR)/etc/
	install -m0755 -d $(VSFTPD_BUILD_DIR)/etc/init.d
	install -m0755 $(VSFTPD_IPK_DIR)/root/etc/init.d/vsftpd $(VSFTPD_BUILD_DIR)/etc/init.d/

	$(STRIP) $(VSFTPD_BUILD_DIR)/usr/sbin/*


$(VSFTPD_IPK): $(VSFTPD_BUILD_DIR)
	cp -a $(VSFTPD_IPK_DIR)/CONTROL $(VSFTPD_BUILD_DIR)/
	perl -pi -e "s/^Vers.*:.*$$/Version: $(VSFTPD_VERSION)-$(VSFTPD_RELEASE)/" $(VSFTPD_BUILD_DIR)/CONTROL/control
	perl -pi -e "s/^Arch.*:.*$$/Architecture: $(ARCH)/" $(VSFTPD_BUILD_DIR)/CONTROL/control
	cd $(BUILD_DIR); $(IPKG_BUILD) $(VSFTPD_BUILD_DIR)


vsftpd-source: $(DL_DIR)/$(VSFTPD_SOURCE)

vsftpd-ipk: ipkg-utils $(VSFTPD_IPK)

vsftpd-clean:
	rm -rf $(VSFTPD_DIR)
	rm -rf $(VSFTPD_BUILD_DIR)
	rm -rf $(VSFTPD_IPK)
