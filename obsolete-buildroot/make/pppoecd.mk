# build the Linksys pppoecd

# Note that this patches the linksys pppd before patching pppoecd.
# The pppd patch changes the pathnames that pppoecd will use. 
# In an attempt to avoid conflicts the marker file is called 
# .patched-pppoecd in the pppd directory.

PPPD_DIR=$(BUILD_DIR)/WRT54GS/release/src/router/ppp/pppd
PPPD_PATCH_DIR=$(OPENWRT_IPK_DIR)/pppoecd

PPPOECD_DIR=$(BUILD_DIR)/WRT54GS/release/src/router/ppp/pppoecd
PPPOECD_IPK_DIR=$(OPENWRT_IPK_DIR)/pppoecd
PPPOECD_IPK_BUILD_DIR=$(BUILD_DIR)/pppoecd-1.0
PPPOECD_PACKAGE=$(BUILD_DIR)/pppoecd_1.0_mipsel.ipk
PPPOECD_BIN=$(TARGET_DIR)/sbin/pppoecd
PPPOECD_IPK_BIN=$(PPPOECD_IPK_BUILD_DIR)/sbin/pppoecd

# patch pppd for the openwrt compatible path names
$(PPPD_DIR)/.patched-pppoecd: $(BUILD_DIR)/WRT54GS/.source
	cat $(PPPD_PATCH_DIR)/pppoecd-pathnames.patch | patch -d $(PPPD_DIR)
	touch $(PPPD_DIR)/.patched-pppoecd

# patch the pppoecd itself for GCC3.x compatibility and to move pppoecd to /sbin
$(PPPOECD_DIR)/.patched: $(PPPD_DIR)/.patched-pppoecd
	cat $(PPPOECD_IPK_DIR)/pppoecd.patch | patch -d $(PPPOECD_DIR)
	touch $(PPPOECD_DIR)/.patched

# build the pppoecd binary for the ipk version
$(PPPOECD_IPK_BIN): $(PPPOECD_DIR)/.patched
	mkdir -p $(PPPOECD_IPK_BUILD_DIR)/etc/ppp
	cp $(PPPOECD_IPK_DIR)/root/etc/ppp/ip-up $(PPPOECD_IPK_BUILD_DIR)/etc/ppp/ip-up
	chmod a+x $(PPPOECD_IPK_BUILD_DIR)/etc/ppp/ip-up
	$(MAKE) -C $(PPPOECD_DIR) CC=$(TARGET_CC) LD=$(TARGET_CROSS)ld \
		SRCBASE=$(OPENWRT_SRCBASE) INSTALLDIR=$(PPPOECD_IPK_BUILD_DIR) LIBDIR=$(UCLIBC_DIR)/lib \
		STRIP="$(STRIP)" \
		install

# setup ipkg control files
$(PPPOECD_IPK_BUILD_DIR)/CONTROL/control: 
	mkdir -p $(PPPOECD_IPK_BUILD_DIR)/CONTROL
	cp $(PPPOECD_IPK_DIR)/CONTROL/prerm $(PPPOECD_IPK_BUILD_DIR)/CONTROL/prerm
	chmod a+x $(PPPOECD_IPK_BUILD_DIR)/CONTROL/prerm
	cp $(PPPOECD_IPK_DIR)/CONTROL/postrm $(PPPOECD_IPK_BUILD_DIR)/CONTROL/postrm
	chmod a+x $(PPPOECD_IPK_BUILD_DIR)/CONTROL/postrm
	cp $(PPPOECD_IPK_DIR)/CONTROL/conffiles $(PPPOECD_IPK_BUILD_DIR)/CONTROL/conffiles
	cp $(PPPOECD_IPK_DIR)/CONTROL/control $(PPPOECD_IPK_BUILD_DIR)/CONTROL/control
	
# build the ipk package
$(PPPOECD_PACKAGE): $(PPPOECD_IPK_BIN) $(PPPOECD_IPK_BUILD_DIR)/CONTROL/control
	cd $(BUILD_DIR); $(STAGING_DIR)/bin/ipkg-build -c -o root -g root $(PPPOECD_IPK_BUILD_DIR)

# main target for building the ipk version
pppoecd-ipk: $(PPPOECD_PACKAGE)
	
# the embedded binary
$(PPPOECD_BIN): $(PPPOECD_DIR)/.patched
	mkdir -p $(TARGET_DIR)/etc/ppp
	cp $(PPPOECD_IPK_DIR)/root/etc/ppp/ip-up $(TARGET_DIR)/etc/ppp/ip-up
	chmod a+x $(TARGET_DIR)/etc/ppp/ip-up
	$(MAKE) -C $(PPPOECD_DIR) CC=$(TARGET_CC) LD=$(TARGET_CROSS)ld \
		SRCBASE=$(OPENWRT_SRCBASE) INSTALLDIR=$(TARGET_DIR) LIBDIR=$(UCLIBC_DIR)/lib \
		STRIP="$(STRIP)" \
		install

# main target for building the embedded version	
pppoecd: $(PPPOECD_BIN)

pppoecd-clean:
	-$(MAKE) -C $(PPPOECD_DIR) clean
	rm -f $(TARGET_DIR)/usr/sbin/pppoecd
	rm -rf $(PPPOECD_IPK_BUILD_DIR)
	rm -f $(PPPOECD_PACKAGE)
