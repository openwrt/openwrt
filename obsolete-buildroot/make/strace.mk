#############################################################
#
# strace
#
#############################################################
STRACE_SOURCE:=strace-4.5.6.tar.bz2
STRACE_SITE:=http://aleron.dl.sourceforge.net/sourceforge/strace
STRACE_CAT:=bzcat
STRACE_DIR:=$(BUILD_DIR)/strace-4.5.6
STRACE_IPK=$(BUILD_DIR)/strace_4.5.6-1_mipsel.ipk
STRACE_IPK_DIR:=$(BUILD_DIR)/strace-4.5.6-ipk


$(DL_DIR)/$(STRACE_SOURCE):
	 $(WGET) -P $(DL_DIR) $(STRACE_SITE)/$(STRACE_SOURCE)

strace-source: $(DL_DIR)/$(STRACE_SOURCE)

$(STRACE_DIR)/.unpacked: $(DL_DIR)/$(STRACE_SOURCE)
	$(STRACE_CAT) $(DL_DIR)/$(STRACE_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(STRACE_DIR)/.unpacked

$(STRACE_DIR)/.configured: $(STRACE_DIR)/.unpacked
	(cd $(STRACE_DIR); rm -rf config.cache; \
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
		$(DISABLE_LARGEFILE) \
	);
	touch  $(STRACE_DIR)/.configured

$(STRACE_DIR)/strace: $(STRACE_DIR)/.configured
	$(MAKE) CC=$(TARGET_CC) -C $(STRACE_DIR)

$(TARGET_DIR)/usr/bin/strace: $(STRACE_DIR)/strace
	install -c $(STRACE_DIR)/strace $(TARGET_DIR)/usr/bin/strace
	$(STRIP) $(TARGET_DIR)/usr/bin/strace > /dev/null 2>&1

strace: uclibc $(TARGET_DIR)/usr/bin/strace 

strace-ipk: $(STRACE_DIR)/strace
	mkdir -p $(STRACE_IPK_DIR)/CONTROL
	mkdir -p $(STRACE_IPK_DIR)/usr/bin
	install -m 644 $(OPENWRT_IPK_DIR)/strace/CONTROL/control $(STRACE_IPK_DIR)/CONTROL/control
	install -m 755 $(STRACE_DIR)/strace $(STRACE_IPK_DIR)/usr/bin/
	$(STRIP) $(STRACE_IPK_DIR)/usr/bin/strace 
	cd $(BUILD_DIR); $(STAGING_DIR)/bin/ipkg-build -c -o root -g root $(STRACE_IPK_DIR)

strace-clean: 
	$(MAKE) -C $(STRACE_DIR) clean

strace-dirclean: 
	rm -rf $(STRACE_DIR) $(STRACE_IPK_DIR)
