# $Id$

include $(TOPDIR)/rules.mk

PKG_OS:=$(shell uname -s)
PKG_CPU:=$(shell uname -m)

PKG_RELEASE:=1
PKG_NAME:=OpenWrt-SDK-$(PKG_OS)-$(PKG_CPU)-$(PKG_RELEASE)

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

all: compile

$(BIN_DIR)/$(PKG_NAME).tar.bz2:
	(cd $(STAGING_DIR); \
		rm -rf info man share; \
		cd usr; \
		rm -rf doc info man share; \
	)
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)/dl $(PKG_BUILD_DIR)/examples $(PKG_BUILD_DIR)/package
	cp -a $(STAGING_DIR) $(SCRIPT_DIR) $(TOPDIR)/docs $(PKG_BUILD_DIR)/
	cp -a $(TOPDIR)/package/strace $(TOPDIR)/package/iproute2 $(PKG_BUILD_DIR)/examples
	cp $(TOPDIR)/rules.mk $(PKG_BUILD_DIR)/
	cp ./files/Makefile.sdk $(PKG_BUILD_DIR)/Makefile
	cp ./files/README.SDK $(PKG_BUILD_DIR)/
	cp ./files/depend.mk $(PKG_BUILD_DIR)/package/
	egrep '^BR2_(ARCH|WGET|STAGING|JLEVEL|LARGEFILE|TARGET_OPT)' $(TOPDIR)/.config > $(PKG_BUILD_DIR)/.config
	find $(PKG_BUILD_DIR) -name CVS | xargs rm -rf 
	(cd $(BUILD_DIR); \
		tar cfj $@ $(PKG_NAME); \
	)

source:
prepare:
compile: $(BIN_DIR)/$(PKG_NAME).tar.bz2
install:

clean:
	rm -rf $(PKG_BUILD_DIR)
