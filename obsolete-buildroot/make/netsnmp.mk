#############################################################
#
# netsnmp
#
#############################################################

NETSNMP_URL:=http://aleron.dl.sourceforge.net/sourceforge/net-snmp/
NETSNMP_DIR:=$(BUILD_DIR)/net-snmp-5.1
NETSNMP_SOURCE:=net-snmp-5.1.tar.gz
NETSNMP_PATCH1:=net-snmp_5.1-5.diff.gz
NETSNMP_PATCH1_URL:=http://ftp.debian.org/debian/pool/main/n/net-snmp/
NETSNMP_PATCH2:=$(SOURCE_DIR)/netsnmp.patch

$(DL_DIR)/$(NETSNMP_SOURCE):
	$(WGET) -P $(DL_DIR) $(NETSNMP_URL)/$(NETSNMP_SOURCE)

$(DL_DIR)/$(NETSNMP_PATCH1):
	$(WGET) -P $(DL_DIR) $(NETSNMP_PATCH1_URL)/$(NETSNMP_PATCH1)

$(NETSNMP_DIR)/.unpacked: $(DL_DIR)/$(NETSNMP_SOURCE) $(DL_DIR)/$(NETSNMP_PATCH1)
	zcat $(DL_DIR)/$(NETSNMP_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	zcat $(DL_DIR)/$(NETSNMP_PATCH1) | patch -p1 -d $(NETSNMP_DIR)
	cat $(NETSNMP_PATCH2) | patch -p1 -d $(NETSNMP_DIR)
	touch  $(NETSNMP_DIR)/.unpacked

# We set CAN_USE_SYSCTL to no and use /proc since the
# sysctl code in this thing is apparently intended for
# freebsd or some such thing...
$(NETSNMP_DIR)/.configured: $(NETSNMP_DIR)/.unpacked
	(cd $(NETSNMP_DIR); autoconf; \
		ac_cv_CAN_USE_SYSCTL=no \
		PATH=$(TARGET_PATH) \
		./configure \
		--with-cc=$(TARGET_CROSS)gcc \
		--with-ar=$(TARGET_CROSS)ar \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--with-endianness=little \
		--with-persistent-directory=/var/lib/snmp \
		--enable-ucd-snmp-compatibility \
		--enable-shared \
		--disable-static \
		--with-logfile=none \
		--without-rpm \
		--with-openssl \
		--without-dmalloc \
		--without-efence \
		--without-rsaref \
		--with-sys-contact="root" \
		--with-sys-location="Unknown" \
		--with-mib-modules="host smux ucd-snmp/dlmod" \
		--with-defaults \
		--prefix=/usr \
		--sysconfdir=/etc \
		--mandir=/usr/man \
		--infodir=/usr/info \
	);
	touch  $(NETSNMP_DIR)/.configured

$(NETSNMP_DIR)/agent/snmpd: $(NETSNMP_DIR)/.configured
	$(MAKE) -C $(NETSNMP_DIR)

$(TARGET_DIR)/usr/sbin/snmpd: $(NETSNMP_DIR)/agent/snmpd
	#$(MAKE) DESTDIR=$(TARGET_DIR) -C $(NETSNMP_DIR) install
	$(MAKE) PREFIX=$(TARGET_DIR)/usr \
	    prefix=$(TARGET_DIR)/usr \
	    exec_prefix=$(TARGET_DIR)/usr \
	    persistentdir=$(TARGET_DIR)/var/lib/snmp \
	    infodir=$(TARGET_DIR)/usr/info \
	    mandir=$(TARGET_DIR)/usr/man \
	    includedir=$(STAGING_DIR)/include/net-snmp \
	    ucdincludedir=$(STAGING_DIR)/include/ucd-snmp \
	    -C $(NETSNMP_DIR) install;
	rm -rf $(TARGET_DIR)/share/locale $(TARGET_DIR)/usr/info \
		$(TARGET_DIR)/usr/man $(TARGET_DIR)/usr/share/doc
	# Copy the .conf files.
	mkdir -p $(TARGET_DIR)/etc/snmp
	cp $(NETSNMP_DIR)/EXAMPLE.conf $(TARGET_DIR)/etc/snmp/snmpd.conf
	cp $(NETSNMP_DIR)/EXAMPLE-trap.conf $(TARGET_DIR)/etc/snmp/snmptrapd.conf
	-mv $(TARGET_DIR)/usr/share/snmp/mib2c*.conf $(TARGET_DIR)/etc/snmp
	mkdir -p $(TARGET_DIR)/etc/default
	cp $(NETSNMP_DIR)/debian/snmpd.default $(TARGET_DIR)/etc/default/snmpd
	# Remove the unsupported snmpcheck program
	rm $(TARGET_DIR)/usr/bin/snmpcheck
	# Install the "broken" headers
	cp $(NETSNMP_DIR)/agent/mibgroup/struct.h $(STAGING_DIR)/include/net-snmp/agent
	cp $(NETSNMP_DIR)/agent/mibgroup/util_funcs.h $(STAGING_DIR)/include/net-snmp
	cp $(NETSNMP_DIR)/agent/mibgroup/mibincl.h $(STAGING_DIR)/include/net-snmp/library
	cp $(NETSNMP_DIR)/agent/mibgroup/header_complex.h $(STAGING_DIR)/include/net-snmp/agent

netsnmp: $(TARGET_DIR)/usr/sbin/snmpd

netsnmp-headers: $(TARGET_DIR)/usr/include/net-snmp/net-snmp-config.h
	cp -a $(STAGING_DIR)/include/net-snmp $(TARGET_DIR)/usr/include/net-snmp
	cp -a $(STAGING_DIR)/include/ucd-snmp $(TARGET_DIR)/usr/include/net-snmp

netsnmp-source: $(DL_DIR)/$(NETSNMP_SOURCE)

netsnmp-clean: 
	$(MAKE) -C $(NETSNMP_DIR) clean

netsnmp-dirclean: 
	rm -rf $(NETSNMP_DIR)

