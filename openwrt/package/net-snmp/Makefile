# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=net-snmp
PKG_VERSION:=5.1.2
PKG_RELEASE:=2
PKG_MD5SUM:=8080555ab3f90011f25d5122042d9a8d

PKG_SOURCE_URL:=@SF/net-snmp
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

SNMP_MIB_MODULES_INCLUDED = \
	host/hr_device \
	host/hr_disk \
	host/hr_filesys \
	host/hr_network \
	host/hr_partition \
	host/hr_proc \
	host/hr_storage \
	host/hr_system \
	mibII/at \
	mibII/icmp \
	mibII/interfaces \
	mibII/ip \
	mibII/snmp_mib \
	mibII/sysORTable \
	mibII/system_mib \
	mibII/tcp \
	mibII/udp \
	mibII/vacm_context \
	mibII/vacm_vars \
	snmpv3/snmpEngine \
	snmpv3/snmpMPDStats \
	snmpv3/usmStats \
	snmpv3/usmUser \
	snmpv3mibs \
	tunnel \
	ucd-snmp/disk \
	ucd-snmp/extensible \
	ucd-snmp/loadave \
	ucd-snmp/memory \
	ucd-snmp/pass \
	ucd-snmp/proc \
	ucd-snmp/vmstat \
	util_funcs \
	utilities/execute \

SNMP_MIB_MODULES_EXCLUDED = \
	agent_mibs \
	agentx \
	host \
	ieee802dot11 \
	mibII \
	notification \
	snmpv3mibs \
	target \
	ucd_snmp \
	utilities \

SNMP_TRANSPORTS_INCLUDED = UDP

SNMP_TRANSPORTS_EXCLUDED = Callback TCP TCPv6 UDPv6 Unix

PKG_CONFIGURE_OPTIONS = \
	--enable-shared \
	--enable-static \
	--with-endianness=little \
	--with-logfile=/var/log/snmpd.log \
	--with-persistent-directory=/usr/lib/snmp/ \
	--with-default-snmp-version=1 \
	--with-sys-contact=root@localhost \
	--with-sys-location=Unknown \
	--enable-applications \
	--disable-debugging \
	--disable-ipv6 \
	--disable-manuals \
	--disable-mib-loading \
	--disable-mibs \
	--disable-scripts \
	--with-out-mib-modules="$(SNMP_MIB_MODULES_EXCLUDED)" \
	--with-mib-modules="$(SNMP_MIB_MODULES_INCLUDED)" \
	--with-out-transports="$(SNMP_TRANSPORTS_EXCLUDED)" \
	--with-transports="$(SNMP_TRANSPORTS_INCLUDED)" \
	--without-opaque-special-types \
	--without-openssl \
	--without-libwrap \
	--without-rpm \
	--without-zlib \

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBNETSNMP,libnetsnmp,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,SNMPD,snmpd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,SNMP_UTILS,snmp-utils,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		ac_cv_header_pcap_h=no \
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
		  $(PKG_CONFIGURE_OPTIONS) \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		INSTALL_PREFIX="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_LIBNETSNMP):
	install -d -m0755 $(IDIR_LIBNETSNMP)/usr/lib
	cp -fpR $(PKG_INSTALL_DIR)/usr/lib/libnetsnmp{,agent,helpers,mibs}.so.* $(IDIR_LIBNETSNMP)/usr/lib/
	$(RSTRIP) $(IDIR_LIBNETSNMP)
	$(IPKG_BUILD) $(IDIR_LIBNETSNMP) $(PACKAGE_DIR)

$(IPKG_SNMPD):
	install -d -m0755 $(IDIR_SNMPD)/etc/snmp
	install -m0644 ./files/snmpd.conf $(IDIR_SNMPD)/etc/snmp/snmpd.conf
	install -d -m0755 $(IDIR_SNMPD)/etc/default
	install -m0644 ./files/snmpd.default $(IDIR_SNMPD)/etc/default/snmpd
	install -d -m0755 $(IDIR_SNMPD)/etc/init.d
	install -m0755 ./files/snmpd.init $(IDIR_SNMPD)/etc/init.d/snmpd
	install -d -m0755 $(IDIR_SNMPD)/usr/sbin
	cp -fpR $(PKG_INSTALL_DIR)/usr/sbin/snmpd $(IDIR_SNMPD)/usr/sbin/
	$(RSTRIP) $(IDIR_SNMPD)
	$(IPKG_BUILD) $(IDIR_SNMPD) $(PACKAGE_DIR)

$(IPKG_SNMP_UTILS):
	install -d -m0755 $(IDIR_SNMP_UTILS)/usr/bin
	cp -fpR $(PKG_INSTALL_DIR)/usr/bin/snmp{get,set,status,test,trap,walk} $(IDIR_SNMP_UTILS)/usr/bin/
	$(RSTRIP) $(IDIR_SNMP_UTILS)
	$(IPKG_BUILD) $(IDIR_SNMP_UTILS) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libnetsnmp.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/bin
	cp -fpR $(PKG_INSTALL_DIR)/usr/bin/net-snmp-config $(STAGING_DIR)/usr/bin/
	mkdir -p $(STAGING_DIR)/usr/include
	cp -fpR $(PKG_INSTALL_DIR)/usr/include/net-snmp $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	cp -fpR $(PKG_INSTALL_DIR)/usr/lib/libnetsnmp{,agent,helpers,mibs}.{a,so*} $(STAGING_DIR)/usr/lib/
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/libnetsnmp.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/bin/net-snmp-config \
		$(STAGING_DIR)/usr/include/net-snmp \
		$(STAGING_DIR)/usr/lib/libnetsnmp{,agent,helpers,mibs}.{a,so*} \

compile: install-dev
clean: uninstall-dev
