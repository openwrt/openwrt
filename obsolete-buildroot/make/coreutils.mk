#############################################################
#
# coreutils
#
#############################################################
COREUTILS_SOURCE:=coreutils-5.0.tar.bz2
COREUTILS_SITE:=ftp://ftp.gnu.org/gnu/coreutils/
COREUTILS_CAT:=bzcat
COREUTILS_DIR:=$(BUILD_DIR)/coreutils-5.0
COREUTILS_BINARY:=src/vdir
COREUTILS_TARGET_BINARY:=bin/vdir
BIN_PROGS:=cat chgrp chmod chown cp date dd df dir echo false hostname \
	ln ls mkdir mknod mv pwd rm rmdir vdir sleep stty sync touch true uname

$(DL_DIR)/$(COREUTILS_SOURCE):
	 $(WGET) -P $(DL_DIR) $(COREUTILS_SITE)/$(COREUTILS_SOURCE)

coreutils-source: $(DL_DIR)/$(COREUTILS_SOURCE)

$(COREUTILS_DIR)/.unpacked: $(DL_DIR)/$(COREUTILS_SOURCE)
	$(COREUTILS_CAT) $(DL_DIR)/$(COREUTILS_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(COREUTILS_DIR)/.unpacked

$(COREUTILS_DIR)/.configured: $(COREUTILS_DIR)/.unpacked
	(cd $(COREUTILS_DIR); rm -rf config.cache; \
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
		--disable-rpath \
		--disable-dependency-tracking \
	);
	#Fix up the max number of open files per process, which apparently 
	# is not set when cross compiling
	$(SED) 's,.*UTILS_OPEN_MAX.*,#define UTILS_OPEN_MAX 1019,g' \
		$(COREUTILS_DIR)/config.h
	# This is undefined when crosscompiling...
	$(SED) 's,.*HAVE_PROC_UPTIME.*,#define HAVE_PROC_UPTIME 1,g' \
		$(COREUTILS_DIR)/config.h
	touch  $(COREUTILS_DIR)/.configured

$(COREUTILS_DIR)/$(COREUTILS_BINARY): $(COREUTILS_DIR)/.configured
	$(MAKE) CC=$(TARGET_CC) -C $(COREUTILS_DIR)
	rm -f $(TARGET_DIR)/$(COREUTILS_TARGET_BINARY)

$(TARGET_DIR)/$(COREUTILS_TARGET_BINARY): $(COREUTILS_DIR)/$(COREUTILS_BINARY)
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(COREUTILS_DIR) install
	# some things go in root rather than usr
	for f in $(BIN_PROGS); do \
		mv $(TARGET_DIR)/usr/bin/$$f $(TARGET_DIR)/bin/$$f; \
	done
	# link for archaic shells
	ln -fs test $(TARGET_DIR)/usr/bin/[
	# gnu thinks chroot is in bin, debian thinks it's in sbin
	mv $(TARGET_DIR)/usr/bin/chroot $(TARGET_DIR)/usr/sbin/chroot
	$(STRIP) $(TARGET_DIR)/usr/sbin/chroot > /dev/null 2>&1
	rm -rf $(TARGET_DIR)/share/locale $(TARGET_DIR)/usr/info \
		$(TARGET_DIR)/usr/man $(TARGET_DIR)/usr/share/doc

coreutils: uclibc $(TARGET_DIR)/$(COREUTILS_TARGET_BINARY)

coreutils-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(COREUTILS_DIR) uninstall
	-$(MAKE) -C $(COREUTILS_DIR) clean

coreutils-dirclean:
	rm -rf $(COREUTILS_DIR)

