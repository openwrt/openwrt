# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=asterisk
PKG_VERSION:=1.0.9
PKG_RELEASE:=1
PKG_MD5SUM:=0d7f1c680ae53a739f26a069ef8b580a

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
$(eval $(call PKG_template,ASTERISK_CODEC_ILBC,asterisk-codec-ilbc,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,ASTERISK_CODEC_LPC10,asterisk-codec-lpc10,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,ASTERISK_CODEC_SPEEX,asterisk-codec-speex,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,ASTERISK_PBX_DUNDI,asterisk-pbx-dundi,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

ifneq ($(DEVELOPER),)
APPS:=app_sql_mysql.so app_sql_postgres.so
MODS:=cdr_mysql.so cdr_pgsql.so
SPEEX:=codec_speex.so
else
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
ifneq ($(BR2_PACKAGE_ASTERISK_CODEC_SPEEX),)
SPEEX:=codec_speex.so
endif
endif

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
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
	touch $@

$(IPKG_ASTERISK):
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
		rm -f {codec,format}_ilbc.so ; \
		rm -f codec_lpc10.so ; \
		rm -f pbx_dundi.so ; \
	)
	(cd $(IDIR_ASTERISK)/etc/asterisk; \
		rm -f 	*odbc* *mysql* *postgres* *pgsql* *voicemail* *adsi* *oss* *alsa* \
			*festival* *modem* *meetme* *phone* *tds* *vofr* *rpt* *vpb* \
			*zapata* *musiconhold*; \
		rm -f dundi.conf ; \
	)
	install -d -m0755 $(IDIR_ASTERISK)/etc/default
	install -m0644  ./files/asterisk.default $(IDIR_ASTERISK)/etc/default/asterisk
	install -d -m0755 $(IDIR_ASTERISK)/etc/init.d
	install -m0755  ./files/asterisk.init $(IDIR_ASTERISK)/etc/init.d/asterisk
	ln -sf asterisk $(IDIR_ASTERISK)/etc/init.d/S60asterisk
	$(RSTRIP) $(IDIR_ASTERISK)
	$(IPKG_BUILD) $(IDIR_ASTERISK) $(PACKAGE_DIR)

$(IPKG_ASTERISK_MYSQL):
	install -d -m0755 $(IDIR_ASTERISK_MYSQL)/etc/asterisk
	install -m0600 $(PKG_BUILD_DIR)/configs/cdr_mysql.conf.sample $(IDIR_ASTERISK_MYSQL)/etc/asterisk/cdr_mysql.conf
	install -d -m0755 $(IDIR_ASTERISK_MYSQL)/usr/lib/asterisk/modules
	install -m0755 $(PKG_BUILD_DIR)/apps/app_sql_mysql.so $(IDIR_ASTERISK_MYSQL)/usr/lib/asterisk/modules/
	install -m0755 $(PKG_BUILD_DIR)/cdr/cdr_mysql.so $(IDIR_ASTERISK_MYSQL)/usr/lib/asterisk/modules/
	$(RSTRIP) $(IDIR_ASTERISK_MYSQL)
	$(IPKG_BUILD) $(IDIR_ASTERISK_MYSQL) $(PACKAGE_DIR)

$(IPKG_ASTERISK_PGSQL):
	install -d -m0755 $(IDIR_ASTERISK_PGSQL)/etc/asterisk
	install -m0600 $(PKG_BUILD_DIR)/configs/cdr_pgsql.conf.sample $(IDIR_ASTERISK_PGSQL)/etc/asterisk/cdr_pgsql.conf
	install -d -m0755 $(IDIR_ASTERISK_PGSQL)/usr/lib/asterisk/modules
	install -m0755 $(PKG_BUILD_DIR)/apps/app_sql_postgres.so $(IDIR_ASTERISK_PGSQL)/usr/lib/asterisk/modules/
	install -m0755 $(PKG_BUILD_DIR)/cdr/cdr_pgsql.so $(IDIR_ASTERISK_PGSQL)/usr/lib/asterisk/modules/
	$(RSTRIP) $(IDIR_ASTERISK_PGSQL)
	$(IPKG_BUILD) $(IDIR_ASTERISK_PGSQL) $(PACKAGE_DIR)

$(IPKG_ASTERISK_SOUNDS):
	install -d -m0755 $(IDIR_ASTERISK_SOUNDS)/usr/lib/asterisk/sounds
	cp -fpR $(PKG_BUILD_DIR)/sounds/* $(IDIR_ASTERISK_SOUNDS)/usr/lib/asterisk/sounds/
	rm -f $(IDIR_ASTERISK_SOUNDS)/usr/lib/asterisk/sounds/*.mp3
	rm -f $(IDIR_ASTERISK_SOUNDS)/usr/lib/asterisk/sounds/vm-*
	$(IPKG_BUILD) $(IDIR_ASTERISK_SOUNDS) $(PACKAGE_DIR)

$(IPKG_ASTERISK_VOICEMAIL):
	install -d -m0755 $(IDIR_ASTERISK_VOICEMAIL)/etc/asterisk
	install -m0644 $(PKG_BUILD_DIR)/configs/voicemail.conf.sample $(IDIR_ASTERISK_VOICEMAIL)/etc/asterisk/voicemail.conf
	install -d -m0755 $(IDIR_ASTERISK_VOICEMAIL)/usr/lib/asterisk/modules
	install -m0755 $(PKG_BUILD_DIR)/apps/*voicemail.so $(IDIR_ASTERISK_VOICEMAIL)/usr/lib/asterisk/modules/
	$(RSTRIP) $(IDIR_ASTERISK_VOICEMAIL)
	$(IPKG_BUILD) $(IDIR_ASTERISK_VOICEMAIL) $(PACKAGE_DIR)

$(IPKG_ASTERISK_CODEC_ILBC):
	install -d -m0755 $(IDIR_ASTERISK_CODEC_ILBC)/usr/lib/asterisk/modules
	install -m0755 $(PKG_BUILD_DIR)/codecs/codec_ilbc.so $(IDIR_ASTERISK_CODEC_ILBC)/usr/lib/asterisk/modules/
	install -m0755 $(PKG_BUILD_DIR)/formats/format_ilbc.so $(IDIR_ASTERISK_CODEC_ILBC)/usr/lib/asterisk/modules/
	$(RSTRIP) $(IDIR_ASTERISK_CODEC_ILBC)
	$(IPKG_BUILD) $(IDIR_ASTERISK_CODEC_ILBC) $(PACKAGE_DIR)

$(IPKG_ASTERISK_CODEC_LPC10):
	install -d -m0755 $(IDIR_ASTERISK_CODEC_LPC10)/usr/lib/asterisk/modules
	install -m0755 $(PKG_BUILD_DIR)/codecs/codec_lpc10.so $(IDIR_ASTERISK_CODEC_LPC10)/usr/lib/asterisk/modules/
	$(RSTRIP) $(IDIR_ASTERISK_CODEC_LPC10)
	$(IPKG_BUILD) $(IDIR_ASTERISK_CODEC_LPC10) $(PACKAGE_DIR)

$(IPKG_ASTERISK_CODEC_SPEEX):
	install -d -m0755 $(IDIR_ASTERISK_CODEC_SPEEX)/usr/lib/asterisk/modules
	install -m0755 $(PKG_BUILD_DIR)/codecs/codec_speex.so $(IDIR_ASTERISK_CODEC_SPEEX)/usr/lib/asterisk/modules/
	$(RSTRIP) $(IDIR_ASTERISK_CODEC_SPEEX)
	$(IPKG_BUILD) $(IDIR_ASTERISK_CODEC_SPEEX) $(PACKAGE_DIR)

$(IPKG_ASTERISK_PBX_DUNDI):
	install -d -m0755 $(IDIR_ASTERISK_PBX_DUNDI)/etc/asterisk
	install -m0644 $(PKG_BUILD_DIR)/configs/dundi.conf.sample $(IDIR_ASTERISK_PBX_DUNDI)/etc/asterisk/dundi.conf
	install -d -m0755 $(IDIR_ASTERISK_PBX_DUNDI)/usr/lib/asterisk/modules
	install -m0755 $(PKG_BUILD_DIR)/pbx/pbx_dundi.so $(IDIR_ASTERISK_PBX_DUNDI)/usr/lib/asterisk/modules/
	$(RSTRIP) $(IDIR_ASTERISK_PBX_DUNDI)
	$(IPKG_BUILD) $(IDIR_ASTERISK_PBX_DUNDI) $(PACKAGE_DIR)

