#############################################################
#
# radvd
#
#############################################################
RADVD_VERSION:=0.7.2
RADVD_SOURCE:=radvd-$(RADVD_VERSION).tar.gz
RADVD_SITE:=http://v6web.litech.org/radvd/dist
RADVD_DIR:=$(BUILD_DIR)/radvd-$(RADVD_VERSION)
RADVD_IPK=$(BUILD_DIR)/radvd_0.7.2-1_mipsel.ipk
RADVD_IPK_DIR:=$(BUILD_DIR)/radvd-0.7.2-ipk

$(DL_DIR)/$(RADVD_SOURCE):
	 $(WGET) -P $(DL_DIR) $(RADVD_SITE)/$(RADVD_SOURCE)

radvd-source: $(DL_DIR)/$(RADVD_SOURCE)

$(RADVD_DIR)/.unpacked: $(DL_DIR)/$(RADVD_SOURCE)
	tar -C $(BUILD_DIR) -zxf $(DL_DIR)/$(RADVD_SOURCE)
	touch $(RADVD_DIR)/.unpacked

$(RADVD_DIR)/.configured: $(RADVD_DIR)/.unpacked
	(cd $(RADVD_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/usr \
		--with-logfile=/tmp/radvd.log \
		--with-pidfile=/tmp/run/radvd.pid \
		--with-configfile=/etc/radvd.conf \
	);
	touch  $(RADVD_DIR)/.configured

$(RADVD_DIR)/radvd: $(RADVD_DIR)/.configured
	cp $(FLEX_DIR)/libfl.a $(RADVD_DIR)/libfl.a
	$(MAKE) $(TARGET_CONFIGURE_OPTS) LD=$(TARGET_CC) LDFLAGS=-L. -C $(RADVD_DIR)

$(RADVD_IPK):  $(RADVD_DIR)/radvd
	mkdir -p $(RADVD_IPK_DIR)/CONTROL $(RADVD_IPK_DIR)/etc $(RADVD_IPK_DIR)/usr/sbin
	install -m 644 $(OPENWRT_IPK_DIR)/radvd/CONTROL/control $(RADVD_IPK_DIR)/CONTROL/control
	install -m 755 $(OPENWRT_IPK_DIR)/radvd/CONTROL/postinst $(RADVD_IPK_DIR)/CONTROL/postinst
	install -m 644 $(OPENWRT_IPK_DIR)/radvd/radvd.conf.example $(RADVD_IPK_DIR)/etc/radvd.conf.example
	install -m 755 $(RADVD_DIR)/radvd $(RADVD_IPK_DIR)/usr/sbin/
	install -m 755 $(RADVD_DIR)/radvdump $(RADVD_IPK_DIR)/usr/sbin/
	$(STRIP) $(RADVD_IPK_DIR)/usr/sbin/radvd
	$(STRIP) $(RADVD_IPK_DIR)/usr/sbin/radvdump
	cd $(BUILD_DIR); $(IPKG_BUILD) $(RADVD_IPK_DIR)

radvd-ipk : uclibc $(FLEX_DIR)/$(FLEX_BINARY) $(RADVD_IPK)

radvd-clean:
	-$(MAKE) -C $(RADVD_DIR) clean

radvd-dirclean:
	rm -rf $(RADVD_DIR) $(RADVD_IPK_DIR)

