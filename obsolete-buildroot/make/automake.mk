#############################################################
#
# automake
#
#############################################################
AUTOMAKE_SOURCE:=automake-1.6.3.tar.bz2
AUTOMAKE_SITE:=ftp://ftp.gnu.org/gnu/automake
AUTOMAKE_CAT:=bzcat
AUTOMAKE_DIR:=$(BUILD_DIR)/automake-1.6.3
AUTOMAKE_BINARY:=automake
AUTOMAKE_TARGET_BINARY:=usr/bin/automake

$(DL_DIR)/$(AUTOMAKE_SOURCE):
	 $(WGET) -P $(DL_DIR) $(AUTOMAKE_SITE)/$(AUTOMAKE_SOURCE)

automake-source: $(DL_DIR)/$(AUTOMAKE_SOURCE)

$(AUTOMAKE_DIR)/.unpacked: $(DL_DIR)/$(AUTOMAKE_SOURCE)
	$(AUTOMAKE_CAT) $(DL_DIR)/$(AUTOMAKE_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(AUTOMAKE_DIR)/.unpacked

$(AUTOMAKE_DIR)/.configured: $(AUTOMAKE_DIR)/.unpacked
	(cd $(AUTOMAKE_DIR); rm -rf config.cache; \
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
	);
	touch  $(AUTOMAKE_DIR)/.configured

$(AUTOMAKE_DIR)/$(AUTOMAKE_BINARY): $(AUTOMAKE_DIR)/.configured
	$(MAKE) -C $(AUTOMAKE_DIR)
	touch -c $(AUTOMAKE_DIR)/$(AUTOMAKE_BINARY)

$(TARGET_DIR)/$(AUTOMAKE_TARGET_BINARY): $(AUTOMAKE_DIR)/$(AUTOMAKE_BINARY)
	$(MAKE) \
	    prefix=$(TARGET_DIR)/usr \
	    exec_prefix=$(TARGET_DIR)/usr \
	    bindir=$(TARGET_DIR)/usr/bin \
	    sbindir=$(TARGET_DIR)/usr/sbin \
	    libexecdir=$(TARGET_DIR)/usr/lib \
	    datadir=$(TARGET_DIR)/usr/share \
	    sysconfdir=$(TARGET_DIR)/etc \
	    localstatedir=$(TARGET_DIR)/var \
	    libdir=$(TARGET_DIR)/usr/lib \
	    infodir=$(TARGET_DIR)/usr/info \
	    mandir=$(TARGET_DIR)/usr/man \
	    includedir=$(TARGET_DIR)/usr/include \
	    -C $(AUTOMAKE_DIR) install;
	rm -rf $(TARGET_DIR)/share/locale $(TARGET_DIR)/usr/info \
		$(TARGET_DIR)/usr/man $(TARGET_DIR)/usr/share/doc
	touch -c $(TARGET_DIR)/$(AUTOMAKE_TARGET_BINARY)

automake: uclibc $(TARGET_DIR)/$(AUTOMAKE_TARGET_BINARY)

automake-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) -C $(AUTOMAKE_DIR) uninstall
	-$(MAKE) -C $(AUTOMAKE_DIR) clean

automake-dirclean:
	rm -rf $(AUTOMAKE_DIR)

