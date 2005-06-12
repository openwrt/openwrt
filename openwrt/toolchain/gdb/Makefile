# $Id$

include $(TOPDIR)/rules.mk

GDB_NAME:=gdb
GDB_SITE:=http://ftp.gnu.org/gnu/gdb
GDB_VERSION:=6.3
GDB_SOURCE:=$(GDB_NAME)-$(GDB_VERSION).tar.bz2
GDB_CAT:=bzcat

GDB_DIR:=$(TOOL_BUILD_DIR)/$(GDB_NAME)-$(GDB_VERSION)

$(DL_DIR)/$(GDB_SOURCE):
	$(WGET) -P $(DL_DIR) $(GDB_SITE)/$(GDB_SOURCE)

$(GDB_DIR)/.patched: $(DL_DIR)/$(GDB_SOURCE)
	$(GDB_CAT) $(DL_DIR)/$(GDB_SOURCE) | tar -C $(TOOL_BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(GDB_DIR) ./patches
	# Copy a config.sub from gcc.  This is only necessary until
	# gdb's config.sub supports <arch>-linux-uclibc tuples.
	# Should probably integrate this into the patch.
	touch  $(GDB_DIR)/.patched

# gdb client

GDB_CLIENT_DIR:=$(TOOL_BUILD_DIR)/gdbclient-$(GDB_VERSION)

$(GDB_CLIENT_DIR)/.configured: $(GDB_DIR)/.patched
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
	cd $(STAGING_DIR)/bin && \
	ln -fs $(TARGET_CROSS)gdb $(GNU_TARGET_NAME)-gdb

source: $(DL_DIR)/$(GDB_SOURCE)
prepare: $(GDB_DIR)/.patched
compile: 
install: $(TARGET_CROSS)gdb
clean:
	rm -rf $(GDB_CLIENT_DIR)
