#############################################################
#
# raidtools2
#
#############################################################
RAIDTOOLS2_SOURCE:=raidtools2-1.00.3.tar.gz
RAIDTOOLS2_SITE:=
RAIDTOOLS2_DIR:=$(BUILD_DIR)/raidtools2-1.00.3
RAIDTOOLS2_CAT:=zcat
RAIDTOOLS2_BINARY:=mkraid
RAIDTOOLS2_TARGET_BINARY:=sbin/mkraid

$(DL_DIR)/$(RAIDTOOLS2_SOURCE):
	$(WGET) -P $(DL_DIR) $(RAIDTOOLS2_SITE)/$(RAIDTOOLS2_SOURCE)

raidtools2-source: $(DL_DIR)/$(RAIDTOOLS2_SOURCE)

$(RAIDTOOLS2_DIR)/.unpacked: $(DL_DIR)/$(RAIDTOOLS2_SOURCE)
	$(RAIDTOOLS2_CAT) $(DL_DIR)/$(RAIDTOOLS2_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(RAIDTOOLS2_DIR)/.unpacked

$(RAIDTOOLS2_DIR)/.configured: $(RAIDTOOLS2_DIR)/.unpacked
	(cd $(RAIDTOOLS2_DIR); rm -rf config.cache; \
                $(TARGET_CONFIGURE_OPTS) \
                ./configure \
                --target=$(GNU_TARGET_NAME) \
                --host=$(GNU_TARGET_NAME) \
                --build=$(GNU_HOST_NAME) \
                --prefix=/usr \
                --exec-prefix=/usr \
                --bindir=/usr/bin \
                --sbindir=/usr/sbin \
                --libexecdir=/usr/lib \
                --sysconfdir=/etc \
                --datadir=/usr/share/misc \
                --localstatedir=/var \
                --mandir=/usr/man \
                --infodir=/usr/info \
		$(DISABLE_NLS) \
                --enable-fsect-man5 \
        );
	touch  $(RAIDTOOLS2_DIR)/.configured

$(RAIDTOOLS2_DIR)/$(RAIDTOOLS2_BINARY): $(RAIDTOOLS2_DIR)/.configured
        $(MAKE) CC=$(TARGET_CC) -C $(RAIDTOOLS2_DIR)

$(TARGET_DIR)/$(RAIDTOOLS2_TARGET_BINARY): $(RAIDTOOLS2_DIR)/$(RAIDTOOLS2_BINARY)
	$(MAKE) ROOTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(RAIDTOOLS2_DIR) install
	rm -rf $(TARGET_DIR)/share/locale $(TARGET_DIR)/usr/info \
		$(TARGET_DIR)/usr/man $(TARGET_DIR)/usr/share/doc

raidtools2: zlib uclibc $(TARGET_DIR)/$(RAIDTOOLS2_TARGET_BINARY)

raidtools2-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(RAIDTOOLS2_DIR) uninstall
	-$(MAKE) -C $(RAIDTOOLS2_DIR) clean

raidtools2-dirclean:
	rm -rf $(RAIDTOOLS2_DIR)
