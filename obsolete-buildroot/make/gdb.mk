#############################################################
#
# gdb
#
#############################################################

GDB_SITE:=ftp://ftp.gnu.org/gnu/gdb/
GDB_DIR:=$(BUILD_DIR)/gdb-5.3
GDB_SOURCE:=gdb-5.3.tar.gz
GDB_PATCH:=$(SOURCE_DIR)/gdb.patch
GDB_UCLIBC_PATCH:=$(SOURCE_DIR)/gdb-5.3-uclibc.patch

$(DL_DIR)/$(GDB_SOURCE):
	$(WGET) -P $(DL_DIR) $(GDB_SITE)/$(GDB_SOURCE)

$(GDB_DIR)/.unpacked: $(DL_DIR)/$(GDB_SOURCE) $(GDB_PATCH)
	gunzip -c $(DL_DIR)/$(GDB_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	cat $(GDB_PATCH) | patch -p1 -d $(GDB_DIR)
	cat $(GDB_UCLIBC_PATCH) | patch -p1 -d $(GDB_DIR)
	touch  $(GDB_DIR)/.unpacked

$(GDB_DIR)/.configured: $(GDB_DIR)/.unpacked
	# Copy a config.sub from gcc.  This is only necessary until
	# gdb's config.sub supports <arch>-linux-uclibc tuples.
	cp $(GCC_DIR)/config.sub $(GDB_DIR)
	cp $(GCC_DIR)/config.sub $(GDB_DIR)/readline/support/
	(cd $(GDB_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		ac_cv_type_uintptr_t=yes \
		gt_cv_func_gettext_libintl=yes \
		ac_cv_func_dcgettext=yes \
		gdb_cv_func_sigsetjmp=yes \
		bash_cv_func_strcoll_broken=no \
		bash_cv_must_reinstall_sighandlers=no \
		bash_cv_func_sigsetjmp=present \
		./configure \
		--target=$(REAL_GNU_TARGET_NAME) \
		--host=$(REAL_GNU_TARGET_NAME) \
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
		--disable-sim --enable-gdbserver \
		--without-included-gettext \
	);
ifeq ($(ENABLE_LOCALE),true)
	-$(SED) "s,^INTL *=.*,INTL = -lintl,g;" $(GDB_DIR)/gdb/Makefile
endif
	touch  $(GDB_DIR)/.configured

$(GDB_DIR)/gdb/gdb: $(GDB_DIR)/.configured
	$(MAKE) CC=$(TARGET_CC) -C $(GDB_DIR)
	$(STRIP) $(GDB_DIR)/gdb/gdb

$(TARGET_DIR)/usr/bin/gdb: $(GDB_DIR)/gdb/gdb
	install -c $(GDB_DIR)/gdb/gdb $(TARGET_DIR)/usr/bin/gdb
	rm -rf $(TARGET_DIR)/share/locale $(TARGET_DIR)/usr/info \
		$(TARGET_DIR)/usr/man $(TARGET_DIR)/usr/share/doc

gdb: $(TARGET_DIR)/usr/bin/gdb

gdb-source: $(DL_DIR)/$(GDB_SOURCE)

gdb-clean: 
	$(MAKE) -C $(GDB_DIR) clean

gdb-dirclean: 
	rm -rf $(GDB_DIR)

