# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=fuse
PKG_VERSION:=2.2.1
PKG_RELEASE:=1
PKG_MD5SUM:=250d89b9c7b6ecf531df60c67f75737d

PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

ifeq ($(KERNEL_DIR),)
KERNEL_DIR:=$(LINUX_DIR)
endif

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,KMOD_FUSE,kmod-fuse,$(LINUX_VERSION)$(BOARD)+$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH),kernel ($(LINUX_VERSION)-$(BOARD)-$(KERNEL_RELEASE))))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); \
		rm -rf config.{cache,status} ; \
		touch configure.in ; \
		touch aclocal.m4 ; \
		touch Makefile.in ; \
		touch include/config.h.in ; \
		touch configure ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		./configure \
		  --target=$(GNU_TARGET_NAME) \
		  --host=$(GNU_TARGET_NAME) \
		  --build=$(GNU_HOST_NAME) \
		  --program-prefix="" \
		  --program-suffix="" \
		  --prefix=/usr \
		  --exec-prefix=/usr \
		  --bindir=/usr/bin \
		  --datadir=/usr/share \
		  --includedir=/usr/include \
		  --infodir=/usr/share/info \
		  --libdir=/usr/lib \
		  --libexecdir=/usr/lib \
		  --localstatedir=/var \
		  --mandir=/usr/share/man \
		  --sbindir=/usr/sbin \
		  --sysconfdir=/etc \
		  $(DISABLE_LARGEFILE) \
		  $(DISABLE_NLS) \
		  --enable-shared \
		  --enable-static \
		  --enable-kernel-module \
		  --enable-lib \
		  --enable-util \
		  --disable-example \
		  --disable-auto-modprobe \
		  --with-kernel=$(KERNEL_DIR) \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		SUBDIRS="kernel" \
		all install
	touch $@

$(IPKG_KMOD_FUSE):
	mkdir -p $(IDIR_KMOD_FUSE)/lib/modules/$(LINUX_VERSION)
	cp -fpR $(PKG_INSTALL_DIR)/lib/modules/$(LINUX_VERSION)/kernel/fs/fuse/fuse.* \
		$(IDIR_KMOD_FUSE)/lib/modules/$(LINUX_VERSION)/
	$(IPKG_BUILD) $(IDIR_KMOD_FUSE) $(PACKAGE_DIR)

