#############################################################
#
# iproute2
#
#############################################################
include $(TOPDIR)/rules.mk

PKG_NAME:=iproute2
PKG_VERSION:=2.6.9
PKG_RELEASE:=1
PKG_SOURCE_URL:=http://developer.osdl.org/dev/iproute2/download/
PKG_SOURCE:=$(PKG_NAME)-2.6.9-041019.tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/iproute2-2.6.9
PKG_CAT:=zcat

PKG_IP:=$(PACKAGE_DIR)/ip_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_IP_DIR:=$(PKG_BUILD_DIR)/ipkg/ip

PKG_TC:=$(PACKAGE_DIR)/tc_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_TC_DIR:=$(PKG_BUILD_DIR)/ipkg/tc

$(DL_DIR)/$(PKG_SOURCE):
	 $(WGET) -P $(DL_DIR) $(PKG_SOURCE_URL)/$(PKG_SOURCE)

$(PKG_BUILD_DIR)/.patched: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) ./patches
	$(SED) "s:-O2:${TARGET_CFLAGS}:g" $(PKG_BUILD_DIR)/Makefile
	touch $(PKG_BUILD_DIR)/.patched

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.patched
	$(SED) "s,-I/usr/include/db3,," $(PKG_BUILD_DIR)/Makefile
	$(SED) "s,^KERNEL_INCLUDE.*,KERNEL_INCLUDE=$(LINUX_DIR)/include," \
		$(PKG_BUILD_DIR)/Makefile
	$(SED) "s,^LIBC_INCLUDE.*,LIBC_INCLUDE=$(STAGING_DIR)/include," \
		$(PKG_BUILD_DIR)/Makefile
	# For now disable compiling of the misc directory because it seems to fail
	rm -rf $(PKG_BUILD_DIR)/misc 
	$(SED) "s, misc,," $(PKG_BUILD_DIR)/Makefile
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/tc/tc: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS) KERNEL_INCLUDE=$(LINUX_DIR)/include all tc/tc

$(PKG_BUILD_DIR)/ip/ip: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS) KERNEL_INCLUDE=$(LINUX_DIR)/include all ip/ip


$(PKG_IP): $(PKG_BUILD_DIR)/ip/ip $(PACKAGE_DIR)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_IP_DIR) ip.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_IP_DIR)/usr/sbin
	cp $< $(PKG_IP_DIR)/usr/sbin/
	$(STRIP) $(PKG_IP_DIR)/usr/sbin/*
	$(IPKG_BUILD) $(PKG_IP_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/ip.list: $(PKG_IP)
	$(IPKG) install $(PKG_IP)

$(PKG_TC): $(PKG_BUILD_DIR)/tc/tc $(PACKAGE_DIR)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_TC_DIR) tc.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_TC_DIR)/usr/sbin
	cp $< $(PKG_TC_DIR)/usr/sbin/
	$(STRIP) $(PKG_TC_DIR)/usr/sbin/*
	$(IPKG_BUILD) $(PKG_TC_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/tc.list: $(PKG_TC)
	$(IPKG) install $(PKG_TC)

$(PACKAGE_DIR):
	mkdir -p $(PACKAGE_DIR)

TARGETS:=
ifneq ($(BR2_PACKAGE_IPROUTE2_IP),)
TARGETS += $(PKG_IP)
endif
ifneq ($(BR2_PACKAGE_IPROUTE2_TC),)
TARGETS += $(PKG_TC)
endif

ifeq ($(BR2_PACKAGE_IPROUTE2_IP),y)
INSTALL_TARGETS += $(IPKG_STATE_DIR)/info/ip.list
endif
ifeq ($(BR2_PACKAGE_IPROUTE2_TC),y)
INSTALL_TARGETS += $(IPKG_STATE_DIR)/info/tc.list
endif

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.patched
compile: $(TARGETS)
install: $(INSTALL_TARGETS)

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(PKG_IP) $(PKG_TC)
