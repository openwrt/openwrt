# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=mpd
PKG_VERSION:=-uclinux--1--patch-10.normalperson-05
PKG_RELEASE:=1
PKG_MD5SUM:=c707bb24a4acaa7c6a07fd6e4cda1f7c

PKG_SOURCE_URL:=http://mpd.bogomips.org/mpd--uclinux
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

PKG_CONFIGURE_OPTIONS := \
	--disable-http \
	--with-audio=oss \
	--disable-alsa \
	--disable-shout \
	--disable-id3 \
	--disable-mod \
	--disable-audiofile \
	--enable-static --disable-shared \
	--enable-flac --enable-mpd-flac \
	--disable-sse --disable-3dnow \
	--enable-uclinux

ifneq ($(BR2_PACKAGE_MPD_MP3),)
PKG_CONFIGURE_OPTIONS += \
	--enable-mp3 --enable-mpd-mad
else
PKG_CONFIGURE_OPTIONS += --disable-mp3
endif

ifneq ($(BR2_PACKAGE_MPD_OGG),)
PKG_CONFIGURE_OPTIONS += \
	--enable-ogg --enable-mpd-ivorbis
else
PKG_CONFIGURE_OPTIONS += --disable-ogg
endif

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,MPD,mpd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/usr \
		--sysconfdir=/etc \
		$(PKG_CONFIGURE_OPTIONS) \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_MPD):
	install -d -m0755 $(IDIR_MPD)/usr/bin
	install -d -m0755 $(IDIR_MPD)/etc
	cp -fp $(PKG_INSTALL_DIR)/usr/bin/mpd $(IDIR_MPD)/usr/bin
	cp -fp $(PKG_BUILD_DIR)/doc/mpdconf.example $(IDIR_MPD)/etc/mpd.conf
	$(RSTRIP) $(IDIR_MPD)
	$(IPKG_BUILD) $(IDIR_MPD) $(PACKAGE_DIR)

mostlyclean:
	make -C $(PKG_BUILD_DIR) clean
	rm $(PKG_BUILD_DIR)/.built
