#############################################################
#
# chillispot
#
#############################################################

CHILLISPOT_VERSION:=0.96
CHILLISPOT_RELEASE:=1

CHILLISPOT_SOURCE:=chillispot-$(CHILLISPOT_VERSION).tar.gz
CHILLISPOT_SITE:=http://www.chillispot.org/download/
CHILLISPOT_CAT:=zcat
CHILLISPOT_DIR:=$(BUILD_DIR)/chillispot-$(CHILLISPOT_VERSION)
CHILLISPOT_BINARY:=src/chilli
CHILLISPOT_TARGET_BINARY:=usr/sbin/chilli

CHILLISPOT_BUILD_DIR := $(BUILD_DIR)/chillispot_$(CHILLISPOT_VERSION)-$(CHILLISPOT_RELEASE)
CHILLISPOT_IPK_DIR := $(OPENWRT_IPK_DIR)/chillispot
CHILLISPOT_IPK := $(CHILLISPOT_BUILD_DIR)_$(ARCH).ipk


$(DL_DIR)/$(CHILLISPOT_SOURCE):
	 $(WGET) -P $(DL_DIR) $(CHILLISPOT_SITE)/$(CHILLISPOT_SOURCE)

chillispot-source: $(DL_DIR)/$(CHILLISPOT_SOURCE)

$(CHILLISPOT_DIR)/.stamp-unpacked: $(DL_DIR)/$(CHILLISPOT_SOURCE)
	$(CHILLISPOT_CAT) $(DL_DIR)/$(CHILLISPOT_SOURCE) | tar -C $(BUILD_DIR) -xvf -

	touch $(CHILLISPOT_DIR)/.stamp-unpacked

$(CHILLISPOT_DIR)/.stamp-configured: $(CHILLISPOT_DIR)/.stamp-unpacked
	(cd $(CHILLISPOT_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		ac_cv_func_malloc_0_nonnull=yes \
		ac_cv_func_memcmp_working=yes \
		ac_cv_func_setvbuf_reversed=no \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/usr \
		--exec-prefix=/usr \
		--bindir=/usr/bin \
		--sbindir=/usr/sbin \
		--libexecdir=/usr/lib/locate \
		--sysconfdir=/etc \
		--datadir=/usr/share \
		--localstatedir=/var/lib \
		--mandir=/usr/man \
		--infodir=/usr/info \
		$(DISABLE_NLS) \
		$(DISABLE_LARGEFILE) \
	);
	touch  $(CHILLISPOT_DIR)/.stamp-configured

$(CHILLISPOT_DIR)/.stamp-built: $(CHILLISPOT_DIR)/.stamp-configured
	$(MAKE) CC=$(TARGET_CC) $(TARGET_CONFIGURE_OPTS) -C $(CHILLISPOT_DIR) 
	touch  $(CHILLISPOT_DIR)/.stamp-built

$(CHILLISPOT_BUILD_DIR)/CONTROL/control: $(CHILLISPOT_DIR)/.stamp-built
	rm -rf $(CHILLISPOT_BUILD_DIR)
	mkdir -p $(CHILLISPOT_BUILD_DIR)/usr/sbin
	cp -a $(CHILLISPOT_DIR)/src/chilli $(CHILLISPOT_BUILD_DIR)/usr/sbin/
	$(STRIP) $(CHILLISPOT_BUILD_DIR)/usr/sbin/*
	mkdir -p $(CHILLISPOT_BUILD_DIR)/etc
	cp -a $(CHILLISPOT_DIR)/doc/chilli.conf $(CHILLISPOT_BUILD_DIR)/etc
	cp -a $(CHILLISPOT_IPK_DIR)/root/* $(CHILLISPOT_BUILD_DIR)/
	chmod 0755 $(CHILLISPOT_BUILD_DIR)/etc
	chmod 0600 $(CHILLISPOT_BUILD_DIR)/etc/chilli.conf
	chmod 0755 $(CHILLISPOT_BUILD_DIR)/etc/init.d
	chmod 0755 $(CHILLISPOT_BUILD_DIR)/etc/init.d/*
	chmod 0755 $(CHILLISPOT_BUILD_DIR)/usr
	chmod 0755 $(CHILLISPOT_BUILD_DIR)/usr/sbin
	chmod 0755 $(CHILLISPOT_BUILD_DIR)/usr/sbin/*
	cp -a $(CHILLISPOT_IPK_DIR)/CONTROL $(CHILLISPOT_BUILD_DIR)/
	perl -pi -e "s/^Vers.*:.*$$/Version: $(CHILLISPOT_VERSION)-$(CHILLISPOT_RELEASE)/" $(CHILLISPOT_BUILD_DIR)/CONTROL/control
	perl -pi -e "s/^Arch.*:.*$$/Architecture: $(ARCH)/" $(CHILLISPOT_BUILD_DIR)/CONTROL/control

	touch $(CHILLISPOT_BUILD_DIR)/CONTROL/control


$(CHILLISPOT_IPK): $(CHILLISPOT_BUILD_DIR)/CONTROL/control
	cd $(BUILD_DIR); $(IPKG_BUILD) $(CHILLISPOT_BUILD_DIR)


chillispot-ipk: ipkg-utils $(CHILLISPOT_IPK)

chillispot-clean:
	$(MAKE) -C $(CHILLISPOT_DIR) clean

chillispot-dirclean:
	rm -rf $(CHILLISPOT_DIR)
