## libpcap

LIBPCAP_VERSION:=0.8.3
LIBPCAP_RELEASE:=1

LIBPCAP_SOURCE:=libpcap-$(LIBPCAP_VERSION).tar.gz
LIBPCAP_SITE:=http://www.tcpdump.org/release/
LIBPCAP_DIR:=$(BUILD_DIR)/libpcap-$(LIBPCAP_VERSION)
LIBPCAP_CAT:=zcat

LIBPCAP_PATCH_DIR := $(SOURCE_DIR)/openwrt/patches/libpcap

LIBPCAP_BUILD_DIR := $(BUILD_DIR)/libpcap_$(LIBPCAP_VERSION)-$(LIBPCAP_RELEASE)
LIBPCAP_IPK_DIR := $(OPENWRT_IPK_DIR)/libpcap
LIBPCAP_IPK := $(LIBPCAP_BUILD_DIR)_$(ARCH).ipk


$(DL_DIR)/$(LIBPCAP_SOURCE):
	 $(WGET) -P $(DL_DIR) $(LIBPCAP_SITE)/$(LIBPCAP_SOURCE)


$(LIBPCAP_DIR)/.stamp-unpacked: $(DL_DIR)/$(LIBPCAP_SOURCE)
	$(LIBPCAP_CAT) $(DL_DIR)/$(LIBPCAP_SOURCE) | tar -C $(BUILD_DIR) -xvf -

	touch $(LIBPCAP_DIR)/.stamp-unpacked


$(LIBPCAP_DIR)/.stamp-patched: $(LIBPCAP_DIR)/.stamp-unpacked
	$(SOURCE_DIR)/patch-kernel.sh $(LIBPCAP_DIR) $(LIBPCAP_PATCH_DIR)
	$(SOURCE_DIR)/patch-kernel.sh $(LIBPCAP_DIR) $(LIBPCAP_DIR)/debian/patches *patch

	touch $(LIBPCAP_DIR)/.stamp-patched


$(LIBPCAP_DIR)/.stamp-configured: $(LIBPCAP_DIR)/.stamp-patched
	cd $(LIBPCAP_DIR) ; \
	rm -rf config.cache ; \
	$(TARGET_CONFIGURE_OPTS) \
	CFLAGS="$(TARGET_CFLAGS)" \
	ac_cv_linux_vers="2" \
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
	  --mandir=/usr/share/man \
	  --infodir=/usr/share/info \
	  --includedir=/usr/include \
	  --libdir=/usr/lib \
	  $(DISABLE_NLS) \
	  --enable-shared \
	  --disable-static \
	  --with-pcap=linux \

	touch $(LIBPCAP_DIR)/.stamp-configured


$(LIBPCAP_DIR)/.stamp-built: $(LIBPCAP_DIR)/.stamp-configured
	cd $(LIBPCAP_DIR) ; \
	$(MAKE) \
	  CC=$(TARGET_CC) \
	  CCOPT="$(TARGET_OPTIMIZATION)" \

	touch $(LIBPCAP_DIR)/.stamp-built


$(LIBPCAP_DIR)/.stamp-installed: $(LIBPCAP_DIR)/.stamp-built
	mkdir -p $(LIBPCAP_BUILD_DIR)
	cd $(LIBPCAP_DIR) ; \
	$(MAKE) \
	  DESTDIR="$(LIBPCAP_BUILD_DIR)" \
	 install \
	 
	install -m0644 $(LIBPCAP_BUILD_DIR)/usr/include/pcap*.h $(STAGING_DIR)/include/
	install -m0644 $(LIBPCAP_BUILD_DIR)/usr/lib/libpcap.a $(STAGING_DIR)/lib/
	install -m0755 $(LIBPCAP_BUILD_DIR)/usr/lib/libpcap.so* $(STAGING_DIR)/lib/

	rm -rf $(LIBPCAP_BUILD_DIR)/usr/share
	rm -rf $(LIBPCAP_BUILD_DIR)/usr/include
	rm -rf $(LIBPCAP_BUILD_DIR)/usr/lib/*.a
	rm -rf $(LIBPCAP_BUILD_DIR)/usr/lib/*.so

	$(STRIP) $(LIBPCAP_BUILD_DIR)/usr/lib/libpcap.so.*
	
	touch $(LIBPCAP_DIR)/.stamp-installed


$(LIBPCAP_IPK): $(LIBPCAP_DIR)/.stamp-installed
	cp -a $(LIBPCAP_IPK_DIR)/CONTROL $(LIBPCAP_BUILD_DIR)/
	perl -pi -e "s/^Vers.*:.*$$/Version: $(LIBPCAP_VERSION)-$(LIBPCAP_RELEASE)/" $(LIBPCAP_BUILD_DIR)/CONTROL/control
	perl -pi -e "s/^Arch.*:.*$$/Architecture: $(ARCH)/" $(LIBPCAP_BUILD_DIR)/CONTROL/control
	cd $(BUILD_DIR); $(IPKG_BUILD) $(LIBPCAP_BUILD_DIR)


libpcap-source: $(DL_DIR)/$(LIBPCAP_SOURCE)

libpcap-ipk: ipkg-utils $(LIBPCAP_IPK)

libpcap-clean:
	$(MAKE) -C $(LIBPCAP_DIR) clean

libpcap-clean-all:
	rm -rf $(LIBPCAP_DIR)
	rm -rf $(LIBPCAP_BUILD_DIR)
	rm -rf $(LIBPCAP_IPK)
	rm -rf $(STAGING_DIR)/include/pcap*.h
	rm -rf $(STAGING_DIR)/lib/libpcap.a
	rm -rf $(STAGING_DIR)/lib/libpcap.so*
