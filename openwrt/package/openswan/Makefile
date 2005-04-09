# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=openswan
PKG_VERSION:=2.3.1dr6
PKG_RELEASE:=1
PKG_MD5SUM:=x

PKG_SOURCE_URL:=http://www.openswan.org/download
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT:=zcat
PKG_IPK:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_IPK_DIR:=$(PKG_BUILD_DIR)/ipkg

$(DL_DIR)/$(PKG_SOURCE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.patched: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) ./patches
	touch $(PKG_BUILD_DIR)/.patched

FLAGS := $(TARGET_CFLAGS) -I$(PKG_BUILD_DIR)/linux/include -L$(STAGING_DIR)/usr/lib -I$(STAGING_DIR)/usr/include

$(PKG_BUILD_DIR)/$(PKG_NAME): $(PKG_BUILD_DIR)/.patched
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		KERNELSRC="$(LINUX_DIR)" \
		ARCH="mips" \
		USERCOMPILE="$(FLAGS)" \
		module
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		KERNELSRC="$(LINUX_DIR)" \
		ARCH="mips" \
		USERCOMPILE="$(FLAGS)" \
		EXTRA_INCLUDE="-I$(STAGING_DIR)/usr/include" \
		EXTRA_LIBS="-L$(STAGING_DIR)/usr/lib" \
		IPSECDIR="/usr/lib/ipsec" \
		INC_USRLOCAL="/usr" \
		programs

$(PKG_IPK): $(PKG_BUILD_DIR)/$(PKG_NAME)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_IPK_DIR) $(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		DESTDIR="$(PKG_IPK_DIR)" \
		KERNELSRC="$(LINUX_DIR)" \
		ARCH="mips" \
		USERCOMPILE="$(FLAGS)" \
		IPSECDIR="/usr/lib/ipsec" \
		INC_USRLOCAL="/usr" \
		install
	-$(STRIP) $(PKG_IPK_DIR)/usr/lib/ipsec/*
	-$(STRIP) $(PKG_IPK_DIR)/usr/libexec/ipsec/*
	rm -rf $(PKG_IPK_DIR)/usr/share
	rm -rf $(PKG_IPK_DIR)/usr/man
	rm -rf $(PKG_IPK_DIR)/var
	rm -rf $(PKG_IPK_DIR)/etc/rc.d/rc*.d
	mkdir -p $(PKG_IPK_DIR)/etc/init.d
	ln -sf ../rc.d/init.d/ipsec $(PKG_IPK_DIR)/etc/init.d/S60ipsec
	mkdir -p $(PKG_IPK_DIR)/lib/modules/$(LINUX_VERSION)
	cp $(PKG_BUILD_DIR)/modobj/ipsec.o $(PKG_IPK_DIR)/lib/modules/$(LINUX_VERSION)/
	$(TARGET_CROSS)strip --remove-section=.comment --remove-section=.note \
	  $(PKG_IPK_DIR)/lib/modules/$(LINUX_VERSION)/ipsec.o
	mkdir -p $(PACKAGE_DIR)
	find $(PKG_BUILD_DIR) -name \*.old | xargs rm -rf
	$(IPKG_BUILD) $(PKG_IPK_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/$(PKG_NAME).list: $(PKG_IPK)
	$(IPKG) install $(PKG_IPK)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.patched
compile: $(PKG_IPK)
install: $(IPKG_STATE_DIR)/info/$(PKG_NAME).list

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(PKG_IPK)
