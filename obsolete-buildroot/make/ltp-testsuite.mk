#############################################################
#
# ltp-testsuite
#
#############################################################
LTP_TESTSUITE_SOURCE:=ltp-full-20040206.tgz
LTP_TESTSUITE_SITE:=http://aleron.dl.sourceforge.net/sourceforge/ltp
LTP_TESTSUITE_CAT:=zcat
LTP_TESTSUITE_DIR:=$(BUILD_DIR)/ltp-full-20040206
LTP_TESTSUITE_PATCH:=$(SOURCE_DIR)/ltp-testsuite.patch


$(DL_DIR)/$(LTP_TESTSUITE_SOURCE):
	 $(WGET) -P $(DL_DIR) $(LTP_TESTSUITE_SITE)/$(LTP_TESTSUITE_SOURCE)

ltp-testsuite-source: $(DL_DIR)/$(LTP_TESTSUITE_SOURCE)

$(LTP_TESTSUITE_DIR)/.unpacked: $(DL_DIR)/$(LTP_TESTSUITE_SOURCE)
	$(LTP_TESTSUITE_CAT) $(DL_DIR)/$(LTP_TESTSUITE_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	cat $(LTP_TESTSUITE_PATCH) | patch -p1 -d $(LTP_TESTSUITE_DIR)
	touch $(LTP_TESTSUITE_DIR)/.unpacked

$(LTP_TESTSUITE_DIR)/ltp-testsuite: $(LTP_TESTSUITE_DIR)/.unpacked
	$(MAKE) $(TARGET_CONFIGURE_OPTS) CROSS_COMPILER=$(TARGET_CROSS) \
		-C $(LTP_TESTSUITE_DIR)

$(TARGET_DIR)/usr/bin/ltp-testsuite: $(LTP_TESTSUITE_DIR)/ltp-testsuite
	$(MAKE) $(TARGET_CONFIGURE_OPTS) CROSS_COMPILER=$(TARGET_CROSS) \
		-C $(LTP_TESTSUITE_DIR) install

ltp-testsuite: uclibc $(TARGET_DIR)/usr/bin/ltp-testsuite

ltp-testsuite-clean:
	$(MAKE) -C $(LTP_TESTSUITE_DIR) clean

ltp-testsuite-dirclean:
	rm -rf $(LTP_TESTSUITE_DIR)


