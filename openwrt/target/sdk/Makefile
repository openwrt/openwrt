# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_OS:=$(shell uname -s)
PKG_CPU:=$(shell uname -m)

SDK_NAME:=OpenWrt-SDK-$(BOARD)-$(KERNEL)-for-$(PKG_OS)-$(PKG_CPU)
SDK_BUILD_DIR:=$(BUILD_DIR)/$(SDK_NAME)

all: compile

$(BIN_DIR)/$(SDK_NAME).tar.bz2:
	(cd $(STAGING_DIR); \
		rm -rf info man share stampfiles; \
		cd usr; \
		rm -rf doc info man share; \
	)
	rm -rf $(SDK_BUILD_DIR)
	mkdir -p $(SDK_BUILD_DIR)/dl $(SDK_BUILD_DIR)/package
	$(CP) $(STAGING_DIR) $(INCLUDE_DIR) $(SCRIPT_DIR) $(TOPDIR)/docs $(SDK_BUILD_DIR)/
	$(CP) $(TOPDIR)/rules.mk $(TOPDIR)/.config $(SDK_BUILD_DIR)/
	$(CP) $(TOPDIR)/package/Makefile $(SDK_BUILD_DIR)/package/
	$(CP) ./files/Makefile.sdk $(SDK_BUILD_DIR)/Makefile
	$(CP) ./files/README.SDK $(SDK_BUILD_DIR)/
	echo OPENWRTVERSION:=$(OPENWRTVERSION) > $(SDK_BUILD_DIR)/.version.mk
	find $(SDK_BUILD_DIR) -name .svn | xargs rm -rf 
	find $(SDK_BUILD_DIR) -name CVS | xargs rm -rf 
	(cd $(BUILD_DIR); \
		tar cfj $@ $(SDK_NAME); \
	)

source:
prepare:
compile: $(BIN_DIR)/$(SDK_NAME).tar.bz2
install:

clean:
	rm -rf $(SDK_BUILD_DIR) $(BIN_DIR)/$(SDK_NAME).tar.bz2
