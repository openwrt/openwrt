#############################################################
#
# bridgeutils - User Space Program For Controling Bridging
#
#############################################################
#
BRIDGE_SOURCE_URL=http://umn.dl.sourceforge.net/sourceforge/bridge/
BRIDGE_SOURCE=bridge-utils-1.0.4.tar.gz
BRIDGE_BUILD_DIR=$(BUILD_DIR)/bridge-utils-1.0.4
BRIDGE_TARGET_BINARY:=usr/sbin/brctl

$(DL_DIR)/$(BRIDGE_SOURCE):
	 $(WGET) -P $(DL_DIR) $(BRIDGE_SOURCE_URL)/$(BRIDGE_SOURCE)

$(BRIDGE_BUILD_DIR)/.unpacked: $(DL_DIR)/$(BRIDGE_SOURCE)
	zcat $(DL_DIR)/$(BRIDGE_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	patch -p1 -d $(BRIDGE_BUILD_DIR) < package/bridge/bridge.patch
	touch $(BRIDGE_BUILD_DIR)/.unpacked

$(BRIDGE_BUILD_DIR)/.configured: $(BRIDGE_BUILD_DIR)/.unpacked
	(cd $(BRIDGE_BUILD_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
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
		--datadir=/usr/share \
		--localstatedir=/var \
		--mandir=/usr/man \
		--infodir=/usr/info \
		$(DISABLE_NLS) \
		--with-linux-headers=$(LINUX_DIR) \
	);
	touch  $(BRIDGE_BUILD_DIR)/.configured

$(BRIDGE_BUILD_DIR)/brctl/brctl: $(BRIDGE_BUILD_DIR)/.configured
	$(MAKE) -C $(BRIDGE_BUILD_DIR)

$(TARGET_DIR)/$(BRIDGE_TARGET_BINARY): $(BRIDGE_BUILD_DIR)/brctl/brctl
	cp -af $(BRIDGE_BUILD_DIR)/brctl/brctl $(TARGET_DIR)/$(BRIDGE_TARGET_BINARY)
	$(STRIP) $(TARGET_DIR)/$(BRIDGE_TARGET_BINARY)
	#cp -af $(BRIDGE_BUILD_DIR)/brctl/brctld $(TARGET_DIR)/usr/sbin/
	#$(STRIP) $(TARGET_DIR)/usr/sbin/brctld

bridge: linux $(TARGET_DIR)/$(BRIDGE_TARGET_BINARY)

bridge-source: $(DL_DIR)/$(BRIDGE_SOURCE)

bridge-clean:
	#$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(BRIDGE_BUILD_DIR) uninstall
	-$(MAKE) -C $(BRIDGE_BUILD_DIR) clean

bridge-dirclean:
	rm -rf $(BRIDGE_BUILD_DIR)
