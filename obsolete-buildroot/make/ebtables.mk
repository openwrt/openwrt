########################################################################
#
# ebtables
#
# Utility that enables basic Ethernet frame filtering on a Linux bridge,
# MAC NAT and brouting.
#
########################################################################

EBTABLES_NAME=ebtables
EBTABLES_VERSION=v2.0.6
EBTABLES_SOURCE=$(EBTABLES_NAME)-$(EBTABLES_VERSION).tar.gz
#EBTABLES_SOURCE_URL=http://unc.dl.sourceforge.net/sourceforge/$(EBTABLES_NAME)/
EBTABLES_SOURCE_URL=http://aleron.dl.sourceforge.net/sourceforge/$(EBTABLES_NAME)/
EBTABLES_BUILD_DIR=$(BUILD_DIR)/$(EBTABLES_NAME)-$(EBTABLES_VERSION)

$(DL_DIR)/$(EBTABLES_SOURCE):
	 $(WGET) -P $(DL_DIR) $(EBTABLES_SOURCE_URL)/$(EBTABLES_SOURCE)

$(EBTABLES_BUILD_DIR)/.unpacked: $(DL_DIR)/$(EBTABLES_SOURCE)
	zcat $(DL_DIR)/$(EBTABLES_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(EBTABLES_BUILD_DIR)/.unpacked

$(EBTABLES_BUILD_DIR)/.configured: $(EBTABLES_BUILD_DIR)/.unpacked
	# Allow patches.  Needed for openwrt for instance.
	$(SOURCE_DIR)/patch-kernel.sh $(EBTABLES_BUILD_DIR) $(SOURCE_DIR) $(EBTABLES_NAME)-*.patch
	touch  $(EBTABLES_BUILD_DIR)/.configured

$(EBTABLES_BUILD_DIR)/$(EBTABLES_NAME): $(EBTABLES_BUILD_DIR)/.configured
	$(TARGET_CONFIGURE_OPTS) \
	$(MAKE) -C $(EBTABLES_BUILD_DIR) \
		CC=$(TARGET_CC) CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)"

$(TARGET_DIR)/sbin/$(EBTABLES_NAME): $(EBTABLES_BUILD_DIR)/$(EBTABLES_NAME)
	mkdir -p $(TARGET_DIR)/sbin/
	$(TARGET_CONFIGURE_OPTS) \
	$(MAKE) -C $(EBTABLES_BUILD_DIR)	\
		MANDIR=$(TARGET_DIR)/usr/share/man \
		ETHERTYPESPATH=${D}/etc/ \
		BINPATH=$(TARGET_DIR)/sbin/ \
		CC=$(TARGET_CC) \
		CFLAGS="$(TARGET_CFLAGS)" \
		install
	$(STRIP) $(TARGET_DIR)/sbin/$(EBTABLES_NAME)
	rm -rf $(TARGET_DIR)/usr/share/man

$(EBTABLES_NAME): $(TARGET_DIR)/sbin/$(EBTABLES_NAME) 

$(EBTABLES_NAME)-source: $(DL_DIR)/$(EBTABLES_SOURCE)

$(EBTABLES_NAME)-clean:
	-$(MAKE) -C $(EBTABLES_BUILD_DIR) clean

$(EBTABLES_NAME)-dirclean:
	rm -rf $(EBTABLES_BUILD_DIR)
