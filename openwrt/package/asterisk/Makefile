# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=asterisk
PKG_VERSION:=1.0.7
PKG_RELEASE:=1
PKG_MD5SUM:=4cc3c1e4a1b12e0e4c748326ad153291

PKG_SOURCE_URL:=http://www.asterisk.org/html/downloads ftp://ftp.asterisk.org/pub/asterisk
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT:=zcat

include $(TOPDIR)/package/rules.mk
$(eval $(call PKG_template,ASTERISK,asterisk,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,ASTERISK_MYSQL,asterisk-mysql,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,ASTERISK_PGSQL,asterisk-pgsql,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,ASTERISK_VOICEMAIL,asterisk-voicemail,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,ASTERISK_SOUNDS,asterisk-sounds,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,ASTERISK_SPEEX,asterisk-codec-speex,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

APPS:=
MODS:=
ifneq ($(BR2_PACKAGE_ASTERISK_MYSQL),)
APPS += app_sql_mysql.so
MODS += cdr_mysql.so
endif
ifneq ($(BR2_PACKAGE_ASTERISK_PGSQL),)
APPS += app_sql_postgres.so
MODS += cdr_pgsql.so
endif
ifneq ($(BR2_PACKAGE_ASTERISK_SPEEX),)
SPEEX:=codec_speex.so
endif

asterisk-compile: $(PKG_BUILD_DIR)/.prepared
	$(MAKE) -C "$(PKG_BUILD_DIR)/channels" \
		CC="$(HOSTCC)" \
		gentone 
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		CC_FOR_BUILD="$(HOSTCC)" \
		$(TARGET_CONFIGURE_OPTS) \
		OPTIMIZE="$(TARGET_CFLAGS)" \
		PROC="$(ARCH)" \
		CFLAGS_EXTRA="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/usr/include/speex" \
		LDFLAGS_EXTRA="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/usr/lib/mysql" \
		CRYPTO_LIBS="-L$(STAGING_DIR)/usr/lib -Wl,-Bstatic -lssl -lcrypto -Wl,-Bdynamic" \
		EXTRA_APPS="$(APPS)" \
		EXTRA_MODS="$(MODS)" \
		MODSPEEX="$(SPEEX)"

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built: asterisk-compile
	touch $@

$(IPKG_ASTERISK): asterisk-compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(IDIR_ASTERISK)" \
		install samples
	rm -rf $(IDIR_ASTERISK)/usr/sbin/astgenkey
	rm -rf $(IDIR_ASTERISK)/usr/bin
	rm -rf $(IDIR_ASTERISK)/usr/share
	rm -rf $(IDIR_ASTERISK)/usr/include
	rm -rf $(IDIR_ASTERISK)/var
	(cd $(IDIR_ASTERISK)/usr/lib/asterisk; \
		rm -rf agi-bin/*; \
		rm -rf firmware; \
		rm -rf images; \
		rm -rf keys/*; \
		rm -rf mohmp3; \
		rm -rf sounds/*; \
		cd modules; \
		rm -rf	*adsi* *festival* *modem* *meetme* *oss* *phone* *intercom* \
			*mp3* *nbscat* *mysql* *postgres* *pgsql* *voicemail* *speex* \
			*musiconhold* *zapateller* *jpeg*; \
	)
	(cd $(IDIR_ASTERISK)/etc/asterisk; \
		rm -f 	*odbc* *mysql* *postgres* *pgsql* *voicemail* *adsi* *oss* *alsa* \
			*festival* *modem* *meetme* *phone* *tds* *vofr* *rpt* *vpb* \
			*zapata* *musiconhold*; \
	)
	-$(STRIP) $(IDIR_ASTERISK)/usr/sbin/asterisk
	-$(STRIP) $(IDIR_ASTERISK)/usr/lib/asterisk/modules/* 
	cp -a ./files/* $(IDIR_ASTERISK)/
	find $(IDIR_ASTERISK) -name CVS | xargs rm -rf
	chmod +x $(IDIR_ASTERISK)/etc/init.d/*
	$(IPKG_BUILD) $(IDIR_ASTERISK) $(PACKAGE_DIR)

$(IPKG_ASTERISK_MYSQL): asterisk-compile
	mkdir -p $(IDIR_ASTERISK_MYSQL)/usr/lib/asterisk/modules
	mkdir -p $(IDIR_ASTERISK_MYSQL)/etc/asterisk
	cp $(PKG_BUILD_DIR)/apps/app_sql_mysql.so $(IDIR_ASTERISK_MYSQL)/usr/lib/asterisk/modules/
	cp $(PKG_BUILD_DIR)/cdr/cdr_mysql.so $(IDIR_ASTERISK_MYSQL)/usr/lib/asterisk/modules/
	$(STRIP) $(IDIR_ASTERISK_MYSQL)/usr/lib/asterisk/modules/*
	cp $(PKG_BUILD_DIR)/configs/cdr_mysql.conf.sample $(IDIR_ASTERISK_MYSQL)/etc/asterisk/cdr_mysql.conf
	$(IPKG_BUILD) $(IDIR_ASTERISK_MYSQL) $(PACKAGE_DIR)

$(IPKG_ASTERISK_PGSQL): asterisk-compile
	mkdir -p $(IDIR_ASTERISK_PGSQL)/usr/lib/asterisk/modules
	mkdir -p $(IDIR_ASTERISK_PGSQL)/etc/asterisk
	cp $(PKG_BUILD_DIR)/apps/app_sql_postgres.so $(IDIR_ASTERISK_PGSQL)/usr/lib/asterisk/modules/
	cp $(PKG_BUILD_DIR)/cdr/cdr_pgsql.so $(IDIR_ASTERISK_PGSQL)/usr/lib/asterisk/modules/
	$(STRIP) $(IDIR_ASTERISK_PGSQL)/usr/lib/asterisk/modules/*
	cp $(PKG_BUILD_DIR)/configs/cdr_pgsql.conf.sample $(IDIR_ASTERISK_PGSQL)/etc/asterisk/cdr_pgsql.conf
	$(IPKG_BUILD) $(IDIR_ASTERISK_PGSQL) $(PACKAGE_DIR)

$(IPKG_ASTERISK_SOUNDS): asterisk-compile
	mkdir -p $(IDIR_ASTERISK_SOUNDS)/usr/lib/asterisk/sounds
	cp -a $(PKG_BUILD_DIR)/sounds/* $(IDIR_ASTERISK_SOUNDS)/usr/lib/asterisk/sounds/
	rm -f $(IDIR_ASTERISK_SOUNDS)/usr/lib/asterisk/sounds/*.mp3
	rm -f $(IDIR_ASTERISK_SOUNDS)/usr/lib/asterisk/sounds/vm-*
	$(IPKG_BUILD) $(IDIR_ASTERISK_SOUNDS) $(PACKAGE_DIR)

$(IPKG_ASTERISK_SPEEX): asterisk-compile
	mkdir -p $(IDIR_ASTERISK_SPEEX)/usr/lib/asterisk/modules
	cp $(PKG_BUILD_DIR)/codecs/*speex.so $(IDIR_ASTERISK_SPEEX)/usr/lib/asterisk/modules
	$(STRIP) $(IDIR_ASTERISK_SPEEX)/usr/lib/asterisk/modules/*
	$(IPKG_BUILD) $(IDIR_ASTERISK_SPEEX) $(PACKAGE_DIR)

$(IPKG_ASTERISK_VOICEMAIL): asterisk-compile
	mkdir -p $(IDIR_ASTERISK_VOICEMAIL)/etc/asterisk
	cp $(PKG_BUILD_DIR)/configs/voicemail.conf.sample $(IDIR_ASTERISK_VOICEMAIL)/etc/asterisk/voicemail.conf
	mkdir -p $(IDIR_ASTERISK_VOICEMAIL)/usr/lib/asterisk/modules
	cp $(PKG_BUILD_DIR)/apps/*voicemail.so $(IDIR_ASTERISK_VOICEMAIL)/usr/lib/asterisk/modules
	$(STRIP) $(IDIR_ASTERISK_VOICEMAIL)/usr/lib/asterisk/modules/*
	$(IPKG_BUILD) $(IDIR_ASTERISK_VOICEMAIL) $(PACKAGE_DIR)
