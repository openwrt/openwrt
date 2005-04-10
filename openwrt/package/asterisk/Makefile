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

PKG_APP:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_APP_DIR:=$(PKG_BUILD_DIR)/ipkg/asterisk

PKG_MYSQL:=$(PACKAGE_DIR)/$(PKG_NAME)-mysql_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_MYSQL_DIR:=$(PKG_BUILD_DIR)/ipkg/mysql

PKG_PGSQL:=$(PACKAGE_DIR)/$(PKG_NAME)-pgsql_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_PGSQL_DIR:=$(PKG_BUILD_DIR)/ipkg/pgsql

PKG_SPEEX:=$(PACKAGE_DIR)/$(PKG_NAME)-codec-speex_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_SPEEX_DIR:=$(PKG_BUILD_DIR)/ipkg/speex

PKG_SOUNDS:=$(PACKAGE_DIR)/$(PKG_NAME)-sounds_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_SOUNDS_DIR:=$(PKG_BUILD_DIR)/ipkg/sounds

PKG_VOICEMAIL:=$(PACKAGE_DIR)/$(PKG_NAME)-voicemail_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_VOICEMAIL_DIR:=$(PKG_BUILD_DIR)/ipkg/voicemail

APPS:=
MODS:=
COMPILE_TARGETS:=$(PKG_APP)
INSTALL_TARGETS:=$(IPKG_STATE_DIR)/info/$(PKG_NAME).list
ifneq ($(BR2_PACKAGE_ASTERISK_MYSQL),)
APPS += app_sql_mysql.so
MODS += cdr_mysql.so
COMPILE_TARGETS += $(PKG_MYSQL)
endif
ifneq ($(BR2_PACKAGE_ASTERISK_PGSQL),)
APPS += app_sql_postgres.so
MODS += cdr_pgsql.so
COMPILE_TARGETS += $(PKG_PGSQL)
endif
ifneq ($(BR2_PACKAGE_ASTERISK_SOUNDS),)
COMPILE_TARGETS += $(PKG_SOUNDS)
endif
ifneq ($(BR2_PACKAGE_ASTERISK_SPEEX),)
SPEEX:=codec_speex.so
COMPILE_TARGETS += $(PKG_SPEEX)
endif
ifneq ($(BR2_PACKAGE_ASTERISK_VOICEMAIL),)
COMPILE_TARGETS += $(PKG_VOICEMAIL)
endif
ifeq ($(BR2_PACKAGE_ASTERISK_MYSQL),y)
INSTALL_TARGETS:=$(IPKG_STATE_DIR)/info/$(PKG_NAME)-mysql.list
endif
ifeq ($(BR2_PACKAGE_ASTERISK_PGSQL),y)
INSTALL_TARGETS:=$(IPKG_STATE_DIR)/info/$(PKG_NAME)-pgsql.list
endif
ifeq ($(BR2_PACKAGE_ASTERISK_VOICEMAIL),y)
INSTALL_TARGETS:=$(IPKG_STATE_DIR)/info/$(PKG_NAME)-voicemail.list
endif
ifeq ($(BR2_PACKAGE_ASTERISK_SOUNDS),y)
INSTALL_TARGETS:=$(IPKG_STATE_DIR)/info/$(PKG_NAME)-sounds.list
endif
ifeq ($(BR2_PACKAGE_ASTERISK_SPEEX),y)
INSTALL_TARGETS:=$(IPKG_STATE_DIR)/info/$(PKG_NAME)-codec-speex.list
endif

$(DL_DIR)/$(PKG_SOURCE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.patched: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) ./patches
	touch $(PKG_BUILD_DIR)/.patched


asterisk-compile: $(PKG_BUILD_DIR)/.patched
	$(MAKE) -C "$(PKG_BUILD_DIR)/channels" \
		CC="$(HOSTCC)" \
		gentone 
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		CC_FOR_BUILD="$(HOSTCC)" \
		$(TARGET_CONFIGURE_OPTS) \
		OPTIMIZE="$(TARGET_CFLAGS)" \
		PROC="$(ARCH)" \
		CFLAGS_EXTRA="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS_EXTRA="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/usr/lib/mysql" \
		CRYPTO_LIBS="-L$(STAGING_DIR)/usr/lib -Wl,-Bstatic -lssl -lcrypto -Wl,-Bdynamic" \
		EXTRA_APPS="$(APPS)" \
		EXTRA_MODS="$(MODS)" \
		MODSPEEX="$(SPEEX)"
	
