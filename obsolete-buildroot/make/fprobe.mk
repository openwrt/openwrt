## fprobe

FPROBE_VERSION:=1.0.5
FPROBE_RELEASE:=1

FPROBE_SOURCE:=fprobe-$(FPROBE_VERSION).tar.bz2
FPROBE_SITE:=http://dl.sourceforge.net/sourceforge/fprobe/
FPROBE_DIR:=$(BUILD_DIR)/fprobe-$(FPROBE_VERSION)
FPROBE_CAT:=bzcat

FPROBE_PATCH_DIR := $(SOURCE_DIR)/openwrt/patches/fprobe

FPROBE_BUILD_DIR := $(BUILD_DIR)/fprobe_$(FPROBE_VERSION)-$(FPROBE_RELEASE)
FPROBE_IPK_DIR := $(OPENWRT_IPK_DIR)/fprobe
FPROBE_IPK := $(FPROBE_BUILD_DIR)_$(ARCH).ipk


$(DL_DIR)/$(FPROBE_SOURCE):
	 $(WGET) -P $(DL_DIR) $(FPROBE_SITE)/$(FPROBE_SOURCE)


$(FPROBE_DIR)/.stamp-unpacked: $(DL_DIR)/$(FPROBE_SOURCE)
	$(FPROBE_CAT) $(DL_DIR)/$(FPROBE_SOURCE) | tar -C $(BUILD_DIR) -xvf -

	touch $(FPROBE_DIR)/.stamp-unpacked


$(FPROBE_DIR)/.stamp-configured: $(FPROBE_DIR)/.stamp-unpacked
	cd $(FPROBE_DIR) ; \
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
	  --libexecdir=/usr/lib/locate \
	  --sysconfdir=/etc \
	  --datadir=/usr/share \
	  --localstatedir=/var/lib \
	  --mandir=/usr/share/man \
	  --infodir=/usr/share/info \
	  --includedir=/usr/include \
	  --libdir=/usr/lib \
	  $(DISABLE_NLS) \
	  --with-membulk=index8 \
	  --with-hash=xor8

	touch $(FPROBE_DIR)/.stamp-configured


$(FPROBE_DIR)/.stamp-built: $(FPROBE_DIR)/.stamp-configured
	cd $(FPROBE_DIR) ; \
	$(MAKE) \
	  CC=$(TARGET_CC) \

	touch $(FPROBE_DIR)/.stamp-built


$(FPROBE_DIR)/.stamp-installed: $(FPROBE_DIR)/.stamp-built
	mkdir -p $(FPROBE_BUILD_DIR)
	cd $(FPROBE_DIR) ; \
	$(MAKE) \
	  DESTDIR="$(FPROBE_BUILD_DIR)" \
	 install \
	 
	install -m0755 -d $(FPROBE_BUILD_DIR)/etc
	install -m0644 $(FPROBE_IPK_DIR)/root/etc/fprobe.conf $(FPROBE_BUILD_DIR)/etc/

	install -m0755 -d $(FPROBE_BUILD_DIR)/etc/init.d
	install -m0755 $(FPROBE_IPK_DIR)/root/etc/init.d/fprobe $(FPROBE_BUILD_DIR)/etc/init.d/

	rm -rf $(FPROBE_BUILD_DIR)/usr/share

	$(STRIP) $(FPROBE_BUILD_DIR)/usr/sbin/*
	
	touch $(FPROBE_DIR)/.stamp-installed


$(FPROBE_IPK): $(FPROBE_DIR)/.stamp-installed
	cp -a $(FPROBE_IPK_DIR)/CONTROL $(FPROBE_BUILD_DIR)/
	perl -pi -e "s/^Vers.*:.*$$/Version: $(FPROBE_VERSION)-$(FPROBE_RELEASE)/" $(FPROBE_BUILD_DIR)/CONTROL/control
	perl -pi -e "s/^Arch.*:.*$$/Architecture: $(ARCH)/" $(FPROBE_BUILD_DIR)/CONTROL/control
	cd $(BUILD_DIR); $(IPKG_BUILD) $(FPROBE_BUILD_DIR)


fprobe-source: $(DL_DIR)/$(FPROBE_SOURCE)

fprobe-ipk: ipkg-utils libpcap-ipk $(FPROBE_IPK)

fprobe-clean:
	$(MAKE) -C $(FPROBE_DIR) clean

fprobe-clean-all:
	rm -rf $(FPROBE_DIR)
	rm -rf $(FPROBE_BUILD_DIR)
	rm -rf $(FPROBE_IPK)
