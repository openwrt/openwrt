#############################################################
#
# ltrace
#
#############################################################
LTRACE_SOURCE=ltrace_0.3.31.tar.gz
LTRACE_SITE=http://ftp.debian.org/debian/pool/main/l/ltrace
LTRACE_DIR=$(BUILD_DIR)/ltrace-0.3.31
LTRACE_BINARY=ltrace
LTRACE_TARGET_BINARY=usr/bin/ltrace

$(DL_DIR)/$(LTRACE_SOURCE):
	$(WGET) -P $(DL_DIR) $(LTRACE_SITE)/$(LTRACE_SOURCE)

$(LTRACE_DIR)/.source: $(DL_DIR)/$(LTRACE_SOURCE)
	zcat $(DL_DIR)/$(LTRACE_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(LTRACE_DIR)/.source

$(LTRACE_DIR)/.configured: $(LTRACE_DIR)/.source
	(cd $(LTRACE_DIR); \
		$(TARGET_CONFIGURE_OPTS) \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/usr \
		--sysconfdir=/etc \
	);
	touch $(LTRACE_DIR)/.configured;

$(LTRACE_DIR)/$(LTRACE_BINARY): $(LTRACE_DIR)/.configured
	$(MAKE) CC=$(TARGET_CC) -C $(LTRACE_DIR)

$(TARGET_DIR)/$(LTRACE_TARGET_BINARY): $(LTRACE_DIR)/$(LTRACE_BINARY)
	$(MAKE) DESTDIR=$(TARGET_DIR) -C $(LTRACE_DIR) install
	rm -Rf $(TARGET_DIR)/usr/man

ltrace: uclibc $(TARGET_DIR)/$(LTRACE_TARGET_BINARY)

ltrace-source: $(DL_DIR)/$(LTRACE_SOURCE)

ltrace-clean:
	$(MAKE) prefix=$(TARGET_DIR)/usr -C $(LTRACE_DIR) uninstall
	-$(MAKE) -C $(LTRACE_DIR) clean

ltrace-dirclean:
	rm -rf $(LTRACE_DIR)

