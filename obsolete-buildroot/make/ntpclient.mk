#############################################################
#
# ntpclient
#
#############################################################
NTPCLIENT_VERSION:=2003_194
NTPCLIENT_SOURCE:=ntpclient_$(NTPCLIENT_VERSION).tar.gz
NTPCLIENT_SITE:=http://doolittle.faludi.com/ntpclient
NTPCLIENT_DIR:=$(BUILD_DIR)/ntpclient
NTPCLIENT_IPK=$(BUILD_DIR)/ntpclient_$(NTPCLIENT_VERSION)-1_mipsel.ipk
NTPCLIENT_IPK_DIR:=$(BUILD_DIR)/ntpclient-$(NTPCLIENT_VERSION)-ipk

$(DL_DIR)/$(NTPCLIENT_SOURCE):
	 $(WGET) -P $(DL_DIR) $(NTPCLIENT_SITE)/$(NTPCLIENT_SOURCE)

ntpclient-source: $(DL_DIR)/$(NTPCLIENT_SOURCE)

$(NTPCLIENT_DIR)/.unpacked: $(DL_DIR)/$(NTPCLIENT_SOURCE)
	tar -C $(BUILD_DIR) -zxf $(DL_DIR)/$(NTPCLIENT_SOURCE)
	touch $(NTPCLIENT_DIR)/.unpacked

$(NTPCLIENT_DIR)/ntpclient: $(NTPCLIENT_DIR)/.unpacked
	CFLAGS="$(TARGET_CFLAGS)" $(MAKE) $(TARGET_CONFIGURE_OPTS) LD=$(TARGET_CC) -C $(NTPCLIENT_DIR)

$(NTPCLIENT_IPK): $(NTPCLIENT_DIR)/ntpclient
	mkdir -p $(NTPCLIENT_IPK_DIR)/CONTROL $(NTPCLIENT_IPK_DIR)/usr/sbin
	install -m 644 $(OPENWRT_IPK_DIR)/ntpclient/CONTROL/control $(NTPCLIENT_IPK_DIR)/CONTROL
	install -m 755 $(NTPCLIENT_DIR)/ntpclient $(NTPCLIENT_IPK_DIR)/usr/sbin/
	$(STRIP) $(NTPCLIENT_IPK_DIR)/usr/sbin/ntpclient
	cd $(BUILD_DIR); $(IPKG_BUILD) $(NTPCLIENT_IPK_DIR)

ntpclient-ipk : uclibc $(NTPCLIENT_IPK)

ntpclient-clean:
	-$(MAKE) -C $(NTPCLIENT_DIR) clean

ntpclient-dirclean:
	rm -rf $(NTPCLIENT_DIR) $(NTPCLIENT_IPK_DIR)
