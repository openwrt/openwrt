#############################################################
#
# gdbserver
#
#############################################################

#Use GDB_DIR/etc values from gdb.mk
#Build gdbserver in a dir outside of the main gdb tree
GDB_WDIR:=$(BUILD_DIR)/gdbserver


$(GDB_WDIR)/.configured: $(GDB_DIR)/.unpacked
	mkdir -p $(GDB_WDIR)
	(cd $(GDB_WDIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		$(GDB_DIR)/gdb/gdbserver/configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
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
		--includedir=$(STAGING_DIR)/include \
		$(DISABLE_NLS) \
		--without-uiout --disable-gdbmi \
		--disable-tui --disable-gdbtk --without-x \
		--without-included-gettext \
	);
	touch  $(GDB_WDIR)/.configured

$(GDB_WDIR)/gdbserver: $(GDB_WDIR)/.configured
	$(MAKE) CC=$(TARGET_CC) -C $(GDB_WDIR)
	$(STRIP) $(GDB_WDIR)/gdbserver

$(TARGET_DIR)/usr/bin/gdbserver: $(GDB_WDIR)/gdbserver
	install -c $(GDB_WDIR)/gdbserver $(TARGET_DIR)/usr/bin/gdbserver
	rm -rf $(TARGET_DIR)/share/locale $(TARGET_DIR)/usr/info \
		$(TARGET_DIR)/usr/man $(TARGET_DIR)/usr/share/doc

gdbserver: $(TARGET_DIR)/usr/bin/gdbserver

gdbserver-clean: 
	$(MAKE) -C $(GDB_WDIR) clean

gdbserver-dirclean: 
	rm -rf $(GDB_WDIR)

