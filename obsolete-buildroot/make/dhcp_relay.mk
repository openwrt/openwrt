#############################################################
#
# dhcp_relay
#
#############################################################
DHCP_RELAY_SOURCE:=dhcp-3.0pl2.tar.gz
DHCP_RELAY_SITE:=ftp://ftp.isc.org/isc/dhcp
DHCP_RELAY_CAT:=zcat
DHCP_RELAY_DIR:=$(BUILD_DIR)/dhcp-3.0pl2
DHCP_RELAY_BINARY:=work.linux-2.2/relay/dhcrelay
DHCP_RELAY_TARGET_BINARY:=usr/sbin/dhcrelay
BVARS=PREDEFINES='-D_PATH_DHCPD_DB=\"/var/lib/dhcp/dhcpd.leases\" \
	-D_PATH_DHCLIENT_DB=\"/var/lib/dhcp/dhclient.leases\"' \
	VARDB=/var/lib/dhcp

$(DL_DIR)/$(DHCP_RELAY_SOURCE):
	 $(WGET) -P $(DL_DIR) $(DHCP_RELAY_SITE)/$(DHCP_RELAY_SOURCE)

dhcp_relay-source: $(DL_DIR)/$(DHCP_RELAY_SOURCE)

$(DHCP_RELAY_DIR)/.unpacked: $(DL_DIR)/$(DHCP_RELAY_SOURCE)
	$(DHCP_RELAY_CAT) $(DL_DIR)/$(DHCP_RELAY_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(DHCP_RELAY_DIR)/.unpacked

$(DHCP_RELAY_DIR)/.configured: $(DHCP_RELAY_DIR)/.unpacked
	(cd $(DHCP_RELAY_DIR); $(TARGET_CONFIGURE_OPTS) ./configure );
	touch  $(DHCP_RELAY_DIR)/.configured

$(DHCP_RELAY_DIR)/$(DHCP_RELAY_BINARY): $(DHCP_RELAY_DIR)/.configured
	$(MAKE) CC=$(TARGET_CC) $(BVARS) -C $(DHCP_RELAY_DIR)
	$(STRIP) $(DHCP_RELAY_DIR)/$(DHCP_RELAY_BINARY)

$(TARGET_DIR)/$(DHCP_RELAY_TARGET_BINARY): $(DHCP_RELAY_DIR)/$(DHCP_RELAY_BINARY)
	(cd $(TARGET_DIR)/var/lib; ln -sf /tmp dhcp)
	cp -a $(DHCP_RELAY_DIR)/$(DHCP_RELAY_BINARY) $(TARGET_DIR)/$(DHCP_RELAY_TARGET_BINARY) 
	rm -rf $(TARGET_DIR)/share/locale $(TARGET_DIR)/usr/info \
		$(TARGET_DIR)/usr/man $(TARGET_DIR)/usr/share/doc

dhcp_relay: uclibc $(TARGET_DIR)/$(DHCP_RELAY_TARGET_BINARY)

dhcp_relay-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(DHCP_RELAY_DIR) uninstall
	-$(MAKE) -C $(DHCP_RELAY_DIR) clean

dhcp_relay-dirclean:
	rm -rf $(DHCP_RELAY_DIR)

