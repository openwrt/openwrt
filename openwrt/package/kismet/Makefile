# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=kismet
PKG_VERSION:=2005-04-R1
PKG_RELEASE:=1
PKG_MD5SUM:=19b4f192eb11a418ed3f6bf65c1226af

PKG_SOURCE_URL:=http://www.kismetwireless.net/code
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

IPKG_SERVER:=$(PACKAGE_DIR)/$(PKG_NAME)-server_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
IDIR_SERVER:=$(PKG_BUILD_DIR)/ipkg/$(PKG_NAME)-server
INFO_SERVER:=$(IPKG_STATE_DIR)/info/$(PKG_NAME)-server.list

IPKG_DRONE:=$(PACKAGE_DIR)/$(PKG_NAME)-drone_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
IDIR_DRONE:=$(PKG_BUILD_DIR)/ipkg/$(PKG_NAME)-drone
INFO_DRONE:=$(IPKG_STATE_DIR)/info/$(PKG_NAME)-drone.list

IPKG_CLIENT:=$(PACKAGE_DIR)/$(PKG_NAME)-client_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
IDIR_CLIENT:=$(PKG_BUILD_DIR)/ipkg/$(PKG_NAME)-client
INFO_CLIENT:=$(IPKG_STATE_DIR)/info/$(PKG_NAME)-client.list

COMPILE_TARGETS:=
INSTALL_TARGETS:=
ifneq ($(BR2_PACKAGE_KISMET_SERVER),)
COMPILE_TARGETS+=$(IPKG_SERVER)
endif
ifeq ($(BR2_PACLAGE_KISMET_SERVER),y)
INSTALL_TARGETS+=$(INFO_SERVER)
endif
ifneq ($(BR2_PACKAGE_KISMET_CLIENT),)
COMPILE_TARGETS+=$(IPKG_CLIENT)
endif
ifeq ($(BR2_PACLAGE_KISMET_CLIENT),y)
INSTALL_TARGETS+=$(INFO_CLIENT)
endif
ifneq ($(BR2_PACKAGE_KISMET_DRONE),)
COMPILE_TARGETS+=$(IPKG_DRONE)
endif
ifeq ($(BR2_PACLAGE_KISMET_DRONE),y)
INSTALL_TARGETS+=$(INFO_DRONE)
endif



$(DL_DIR)/$(PKG_SOURCE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.prepared: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) ./patches
	touch $(PKG_BUILD_DIR)/.prepared

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		CXXFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include  -fno-builtin -nostdinc++ -nodefaultlibs"  \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(LINUX_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--program-prefix="" \
		--program-suffix="" \
		--prefix=/usr \
		--exec-prefix=/usr \
		--bindir=/usr/bin \
		--datadir=/usr/share \
		--includedir=/usr/include \
		--infodir=/usr/share/info \
		--libdir=/usr/lib \
		--libexecdir=/usr/lib \
		--localstatedir=/var \
		--mandir=/usr/share/man \
		--sbindir=/usr/sbin \
		--sysconfdir=/etc \
		--enable-syspcap=yes \
		$(DISABLE_NLS) \
		$(DISABLE_LARGEFILE) \
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/kismet_server $(PKG_BUILD_DIR)/kismet_drone $(PKG_BUILD_DIR)/kismet_client: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) \
		LIBS="-L$(STAGING_DIR)/lib -fno-builtin -nostdinc++ -nodefaultlibs -luClibc++ -lc -lm" \
		CPPFLAGS="-I$(STAGING_DIR)/include -I$(STAGING_DIR)/usr/include -I$(LINUX_DIR)/include" 

$(IPKG_SERVER): $(PKG_BUILD_DIR)/kismet_server
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(IDIR_SERVER) kismet-server.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(IDIR_SERVER)/etc
	cp ./files/kismet.conf $(IDIR_SERVER)/etc/
	echo '/etc/kismet.conf' > $(IDIR_SERVER)/CONTROL/conffiles
	mkdir -p $(IDIR_SERVER)/usr/sbin
	cp $< $(IDIR_SERVER)/usr/sbin/
	$(STRIP) $(IDIR_SERVER)/usr/sbin/*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(IDIR_SERVER) $(PACKAGE_DIR)

$(INFO_SERVER): $(IPKG_SERVER)
	$(IPKG) install $(IPKG_SERVER)

$(IPKG_DRONE): $(PKG_BUILD_DIR)/kismet_drone
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(IDIR_DRONE) kismet-drone.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(IDIR_DRONE)/etc
	cp ./files/kismet_drone.conf $(IDIR_DRONE)/etc/
	echo '/etc/kismet_drone.conf' > $(IDIR_DRONE)/CONTROL/conffiles
	mkdir -p $(IDIR_DRONE)/usr/sbin
	cp $< $(IDIR_DRONE)/usr/sbin/
	$(STRIP) $(IDIR_DRONE)/usr/sbin/*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(IDIR_DRONE) $(PACKAGE_DIR)

$(INFO_DRONE): $(IPKG_DRONE)
	$(IPKG) install $(IPKG_DRONE)

$(IPKG_CLIENT): $(PKG_BUILD_DIR)/kismet_client
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(IDIR_CLIENT) kismet-client.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(IDIR_CLIENT)/etc
	cp ./files/kismet.conf $(IDIR_CLIENT)/etc/
	echo '/etc/kismet.conf' > $(IDIR_CLIENT)/CONTROL/conffiles
	mkdir -p $(IDIR_CLIENT)/usr/sbin
	cp $< $(IDIR_CLIENT)/usr/sbin/
	$(STRIP) $(IDIR_CLIENT)/usr/sbin/*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(IDIR_CLIENT) $(PACKAGE_DIR)

$(INFO_CLIENT): $(IPKG_CLIENT)
	$(IPKG) install $(IPKG_CLIENT)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.prepared
compile: $(COMPILE_TARGETS)
install: $(INSTALL_TARGETS)

clean:
	rm -rf $(PKG_BUILD_DIR) $(IPKG_KISMET)
