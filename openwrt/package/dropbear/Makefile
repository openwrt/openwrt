#############################################################
#
# dropbear
#
#############################################################
include $(TOPDIR)/rules.mk

DROPBEAR_VERSION:=0.45
DROPBEAR_SOURCE:=dropbear-$(DROPBEAR_VERSION).tar.bz2
DROPBEAR_SITE:=http://matt.ucc.asn.au/dropbear/releases/
DROPBEAR_DIR:=$(BUILD_DIR)/dropbear-$(DROPBEAR_VERSION)
DROPBEAR_CAT:=bzcat
DROPBEAR_BINARY:=dropbearmulti
DROPBEAR_TARGET_BINARY:=usr/sbin/dropbear
DROPBEAR_IPK:=dropbear_$(DROPBEAR_VERSION)-1_$(ARCH).ipk
DROPBEAR_IPK_DIR:=$(DROPBEAR_DIR)/ipkg

$(DL_DIR)/$(DROPBEAR_SOURCE):
	 $(WGET) -P $(DL_DIR) $(DROPBEAR_SITE)/$(DROPBEAR_SOURCE)

$(DROPBEAR_DIR)/.patched: $(DL_DIR)/$(DROPBEAR_SOURCE)
	$(DROPBEAR_CAT) $(DL_DIR)/$(DROPBEAR_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(DROPBEAR_DIR) ./patches \*.patch
	$(SED) 's,^/\* #define DROPBEAR_MULTI.*,#define DROPBEAR_MULTI,g' $(DROPBEAR_DIR)/options.h
	touch $(DROPBEAR_DIR)/.patched

$(DROPBEAR_DIR)/.configured: $(DROPBEAR_DIR)/.patched
	(cd $(DROPBEAR_DIR); rm -rf config.cache; \
		autoconf; \
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
		--sysconfdir=/etc \
		--datadir=/usr/share \
		--localstatedir=/var \
		--mandir=/usr/man \
		--infodir=/usr/info \
		$(DISABLE_NLS) \
		--with-shared \
	);
	touch  $(DROPBEAR_DIR)/.configured

$(DROPBEAR_DIR)/$(DROPBEAR_BINARY): $(DROPBEAR_DIR)/.configured
	$(MAKE) $(TARGET_CONFIGURE_OPTS) LD=$(TARGET_CC) \
		PROGRAMS="dropbear dbclient dropbearkey dropbearconvert scp" \
		MULTI=1 SCPPROGRESS=1 -C $(DROPBEAR_DIR)

$(PACKAGE_DIR)/$(DROPBEAR_IPK): $(DROPBEAR_DIR)/$(DROPBEAR_BINARY)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(DROPBEAR_IPK_DIR) dropbear.control $(DROPBEAR_VERSION)-1 $(ARCH)
	cp dropbear.conffiles $(DROPBEAR_IPK_DIR)/CONTROL/conffiles
	mkdir -p $(DROPBEAR_IPK_DIR)/usr/bin
	mkdir -p $(DROPBEAR_IPK_DIR)/usr/sbin
	install -m 755 $(DROPBEAR_DIR)/$(DROPBEAR_BINARY) \
		$(DROPBEAR_IPK_DIR)/$(DROPBEAR_TARGET_BINARY)
	$(STRIP) $(DROPBEAR_IPK_DIR)/usr/sbin/dropbear
	ln -sf ../sbin/dropbear $(DROPBEAR_IPK_DIR)/usr/bin/scp
	ln -sf ../sbin/dropbear $(DROPBEAR_IPK_DIR)/usr/bin/ssh
	ln -sf ../sbin/dropbear $(DROPBEAR_IPK_DIR)/usr/bin/dropbearkey
	ln -sf ../sbin/dropbear $(DROPBEAR_IPK_DIR)/usr/bin/dropbearconvert
	mkdir -p $(DROPBEAR_IPK_DIR)/etc/init.d
	cp ./files/passwd $(DROPBEAR_IPK_DIR)/etc
	cp ./files/S50dropbear $(DROPBEAR_IPK_DIR)/etc/init.d/
	chmod a+x $(DROPBEAR_IPK_DIR)/etc/init.d/S50dropbear
	$(IPKG_BUILD) $(DROPBEAR_IPK_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/dropbear.list: $(PACKAGE_DIR)/$(DROPBEAR_IPK)
	$(IPKG) install $(PACKAGE_DIR)/$(DROPBEAR_IPK)

source: $(DL_DIR)/$(DROPBEAR_SOURCE)
prepare: $(DROPBEAR_DIR)/.patched
compile: $(PACKAGE_DIR)/$(DROPBEAR_IPK)
install: $(IPKG_STATE_DIR)/info/dropbear.list

clean:
	rm -rf $(DROPBEAR_DIR)
	rm -f $(PACKAGE_DIR)/$(DROPBEAR_IPK)

