# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 6070 2007-01-10 21:21:59Z nbd $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/host.mk

PKG_OS:=$(shell uname -s)
PKG_CPU:=$(shell uname -m)

IB_NAME:=OpenWrt-ImageBuilder-$(BOARD)-$(KERNEL)-for-$(PKG_OS)-$(PKG_CPU)
IB_BUILD_DIR:=$(BUILD_DIR)/$(IB_NAME)

all: compile

$(BIN_DIR)/$(IB_NAME).tar.bz2:
	rm -rf $(IB_BUILD_DIR)
	mkdir -p $(IB_BUILD_DIR)/build_$(ARCH)/linux-$(KERNEL)-$(BOARD) $(IB_BUILD_DIR)/staging_dir_$(ARCH)/bin $(IB_BUILD_DIR)/target/linux
	$(CP) \
		$(INCLUDE_DIR) $(SCRIPT_DIR) $(PACKAGE_DIR) \
		$(TOPDIR)/rules.mk $(TOPDIR)/.config \
		$(TMP_DIR)/.target.mk \
		./files/Makefile \
		$(IB_BUILD_DIR)/
	$(CP) $(STAGING_DIR)/bin/* $(IB_BUILD_DIR)/staging_dir_$(ARCH)/bin
	$(CP) $(TOPDIR)/target/linux/* $(IB_BUILD_DIR)/target/linux
	rm -rf $(IB_BUILD_DIR)/target/linux/*/patches
	-cp $(BUILD_DIR)/linux-$(KERNEL)-$(BOARD)/* $(IB_BUILD_DIR)/build_$(ARCH)/linux-$(KERNEL)-$(BOARD) # don't copy subdirectories here
	find $(IB_BUILD_DIR) -name .svn | xargs rm -rf 
	find $(IB_BUILD_DIR) -name CVS | xargs rm -rf 
	(cd $(BUILD_DIR); \
		tar cfj $@ $(IB_NAME); \
	)

download:
prepare:
compile: $(BIN_DIR)/$(IB_NAME).tar.bz2
install: compile

clean:
	rm -rf $(IB_BUILD_DIR) $(BIN_DIR)/$(IB_NAME).tar.bz2
