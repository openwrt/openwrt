#############################################################
#
# fakeroot
#
#############################################################
FAKEROOT_SOURCE:=fakeroot_0.7.5.tar.gz
FAKEROOT_SITE:=http://ftp.debian.org/debian/pool/main/f/fakeroot
FAKEROOT_CAT:=zcat
FAKEROOT_DIR:=$(BUILD_DIR)/fakeroot-0.7.5


$(DL_DIR)/$(FAKEROOT_SOURCE):
	 $(WGET) -P $(DL_DIR) $(FAKEROOT_SITE)/$(FAKEROOT_SOURCE)

fakeroot-source: $(DL_DIR)/$(FAKEROOT_SOURCE)

$(FAKEROOT_DIR)/.unpacked: $(DL_DIR)/$(FAKEROOT_SOURCE)
	$(FAKEROOT_CAT) $(DL_DIR)/$(FAKEROOT_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	# If using busybox getopt, make it be quiet.
	$(SED) "s,getopt --version,getopt --version 2>/dev/null," \
		$(FAKEROOT_DIR)/scripts/fakeroot
	touch $(FAKEROOT_DIR)/.unpacked

$(FAKEROOT_DIR)/.configured: $(FAKEROOT_DIR)/.unpacked
	(cd $(FAKEROOT_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/usr \
		--exec-prefix=/usr \
		--bindir=/usr/bin \
		--sbindir=/usr/sbin \
		--libdir=/usr/lib/libfakeroot \
		--sysconfdir=/etc \
		--datadir=/usr/share \
		--localstatedir=/var \
		--mandir=/usr/man \
		--infodir=/usr/info \
		$(DISABLE_NLS) \
	);
	touch  $(FAKEROOT_DIR)/.configured

$(FAKEROOT_DIR)/faked: $(FAKEROOT_DIR)/.configured
	$(MAKE) CC=$(TARGET_CC) -C $(FAKEROOT_DIR)

$(TARGET_DIR)/usr/bin/fakeroot: $(FAKEROOT_DIR)/faked
	$(MAKE) DESTDIR=$(TARGET_DIR) -C $(FAKEROOT_DIR) install
	-mv $(TARGET_DIR)/usr/bin/$(ARCH)-linux-faked $(TARGET_DIR)/usr/bin/faked 
	-mv $(TARGET_DIR)/usr/bin/$(ARCH)-linux-fakeroot $(TARGET_DIR)/usr/bin/fakeroot 
	rm -rf $(TARGET_DIR)/share/locale $(TARGET_DIR)/usr/info \
		$(TARGET_DIR)/usr/man $(TARGET_DIR)/usr/share/doc

fakeroot: uclibc $(TARGET_DIR)/usr/bin/fakeroot 

fakeroot-clean: 
	$(MAKE) -C $(FAKEROOT_DIR) clean

fakeroot-dirclean: 
	rm -rf $(FAKEROOT_DIR) 


