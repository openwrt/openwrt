#############################################################
#
# ttcp
#
#############################################################
#
TTCP_SOURCE_URL=http://ftp.sunet.se/ftp/pub/network/monitoring/ttcp
TTCP_SOURCE=ttcp.c
TTCP_BUILD_DIR=$(BUILD_DIR)/ttcp

$(DL_DIR)/$(TTCP_SOURCE):
	 $(WGET) -P $(DL_DIR) $(TTCP_SOURCE_URL)/$(TTCP_SOURCE) 

$(TTCP_BUILD_DIR)/.unpacked: $(DL_DIR)/$(TTCP_SOURCE)
	-mkdir $(TTCP_BUILD_DIR)
	cp -af $(DL_DIR)/$(TTCP_SOURCE) $(TTCP_BUILD_DIR)
	touch $(TTCP_BUILD_DIR)/.unpacked

$(TTCP_BUILD_DIR)/.configured: $(TTCP_BUILD_DIR)/.unpacked
	touch  $(TTCP_BUILD_DIR)/.configured

$(TTCP_BUILD_DIR)/ttcp: $(TTCP_BUILD_DIR)/.configured
	$(TARGET_CC) -O2 -o $(TTCP_BUILD_DIR)/ttcp $(TTCP_BUILD_DIR)/$(TTCP_SOURCE) 

$(TARGET_DIR)/usr/bin/ttcp: $(TTCP_BUILD_DIR)/ttcp
	cp -af $(TTCP_BUILD_DIR)/ttcp $(TARGET_DIR)/usr/bin/

ttcp: $(TARGET_DIR)/usr/bin/ttcp 

ttcp-source: $(DL_DIR)/$(TTCP_SOURCE)

ttcp-clean:
	rm -f $(TTCP_BUILD_DIR)/*.o $(TTCP_BUILD_DIR)/ttcp	

ttcp-dirclean:
	rm -rf $(TTCP_BUILD_DIR)

