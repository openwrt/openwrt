#############################################################
#
# wtools - Wireless Tools
#
#############################################################
#
WTOOLS_SOURCE_URL=http://www.hpl.hp.com/personal/Jean_Tourrilhes/Linux
WTOOLS_SOURCE=wireless_tools.26.tar.gz
WTOOLS_BUILD_DIR=$(BUILD_DIR)/wireless_tools.26

$(DL_DIR)/$(WTOOLS_SOURCE):
	 $(WGET) -P $(DL_DIR) $(WTOOLS_SOURCE_URL)/$(WTOOLS_SOURCE) 

$(WTOOLS_BUILD_DIR)/.unpacked: $(DL_DIR)/$(WTOOLS_SOURCE)
	zcat $(DL_DIR)/$(WTOOLS_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(WTOOLS_BUILD_DIR)/.unpacked

$(WTOOLS_BUILD_DIR)/.configured: $(WTOOLS_BUILD_DIR)/.unpacked
	touch  $(WTOOLS_BUILD_DIR)/.configured

$(WTOOLS_BUILD_DIR)/iwconfig: $(WTOOLS_BUILD_DIR)/.configured
	$(MAKE) -C $(WTOOLS_BUILD_DIR) \
		CC=$(TARGET_CC) CFLAGS="$(TARGET_CFLAGS)" \
		BUILD_SHARED=y # may want to make this an option

$(TARGET_DIR)/sbin/iwconfig: $(WTOOLS_BUILD_DIR)/iwconfig
	# Copy The Wireless Tools
	cp -af $(WTOOLS_BUILD_DIR)/iwconfig $(TARGET_DIR)/sbin/
	cp -af $(WTOOLS_BUILD_DIR)/iwevent $(TARGET_DIR)/sbin/
	cp -af $(WTOOLS_BUILD_DIR)/iwgetid $(TARGET_DIR)/sbin/
	cp -af $(WTOOLS_BUILD_DIR)/iwlist $(TARGET_DIR)/sbin/
	cp -af $(WTOOLS_BUILD_DIR)/iwpriv $(TARGET_DIR)/sbin/
	cp -af $(WTOOLS_BUILD_DIR)/iwspy $(TARGET_DIR)/sbin/
	cp -af $(WTOOLS_BUILD_DIR)/libiw.so.26 $(TARGET_DIR)/lib
	$(STRIP) $(TARGET_DIR)/sbin/iwconfig $(TARGET_DIR)/sbin/iwevent \
		$(TARGET_DIR)/sbin/iwgetid $(TARGET_DIR)/sbin/iwlist \
		$(TARGET_DIR)/sbin/iwpriv $(TARGET_DIR)/sbin/iwspy \
		$(TARGET_DIR)/lib/libiw.so.26

wtools: $(TARGET_DIR)/sbin/iwconfig 

wtools-source: $(DL_DIR)/$(WTOOLS_SOURCE)

wtools-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(WTOOLS_BUILD_DIR) uninstall
	-$(MAKE) -C $(WTOOLS_BUILD_DIR) clean

wtools-dirclean:
	rm -rf $(WTOOLS_BUILD_DIR)

