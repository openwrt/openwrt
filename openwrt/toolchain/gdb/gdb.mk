######################################################################
#
# gdb
#
######################################################################
GDB_VERSION:=$(strip $(GDB_VERSION))

GDB_SITE:=http://ftp.gnu.org/gnu/gdb

ifeq ($(GDB_VERSION),5.3)
GDB_SOURCE:=gdb-$(GDB_VERSION).tar.gz
GDB_CAT:=zcat
else
GDB_SOURCE:=gdb-$(GDB_VERSION).tar.bz2
GDB_CAT:=bzcat
endif

GDB_DIR:=$(TOOL_BUILD_DIR)/gdb-$(GDB_VERSION)

$(DL_DIR)/$(GDB_SOURCE):
	$(WGET) -P $(DL_DIR) $(GDB_SITE)/$(GDB_SOURCE)

$(GDB_DIR)/.unpacked: $(DL_DIR)/$(GDB_SOURCE)
	$(GDB_CAT) $(DL_DIR)/$(GDB_SOURCE) | tar -C $(TOOL_BUILD_DIR) $(TAR_OPTIONS) -
	toolchain/patch-kernel.sh $(GDB_DIR) toolchain/gdb/$(GDB_VERSION) \*.patch
	# Copy a config.sub from gcc.  This is only necessary until
	# gdb's config.sub supports <arch>-linux-uclibc tuples.
	# Should probably integrate this into the patch.
ifeq ($(GDB_VERSION),5.3)
	chmod u+x $(GDB_DIR)/gdb/gdbserver/configure
	cp $(GCC_DIR)/config.sub $(GDB_DIR)
	cp $(GCC_DIR)/config.sub $(GDB_DIR)/readline/support/
endif
	touch  $(GDB_DIR)/.unpacked

######################################################################
#
# gdb target
#
######################################################################

GDB_TARGET_DIR:=$(BUILD_DIR)/gdb-$(GDB_VERSION)-target

GDB_TARGET_CONFIGURE_VARS:= \
	ac_cv_type_uintptr_t=yes \
	gt_cv_func_gettext_libintl=yes \
	ac_cv_func_dcgettext=yes \
	gdb_cv_func_sigsetjmp=yes \
	bash_cv_func_strcoll_broken=no \
	bash_cv_must_reinstall_sighandlers=no \
	bash_cv_func_sigsetjmp=present \
	bash_cv_have_mbstate_t=yes

$(GDB_TARGET_DIR)/.configured: $(GDB_DIR)/.unpacked
	mkdir -p $(GDB_TARGET_DIR)
	(cd $(GDB_TARGET_DIR); \
		gdb_cv_func_sigsetjmp=yes \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS_FOR_TARGET="$(TARGET_CFLAGS)" \
		$(GDB_TARGET_CONFIGURE_VARS) \
		$(GDB_DIR)/configure \
		--build=$(GNU_HOST_NAME) \
		--host=$(REAL_GNU_TARGET_NAME) \
		--target=$(REAL_GNU_TARGET_NAME) \
		--prefix=/usr \
		$(DISABLE_NLS) \
		--without-uiout --disable-gdbmi \
		--disable-tui --disable-gdbtk --without-x \
		--disable-sim --enable-gdbserver \
		--without-included-gettext \
	);
ifeq ($(ENABLE_LOCALE),true)
	-$(SED) "s,^INTL *=.*,INTL = -lintl,g;" $(GDB_DIR)/gdb/Makefile
endif
	touch  $(GDB_TARGET_DIR)/.configured

$(GDB_TARGET_DIR)/gdb/gdb: $(GDB_TARGET_DIR)/.configured
	$(MAKE) CC=$(TARGET_CC) MT_CFLAGS="$(TARGET_CFLAGS)" \
		-C $(GDB_TARGET_DIR)
	$(STRIP) $(GDB_TARGET_DIR)/gdb/gdb

