########################################################################
#
# arptables
#
# Arptables is used to set up, maintain, and inspect the tables of ARP
# rules in the Linux kernel. It is analogous to iptables, but operates
# at the ARP layer rather than the IP layer."
#
########################################################################

ARPTABLES_NAME=arptables
ARPTABLES_VER=v0.0.3-2
ARPTABLES_SOURCE=$(ARPTABLES_NAME)-$(ARPTABLES_VER).tar.gz
ARPTABLES_SOURCE_URL=http://unc.dl.sourceforge.net/sourceforge/ebtables/
ARPTABLES_BUILD_DIR=$(BUILD_DIR)/$(ARPTABLES_NAME)-$(ARPTABLES_VER)

$(DL_DIR)/$(ARPTABLES_SOURCE):
	 $(WGET) -P $(DL_DIR) $(ARPTABLES_SOURCE_URL)/$(ARPTABLES_SOURCE)

$(ARPTABLES_BUILD_DIR)/.unpacked: $(DL_DIR)/$(ARPTABLES_SOURCE)
	zcat $(DL_DIR)/$(ARPTABLES_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(ARPTABLES_BUILD_DIR)/.unpacked

$(ARPTABLES_BUILD_DIR)/.configured: $(ARPTABLES_BUILD_DIR)/.unpacked
	# Allow patches.  Needed for openwrt for instance.
	$(SOURCE_DIR)/patch-kernel.sh $(ARPTABLES_BUILD_DIR) $(SOURCE_DIR) $(ARPTABLES_NAME)-*.patch
	touch  $(ARPTABLES_BUILD_DIR)/.configured

$(ARPTABLES_BUILD_DIR)/$(ARPTABLES_NAME): $(ARPTABLES_BUILD_DIR)/.configured
	$(TARGET_CONFIGURE_OPTS) \
	$(MAKE) -C $(ARPTABLES_BUILD_DIR) \
		CC=$(TARGET_CC) COPT_FLAGS="$(TARGET_CFLAGS)"

$(TARGET_DIR)/sbin/$(ARPTABLES_NAME): $(ARPTABLES_BUILD_DIR)/$(ARPTABLES_NAME)
	cp $(ARPTABLES_BUILD_DIR)/$(ARPTABLES_NAME) $(TARGET_DIR)/sbin/$(ARPTABLES_NAME)
	$(STRIP) $(TARGET_DIR)/sbin/$(ARPTABLES_NAME)

$(ARPTABLES_NAME): $(TARGET_DIR)/sbin/$(ARPTABLES_NAME) 

$(ARPTABLES_NAME)-source: $(DL_DIR)/$(ARPTABLES_SOURCE)

$(ARPTABLES_NAME)-clean:
	-$(MAKE) -C $(ARPTABLES_BUILD_DIR) clean

$(ARPTABLES_NAME)-dirclean:
	rm -rf $(ARPTABLES_BUILD_DIR)
