#############################################################
#
# ebtables
#
#############################################################

EBTABLES_SOURCE_URL=http://umn.dl.sourceforge.net/sourceforge/ebtables/
EBTABLES_SOURCE=ebtables-v2.0.6.tar.gz
EBTABLES_BUILD_DIR=$(BUILD_DIR)/ebtables-v2.0.6
EBTABLES_TARGET_BINARY:=usr/sbin/ebtables

$(DL_DIR)/$(EBTABLES_SOURCE):
	 $(WGET) -P $(DL_DIR) $(EBTABLES_SOURCE_URL)/$(EBTABLES_SOURCE)

$(EBTABLES_BUILD_DIR)/.unpacked: $(DL_DIR)/$(EBTABLES_SOURCE)
	zcat $(DL_DIR)/$(EBTABLES_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(EBTABLES_BUILD_DIR)/.unpacked

$(EBTABLES_BUILD_DIR)/ebtables: $(EBTABLES_BUILD_DIR)/.unpacked
	$(MAKE) -C $(EBTABLES_BUILD_DIR) \
		CC=$(TARGET_CC) LD=$(TARGET_CROSS)ld \
		CFLAGS="$(TARGET_CFLAGS) -Wall"

$(TARGET_DIR)/$(EBTABLES_TARGET_BINARY): $(EBTABLES_BUILD_DIR)/ebtables
	cp -af $(EBTABLES_BUILD_DIR)/ebtables $(TARGET_DIR)/$(EBTABLES_TARGET_BINARY)
	$(STRIP) $(TARGET_DIR)/$(EBTABLES_TARGET_BINARY)

ebtables: $(TARGET_DIR)/$(EBTABLES_TARGET_BINARY)

ebtables-source: $(DL_DIR)/$(EBTABLES_SOURCE)

ebtables-clean:
	#$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(EBTABLES_BUILD_DIR) uninstall
	-$(MAKE) -C $(EBTABLES_BUILD_DIR) clean

ebtables-dirclean:
	rm -rf $(EBTABLES_BUILD_DIR)

ifeq ($(strip $(BR2_PACKAGE_EBTABLES)),y)
TARGETS+=ebtables
endif