$(PKG_APP): asterisk-compile
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_APP_DIR) control/$(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_APP_DIR)" \
		install samples
	rm -rf $(PKG_APP_DIR)/usr/sbin/astgenkey
	rm -rf $(PKG_APP_DIR)/usr/bin
	rm -rf $(PKG_APP_DIR)/usr/share
	rm -rf $(PKG_APP_DIR)/usr/include
	rm -rf $(PKG_APP_DIR)/var
	(cd $(PKG_APP_DIR)/usr/lib/asterisk; \
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
	(cd $(PKG_APP_DIR)/etc/asterisk; \
		rm -f 	*odbc* *mysql* *postgres* *pgsql* *voicemail* *adsi* *oss* *alsa* \
			*festival* *modem* *meetme* *phone* *tds* *vofr* *rpt* *vpb* \
			*zapata* *musiconhold*; \
	)
	-$(STRIP) $(PKG_APP_DIR)/usr/sbin/asterisk
	-$(STRIP) $(PKG_APP_DIR)/usr/lib/asterisk/modules/* 
	cp -a ./files/* $(PKG_APP_DIR)/
	find $(PKG_APP_DIR) -name CVS | xargs rm -rf
	chmod +x $(PKG_APP_DIR)/etc/init.d/*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(PKG_APP_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/$(PKG_NAME).list: $(PKG_APP)
	$(IPKG) install $(PKG_APP)

$(PKG_MYSQL): asterisk-compile
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_MYSQL_DIR) control/$(PKG_NAME)-mysql.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_MYSQL_DIR)/usr/lib/asterisk/modules
	mkdir -p $(PKG_MYSQL_DIR)/etc/asterisk
	cp $(PKG_BUILD_DIR)/apps/app_sql_mysql.so $(PKG_MYSQL_DIR)/usr/lib/asterisk/modules/
	cp $(PKG_BUILD_DIR)/cdr/cdr_mysql.so $(PKG_MYSQL_DIR)/usr/lib/asterisk/modules/
	$(STRIP) $(PKG_MYSQL_DIR)/usr/lib/asterisk/modules/*
	echo /etc/asterisk/cdr_mysql.conf > $(PKG_MYSQL_DIR)/CONTROL/conffiles
	cp $(PKG_BUILD_DIR)/configs/cdr_mysql.conf.sample $(PKG_MYSQL_DIR)/etc/asterisk/cdr_mysql.conf
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(PKG_MYSQL_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/$(PKG_NAME)-mysql.list: $(PKG_MYSQL)
	$(IPKG) install $(PKG_MYSQL)

$(PKG_PGSQL): asterisk-compile
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_PGSQL_DIR) control/$(PKG_NAME)-pgsql.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_PGSQL_DIR)/usr/lib/asterisk/modules
	mkdir -p $(PKG_PGSQL_DIR)/etc/asterisk
	cp $(PKG_BUILD_DIR)/apps/app_sql_postgres.so $(PKG_PGSQL_DIR)/usr/lib/asterisk/modules/
	cp $(PKG_BUILD_DIR)/cdr/cdr_pgsql.so $(PKG_PGSQL_DIR)/usr/lib/asterisk/modules/
	$(STRIP) $(PKG_PGSQL_DIR)/usr/lib/asterisk/modules/*
	echo /etc/asterisk/cdr_pgsql.conf > $(PKG_PGSQL_DIR)/CONTROL/conffiles
	cp $(PKG_BUILD_DIR)/configs/cdr_pgsql.conf.sample $(PKG_PGSQL_DIR)/etc/asterisk/cdr_pgsql.conf
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(PKG_PGSQL_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/$(PKG_NAME)-pgsql.list: $(PKG_PGSQL)
	$(IPKG) install $(PKG_PGSQL)

$(PKG_SOUNDS): asterisk-compile
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_SOUNDS_DIR) control/$(PKG_NAME)-sounds.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_SOUNDS_DIR)/usr/lib/asterisk/sounds
	cp -a $(PKG_BUILD_DIR)/sounds/* $(PKG_SOUNDS_DIR)/usr/lib/asterisk/sounds/
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(PKG_SOUNDS_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/$(PKG_NAME)-sounds.list: $(PKG_SOUNDS)
	$(IPKG) install $(PKG_SOUNDS)

$(PKG_SPEEX): asterisk-compile
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_SPEEX_DIR) control/$(PKG_NAME)-codec-speex.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_SPEEX_DIR)/usr/lib/asterisk/modules
	cp $(PKG_BUILD_DIR)/codecs/*speex.so $(PKG_SPEEX_DIR)/usr/lib/asterisk/modules
	$(STRIP) $(PKG_SPEEX_DIR)/usr/lib/asterisk/modules/*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(PKG_SPEEX_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/$(PKG_NAME)-speex.list: $(PKG_SPEEX)
	$(IPKG) install $(PKG_SPEEX)

$(PKG_VOICEMAIL): asterisk-compile
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_VOICEMAIL_DIR) control/$(PKG_NAME)-voicemail.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	echo /etc/asterisk/voicemail.conf > $(PKG_VOICEMAIL_DIR)/CONTROL/conffiles
	mkdir -p $(PKG_VOICEMAIL_DIR)/etc/asterisk
	cp $(PKG_BUILD_DIR)/configs/voicemail.conf.sample $(PKG_VOICEMAIL_DIR)/etc/asterisk/voicemail.conf
	mkdir -p $(PKG_VOICEMAIL_DIR)/usr/lib/asterisk/modules
	cp $(PKG_BUILD_DIR)/apps/*voicemail.so $(PKG_VOICEMAIL_DIR)/usr/lib/asterisk/modules
	$(STRIP) $(PKG_VOICEMAIL_DIR)/usr/lib/asterisk/modules/*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(PKG_VOICEMAIL_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/$(PKG_NAME)-voicemail.list: $(PKG_VOICEMAIL)
	$(IPKG) install $(PKG_VOICEMAIL)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.patched
compile: $(COMPILE_TARGETS)
install: $(INSTALL_TARGETS)

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(PKG_APP)
