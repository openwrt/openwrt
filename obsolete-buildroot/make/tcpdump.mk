## tcpdump

TCPDUMP_VERSION:=3.8.3
TCPDUMP_RELEASE:=1

TCPDUMP_SOURCE:=tcpdump-$(TCPDUMP_VERSION).tar.gz
TCPDUMP_SITE:=http://www.tcpdump.org/release/
TCPDUMP_DIR:=$(BUILD_DIR)/tcpdump-$(TCPDUMP_VERSION)
TCPDUMP_CAT:=zcat

TCPDUMP_PATCH_DIR := $(SOURCE_DIR)/openwrt/patches/tcpdump

TCPDUMP_BUILD_DIR := $(BUILD_DIR)/tcpdump_$(TCPDUMP_VERSION)-$(TCPDUMP_RELEASE)
TCPDUMP_IPK_DIR := $(OPENWRT_IPK_DIR)/tcpdump
TCPDUMP_IPK := $(TCPDUMP_BUILD_DIR)_$(ARCH).ipk


$(DL_DIR)/$(TCPDUMP_SOURCE):
	 $(WGET) -P $(DL_DIR) $(TCPDUMP_SITE)/$(TCPDUMP_SOURCE)


$(TCPDUMP_DIR)/.stamp-unpacked: $(DL_DIR)/$(TCPDUMP_SOURCE)
	$(TCPDUMP_CAT) $(DL_DIR)/$(TCPDUMP_SOURCE) | tar -C $(BUILD_DIR) -xvf -

	touch $(TCPDUMP_DIR)/.stamp-unpacked


$(TCPDUMP_DIR)/.stamp-patched: $(TCPDUMP_DIR)/.stamp-unpacked
	$(SOURCE_DIR)/patch-kernel.sh $(TCPDUMP_DIR) $(TCPDUMP_PATCH_DIR)
	$(SOURCE_DIR)/patch-kernel.sh $(TCPDUMP_DIR) $(TCPDUMP_DIR)/debian/patches *patch

	touch $(TCPDUMP_DIR)/.stamp-patched


$(TCPDUMP_DIR)/.stamp-configured: $(TCPDUMP_DIR)/.stamp-patched
	cd $(TCPDUMP_DIR) ; \
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

	touch $(TCPDUMP_DIR)/.stamp-configured


$(TCPDUMP_DIR)/.stamp-built: $(TCPDUMP_DIR)/.stamp-configured
	cd $(TCPDUMP_DIR) ; \
	$(MAKE) \
	  CC=$(TARGET_CC) \
	  CCOPT="$(TARGET_OPTIMIZATION)" \
	  INCLS="-I. -I$(srcdir)/missing" \
	  LDFLAGS="-lpcap" \
	  LIBS="" \

	touch $(TCPDUMP_DIR)/.stamp-built


$(TCPDUMP_DIR)/.stamp-installed: $(TCPDUMP_DIR)/.stamp-built
	mkdir -p $(TCPDUMP_BUILD_DIR)
	cd $(TCPDUMP_DIR) ; \
	$(MAKE) \
	  DESTDIR="$(TCPDUMP_BUILD_DIR)" \
	 install \
	 
	rm -rf $(TCPDUMP_BUILD_DIR)/usr/share

	$(STRIP) $(TCPDUMP_BUILD_DIR)/usr/sbin/*
	
	touch $(TCPDUMP_DIR)/.stamp-installed


$(TCPDUMP_IPK): $(TCPDUMP_DIR)/.stamp-installed
	cp -a $(TCPDUMP_IPK_DIR)/CONTROL $(TCPDUMP_BUILD_DIR)/
	perl -pi -e "s/^Vers.*:.*$$/Version: $(TCPDUMP_VERSION)-$(TCPDUMP_RELEASE)/" $(TCPDUMP_BUILD_DIR)/CONTROL/control
	perl -pi -e "s/^Arch.*:.*$$/Architecture: $(ARCH)/" $(TCPDUMP_BUILD_DIR)/CONTROL/control
	cd $(BUILD_DIR); $(IPKG_BUILD) $(TCPDUMP_BUILD_DIR)


tcpdump-source: $(DL_DIR)/$(TCPDUMP_SOURCE)

# libpcap-clean prevent configure to find a local libpcap library
tcpdump-ipk: ipkg-utils libpcap-ipk libpcap-clean $(TCPDUMP_IPK)

tcpdump-clean:
	$(MAKE) -C $(TCPDUMP_DIR) clean

tcpdump-clean-all:
	rm -rf $(TCPDUMP_DIR)
	rm -rf $(TCPDUMP_BUILD_DIR)
	rm -rf $(TCPDUMP_IPK)
