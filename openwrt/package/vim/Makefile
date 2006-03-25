# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=vim
PKG_VERSION:=5.8
PKG_RELEASE:=1

PKG_SOURCE_URL:=ftp://ftp.vim.org/pub/vim/unix/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-src.tar.gz
PKG_MD5SUM:=
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,VIM,vim,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR)/src; rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		ac_cv_c_bigendian=no \
		ac_cv_sizeof_off_t=8 \
		ac_cv_sizeof_int=4 \
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
		--program-prefix="" \
		$(DISABLE_NLS) \
		--enable-min-features \
		--disable-gui \
		--without-x \
		--disable-multibyte \
		--disable-cscope \
		--disable-gpm \
		--with-tlib=ncurses \
	);
	touch $@

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC)
	touch $@

$(IPKG_VIM): 
	mkdir -p $(IDIR_VIM)/usr/bin
	mkdir -p $(IDIR_VIM)//usr/share/vim
	$(CP) ./files/vimrc $(IDIR_VIM)//usr/share/vim
	$(CP) $(PKG_BUILD_DIR)/src/$(PKG_NAME) $(IDIR_VIM)/usr/bin/
	$(RSTRIP) $(IDIR_VIM)
	$(IPKG_BUILD) $(IDIR_VIM) $(PACKAGE_DIR)
