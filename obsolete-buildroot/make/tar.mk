#############################################################
#
# tar
#
#############################################################
GNUTAR_SOURCE:=tar-1.13.25.tar.gz
GNUTAR_SITE:=ftp://alpha.gnu.org/gnu/tar
GNUTAR_DIR:=$(BUILD_DIR)/tar-1.13.25
GNUTAR_CAT:=zcat
GNUTAR_BINARY:=src/tar
GNUTAR_TARGET_BINARY:=bin/tar

$(DL_DIR)/$(GNUTAR_SOURCE):
	 $(WGET) -P $(DL_DIR) $(GNUTAR_SITE)/$(GNUTAR_SOURCE)

tar-source: $(DL_DIR)/$(GNUTAR_SOURCE)

$(GNUTAR_DIR)/.unpacked: $(DL_DIR)/$(GNUTAR_SOURCE)
	$(GNUTAR_CAT) $(DL_DIR)/$(GNUTAR_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(GNUTAR_DIR)/.unpacked

$(GNUTAR_DIR)/.configured: $(GNUTAR_DIR)/.unpacked
	(cd $(GNUTAR_DIR); rm -rf config.cache; \
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
	touch  $(GNUTAR_DIR)/.configured

$(GNUTAR_DIR)/$(GNUTAR_BINARY): $(GNUTAR_DIR)/.configured
	$(MAKE) -C $(GNUTAR_DIR)

# This stuff is needed to work around GNU make deficiencies
tar-target_binary: $(GNUTAR_DIR)/$(GNUTAR_BINARY)
	@if [ -L $(TARGET_DIR)/$(GNUTAR_TARGET_BINARY) ] ; then \
		rm -f $(TARGET_DIR)/$(GNUTAR_TARGET_BINARY); fi;
	@if [ ! -f $(GNUTAR_DIR)/$(GNUTAR_BINARY) -o $(TARGET_DIR)/$(GNUTAR_TARGET_BINARY) \
	-ot $(GNUTAR_DIR)/$(GNUTAR_BINARY) ] ; then \
	    set -x; \
	    rm -f $(TARGET_DIR)/$(GNUTAR_TARGET_BINARY); \
	    cp -a $(GNUTAR_DIR)/$(GNUTAR_BINARY) $(TARGET_DIR)/$(GNUTAR_TARGET_BINARY); fi ;

tar: uclibc tar-target_binary

tar-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) -C $(GNUTAR_DIR) uninstall
	-$(MAKE) -C $(GNUTAR_DIR) clean

tar-dirclean:
	rm -rf $(GNUTAR_DIR)

