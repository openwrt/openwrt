# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=snort-wireless
PKG_VERSION:=2.4.3-alpha04
PKG_RELEASE:=1
PKG_MD5SUM:=1aa699ae279bf7a1140cf6cca02f9999

PKG_SOURCE_URL:=http://www.snort-wireless.org/files/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

PKG_CONFIGURE_OPTS := \
	--target=$(GNU_TARGET_NAME) \
	--host=$(GNU_TARGET_NAME) \
	--build=$(GNU_HOST_NAME) \
	--program-prefix="" \
	--program-suffix="" \
	--prefix=/usr \
	--exec-prefix=/usr \
	--bindir=/usr/sbin \
	--datadir=/usr/share \
	--includedir=/usr/include \
	--infodir=/usr/share/info \
	--libdir=/usr/lib \
	--libexecdir=/usr/lib/locate \
	--localstatedir=/var/lib \
	--mandir=/usr/share/man \
	--sbindir=/usr/sbin \
	--sysconfdir=/etc \
	$(DISABLE_LARGEFILE) \
	$(DISABLE_NLS) \
	--enable-shared \
	--disable-static \
	--enable-flexresp \
	--with-libnet-includes="$(STAGING_DIR)/usr/include" \
	--with-libnet-libraries="$(STAGING_DIR)/usr/lib" \
	--with-libpcap-includes="$(STAGING_DIR)/usr/include" \
	--with-libpcap-libraries="$(STAGING_DIR)/usr/lib" \
	--with-libpcre-includes="$(STAGING_DIR)/usr/include" \
	--with-libpcre-libraries="$(STAGING_DIR)/usr/lib" \

SNORT_WIRELESS_BASIC_CONFIGURE_OPTS := \
	--without-mysql \
	--without-postgresql \

SNORT_WIRELESS_MYSQL_CONFIGURE_OPTS := \
	--with-mysql=$(STAGING_DIR)/usr \
	--without-postgresql \

SNORT_WIRELESS_PGSQL_CONFIGURE_OPTS := \
	--without-mysql \
	--with-postgresql=$(STAGING_DIR)/usr \

SNORT_WIRELESS_CUSTOM_CONFIGURE_OPTS := \

ifeq ($(BR2_PACKAGE_SNORT_WIRELESS_ENABLE_DEBUG),y)
SNORT_WIRELESS_CUSTOM_CONFIGURE_OPTS += --enable-debug
else
SNORT_WIRELESS_CUSTOM_CONFIGURE_OPTS += --disable-debug
endif

ifeq ($(BR2_PACKAGE_SNORT_WIRELESS_ENABLE_INLINE),y)
SNORT_WIRELESS_CUSTOM_CONFIGURE_OPTS += --enable-inline
SNORT_WIRELESS_CUSTOM_CONFIGURE_OPTS += --with-libipq-includes="$(STAGING_DIR)/include/libipq"
SNORT_WIRELESS_CUSTOM_CONFIGURE_OPTS += --with-libipq-libraries="$(STAGING_DIR)/lib"
else
SNORT_WIRELESS_CUSTOM_CONFIGURE_OPTS += --disable-inline
endif

ifeq ($(BR2_PACKAGE_SNORT_WIRELESS_WITH_MYSQL),y)
SNORT_WIRELESS_CUSTOM_CONFIGURE_OPTS += --with-mysql="$(STAGING_DIR)/usr"
else
SNORT_WIRELESS_CUSTOM_CONFIGURE_OPTS += --without-mysql
endif

ifeq ($(BR2_PACKAGE_SNORT_WIRELESS_WITH_PGSQL),y)
SNORT_WIRELESS_CUSTOM_CONFIGURE_OPTS += --with-postgresql="$(STAGING_DIR)/usr"
else
SNORT_WIRELESS_CUSTOM_CONFIGURE_OPTS += --without-postgresql
endif

define PKG_build

ifneq ($(BR2_PACKAGE_$(1)),)
BUILD_TARGETS += $(PKG_BUILD_DIR)/$(2)
endif

$(PKG_BUILD_DIR)/$(2): $(PKG_BUILD_DIR)/.prepared
	touch -r $(PKG_BUILD_DIR)/Makefile.am $(PKG_BUILD_DIR)/configure.in 
	touch -r $(PKG_BUILD_DIR)/Makefile.in $(PKG_BUILD_DIR)/configure
	-$(MAKE) -C $(PKG_BUILD_DIR) distclean
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/usr/include/mysql" \
		LDFLAGS="-L$(STAGING_DIR)/lib -L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/usr/lib/mysql" \
		./configure \
		  $(PKG_CONFIGURE_OPTS) \
		  $$($(1)_CONFIGURE_OPTS) \
	);
	$(MAKE) -C $(PKG_BUILD_DIR)
	mv $(PKG_BUILD_DIR)/src/snort $(PKG_BUILD_DIR)/$(2)

$$(IPKG_$(1)): $(PKG_BUILD_DIR)/.installed $(PKG_BUILD_DIR)/$(2)
	install -m0644 ./ipkg/snort-wireless.conffiles $$(IDIR_$(1))/CONTROL/conffiles
	install -d -m0755 $$(IDIR_$(1))/etc/default
	install -m0644 ./files/snort-wireless.default $$(IDIR_$(1))/etc/default/snort
	install -d -m0755 $$(IDIR_$(1))/etc/init.d
	install -m0755 ./files/snort-wireless.init $$(IDIR_$(1))/etc/init.d/snort
	install -d -m0755 $$(IDIR_$(1))/etc/snort
	install -m0644 $(PKG_BUILD_DIR)/etc/snort.conf $$(IDIR_$(1))/etc/snort/
	install -m0644 $(PKG_BUILD_DIR)/etc/classification.config $$(IDIR_$(1))/etc/snort/
	install -m0644 $(PKG_BUILD_DIR)/etc/gen-msg.map $$(IDIR_$(1))/etc/snort/
	install -m0644 $(PKG_BUILD_DIR)/etc/reference.config $$(IDIR_$(1))/etc/snort/
	install -m0644 $(PKG_BUILD_DIR)/etc/sid-msg.map $$(IDIR_$(1))/etc/snort/
	install -m0644 $(PKG_BUILD_DIR)/etc/threshold.conf $$(IDIR_$(1))/etc/snort/
	install -m0644 $(PKG_BUILD_DIR)/etc/unicode.map $$(IDIR_$(1))/etc/snort/
	install -d -m0755 $$(IDIR_$(1))/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/$(2) $$(IDIR_$(1))/usr/sbin/snort
	$(RSTRIP) $$(IDIR_$(1))
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $$(IDIR_$(1)) $(PACKAGE_DIR)

endef

$(eval $(call PKG_template,SNORT_WIRELESS_BASIC,snort-wireless,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,SNORT_WIRELESS_MYSQL,snort-wireless-mysql,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,SNORT_WIRELESS_PGSQL,snort-wireless-pgsql,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,SNORT_WIRELESS_CUSTOM,snort-wireless-custom,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(eval $(call PKG_build,SNORT_WIRELESS_BASIC,snort-wireless,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_build,SNORT_WIRELESS_MYSQL,snort-wireless-mysql,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_build,SNORT_WIRELESS_PGSQL,snort-wireless-pgsql,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_build,SNORT_WIRELESS_CUSTOM,snort-wireless-custom,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built: $(BUILD_TARGETS)
	touch $@

$(PKG_BUILD_DIR)/.installed: $(PKG_BUILD_DIR)/.built
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
	touch $@

