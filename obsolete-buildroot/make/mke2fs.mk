#############################################################
#
# mke2fs
#
#############################################################
MKE2FS_SOURCE=e2fsprogs-1.27.tar.gz
MKE2FS_SITE=http://aleron.dl.sourceforge.net/sourceforge/e2fsprogs
MKE2FS_DIR=$(BUILD_DIR)/e2fsprogs-1.27
MKE2FS_CAT:=zcat
MKE2FS_BINARY:=misc/mke2fs
MKE2FS_TARGET_BINARY:=sbin/mke2fs

$(DL_DIR)/$(MKE2FS_SOURCE):
	 $(WGET) -P $(DL_DIR) $(MKE2FS_SITE)/$(MKE2FS_SOURCE)

mke2fs-source: $(DL_DIR)/$(MKE2FS_SOURCE)

$(MKE2FS_DIR)/.unpacked: $(DL_DIR)/$(MKE2FS_SOURCE)
	$(MKE2FS_CAT) $(DL_DIR)/$(MKE2FS_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(MKE2FS_DIR)/.unpacked

$(MKE2FS_DIR)/.configured: $(MKE2FS_DIR)/.unpacked
	(cd $(MKE2FS_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--with-cc=$(TARGET_CC) \
		--with-linker=$(TARGET_CROSS)ld \
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
		--disable-elf-shlibs --disable-swapfs \
		--disable-debugfs --disable-imager \
		--disable-resizer --disable-fsck \
		--without-catgets $(DISABLE_NLS) \
	);
	touch  $(MKE2FS_DIR)/.configured

$(MKE2FS_DIR)/$(MKE2FS_BINARY): $(MKE2FS_DIR)/.configured
	$(MAKE) CC=$(TARGET_CC) -C $(MKE2FS_DIR)
	$(STRIP) $(MKE2FS_DIR)/misc/mke2fs $(MKE2FS_DIR)/misc/badblocks;
	touch -c $(MKE2FS_DIR)/misc/mke2fs

$(TARGET_DIR)/$(MKE2FS_TARGET_BINARY): $(MKE2FS_DIR)/$(MKE2FS_BINARY)
	#$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(MKE2FS_DIR) install
	#rm -rf $(TARGET_DIR)/share/locale $(TARGET_DIR)/usr/info \
	#	$(TARGET_DIR)/usr/man $(TARGET_DIR)/usr/share/doc
	# Only install a few selected items...
	cp -dpf $(MKE2FS_DIR)/misc/mke2fs $(TARGET_DIR)/sbin/mke2fs;
	cp -dpf $(MKE2FS_DIR)/misc/badblocks $(TARGET_DIR)/sbin/badblocks;
	touch -c $(TARGET_DIR)/sbin/mke2fs

mke2fs: uclibc $(TARGET_DIR)/$(MKE2FS_TARGET_BINARY)

mke2fs-clean:
	#$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(MKE2FS_DIR) uninstall
	rm -f $(TARGET_DIR)/sbin/mke2fs $(TARGET_DIR)/sbin/badblocks;
	-$(MAKE) -C $(MKE2FS_DIR) clean

mke2fs-dirclean:
	rm -rf $(MKE2FS_DIR)

