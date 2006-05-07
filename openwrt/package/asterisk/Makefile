# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=asterisk
PKG_VERSION:=1.2.1
PKG_RELEASE:=1
PKG_MD5SUM:=04657086791e80f319c0d728af705001

PKG_SOURCE_URL:=http://ftp.digium.com/pub/asterisk/old-releases/ ftp://ftp.digium.com/pub/asterisk/old-releases/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk
$(eval $(call PKG_template,ASTERISK,asterisk,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,ASTERISK_MINI,asterisk-mini,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,ASTERISK_MYSQL,asterisk-mysql,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,ASTERISK_PGSQL,asterisk-pgsql,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,ASTERISK_SQLITE,asterisk-sqlite,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,ASTERISK_VOICEMAIL,asterisk-voicemail,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,ASTERISK_SOUNDS,asterisk-sounds,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,ASTERISK_CHAN_BLUETOOTH,asterisk-chan-bluetooth,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,ASTERISK_CHAN_H323,asterisk-chan-h323,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,ASTERISK_CHAN_MGCP,asterisk-chan-mgcp,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,ASTERISK_CHAN_SKINNY,asterisk-chan-skinny,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,ASTERISK_CODEC_ILBC,asterisk-codec-ilbc,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,ASTERISK_CODEC_LPC10,asterisk-codec-lpc10,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,ASTERISK_CODEC_SPEEX,asterisk-codec-speex,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,ASTERISK_PBX_DUNDI,asterisk-pbx-dundi,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,ASTERISK_RES_AGI,asterisk-res-agi,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

EXTRA_CFLAGS := -I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include
EXTRA_LDFLAGS := -L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib

ifneq ($(BR2_PACKAGE_ASTERISK_MYSQL),)
EXTRA_CFLAGS += -I$(STAGING_DIR)/usr/include/mysql
EXTRA_LDFLAGS += -L$(STAGING_DIR)/usr/lib/mysql
EXTRA_APP_MODULES += app_sql_mysql.so
EXTRA_CDR_MODULES += cdr_mysql.so
EXTRA_RES_MODULES += res_config_mysql.so
endif
ifneq ($(BR2_PACKAGE_ASTERISK_PGSQL),)
EXTRA_APP_MODULES += app_sql_postgres.so
EXTRA_CDR_MODULES += cdr_pgsql.so
endif
ifneq ($(BR2_PACKAGE_ASTERISK_SQLITE),)
EXTRA_CDR_MODULES += cdr_sqlite.so
endif
ifneq ($(BR2_PACKAGE_ASTERISK_CHAN_BLUETOOTH),)
EXTRA_CHAN_MODULES += chan_bluetooth.so
endif
ifneq ($(BR2_PACKAGE_ASTERISK_CHAN_H323),)
EXTRA_DEFINES += \
	OPENH323DIR="$(BUILD_DIR)/openh323" \
	PWLIBDIR="$(BUILD_DIR)/pwlib" \
	CXXLIBS="-nodefaultlibs -luClibc++ -lc -lm -lgcc"
EXTRA_CHAN_MODULES += chan_h323.so
endif
ifneq ($(BR2_PACKAGE_ASTERISK_CODEC_SPEEX),)
EXTRA_CFLAGS += -I$(STAGING_DIR)/usr/include/speex
EXTRA_CODEC_MODULES += codec_speex.so
endif

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
ifneq ($(BR2_PACKAGE_ASTERISK_CHAN_H323),)
	$(MAKE) -C "$(PKG_BUILD_DIR)/channels/h323" \
		$(TARGET_CONFIGURE_OPTS) \
		$(EXTRA_DEFINES) \
		optnoshared
endif
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		CROSS_ARCH="Linux" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		CROSS_COMPILE_BIN="/void/" \
		CROSS_COMPILE_TARGET="/void/" \
		$(TARGET_CONFIGURE_OPTS) \
		HOST_CC="$(HOSTCC)" \
		OPTIMIZE="$(TARGET_CFLAGS)" \
		PROC="$(ARCH)" \
		DEBUG="" \
		OPTIONS="-DLOW_MEMORY -Dlinux" \
		NOCRYPTO="yes" \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
		EXTRA_LDFLAGS="$(EXTRA_LDFLAGS)" \
		EXTRA_APP_MODULES="$(EXTRA_APP_MODULES)" \
		EXTRA_CDR_MODULES="$(EXTRA_CDR_MODULES)" \
		EXTRA_CHAN_MODULES="$(EXTRA_CHAN_MODULES)" \
		EXTRA_CODEC_MODULES="$(EXTRA_CODEC_MODULES)" \
		EXTRA_RES_MODULES="$(EXTRA_RES_MODULES)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		ASTVARLIBDIR="/usr/lib/asterisk" \
		$(EXTRA_DEFINES) \
		all install samples
	rm -f $(PKG_INSTALL_DIR)/etc/asterisk/*.old
	touch $@

$(IPKG_ASTERISK):
	$(CP) $(PKG_INSTALL_DIR)/* $(IDIR_ASTERISK)
	rm -rf $(IDIR_ASTERISK)/usr/sbin/astgenkey
	rm -rf $(IDIR_ASTERISK)/usr/bin
	rm -rf $(IDIR_ASTERISK)/usr/share
	rm -rf $(IDIR_ASTERISK)/usr/include
	rm -rf $(IDIR_ASTERISK)/var
	(cd $(IDIR_ASTERISK)/usr/lib/asterisk; \
		rm -rf agi-bin; \
		rm -rf images; \
		rm -rf keys/*; \
		rm -rf mohmp3/*.mp3; \
		rm -rf sounds/*; \
		cd modules; \
		rm -rf	*adsi* *festival* *modem* *meetme* *oss* *phone* *intercom* \
			*mp3* *nbscat* *mysql* *postgres* *pgsql* *voicemail* *speex* \
			*zapateller* *jpeg*; \
		rm -f chan_bluetooth.so ; \
		rm -f chan_h323.so ; \
		rm -f chan_mgcp.so ; \
		rm -f chan_skinny.so ; \
		rm -f {codec,format}_ilbc.so ; \
		rm -f codec_lpc10.so ; \
		rm -f pbx_dundi.so ; \
		rm -f res_agi.so ; \
	)
	(cd $(IDIR_ASTERISK)/etc/asterisk; \
		rm -f 	*odbc* *mysql* *postgres* *pgsql* *voicemail* *adsi* *oss* *alsa* \
			*festival* *modem* *meetme* *phone* *tds* *vofr* *rpt* *vpb* \
			*zapata*; \
		rm -f bluetooth.conf ; \
		rm -f mgcp.conf ; \
		rm -f skinny.conf ; \
		rm -f dundi.conf ; \
	)
	install -m0644 ./files/modules.conf $(IDIR_ASTERISK)/etc/asterisk/
	$(SED) 's|/var/lib/asterisk|/usr/lib/asterisk|g' $(PKG_INSTALL_DIR)/etc/asterisk/musiconhold.conf
	install -d -m0755 $(IDIR_ASTERISK)/etc/default
	install -m0644  ./files/asterisk.default $(IDIR_ASTERISK)/etc/default/asterisk
	install -d -m0755 $(IDIR_ASTERISK)/etc/init.d
	install -m0755  ./files/asterisk.init $(IDIR_ASTERISK)/etc/init.d/asterisk
	ln -sf asterisk $(IDIR_ASTERISK)/etc/init.d/S60asterisk
	$(RSTRIP) $(IDIR_ASTERISK)
	$(IPKG_BUILD) $(IDIR_ASTERISK) $(PACKAGE_DIR)

$(IPKG_ASTERISK_MINI):
	install -d -m0755 $(IDIR_ASTERISK_MINI)/etc/asterisk
	for f in asterisk enum extconfig extensions features iax iaxprov logger manager modules musiconhold rtp sip; do \
	  $(CP) $(PKG_INSTALL_DIR)/etc/asterisk/$$f.conf $(IDIR_ASTERISK_MINI)/etc/asterisk/ ; \
	done
	install -m0644 ./files/modules.conf $(IDIR_ASTERISK)/etc/asterisk/
	$(SED) 's|/var/lib/asterisk|/usr/lib/asterisk|g' $(PKG_INSTALL_DIR)/etc/asterisk/musiconhold.conf
	install -d -m0755 $(IDIR_ASTERISK_MINI)/usr/lib/asterisk
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/asterisk/firmware $(IDIR_ASTERISK_MINI)/usr/lib/asterisk/
	install -d -m0755 $(IDIR_ASTERISK_MINI)/usr/lib/asterisk/keys
	install -d -m0755 $(IDIR_ASTERISK_MINI)/usr/lib/asterisk/modules
	for f in chan_iax2 chan_local chan_sip codec_gsm codec_ulaw format_gsm format_pcm format_wav \
	  format_wav_gsm pbx_config res_features res_musiconhold; do \
	  $(CP) $(PKG_INSTALL_DIR)/usr/lib/asterisk/modules/$$f.so $(IDIR_ASTERISK_MINI)/usr/lib/asterisk/modules/ ; \
	done
	install -d -m0755 $(IDIR_ASTERISK_MINI)/usr/lib/asterisk/mohmp3
	install -d -m0755 $(IDIR_ASTERISK_MINI)/usr/lib/asterisk/sounds
	install -d -m0755 $(IDIR_ASTERISK_MINI)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/asterisk $(IDIR_ASTERISK_MINI)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/safe_asterisk $(IDIR_ASTERISK_MINI)/usr/sbin/
	install -d -m0755 $(IDIR_ASTERISK_MINI)/etc/default
	install -m0644  ./files/asterisk.default $(IDIR_ASTERISK_MINI)/etc/default/asterisk
	install -d -m0755 $(IDIR_ASTERISK_MINI)/etc/init.d
	install -m0755  ./files/asterisk.init $(IDIR_ASTERISK_MINI)/etc/init.d/asterisk
	ln -sf asterisk $(IDIR_ASTERISK_MINI)/etc/init.d/S60asterisk
	$(RSTRIP) $(IDIR_ASTERISK_MINI)
	$(IPKG_BUILD) $(IDIR_ASTERISK_MINI) $(PACKAGE_DIR)

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

$(IPKG_ASTERISK_SQLITE):
	install -d -m0755 $(IDIR_ASTERISK_SQLITE)/usr/lib/asterisk/modules
	install -m0755 $(PKG_BUILD_DIR)/cdr/cdr_sqlite.so $(IDIR_ASTERISK_SQLITE)/usr/lib/asterisk/modules/
	$(RSTRIP) $(IDIR_ASTERISK_SQLITE)
	$(IPKG_BUILD) $(IDIR_ASTERISK_SQLITE) $(PACKAGE_DIR)

$(IPKG_ASTERISK_SOUNDS):
	install -d -m0755 $(IDIR_ASTERISK_SOUNDS)/usr/lib/asterisk/sounds
	$(CP) $(PKG_BUILD_DIR)/sounds/* $(IDIR_ASTERISK_SOUNDS)/usr/lib/asterisk/sounds/
	rm -f $(IDIR_ASTERISK_SOUNDS)/usr/lib/asterisk/sounds/*.mp3
	rm -f $(IDIR_ASTERISK_SOUNDS)/usr/lib/asterisk/sounds/vm-*
	$(IPKG_BUILD) $(IDIR_ASTERISK_SOUNDS) $(PACKAGE_DIR)

$(IPKG_ASTERISK_VOICEMAIL):
	install -d -m0755 $(IDIR_ASTERISK_VOICEMAIL)/etc/asterisk
	install -m0644 $(PKG_BUILD_DIR)/configs/voicemail.conf.sample $(IDIR_ASTERISK_VOICEMAIL)/etc/asterisk/voicemail.conf
	install -d -m0755 $(IDIR_ASTERISK_VOICEMAIL)/usr/lib/asterisk/modules
	install -m0755 $(PKG_BUILD_DIR)/apps/*voicemail.so $(IDIR_ASTERISK_VOICEMAIL)/usr/lib/asterisk/modules/
	install -m0755 $(PKG_BUILD_DIR)/res/res_adsi.so $(IDIR_ASTERISK_VOICEMAIL)/usr/lib/asterisk/modules/
	install -d -m0755 $(IDIR_ASTERISK_VOICEMAIL)/usr/lib/asterisk/sounds
	$(CP) $(PKG_BUILD_DIR)/sounds/vm-*.gsm $(IDIR_ASTERISK_VOICEMAIL)/usr/lib/asterisk/sounds/
	$(RSTRIP) $(IDIR_ASTERISK_VOICEMAIL)
	$(IPKG_BUILD) $(IDIR_ASTERISK_VOICEMAIL) $(PACKAGE_DIR)

$(IPKG_ASTERISK_CHAN_BLUETOOTH):
	install -d -m0755 $(IDIR_ASTERISK_CHAN_BLUETOOTH)/etc/asterisk
	install -m0644 $(PKG_BUILD_DIR)/configs/bluetooth.conf $(IDIR_ASTERISK_CHAN_BLUETOOTH)/etc/asterisk/bluetooth.conf
	install -d -m0755 $(IDIR_ASTERISK_CHAN_BLUETOOTH)/usr/lib/asterisk/modules
	install -m0755 $(PKG_BUILD_DIR)/channels/chan_bluetooth.so $(IDIR_ASTERISK_CHAN_BLUETOOTH)/usr/lib/asterisk/modules/
	$(RSTRIP) $(IDIR_ASTERISK_CHAN_BLUETOOTH)
	$(IPKG_BUILD) $(IDIR_ASTERISK_CHAN_BLUETOOTH) $(PACKAGE_DIR)

$(IPKG_ASTERISK_CHAN_H323):
	install -d -m0755 $(IDIR_ASTERISK_CHAN_H323)/etc/asterisk
	install -m0644 $(PKG_BUILD_DIR)/channels/h323/h323.conf.sample $(IDIR_ASTERISK_CHAN_H323)/etc/asterisk/h323.conf
	install -d -m0755 $(IDIR_ASTERISK_CHAN_H323)/usr/lib/asterisk/modules
	install -m0755 $(PKG_BUILD_DIR)/channels/chan_h323.so $(IDIR_ASTERISK_CHAN_H323)/usr/lib/asterisk/modules/
	$(RSTRIP) $(IDIR_ASTERISK_CHAN_H323)
	$(IPKG_BUILD) $(IDIR_ASTERISK_CHAN_H323) $(PACKAGE_DIR)

$(IPKG_ASTERISK_CHAN_MGCP):
	install -d -m0755 $(IDIR_ASTERISK_CHAN_MGCP)/etc/asterisk
	install -m0644 $(PKG_BUILD_DIR)/configs/mgcp.conf.sample $(IDIR_ASTERISK_CHAN_MGCP)/etc/asterisk/mgcp.conf
	install -d -m0755 $(IDIR_ASTERISK_CHAN_MGCP)/usr/lib/asterisk/modules
	install -m0755 $(PKG_BUILD_DIR)/channels/chan_mgcp.so $(IDIR_ASTERISK_CHAN_MGCP)/usr/lib/asterisk/modules/
	$(RSTRIP) $(IDIR_ASTERISK_CHAN_MGCP)
	$(IPKG_BUILD) $(IDIR_ASTERISK_CHAN_MGCP) $(PACKAGE_DIR)

$(IPKG_ASTERISK_CHAN_SKINNY):
	install -d -m0755 $(IDIR_ASTERISK_CHAN_SKINNY)/etc/asterisk
	install -m0644 $(PKG_BUILD_DIR)/configs/skinny.conf.sample $(IDIR_ASTERISK_CHAN_SKINNY)/etc/asterisk/skinny.conf
	install -d -m0755 $(IDIR_ASTERISK_CHAN_SKINNY)/usr/lib/asterisk/modules
	install -m0755 $(PKG_BUILD_DIR)/channels/chan_skinny.so $(IDIR_ASTERISK_CHAN_SKINNY)/usr/lib/asterisk/modules/
	$(RSTRIP) $(IDIR_ASTERISK_CHAN_SKINNY)
	$(IPKG_BUILD) $(IDIR_ASTERISK_CHAN_SKINNY) $(PACKAGE_DIR)

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

$(IPKG_ASTERISK_RES_AGI):
	install -d -m0755 $(IDIR_ASTERISK_RES_AGI)/usr/lib/asterisk/agi-bin
	install -d -m0755 $(IDIR_ASTERISK_RES_AGI)/usr/lib/asterisk/modules
	install -m0755 $(PKG_BUILD_DIR)/res/res_agi.so $(IDIR_ASTERISK_RES_AGI)/usr/lib/asterisk/modules/
	$(RSTRIP) $(IDIR_ASTERISK_RES_AGI)
	$(IPKG_BUILD) $(IDIR_ASTERISK_RES_AGI) $(PACKAGE_DIR)

