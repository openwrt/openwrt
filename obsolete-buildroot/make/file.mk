#############################################################
#
# file
#
#############################################################
FILE_SOURCE:=file-4.08.tar.gz
FILE_SITE:=ftp://ftp.astron.com/pub/file
FILE_DIR:=$(BUILD_DIR)/file-4.08
FILE_CAT:=zcat
FILE_BINARY:=src/file
FILE_TARGET_BINARY:=usr/bin/file

$(DL_DIR)/$(FILE_SOURCE):
	 $(WGET) -P $(DL_DIR) $(FILE_SITE)/$(FILE_SOURCE)

file-source: $(DL_DIR)/$(FILE_SOURCE)

$(FILE_DIR)/.unpacked: $(DL_DIR)/$(FILE_SOURCE)
	$(FILE_CAT) $(DL_DIR)/$(FILE_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	cat $(SOURCE_DIR)/file.patch | patch -p1 -d $(FILE_DIR)
	touch  $(FILE_DIR)/.unpacked

$(FILE_DIR)/.configured: $(FILE_DIR)/.unpacked
	(cd $(FILE_DIR); rm -rf config.cache; \
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
		--datadir=/usr/share/misc \
		--localstatedir=/var \
		--mandir=/usr/man \
		--infodir=/usr/info \
		$(DISABLE_NLS) \
		$(DISABLE_LARGEFILE) \
		--disable-fsect-man5 \
	);
	touch  $(FILE_DIR)/.configured

$(FILE_DIR)/$(FILE_BINARY): $(FILE_DIR)/.configured
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(FILE_DIR)

$(TARGET_DIR)/$(FILE_TARGET_BINARY): $(FILE_DIR)/$(FILE_BINARY)
	$(MAKE) $(TARGET_CONFIGURE_OPTS) DESTDIR=$(TARGET_DIR) -C $(FILE_DIR) install
	-($(STRIP) $(TARGET_DIR)/usr/lib/libmagic.so.*.* > /dev/null 2>&1)
	rm -rf $(TARGET_DIR)/share/locale $(TARGET_DIR)/usr/info \
		$(TARGET_DIR)/usr/man $(TARGET_DIR)/usr/share/doc

file: zlib uclibc $(TARGET_DIR)/$(FILE_TARGET_BINARY)

file-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(FILE_DIR) uninstall
	-$(MAKE) -C $(FILE_DIR) clean

file-dirclean:
	rm -rf $(FILE_DIR)

