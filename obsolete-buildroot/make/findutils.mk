#############################################################
#
# findutils
#
#############################################################
FINDUTILS_SOURCE:=findutils_4.1.7.orig.tar.gz
FINDUTILS_SITE:=http://ftp.debian.org/debian/pool/main/f/findutils
FINDUTILS_CAT:=zcat
FINDUTILS_DIR:=$(BUILD_DIR)/findutils-4.1.7
FINDUTILS_BINARY:=find/find
FINDUTILS_TARGET_BINARY:=usr/bin/find

$(DL_DIR)/$(FINDUTILS_SOURCE):
	 $(WGET) -P $(DL_DIR) $(FINDUTILS_SITE)/$(FINDUTILS_SOURCE)

findutils-source: $(DL_DIR)/$(FINDUTILS_SOURCE)

$(FINDUTILS_DIR)/.unpacked: $(DL_DIR)/$(FINDUTILS_SOURCE)
	$(FINDUTILS_CAT) $(DL_DIR)/$(FINDUTILS_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	mv $(FINDUTILS_DIR).orig $(FINDUTILS_DIR)
	touch $(FINDUTILS_DIR)/.unpacked

$(FINDUTILS_DIR)/.configured: $(FINDUTILS_DIR)/.unpacked
	(cd $(FINDUTILS_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
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
	touch  $(FINDUTILS_DIR)/.configured

$(FINDUTILS_DIR)/$(FINDUTILS_BINARY): $(FINDUTILS_DIR)/.configured
	$(MAKE) CC=$(TARGET_CC) -C $(FINDUTILS_DIR)

# This stuff is needed to work around GNU make deficiencies
findutils-target_binary: $(FINDUTILS_DIR)/$(FINDUTILS_BINARY)
	@if [ -L $(TARGET_DIR)/$(FINDUTILS_TARGET_BINARY) ] ; then \
		rm -f $(TARGET_DIR)/$(FINDUTILS_TARGET_BINARY); fi;
	@if [ ! -f $(FINDUTILS_DIR)/$(FINDUTILS_BINARY) -o $(TARGET_DIR)/$(FINDUTILS_TARGET_BINARY) \
	-ot $(FINDUTILS_DIR)/$(FINDUTILS_BINARY) ] ; then \
	    set -x; \
	    $(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(FINDUTILS_DIR) install; \
	    $(STRIP) $(TARGET_DIR)/usr/lib/locate/* > /dev/null 2>&1; \
	    rm -rf $(TARGET_DIR)/share/locale $(TARGET_DIR)/usr/info \
		$(TARGET_DIR)/usr/man $(TARGET_DIR)/usr/share/doc; fi;

findutils: uclibc findutils-target_binary

findutils-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(FINDUTILS_DIR) uninstall
	-$(MAKE) -C $(FINDUTILS_DIR) clean

findutils-dirclean:
	rm -rf $(FINDUTILS_DIR)

