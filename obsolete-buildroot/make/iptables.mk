#############################################################
#
# iptables
#
#############################################################
IPTABLES_SOURCE_URL=http://www.netfilter.org/files
IPTABLES_SOURCE=iptables-1.2.9.tar.bz2
IPTABLES_BUILD_DIR=$(BUILD_DIR)/iptables-1.2.9
IP6TABLES_IPK=$(BUILD_DIR)/ip6tables_1.2.9-1_mipsel.ipk
IP6TABLES_IPK_DIR:=$(BUILD_DIR)/ip6tables-1.2.9-ipk

$(DL_DIR)/$(IPTABLES_SOURCE):
	 $(WGET) -P $(DL_DIR) $(IPTABLES_SOURCE_URL)/$(IPTABLES_SOURCE) 

$(IPTABLES_BUILD_DIR)/.unpacked: $(DL_DIR)/$(IPTABLES_SOURCE)
	bzcat $(DL_DIR)/$(IPTABLES_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(IPTABLES_BUILD_DIR)/.unpacked

$(IPTABLES_BUILD_DIR)/.configured: $(IPTABLES_BUILD_DIR)/.unpacked
	# Allow patches.  Needed for openwrt for instance.
	$(SOURCE_DIR)/patch-kernel.sh $(IPTABLES_BUILD_DIR) $(SOURCE_DIR) iptables-*.patch
	#
	$(SED) "s;\[ -f /usr/include/netinet/ip6.h \];grep -q '__UCLIBC_HAS_IPV6__ 1' \
		$(BUILD_DIR)/uClibc/include/bits/uClibc_config.h;" $(IPTABLES_BUILD_DIR)/Makefile
	touch  $(IPTABLES_BUILD_DIR)/.configured

$(IPTABLES_BUILD_DIR)/iptables: $(IPTABLES_BUILD_DIR)/.configured
	$(TARGET_CONFIGURE_OPTS) \
	$(MAKE) -C $(IPTABLES_BUILD_DIR) \
		KERNEL_DIR=$(LINUX_DIR) PREFIX=/usr \
		CC=$(TARGET_CC) COPT_FLAGS="$(TARGET_CFLAGS)"

$(TARGET_DIR)/sbin/iptables: $(IPTABLES_BUILD_DIR)/iptables
	$(TARGET_CONFIGURE_OPTS) \
	$(MAKE) -C $(IPTABLES_BUILD_DIR) \
		KERNEL_DIR=$(LINUX_DIR) PREFIX=/usr \
		CC=$(TARGET_CC) COPT_FLAGS="$(TARGET_CFLAGS)" \
		DESTDIR=$(TARGET_DIR) install
	$(STRIP) $(TARGET_DIR)/usr/sbin/iptables*
	$(STRIP) $(TARGET_DIR)/usr/sbin/ip6tables*
	$(STRIP) $(TARGET_DIR)/usr/lib/iptables/*.so
	rm -rf $(TARGET_DIR)/usr/man

iptables: $(TARGET_DIR)/sbin/iptables 

$(IP6TABLES_IPK):
	mkdir -p $(IP6TABLES_IPK_DIR)/CONTROL
	mkdir -p $(IP6TABLES_IPK_DIR)/usr/lib/iptables
	mkdir -p $(IP6TABLES_IPK_DIR)/usr/sbin
	install -m 644 $(OPENWRT_IPK_DIR)/ip6tables/ip6tables.control $(IP6TABLES_IPK_DIR)/CONTROL/control
	install -m 755 $(IPTABLES_BUILD_DIR)/ip6tables $(IP6TABLES_IPK_DIR)/usr/sbin
	install -m 755 $(IPTABLES_BUILD_DIR)/extensions/libip6t_*.so $(IP6TABLES_IPK_DIR)/usr/lib/iptables/
	$(STRIP) $(IP6TABLES_IPK_DIR)/usr/sbin/ip6tables*
	$(STRIP) $(IP6TABLES_IPK_DIR)/usr/lib/iptables/*.so
	cd $(BUILD_DIR); $(STAGING_DIR)/bin/ipkg-build -c -o root -g root $(IP6TABLES_IPK_DIR)

ip6tables-ipk: iptables $(IP6TABLES_IPK)

iptables-source: $(DL_DIR)/$(IPTABLES_SOURCE)

iptables-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(IPTABLES_BUILD_DIR) uninstall
	-$(MAKE) -C $(IPTABLES_BUILD_DIR) clean

iptables-dirclean:
	rm -rf $(IPTABLES_BUILD_DIR)

