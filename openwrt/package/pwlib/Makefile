# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=pwlib
PKG_VERSION:=cvs-20051227
PKG_RELEASE:=1
PKG_MD5SUM:=7d5b8e9ab61a05658a5630f91505420d

PKG_SOURCE_URL:=http://trash.uid0.hu/openwrt
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,PWLIB,pwlib,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	( cd $(PKG_BUILD_DIR); rm -rf config.{cache,status}; \
		CFLAGS="$(TARGET_CFLAGS)" \
		CXXFLAGS="$(TARGET_CFLAGS) -fno-builtin -fno-rtti"  \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		$(TARGET_CONFIGURE_OPTS) \
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
			\
			--enable-minsize \
			--enable-openh323 \
			--disable-opal \
			--disable-internalregex \
			--disable-plugins \
			--enable-audio \
			--disable-alsa \
			--disable-asn \
			--disable-avc \
			--disable-dc \
			--disable-dtmf \
			--disable-expat \
			--disable-ftp \
			--disable-http \
			--disable-httpsvc \
			--disable-ipv6 \
			--disable-jabber \
			--disable-openldap \
			--disable-openssl \
			--disable-oss \
			--disable-pipechan \
			--disable-pop3smtp \
			--disable-remconn \
			--disable-resolver \
			--disable-qos \
			--disable-sasl \
			--disable-sdl \
			--disable-serial \
			--disable-shm-video \
			--disable-snmp \
			--disable-soap \
			--disable-socks \
			--disable-stun \
			--disable-telnet \
			--disable-tts \
			--disable-v4l \
			--disable-v4l2 \
			--disable-bsdvideo \
			--disable-video \
			--disable-vxml \
			--disable-wavfile \
			--disable-xmlrpc \
	);
	ln -sf $(PKG_NAME)-$(PKG_VERSION) $(BUILD_DIR)/$(PKG_NAME)
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		optnoshared
	touch $@

uninstall-link:
	rm -f $(BUILD_DIR)/$(PKG_NAME)

compile-targets: $(PKG_BUILD_DIR)/.built
clean-targets: uninstall-link
