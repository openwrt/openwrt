# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=mpd
PKG_VERSION:=0.12.0
PKG_RELEASE:=1
PKG_MD5SUM:=65e62cc813f2186dff0f96f164a853f8

PKG_SOURCE_URL:=http://users.tpg.com.au/davico/openwrt/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

PKG_CONFIGURE_OPTIONS := \
	--disable-alsa \
	--disable-shout \
	--disable-mod \
	--disable-audiofile \
	--disable-iconv \
	--disable-mpc \
	--disable-aac \

ifneq ($(BR2_COMPILE_MPD_WITH_MP3),)
PKG_DEPENDS += libid3tag libmad
PKG_CONFIGURE_OPTIONS += \
	--with-id3tag-includes=$(STAGING_DIR)/usr/include \
	--with-id3tag-libraries=$(STAGING_DIR)/usr/lib \
	--with-mad-includes=$(STAGING_DIR)/usr/include \
	--with-mad-libraries=$(STAGING_DIR)/usr/lib
else
PKG_CONFIGURE_OPTIONS += --disable-id3 --disable-mp3
endif

ifneq ($(BR2_COMPILE_MPD_WITH_OGG),)
PKG_DEPENDS += libvorbisidec
PKG_CONFIGURE_OPTIONS += \
	--with-tremor \
	--with-tremor-includes=$(STAGING_DIR)/usr/include \
	--with-tremor-libraries=$(STAGING_DIR)/usr/lib
else
PKG_CONFIGURE_OPTIONS += --disable-ogg
endif

ifneq ($(BR2_COMPILE_MPD_WITH_FLAC),)
PKG_DEPENDS += libflac
PKG_CONFIGURE_OPTIONS += \
	--with-libFLAC-includes=$(STAGING_DIR)/usr/include \
	--with-libFLAC-libraries=$(STAGING_DIR)/usr/lib
else
PKG_CONFIGURE_OPTIONS += --disable-flac
endif

COMMA:= ,
EMPTY:=
SPACE:= $(EMPTY) $(EMPTY)
PKG_DEPENDS:=$(subst $(SPACE),$(COMMA)$(SPACE),$(sort $(PKG_DEPENDS)))

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,MPD,mpd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(strip $(TARGET_CFLAGS))" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
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
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/mpd $(IDIR_MPD)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/doc/mpdconf.example $(IDIR_MPD)/etc/mpd.conf
	echo "Depends: $(PKG_DEPENDS)" >> $(IDIR_MPD)/CONTROL/control
	$(RSTRIP) $(IDIR_MPD)
	$(IPKG_BUILD) $(IDIR_MPD) $(PACKAGE_DIR)

mostlyclean:
	make -C $(PKG_BUILD_DIR) clean
	rm $(PKG_BUILD_DIR)/.built