$(TARGET_DIR)/usr/bin/gdb: $(GDB_TARGET_DIR)/gdb/gdb
	install -c $(GDB_TARGET_DIR)/gdb/gdb $(TARGET_DIR)/usr/bin/gdb

gdb_target: ncurses-headers $(TARGET_DIR)/usr/bin/gdb

gdb_target-source: $(DL_DIR)/$(GDB_SOURCE)

gdb_target-clean:
	$(MAKE) -C $(GDB_DIR) clean

gdb_target-dirclean:
	rm -rf $(GDB_DIR)

######################################################################
#
# gdbserver
#
######################################################################

GDB_SERVER_DIR:=$(BUILD_DIR)/gdbserver-$(GDB_VERSION)

$(GDB_SERVER_DIR)/.configured: $(GDB_DIR)/.unpacked
	mkdir -p $(GDB_SERVER_DIR)
	(cd $(GDB_SERVER_DIR); \
		$(TARGET_CONFIGURE_OPTS) \
		gdb_cv_func_sigsetjmp=yes \
		$(GDB_DIR)/gdb/gdbserver/configure \
		--build=$(GNU_HOST_NAME) \
		--host=$(REAL_GNU_TARGET_NAME) \
		--target=$(REAL_GNU_TARGET_NAME) \
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
	touch  $(GDB_SERVER_DIR)/.configured

$(GDB_SERVER_DIR)/gdbserver: $(GDB_SERVER_DIR)/.configured
	$(MAKE) CC=$(TARGET_CC) MT_CFLAGS="$(TARGET_CFLAGS)" \
		-C $(GDB_SERVER_DIR)
	$(STRIP) $(GDB_SERVER_DIR)/gdbserver

$(TARGET_DIR)/usr/bin/gdbserver: $(GDB_SERVER_DIR)/gdbserver
	install -c $(GDB_SERVER_DIR)/gdbserver $(TARGET_DIR)/usr/bin/gdbserver

gdbserver: ncurses-headers $(TARGET_DIR)/usr/bin/gdbserver

gdbserver-clean:
	$(MAKE) -C $(GDB_SERVER_DIR) clean

gdbserver-dirclean:
	rm -rf $(GDB_SERVER_DIR)

######################################################################
#
# gdb client
#
######################################################################

GDB_CLIENT_DIR:=$(TOOL_BUILD_DIR)/gdbclient-$(GDB_VERSION)

$(GDB_CLIENT_DIR)/.configured: $(GDB_DIR)/.unpacked
	mkdir -p $(GDB_CLIENT_DIR)
	(cd $(GDB_CLIENT_DIR); \
		gdb_cv_func_sigsetjmp=yes \
		$(GDB_DIR)/configure \
		--prefix=$(STAGING_DIR) \
		--build=$(GNU_HOST_NAME) \
		--host=$(GNU_HOST_NAME) \
		--target=$(REAL_GNU_TARGET_NAME) \
		$(DISABLE_NLS) \
		--without-uiout --disable-gdbmi \
		--disable-tui --disable-gdbtk --without-x \
		--without-included-gettext \
		--enable-threads \
	);
	touch  $(GDB_CLIENT_DIR)/.configured

$(GDB_CLIENT_DIR)/gdb/gdb: $(GDB_CLIENT_DIR)/.configured
	$(MAKE) -C $(GDB_CLIENT_DIR)
	strip $(GDB_CLIENT_DIR)/gdb/gdb

$(TARGET_CROSS)gdb: $(GDB_CLIENT_DIR)/gdb/gdb
	install -c $(GDB_CLIENT_DIR)/gdb/gdb $(TARGET_CROSS)gdb
	ln -s ../../bin/$(REAL_GNU_TARGET_NAME)-gdb \
		$(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)/bin/gdb

gdbclient: $(TARGET_CROSS)gdb

gdbclient-clean:
	$(MAKE) -C $(GDB_CLIENT_DIR) clean

gdbclient-dirclean:
	rm -rf $(GDB_CLIENT_DIR)



